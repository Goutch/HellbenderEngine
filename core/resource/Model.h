#pragma once

#include "Core.h"
#include "core/utility/Log.h"
#include "string"
#include "vector"
#include "map"
#include "Resource.h"
#include "core/resource/Texture.h"

namespace HBE {
	class GraphicPipelineInstance;

	class GraphicPipeline;

	class Mesh;

	class Texture;

	typedef uint32_t MODEL_FLAGS;

	enum MODEL_FLAG {
		MODEL_FLAG_NONE = 0,
		MODEL_FLAG_USED_IN_RAYTRACING = 1 << 2,
	};


	typedef uint32_t MODEL_VERTEX_ATTRIBUTE_TYPE_FLAGS;
	enum MODEL_VERTEX_ATTRIBUTE_TYPE_FLAG {
		MODEL_VERTEX_ATTRIBUTE_TYPE_FLAG_NONE = 0,
		MODEL_VERTEX_ATTRIBUTE_TYPE_FLAG_POSITION = 1 << 0,
		MODEL_VERTEX_ATTRIBUTE_TYPE_FLAG_NORMAL = 1 << 1,
		MODEL_VERTEX_ATTRIBUTE_TYPE_FLAG_TANGENT = 1 << 2,
		MODEL_VERTEX_ATTRIBUTE_TYPE_FLAG_TEXCOORD = 1 << 3,
		MODEL_VERTEX_ATTRIBUTE_TYPE_FLAG_COLOR = 1 << 4,
		MODEL_VERTEX_ATTRIBUTE_TYPE_FLAG_JOINTS = 1 << 5,
		MODEL_VERTEX_ATTRIBUTE_TYPE_FLAG_WEIGHTS = 1 << 6,
		MODEL_VERTEX_ATTRIBUTE_TYPE_FLAG_CUSTOM = 1 << 7,
	};

	enum MODEL_TEXTURE_TYPE {
		MODEL_TEXTURE_TYPE_ALBEDO,
		MODEL_TEXTURE_TYPE_METALLIC_ROUGHNESS,
		MODEL_TEXTURE_TYPE_EMMISIVE,
		MODEL_TEXTURE_TYPE_NORMAL,
		MODEL_TEXTURE_TYPE_OCCLUSION
	};

	struct MaterialProperty {
		vec4 base_color = vec4(1.0f);
		vec4 emmisive_factor = vec4(0.0f);
		float metal_factor = 0.0f;
		float roughness = 0.0f;
		uint32_t has_albedo = false;
		uint32_t has_metallic_roughness = false;
		uint32_t has_emmisive = false;
		uint32_t has_normal = false;
		uint32_t has_occlusion = false;
	};

	struct ModelVertexBuffer {
		MODEL_VERTEX_ATTRIBUTE_TYPE_FLAGS type = MODEL_VERTEX_ATTRIBUTE_TYPE_FLAG_NONE;
		const void *data = nullptr;
		size_t count = 0;
		size_t element_size = 0;
	};

	struct ModelMaterialData {
		MaterialProperty properties;
		bool double_sided = false;

		int albedo_texture = -1;
		int metallic_roughness_texture = -1;
		int emmisive_texture = -1;
		int normal_texture = -1;
		int occlusion_texture = -1;
		int custom_texture = -1;


		MODEL_VERTEX_ATTRIBUTE_TYPE_FLAGS used_with_attributes = MODEL_VERTEX_ATTRIBUTE_TYPE_FLAG_NONE;
	};

	struct ModelTextureData {
		std::string path;
		const void *data = nullptr;
		size_t width = 0;
		size_t height = 0;
		size_t channels = 0;
		IMAGE_FORMAT format = IMAGE_FORMAT_RGBA8;
		size_t bits_per_channels = 0;
		TextureSamplerInfo sampler_info = {};
	};

	struct ModelPrimitiveData {
		ModelVertexBuffer indices;
		std::map<MODEL_VERTEX_ATTRIBUTE_TYPE_FLAG, std::vector<ModelVertexBuffer>> buffers;
		MODEL_VERTEX_ATTRIBUTE_TYPE_FLAGS available_attributes = MODEL_VERTEX_ATTRIBUTE_TYPE_FLAG_NONE;
		vec3 position_min = vec3(0.0f);
		vec3 position_max = vec3(0.0f);
	};

	struct ModelMeshData {
		std::vector<ModelPrimitiveData> primitives;
	};

	struct ModelPrimitive {
		int material = -1;
	};

	//A node represent an object in the model hierarchy. with zero, one or multiple meshes to render.
	struct ModelNode {
		std::string name;
		mat4 transform = mat4(1.0f);
		int mesh = -1;
		std::vector<ModelPrimitive> primitives;
		std::vector<ModelNode> children;
	};
	struct ModelResources {
		std::vector<std::vector<Mesh *>> meshes;
		std::vector<Texture *> textures;
		std::vector<GraphicPipelineInstance *> materials;
	};

	struct ModelData {
		std::vector<ModelNode> nodes;
		std::vector<ModelMeshData> meshes;
		std::vector<ModelTextureData> textures_data;
		std::vector<ModelMaterialData> material_properties;
	};

	class HB_API ModelParser;

	struct ModelInfo {
		ModelParser *parser = nullptr;
		std::string path;
		MODEL_FLAGS flags = MODEL_FLAG_NONE;
	};

	class HB_API ModelParser {
	public:
		virtual Mesh *createMesh(const ModelPrimitiveData &data, ModelInfo info) = 0;

		virtual GraphicPipelineInstance *createMaterial(const ModelMaterialData &materialData, Texture **textures) = 0;
	};


	class HB_API Model : public Resource {
		friend class Resources;

		ModelInfo info;
		ModelData data;
		ModelResources resources;

		void load(const ModelInfo &info);

		Model(const ModelInfo &info);

	public:
		~Model();

		const ModelResources &getResources();

		void createMeshes();

		const std::vector<ModelNode> &getNodes();

		void createTextures();

		void createMaterials();
	};
}

