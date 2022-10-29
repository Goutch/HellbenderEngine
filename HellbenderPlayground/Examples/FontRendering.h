#pragma once

#include "HBE.h"
#include "msdf-atlas-gen/msdf-atlas-gen.h"
using namespace msdf_atlas;
using namespace HBE;
bool generateAtlas(const char *fontFilename) {
	bool success = false;
	// Initialize instance of FreeType library
	if (msdfgen::FreetypeHandle *ft = msdfgen::initializeFreetype()) {
		// Load font file
		if (msdfgen::FontHandle *font = msdfgen::loadFont(ft, fontFilename)) {
			// Storage for glyph geometry and their coordinates in the atlas
			std::vector<GlyphGeometry> glyphs;
			// FontGeometry is a helper class that loads a set of glyphs from a single font.
			// It can also be used to get additional font metrics, kerning information, etc.
			FontGeometry fontGeometry(&glyphs);
			// Load a set of character glyphs:
			// The second argument can be ignored unless you mix different font sizes in one atlas.
			// In the last argument, you can specify a charset other than ASCII.
			// To load specific glyph indices, use loadGlyphs instead.
			fontGeometry.loadCharset(font, 1.0, Charset::ASCII);
			// Apply MSDF edge coloring. See edge-coloring.h for other coloring strategies.
			const double maxCornerAngle = 3.0;
			for (GlyphGeometry &glyph : glyphs)
				glyph.edgeColoring(&msdfgen::edgeColoringInkTrap, maxCornerAngle, 0);
			// TightAtlasPacker class computes the layout of the atlas.
			TightAtlasPacker packer;
			// Set atlas parameters:
			// setDimensions or setDimensionsConstraint to find the best value
			packer.setDimensionsConstraint(TightAtlasPacker::DimensionsConstraint::SQUARE);
			// setScale for a fixed size or setMinimumScale to use the largest that fits
			packer.setMinimumScale(24.0);
			// setPixelRange or setUnitRange
			packer.setPixelRange(2.0);
			packer.setMiterLimit(1.0);
			// Compute atlas layout - pack glyphs
			packer.pack(glyphs.data(), glyphs.size());
			// Get final atlas dimensions
			int width = 0, height = 0;
			packer.getDimensions(width, height);
			// The ImmediateAtlasGenerator class facilitates the generation of the atlas bitmap.
			ImmediateAtlasGenerator<
					float, // pixel type of buffer for individual glyphs depends on generator function
					3, // number of atlas color channels
					msdfGenerator, // function to generate bitmaps for individual glyphs
					BitmapAtlasStorage<byte, 3> // class that stores the atlas bitmap
					// For example, a custom atlas storage class that stores it in VRAM can be used.
			> generator(width, height);
			// GeneratorAttributes can be modified to change the generator's default settings.
			GeneratorAttributes attributes;
			generator.setAttributes(attributes);
			generator.setThreadCount(4);
			// Generate atlas bitmap
			generator.generate(glyphs.data(), glyphs.size());
			// The atlas bitmap can now be retrieved via atlasStorage as a BitmapConstRef.
			// The glyphs array (or fontGeometry) contains positioning data for typesetting text.
			//success = myProject::submitAtlasBitmapAndLayout(generator.atlasStorage(), glyphs);
			// Cleanup
			msdfgen::destroyFont(font);
		}
		msdfgen::deinitializeFreetype(ft);
	}
	return success;
}
class FontRenderingScene {
public:


	FontRenderingScene() {

		Texture *font_atlas;
		font_atlas = Texture::load("fonts/OpenSans-mtsdf.png", IMAGE_FORMAT_RGBA8, IMAGE_FLAG_NONE);

		std::vector<VertexAttributeInfo> attributes;
		attributes.emplace_back();
		attributes[0].location = 0;
		attributes[0].size = sizeof(vec3)+sizeof(vec2);
		attributes[0].flags = VERTEX_ATTRIBUTE_FLAG_NONE;

		Mesh *text_mesh;
		MeshInfo mesh_info{};
		mesh_info.flags = MESH_FLAG_NONE;
		mesh_info.attribute_infos = attributes.data();
		mesh_info.attribute_info_count = attributes.size();
		text_mesh = Resources::createMesh(mesh_info, "mesh");
		Geometry::createQuad(*text_mesh, 1, 1, VERTEX_FLAG_UV);

		Shader *text_vertex_shader;
		Shader *text_fragment_shader;
		ShaderInfo vertex_shader_info{};
		ShaderInfo fragment_shader_info{};
		vertex_shader_info.stage = SHADER_STAGE_VERTEX;
		fragment_shader_info.stage = SHADER_STAGE_FRAGMENT;
		vertex_shader_info.path = "shaders/defaults/textMSDF.vert";
		fragment_shader_info.path = "shaders/defaults/textMSDF.frag";
		text_vertex_shader = Resources::createShader(vertex_shader_info, "vertex");
		text_fragment_shader = Resources::createShader(fragment_shader_info, "fragment");

		GraphicPipeline *text_pipeline;
		GraphicPipelineInfo pipeline_info{};
		pipeline_info.attribute_infos = attributes.data();
		pipeline_info.attribute_info_count = attributes.size();
		pipeline_info.vertex_shader = text_vertex_shader;
		pipeline_info.fragement_shader = text_fragment_shader;
		pipeline_info.topology = VERTEX_TOPOLOGY_TRIANGLE;
		pipeline_info.flags = GRAPHIC_PIPELINE_FLAG_NONE;
		text_pipeline = Resources::createGraphicPipeline(pipeline_info, "pipeline");

		GraphicPipelineInstance *text_pipeline_instance;
		GraphicPipelineInstanceInfo pipeline_instance_info{};
		pipeline_instance_info.graphic_pipeline = text_pipeline;
		pipeline_instance_info.flags = GRAPHIC_PIPELINE_INSTANCE_FLAG_NONE;
		text_pipeline_instance = Resources::createGraphicPipelineInstance(pipeline_instance_info, "pipeline_instance");

		text_pipeline_instance->setTexture("msdf", font_atlas);

		Scene *scene = Application::getScene();

		Entity camera_entity = scene->createEntity();

		Camera2D camera2D = camera_entity.attach<Camera2D>();

		scene->setCameraEntity(camera_entity);

		Entity text_entity = scene->createEntity();

		MeshRenderer &text_mesh_renderer = text_entity.attach<MeshRenderer>();
		text_mesh_renderer.mesh = text_mesh;
		text_mesh_renderer.pipelineInstance = text_pipeline_instance;

		generateAtlas(std::string(RESOURCE_PATH+std::string("font/OpenSans-Regular.ttf")).c_str());
	}

};