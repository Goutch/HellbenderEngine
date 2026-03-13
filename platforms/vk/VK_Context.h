#pragma once

#include "VK_Allocator.h"
#include "VK_ComputePipeline.h"
#include "VK_Device.h"
#include "VK_Renderer.h"
#include "VK_Surface.h"
#include "VK_Swapchain.h"
#include "VK_Instance.h"
#include "VK_RenderPass.h"

#include "core/interface/ContextInterface.h"
#include "core/interface/ComputePipelineInterface.h"
#include "core/interface/AABBAccelerationStructureInterface.h"
#include "core/interface/FenceInterface.h"
#include "core/interface/RootAccelerationStructureInterface.h"
#include "Core/interface/MeshAccelerationStructureInterface.h"
#include "resources/VK_PipelineInstance.h"
#include "resources/VK_RasterizationPipeline.h"
#include "resources/VK_RasterizationTargets.h"
#include "resources/raytracing/VK_AABBBottomLevelAccelerationStructure.h"
#include "resources/raytracing/VK_MeshBottomLevelAccelerationStructure.h"
#include "resources/raytracing/VK_RaytracingPipeline.h"
#include "resources/raytracing/VK_TopLevelAccelerationStructure.h"
#include "platforms/vk/VK_RenderPass.h"
#include "VK_TexelBuffer.h"

#define VK_CONTEXT_CMD_API_FUNC(ReturnType, FuncName, Params, Args)    \
inline ReturnType FuncName(Params)          \
{                                           \
    renderer.FuncName(Args);                \
    return HBE_RESULT_SUCCESS;           \
}
#define VK_CONTEXT_CREATE_API_FUNC(ReturnType, FuncName, Params, Args, Collection)    \
inline ReturnType FuncName(Params)\
{                                           \
        Collection.create();                \
        Collection[handle].alloc(this,info);    \
        return HBE_RESULT_SUCCESS;          \
}

#define VK_CONTEXT_RELEASE_API_FUNC(ReturnType, FuncName, Params, Args, Collection)      \
inline ReturnType FuncName(Params)\
{                                                                                       \
    if(!Collection.valid(handle)) return HBE_RESULT_INVALID_HANDLE;                  \
    Collection[handle].release();                                                    \
    Collection.release(handle);                                                      \
    return HBE_RESULT_SUCCESS;                                                          \
}

#define VK_CONTEXT_MEMBER_CALL_API_FUNC(ReturnType, FuncName, Params, Args, Collection, MemberName, MemberArgs)    \
inline ReturnType FuncName(Params)                                                                                  \
{                                                                                                                    \
    if(!Collection.valid(handle)) return HBE_RESULT_INVALID_HANDLE;                                               \
       Collection[handle].MemberName(MemberArgs);                                                              \
    return HBE_RESULT_SUCCESS;                                                                                      \
}

#define VK_CONTEXT_RENDERER_API_FUNC(ReturnType, FuncName, Params, Args, MemberName, MemberArgs)    \
inline ReturnType FuncName(Params)                                                                                  \
{                                                                                                               \
   renderer.MemberName(MemberArgs);                                                                             \
   return HBE_RESULT_SUCCESS;                                                                                      \
}
#define FUNC_ARGS(...) __VA_ARGS__
#define FUNC_PARAMS(...) __VA_ARGS__

namespace HBE {
	class VK_Context {
	public:
		VK_Instance instance{};
		VK_Surface surface{};
		VK_PhysicalDevice physical_device{};
		VK_Device device{};
		VK_Allocator allocator{};
		VK_Swapchain swapchain{};
		VK_Renderer renderer{};
		GraphicLimits graphic_limits{};

