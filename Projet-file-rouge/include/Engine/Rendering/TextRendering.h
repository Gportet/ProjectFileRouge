#pragma once

#include <memory>
#include <string>
#include <cstdint>

#include "Engine/utils/Font.h"

class Pipeline;

class TextRendering
{
public:
    explicit TextRendering(std::shared_ptr<Font> font);
    void DrawText(Pipeline& pipeline, const std::string& text, float x, float y, uint32_t color);
    
private:
    int DrawGlyph(Pipeline& pipeline,FT_Face font, char character, float x, float y, uint32_t color);
    std::shared_ptr<Font> m_Font;
};
