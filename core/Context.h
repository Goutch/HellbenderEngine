#pragma message("Context")
#pragma once

#include "HBETypes.h"
#include "graphics/GraphicLimits.h"
#include "interface/PipelineInstanceInterface.h"
#include "interface/RaytracingPipelineInterface.h"
#include "interface/MeshAccelerationStructureInterface.h"
#include "interface/AABBAccelerationStructureInterface.h"
#include "interface/ROOTAccelerationStructureInterface.h"
#include "interface/ComputePipelineInterface.h"
#include "interface/MeshInterface.h"
#include "interface/RasterizationPipelineInterface.h"
#include "interface/RasterizationTargetInterface.h"
#include "interface/BufferInterface.h"
#include "interface/ContextInterface.h"
#include "interface/FenceInterface.h"
#include "interface/RendererInterface.h"

#define FUNC_ARGS(...) __VA_ARGS__
#define FUNC_PARAMS(...) __VA_ARGS__
#define CONTEXT_API_FUNC(ReturnType, FuncName, Params, Args)    \
inline ReturnType FuncName(Params)                              \
{                                                               \
    return context_impl.FuncName(Args);                          \
}

namespace HBE
{

    template <typename Implementation>
    class HB_API ContextBase
    {
        Implementation context_impl;

    public :

        //lifetime
        inline HBE_RESULT init(const ContextInfo& info) { return context_impl.init(info); }

        inline HBE_RESULT release() { return context_impl.release(); };

        CONTEXT_API_FUNC(HBE_RESULT, rendererBeginRecordCommands, FUNC_PARAMS(), FUNC_ARGS());
        CONTEXT_API_FUNC(HBE_RESULT, rendererEndRecordCommandsAndSubmit, FUNC_PARAMS(), FUNC_ARGS());
        CONTEXT_API_FUNC(HBE_RESULT, rendererGetResources, FUNC_PARAMS(RendererResources& resources), FUNC_ARGS(resources));
        //cmds
        CONTEXT_API_FUNC(HBE_RESULT, cmdRasterizeGraph, FUNC_PARAMS(const RasterizeGraphCmdInfo& info), FUNC_ARGS(info));
        CONTEXT_API_FUNC(HBE_RESULT, cmdRaytrace, FUNC_PARAMS(const TraceRaysCmdInfo& info), FUNC_ARGS(info));
        CONTEXT_API_FUNC(HBE_RESULT, cmdDispatch, FUNC_PARAMS(const ComputeDispatchCmdInfo& info), FUNC_ARGS(info));
        CONTEXT_API_FUNC(HBE_RESULT, cmdPresent, FUNC_PARAMS(const PresentCmdInfo& info), FUNC_ARGS(info));
        //CONTEXT_API_FUNC(void, getGraphicLimits, FUNC_PARAMS(GraphicLimits& limits), FUNC_ARGS(limits))
        //create
        CONTEXT_API_FUNC(HBE_RESULT, createImage, FUNC_PARAMS(ImageHandle& handle,const ImageInfo& info), FUNC_ARGS(handle,info))
        CONTEXT_API_FUNC(HBE_RESULT, createMesh, FUNC_PARAMS(MeshHandle &handle,const MeshInfo& mesh_info), FUNC_ARGS(handle,mesh_info));
        CONTEXT_API_FUNC(HBE_RESULT, createPipelineInstance, FUNC_PARAMS(PipelineInstanceHandle& handle,const PipelineInstanceInfo& info), FUNC_ARGS(handle,info))
        CONTEXT_API_FUNC(HBE_RESULT, createShader, FUNC_PARAMS(ShaderHandle& handle,const ShaderInfo& info), FUNC_ARGS(handle,info));
        CONTEXT_API_FUNC(HBE_RESULT, createRasterizationTarget, FUNC_PARAMS(RasterizationTargetHandle &handle,const RasterizationTargetInfo& rasterization_target_info), FUNC_ARGS(handle,rasterization_target_info));
        CONTEXT_API_FUNC(HBE_RESULT, createRasterizationPipeline, FUNC_PARAMS(RasterizationPipelineHandle& handle,const RasterizationPipelineInfo& raster_pipeline_info), FUNC_ARGS(handle,raster_pipeline_info));
        CONTEXT_API_FUNC(HBE_RESULT, createComputePipeline, FUNC_PARAMS(ComputePipelineHandle& handle,const ComputePipelineInfo& raster_pipeline_info), FUNC_ARGS(handle,raster_pipeline_info));
        CONTEXT_API_FUNC(HBE_RESULT, createRaytracingPipeline, FUNC_PARAMS(RaytracingPipelineHandle& handle,const RaytracingPipelineInfo& raster_pipeline_info), FUNC_ARGS(handle,raster_pipeline_info));
        CONTEXT_API_FUNC(HBE_RESULT, createRootAccelerationStructure, FUNC_PARAMS(RootAccelerationStructureHandle& handle,const RootAccelerationStructureInfo& root_acceleration_structure_info_info), FUNC_ARGS(handle,root_acceleration_structure_info_info));
        CONTEXT_API_FUNC(HBE_RESULT, createAABBAccelerationStructure, FUNC_PARAMS(AABBAccelerationStructureHandle& handle,const AABBAccelerationStructureInfo& aabb_acceleration_structure_info_info), FUNC_ARGS(handle,aabb_acceleration_structure_info_info));
        CONTEXT_API_FUNC(HBE_RESULT, createMeshAccelerationStructure, FUNC_PARAMS(MeshAccelerationStructureHandle& handle,const MeshAccelerationStructureInfo& mesh_acceleration_structure_info_info), FUNC_ARGS(handle,mesh_acceleration_structure_info_info));

