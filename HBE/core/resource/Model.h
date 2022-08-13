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
	};

	struct ModelPrimitive {
		Mesh *mesh = nullptr;
		ModelMaterial material;
		GraphicPipeline *pipeline = nullptr;
	};

	//A node represent an object in the model hierarchy. with zero, one or multiple meshes to render.
	struct ModelNode {
		mat4 transform;
		std::vector<ModelPrimitive> primitives;
		std::vector<ModelNode *> children;

		~ModelNode() {
			for (int i = 0; i < children.size(); ++i) {
				delete children[i];
			}
		}
	};

	struct ModelData {
		std::vector<ModelNode *> nodes;
		std::unordered_map<int, std::vector<Mesh *>> meshes;
		std::unordered_map<int, std::vector<ModelMaterial>> materials;
		std::unordered_map<int, std::vector<GraphicPipeline *>> pipelines;
	};


	class HB_API Model : public Resource {
		friend class Resources;

		ModelData data;
		void load(const std::string &path);
		Model(const ModelInfo &info);
		void processNodes(tinygltf::Model &model, const tinygltf::Node &node, std::vector<ModelNode *> &parent_node_array);
	public:
		~Model();
		std::vector<ModelNode *> getNodes();
	};

}