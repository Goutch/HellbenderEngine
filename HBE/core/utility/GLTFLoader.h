#pragma once
#include "core/resource/Model.h"
namespace HBE{
	class HB_API GLTFLoader {
			public:
			static void load(const std::string& path, std::vector<Mesh>& meshes, std::vector<Material>& materials);
	};

}