        CONTEXT_API_FUNC(HBE_RESULT, releaseImage, FUNC_PARAMS(ImageHandle handle), FUNC_ARGS(handle))
        CONTEXT_API_FUNC(HBE_RESULT, releaseMesh, FUNC_PARAMS(MeshHandle handle), FUNC_ARGS(handle));
        CONTEXT_API_FUNC(HBE_RESULT, releasePipelineInstance, FUNC_PARAMS(PipelineInstanceHandle handle), FUNC_ARGS(handle))
        CONTEXT_API_FUNC(HBE_RESULT, releaseShader, FUNC_PARAMS(ShaderHandle handle), FUNC_ARGS(handle))
        CONTEXT_API_FUNC(HBE_RESULT, releaseRasterizationTarget, FUNC_PARAMS(RasterizationTargetHandle handle), FUNC_ARGS(handle));
        CONTEXT_API_FUNC(HBE_RESULT, releaseRasterizationPipeline, FUNC_PARAMS(RasterizationPipelineHandle handle), FUNC_ARGS(handle));
        CONTEXT_API_FUNC(HBE_RESULT, releaseComputePipeline, FUNC_PARAMS(ComputePipelineHandle handle), FUNC_ARGS(handle));
        CONTEXT_API_FUNC(HBE_RESULT, releaseRaytracingPipeline, FUNC_PARAMS(RaytracingPipelineHandle handle), FUNC_ARGS(handle));
        CONTEXT_API_FUNC(HBE_RESULT, releaseRootAccelerationStructure, FUNC_PARAMS(RootAccelerationStructureHandle handle), FUNC_ARGS(handle));
        CONTEXT_API_FUNC(HBE_RESULT, releaseAABBAccelerationStructure, FUNC_PARAMS(AABBAccelerationStructureHandle handle), FUNC_ARGS(handle));
        CONTEXT_API_FUNC(HBE_RESULT, releaseMeshAccelerationStructure, FUNC_PARAMS(MeshAccelerationStructureHandle handle), FUNC_ARGS(handle));

        //images
        CONTEXT_API_FUNC(HBE_RESULT, updateImage, FUNC_PARAMS(ImageHandle handle, const void* data), FUNC_ARGS(handle,data));
        CONTEXT_API_FUNC(HBE_RESULT, getImageSize, FUNC_PARAMS(ImageHandle handle,uvec3& size_ref), FUNC_ARGS(handle,size_ref));

