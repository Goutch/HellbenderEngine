#pragma once
#include "graphics/GraphicLimits.h"
#include "resource/Font.h"
#include "scene/systems/ui/TextSystem.h"
#include "core/interface/ImageInterface.h"
#include "data-structure/Handle.h"
#include "interface/AccelerationStructureInterface.h"
#include "interface/ComputePipelineInterface.h"
#include "interface/MeshInterface.h"
#include "interface/RasterizationPipelineInterface.h"
#include "interface/RasterizationTargetInterface.h"
#include "interface/RaytracingPipelineInterface.h"
#include "interface/ShaderInterface.h"
#include "resource/PipelineInstance.h"

namespace HBE {
    class Font;
    class Renderer;

    enum GRAPHICS_API {
        GRAPHICS_API_NONE,
        GRAPHICS_API_VULKAN,
    };

    typedef uint32_t VULKAN_REQUIRED_EXTENSION_FLAGS;

    enum VULKAN_REQUIRED_EXTENSIONS_FLAG {
        VULKAN_REQUIRED_EXTENSION_NONE = 0,
        VULKAN_REQUIRED_EXTENSION_RTX = 1,
        VULKAN_REQUIRED_EXTENSION_DESCRIPTOR_INDEXING = 2,
    };

    enum VULKAN_VERSION {
        VULKAN_VERSION_1_0 = 0,
        VULKAN_VERSION_1_1 = 1,
        VULKAN_VERSION_1_2 = 2,
        VULKAN_VERSION_1_3 = 3,
    };

    struct ContextInfo {
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

