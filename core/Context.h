#pragma once
#include "graphics/GraphicLimits.h"
#include "resource/Font.h"
#include "scene/systems/ui/TextSystem.h"

namespace HBE
{
    class Font;
    class Renderer;

    enum GRAPHICS_API
    {
        GRAPHICS_API_VULKAN,
    };

    typedef uint32_t VULKAN_REQUIRED_EXTENSION_FLAGS;

    enum VULKAN_REQUIRED_EXTENSIONS_FLAG
    {
        VULKAN_REQUIRED_EXTENSION_NONE = 0,
        VULKAN_REQUIRED_EXTENSION_RTX = 1,
        VULKAN_REQUIRED_EXTENSION_DESCRIPTOR_INDEXING = 2,
    };

    enum VULKAN_VERSION
    {
        VULKAN_VERSION_1_0 = 0,
        VULKAN_VERSION_1_1 = 1,
        VULKAN_VERSION_1_2 = 2,
        VULKAN_VERSION_1_3 = 3,
    };

    struct ContextInfo
    {
        GRAPHICS_API api = GRAPHICS_API_VULKAN;
        VULKAN_REQUIRED_EXTENSION_FLAGS required_extension_flags = VULKAN_REQUIRED_EXTENSION_NONE;
        VULKAN_VERSION vulkan_version = VULKAN_VERSION_1_0;
    };

    class RasterizationPipeline;

    struct RasterizationPipelineInfo;

    using ImageHandle = Handle;
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


    class HB_API Context
    {
        //BufferInterface buffer_interface;
        //ImageInterface image_interface;
        //StorageBufferInterface storage_buffer_interface;
        //RasterizationPipelineInterface rasterization_pipeline_interface;
    public:
        virtual ~Context() = default;

        virtual Renderer* getRenderer() =0;

        virtual Image* createImage(const ImageInfo& info) =0;

        virtual RasterizationPipeline* createRasterizationPipeline(const RasterizationPipelineInfo& info) = 0;

        virtual Shader* createShader(const ShaderInfo& info) = 0;

        virtual Mesh* createMesh(const MeshInfo& info) = 0;

        virtual ComputePipeline* createComputePipeline(const ComputePipelineInfo& info) = 0;

        virtual RasterizationTarget* createRenderTarget(const RenderTargetInfo& info) = 0;

        virtual RasterizationPipelineInstance* createRasterizationPipelineInstance(const RasterizationPipelineInstanceInfo& info) = 0;

        virtual ComputeInstance* createComputeInstance(const ComputeInstanceInfo& info) = 0;

        virtual RootAccelerationStructure* createRootAccelerationStructure(const RootAccelerationStructureInfo& info) = 0;

        virtual AABBAccelerationStructure* createAABBAccelerationStructure(const AABBAccelerationStructureInfo& info) = 0;

        virtual MeshAccelerationStructure* createMeshAccelerationStructure(const MeshAccelerationStructureInfo& info) = 0;

        virtual RaytracingPipeline* createRaytracingPipeline(const RaytracingPipelineInfo& info) = 0;

        virtual RaytracingPipelineInstance* createRaytracingPipelineInstance(const RaytracingPipelineInstanceInfo& info) = 0;

        virtual StorageBuffer* createStorageBuffer(const StorageBufferInfo& info) = 0;

        virtual TexelBuffer* createTexelBuffer(const TexelBufferInfo& info) = 0;

        virtual GraphicLimits getGraphicLimits() const = 0;

        virtual Font* createFont(const FontInfo& font_info) const = 0;
    };
}
