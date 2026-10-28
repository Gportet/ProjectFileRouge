#include "Engine/Ressources/Material.h"


bool MaterialTag::contains(const MaterialTag& tag) const
{
    return name.find(tag.getName()) != std::string::npos;
}

Material::Material(const MaterialTag& _tag, 
    std::shared_ptr<Texture> _texture, 
    const MathsSIMD::Vec4_SIMD& _ambient, 
    const MathsSIMD::Vec4_SIMD& _diffuse, 
    const MathsSIMD::Vec4_SIMD& _specular, 
    uint16_t _illum, float _opacity) 
: tag(_tag), texture(_texture), ambient(_ambient), diffuse(_diffuse), specular(_specular), illum(_illum), opacity(_opacity)
{
}

void Material::setAmbient(const MathsSIMD::Vec4_SIMD& _color)
{
    ambient = _color;
}

void Material::setDiffuse(const MathsSIMD::Vec4_SIMD& _color)
{
    diffuse = _color;
}


void Material::setSpecular(const MathsSIMD::Vec4_SIMD& _color)
{
    specular = _color;
}

void Material::setIllum(const uint16_t _illum)
{
    illum = _illum;
}

void Material::setOpacity(const float _opacity)
{
    opacity = _opacity;
}

void Material::setShininess(const float _shininess)
{
    shininess = _shininess;
}

void Material::setTexture(std::shared_ptr<Texture> _texture)
{
    texture = _texture;
}
