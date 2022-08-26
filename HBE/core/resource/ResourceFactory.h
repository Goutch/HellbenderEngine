#pragma once


#include "Core.h"
#include "GLSL_TYPE.h"

namespace HBE {

	class GraphicPipeline;

	struct GraphicPipelineInfo;

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

	class Material;

	struct MaterialInfo;

	class ComputeInstance;

	struct ComputeInstanceInfo;

	struct RootAccelerationStructureInfo;

	class RootAccelerationStructure;

	struct AABBAccelerationStructureInfo;

	class AABBAccelerationStructure;

	struct MeshAccelerationStructureInfo;;

	class MeshAccelerationStructure;

	class HB_API ResourceFactory {
	public:
		virtual ~ResourceFactory() = default;

		virtual GraphicPipeline *createGraphicPipeline(const GraphicPipelineInfo &info) const = 0;

		virtual Shader *createShader(const ShaderInfo &info) const = 0;

		virtual Texture *createTexture(const TextureInfo &info) const = 0;

		virtual Mesh *createMesh(const MeshInfo &info) const = 0;

		virtual ComputePipeline *createComputePipeline(const ComputePipelineInfo &info) const = 0;

		virtual RenderTarget *createRenderTarget(const RenderTargetInfo &info) const = 0;

		virtual Material *createMaterial(const MaterialInfo &info) const = 0;

		virtual ComputeInstance *createComputeInstance(const ComputeInstanceInfo &info) const = 0;

		virtual RootAccelerationStructure *createRootAccelerationStructure(const RootAccelerationStructureInfo &info) const = 0;

		virtual AABBAccelerationStructure *createAABBAccelerationStructure(const AABBAccelerationStructureInfo &info) const = 0;

		virtual MeshAccelerationStructure *createMeshAccelerationStructure(const MeshAccelerationStructureInfo &info) const = 0;
	};
}
