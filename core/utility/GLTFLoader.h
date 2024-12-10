
#pragma once

#include "core/resource/Model.h"
#include "tiny_gltf.h"

namespace HBE {
	class GLTFLoader {
		tinygltf::TinyGLTF loader;
		tinygltf::Model model;
	public:

		void load(ModelData &data, const ModelInfo &info);

		~GLTFLoader();
	};
}