		//resources
		StableHandleContainer<VK_Image, 64> images;
		StableHandleContainer<VK_Shader, 32> shaders;
		StableHandleContainer<VK_Mesh, 64> meshes;
		StableHandleContainer<VK_RasterizationPipeline, 16> rasterization_pipelines;
		StableHandleContainer<VK_RaytracingPipeline, 16> raytracing_pipelines;
		StableHandleContainer<VK_ComputePipeline, 16> compute_pipelines;
		StableHandleContainer<VK_PipelineInstance, 64> pipeline_instances;
		StableHandleContainer<VK_RenderPass, 16> rasterization_targets;
		StableHandleContainer<VK_TopLevelAccelerationStructure, 16> root_acceleration_structures;
		StableHandleContainer<VK_AABBBottomLevelAccelerationStructure, 16> aabb_acceleration_structures;
		StableHandleContainer<VK_MeshBottomLevelAccelerationStructure, 16> mesh_acceleration_structures;
		StableHandleContainer<VK_Fence, 16> fences;
		StableHandleContainer<VK_StorageBuffer, 16> storage_buffers;
		StableHandleContainer<VK_TexelBuffer, 16> texel_buffers;


		HBE_RESULT init(const ContextInfo &info);

		HBE_RESULT release();

		//renderer
		VK_CONTEXT_RENDERER_API_FUNC(HBE_RESULT, rendererBeginRecordCommands, FUNC_PARAMS(), FUNC_ARGS(), beginFrame, FUNC_ARGS());

		VK_CONTEXT_RENDERER_API_FUNC(HBE_RESULT, rendererEndRecordCommandsAndSubmit, FUNC_PARAMS(), FUNC_ARGS(), endFrame, FUNC_ARGS());

		VK_CONTEXT_RENDERER_API_FUNC(HBE_RESULT, rendererGetResources, FUNC_PARAMS(RendererResources & resources), FUNC_ARGS(resources), getRendererResrouces, FUNC_ARGS(resources));
		//cmds
		VK_CONTEXT_CMD_API_FUNC(HBE_RESULT, cmdRasterizeGraph, FUNC_PARAMS(const RasterizeGraphCmdInfo &info), FUNC_ARGS(info));

		VK_CONTEXT_CMD_API_FUNC(HBE_RESULT, cmdTraceRays, FUNC_PARAMS(const TraceRaysCmdInfo &info), FUNC_ARGS(info));

		VK_CONTEXT_CMD_API_FUNC(HBE_RESULT, cmdDispatch, FUNC_PARAMS(const ComputeDispatchCmdInfo &info), FUNC_ARGS(info));

		VK_CONTEXT_CMD_API_FUNC(HBE_RESULT, cmdPresent, FUNC_PARAMS(const PresentCmdInfo &info), FUNC_ARGS(info));

		//create
		VK_CONTEXT_CREATE_API_FUNC(HBE_RESULT, createImage, FUNC_PARAMS(ImageHandle & handle,const ImageInfo &info), FUNC_ARGS(handle, info), images)

		VK_CONTEXT_CREATE_API_FUNC(HBE_RESULT, createMesh, FUNC_PARAMS(MeshHandle & handle,const MeshInfo &info), FUNC_ARGS(handle, info), meshes);

		VK_CONTEXT_CREATE_API_FUNC(HBE_RESULT, createPipelineInstance, FUNC_PARAMS(PipelineInstanceHandle & handle,const PipelineInstanceInfo &info), FUNC_ARGS(handle, info), pipeline_instances)

		VK_CONTEXT_CREATE_API_FUNC(HBE_RESULT, createShader, FUNC_PARAMS(ShaderHandle & handle,const ShaderInfo &info), FUNC_ARGS(handle, info), shaders);

		VK_CONTEXT_CREATE_API_FUNC(HBE_RESULT, createRasterizationTarget, FUNC_PARAMS(RasterizationTargetHandle & handle,const RasterizationTargetInfo &info), FUNC_ARGS(handle, info),
		                           rasterization_targets);

		VK_CONTEXT_CREATE_API_FUNC(HBE_RESULT, createRasterizationPipeline, FUNC_PARAMS(RasterizationPipelineHandle & handle,const RasterizationPipelineInfo &info), FUNC_ARGS(handle, info),
		                           rasterization_pipelines);

		VK_CONTEXT_CREATE_API_FUNC(HBE_RESULT, createComputePipeline, FUNC_PARAMS(ComputePipelineHandle & handle,const ComputePipelineInfo &info), FUNC_ARGS(handle, info), compute_pipelines);

		VK_CONTEXT_CREATE_API_FUNC(HBE_RESULT, createRaytracingPipeline, FUNC_PARAMS(RaytracingPipelineHandle & handle,const RaytracingPipelineInfo &info), FUNC_ARGS(handle, info),
		                           raytracing_pipelines);

