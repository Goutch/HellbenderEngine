#pragma once


#include "Core.h"
#include "AudioClip.h"

namespace HBE {

	class RaterizationPipeline;

	struct RasterizationPipelineInfo;

	class Texture;

	struct TextureInfo;

	class Mesh;

	struct MeshInfo;

	class Shader;

	struct ShaderInfo;

	class ComputePipeline;

	struct ComputePipelineInfo;

	class RenderTarget;

	struct RenderTargetInfo;

	class RasterizationPipelineInstance;

	struct GraphicPipelineInstanceInfo;

	class ComputeInstance;

	struct ComputeInstanceInfo;

	struct RootAccelerationStructureInfo;

	class RootAccelerationStructure;

	struct AABBAccelerationStructureInfo;

	class AABBAccelerationStructure;

	struct MeshAccelerationStructureInfo;;

	class MeshAccelerationStructure;

	struct RaytracingPipelineInfo;

	class RaytracingPipeline;

	struct RaytracingPipelineInstanceInfo;

	class RaytracingPipelineInstance;

	struct StorageBufferInfo;

	class StorageBuffer;

	struct TexelBufferInfo;

	class TexelBuffer;

	class HB_API ResourceFactory {
	public:
		virtual ~ResourceFactory() = default;

		virtual RaterizationPipeline *createGraphicPipeline(const RasterizationPipelineInfo &info) const = 0;

		virtual Shader *createShader(const ShaderInfo &info) const = 0;

		virtual Texture *createTexture(const TextureInfo &info) const = 0;

		virtual Mesh *createMesh(const MeshInfo &info) const = 0;

		virtual ComputePipeline *createComputePipeline(const ComputePipelineInfo &info) const = 0;

		virtual RenderTarget *createRenderTarget(const RenderTargetInfo &info) const = 0;

		virtual RasterizationPipelineInstance *createMaterial(const GraphicPipelineInstanceInfo &info) const = 0;

		virtual ComputeInstance *createComputeInstance(const ComputeInstanceInfo &info) const = 0;

		virtual RootAccelerationStructure *createRootAccelerationStructure(const RootAccelerationStructureInfo &info) const = 0;

		virtual AABBAccelerationStructure *createAABBAccelerationStructure(const AABBAccelerationStructureInfo &info) const = 0;

		virtual MeshAccelerationStructure *createMeshAccelerationStructure(const MeshAccelerationStructureInfo &info) const = 0;

		virtual RaytracingPipeline *createRaytracingPipeline(const RaytracingPipelineInfo &info) const = 0;

		virtual RaytracingPipelineInstance *createRaytracingPipelineInstance(const RaytracingPipelineInstanceInfo &info) const = 0;

		virtual StorageBuffer *createStorageBuffer(const StorageBufferInfo &info) const = 0;

		virtual TexelBuffer *createTexelBuffer(const TexelBufferInfo &info) const = 0;
	};
}
