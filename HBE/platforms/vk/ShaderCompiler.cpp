

#include "glslang/Include/ResourceLimits.h"
#include "glslang/Include/glslang_c_interface.h"
#include "SPIRV/GlslangToSpv.h"
#include "ShaderCompiler.h"
#include "core/utility/Log.h"


namespace HBE {
    const glslang_resource_s DEFAULT_BUILT_IN_RESOURECE_LIMIT = {
            /* .MaxLights = */ 32,
            /* .MaxClipPlanes = */ 6,
            /* .MaxTextureUnits = */ 32,
            /* .MaxTextureCoords = */ 32,
            /* .MaxVertexAttribs = */ 64,
            /* .MaxVertexUniformComponents = */ 4096,
            /* .MaxVaryingFloats = */ 64,
            /* .MaxVertexTextureImageUnits = */ 32,
            /* .MaxCombinedTextureImageUnits = */ 80,
            /* .MaxTextureImageUnits = */ 32,
            /* .MaxFragmentUniformComponents = */ 4096,
            /* .MaxDrawBuffers = */ 32,
            /* .MaxVertexUniformVectors = */ 128,
            /* .MaxVaryingVectors = */ 8,
            /* .MaxFragmentUniformVectors = */ 16,
            /* .MaxVertexOutputVectors = */ 16,
            /* .MaxFragmentInputVectors = */ 15,
            /* .MinProgramTexelOffset = */ -8,
            /* .MaxProgramTexelOffset = */ 7,
            /* .MaxClipDistances = */ 8,
            /* .MaxComputeWorkGroupCountX = */ 65535,
            /* .MaxComputeWorkGroupCountY = */ 65535,
            /* .MaxComputeWorkGroupCountZ = */ 65535,
            /* .MaxComputeWorkGroupSizeX = */ 1024,
            /* .MaxComputeWorkGroupSizeY = */ 1024,
            /* .MaxComputeWorkGroupSizeZ = */ 64,
            /* .MaxComputeUniformComponents = */ 1024,
            /* .MaxComputeTextureImageUnits = */ 16,
            /* .MaxComputeImageUniforms = */ 8,
            /* .MaxComputeAtomicCounters = */ 8,
            /* .MaxComputeAtomicCounterBuffers = */ 1,
            /* .MaxVaryingComponents = */ 60,
            /* .MaxVertexOutputComponents = */ 64,
            /* .MaxGeometryInputComponents = */ 64,
            /* .MaxGeometryOutputComponents = */ 128,
            /* .MaxFragmentInputComponents = */ 128,
            /* .MaxImageUnits = */ 8,
            /* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
            /* .MaxCombinedShaderOutputResources = */ 8,
            /* .MaxImageSamples = */ 0,
            /* .MaxVertexImageUniforms = */ 0,
            /* .MaxTessControlImageUniforms = */ 0,
            /* .MaxTessEvaluationImageUniforms = */ 0,
            /* .MaxGeometryImageUniforms = */ 0,
            /* .MaxFragmentImageUniforms = */ 8,
            /* .MaxCombinedImageUniforms = */ 8,
            /* .MaxGeometryTextureImageUnits = */ 16,
            /* .MaxGeometryOutputVertices = */ 256,
            /* .MaxGeometryTotalOutputComponents = */ 1024,
            /* .MaxGeometryUniformComponents = */ 1024,
            /* .MaxGeometryVaryingComponents = */ 64,
            /* .MaxTessControlInputComponents = */ 128,
            /* .MaxTessControlOutputComponents = */ 128,
            /* .MaxTessControlTextureImageUnits = */ 16,
            /* .MaxTessControlUniformComponents = */ 1024,
            /* .MaxTessControlTotalOutputComponents = */ 4096,
            /* .MaxTessEvaluationInputComponents = */ 128,
            /* .MaxTessEvaluationOutputComponents = */ 128,
            /* .MaxTessEvaluationTextureImageUnits = */ 16,
            /* .MaxTessEvaluationUniformComponents = */ 1024,
            /* .MaxTessPatchComponents = */ 120,
            /* .MaxPatchVertices = */ 32,
            /* .MaxTessGenLevel = */ 64,
            /* .MaxViewports = */ 16,
            /* .MaxVertexAtomicCounters = */ 0,
            /* .MaxTessControlAtomicCounters = */ 0,
            /* .MaxTessEvaluationAtomicCounters = */ 0,
            /* .MaxGeometryAtomicCounters = */ 0,
            /* .MaxFragmentAtomicCounters = */ 8,
            /* .MaxCombinedAtomicCounters = */ 8,
            /* .MaxAtomicCounterBindings = */ 1,
            /* .MaxVertexAtomicCounterBuffers = */ 0,
            /* .MaxTessControlAtomicCounterBuffers = */ 0,
            /* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
            /* .MaxGeometryAtomicCounterBuffers = */ 0,
            /* .MaxFragmentAtomicCounterBuffers = */ 1,
            /* .MaxCombinedAtomicCounterBuffers = */ 1,
            /* .MaxAtomicCounterBufferSize = */ 16384,
            /* .MaxTransformFeedbackBuffers = */ 4,
            /* .MaxTransformFeedbackInterleavedComponents = */ 64,
            /* .MaxCullDistances = */ 8,
            /* .MaxCombinedClipAndCullDistances = */ 8,
            /* .MaxSamples = */ 4,
            /* .maxMeshOutputVerticesNV = */ 256,
            /* .maxMeshOutputPrimitivesNV = */ 512,
            /* .maxMeshWorkGroupSizeX_NV = */ 32,
            /* .maxMeshWorkGroupSizeY_NV = */ 1,
            /* .maxMeshWorkGroupSizeZ_NV = */ 1,
            /* .maxTaskWorkGroupSizeX_NV = */ 32,
            /* .maxTaskWorkGroupSizeY_NV = */ 1,
            /* .maxTaskWorkGroupSizeZ_NV = */ 1,
            /* .maxMeshViewCountNV = */ 4,
            /* .maxDualSourceDrawBuffersEXT = */ 1,

            /* .limits = */ {
                                       /* .nonInductiveForLoops = */ 1,
                                       /* .whileLoops = */ 1,
                                       /* .doWhileLoops = */ 1,
                                       /* .generalUniformIndexing = */ 1,
                                       /* .generalAttributeMatrixVectorIndexing = */ 1,
                                       /* .generalVaryingIndexing = */ 1,
                                       /* .generalSamplerIndexing = */ 1,
                                       /* .generalVariableIndexing = */ 1,
                                       /* .generalConstantMatrixVectorIndexing = */ 1,
                               }};

