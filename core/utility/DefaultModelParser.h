
#pragma once

#include "core/resource/Mesh.h"
#include "core/resource/Model.h"
#include "core/resource/raytracing/AccelerationStructure.h"

namespace HBE {
	struct DefaultModelParserInfo {
		MESH_FLAG additional_mesh_flags = MESH_FLAG_NONE;
		std::map<MODEL_TEXTURE_TYPE, std::string> texture_names;
		std::string material_property_name;
		RaterizationPipeline *graphic_pipeline = nullptr;
		RaterizationPipeline *graphic_pipeline_2_sided = nullptr;
	};

	class HB_API DefaultModelParser : public HBE::ModelParser {
	public:
		DefaultModelParserInfo parser_info;
	public:
		void onStartParsingModel(Model *model) override {};

		void onEndParsingModel(Model *model) override {};

		DefaultModelParser(DefaultModelParserInfo info);

		Mesh *createMesh(const HBE::ModelPrimitiveData &data, ModelInfo model_info) override;

		RasterizationPipelineInstance *createMaterial(const HBE::ModelMaterialData &materialData, HBE::Texture **textures) override;

		Texture *createTexture(const ModelTextureData &data) override;

		MeshAccelerationStructure *createMeshAccelerationStructure(Mesh *mesh, int mesh_index) override;
	};
}

