
#include "Resources.h"
#include "core/resource/Shader.h"
#include "core/resource/RasterizationPipeline.h"
#include "core/resource/ComputePipeline.h"
#include "core/resource/Mesh.h"
#include "core/resource/Image.h"
#include "core/resource/Model.h"
#include "core/resource/RenderTarget.h"
#include "core/resource/RasterizationPipelineInstance.h"
#include "core/resource/ComputeInstance.h"
#include "core/resource/raytracing/AccelerationStructure.h"
#include "core/resource/raytracing/RaytracingPipeline.h"
#include "core/resource/raytracing/RaytracingPipelineInstance.h"
#include "core/resource/StorageBuffer.h"
#include "AudioClipInstance.h"
#include "core/resource/TexelBuffer.h"

namespace HBE {
	const ResourceFactory *Resources::factory = nullptr;
	std::unordered_map<std::string, Resource *> Resources::registry = std::unordered_map<std::string, Resource *>();

	Model *Resources::createModel(const ModelInfo &info, const std::string &name) {
		auto m = new Model(info);
		add(name, m);
		return m;
	}

	Model *Resources::createModel(const ModelInfo &info) {

		return new Model(info);
	}

	Mesh *Resources::createMesh(const MeshInfo &info, const std::string &name) {
		auto m = factory->createMesh(info);
		add(name, m);
		return m;
	}

	Mesh *Resources::createMesh(const MeshInfo &info) {
		return factory->createMesh(info);
	}


	Shader *Resources::createShader(const ShaderInfo &info, const std::string &name) {
		auto s = factory->createShader(info);
		add(name, s);
		return s;
	}

	Shader *Resources::createShader(const ShaderInfo &info) {
		return factory->createShader(info);
	}

	RasterizationPipeline *Resources::createRasterizationPipeline(const RasterizationPipelineInfo &info, const std::string &name) {
		auto gp = factory->createRasterizationPipeline(info);
		add(name, gp);
		return gp;

	}

	RasterizationPipeline *Resources::createRasterizationPipeline(const RasterizationPipelineInfo &info) {
		return factory->createRasterizationPipeline(info);
	}

	ComputePipeline *Resources::createComputePipeline(const ComputePipelineInfo &info, const std::string &name) {
		auto cp = factory->createComputePipeline(info);
		add(name, cp);
		return cp;
	}

	ComputePipeline *Resources::createComputePipeline(const ComputePipelineInfo &info) {
		return factory->createComputePipeline(info);
	}

	Image *Resources::createTexture(const ImageInfo &info, const std::string &name) {
		auto t = factory->createTexture(info);
		add(name, t);
		return t;
	}

	Image *Resources::createTexture(const ImageInfo &info) {
		return factory->createTexture(info);
	}


	RenderTarget *Resources::createRenderTarget(const RenderTargetInfo &info) {
		return factory->createRenderTarget(info);
	}

	RenderTarget *Resources::createRenderTarget(const RenderTargetInfo &info, const std::string &name) {
		auto rt = factory->createRenderTarget(info);
		add(name, rt);
		return rt;
	}

	RasterizationPipelineInstance *Resources::createRasterizationPipelineInstance(const RasterizationPipelineInstanceInfo &info, const std::string &name) {
		RasterizationPipelineInstance *m = factory->createRasterizationPipelineInstance(info);
		add(name, m);
		return m;
	}

	RasterizationPipelineInstance *Resources::createRasterizationPipelineInstance(const RasterizationPipelineInstanceInfo &info) {
		return factory->createRasterizationPipelineInstance(info);
	}

	ComputeInstance *Resources::createComputeInstance(const ComputeInstanceInfo &info, const std::string &name) {
		ComputeInstance *c = factory->createComputeInstance(info);
		add(name, c);
		return c;
	}

	ComputeInstance *Resources::createComputeInstance(const ComputeInstanceInfo &info) {
		return factory->createComputeInstance(info);
	}

