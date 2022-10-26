/*
Copyright (c) 2022, NVIDIA CORPORATION. All rights reserved.

NVIDIA CORPORATION and its licensors retain all intellectual property
and proprietary rights in and to this software, related documentation
and any modifications thereto. Any use, reproduction, disclosure or
distribution of this software and related documentation without an express
license agreement from NVIDIA CORPORATION is strictly prohibited.
*/

NRD_SAMPLER_START
    NRD_SAMPLER( SamplerState, gNearestClamp, s, 0 )
    NRD_SAMPLER( SamplerState, gNearestMirror, s, 1 )
    NRD_SAMPLER( SamplerState, gLinearClamp, s, 2 )
    NRD_SAMPLER( SamplerState, gLinearMirror, s, 3 )
NRD_SAMPLER_END

NRD_CONSTANTS_START
    REBLUR_SHARED_CB_DATA
    NRD_CONSTANT( float, gSplitScreen )
    NRD_CONSTANT( uint, gDiffCheckerboard )
    NRD_CONSTANT( uint, gSpecCheckerboard )
NRD_CONSTANTS_END

#if( defined REBLUR_DIFFUSE && defined REBLUR_SPECULAR )

    NRD_INPUT_TEXTURE_START
        NRD_INPUT_TEXTURE( Texture2D<float>, gIn_ViewZ, t, 0 )
        NRD_INPUT_TEXTURE( Texture2D<float4>, gIn_Diff, t, 1 )
        NRD_INPUT_TEXTURE( Texture2D<float4>, gIn_Spec, t, 2 )
        #ifdef REBLUR_SH
            NRD_INPUT_TEXTURE( Texture2D<float4>, gIn_DiffSh, t, 3 )
            NRD_INPUT_TEXTURE( Texture2D<float4>, gIn_SpecSh, t, 4 )
        #endif
    NRD_INPUT_TEXTURE_END

    NRD_OUTPUT_TEXTURE_START
        NRD_OUTPUT_TEXTURE( RWTexture2D<float4>, gOut_Diff, u, 0 )
        NRD_OUTPUT_TEXTURE( RWTexture2D<float4>, gOut_Spec, u, 1 )
        #ifdef REBLUR_SH
            NRD_OUTPUT_TEXTURE( RWTexture2D<float4>, gOut_DiffSh, u, 2 )
            NRD_OUTPUT_TEXTURE( RWTexture2D<float4>, gOut_SpecSh, u, 3 )
        #endif
    NRD_OUTPUT_TEXTURE_END

#elif( defined REBLUR_DIFFUSE )

    NRD_INPUT_TEXTURE_START
        NRD_INPUT_TEXTURE( Texture2D<float>, gIn_ViewZ, t, 0 )
        NRD_INPUT_TEXTURE( Texture2D<float4>, gIn_Diff, t, 1 )
        #ifdef REBLUR_SH
            NRD_INPUT_TEXTURE( Texture2D<float4>, gIn_DiffSh, t, 2 )
        #endif
    NRD_INPUT_TEXTURE_END

    NRD_OUTPUT_TEXTURE_START
        NRD_OUTPUT_TEXTURE( RWTexture2D<float4>, gOut_Diff, u, 0 )
        #ifdef REBLUR_SH
            NRD_OUTPUT_TEXTURE( RWTexture2D<float4>, gOut_DiffSh, u, 1 )
        #endif
    NRD_OUTPUT_TEXTURE_END

#else

    NRD_INPUT_TEXTURE_START
        NRD_INPUT_TEXTURE( Texture2D<float>, gIn_ViewZ, t, 0 )
        NRD_INPUT_TEXTURE( Texture2D<float4>, gIn_Spec, t, 1 )
        #ifdef REBLUR_SH
            NRD_INPUT_TEXTURE( Texture2D<float4>, gIn_SpecSh, t, 2 )
        #endif
    NRD_INPUT_TEXTURE_END

    NRD_OUTPUT_TEXTURE_START
        NRD_OUTPUT_TEXTURE( RWTexture2D<float4>, gOut_Spec, u, 0 )
        #ifdef REBLUR_SH
            NRD_OUTPUT_TEXTURE( RWTexture2D<float4>, gOut_SpecSh, u, 1 )
        #endif
    NRD_OUTPUT_TEXTURE_END

#endif
