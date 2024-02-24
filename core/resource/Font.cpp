#include "Font.h"
#include "msdfgen.h"
#include "msdfgen-ext.h"
#include "Texture.h"
#include "core/utility/Log.h"
#include "msdf-atlas-gen/msdf-atlas-gen.h"
#include "Resources.h"

using namespace msdfgen;
using namespace msdf_atlas;
namespace HBE {
    const double pixelRange = 2.0;
    const double glyphScale = 32.0;
    const double miterLimit = 1.0;
    const double maxCornerAngle = 3.0;
    const int number_of_channels = 4;

    Font::Font(const FontInfo &info) {

        // Initialize instance of FreeType library
        if (msdfgen::FreetypeHandle *ft = msdfgen::initializeFreetype()) {
            // Load font file
            if (msdfgen::FontHandle *font = msdfgen::loadFont(ft, (RESOURCE_PATH + info.path).c_str())) {
                // Storage for glyph geometry and their coordinates in the atlas
                std::vector<GlyphGeometry> glyphs;
                // FontGeometry is a helper class that loads a set of glyphs from a single font.
                // It can also be used to get additional font metrics, kerning information, etc.
                FontGeometry fontGeometry(&glyphs);
                // Load a set of character glyphs:
                // The second argument can be ignored unless you mix different font sizes in one atlas.
                // In the last argument, you can specify a charset other than ASCII.
                // To load specific glyph indices, use loadGlyphs instead.
                //fontGeometry.loadCharset(font, 1.0, Charset::ASCII);
                Charset charset;
                for (int i = 0; i < info.characters_count; ++i) {
                    charset.add(info.characters[i]);
                }
                fontGeometry.loadCharset(font, 1.0, charset);
                // Apply MSDF edge coloring. See edge-coloring.h for other coloring strategies.
                const double maxCornerAngle = 3.0;
                for (GlyphGeometry &glyph: glyphs)
                    glyph.edgeColoring(&msdfgen::edgeColoringInkTrap, maxCornerAngle, 0);
                // TightAtlasPacker class computes the layout of the atlas.
                TightAtlasPacker packer;
                // Set atlas parameters:
                // setDimensions or setDimensionsConstraint to find the best value
                packer.setDimensionsConstraint(TightAtlasPacker::DimensionsConstraint::SQUARE);
                // setLocalScale for a fixed size or setMinimumScale to use the largest that fits
                packer.setMinimumScale(info.glyph_resolution);
                //packer.setDimensions(info.glyph_size,info.glyph_size);
                // setPixelRange or setUnitRange
                packer.setPixelRange(2.0);
                packer.setMiterLimit(1.0);

                packer.setPadding(3);
                // Compute atlas layout - pack glyphs
                packer.pack(glyphs.data(), glyphs.size());
                // Get final atlas dimensions
                int width = 0, height = 0;
                packer.getDimensions(width, height);
                // The ImmediateAtlasGenerator class facilitates the generation of the atlas bitmap.
                ImmediateAtlasGenerator<
                        float, // pixel type of buffer for individual glyphs depends on generator function
                        number_of_channels, // number of atlas color channels
                        mtsdfGenerator, // function to generate bitmaps for individual glyphs
                        BitmapAtlasStorage<float, number_of_channels> // class that stores the atlas bitmap
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
                msdfgen::BitmapConstRef<float, number_of_channels> storage = generator.atlasStorage();
                TextureInfo atlas_info{};
                atlas_info.width = width;
                atlas_info.height = height;
                atlas_info.format = IMAGE_FORMAT_RGBA32F;
                atlas_info.data = storage.pixels;


                atlas = Resources::createTexture(atlas_info);
                for (auto glyph: glyphs) {
                    msdf_atlas::unicode_t unicode = glyph.getCodepoint();
                    HBE::Glyph hbe_glyph{};

                    auto box_rect = glyph.getBoxRect();
                    auto bounds = glyph.getShape().getBounds();
                    hbe_glyph.uv_min = vec2(box_rect.x / static_cast<float>( width),
                                            box_rect.y / static_cast<float>(height));
                    hbe_glyph.uv_max = vec2((box_rect.x + box_rect.w) / static_cast<float>(width),
                                            (box_rect.y + box_rect.h) / static_cast<float>(height));
                    hbe_glyph.size = vec2(box_rect.w / static_cast<float>(info.glyph_resolution),
                                          box_rect.h / static_cast<float>(info.glyph_resolution));
                    hbe_glyph.offset = vec2(glyph.getBoxTranslate().x / static_cast<float>(info.glyph_resolution),
                                            glyph.getBoxTranslate().y / static_cast<float>(info.glyph_resolution));

                    double scale = glyph.getBoxScale();
                    hbe_glyph.offset*=scale;
                    characters_glyphs.emplace(unicode, hbe_glyph);
                }

                // Cleanup
                msdfgen::destroyFont(font);
            }
            msdfgen::deinitializeFreetype(ft);
        }
    }

    Font::~Font() {
        delete atlas;
    }

    Texture *Font::getTextureAtlas() {
        return atlas;
    }

    Glyph Font::getCharacterGlyph(char character) {
        HB_ASSERT(characters_glyphs.find(character) != characters_glyphs.end(), "Character not found in font");
        return characters_glyphs[character];
    }
}
