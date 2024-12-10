
#pragma once

#include "core/resource/Model.h"

namespace HBE {
	struct DefaultModelParserInfo {
		GraphicPipeline *graphic_pipeline = nullptr;
		GraphicPipeline *graphic_pipeline_2_sided = nullptr;
		std::map<MODEL_TEXTURE_TYPE, std::string> texture_names;
		std::string material_property_name;
	};

	class HB_API DefaultModelParser : public HBE::ModelParser {
	public:
		DefaultModelParserInfo parser_info;
	public:

		DefaultModelParser(DefaultModelParserInfo info);

		Mesh *createMesh(const HBE::ModelPrimitiveData &data, ModelInfo model_info) override;

		GraphicPipelineInstance *createMaterial(const HBE::ModelMaterialData &materialData, HBE::Texture **textures) override;
	};
}