	RootAccelerationStructure *Resources::createRootAccelerationStructure(const RootAccelerationStructureInfo &info, const std::string &name) {
		RootAccelerationStructure *r = factory->createRootAccelerationStructure(info);
		add(name, r);
		return r;
	}

	RootAccelerationStructure *Resources::createRootAccelerationStructure(const RootAccelerationStructureInfo &info) {
		return factory->createRootAccelerationStructure(info);
	}

	AABBAccelerationStructure *Resources::createAABBAccelerationStructure(const AABBAccelerationStructureInfo &info, const std::string &name) {
		AABBAccelerationStructure *a = factory->createAABBAccelerationStructure(info);
		add(name, a);
		return a;
	}

	AABBAccelerationStructure *Resources::createAABBAccelerationStructure(const AABBAccelerationStructureInfo &info) {
		return factory->createAABBAccelerationStructure(info);
	}

	MeshAccelerationStructure *Resources::createMeshAccelerationStructure(const MeshAccelerationStructureInfo &info, const std::string &name) {
		MeshAccelerationStructure *m = factory->createMeshAccelerationStructure(info);
		add(name, m);
		return m;
	}

	MeshAccelerationStructure *Resources::createMeshAccelerationStructure(const MeshAccelerationStructureInfo &info) {
		return factory->createMeshAccelerationStructure(info);
	}

	RaytracingPipeline *Resources::createRaytracingPipeline(const RaytracingPipelineInfo &info, const std::string &name) {
		RaytracingPipeline *rtp = factory->createRaytracingPipeline(info);
		add(name, rtp);
		return rtp;
	}

	RaytracingPipeline *Resources::createRaytracingPipeline(const RaytracingPipelineInfo &info) {
		return factory->createRaytracingPipeline(info);
	}

	RaytracingPipelineInstance *Resources::createRaytracingPipelineInstance(const RaytracingPipelineInstanceInfo &info, const std::string &name) {
		RaytracingPipelineInstance *rtpi = factory->createRaytracingPipelineInstance(info);
		add(name, rtpi);
		return rtpi;
	}

	RaytracingPipelineInstance *Resources::createRaytracingPipelineInstance(const RaytracingPipelineInstanceInfo &info) {
		return factory->createRaytracingPipelineInstance(info);
	}

	StorageBuffer *Resources::createStorageBuffer(const StorageBufferInfo &info, const std::string &name) {
		StorageBuffer *sb = factory->createStorageBuffer(info);
		add(name, sb);
		return sb;
	}

	StorageBuffer *Resources::createStorageBuffer(const StorageBufferInfo &info) {
		return factory->createStorageBuffer(info);
	}

	Font *Resources::createFont(const FontInfo &info) {
		return new Font(info);
	}

	Font *Resources::createFont(const FontInfo &info, const std::string &name) {
		Font *f = new Font(info);
		add(name, f);
		return f;
	}

	AudioClip *Resources::createAudioClip(const AudioClipInfo &info) {
		return new AudioClip(info);
	}

	AudioClip *Resources::createAudioClip(const AudioClipInfo &info, const std::string &name) {
		AudioClip *ac = new AudioClip(info);
		add(name, ac);
		return ac;
	}

	AudioClipInstance *Resources::createAudioClipInstance(const AudioClipInstanceInfo &info) {
		return new AudioClipInstance(info);
	}

	AudioClipInstance *Resources::createAudioClipInstance(const AudioClipInstanceInfo &info, const std::string &name) {
		AudioClipInstance *ac = new AudioClipInstance(info);
		add(name, ac);
		return ac;
	}

	TexelBuffer *Resources::createTexelBuffer(const TexelBufferInfo &info, const std::string &name) {
		TexelBuffer *tb = factory->createTexelBuffer(info);
		add(name, tb);
		return tb;
	}

	TexelBuffer *Resources::createTexelBuffer(const TexelBufferInfo &info) {
		return factory->createTexelBuffer(info);
	}
}
