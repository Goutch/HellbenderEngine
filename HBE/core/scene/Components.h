#pragma once


#include "Core.h"
#include "HBETypes.h"
#include <glm/detail/type_quat.hpp>
#include <core/resource/Font.h>
#include "string"
#include "Scene.h"

namespace HBE {
	class GraphicPipeline;

	class Mesh;

	class GraphicPipelineInstance;

	class RenderTarget;

	class Model;

	class SceneNode;

	class AudioClipInstance;

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
	private:
		mat4 world_mat = mat4(1.0f);
		mat4 local_mat = mat4(1.0f);
	public:
		Entity entity;

		bool is_dirty = false;

		const mat4 &local() const;

		void setLocal(const mat4 &local);

		const mat4 &world();

		void translate(vec3 translation);

		vec3 position() const;

		vec3 worldPosition();

		void setPosition(vec3 pos);

		quat rotation() const;

		void rotate(quat rot);

		void rotate(vec3 rot);

		void setRotation(quat rot);

		void setRotation(vec3 rot);

		void setLocalScale(vec3 s);

		vec3 localScale() const;

		void setWorldScale(vec3 s);

		vec3 worldScale();

		vec3 eulerRotation() const;

		quat worldRotation();

		vec3 worldEulerRotation();

		vec3 worldDown();

		vec3 worldUp();

		vec3 worldLeft();

		vec3 worldRight();

		vec3 worldBackward();

		vec3 worldForward();

		vec3 down() const;

		vec3 up() const;

		vec3 back() const;

		vec3 right() const;

		vec3 front() const;

		vec3 left() const;
	};

	struct HB_API Camera2D {
		Camera2D() {};

		Camera2D(const Camera2D &other);

		bool active = true;
		mat4 projection = mat4(1.0f);

		uint32_t layer_mask = UINT32_MAX;

		void setNearPlane(float near);

		void setFarPlane(float far);

		float getNearPlane();

		float getFarPlane();

		float getZoomRatio();

		void setZoomRatio(float ratio);

		float aspectRatio();

		void setRenderTarget(RenderTarget *render_target);

		RenderTarget *getRenderTarget();

	private:
		RenderTarget *render_target = nullptr;
		float zoom_ratio = 10.0f;
		float near = -1000;
		float far = 1000;

		void calculateProjection(RenderTarget *render_target);
	};

	struct HB_API PixelCamera {
		PixelCamera() {};

		PixelCamera(const PixelCamera &other);

		bool active = true;
		mat4 projection = mat4(1.0f);
		uint32_t layer_mask = UINT32_MAX;

		float aspectRatio();

		void setRenderTarget(RenderTarget *render_target);

		RenderTarget *getRenderTarget();

		void setNearPlane(float near);

		void setFarPlane(float far);

		float getNearPlane();

		float getFarPlane();

	private:
		RenderTarget *render_target = nullptr;
		float near = -1000;
		float far = 1000;

		void calculateProjection(RenderTarget *render_target);
	};

	struct HB_API Camera {
		Camera() {};

		Camera(const Camera &other);

		mat4 projection = mat4(1.0f);
		float fov = 70.0f;
		float far = 1000.0f;
		float near = 0.001f;
		bool active = true;
		uint32_t layer_mask = UINT32_MAX;

		void setRenderTarget(RenderTarget *render_target);

		RenderTarget *getRenderTarget();

		float aspectRatio();

		void setFOV(float fov);

		float getFOV();

		void setNearPlane(float near);

		void setFarPlane(float far);

		float getNearPlane();

		float getFarPlane();

	private:
		RenderTarget *render_target = nullptr;

		void calculateProjection(RenderTarget *render_target);
	};

	struct HB_API MeshRenderer {
		Mesh *mesh = nullptr;
		GraphicPipelineInstance *pipeline_instance = nullptr;
		uint32_t layer = 0;
		bool ordered = false;
		bool active = true;
	};

	struct HB_API ModelRenderer {
		Model *model = nullptr;
		uint32_t layer = 0;
		bool ordered = false;
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
