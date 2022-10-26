/*
Copyright (c) 2022, NVIDIA CORPORATION. All rights reserved.

NVIDIA CORPORATION and its licensors retain all intellectual property
and proprietary rights in and to this software, related documentation
and any modifications thereto. Any use, reproduction, disclosure or
distribution of this software and related documentation without an express
license agreement from NVIDIA CORPORATION is strictly prohibited.
*/

groupshared float4 s_Normal_Roughness[ BUFFER_Y ][ BUFFER_X ];
groupshared float3 s_HitDist_ViewZ[ BUFFER_Y ][ BUFFER_X ];

void Preload( uint2 sharedPos, int2 globalPos )
{
    globalPos = clamp( globalPos, 0, gRectSize - 1.0 );
    uint2 globalIdUser = gRectOrigin + globalPos;

    float viewZ = abs( gIn_ViewZ[ globalIdUser ] );

    float4 normalAndRoughness = NRD_FrontEnd_UnpackNormalAndRoughness( gIn_Normal_Roughness[ globalIdUser ] );
    s_Normal_Roughness[ sharedPos.y ][ sharedPos.x ] = normalAndRoughness;

    float2 hitDist = 1.0;
    #ifdef REBLUR_DIFFUSE
        hitDist.x = ExtractHitDist( gIn_Diff[ globalPos ] );

        #if( REBLUR_USE_DECOMPRESSED_HIT_DIST_IN_RECONSTRUCTION == 1 )
            hitDist.x *= _REBLUR_GetHitDistanceNormalization( viewZ, gHitDistParams, 1.0 );
        #endif
    #endif

    #ifdef REBLUR_SPECULAR
        hitDist.y = ExtractHitDist( gIn_Spec[ globalPos ] );

        #if( REBLUR_USE_DECOMPRESSED_HIT_DIST_IN_RECONSTRUCTION == 1 )
            hitDist.y *= _REBLUR_GetHitDistanceNormalization( viewZ, gHitDistParams, normalAndRoughness.w );
        #endif
    #endif

    s_HitDist_ViewZ[ sharedPos.y ][ sharedPos.x ] = float3( hitDist, viewZ );
}

[numthreads( GROUP_X, GROUP_Y, 1 )]
NRD_EXPORT void NRD_CS_MAIN( int2 threadPos : SV_GroupThreadId, int2 pixelPos : SV_DispatchThreadId, uint threadIndex : SV_GroupIndex )
{
    uint2 pixelPosUser = gRectOrigin + pixelPos;
    float2 pixelUv = float2( pixelPos + 0.5 ) * gInvRectSize;

    PRELOAD_INTO_SMEM;

    // Early out
    int2 smemPos = threadPos + BORDER;
    float3 center = s_HitDist_ViewZ[ smemPos.y ][ smemPos.x ];

    [branch]
    if( center.z > gDenoisingRange )
        return;

    // Center data
    float4 normalAndRoughness = s_Normal_Roughness[ smemPos.y ][ smemPos.x ];
    float3 N = normalAndRoughness.xyz;
    float roughness = normalAndRoughness.w;

    float frustumHeight = PixelRadiusToWorld( gUnproject, gOrthoMode, gRectSize.y, center.z );
    float3 Xv = STL::Geometry::ReconstructViewPosition( pixelUv, gFrustum, center.z, gOrthoMode );
    float3 Nv = STL::Geometry::RotateVectorInverse( gViewToWorld, N );
    float2 geometryWeightParams = GetGeometryWeightParams( gPlaneDistSensitivity, frustumHeight, Xv, Nv, 1.0 );

    float2 roughnessWeightParams = GetCoarseRoughnessWeightParams( roughness );
    float diffNormalWeightParam = GetNormalWeightParams( 1.0, 1.0, 1.0 );
    float specNormalWeightParam = GetNormalWeightParams( 1.0, 1.0, roughness );

    // Hit distance reconstruction
    float2 sum = 1000.0 * float2( center.xy != 0.0 );
    center.xy *= sum;

    [unroll]
    for( j = 0; j <= BORDER * 2; j++ )
    {
        [unroll]
        for( i = 0; i <= BORDER * 2; i++ )
        {
            float2 o = float2( i, j ) - BORDER;
            if( o.x == 0.0 && o.y == 0.0 )
                continue;

            int2 pos = threadPos + int2( i, j );
            float3 temp = s_HitDist_ViewZ[ pos.y ][ pos.x ];

            float w = IsInScreen( pixelUv + o * gInvRectSize );
            w *= GetGaussianWeight( length( o ) * 0.5 );

            // This weight is strict ( non exponential ) because we need to avoid accessing data from other surfaces
            float3 Xvs = STL::Geometry::ReconstructViewPosition( pixelUv + o * gInvRectSize, gFrustum, temp.z, gOrthoMode );
            w *= GetGeometryWeight( geometryWeightParams, Nv, Xvs );

            float2 ww = w * float2( temp.xy != 0.0 );
            #ifndef REBLUR_PERFORMANCE_MODE
                float4 normalAndRoughness = s_Normal_Roughness[ pos.y ][ pos.x ];

                float cosa = saturate( dot( N, normalAndRoughness.xyz ) );
                float angle = STL::Math::AcosApprox( cosa );

                // These weights have infinite exponential tails, because with strict weights we are reducing a chance to find a valid sample in 3x3 or 5x5 area
                ww.x *= _ComputeExponentialWeight( angle, diffNormalWeightParam, 0.0 );
                ww.y *= _ComputeExponentialWeight( angle, specNormalWeightParam, 0.0 );
                ww.y *= _ComputeExponentialWeight( normalAndRoughness.w, roughnessWeightParams.x, roughnessWeightParams.y );
            #endif

            center.xy += temp.xy * ww;
            sum += ww;
        }
    }

    // Normalize weighted sum
    center.xy /= max( sum, NRD_EPS ); // IMPORTANT: if all conditions are met, "sum" can't be 0

    // Return back to normalized hit distances
    #if( REBLUR_USE_DECOMPRESSED_HIT_DIST_IN_RECONSTRUCTION == 1 )
        center.x /= _REBLUR_GetHitDistanceNormalization( center.z, gHitDistParams, 1.0 );
        center.y /= _REBLUR_GetHitDistanceNormalization( center.z, gHitDistParams, roughness );
    #endif

    // Output
    #ifdef REBLUR_DIFFUSE
        #ifdef REBLUR_OCCLUSION
            gOut_Diff[ pixelPos ] = center.x;
        #else
            float3 diff = gIn_Diff[ pixelPos ].xyz;
            gOut_Diff[ pixelPos ] = float4( diff, center.x );
        #endif
    #endif

    #ifdef REBLUR_SPECULAR
        #ifdef REBLUR_OCCLUSION
            gOut_Spec[ pixelPos ] = center.y;
        #else
            float3 spec = gIn_Spec[ pixelPos ].xyz;
            gOut_Spec[ pixelPos ] = float4( spec, center.y );
        #endif
    #endif
}
