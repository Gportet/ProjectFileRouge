#include "Engine/Ressources/Material.h"


bool MaterialTag::contains(const MaterialTag& tag) const
{
    if (name.find(tag.getName()) != std::string::npos)
    {
        return true;
    }
    return false;
}

Material::Material(const MaterialTag& _tag, 
    std::shared_ptr<Texture> _texture, 
    const MathsSIMD::Vec4_SIMD& _ambient, 
    const MathsSIMD::Vec4_SIMD& _diffuse, 
    const MathsSIMD::Vec4_SIMD& _specular, 
    uint16_t _illum, double _opacity) 
: tag(_tag), texture(_texture), ambient(_ambient), diffuse(_diffuse), specular(_specular), illum(_illum), opacity(_opacity)
{
}

void Material::setAmbient(const MathsSIMD::Vec4_SIMD& _color)
{
    ambient = _color;
}

