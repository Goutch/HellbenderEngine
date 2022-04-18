

#include "glslang/Public/ShaderLang.h"
#include "glslang/Include/intermediate.h"
#include "SPIRV/GlslangToSpv.h"
#include "ShaderCompiler.h"


namespace HBE {
	const TBuiltInResource DEFAULT_BUILT_IN_RESOURCE_LIMIT = {
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
			char *data= nullptr;
			size_t size=0;
			Shader::getSource(file_path, &data, size);
			IncludeResult *result = new IncludeResult(path, data, size, nullptr);
			results.emplace(result);
			return result;
		}

		// For the "local"-only aspect of a "" include. Should not search in the
		// "system" paths, because on returning a failure, the parser will
		// call includeSystem() to look in the "system" locations.
		IncludeResult *includeLocal(const char *file_path, const char *includer_name, size_t inclusion_depth) override {
			char *data= nullptr;
			size_t size=0;

			Shader::getSource(path + file_path, &data, size);
			IncludeResult *result = new IncludeResult(path, data, size, nullptr);
			results.emplace(result);
			return result;
		}

		void releaseInclude(IncludeResult *result) override {
			results.erase(result);
			delete[] result->headerData;
			delete result;
		}

		~HBE_Includer() {
			for (auto r:results) {
				delete[] r->headerData;
				delete r;
			}
		}
	};

	//https://lxjk.github.io/2020/03/10/Translate-GLSL-to-SPIRV-for-Vulkan-at-Runtime.html
	void ShaderCompiler::GLSLToSpirV(const char *source, size_t size, std::vector<uint32_t> &spirv, SHADER_STAGE type, const std::string &shader_path) {
		Log::message("Compiling shader at: " + shader_path);
		glslang::InitializeProcess();
		EShLanguage stage;
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
			case SHADER_STAGE_NONE:
				break;
		}


		{
			glslang::TShader shader(stage);

#ifdef DEBUG_MODE
			shader.getIntermediate()->addSourceText(source, size);
			shader.getIntermediate()->setSource(glslang::EShSourceGlsl);
#endif
			int l=(int)size;
			shader.setStringsWithLengths(&source,&l, 1);
			shader.setEnvClient(glslang::EShClient::EShClientVulkan, glslang::EShTargetVulkan_1_2);
			shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_2);

			HBE_Includer includer(shader_path);

			if (!shader.parse(&DEFAULT_BUILT_IN_RESOURCE_LIMIT,
							  330,
							  ENoProfile,
							  false,
							  false,
							  (EShMessages) (EShMessages::EShMsgVulkanRules | EShMessages::EShMsgSpvRules),
							  includer)) {
				Log::warning(shader.getInfoDebugLog());
				Log::error(shader.getInfoLog());
			}

			{
				glslang::TProgram program;

				program.addShader(&shader);

				if (!program.link((EShMessages) (EShMessages::EShMsgVulkanRules | EShMessages::EShMsgSpvRules))) {
					Log::warning(program.getInfoDebugLog());
					Log::error(program.getInfoLog());
				}
				glslang::GlslangToSpv(*program.getIntermediate(stage), spirv);
			}
		}
		glslang::FinalizeProcess();
		/*glslang_stage_t stage = GLSLANG_STAGE_VERTEX;;
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
			case SHADER_STAGE_NONE:
				break;
		}
		const glslang_input_t input =
				{
						.language = GLSLANG_SOURCE_GLSL,
						.stage = stage,
						.client = GLSLANG_CLIENT_VULKAN,
						.client_version = GLSLANG_TARGET_VULKAN_1_2,
						.target_language = GLSLANG_TARGET_SPV,
						.target_language_version = GLSLANG_TARGET_SPV_1_1,
						.code = source.data(),
						.default_version = 100,
						.default_profile = GLSLANG_NO_PROFILE,
						.force_default_version_and_profile = false,
						.forward_compatible = false,
						.messages = GLSLANG_MSG_DEFAULT_BIT,
						.resource =  &DEFAULT_BUILT_IN_RESOURCE_LIMIT,
				};

		glslang_initialize_process();
		//glslang_shader_t *shader = glslang_shader_create(&input);


		if (!glslang_shader_preprocess(shader, &input)) {
			const char *info = glslang_shader_get_info_log(shader);
			// const char *debug_info = glslang_shader_get_info_debug_log(shader);
			Log::warning(info);
			//Log::debug(debug_info);
		}

		if (!glslang_shader_parse(shader, &input)) {
			const char *info = glslang_shader_get_info_log(shader);
			Log::warning(info);
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


		glslang_program_delete(program);*/
	}
}

