/*
Copyright (c) 2022, NVIDIA CORPORATION. All rights reserved.

NVIDIA CORPORATION and its licensors retain all intellectual property
and proprietary rights in and to this software, related documentation
and any modifications thereto. Any use, reproduction, disclosure or
distribution of this software and related documentation without an express
license agreement from NVIDIA CORPORATION is strictly prohibited.
*/

[numthreads(GROUP_X, GROUP_Y, 1)]
NRD_EXPORT void NRD_CS_MAIN(uint2 pixelPos : SV_DispatchThreadId)
{
    float centerMaterialID;
    float centerViewZ = gViewZFP16[pixelPos] / NRD_FP16_VIEWZ_SCALE;

    // Early out if linearZ is beyond denoising range
    [branch]
    if (centerViewZ > gDenoisingRange)
    {
#if( RELAX_BLACK_OUT_INF_PIXELS == 1 )
#ifdef RELAX_SPECULAR
        gOutSpecularIlluminationAndVariance[pixelPos] = 0;
#endif
#ifdef RELAX_DIFFUSE
        gOutDiffuseIlluminationAndVariance[pixelPos] = 0;
#endif
#endif
        return;
    }

    float4 centerNormalRoughness = NRD_FrontEnd_UnpackNormalAndRoughness(gNormalRoughness[pixelPos], centerMaterialID);
    float3 centerNormal = centerNormalRoughness.rgb;
    float centerRoughness = centerNormalRoughness.a;
    float historyLength = 255.0 * gHistoryLength[pixelPos];

    // Diffuse normal weight is used for diffuse and can be used for specular depending on settings.
    // Weight strictness is higher as the Atrous step size increases.
    float diffuseLobeAngleFraction = gDiffuseLobeAngleFraction / sqrt(gStepSize);
    diffuseLobeAngleFraction = lerp(0.99, diffuseLobeAngleFraction, saturate(historyLength / 5.0));

#ifdef RELAX_SPECULAR
    float4 centerSpecularIlluminationAndVariance = gSpecularIlluminationAndVariance[pixelPos];
    float centerSpecularLuminance = STL::Color::Luminance(centerSpecularIlluminationAndVariance.rgb);
    float centerSpecularVar = centerSpecularIlluminationAndVariance.a;

    float specularReprojectionConfidence = gSpecularReprojectionConfidence[pixelPos];
    float specularLuminanceWeightRelaxation = 1.0;
    if (gStepSize <= 4)
        specularLuminanceWeightRelaxation = lerp(1.0, specularReprojectionConfidence, gLuminanceEdgeStoppingRelaxation);

    float specularPhiLIllumination = gSpecularPhiLuminance * max(1.0e-4, sqrt(centerSpecularVar));
    float2 roughnessWeightParams = GetRoughnessWeightParams(centerRoughness, gRoughnessFraction);

    float diffuseLobeAngleFractionForSimplifiedSpecularNormalWeight = diffuseLobeAngleFraction;
    float specularLobeAngleFraction = gSpecularLobeAngleFraction;

    if (gUseConfidenceInputs != 0)
    {
        float specConfidenceDrivenRelaxation =
            saturate(gConfidenceDrivenRelaxationMultiplier * (1.0 - gSpecConfidence[pixelPos]));

        // Relaxing normal weights for specular
        float r = saturate(specConfidenceDrivenRelaxation * gConfidenceDrivenNormalEdgeStoppingRelaxation);
        diffuseLobeAngleFractionForSimplifiedSpecularNormalWeight = lerp(diffuseLobeAngleFraction, 1.0, r);
        specularLobeAngleFraction = lerp(specularLobeAngleFraction, 1.0, r);

        // Relaxing luminance weight for specular
        r = saturate(specConfidenceDrivenRelaxation * gConfidenceDrivenLuminanceEdgeStoppingRelaxation);
        specularLuminanceWeightRelaxation *= 1.0 - r;
    }

    float specularNormalWeightParamsSimplified = GetNormalWeightParams(1.0, diffuseLobeAngleFractionForSimplifiedSpecularNormalWeight);
    float2 specularNormalWeightParams = GetNormalWeightParams_ATrous(centerRoughness, historyLength, specularReprojectionConfidence, gNormalEdgeStoppingRelaxation, specularLobeAngleFraction);

    float sumWSpecular = 0.44198 * 0.44198;
    float4 sumSpecularIlluminationAndVariance = centerSpecularIlluminationAndVariance * float4(sumWSpecular.xxx, sumWSpecular * sumWSpecular);
#endif

#ifdef RELAX_DIFFUSE
    float4 centerDiffuseIlluminationAndVariance = gDiffuseIlluminationAndVariance[pixelPos];
    float centerDiffuseLuminance = STL::Color::Luminance(centerDiffuseIlluminationAndVariance.rgb);
    float centerDiffuseVar = centerDiffuseIlluminationAndVariance.a;
    float diffusePhiLIllumination = gDiffusePhiLuminance * max(1.0e-4, sqrt(centerDiffuseVar));

    float diffuseLuminanceWeightRelaxation = 1.0;
    if (gUseConfidenceInputs != 0)
    {
        float diffConfidenceDrivenRelaxation =
            saturate(gConfidenceDrivenRelaxationMultiplier * (1.0 - gDiffConfidence[pixelPos]));

        // Relaxing normal weights for diffuse
        float r = saturate(diffConfidenceDrivenRelaxation * gConfidenceDrivenNormalEdgeStoppingRelaxation);
        diffuseLobeAngleFraction = lerp(diffuseLobeAngleFraction, 1.0, r);

        // Relaxing luminance weight for diffuse
        r = saturate(diffConfidenceDrivenRelaxation * gConfidenceDrivenLuminanceEdgeStoppingRelaxation);
        diffuseLuminanceWeightRelaxation = 1.0 - r;
    }
    float diffuseNormalWeightParams = GetNormalWeightParams(1.0, diffuseLobeAngleFraction);

    float sumWDiffuse = 0.44198 * 0.44198;
    float4 sumDiffuseIlluminationAndVariance = centerDiffuseIlluminationAndVariance * float4(sumWDiffuse.xxx, sumWDiffuse * sumWDiffuse);
#endif

    float3 centerWorldPos = GetCurrentWorldPosFromPixelPos(pixelPos, centerViewZ);
    static const float kernelWeightGaussian3x3[2] = { 0.44198, 0.27901 };

    // Adding random offsets to minimize "ringing" at large A-Trous steps
    int2 offset = 0;
    if (gStepSize > 4)
    {
        STL::Rng::Initialize(pixelPos, gFrameIndex);
        offset = int2(gStepSize.xx * 0.5 * (STL::Rng::GetFloat2() - 0.5));
    }

    [unroll]
    for (int yy = -1; yy <= 1; yy++)
    {
        [unroll]
        for (int xx = -1; xx <= 1; xx++)
        {
            int2 p = pixelPos + offset + int2(xx, yy) * gStepSize;
            bool isCenter = ((xx == 0) && (yy == 0));
            if (isCenter)
                continue;

            bool isInside = all(p >= int2(0, 0)) && all(p < int2(gRectSize));
            float kernel = isInside ? kernelWeightGaussian3x3[abs(xx)] * kernelWeightGaussian3x3[abs(yy)] : 0.0;

            // Fetching normal, roughness, linear Z
            float sampleMaterialID;
            float4 sampleNormalRoughnes = NRD_FrontEnd_UnpackNormalAndRoughness(gNormalRoughness[p], sampleMaterialID);
            float3 sampleNormal = sampleNormalRoughnes.rgb;
            float sampleRoughness = sampleNormalRoughnes.a;
            float sampleViewZ = gViewZFP16[p] / NRD_FP16_VIEWZ_SCALE;

            // Calculating sample world position
            float3 sampleWorldPos = GetCurrentWorldPosFromPixelPos(p, sampleViewZ);

            // Calculating geometry weight for diffuse and specular
            float geometryW = GetPlaneDistanceWeight(
                centerWorldPos,
                centerNormal,
                gOrthoMode == 0 ? centerViewZ : 1.0,
                sampleWorldPos,
                gDepthThreshold);

            geometryW *= kernel;

#ifdef RELAX_SPECULAR
            // Calculating weights for specular
            float normalWSpecularSimplified = GetNormalWeight(specularNormalWeightParamsSimplified, centerNormal, sampleNormal);
            float normalWSpecular = GetSpecularNormalWeight_ATrous(specularNormalWeightParams, gSpecularLobeAngleSlack, centerNormal, sampleNormal);
            float roughnessWSpecular = GetRoughnessWeight(roughnessWeightParams, sampleRoughness);

            // Summing up specular
            float wSpecular = geometryW * (gRoughnessEdgeStoppingEnabled ? (normalWSpecular * roughnessWSpecular) : normalWSpecularSimplified);
            wSpecular *= CompareMaterials(sampleMaterialID, centerMaterialID, gSpecMaterialMask);
            if (wSpecular > 1e-4)
            {
                float4 sampleSpecularIlluminationAndVariance = gSpecularIlluminationAndVariance[p];
                float sampleSpecularLuminance = STL::Color::Luminance(sampleSpecularIlluminationAndVariance.rgb);

                float specularLuminanceW = abs(centerSpecularLuminance - sampleSpecularLuminance) / specularPhiLIllumination;
                specularLuminanceW = min(gMaxSpecularLuminanceRelativeDifference, specularLuminanceW);
                specularLuminanceW *= specularLuminanceWeightRelaxation;
                wSpecular *= exp(-specularLuminanceW);

                sumSpecularIlluminationAndVariance += float4(wSpecular.xxx, wSpecular * wSpecular) * sampleSpecularIlluminationAndVariance;
                sumWSpecular += wSpecular;
            }
#endif

#ifdef RELAX_DIFFUSE
            // Calculating weights for diffuse
            float normalWDiffuse = GetNormalWeight(diffuseNormalWeightParams, centerNormal, sampleNormal);

            // Summing up diffuse
            float wDiffuse = geometryW * normalWDiffuse;
            wDiffuse *= CompareMaterials(sampleMaterialID, centerMaterialID, gDiffMaterialMask);
            if (wDiffuse > 1e-4)
            {
                float4 sampleDiffuseIlluminationAndVariance = gDiffuseIlluminationAndVariance[p];
                float sampleDiffuseLuminance = STL::Color::Luminance(sampleDiffuseIlluminationAndVariance.rgb);

                float diffuseLuminanceW = abs(centerDiffuseLuminance - sampleDiffuseLuminance) / diffusePhiLIllumination;
                diffuseLuminanceW = min(gMaxDiffuseLuminanceRelativeDifference, diffuseLuminanceW);
                if (gUseConfidenceInputs != 0)
                {
                    diffuseLuminanceW *= diffuseLuminanceWeightRelaxation;
                }
                wDiffuse *= exp(-diffuseLuminanceW);
                sumDiffuseIlluminationAndVariance += float4(wDiffuse.xxx, wDiffuse * wDiffuse) * sampleDiffuseIlluminationAndVariance;
                sumWDiffuse += wDiffuse;
            }
#endif
        }
    }

#ifdef RELAX_SPECULAR
    float4 filteredSpecularIlluminationAndVariance = float4(sumSpecularIlluminationAndVariance / float4(sumWSpecular.xxx, sumWSpecular * sumWSpecular));
    gOutSpecularIlluminationAndVariance[pixelPos] = filteredSpecularIlluminationAndVariance;
#endif

#ifdef RELAX_DIFFUSE
    float4 filteredDiffuseIlluminationAndVariance = float4(sumDiffuseIlluminationAndVariance / float4(sumWDiffuse.xxx, sumWDiffuse * sumWDiffuse));
    gOutDiffuseIlluminationAndVariance[pixelPos] = filteredDiffuseIlluminationAndVariance;
#endif
}
