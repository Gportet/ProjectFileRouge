#pragma once

#include <string>

#include "MathLib_SIMD/include/Vec3_SIMD.h"
#include "MathLib_SIMD/include/Vec4_SIMD.h"

#include "Engine/Ressources/Texture.h"


class MaterialTag final
{
private :
    std::string name;

public:
    explicit MaterialTag(std::string name = "") : name(std::move(name)) {}
    
    [[nodiscard]] std::string getName() const { return name; };
    
    bool operator==(const MaterialTag& tag) const { return (name == tag.name); }
    bool operator!=(const MaterialTag& tag) const { return (name != tag.name); }
    bool operator<(const MaterialTag& tag) const { return (name < tag.name); }
    
    [[nodiscard]] bool contains(const MaterialTag& tag )const;
};

class Material
{
private:
    const MaterialTag tag;
    std::shared_ptr<Texture> texture = nullptr;
    
    MathsSIMD::Vec4_SIMD ambient, diffuse, specular;
    uint16_t illum = 1;
    double opacity = 1.0;

    
public:
    Material(const MaterialTag& _tag, 
        std::shared_ptr<Texture> _texture, 
        const MathsSIMD::Vec4_SIMD& _ambient = _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f),
        const MathsSIMD::Vec4_SIMD& _diffuse = _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f),
        const MathsSIMD::Vec4_SIMD& _specular = _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f),
        uint16_t _illum = 1,
        double _opacity = 1.0);
    
    void setAmbient(const MathsSIMD::Vec4_SIMD& _color);
    
    [[nodiscard]] std::shared_ptr<Texture> getTexture() const { return texture; }
    [[nodiscard]] MaterialTag getTag() const { return tag; }
};
