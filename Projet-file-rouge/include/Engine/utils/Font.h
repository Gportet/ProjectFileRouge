#pragma once

#include <string>
#include <map>

#include <Engine/utils/Filepath.h>
#include <ft2build.h>
#include FT_FREETYPE_H

class FontTag final
{
private:
    const std::string _name;
public:
    explicit FontTag(const std::string& name = "") : _name(name) {}
    FontTag(const FontTag& other) : _name(other._name) {}
    
    [[nodiscard]] std::string str() const { return _name; }
    [[nodiscard]] bool empty() const { return _name.empty(); }
    
    bool operator==(const FontTag &tag) const { return _name == tag._name; }
    bool operator!=(const FontTag &tag) const { return _name != tag._name; }
    bool operator<(const FontTag &tag) const { return _name < tag._name; }
    
    [[nodiscard]] bool contains(const std::string& str) const;
};


class Font
{
private:
    const FontTag _tag;
    const Filepath _fileName;
    FT_Library _ft = nullptr;
    
    std::map<uint16_t, FT_Face> fonts;
public:
    Font(const FontTag& tag, const Filepath& fileName);
    FT_Face getFont(uint16_t fontSize=14);
    
    [[nodiscard]] std::pair<int, int> getTextSize(const std::string& text, uint16_t fontSize=14);
    
    ~Font();
    
};