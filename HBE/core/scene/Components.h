#pragma once


#include "Core.h"
#include "HBETypes.h"
#include <glm/detail/type_quat.hpp>
#include "string"

namespace HBE {
	class GraphicPipeline;

	class Mesh;

	class Material;

	class RenderTarget;

	class Model;

	struct HB_API Identity {
		std::string name = "Default";
	};

	struct HB_API Transform2D {
		mat3 local = mat3(1.0f);
		Transform2D *parent;
		mat3 world() const;
		vec2 worldPosition() const;
		void translate(vec2 translation);
		vec2 position();
		void setPosition(vec2 pos);
		float rotation() const;
		void rotate(float rotation);
		void setRotaton(float rotation);
		float worldRotation() const;
		void setScale(vec2 s);
		vec2 scale() const;
	};

	struct HB_API Transform {
		mat4 local = mat4(1.0f);
		Transform *parent = nullptr;
		mat4 world() const;
		void translate(vec3 translation);
		vec3 position() const;
		vec3 worldPosition() const;
		void setPosition(vec3 pos);
		quat rotation() const;
		void rotate(quat rot);
		void rotate(vec3 rot);
		void setRotation(quat rot);
		void setRotation(vec3 rot);
		void setScale(vec3 s);
		vec3 scale() const;
		vec3 eulerRotation() const;
		quat worldRotation() const;
		vec3 worldEulerRotation() const;
		vec3 worldDown() const;
		vec3 worldUp() const;
		vec3 worldLeft() const;
		vec3 worldRight() const;
		vec3 worldBackward() const;
		vec3 worldForward() const;
		vec3 down() const;
		vec3 up() const;
		vec3 back() const;
		vec3 right() const;
		vec3 front() const;
		vec3 left() const;
	};

	struct HB_API Camera2D {
		bool active = true;
		mat4 projection = mat3(1.0f);
		float zoom_ratio = 10;
		RenderTarget *render_target = nullptr;
		void calculateProjection();
		float aspectRatio();
	};

	struct HB_API Camera {
		mat4 projection = mat4(1.0f);
		float fov = 70.0f;
		float render_distance = 100.0f;
		float near = 0.001f;
		bool active = true;
		RenderTarget *render_target = nullptr;
		void calculateProjection();
		float aspectRatio();
	};

	struct HB_API MeshRenderer {
		Mesh *mesh = nullptr;
		Material *material = nullptr;
		bool active = true;
	};

	struct HB_API ModelRenderer {
		Model *model = nullptr;
		bool active = true;
	};

	struct HB_API CameraController {
		float speed = 1.0f;
		float sensibility = 1.0f;
		float current_pitch = 0.0f;
		float max_pitch = 90.0f;
		bool active = true;
	};

}
