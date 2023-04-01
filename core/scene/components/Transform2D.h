#pragma once

#include "Core.h"
#include "HBETypes.h"

namespace HBE {
	struct HB_API Transform2D {
	private:
		mat3 local = mat3(1.0f);
		Transform2D *parent = nullptr;
	public:
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


}