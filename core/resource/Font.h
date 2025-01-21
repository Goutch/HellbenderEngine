#pragma once

#include <string>
#include "Resource.h"
#include "Core.h"
#include "unordered_map"

namespace HBE {
    class Image;

    struct FontInfo {
        std::string path;
        const char *characters;
        uint32_t characters_count;
        uint32_t glyph_resolution = 32;
    };

    struct Glyph {
        vec2 size;
        vec2 uv_min;
        vec2 uv_max;
        vec2 offset;
    };

    class HB_API Font : public Resource {
        friend class Resources;

        std::unordered_map<char, Glyph> characters_glyphs;
        Image *atlas;
    private:
        Font(const FontInfo &info);

    public:
        ~Font();

        Image *getTextureAtlas();

        Glyph getCharacterGlyph(char character);

    };
}

