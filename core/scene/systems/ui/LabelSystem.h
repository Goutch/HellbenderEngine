
#pragma once
#include "Core.h"
#include "string"
#include "core/scene/System.h"
#include "core/scene/Entity.h"
#include "HBETypes.h"
#include "core/resource/Mesh.h"

namespace HBE {
	class Font;

	class RasterizationPipeline;

	class RasterizationPipelineInstance;

	class Shader;

	class RasterizationTarget;

	class RenderGraph;

	class SceneNode;

	struct HB_API LabelComponent {
		friend class LabelSystem;

		bool active = true;
		int layer = 0;
		float height = 40;
		PIVOT anchor = PIVOT_CENTER;
		TEXT_ALIGNMENT alignment = TEXT_ALIGNMENT_CENTER;

		Font *font;
		RasterizationPipelineInstance *pipeline_instance = nullptr;

		const std::string &getText() const;

		void setText(const std::string &text);

		vec2 getSize() const;

	private :
		Mesh *mesh = nullptr;
		std::string text;
		vec2 size;
	};

	class HB_API LabelSystem : public System {
		struct LabelPushConstant {
			mat4 world_matrix;
			float text_height;
		};

		const vec4 DEFAULT_TEXT_COLOR = vec4(1, 1, 1, 1);
		Font *default_font = nullptr;
		Shader *default_text_frag_shader = nullptr;
		Shader *default_text_vert_shader = nullptr;
		RasterizationPipeline *default_text_pipeline = nullptr;
		RasterizationPipelineInstance *default_text_pipeline_instance = nullptr;
	public:
		LabelSystem(Scene *scene, RasterizationTarget *render_target);

		void drawSceneNode(RenderGraph *graph, SceneNode &node);

		~LabelSystem() override;

		void onDetachLabel(Entity label);

		void onAttachLabel(Entity label);
	};
}

