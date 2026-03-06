
#pragma once

#include "core/interface/RasterizationPipelineInterface.h"
#include "core/resource/Mesh.h"
#include "core/resource/Model.h"
#include "core/resource/raytracing/AccelerationStructure.h"

namespace HBE {
	struct DefaultModelParserInfo {
		MESH_FLAG additional_mesh_flags = MESH_FLAG_NONE;
		std::map<MODEL_TEXTURE_TYPE, std::string> texture_names;
		std::string material_property_name;
		RasterizationPipelineHandle graphic_pipeline = HBE_NULL_HANDLE;
		RasterizationPipelineHandle graphic_pipeline_2_sided = HBE_NULL_HANDLE;
	};

	class HB_API DefaultModelParser : public HBE::ModelParser {
		Context& context;
	public:
		DefaultModelParserInfo parser_info;
	public:
		void onStartParsingModel(Model *model) override {};

		void onEndParsingModel(Model *model) override {};

		DefaultModelParser(DefaultModelParserInfo info);

		MeshHandle createMesh(const HBE::ModelPrimitiveData &data, ModelInfo model_info) override;

		PipelineInstanceHandle *createMaterial(const HBE::ModelMaterialData &materialData, HBE::ImageHandle *textures) override;

		ImageHandle createTexture(const ModelTextureData &data) override;

		MeshAccelerationStructure *createMeshAccelerationStructure(Mesh *mesh, int mesh_index) override;
	};
}

