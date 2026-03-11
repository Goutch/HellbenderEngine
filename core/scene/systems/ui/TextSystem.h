
#pragma once
#include "Core.h"
#include "string"
#include "core/scene/System.h"
#include "core/scene/Entity.h"
#include "HBETypes.h"
#include "core/resource/Mesh.h"
#include "core/scene/ecs/Component.h"
#include "Event.h"
#include "core/resource/Font.h"
#include "core/resource/PipelineInstance.h"
#include "core/resource/RasterizationPipeline.h"
#include "core/resource/Shader.h"

namespace HBE {
	class Font;

	class RasterizationPipeline;

	class RasterizationPipelineInstance;

	class Shader;

	class RasterizationTarget;

	class RenderGraph;

	struct  Node;


	struct HB_API TextRenderer {
		COMPONENT_IDS(TextComponent)
		friend class TextSystem;

		bool active = true;
		int layer = 0;
		float height = 40;
		PIVOT anchor = PIVOT_CENTER;
		TEXT_ALIGNMENT alignment = TEXT_ALIGNMENT_CENTER;

		Font *font;
		PipelineInstanceHandle pipeline_instance = HBE_NULL_HANDLE;

		const std::string &getText() const;

		void setText(const std::string &text);

		vec2 getSize() const;

	private :
		Mesh *mesh = nullptr;
		std::string text;
		vec2 size;
	};

	class HB_API TextSystem : public System {
		struct LabelPushConstant {
			mat4 world_matrix;
			float text_height;
		};
		event_subscription_id on_attach_subscription_id;
		event_subscription_id on_detach_subscription_id;
		event_subscription_id on_draw_subscription_id;
		const vec4 DEFAULT_TEXT_COLOR = vec4(1, 1, 1, 1);
		Font default_font;
		Shader default_text_frag_shader;
		Shader default_text_vert_shader;
		RasterizationPipeline default_text_pipeline;
		PipelineInstance default_text_pipeline_instance;
	public:
		TextSystem(Scene *scene, RasterizationTarget *render_target);

		~TextSystem() override;

		void onDetachLabel(Entity label);
		void onPrepareRenderGraph(RenderGraph* render_graph);

		void onAttachLabel(Entity text_entity);


	};
}

