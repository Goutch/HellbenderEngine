
//#define GLSLANG_C

#ifdef GLSLANG_C

#include "glslang/Include/glslang_c_interface.h"

#else

#include <vulkan/vulkan_core.h>
#include "glslang/Public/ShaderLang.h"
#include "glslang/Include/intermediate.h"

#endif

#include "SPIRV/GlslangToSpv.h"
#include "ShaderCompiler.h"
#include "core/utility/Log.h"
#include "HBE/core/Application.h"

namespace HBE {
#ifdef GLSLANG_C
	const glslang_resource_s DEFAULT_BUILT_IN_RESOURCE_LIMIT = {
#else
	const TBuiltInResource DEFAULT_BUILT_IN_RESOURCE_LIMIT = {
#endif

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
			/* maxMeshOutputVerticesEXT;*/256,
			/* maxMeshOutputPrimitivesEXT;*/512,
			/* maxMeshWorkGroupSizeX_EXT;*/32,
			/* maxMeshWorkGroupSizeY_EXT;*/1,
			/* maxMeshWorkGroupSizeZ_EXT;*/32,
			/* maxTaskWorkGroupSizeX_EXT;*/1,
			/* maxTaskWorkGroupSizeY_EXT;*/1,
			/* maxTaskWorkGroupSizeZ_EXT;*/1,
			/* maxMeshViewCountEXT;*/1,
			/* .maxDualSourceDrawBuffersEXT = */1,
			/* .limits = */
			                   {
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

#ifndef GLSLANG_C


	struct HBE_Includer : glslang::TShader::Includer {
		std::string path;
		std::set<IncludeResult *> results;

		HBE_Includer(const std::string &path) {
			this->path = path.substr(0, path.find_last_of("\\/") + 1);
		}

		// For both include methods below:
		//
		// Resolves an inclusion request by name, current source name,
		// and include depth.
		// On success, returns an IncludeResult containing the resolved name
		// and content of the include.
		// On failure, returns a nullptr, or an IncludeResult
		// with an empty string for the headerName and error details in the
		// header field.
		// The Includer retains ownership of the contents
		// of the returned IncludeResult value, and those contents must
		// remain valid until the releaseInclude method is called on that
		// IncludeResult object.
		//
		// Note "local" vs. "system" is not an "either/or": "local" is an
		// extra thing to do over "system". Both might get called, as per
		// the C++ specification.

		// For the "system" or <>-style includes; search the "system" paths.


		IncludeResult *includeSystem(const char *file_path, const char *includer_name, size_t inclusion_depth) override {
			std::string *source = new std::string();
			source->erase(std::find(source->begin(), source->end(), '\0'), source->end());
			Shader::getSource(file_path, *source);
			IncludeResult *result = new IncludeResult(path, source->c_str(), source->size() - 2, source);
			results.emplace(result);
			return result;
		}

		// For the "local"-only aspect of a "" include. Should not search in the
		// "system" paths, because on returning a failure, the parser will
		// call includeSystem() to look in the "system" locations.
		IncludeResult *includeLocal(const char *file_path, const char *includer_name, size_t inclusion_depth) override {
			std::string *source = new std::string();
			Shader::getSource(path + file_path, *source);
			source->erase(std::find(source->begin(), source->end(), '\0'), source->end());
			IncludeResult *result = new IncludeResult(path + file_path, source->c_str(), source->size(), source);
			results.emplace(result);
			return result;
		}

		void releaseInclude(IncludeResult *result) override {
			if (result != NULL) {
				results.erase(result);
				delete (std::string *) result->userData;
				delete result;
			}
		}

		~HBE_Includer() {
			for (auto r: results) {
				delete[] r->headerData;
				delete r;
			}
		}
	};

#endif

	void resolveIncludes(const char *source, size_t size, std::string &result, const std::string &local_path) {
		std::string source_str(source, size);
		result.reserve(size);

		int position = source_str.find("#include", 0);
		if (position == -1) {
			position = size - 1;
		}
		result.insert(result.end(), source, source + position);
		position += 8;
		for (; position < size; position++) {
			if (source_str[position] == '"') {
				position++;
				uint32_t end = source_str.find('"', position);

				std::string include_path = local_path + source_str.substr(position, end - position);

				std::string include_source;
				Shader::getSource(include_path, include_source);
				std::string new_local_path = local_path.substr(0, local_path.find_last_of("\\/") + 1);
				std::string reolved_source;
				resolveIncludes(include_source.c_str(), include_source.size(), reolved_source, new_local_path);

				result.insert(result.end(), reolved_source.begin(), reolved_source.end() - 1);


				position = source_str.find("#include", position);
				if (position == -1) {
					result.insert(result.end(), source + end + 1, source + ((size - 2)));
					Log::debug(result);
					break;
				}
				position = end;
			}
		}
	}

//https://lxjk.github.io/2020/03/10/Translate-GLSL-to-SPIRV-for-Vulkan-at-Runtime.html
	void ShaderCompiler::GLSLToSpirV(const char *source, size_t size,
	                                 std::vector<uint32_t> &spirv,
	                                 SHADER_STAGE type,
	                                 const std::string &shader_path,
	                                 const std::string &preamble) {
		Log::status("Compiling shader at: " + shader_path);
#ifndef GLSLANG_C
		glslang::InitializeProcess();
		EShLanguage stage;
		auto spirv_target = glslang::EShTargetSpv_1_3;
		switch (type) {
			case SHADER_STAGE::SHADER_STAGE_COMPUTE:
				stage = EShLangCompute;
				break;
			case SHADER_STAGE::SHADER_STAGE_VERTEX:
				stage = EShLangVertex;
				break;
			case SHADER_STAGE::SHADER_STAGE_FRAGMENT:
				stage = EShLangFragment;
				break;
			case SHADER_STAGE::SHADER_STAGE_GEOMETRY:
				stage = EShLangGeometry;
				break;
			case SHADER_STAGE::SHADER_STAGE_RAY_GEN:
				stage = EShLangRayGen;
				spirv_target = glslang::EShTargetSpv_1_4;
				break;
			case SHADER_STAGE::SHADER_STAGE_RAY_MISS:
				stage = EShLangMiss;
				spirv_target = glslang::EShTargetSpv_1_4;
				break;
			case SHADER_STAGE::SHADER_STAGE_CLOSEST_HIT:
				stage = EShLangClosestHit;
				spirv_target = glslang::EShTargetSpv_1_4;
				break;
			case SHADER_STAGE::SHADER_STAGE_ANY_HIT:
				stage = EShLangAnyHit;
				spirv_target = glslang::EShTargetSpv_1_4;
				break;
			case SHADER_STAGE::SHADER_STAGE_INTERSECTION:
				stage = EShLangIntersect;
				spirv_target = glslang::EShTargetSpv_1_4;
				break;

			case SHADER_STAGE_NONE:
				break;
		}


		{
			glslang::TShader shader(stage);

			std::string source_str = source;
			const char *source_str_ptr = source_str.c_str();
			const char *const *source_ptr = &source_str_ptr;
			int lenght = source_str.size();

			glslang::EshTargetClientVersion vulkan_version;
			switch (Application::getInfo().vulkan_version) {
				case VULKAN_VERSION_1_0:
					vulkan_version = glslang::EShTargetVulkan_1_0;
					break;
				case VULKAN_VERSION_1_1:
					vulkan_version = glslang::EShTargetVulkan_1_1;
					break;
				case VULKAN_VERSION_1_2:
					vulkan_version = glslang::EShTargetVulkan_1_2;
					break;
				case VULKAN_VERSION_1_3:
					vulkan_version = glslang::EShTargetVulkan_1_3;
					break;
			}

			shader.setEnvClient(glslang::EShClient::EShClientVulkan, vulkan_version);
			shader.setEnvTarget(glslang::EShTargetSpv, spirv_target);
			shader.setStringsWithLengths(source_ptr, &lenght, 1);
			shader.setPreamble(preamble.c_str());
			shader.getIntermediate()->setSource(glslang::EShSourceGlsl);

			//shader.setAutoMapBindings(true);
			shader.getIntermediate()->setSourceFile(shader_path.c_str());
#ifdef DEBUG_MODE
			shader.getIntermediate()->addSourceText(source, size);
			shader.getIntermediate()->setSourceFile((RESOURCE_PATH + shader_path).c_str());
#endif
			HBE_Includer includer(shader_path);
			EShMessages message = static_cast<EShMessages>(EShMessages::EShMsgVulkanRules | EShMessages::EShMsgSpvRules);

			if (!shader.parse(&DEFAULT_BUILT_IN_RESOURCE_LIMIT,
			                  450,
			                  ENoProfile,
			                  false,
			                  false,
			                  message,
			                  includer)) {
				//Log::warning(shader.getInfoDebugLog());
				Log::error(shader.getInfoLog());
			}
			{
				glslang::TProgram program;
				program.addShader(&shader);
				//program.mapIO(); //auto bindings
				//program.buildReflection(EShReflectionDefault);
				//program.getIntermediate(stage)->addSourceText(source, size);
				if (!program.link(message)) {
					Log::warning(program.getInfoDebugLog());
					Log::error(program.getInfoLog());
				}
				glslang::SpvOptions options{};
#ifdef DEBUG_MODE
				options.generateDebugInfo = true;
				options.stripDebugInfo = false;
				options.disableOptimizer = true;
#else
				options.generateDebugInfo = false;
				options.stripDebugInfo = true;
				options.disableOptimizer = false;
#endif
				glslang::GlslangToSpv(*program.getIntermediate(stage), spirv, &options);

			}
		}
		glslang::FinalizeProcess();
#else
		glslang_stage_t stage = GLSLANG_STAGE_VERTEX;;
		switch (type) {
			case SHADER_STAGE::SHADER_STAGE_COMPUTE:
				stage = GLSLANG_STAGE_COMPUTE;
				break;
			case SHADER_STAGE::SHADER_STAGE_VERTEX:
				stage = GLSLANG_STAGE_VERTEX;
				break;
			case SHADER_STAGE::SHADER_STAGE_FRAGMENT:
				stage = GLSLANG_STAGE_FRAGMENT;
				break;
			case SHADER_STAGE::SHADER_STAGE_GEOMETRY:
				stage = GLSLANG_STAGE_GEOMETRY;
				break;
			case SHADER_STAGE::SHADER_STAGE_RAY_GEN:
				stage = GLSLANG_STAGE_RAYGEN;
				break;
			case SHADER_STAGE::SHADER_STAGE_RAY_MISS:
				stage = GLSLANG_STAGE_MISS;
				break;
			case SHADER_STAGE::SHADER_STAGE_CLOSEST_HIT:
				stage = GLSLANG_STAGE_CLOSESTHIT;
				break;
			case SHADER_STAGE::SHADER_STAGE_ANY_HIT:
				stage = GLSLANG_STAGE_ANYHIT;
				break;
			case SHADER_STAGE::SHADER_STAGE_INTERSECTION:
				stage = GLSLANG_STAGE_INTERSECT;
				break;
			case SHADER_STAGE_NONE:
				break;
		}
		std::string local_path = shader_path.substr(0, shader_path.find_last_of("\\/") + 1);
		std::string resolved_source;
		resolveIncludes(source, size, resolved_source, local_path);
		size = resolved_source.length();
		glslang_input_t input =
				{
						.language = GLSLANG_SOURCE_GLSL,
						.stage = stage,
						.client = GLSLANG_CLIENT_VULKAN,
						.client_version = GLSLANG_TARGET_VULKAN_1_3,
						.target_language = GLSLANG_TARGET_SPV,
						.target_language_version = GLSLANG_TARGET_SPV_1_4,
						.code = resolved_source.c_str(),
						.default_version = 100,
						.default_profile = GLSLANG_NO_PROFILE,
						.force_default_version_and_profile = false,
						.forward_compatible = false,
						.messages = GLSLANG_MSG_DEFAULT_BIT,
						.resource =  &DEFAULT_BUILT_IN_RESOURCE_LIMIT,
				};


		glslang_initialize_process();

		glslang_shader_t *shader = glslang_shader_create(&input);

		if (!glslang_shader_preprocess(shader, &input)) {
			const char *info = glslang_shader_get_info_log(shader);
			const char *debug_info = glslang_shader_get_info_debug_log(shader);
			Log::warning(info);
			Log::debug(debug_info);
		}
		input.code = glslang_shader_get_preprocessed_code(shader);

		if (!glslang_shader_parse(shader, &input)) {
			const char *info = glslang_shader_get_info_log(shader);
			Log::warning(info);
		}

		glslang_program_t *program = glslang_program_create();
		glslang_program_add_shader(program, shader);
		//glslang_program_add_source_text(program, stage, source, size);

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
		size_t spv_size = glslang_program_SPIRV_get_size(program);

		//todo: Find a way to create the shader before deleting the data to stop copying the data
		spirv = std::vector<uint32_t>();
		spirv.assign(code, code + spv_size);


		glslang_program_delete(program);
#endif
	}

}

