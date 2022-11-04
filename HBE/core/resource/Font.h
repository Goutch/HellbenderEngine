#pragma once

#include <string>
#include "Resource.h"
#include "Core.h"
#include "unordered_map"

namespace HBE{
	class Texture;
	struct FontInfo
	{
		std::string path;
		char* characters;
		uint32_t characters_count;
		uint32_t glyph_size=32;
	};

	struct Glyph{
		vec2 size;
		vec2 uv_min;
		vec2 uv_max;
	};
	class HB_API Font :public Resource{
		friend class Resources;
		std::unordered_map<char,Glyph> characters_glyphs;
		Texture* atlas;
	private:
		Font(const FontInfo &info);
	public:
		~Font();
		Texture* getTextureAtlas();
		Glyph getCharacterGlyph(char character);

	};
}

