#pragma once

#include "Core.h"
#include "core/utility/Log.h"
#include "string"
#include "vector"
#include "Resource.h"

namespace HBE {
	class GraphicPipelineInstance;

	class GraphicPipeline;

	class Mesh;

	class Texture;

	typedef uint32_t MODEL_FLAGS;

	enum MODEL_FLAG {
		MODEL_FLAG_NONE = 0,
		MODEL_FLAG_DONT_LOAD_TEXTURES = 1 << 0,
		MODEL_FLAG_DONT_LOAD_MATERIALS = 1 << 1,
		MODEL_FLAG_USED_IN_RAYTRACING = 1 << 2,
	};

	struct ModelInfo {
		std::string path;
		MODEL_FLAGS flags = MODEL_FLAG_NONE;
	};

	struct ModelMaterialProperties {
		vec4 color = vec4(1.0f);
		Texture *texture = nullptr;
	};

	struct ModelPrimitive {
		Mesh *mesh = nullptr;
		GraphicPipelineInstance *material = nullptr;
	};

	//A node represent an object in the model hierarchy. with zero, one or multiple meshes to render.
	struct ModelNode {
		mat4 transform;
		std::vector<ModelPrimitive> primitives;
		std::vector<ModelNode> children;
	};

	struct ModelData {
		std::vector<ModelNode> nodes;
		std::vector<std::vector<Mesh *>> meshes;
		std::vector<GraphicPipelineInstance *> materials;
		std::vector<ModelMaterialProperties> material_properties;
	};


	class HB_API Model : public Resource {
		friend class Resources;

		ModelData data;
		void load(const ModelInfo &info);
		Model(const ModelInfo &info);


	public:
		~Model();
		const std::vector<ModelNode> &getNodes();

	};

}