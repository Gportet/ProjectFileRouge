
#include "AssetEngine/Serializer/MaterialSerializer.h"

#include "Engine/Ressources/Material.h"
#include "AssetEngine/Binary/BinaryWriter.h"
#include "AssetEngine/Binary/BinaryReader.h"

#include "MathLib/MathLib_SIMD/Vec4_SIMD.h"

#include <xmmintrin.h>
#include <stdexcept>

namespace
{
    // Écrit les 4 composantes d'un Vec4_SIMD
    void WriteVec4(BinaryWriter& writer, const MathsSIMD::Vec4_SIMD& value)
    {
        float components[4];

        _mm_storeu_ps(components, value.data);

        for (float component : components)
        {
            writer.Write(component);
        }
    }

    // Reconstruit un Vec4_SIMD depuis 4 floats
    MathsSIMD::Vec4_SIMD ReadVec4(BinaryReader& reader)
    {
        const float x = reader.ReadFloat();
        const float y = reader.ReadFloat();
        const float z = reader.ReadFloat();
        const float w = reader.ReadFloat();

        return MathsSIMD::Vec4_SIMD(_mm_setr_ps(x, y, z, w));
    }
}

Bytes MaterialSerializer::SerializeMaterial(const Material& material)
{
    // Pour l'instant, les références vers les textures ne sont pas encore gérées.
    if (material.getTexture() != nullptr)
    {
        throw std::runtime_error(
            "MaterialSerializer: texture references are not supported yet."
        );
    }

    BinaryWriter writer;

    // Nom du matériau
    writer.Write(material.getTag().getName());

    // Couleurs
    WriteVec4(writer, material.getAmbient());
    WriteVec4(writer, material.getDiffuse());
    WriteVec4(writer, material.getSpecular());

    // Propriétés numériques
    writer.Write(material.getIllum());
    writer.Write(material.getOpacity());
    writer.Write(material.getShininess());

    return writer.getBuffer();
}

Material MaterialSerializer::DeserializeMaterial(const Bytes& data)
{
    BinaryReader reader(data);

    // Nom du matériau
    const std::string tagName = reader.ReadString();

    // Couleurs
    const MathsSIMD::Vec4_SIMD ambient = ReadVec4(reader);
    const MathsSIMD::Vec4_SIMD diffuse = ReadVec4(reader);
    const MathsSIMD::Vec4_SIMD specular = ReadVec4(reader);

    // Propriétés numériques
    const std::uint16_t illum = reader.ReadU16();
    const float opacity = reader.ReadFloat();
    const float shininess = reader.ReadFloat();

    if (!reader.isAtEnd())
    {
        throw std::runtime_error("MaterialSerializer: unexpected trailing data.");
    }
    
    // Création du matériau
    Material material(
        MaterialTag(tagName),
        nullptr,
        ambient,
        diffuse,
        specular,
        illum,
        opacity
    );

    material.setShininess(shininess);
    return material;
}
