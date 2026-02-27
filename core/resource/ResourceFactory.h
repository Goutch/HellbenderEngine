#pragma once

#include "dependencies/utils-collection/HandleContainer.h"
#include "Core.h"


namespace HBE {

	class RasterizationPipeline;

	struct RasterizationPipelineInfo;

	class Image;

	struct ImageInfo;

	class Mesh;

	struct MeshInfo;

	class Shader;

	struct ShaderInfo;

	class ComputePipeline;

	struct ComputePipelineInfo;

	class RasterizationTarget;

	struct RenderTargetInfo;

	class RasterizationPipelineInstance;

	struct RasterizationPipelineInstanceInfo;

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

	struct ImageHandle;

	class HB_API ResourceFactory {
		HandleContainer<ImageInfo> handle_container;
	public:
		virtual ~ResourceFactory() = default;

		virtual RasterizationPipeline *createRasterizationPipeline(const RasterizationPipelineInfo &info) const = 0;

		virtual Shader *createShader(const ShaderInfo &info) const = 0;

		virtual ImageHandle createImage(const ImageInfo &info) const = 0;

		virtual Mesh *createMesh(const MeshInfo &info) const = 0;

		virtual ComputePipeline *createComputePipeline(const ComputePipelineInfo &info) const = 0;

		virtual RasterizationTarget *createRenderTarget(const RenderTargetInfo &info) const = 0;

		virtual RasterizationPipelineInstance *createRasterizationPipelineInstance(const RasterizationPipelineInstanceInfo &info) const = 0;

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