    struct RasterizationTargetInfo;

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

#define CONTEXT_API_FUNC(ReturnType, FuncName, Params, Args)    \
inline ReturnType FuncName(Params)                              \
{                                                               \
    return graphic_api.FuncName##_ptr(context_data, Args);      \
}

#define GRAPHIC_API_FUNC(ReturnType, FuncName, Params,Args)     \
typedef ReturnType (*PFN_##FuncName)(void* contxt_ptr, Params); \
PFN_##FuncName FuncName##_ptr = nullptr;

#define FUNC_ARGS(...) __VA_ARGS__
#define FUNC_PARAMS(...) __VA_ARGS__


    struct GraphicAPI {
        //general
        GRAPHIC_API_FUNC(void, getGraphicLimits, FUNC_PARAMS(GraphicLimits& limits), FUNC_ARGS(limits))

        //images
        GRAPHIC_API_FUNC(HBE_RESULT, createImage, FUNC_PARAMS(ImageHandle& handle,const ImageInfo& info), FUNC_ARGS(handle,info));
        GRAPHIC_API_FUNC(HBE_RESULT, releaseImage, FUNC_PARAMS(ImageHandle handle), FUNC_ARGS(handle));
        GRAPHIC_API_FUNC(HBE_RESULT, updateImage, FUNC_PARAMS(ImageHandle handle, const void* data), FUNC_ARGS(handle,data));
        GRAPHIC_API_FUNC(HBE_RESULT, getImageSize, FUNC_PARAMS(ImageHandle handle,uvec3& size_ref), FUNC_ARGS(handle,size_ref));

        //meshes
        GRAPHIC_API_FUNC(HBE_RESULT, createMesh, FUNC_PARAMS(MeshHandle &handle,const MeshInfo& mesh_info), FUNC_ARGS(handle,mesh_info));
        GRAPHIC_API_FUNC(HBE_RESULT, releaseMesh, FUNC_PARAMS(MeshHandle handle), FUNC_ARGS(handle));
        GRAPHIC_API_FUNC(HBE_RESULT, setMeshVertexIndices16, FUNC_PARAMS(MeshHandle handle,const uint16_t *indices, size_t count), FUNC_ARGS(handle,indices,count));
        GRAPHIC_API_FUNC(HBE_RESULT, setMeshVertexIndices, FUNC_PARAMS(MeshHandle handle,const uint32_t *indices, size_t count), FUNC_ARGS(handle,indices,count));
        GRAPHIC_API_FUNC(HBE_RESULT, setMeshVertexBuffer, FUNC_PARAMS(MeshHandle handle,uint32_t location,const void *vertices, size_t count), FUNC_ARGS(handle,location,vertices,count));
        GRAPHIC_API_FUNC(HBE_RESULT, setMeshInstanceBuffer, FUNC_PARAMS(MeshHandle handle,uint32_t location,const void *buffer, size_t count), FUNC_ARGS(handle,location,buffer,count));

        //rasterization target
        GRAPHIC_API_FUNC(HBE_RESULT, createRasterizationTarget, FUNC_PARAMS(RasterizationTargetHandle &handle,const RasterizationTargetInfo& rasterization_target_info), FUNC_ARGS(handle,rasterization_target_info));
        GRAPHIC_API_FUNC(HBE_RESULT, releaseRasterizationTarget, FUNC_PARAMS(RasterizationTargetHandle handle), FUNC_ARGS(handle));

        //shaders
        GRAPHIC_API_FUNC(HBE_RESULT, createShader, FUNC_PARAMS(ShaderHandle& handle,const ShaderInfo& info), FUNC_ARGS(handle,info));
        GRAPHIC_API_FUNC(HBE_RESULT, releaseShader, FUNC_PARAMS(ShaderHandle handle), FUNC_ARGS(handle));

        //raster pipeline
        GRAPHIC_API_FUNC(HBE_RESULT, createRasterizationPipeline, FUNC_PARAMS(RasterizationPipelineHandle& handle,const RasterizationPipelineInfo& raster_pipeline_info), FUNC_ARGS(handle,raster_pipeline_info));
        GRAPHIC_API_FUNC(HBE_RESULT, releaseRasterizationPipeline, FUNC_PARAMS(RasterizationPipelineHandle handle), FUNC_ARGS(handle));

        //compute pipeline
        GRAPHIC_API_FUNC(HBE_RESULT, createComputePipeline, FUNC_PARAMS(ComputePipelineHandle& handle,const ComputePipelineInfo& raster_pipeline_info), FUNC_ARGS(handle,raster_pipeline_info));
        GRAPHIC_API_FUNC(HBE_RESULT, releaseComputePipeline, FUNC_PARAMS(ComputePipelineHandle handle), FUNC_ARGS(handle));

        //raytracing pipeline
        GRAPHIC_API_FUNC(HBE_RESULT, createRaytracingPipeline, FUNC_PARAMS(RaytracingPipelineHandle& handle,const RaytracingPipelineInfo& raster_pipeline_info), FUNC_ARGS(handle,raster_pipeline_info));
        GRAPHIC_API_FUNC(HBE_RESULT, releaseRaytracingPipeline, FUNC_PARAMS(RaytracingPipelineHandle handle), FUNC_ARGS(handle));

        //pipeline instance
        GRAPHIC_API_FUNC(HBE_RESULT, createPipelineInstance, FUNC_PARAMS(PipelineInstanceHandle& handle,const PipelineInstanceInfo& info), FUNC_ARGS(handle,info))
        GRAPHIC_API_FUNC(HBE_RESULT, releasePipelineInstance, FUNC_PARAMS(PipelineInstanceHandle handle), FUNC_ARGS(handle))
        GRAPHIC_API_FUNC(HBE_RESULT, setPipelineInstanceUniform, FUNC_PARAMS(uint32_t binding, const void* data), FUNC_ARGS(binding, data));
        GRAPHIC_API_FUNC(HBE_RESULT, setPipelineInstanceImage, FUNC_PARAMS(uint32_t binding, ImageHandle image, uint32_t mip_level), FUNC_ARGS(binding, image, mip_level));
        GRAPHIC_API_FUNC(HBE_RESULT, setPipelineInstanceImageArray, FUNC_PARAMS(uint32_t binding, ImageHandle* images, uint32_t images_count, uint32_t mip_level), FUNC_ARGS(binding, images, images_count, mip_level));
        GRAPHIC_API_FUNC(HBE_RESULT, setPipelineInstanceStorageBuffer, FUNC_PARAMS(uint32_t binding, StorageBufferHandle buffer, size_t count, size_t offset, int32_t frame), FUNC_ARGS(binding, buffer, count, offset, frame));
        GRAPHIC_API_FUNC(HBE_RESULT, setPipelineInstanceStorageBufferArray, FUNC_PARAMS(uint32_t binding, StorageBufferHandle* buffers, uint32_t count), FUNC_ARGS(binding, buffers, count));
        GRAPHIC_API_FUNC(HBE_RESULT, setPipelineInstanceTexelBuffer, FUNC_PARAMS(uint32_t binding, TexelBufferHandle buffer), FUNC_ARGS(binding, buffer));
        GRAPHIC_API_FUNC(HBE_RESULT, setPipelineInstanceTexelBufferArray, FUNC_PARAMS(uint32_t binding, TexelBufferHandle* buffers, uint32_t count), FUNC_ARGS(binding, buffers, count));
        GRAPHIC_API_FUNC(HBE_RESULT, setPipelineInstanceAccelerationStructure, FUNC_PARAMS(uint32_t binding, RootAccelerationStructureHandle accelerationStructure), FUNC_ARGS(binding, accelerationStructure));
        GRAPHIC_API_FUNC(HBE_RESULT, getPipelineFromInstance, FUNC_PARAMS(PipelineInstanceHandle handle,Handle pipeline_handle), FUNC_ARGS(handle,pipeline_handle));

        //root acceleration structure
        GRAPHIC_API_FUNC(HBE_RESULT, createRootAccelerationStructure, FUNC_PARAMS(RootAccelerationStructureHandle& handle,const RootAccelerationStructureInfo& root_acceleration_structure_info_info), FUNC_ARGS(handle,root_acceleration_structure_info_info));
        GRAPHIC_API_FUNC(HBE_RESULT, releaseRootAccelerationStructure, FUNC_PARAMS(RootAccelerationStructureHandle handle), FUNC_ARGS(handle));

        //aabb acceleration structure
        GRAPHIC_API_FUNC(HBE_RESULT, createAABBAccelerationStructure, FUNC_PARAMS(AABBAccelerationStructureHandle& handle,const AABBAccelerationStructureInfo& aabb_acceleration_structure_info_info), FUNC_ARGS(handle,aabb_acceleration_structure_info_info));
        GRAPHIC_API_FUNC(HBE_RESULT, releaseAABBAccelerationStructure, FUNC_PARAMS(AABBAccelerationStructureHandle handle), FUNC_ARGS(handle));

        //mesh acceleration structure
        GRAPHIC_API_FUNC(HBE_RESULT, createMeshAccelerationStructure, FUNC_PARAMS(MeshAccelerationStructureHandle& handle,const MeshAccelerationStructureInfo& mesh_acceleration_structure_info_info), FUNC_ARGS(handle,mesh_acceleration_structure_info_info));
        GRAPHIC_API_FUNC(HBE_RESULT, releaseMeshAccelerationStructure, FUNC_PARAMS(MeshAccelerationStructureHandle handle), FUNC_ARGS(handle));
    };

    class HB_API Context {
        void *context_data = nullptr;
        GRAPHICS_API current_api = GRAPHICS_API_NONE;
        GraphicAPI graphic_api{};

    public :
        void init(const ContextInfo &info);

        void release();

        //general
        CONTEXT_API_FUNC(void, getGraphicLimits, FUNC_PARAMS(GraphicLimits& limits), FUNC_ARGS(limits))

        //images
        CONTEXT_API_FUNC(HBE_RESULT, createImage, FUNC_PARAMS(ImageHandle& handle,const ImageInfo& info), FUNC_ARGS(handle,info))


        CONTEXT_API_FUNC(HBE_RESULT, releaseImage, FUNC_PARAMS(ImageHandle handle), FUNC_ARGS(handle));
        CONTEXT_API_FUNC(HBE_RESULT, updateImage, FUNC_PARAMS(ImageHandle handle, const void* data), FUNC_ARGS(handle,data));
        CONTEXT_API_FUNC(HBE_RESULT, getImageSize, FUNC_PARAMS(ImageHandle handle,uvec3& size_ref), FUNC_ARGS(handle,size_ref));

        //meshes
        CONTEXT_API_FUNC(HBE_RESULT, createMesh, FUNC_PARAMS(MeshHandle &handle,const MeshInfo& mesh_info), FUNC_ARGS(handle,mesh_info));
        CONTEXT_API_FUNC(HBE_RESULT, releaseMesh, FUNC_PARAMS(MeshHandle handle), FUNC_ARGS(handle));
        CONTEXT_API_FUNC(HBE_RESULT, setMeshVertexIndices16, FUNC_PARAMS(MeshHandle handle,const uint16_t *indices, size_t count), FUNC_ARGS(handle,indices,count));
        CONTEXT_API_FUNC(HBE_RESULT, setMeshVertexIndices, FUNC_PARAMS(MeshHandle handle,const uint32_t *indices, size_t count), FUNC_ARGS(handle,indices,count));
        CONTEXT_API_FUNC(HBE_RESULT, setMeshVertexBuffer, FUNC_PARAMS(MeshHandle handle,uint32_t location,const void *vertices, size_t count), FUNC_ARGS(handle,location,vertices,count));
        CONTEXT_API_FUNC(HBE_RESULT, setMeshInstanceBuffer, FUNC_PARAMS(MeshHandle handle,uint32_t location,const void *buffer, size_t count), FUNC_ARGS(handle,location,buffer,count));

        //pipeline instance
        CONTEXT_API_FUNC(HBE_RESULT, createPipelineInstance, FUNC_PARAMS(PipelineInstanceHandle& handle,const PipelineInstanceInfo& info), FUNC_ARGS(handle,info))
        CONTEXT_API_FUNC(HBE_RESULT, releasePipelineInstance, FUNC_PARAMS(PipelineInstanceHandle handle), FUNC_ARGS(handle))
        CONTEXT_API_FUNC(HBE_RESULT, setPipelineInstanceUniform, FUNC_PARAMS(uint32_t binding, const void* data), FUNC_ARGS(binding, data));
        CONTEXT_API_FUNC(HBE_RESULT, setPipelineInstanceImage, FUNC_PARAMS(uint32_t binding, ImageHandle image, uint32_t mip_level), FUNC_ARGS(binding, image, mip_level));
        CONTEXT_API_FUNC(HBE_RESULT, setPipelineInstanceImageArray, FUNC_PARAMS(uint32_t binding, ImageHandle* images, uint32_t images_count, uint32_t mip_level), FUNC_ARGS(binding, images, images_count, mip_level));
        CONTEXT_API_FUNC(HBE_RESULT, setPipelineInstanceStorageBuffer, FUNC_PARAMS(uint32_t binding, StorageBufferHandle buffer, size_t count, size_t offset, int32_t frame), FUNC_ARGS(binding, buffer, count, offset, frame));
        CONTEXT_API_FUNC(HBE_RESULT, setPipelineInstanceStorageBufferArray, FUNC_PARAMS(uint32_t binding, StorageBufferHandle* buffers, uint32_t count), FUNC_ARGS(binding, buffers, count));
        CONTEXT_API_FUNC(HBE_RESULT, setPipelineInstanceTexelBuffer, FUNC_PARAMS(uint32_t binding, TexelBufferHandle buffer), FUNC_ARGS(binding, buffer));
        CONTEXT_API_FUNC(HBE_RESULT, setPipelineInstanceTexelBufferArray, FUNC_PARAMS(uint32_t binding, TexelBufferHandle* buffers, uint32_t count), FUNC_ARGS(binding, buffers, count));
        CONTEXT_API_FUNC(HBE_RESULT, setPipelineInstanceAccelerationStructure, FUNC_PARAMS(uint32_t binding, RootAccelerationStructureHandle accelerationStructure), FUNC_ARGS(binding, accelerationStructure));
        CONTEXT_API_FUNC(HBE_RESULT, getPipelineFromInstance, FUNC_PARAMS(PipelineInstanceHandle handle,Handle pipeline_handle), FUNC_ARGS(handle,pipeline_handle));

        //shaders
        CONTEXT_API_FUNC(HBE_RESULT, createShader, FUNC_PARAMS(ShaderHandle& handle,const ShaderInfo& info), FUNC_ARGS(handle,info));
        CONTEXT_API_FUNC(HBE_RESULT, releaseShader, FUNC_PARAMS(ShaderHandle handle), FUNC_ARGS(handle));

        //rasterization target
        CONTEXT_API_FUNC(HBE_RESULT, createRasterizationTarget, FUNC_PARAMS(RasterizationTargetHandle &handle,const RasterizationTargetInfo& rasterization_target_info), FUNC_ARGS(handle,rasterization_target_info));
        CONTEXT_API_FUNC(HBE_RESULT, releaseRasterizationTarget, FUNC_PARAMS(RasterizationTargetHandle handle), FUNC_ARGS(handle));

        //raster pipeline
        CONTEXT_API_FUNC(HBE_RESULT, createRasterizationPipeline, FUNC_PARAMS(RasterizationPipelineHandle& handle,const RasterizationPipelineInfo& raster_pipeline_info), FUNC_ARGS(handle,raster_pipeline_info));
        CONTEXT_API_FUNC(HBE_RESULT, releaseRasterizationPipeline, FUNC_PARAMS(RasterizationPipelineHandle handle), FUNC_ARGS(handle));

        //compute pipeline
        CONTEXT_API_FUNC(HBE_RESULT, createComputePipeline, FUNC_PARAMS(ComputePipelineHandle& handle,const ComputePipelineInfo& raster_pipeline_info), FUNC_ARGS(handle,raster_pipeline_info));
        CONTEXT_API_FUNC(HBE_RESULT, releaseComputePipeline, FUNC_PARAMS(ComputePipelineHandle handle), FUNC_ARGS(handle));

        //pipeline instance
        GRAPHIC_API_FUNC(HBE_RESULT, createPipelineInstance, FUNC_PARAMS(PipelineInstanceHandle& handle,const PipelineInstanceInfo& info), FUNC_ARGS(handle,info))
        GRAPHIC_API_FUNC(HBE_RESULT, releasePipelineInstance, FUNC_PARAMS(PipelineInstanceHandle handle), FUNC_ARGS(handle))
        GRAPHIC_API_FUNC(HBE_RESULT, setPipelineInstanceUniform, FUNC_PARAMS(uint32_t binding, const void* data), FUNC_ARGS(binding, data));
        GRAPHIC_API_FUNC(HBE_RESULT, setPipelineInstanceImage, FUNC_PARAMS(uint32_t binding, ImageHandle image, uint32_t mip_level), FUNC_ARGS(binding, image, mip_level));
        GRAPHIC_API_FUNC(HBE_RESULT, setPipelineInstanceImageArray, FUNC_PARAMS(uint32_t binding, ImageHandle* images, uint32_t images_count, uint32_t mip_level), FUNC_ARGS(binding, images, images_count, mip_level));
        GRAPHIC_API_FUNC(HBE_RESULT, setPipelineInstanceStorageBuffer, FUNC_PARAMS(uint32_t binding, StorageBufferHandle buffer, size_t count, size_t offset, int32_t frame), FUNC_ARGS(binding, buffer, count, offset, frame));
        GRAPHIC_API_FUNC(HBE_RESULT, setPipelineInstanceStorageBufferArray, FUNC_PARAMS(uint32_t binding, StorageBufferHandle* buffers, uint32_t count), FUNC_ARGS(binding, buffers, count));
        GRAPHIC_API_FUNC(HBE_RESULT, setPipelineInstanceTexelBuffer, FUNC_PARAMS(uint32_t binding, TexelBufferHandle buffer), FUNC_ARGS(binding, buffer));
        GRAPHIC_API_FUNC(HBE_RESULT, setPipelineInstanceTexelBufferArray, FUNC_PARAMS(uint32_t binding, TexelBufferHandle* buffers, uint32_t count), FUNC_ARGS(binding, buffers, count));
        GRAPHIC_API_FUNC(HBE_RESULT, setPipelineInstanceAccelerationStructure, FUNC_PARAMS(uint32_t binding, RootAccelerationStructureHandle accelerationStructure), FUNC_ARGS(binding, accelerationStructure));

        //raytracing pipeline
        CONTEXT_API_FUNC(HBE_RESULT, createRaytracingPipeline, FUNC_PARAMS(RaytracingPipelineHandle& handle,const RaytracingPipelineInfo& raster_pipeline_info), FUNC_ARGS(handle,raster_pipeline_info));
        CONTEXT_API_FUNC(HBE_RESULT, releaseRaytracingPipeline, FUNC_PARAMS(RaytracingPipelineHandle handle), FUNC_ARGS(handle));

        //root acceleration structure
        CONTEXT_API_FUNC(HBE_RESULT, createRootAccelerationStructure, FUNC_PARAMS(RootAccelerationStructureHandle& handle,const RootAccelerationStructureInfo& root_acceleration_structure_info_info), FUNC_ARGS(handle,root_acceleration_structure_info_info));
        CONTEXT_API_FUNC(HBE_RESULT, releaseRootAccelerationStructure, FUNC_PARAMS(RootAccelerationStructureHandle handle), FUNC_ARGS(handle));

        //aabb acceleration structure
        CONTEXT_API_FUNC(HBE_RESULT, createAABBAccelerationStructure, FUNC_PARAMS(AABBAccelerationStructureHandle& handle,const AABBAccelerationStructureInfo& aabb_acceleration_structure_info_info), FUNC_ARGS(handle,aabb_acceleration_structure_info_info));
        CONTEXT_API_FUNC(HBE_RESULT, releaseAABBAccelerationStructure, FUNC_PARAMS(AABBAccelerationStructureHandle handle), FUNC_ARGS(handle));

        //mesh acceleration structure
        CONTEXT_API_FUNC(HBE_RESULT, createMeshAccelerationStructure, FUNC_PARAMS(MeshAccelerationStructureHandle& handle,const MeshAccelerationStructureInfo& mesh_acceleration_structure_info_info), FUNC_ARGS(handle,mesh_acceleration_structure_info_info));
        CONTEXT_API_FUNC(HBE_RESULT, releaseMeshAccelerationStructure, FUNC_PARAMS(MeshAccelerationStructureHandle handle), FUNC_ARGS(handle));
    };
}
