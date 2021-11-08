#pragma once


#include "Core.h"
#include "HBETypes.h"
#include "core/resource/Mesh.h"
#include "core/resource/GraphicPipeline.h"
#include "core/resource/RenderTarget.h"
#include <glm/detail/type_quat.hpp>

namespace HBE {
	struct HB_API Identity {
		std::string name = "";
	};

	struct HB_API Transform {
		mat4 local = mat4(1.0f);
		Transform *parent = nullptr;
		mat4 world();
		void translate(vec3 translation);
		vec3 position();

		void setPosition(vec3 pos);
		quat rotation();
		void rotate(quat rot);
		void rotate(vec3 rot);
		void setRotation(quat rot);
		void setRotation(vec3 rot);
		void setScale(vec3 s);
		vec3 scale();
		vec3 eulerRotation();
		quat worldRotation();
		vec3 worldEulerRotation();
		vec3 worldDown();
		vec3 worldUp();
		vec3 worldLeft();
		vec3 worldRight();
		vec3 worldBackward();
		vec3 worldForward();
		vec3 down();
		vec3 up();
		vec3 back();
		vec3 right();
		vec3 front();
		vec3 left();
	};

	struct HB_API Camera {
		mat4 projection = mat4(1.0f);
		float fov = 90.0f;
		float render_distance = 500.0f;
		float near = 0.1f;
		bool active = true;
		RenderTarget *render_target = nullptr;
		void calculateProjection();
		float aspectRatio();
	};

	struct HB_API MeshRenderer {
		Mesh *mesh = nullptr;
		GraphicPipeline *pipeline = nullptr;
		bool active = true;
	};

	struct CameraController {
		float speed = 1.0f;
		float sensibility = 1.0f;
		float current_pitch = 0.0f;
		float max_pitch = 90.0f;
		bool active = true;
	};
}