		VK_CONTEXT_CREATE_API_FUNC(HBE_RESULT, createRootAccelerationStructure, FUNC_PARAMS(RootAccelerationStructureHandle & handle,const RootAccelerationStructureInfo &info),
		                           FUNC_ARGS(handle, info), root_acceleration_structures);

		VK_CONTEXT_CREATE_API_FUNC(HBE_RESULT, createAABBAccelerationStructure, FUNC_PARAMS(AABBAccelerationStructureHandle & handle,const AABBAccelerationStructureInfo &info),
		                           FUNC_ARGS(handle, info), aabb_acceleration_structures);

		VK_CONTEXT_CREATE_API_FUNC(HBE_RESULT, createMeshAccelerationStructure, FUNC_PARAMS(MeshAccelerationStructureHandle & handle,const MeshAccelerationStructureInfo &info),
		                           FUNC_ARGS(handle, info), mesh_acceleration_structures);

		VK_CONTEXT_CREATE_API_FUNC(HBE_RESULT, createStorageBuffer, FUNC_PARAMS(StorageBufferHandle & handle,const StorageBufferInfo &info), FUNC_ARGS(handle, info), storage_buffers);

		VK_CONTEXT_CREATE_API_FUNC(HBE_RESULT, createTexelBuffer, FUNC_PARAMS(TexelBufferHandle & handle,const TexelBufferInfo &info), FUNC_ARGS(handle, info), texel_buffers);

		VK_CONTEXT_RELEASE_API_FUNC(HBE_RESULT, releaseImage, FUNC_PARAMS(ImageHandle handle), FUNC_ARGS(handle), images)

		VK_CONTEXT_RELEASE_API_FUNC(HBE_RESULT, releaseMesh, FUNC_PARAMS(MeshHandle handle), FUNC_ARGS(handle), meshes);

		VK_CONTEXT_RELEASE_API_FUNC(HBE_RESULT, releasePipelineInstance, FUNC_PARAMS(PipelineInstanceHandle handle), FUNC_ARGS(handle), pipeline_instances)

		VK_CONTEXT_RELEASE_API_FUNC(HBE_RESULT, releaseShader, FUNC_PARAMS(ShaderHandle handle), FUNC_ARGS(handle), shaders)

		VK_CONTEXT_RELEASE_API_FUNC(HBE_RESULT, releaseRasterizationTarget, FUNC_PARAMS(RasterizationTargetHandle handle), FUNC_ARGS(handle), rasterization_targets);

		VK_CONTEXT_RELEASE_API_FUNC(HBE_RESULT, releaseRasterizationPipeline, FUNC_PARAMS(RasterizationPipelineHandle handle), FUNC_ARGS(handle), rasterization_pipelines);

		VK_CONTEXT_RELEASE_API_FUNC(HBE_RESULT, releaseComputePipeline, FUNC_PARAMS(ComputePipelineHandle handle), FUNC_ARGS(handle), compute_pipelines);

		VK_CONTEXT_RELEASE_API_FUNC(HBE_RESULT, releaseRaytracingPipeline, FUNC_PARAMS(RaytracingPipelineHandle handle), FUNC_ARGS(handle), rasterization_pipelines);

		VK_CONTEXT_RELEASE_API_FUNC(HBE_RESULT, releaseRootAccelerationStructure, FUNC_PARAMS(RootAccelerationStructureHandle handle), FUNC_ARGS(handle), root_acceleration_structures);

		VK_CONTEXT_RELEASE_API_FUNC(HBE_RESULT, releaseAABBAccelerationStructure, FUNC_PARAMS(AABBAccelerationStructureHandle handle), FUNC_ARGS(handle), aabb_acceleration_structures);

		VK_CONTEXT_RELEASE_API_FUNC(HBE_RESULT, releaseMeshAccelerationStructure, FUNC_PARAMS(MeshAccelerationStructureHandle handle), FUNC_ARGS(handle), mesh_acceleration_structures);

		VK_CONTEXT_RELEASE_API_FUNC(HBE_RESULT, releaseStorageBuffer, FUNC_PARAMS(StorageBufferHandle handle), FUNC_ARGS(handle), storage_buffers);

