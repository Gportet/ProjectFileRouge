#include "Engine/Rendering/TextRendering.h"
#include "Engine/Pipeline.h"

TextRendering::TextRendering(std::shared_ptr<Font> font) : m_Font(std::move(font))
{}

int TextRendering::DrawGlyph(Pipeline& pipeline, FT_Face font, char character, float x, float y, uint32_t color)
{
    FT_Error error = FT_Load_Char(font, character, FT_LOAD_RENDER);
    if (error != 0)
        return 0;
    
    FT_GlyphSlot glyph = font->glyph;
    FT_Bitmap& bitmap = glyph->bitmap;
    
    const int startX = static_cast<int>(x) + glyph->bitmap_left;
    const int startY = static_cast<int>(y) - glyph->bitmap_top;
    
    const uint8_t r = (color >> 16) & 0xFF;
    const uint8_t g = (color >> 8)  & 0xFF;
    const uint8_t b = color & 0xFF;
    const uint8_t a = (color >> 24) & 0xFF;
    
    for (unsigned int rows = 0; rows < bitmap.rows; ++rows)
    {
        for (unsigned int col = 0; col < bitmap.width; ++col)
        {
            const unsigned char coverage = bitmap.buffer[rows * bitmap.width + col];
            if (coverage == 0)
                continue;
            
            pipeline.PutPixel(static_cast<int>(startX) + col, static_cast<int>(startY) + rows, r, g, b, a);
        }
    }
    return static_cast<int>(glyph->advance.x >> 6);
}

void TextRendering::DrawText(Pipeline& pipeline, const std::string& text, float x, float y, uint32_t color)
{
    FT_Face face = m_Font->getFont();
    float cursorX = x;
    
    for (char character : text)
    {
        const int advance = DrawGlyph(
            pipeline, face, character, cursorX, y, color
        );

        cursorX += advance;
    }
}