        //meshes
        CONTEXT_API_FUNC(HBE_RESULT, setMeshVertexIndices16, FUNC_PARAMS(MeshHandle handle,const uint16_t *indices, size_t count), FUNC_ARGS(handle,indices,count));
        CONTEXT_API_FUNC(HBE_RESULT, setMeshVertexIndices, FUNC_PARAMS(MeshHandle handle,const uint32_t *indices, size_t count), FUNC_ARGS(handle,indices,count));
        CONTEXT_API_FUNC(HBE_RESULT, setMeshVertexBuffer, FUNC_PARAMS(MeshHandle handle,uint32_t location,const void *vertices, size_t count), FUNC_ARGS(handle,location,vertices,count));
        CONTEXT_API_FUNC(HBE_RESULT, setMeshInstanceBuffer, FUNC_PARAMS(MeshHandle handle,uint32_t location,const void *buffer, size_t count), FUNC_ARGS(handle,location,buffer,count));

        //pipeline instance
        CONTEXT_API_FUNC(HBE_RESULT, setPipelineInstanceUniform, FUNC_PARAMS(PipelineInstanceHandle handle,uint32_t binding, const void* data,int32_t frame_index), FUNC_ARGS(handle,binding, data,frame_index));
        CONTEXT_API_FUNC(HBE_RESULT, setPipelineInstanceImage, FUNC_PARAMS(PipelineInstanceHandle handle,uint32_t binding, ImageHandle image, uint32_t mip_level,int32_t frame_index), FUNC_ARGS(handle,binding, image, mip_level,frame_index));
        CONTEXT_API_FUNC(HBE_RESULT, setPipelineInstanceImageArray, FUNC_PARAMS(PipelineInstanceHandle handle,uint32_t binding, ImageHandle* images, uint32_t images_count, uint32_t mip_level,int32_t frame_index), FUNC_ARGS(handle,binding, images, images_count, mip_level,frame_index));
        CONTEXT_API_FUNC(HBE_RESULT, setPipelineInstanceStorageBuffer, FUNC_PARAMS(PipelineInstanceHandle handle,uint32_t binding, StorageBufferHandle buffer, size_t count, size_t offset,int32_t frame_index), FUNC_ARGS(handle,binding, buffer, count, offset, frame_index));
        CONTEXT_API_FUNC(HBE_RESULT, setPipelineInstanceStorageBufferArray, FUNC_PARAMS(PipelineInstanceHandle handle,uint32_t binding, StorageBufferHandle* buffers, uint32_t count,int32_t frame_index), FUNC_ARGS(handle,binding, buffers, count,frame_index));
        CONTEXT_API_FUNC(HBE_RESULT, setPipelineInstanceTexelBuffer, FUNC_PARAMS(PipelineInstanceHandle handle,uint32_t binding, TexelBufferHandle buffer,int32_t frame_index), FUNC_ARGS(handle,binding, buffer,frame_index));
        CONTEXT_API_FUNC(HBE_RESULT, setPipelineInstanceTexelBufferArray, FUNC_PARAMS(PipelineInstanceHandle handle,uint32_t binding, TexelBufferHandle* buffers, uint32_t count,int32_t frame_index), FUNC_ARGS(handle,binding, buffers, count,frame_index));
        CONTEXT_API_FUNC(HBE_RESULT, setPipelineInstanceAccelerationStructure, FUNC_PARAMS(PipelineInstanceHandle handle,uint32_t binding, RootAccelerationStructureHandle acceleration_structure,int32_t frame_index), FUNC_ARGS(handle,binding, acceleration_structure,frame_index));

        //Rasterization Target
        CONTEXT_API_FUNC(HBE_RESULT, getRasterizationTargetResolution, FUNC_PARAMS(RasterizationTargetHandle handle,vec2u& resolution), FUNC_ARGS(handle,resolution));
        CONTEXT_API_FUNC(HBE_RESULT, setRasterizationTargetResolution, FUNC_PARAMS(RasterizationTargetHandle handle,vec2u resolution), FUNC_ARGS(handle,resolution));
        CONTEXT_API_FUNC(HBE_RESULT, getRasterizationTargetFrameBuffer, FUNC_PARAMS(RasterizationTargetHandle handle), FUNC_ARGS(handle));

        //Fences
        CONTEXT_API_FUNC(HBE_RESULT, waitForFence, FUNC_PARAMS(FenceHandle handle), FUNC_ARGS(handle))
        CONTEXT_API_FUNC(HBE_RESULT, getFenceStatus, FUNC_PARAMS(FenceHandle handle,FENCE_STATUS& status), FUNC_ARGS(handle,status))
    };
}
