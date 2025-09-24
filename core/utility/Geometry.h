#pragma once

#include "Core.h"
#include "core/resource/Mesh.h"

namespace HBE
{
	typedef uint32_t VERTEX_FLAGS;

	enum VERTEX_FLAG
	{
		VERTEX_FLAG_NONE = 0,
		VERTEX_FLAG_UV = 1,
		VERTEX_FLAG_NORMAL = 2,
	};

	class Font;

	class HB_API Geometry
	{
	public:
		static void createQuad(Mesh& mesh,
		                       float size_x,
		                       float size_y,
		                       VERTEX_FLAGS flags,
		                       PIVOT pivot = PIVOT_CENTER);

		static void createCube(Mesh& mesh,
		                       float size_x,
		                       float size_y,
		                       float size_z,
		                       VERTEX_FLAGS flags);
		static void createRoundedRectTriangleFan(Mesh& mesh,
		                                         float size_x,
		                                         float size_y,
		                                         float radius,
		                                         uint32_t subdivision_count,
		                                         VERTEX_FLAGS flags,
		                                         PIVOT pivot);
		static void createRoundedRectOutlineTriangleStrip(Mesh& mesh,
		                                                  float size_x,
		                                                  float size_y,
		                                                  float radius,
		                                                  float width,
		                                                  uint32_t subdivision_count,
		                                                  VERTEX_FLAGS flags,
		                                                  PIVOT pivot);
		static Mesh* createText(
			const std::string& text,
			Font& font,
			float line_height,
			float space_width,
			TEXT_ALIGNMENT alignment,
			PIVOT pivot,
			float& total_width,
			float& total_height);
		static void updateText(Mesh& mesh,
		                       const std::string& text,
		                       Font& font,
		                       float line_height,
		                       float space_width,
		                       TEXT_ALIGNMENT alignment,
		                       PIVOT pivot,
		                       float& total_width,
		                       float& total_height);

	private:
		static void getRoundedRectanglePoints(std::vector<vec2>& points,
		                                      float size_x,
		                                      float size_y,
		                                      float radius,
		                                      uint32_t subdivision_count,
		                                      VERTEX_FLAGS flags,
		                                      PIVOT pivot);
	};
}