		VK_CONTEXT_RELEASE_API_FUNC(HBE_RESULT, releaseTexelBuffer, FUNC_PARAMS(TexelBufferHandle handle), FUNC_ARGS(handle), texel_buffers);

		//images
		VK_CONTEXT_MEMBER_CALL_API_FUNC(HBE_RESULT, updateImage, FUNC_PARAMS(ImageHandle handle, const void *data), FUNC_ARGS(handle, data), images, update, FUNC_ARGS(data));

		VK_CONTEXT_MEMBER_CALL_API_FUNC(HBE_RESULT, getImageSize, FUNC_PARAMS(ImageHandle handle, uvec3 & size_ref), FUNC_ARGS(handle, size_ref), images, getSize, FUNC_ARGS());

		//meshes
		VK_CONTEXT_MEMBER_CALL_API_FUNC(HBE_RESULT, setMeshVertexIndices16, FUNC_PARAMS(MeshHandle handle,const uint16_t *indices, size_t count), FUNC_ARGS(handle, indices, count), meshes,
		                                setVertexIndices, FUNC_ARGS(indices, count));

		VK_CONTEXT_MEMBER_CALL_API_FUNC(HBE_RESULT, setMeshVertexIndices, FUNC_PARAMS(MeshHandle handle,const uint32_t *indices, size_t count), FUNC_ARGS(handle, indices, count), meshes,
		                                setVertexIndices, FUNC_ARGS(indices, count));

		VK_CONTEXT_MEMBER_CALL_API_FUNC(HBE_RESULT, setMeshVertexBuffer, FUNC_PARAMS(MeshHandle handle, uint32_t location,const void *vertices, size_t count),
		                                FUNC_ARGS(handle, location, vertices, count), meshes, setBuffer, FUNC_ARGS(location, vertices, count));

		VK_CONTEXT_MEMBER_CALL_API_FUNC(HBE_RESULT, setMeshInstanceBuffer, FUNC_PARAMS(MeshHandle handle, uint32_t location,const void *buffer, size_t count),
		                                FUNC_ARGS(handle, location, buffer, count), meshes, setInstanceBuffer, FUNC_ARGS(location, buffer, count));

		//pipeline instance
		VK_CONTEXT_MEMBER_CALL_API_FUNC(HBE_RESULT, setPipelineInstanceUniform, FUNC_PARAMS(PipelineInstanceHandle handle, uint32_t binding, const void *data, int32_t frame_index),
		                                FUNC_ARGS(handle, binding, data, frame_index), pipeline_instances, setUniform, FUNC_ARGS(binding, data, frame_index));

		VK_CONTEXT_MEMBER_CALL_API_FUNC(HBE_RESULT, setPipelineInstanceImage, FUNC_PARAMS(PipelineInstanceHandle handle, uint32_t binding, ImageHandle image, uint32_t mip_level, int32_t frame_index),
		                                FUNC_ARGS(handle, binding, image, mip_level, frame_index), pipeline_instances, setImage, FUNC_ARGS(binding, image, mip_level, frame_index));

		VK_CONTEXT_MEMBER_CALL_API_FUNC(HBE_RESULT, setPipelineInstanceImageArray,
		                                FUNC_PARAMS(PipelineInstanceHandle handle, uint32_t binding, ImageHandle * images, uint32_t images_count, uint32_t mip_level, int32_t frame_index),
		                                FUNC_ARGS(handle, binding, images, images_count, mip_level, frame_index), pipeline_instances, setImageArray,
		                                FUNC_ARGS(binding, images, images_count, mip_level, frame_index));

		VK_CONTEXT_MEMBER_CALL_API_FUNC(HBE_RESULT, setPipelineInstanceStorageBuffer,
		                                FUNC_PARAMS(PipelineInstanceHandle handle, uint32_t binding, StorageBufferHandle buffer, size_t count, size_t offset, int32_t frame_index),
		                                FUNC_ARGS(handle, binding, buffer, count, offset, frame, frame_index), pipeline_instances, setStorageBuffer,
		                                FUNC_ARGS(binding, buffer, count, offset, frame_index));

