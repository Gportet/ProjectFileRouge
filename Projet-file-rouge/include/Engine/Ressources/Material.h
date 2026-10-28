#pragma once

#include <string>
#include <memory>
#include <utility>
#include <cstdint>
#include <xmmintrin.h>

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
    float opacity = 1.0;
    float shininess = 32.0f;

    
public:
    Material(const MaterialTag& _tag, 
        std::shared_ptr<Texture> _texture, 
        const MathsSIMD::Vec4_SIMD& _ambient =  MathsSIMD::Vec4_SIMD(_mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f)),
        const MathsSIMD::Vec4_SIMD& _diffuse = MathsSIMD::Vec4_SIMD(_mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f)),
        const MathsSIMD::Vec4_SIMD& _specular = MathsSIMD::Vec4_SIMD(_mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f)),
        uint16_t _illum = 1,
        float _opacity = 1.0);
    
    void setAmbient(const MathsSIMD::Vec4_SIMD& _color);
    void setDiffuse(const MathsSIMD::Vec4_SIMD& _color);
    void setSpecular(const MathsSIMD::Vec4_SIMD& _color);
    void setIllum(const uint16_t _illum);
    void setOpacity(const float _opacity);
    void setShininess(const float _shininess);
    void setTexture(std::shared_ptr<Texture> _texture);
    
    [[nodiscard]] std::shared_ptr<Texture> getTexture() const { return texture; }
    [[nodiscard]] const MaterialTag& getTag() const { return tag; }
    
    [[nodiscard]] const MathsSIMD::Vec4_SIMD& getAmbient() const { return ambient; };
    [[nodiscard]] const MathsSIMD::Vec4_SIMD& getDiffuse() const { return diffuse; };
    [[nodiscard]] const MathsSIMD::Vec4_SIMD& getSpecular() const { return specular; };

    [[nodiscard]] uint16_t getIllum() const { return illum; };
    [[nodiscard]] float getOpacity() const { return opacity; };
    [[nodiscard]] float getShininess() const { return shininess; };
};