    void ShaderCompiler::GLSLToSpirV(const std::string &source, std::vector<uint32_t> &buffer, SHADER_TYPE type) {

        glslang_stage_t stage = GLSLANG_STAGE_VERTEX;;
        switch (type) {
            case SHADER_TYPE::COMPUTE:
                stage = GLSLANG_STAGE_COMPUTE;
                break;
            case SHADER_TYPE::VERTEX:
                stage = GLSLANG_STAGE_VERTEX;
                break;
            case SHADER_TYPE::FRAGMENT:
                stage = GLSLANG_STAGE_FRAGMENT;
                break;
            case SHADER_TYPE::GEOMETRY:
                stage = GLSLANG_STAGE_GEOMETRY;
                break;
            case SHADER_TYPE::NONE:
                Log::error("Failed to compile shdaer because it does not have a type");
                break;
        }

        const glslang_input_t input =
                {
                        .language = GLSLANG_SOURCE_GLSL,
                        .stage = stage,
                        .client = GLSLANG_CLIENT_VULKAN,
                        .client_version = GLSLANG_TARGET_VULKAN_1_2,
                        .target_language = GLSLANG_TARGET_SPV,
                        .target_language_version = GLSLANG_TARGET_SPV_1_0,
                        .code = source.c_str(),
                        .default_version = 100,
                        .default_profile = GLSLANG_NO_PROFILE,
                        .force_default_version_and_profile = false,
                        .forward_compatible = false,
                        .messages = GLSLANG_MSG_DEFAULT_BIT,
                        .resource =  &DEFAULT_BUILT_IN_RESOURECE_LIMIT,
                };
        glslang_initialize_process();
        glslang_shader_t *shader = glslang_shader_create(&input);

        if (!glslang_shader_preprocess(shader, &input)) {
            const char *info = glslang_shader_get_info_log(shader);
            // const char *debug_info = glslang_shader_get_info_debug_log(shader);
            Log::warning(info);
            //Log::debug(debug_info);
        }

        if (!glslang_shader_parse(shader, &input)) {
            const char *info = glslang_shader_get_info_log(shader);
            // const char *debug_info = glslang_shader_get_info_debug_log(shader);
            Log::warning(info);
            // Log::debug(debug_info);
        }

        glslang_program_t *program = glslang_program_create();
        glslang_program_add_shader(program, shader);

        if (!glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT)) {
            const char *info = glslang_program_get_info_log(program);
            //const char *debug_info = glslang_program_get_info_debug_log(program);
            Log::warning(info);
            //Log::debug(debug_info);
        }

        glslang_program_SPIRV_generate(program, input.stage);

        if (glslang_program_SPIRV_get_messages(program)) {
            Log::error(glslang_program_SPIRV_get_messages(program));
        }


        glslang_shader_delete(shader);
        const uint32_t *code = glslang_program_SPIRV_get_ptr(program);
        size_t size = glslang_program_SPIRV_get_size(program);

        //todo: Find a way to create the shader before deleting the data to stop copying the data
        buffer = std::vector<uint32_t>();
        buffer.assign(code, code + size);


        glslang_program_delete(program);
    }
}