		VK_CONTEXT_MEMBER_CALL_API_FUNC(HBE_RESULT, setPipelineInstanceStorageBufferArray,
		                                FUNC_PARAMS(PipelineInstanceHandle handle, uint32_t binding, StorageBufferHandle * buffers, uint32_t count, int32_t frame_index),
		                                FUNC_ARGS(handle, binding, buffers, count, frame_index), pipeline_instances, setStorageBufferArray, FUNC_ARGS(binding, buffers, count, frame_index));

		VK_CONTEXT_MEMBER_CALL_API_FUNC(HBE_RESULT, setPipelineInstanceTexelBuffer, FUNC_PARAMS(PipelineInstanceHandle handle, uint32_t binding, TexelBufferHandle buffer, int32_t frame_index),
		                                FUNC_ARGS(handle, binding, buffer, frame_index), pipeline_instances, setTexelBuffer, FUNC_ARGS(binding, buffer, frame_index));

		VK_CONTEXT_MEMBER_CALL_API_FUNC(HBE_RESULT, setPipelineInstanceTexelBufferArray,
		                                FUNC_PARAMS(PipelineInstanceHandle handle, uint32_t binding, TexelBufferHandle * buffers, uint32_t count, int32_t frame_index),
		                                FUNC_ARGS(handle, binding, buffers, count, frame_index), pipeline_instances, setStorageBufferArray, FUNC_ARGS(binding, buffers, count, frame_index));

		VK_CONTEXT_MEMBER_CALL_API_FUNC(HBE_RESULT, setPipelineInstanceAccelerationStructure,
		                                FUNC_PARAMS(PipelineInstanceHandle handle, uint32_t binding, RootAccelerationStructureHandle acceleration_structure, int32_t frame_index),
		                                FUNC_ARGS(handle, binding, acceleration_structure, frame_index), pipeline_instances, setAccelerationStructure,
		                                FUNC_ARGS(binding, acceleration_structure, frame_index));

		VK_CONTEXT_MEMBER_CALL_API_FUNC(HBE_RESULT, getPipelineInstanceBindingFromString, FUNC_PARAMS(PipelineInstanceHandle handle,const char *str, uint32_t&binding), FUNC_ARGS(handle, str, binding),
		                                pipeline_instances, getBinding, FUNC_ARGS(str, binding));
		//Rasterization Target
		VK_CONTEXT_MEMBER_CALL_API_FUNC(HBE_RESULT, getRasterizationTargetResolution, FUNC_PARAMS(RasterizationTargetHandle handle, vec2u & resolution), FUNC_ARGS(handle, resolution),
		                                rasterization_targets, getResolution, FUNC_ARGS(resolution));

		VK_CONTEXT_MEMBER_CALL_API_FUNC(HBE_RESULT, setRasterizationTargetResolution, FUNC_PARAMS(RasterizationTargetHandle handle, vec2u resolution), FUNC_ARGS(handle, resolution),
		                                rasterization_targets, setResolution, FUNC_ARGS(resolution));

		VK_CONTEXT_MEMBER_CALL_API_FUNC(HBE_RESULT, getRasterizationTargetFrameBuffer, FUNC_PARAMS(RasterizationTargetHandle handle), FUNC_ARGS(handle), rasterization_targets, getFramebufferTexture,
		                                FUNC_ARGS());

		//fences
		VK_CONTEXT_MEMBER_CALL_API_FUNC(HBE_RESULT, waitForFence, FUNC_PARAMS(FenceHandle handle), FUNC_ARGS(handle), fences, wait, FUNC_ARGS())

		VK_CONTEXT_MEMBER_CALL_API_FUNC(HBE_RESULT, getFenceStatus, FUNC_PARAMS(FenceHandle handle, FENCE_STATUS & status), FUNC_ARGS(handle, status), fences, getStatus, FUNC_ARGS(status))

		HBE_RESULT getGraphicLimits(GraphicLimits &graphic_limits) {
			graphic_limits = this->graphic_limits;
			return HBE_RESULT_SUCCESS;
		}

		HBE_RESULT getPipelineFromInstance(PipelineInstanceHandle handle, Handle &pipeline_handle) {
			pipeline_handle = pipeline_instances[handle].getPipeline();
			return HBE_RESULT_SUCCESS;
		}
	};
}
