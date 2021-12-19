#pragma once



#include "core/resource/ResourceFactory.h"

namespace HBE {

    class VK_Renderer;

    class VK_ResourceFactory : public ResourceFactory {
        VK_Renderer* renderer;
    public:
        VK_ResourceFactory(VK_Renderer *renderer);
		GraphicPipeline *createGraphicPipeline(const GraphicPipelineInfo &info) const override;
		Shader *createShader(const ShaderInfo &info) const override;
		Texture *createTexture(const TextureInfo &info) const override;
		Mesh *createMesh(const MeshInfo &info) const override;
		ComputePipeline *createComputePipeline(const ComputePipelineInfo &info) const override;
		RenderTarget *createRenderTarget(const RenderTargetInfo &info) const override;

	};
}

