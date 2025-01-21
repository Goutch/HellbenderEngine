#pragma once

#include "Core.h"
#include "memory"
#include "ResourceFactory.h"
#include "Resource.h"
#include "unordered_map"
#include "core/utility/Log.h"
#include "Font.h"
#include "AudioClipInstance.h"


namespace HBE {
	class Renderer;

	class Graphics;

	class Shader;

	class RasterizationPipeline;

	class ComputePipeline;

	class Mesh;

	class Image;

	class Model;

	class RenderTarget;

	class RasterizationPipelineInstance;

	class ComputeInstance;

	class AccelerationStructure;

	class RaytracingPipeline;

	class RaytracingPipelineInstance;

	struct ModelInfo;
	struct MeshInfo;
	struct ShaderInfo;
	struct RasterizationPipelineInfo;
	struct ComputePipelineInfo;
	struct ImageInfo;
	struct RenderTargetInfo;
	struct RasterizationPipelineInstanceInfo;
	struct ComputeInstanceInfo;
	struct RootAccelerationStructureInfo;
	struct AABBAccelerationStructureInfo;
	struct MeshAccelerationStructureInfo;
	struct RaytracingPipelineInfo;
	struct RaytracingPipelineInstanceInfo;

	class HB_API Resources {

		static std::unordered_map<std::string, Resource *> registry;
		static const ResourceFactory *factory;
	public:
		static void init(const ResourceFactory &factory) {
			Resources::factory = &factory;
		}

		template<class T>
		static T *get(const std::string &unique_name) {
#ifdef DEBUG_MODE
			if (!Resources::exist(unique_name)) {
				Log::warning("Cant find resource:" + unique_name);
				return nullptr;
			}
#endif
			return dynamic_cast<T *>(Resources::get(unique_name));
		}

		static bool exist(const std::string &unique_name) {
			return Resources::registry.find(unique_name) != registry.end();
		}

		static Resource *get(const std::string &unique_name) {
#ifdef DEBUG_MODE
			if (!Resources::exist(unique_name)) {
				Log::warning("Cant find resource:" + unique_name);
			}
#endif
			return Resources::registry.at(unique_name);
		}


		static void destroyAll() {
			auto &registry = Resources::registry;
			for (auto &name_res: registry) {
				delete name_res.second;
			}
			Resources::registry.clear();
		}

		static void destroy(const std::string &unique_name) {
			delete get(unique_name);

			registry.erase(unique_name);
		}

		static void add(const std::string &unique_name, Resource *r) {
			HB_ASSERT(registry.find(unique_name) == registry.end(), "resource name \"" + unique_name + "\" already exist!");
			registry.emplace(unique_name, r);
		}

		static Model *createModel(const ModelInfo &info, const std::string &name);

		static Model *createModel(const ModelInfo &info);

		static Mesh *createMesh(const MeshInfo &info, const std::string &name);

		static Mesh *createMesh(const MeshInfo &info);

		static Shader *createShader(const ShaderInfo &info, const std::string &name);

		static Shader *createShader(const ShaderInfo &info);

		static RasterizationPipeline *createRasterizationPipeline(const RasterizationPipelineInfo &info, const std::string &name);

		static RasterizationPipeline *createRasterizationPipeline(const RasterizationPipelineInfo &info);

		static ComputePipeline *createComputePipeline(const ComputePipelineInfo &info, const std::string &name);

		static ComputePipeline *createComputePipeline(const ComputePipelineInfo &info);

		static Image *createTexture(const ImageInfo &info, const std::string &name);

		static Image *createTexture(const ImageInfo &info);

		static RenderTarget *createRenderTarget(const RenderTargetInfo &info);

		static RenderTarget *createRenderTarget(const RenderTargetInfo &info, const std::string &name);

		static RasterizationPipelineInstance *createRasterizationPipelineInstance(const RasterizationPipelineInstanceInfo &info, const std::string &name);

		static RasterizationPipelineInstance *createRasterizationPipelineInstance(const RasterizationPipelineInstanceInfo &info);

		static ComputeInstance *createComputeInstance(const ComputeInstanceInfo &info, const std::string &name);

		static ComputeInstance *createComputeInstance(const ComputeInstanceInfo &info);

		static RootAccelerationStructure *createRootAccelerationStructure(const RootAccelerationStructureInfo &info, const std::string &name);

		static RootAccelerationStructure *createRootAccelerationStructure(const RootAccelerationStructureInfo &info);

		static AABBAccelerationStructure *createAABBAccelerationStructure(const AABBAccelerationStructureInfo &info, const std::string &name);

		static AABBAccelerationStructure *createAABBAccelerationStructure(const AABBAccelerationStructureInfo &info);

		static MeshAccelerationStructure *createMeshAccelerationStructure(const MeshAccelerationStructureInfo &info, const std::string &name);

		static MeshAccelerationStructure *createMeshAccelerationStructure(const MeshAccelerationStructureInfo &info);

		static RaytracingPipeline *createRaytracingPipeline(const RaytracingPipelineInfo &info, const std::string &name);

		static RaytracingPipeline *createRaytracingPipeline(const RaytracingPipelineInfo &info);

		static RaytracingPipelineInstance *createRaytracingPipelineInstance(const RaytracingPipelineInstanceInfo &info, const std::string &name);

		static RaytracingPipelineInstance *createRaytracingPipelineInstance(const RaytracingPipelineInstanceInfo &info);

		static StorageBuffer *createStorageBuffer(const StorageBufferInfo &info, const std::string &name);

		static StorageBuffer *createStorageBuffer(const StorageBufferInfo &info);

		static Font *createFont(const FontInfo &info, const std::string &name);

		static Font *createFont(const FontInfo &info);

		static AudioClip *createAudioClip(const AudioClipInfo &info);

		static AudioClip *createAudioClip(const AudioClipInfo &info, const std::string &name);

		static HBE::AudioClipInstance *createAudioClipInstance(const AudioClipInstanceInfo &info);

		static AudioClipInstance *createAudioClipInstance(const AudioClipInstanceInfo &info, const std::string &name);

		static TexelBuffer *createTexelBuffer(const TexelBufferInfo &info, const std::string &name);

		static TexelBuffer *createTexelBuffer(const TexelBufferInfo &info);
	};
}