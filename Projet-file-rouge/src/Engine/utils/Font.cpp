#include "Engine/utils/Font.h"
#include <iostream>

bool FontTag::contains(const std::string& str) const
{
    if (_name.find(str) != std::string::npos)
    {
        return true;
    }
    return false;
}

Font::Font(const FontTag& tag, const Filepath& fileName) : _tag(tag), _fileName(fileName)
{
    FT_Error error = FT_Init_FreeType(&_ft);

    if (error != 0) {
        _ft = nullptr;
        throw std::runtime_error("Impossible d'initialiser FreeType");
    }
}


FT_Face Font::getFont(uint16_t fontSize)
{
    // Vérifier si la police est déjà chargée
    auto it = fonts.find(fontSize);
    if (it != fonts.end())
    {
        return it->second;
    }

    // Charger la face du fichier de police
    FT_Face font = nullptr;

    FT_Error error = FT_New_Face(_ft,_fileName.str().c_str(),0,&font);

    // Confirmer que le chargement a réussi
    if (error != 0)
    {
        throw std::runtime_error("Impossible d'initialiser FreeType");
    }

    // Afficher le fichier chargé avec succès
    std::cout << "Police chargee avec succes : "
              << _fileName.str() << '\n';

    
    // Définir la taille de la police
    error = FT_Set_Pixel_Sizes(font, 0, fontSize);

    if (error != 0)
    {
        FT_Done_Face(font);
        throw std::runtime_error("Impossible d'initialiser FreeType");
    }

    // Stocker la face dans le cache
    fonts.insert({fontSize, font});

    return font;
}


std::pair<int, int> Font::getTextSize(const std::string& text,uint16_t fontSize)
{
    FT_Face font = getFont(fontSize);

    if (!font)
        return {0, 0};

    int width = 0;

    for (unsigned char c : text)
    {
        if (FT_Load_Char(font, c, FT_LOAD_DEFAULT) == 0)
        {
            width += font->glyph->advance.x >> 6;
        }
    }

    int height = static_cast<int>(font->size->metrics.height >> 6);

    return {width, height};
}

Font::~Font()
{
    // Libérer toutes les faces chargées
    for (auto& [fontSize, face] : fonts)
    {
        if (face)
        {
            FT_Done_Face(face);
        }
    }

    fonts.clear();

    // Libérer la bibliothèque FreeType
    if (_ft)
    {
        FT_Done_FreeType(_ft);
        _ft = nullptr;
    }
}

