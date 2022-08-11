#pragma once

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
	struct ModelNode {
		Mesh *mesh = nullptr;
		GraphicPipeline *pipeline = nullptr;
		ModelMaterial material{};
	};

	class HB_API Model : public Resource {
		friend class Resources;

		std::vector<ModelNode> nodes;
		void load(const std::string &path);
		Model(const ModelInfo &info);
	public:
		~Model();
		std::vector<ModelNode> getNodes();
	};

}