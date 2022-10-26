/*
Copyright (c) 2022, NVIDIA CORPORATION. All rights reserved.

NVIDIA CORPORATION and its licensors retain all intellectual property
and proprietary rights in and to this software, related documentation
and any modifications thereto. Any use, reproduction, disclosure or
distribution of this software and related documentation without an express
license agreement from NVIDIA CORPORATION is strictly prohibited.
*/

void nrd::DenoiserImpl::AddMethod_ReblurDiffuseSpecular(nrd::MethodData& methodData)
{
    #define METHOD_NAME REBLUR_DiffuseSpecular
    #define DIFF_TEMP1 AsUint(Transient::DIFF_TMP1)
    #define DIFF_TEMP2 AsUint(Transient::DIFF_TMP2)
    #define SPEC_TEMP1 AsUint(Transient::SPEC_TMP1)
    #define SPEC_TEMP2 AsUint(Transient::SPEC_TMP2)

    methodData.settings.reblur = ReblurSettings();
    methodData.settingsSize = sizeof(methodData.settings.reblur);

    uint16_t w = methodData.desc.fullResolutionWidth;
    uint16_t h = methodData.desc.fullResolutionHeight;

    enum class Permanent
    {
        PREV_VIEWZ = PERMANENT_POOL_START,
        PREV_NORMAL_ROUGHNESS,
        PREV_INTERNAL_DATA,
        DIFF_HISTORY,
        DIFF_FAST_HISTORY_PING,
        DIFF_FAST_HISTORY_PONG,
        SPEC_HISTORY,
        SPEC_FAST_HISTORY_PING,
        SPEC_FAST_HISTORY_PONG,
    };

    m_PermanentPool.push_back( {REBLUR_FORMAT_PREV_VIEWZ, w, h, 1} );
    m_PermanentPool.push_back( {REBLUR_FORMAT_PREV_NORMAL_ROUGHNESS, w, h, 1} );
    m_PermanentPool.push_back( {REBLUR_FORMAT_PREV_INTERNAL_DATA, w, h, 1} );
    m_PermanentPool.push_back( {REBLUR_FORMAT, w, h, 1} );
    m_PermanentPool.push_back( {REBLUR_FORMAT_FAST_HISTORY, w, h, 1} );
    m_PermanentPool.push_back( {REBLUR_FORMAT_FAST_HISTORY, w, h, 1} );
    m_PermanentPool.push_back( {REBLUR_FORMAT, w, h, 1} );
    m_PermanentPool.push_back( {REBLUR_FORMAT_FAST_HISTORY, w, h, 1} );
    m_PermanentPool.push_back( {REBLUR_FORMAT_FAST_HISTORY, w, h, 1} );

    enum class Transient
    {
        DATA1 = TRANSIENT_POOL_START,
        DATA2,
        SPEC_MIN_HITDIST,
        DIFF_TMP1,
        DIFF_TMP2,
        SPEC_TMP1,
        SPEC_TMP2,
    };

    m_TransientPool.push_back( {Format::RGBA8_UNORM, w, h, 1} );
    m_TransientPool.push_back( {Format::RGBA8_UNORM, w, h, 1} );
    m_TransientPool.push_back( {REBLUR_FORMAT_MIN_HITDIST, w, h, 1} );
    m_TransientPool.push_back( {REBLUR_FORMAT, w, h, 1} );
    m_TransientPool.push_back( {REBLUR_FORMAT, w, h, 1} );
    m_TransientPool.push_back( {REBLUR_FORMAT, w, h, 1} );
    m_TransientPool.push_back( {REBLUR_FORMAT, w, h, 1} );

    REBLUR_SET_SHARED_CONSTANTS;

    for (int i = 0; i < REBLUR_HITDIST_RECONSTRUCTION_PERMUTATION_NUM; i++)
    {
        bool is5x5 = ( ( ( i >> 1 ) & 0x1 ) != 0 );
        bool isPrepassEnabled = ( ( ( i >> 0 ) & 0x1 ) != 0 );

        PushPass("Hit distance reconstruction");
        {
            // Inputs
            PushInput( AsUint(ResourceType::IN_NORMAL_ROUGHNESS) );
            PushInput( AsUint(ResourceType::IN_VIEWZ) );
            PushInput( AsUint(ResourceType::IN_DIFF_RADIANCE_HITDIST) );
            PushInput( AsUint(ResourceType::IN_SPEC_RADIANCE_HITDIST) );

            // Outputs
            PushOutput( isPrepassEnabled ? DIFF_TEMP2 : DIFF_TEMP1 );
            PushOutput( isPrepassEnabled ? SPEC_TEMP2 : SPEC_TEMP1 );

            // Shaders
            if (is5x5)
            {
                AddDispatch( REBLUR_DiffuseSpecular_HitDistReconstruction_5x5, REBLUR_HITDIST_RECONSTRUCTION_CONSTANT_NUM, REBLUR_HITDIST_RECONSTRUCTION_GROUP_DIM, 1 );
                AddDispatch( REBLUR_Perf_DiffuseSpecular_HitDistReconstruction_5x5, REBLUR_HITDIST_RECONSTRUCTION_CONSTANT_NUM, REBLUR_HITDIST_RECONSTRUCTION_GROUP_DIM, 1 );
            }
            else
            {
                AddDispatch( REBLUR_DiffuseSpecular_HitDistReconstruction, REBLUR_HITDIST_RECONSTRUCTION_CONSTANT_NUM, REBLUR_HITDIST_RECONSTRUCTION_GROUP_DIM, 1 );
                AddDispatch( REBLUR_Perf_DiffuseSpecular_HitDistReconstruction, REBLUR_HITDIST_RECONSTRUCTION_CONSTANT_NUM, REBLUR_HITDIST_RECONSTRUCTION_GROUP_DIM, 1 );
            }
        }
    }

    for (int i = 0; i < REBLUR_PREPASS_PERMUTATION_NUM; i++)
    {
        bool isAfterReconstruction = ( ( ( i >> 0 ) & 0x1 ) != 0 );

        PushPass("Pre-pass");
        {
            // Inputs
            PushInput( AsUint(ResourceType::IN_NORMAL_ROUGHNESS) );
            PushInput( AsUint(ResourceType::IN_VIEWZ) );
            PushInput( isAfterReconstruction ? DIFF_TEMP2 : AsUint(ResourceType::IN_DIFF_RADIANCE_HITDIST) );
            PushInput( isAfterReconstruction ? SPEC_TEMP2 : AsUint(ResourceType::IN_SPEC_RADIANCE_HITDIST) );

            // Outputs
            PushOutput( DIFF_TEMP1 );
            PushOutput( SPEC_TEMP1 );
            PushOutput( AsUint(Transient::SPEC_MIN_HITDIST) );

            // Shaders
            AddDispatch( REBLUR_DiffuseSpecular_PrePass, REBLUR_PREPASS_CONSTANT_NUM, REBLUR_PREPASS_GROUP_DIM, 1 );
            AddDispatch( REBLUR_Perf_DiffuseSpecular_PrePass, REBLUR_PREPASS_CONSTANT_NUM, REBLUR_PREPASS_GROUP_DIM, 1 );
        }
    }

    for (int i = 0; i < REBLUR_TEMPORAL_ACCUMULATION_PERMUTATION_NUM; i++)
    {
        bool isTemporalStabilization = ( ( ( i >> 2 ) & 0x1 ) != 0 );
        bool hasConfidenceInputs = ( ( ( i >> 1 ) & 0x1 ) != 0 );
        bool isAfterPrepass = ( ( ( i >> 0 ) & 0x1 ) != 0 );

        PushPass("Temporal accumulation");
        {
            // Inputs
            PushInput( AsUint(ResourceType::IN_NORMAL_ROUGHNESS) );
            PushInput( AsUint(ResourceType::IN_VIEWZ) );
            PushInput( AsUint(ResourceType::IN_MV) );
            PushInput( AsUint(Permanent::PREV_VIEWZ) );
            PushInput( AsUint(Permanent::PREV_NORMAL_ROUGHNESS) );
            PushInput( AsUint(Permanent::PREV_INTERNAL_DATA) );
            PushInput( AsUint(Transient::SPEC_MIN_HITDIST) );
            PushInput( hasConfidenceInputs ? AsUint(ResourceType::IN_DIFF_CONFIDENCE) : REBLUR_DUMMY );
            PushInput( hasConfidenceInputs ? AsUint(ResourceType::IN_SPEC_CONFIDENCE) : REBLUR_DUMMY );
            PushInput( isAfterPrepass ? DIFF_TEMP1 : AsUint(ResourceType::IN_DIFF_RADIANCE_HITDIST) );
            PushInput( isAfterPrepass ? SPEC_TEMP1 : AsUint(ResourceType::IN_SPEC_RADIANCE_HITDIST) );
            PushInput( isTemporalStabilization ? AsUint(Permanent::DIFF_HISTORY) : AsUint(ResourceType::OUT_DIFF_RADIANCE_HITDIST) );
            PushInput( isTemporalStabilization ? AsUint(Permanent::SPEC_HISTORY) : AsUint(ResourceType::OUT_SPEC_RADIANCE_HITDIST) );
            PushInput( AsUint(Permanent::DIFF_FAST_HISTORY_PING), 0, 1, AsUint(Permanent::DIFF_FAST_HISTORY_PONG) );
            PushInput( AsUint(Permanent::SPEC_FAST_HISTORY_PING), 0, 1, AsUint(Permanent::SPEC_FAST_HISTORY_PONG) );
            
            // Outputs
            PushOutput( DIFF_TEMP2 );
            PushOutput( SPEC_TEMP2 );
            PushOutput( AsUint(Transient::DATA1) );
            PushOutput( AsUint(Permanent::DIFF_FAST_HISTORY_PONG), 0, 1, AsUint(Permanent::DIFF_FAST_HISTORY_PING) );
            PushOutput( AsUint(Permanent::SPEC_FAST_HISTORY_PONG), 0, 1, AsUint(Permanent::SPEC_FAST_HISTORY_PING) );
            PushOutput( AsUint(Transient::DATA2) );

            // Shaders
            if (hasConfidenceInputs)
            {
                AddDispatch( REBLUR_DiffuseSpecular_TemporalAccumulation_Confidence, REBLUR_TEMPORAL_ACCUMULATION_CONSTANT_NUM, REBLUR_TEMPORAL_ACCUMULATION_GROUP_DIM, 1 );
                AddDispatch( REBLUR_Perf_DiffuseSpecular_TemporalAccumulation_Confidence, REBLUR_TEMPORAL_ACCUMULATION_CONSTANT_NUM, REBLUR_TEMPORAL_ACCUMULATION_GROUP_DIM, 1 );
            }
            else
            {
                AddDispatch( REBLUR_DiffuseSpecular_TemporalAccumulation, REBLUR_TEMPORAL_ACCUMULATION_CONSTANT_NUM, REBLUR_TEMPORAL_ACCUMULATION_GROUP_DIM, 1 );
                AddDispatch( REBLUR_Perf_DiffuseSpecular_TemporalAccumulation, REBLUR_TEMPORAL_ACCUMULATION_CONSTANT_NUM, REBLUR_TEMPORAL_ACCUMULATION_GROUP_DIM, 1 );
            }
        }
    }

    for (int i = 0; i < REBLUR_HISTORY_FIX_PERMUTATION_NUM; i++)
    {
        PushPass("History fix");
        {
            // Inputs
            PushInput( AsUint(ResourceType::IN_NORMAL_ROUGHNESS) );
            PushInput( AsUint(Transient::DATA1) );
            PushInput( DIFF_TEMP2 );
            PushInput( SPEC_TEMP2 );
            PushInput( AsUint(ResourceType::IN_VIEWZ) );
            PushInput( AsUint(Transient::DATA2) );
            PushInput( AsUint(Permanent::DIFF_FAST_HISTORY_PONG), 0, 1, AsUint(Permanent::DIFF_FAST_HISTORY_PING) );
            PushInput( AsUint(Permanent::SPEC_FAST_HISTORY_PONG), 0, 1, AsUint(Permanent::SPEC_FAST_HISTORY_PING) );

            // Outputs
            PushOutput( DIFF_TEMP1 );
            PushOutput( SPEC_TEMP1 );

            // Shaders
            AddDispatch( REBLUR_DiffuseSpecular_HistoryFix, REBLUR_HISTORY_FIX_CONSTANT_NUM, REBLUR_HISTORY_FIX_GROUP_DIM, 1 );
            AddDispatch( REBLUR_Perf_DiffuseSpecular_HistoryFix, REBLUR_HISTORY_FIX_CONSTANT_NUM, REBLUR_HISTORY_FIX_GROUP_DIM, 1 );
        }
    }

    for (int i = 0; i < REBLUR_BLUR_PERMUTATION_NUM; i++)
    {
        PushPass("Blur");
        {
            // Inputs
            PushInput( AsUint(ResourceType::IN_NORMAL_ROUGHNESS) );
            PushInput( AsUint(Transient::DATA1) );
            PushInput( DIFF_TEMP1 );
            PushInput( SPEC_TEMP1 );
            PushInput( AsUint(ResourceType::IN_VIEWZ) );

            // Outputs
            PushOutput( DIFF_TEMP2 );
            PushOutput( SPEC_TEMP2 );
            PushOutput( AsUint(Permanent::PREV_VIEWZ) );

            // Shaders
            AddDispatch( REBLUR_DiffuseSpecular_Blur, REBLUR_BLUR_CONSTANT_NUM, REBLUR_BLUR_GROUP_DIM, 1 );
            AddDispatch( REBLUR_Perf_DiffuseSpecular_Blur, REBLUR_BLUR_CONSTANT_NUM, REBLUR_BLUR_GROUP_DIM, 1 );
        }
    }

    for (int i = 0; i < REBLUR_POST_BLUR_PERMUTATION_NUM; i++)
    {
        bool isTemporalStabilization = ( ( ( i >> 0 ) & 0x1 ) != 0 );

        PushPass("Post-blur");
        {
            // Inputs
            PushInput( AsUint(ResourceType::IN_NORMAL_ROUGHNESS) );
            PushInput( AsUint(Transient::DATA1) );
            PushInput( DIFF_TEMP2 );
            PushInput( SPEC_TEMP2 );
            PushInput( AsUint(Permanent::PREV_VIEWZ) );

            // Outputs
            PushOutput( AsUint(Permanent::PREV_NORMAL_ROUGHNESS) );

            if (isTemporalStabilization)
            {
                PushOutput( AsUint(Permanent::DIFF_HISTORY) );
                PushOutput( AsUint(Permanent::SPEC_HISTORY) );
            }
            else
            {
                PushOutput( AsUint(ResourceType::OUT_DIFF_RADIANCE_HITDIST) );
                PushOutput( AsUint(ResourceType::OUT_SPEC_RADIANCE_HITDIST) );
                PushOutput( AsUint(Permanent::PREV_INTERNAL_DATA) );
            }

            // Shaders
            if (isTemporalStabilization)
            {
                AddDispatch( REBLUR_DiffuseSpecular_PostBlur, REBLUR_POST_BLUR_CONSTANT_NUM, REBLUR_POST_BLUR_GROUP_DIM, 1 );
                AddDispatch( REBLUR_Perf_DiffuseSpecular_PostBlur, REBLUR_POST_BLUR_CONSTANT_NUM, REBLUR_POST_BLUR_GROUP_DIM, 1 );
            }
            else
            {
                AddDispatch( REBLUR_DiffuseSpecular_PostBlur_NoTemporalStabilization, REBLUR_POST_BLUR_CONSTANT_NUM, REBLUR_POST_BLUR_GROUP_DIM, 1 );
                AddDispatch( REBLUR_Perf_DiffuseSpecular_PostBlur_NoTemporalStabilization, REBLUR_POST_BLUR_CONSTANT_NUM, REBLUR_POST_BLUR_GROUP_DIM, 1 );
            }
        }
    }

    for (int i = 0; i < REBLUR_COPY_STABILIZED_HISTORY_PERMUTATION_NUM; i++)
    {
        PushPass("Copy stabilized history");
        {
            // Inputs
            PushInput( AsUint(ResourceType::OUT_DIFF_RADIANCE_HITDIST) );
            PushInput( AsUint(ResourceType::OUT_SPEC_RADIANCE_HITDIST) );

            // Outputs
            PushOutput( DIFF_TEMP2 );
            PushOutput( SPEC_TEMP2 );

            // Shaders
            AddDispatch( REBLUR_DiffuseSpecular_CopyStabilizedHistory, REBLUR_COPY_STABILIZED_HISTORY_CONSTANT_NUM, REBLUR_COPY_STABILIZED_HISTORY_GROUP_DIM, USE_MAX_DIMS );
        }
    }

    for (int i = 0; i < REBLUR_TEMPORAL_STABILIZATION_PERMUTATION_NUM; i++)
    {
        PushPass("Temporal stabilization");
        {
            // Inputs
            PushInput( AsUint(ResourceType::IN_NORMAL_ROUGHNESS) );
            PushInput( AsUint(Permanent::PREV_VIEWZ) );
            PushInput( AsUint(ResourceType::IN_MV) );
            PushInput( AsUint(Transient::DATA1) );
            PushInput( AsUint(Transient::DATA2) );
            PushInput( AsUint(Permanent::DIFF_HISTORY) );
            PushInput( AsUint(Permanent::SPEC_HISTORY) );
            PushInput( DIFF_TEMP2 );
            PushInput( SPEC_TEMP2 );

            // Outputs
            PushOutput( AsUint(Permanent::PREV_INTERNAL_DATA) );
            PushOutput( AsUint(ResourceType::OUT_DIFF_RADIANCE_HITDIST) );
            PushOutput( AsUint(ResourceType::OUT_SPEC_RADIANCE_HITDIST) );

            // Shaders
            AddDispatch( REBLUR_DiffuseSpecular_TemporalStabilization, REBLUR_TEMPORAL_STABILIZATION_CONSTANT_NUM, REBLUR_TEMPORAL_STABILIZATION_GROUP_DIM, 1 );
            AddDispatch( REBLUR_Perf_DiffuseSpecular_TemporalStabilization, REBLUR_TEMPORAL_STABILIZATION_CONSTANT_NUM, REBLUR_TEMPORAL_STABILIZATION_GROUP_DIM, 1 );
        }
    }

    for (int i = 0; i < REBLUR_SPLIT_SCREEN_PERMUTATION_NUM; i++)
    {
        PushPass("Split screen");
        {
            // Inputs
            PushInput( AsUint(ResourceType::IN_VIEWZ) );
            PushInput( AsUint(ResourceType::IN_DIFF_RADIANCE_HITDIST) );
            PushInput( AsUint(ResourceType::IN_SPEC_RADIANCE_HITDIST) );

            // Outputs
            PushOutput( AsUint(ResourceType::OUT_DIFF_RADIANCE_HITDIST) );
            PushOutput( AsUint(ResourceType::OUT_SPEC_RADIANCE_HITDIST) );

            // Shaders
            AddDispatch( REBLUR_DiffuseSpecular_SplitScreen, REBLUR_SPLIT_SCREEN_CONSTANT_NUM, REBLUR_SPLIT_SCREEN_GROUP_DIM, 1 );
        }
    }

    #undef METHOD_NAME
    #undef DIFF_TEMP1
    #undef SPEC_TEMP1
    #undef DIFF_TEMP2
    #undef SPEC_TEMP2
}
