#pragma once

#include <dependencies/tinygltf/tiny_gltf.h>
#include "Core.h"
#include "core/resource/Mesh.h"
#include "core/utility/Log.h"
#include "core/resource/GraphicPipeline.h"
#include "string"

namespace HBE {
	typedef uint32_t MODEL_FLAGS;
	enum MODEL_FLAG {
		MODEL_FLAG_NONE = 0,
	};


	struct ModelInfo {
		std::string path;
		MODEL_FLAGS flags = MODEL_FLAG_NONE;
	};

	struct ModelMaterial {
		vec4 color = vec4(1.0f);
		Texture *texture = nullptr;
		GraphicPipeline *pipeline = nullptr;
	};

	struct ModelPrimitive {
		Mesh *mesh = nullptr;
		ModelMaterial material;
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
		std::vector<ModelMaterial> materials;
	};


	class HB_API Model : public Resource {
		friend class Resources;

		ModelData data;
		void load(const std::string &path);
		Model(const ModelInfo &info);
		void processNodes(tinygltf::Model &model, const tinygltf::Node &node, std::vector<ModelNode> &parent_node_array);
		void createMaterials(tinygltf::Model &model);
		void createMeshes(tinygltf::Model &model);
	public:
		~Model();
		const std::vector<ModelNode> &getNodes();

	};

}