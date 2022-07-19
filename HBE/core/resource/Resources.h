#pragma once

#include "Core.h"
#include "memory"
#include "ResourceFactory.h"
#include "core/graphics/Renderer.h"
#include "core/graphics/Graphics.h"
#include "core/resource/Shader.h"
#include "core/resource/GraphicPipeline.h"
#include "core/resource/ComputePipeline.h"
#include "core/resource/Mesh.h"
#include "core/resource/Texture.h"
#include "core/resource/Model.h"
#include "core/resource/RenderTarget.h"


namespace HBE {
	class HB_API Resources {
		static std::unordered_map<std::string, Resource *> registry;
		static const ResourceFactory *factory;
	public:
		static void init(const ResourceFactory &factory) {
			Resources::factory = &factory;
		}

		template<class T>
		static T *get(std::string unique_name) {
#ifdef DEBUG_MODE
			if (!Resources::exist(unique_name)) {
				Log::warning("Cant find resource:" + unique_name);
			}
#endif
			return dynamic_cast<T *>(Resources::get(unique_name));
		}

		static bool exist(std::string &unique_name) {
			return Resources::registry.find(unique_name) != registry.end();
		}

		static void add(std::string &unique_name, Resource *res) {
			Resources::registry.emplace(unique_name, res);
		}

		static Resource *get(std::string &unique_name) {
#ifdef DEBUG_MODE
			if (!Resources::exist(unique_name)) {
				Log::warning("Cant find resource:" + unique_name);
			}
#endif
			return Resources::registry.at(unique_name);
		}


		static void destroyAll() {
			for (auto &name_res:Resources::registry) {
				delete name_res.second;
			}
			Resources::registry.clear();
		}

		static void destroy(std::string &unique_name) {
			delete get(unique_name);

			registry.erase(unique_name);
		}

		static void add(const std::string &unique_name, Resource *r) {
			registry.emplace(unique_name, r);
		}


		static Model *createModel(const ModelInfo &info, const std::string &name) {
			auto m = new Model(info);
			add(name, m);
			return m;
		}

		static Model *createModel(const ModelInfo &info) {

			return new Model(info);
		}

		static Mesh *createMesh(const MeshInfo &info, const std::string &name) {
			auto m = factory->createMesh(info);
			add(name, m);
			return m;
		}

		static Mesh *createMesh(const MeshInfo &info) {
			return factory->createMesh(info);
		}


		static Shader *createShader(const ShaderInfo &info, const std::string &name) {
			auto s = factory->createShader(info);
			add(name, s);
			return s;
		}

		static Shader *createShader(const ShaderInfo &info) {
			return factory->createShader(info);
		}

		static GraphicPipeline *createGraphicPipeline(const GraphicPipelineInfo &info, const std::string &name) {
			auto gp = factory->createGraphicPipeline(info);
			add(name, gp);
			return gp;

		}

		static GraphicPipeline *createGraphicPipeline(const GraphicPipelineInfo &info) {
			return factory->createGraphicPipeline(info);
		}

		static ComputePipeline *createComputePipeline(const ComputePipelineInfo &info, const std::string &name) {
			auto cp = factory->createComputePipeline(info);
			add(name, cp);
			return cp;
		}

		static ComputePipeline *createComputePipeline(const ComputePipelineInfo &info) {
			return factory->createComputePipeline(info);
		}

		static Texture *createTexture(const TextureInfo &info, const std::string &name) {
			auto t = factory->createTexture(info);
			add(name, t);
			return t;
		}

		static Texture *createTexture(const TextureInfo &info) {
			return factory->createTexture(info);
		}


		static RenderTarget *createRenderTarget(const RenderTargetInfo &info) {
			return factory->createRenderTarget(info);
		}

		static RenderTarget *createRenderTarget(const RenderTargetInfo &info, const std::string &name) {
			auto rt = factory->createRenderTarget(info);
			add(name, rt);
			return rt;
		}
	};


}