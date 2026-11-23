
#include "AssetEngine/Serializer/MeshSerializer.h"

#include "AssetEngine/Binary/BinaryWriter.h"
#include "AssetEngine/Binary/BinaryReader.h"
#include "Engine/Mesh.h"

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>

// ------------------------------------------------------------
// Collection validation
// ------------------------------------------------------------

template<typename T>
static void CheckCollectionSize(
    std::uint32_t count,
    const BinaryReader& reader,
    std::size_t serializedElementSize)
{
    // Limite la mémoire utilisée par une collection.
    const std::size_t maxElements =
        BinaryReader::MaxAllocationBytes / sizeof(T);

    if (count > maxElements)
        throw std::length_error("Mesh collection allocation limit exceeded");

    // Vérifie que les données nécessaires sont présentes.
    if (serializedElementSize != 0 &&
        count > reader.getRemainingSize() / serializedElementSize)
    {
        throw std::runtime_error("Invalid mesh collection size");
    }
}

static std::uint32_t GetCollectionSize(std::size_t size)
{
    if (size > std::numeric_limits<std::uint32_t>::max())
        throw std::length_error("Mesh collection is too large");

    return static_cast<std::uint32_t>(size);
}

// ------------------------------------------------------------
// Vec3_SIMD
// ------------------------------------------------------------

static void WriteVec3(
    BinaryWriter& writer,
    const MathsSIMD::Vec3_SIMD& vec)
{
    writer.Write(MathsSIMD::Vec3_SIMD::GetX(vec.data));
    writer.Write(MathsSIMD::Vec3_SIMD::GetY(vec.data));
    writer.Write(MathsSIMD::Vec3_SIMD::GetZ(vec.data));
}

static MathsSIMD::Vec3_SIMD ReadVec3(BinaryReader& reader)
{
    const float x = reader.Read<float>();
    const float y = reader.Read<float>();
    const float z = reader.Read<float>();

    return MathsSIMD::Vec3_SIMD(x, y, z);
}

// ------------------------------------------------------------
// Face
// ------------------------------------------------------------

static void WriteFace(BinaryWriter& writer, const Face& face)
{
    writer.Write(static_cast<std::int32_t>(face.v0));
    writer.Write(static_cast<std::int32_t>(face.v1));
    writer.Write(static_cast<std::int32_t>(face.v2));

    writer.Write(static_cast<std::int32_t>(face.vt0));
    writer.Write(static_cast<std::int32_t>(face.vt1));
    writer.Write(static_cast<std::int32_t>(face.vt2));

    writer.Write(face.materialIndex);
}

static Face ReadFace(BinaryReader& reader)
{
    Face face{};

    face.v0 = reader.Read<std::int32_t>();
    face.v1 = reader.Read<std::int32_t>();
    face.v2 = reader.Read<std::int32_t>();

    face.vt0 = reader.Read<std::int32_t>();
    face.vt1 = reader.Read<std::int32_t>();
    face.vt2 = reader.Read<std::int32_t>();

    face.materialIndex = reader.Read<std::uint32_t>();

    return face;
}

// ------------------------------------------------------------
// Mesh validation
// ------------------------------------------------------------

static void ValidateMesh(const Mesh& mesh)
{
    for (const Face& face : mesh.faces)
    {
        // Vertex indices
        if (face.v0 < 0 || face.v1 < 0 || face.v2 < 0 ||
            static_cast<std::size_t>(face.v0) >= mesh.vertices.size() ||
            static_cast<std::size_t>(face.v1) >= mesh.vertices.size() ||
            static_cast<std::size_t>(face.v2) >= mesh.vertices.size())
        {
            throw std::runtime_error("Invalid vertex index in mesh face");
        }

        // UV indices
        if (mesh.uvs.empty())
        {
            // Convention: -1 means no UV assigned.
            if (face.vt0 != -1 || face.vt1 != -1 || face.vt2 != -1)
                throw std::runtime_error("Invalid missing UV indices");
        }
        else if (face.vt0 < 0 || face.vt1 < 0 || face.vt2 < 0 ||
                 static_cast<std::size_t>(face.vt0) >= mesh.uvs.size() ||
                 static_cast<std::size_t>(face.vt1) >= mesh.uvs.size() ||
                 static_cast<std::size_t>(face.vt2) >= mesh.uvs.size())
        {
            throw std::runtime_error("Invalid UV index in mesh face");
        }

        // Material index
        if (mesh.materialTags.empty())
        {
            // Convention: 0 means no material assigned.
            if (face.materialIndex != 0)
                throw std::runtime_error("Invalid material index");
        }
        else if (face.materialIndex >= mesh.materialTags.size())
        {
            throw std::runtime_error("Invalid material index in mesh face");
        }
    }

    // Normals can be absent, but if present their counts must match.
    if (!mesh.faceNormals.empty() &&
        mesh.faceNormals.size() != mesh.faces.size())
    {
        throw std::runtime_error("Face normal count does not match face count");
    }

    if (!mesh.vertexNormals.empty() &&
        mesh.vertexNormals.size() != mesh.vertices.size())
    {
        throw std::runtime_error("Vertex normal count does not match vertex count");
    }
}

// ------------------------------------------------------------
// Serialization
// ------------------------------------------------------------

Bytes MeshSerializer::SerializeMesh(const Mesh& mesh)
{
    ValidateMesh(mesh);

    BinaryWriter writer;

    // Material tags
    writer.Write(GetCollectionSize(mesh.materialTags.size()));

    for (const auto& tag : mesh.materialTags)
        writer.Write(tag.getName());

    // Vertices
    writer.Write(GetCollectionSize(mesh.vertices.size()));

    for (const auto& vertex : mesh.vertices)
        WriteVec3(writer, vertex);

    // Faces
    writer.Write(GetCollectionSize(mesh.faces.size()));

    for (const auto& face : mesh.faces)
        WriteFace(writer, face);

    // UV coordinates
    writer.Write(GetCollectionSize(mesh.uvs.size()));

    for (const auto& uv : mesh.uvs)
    {
        writer.Write(uv.u);
        writer.Write(uv.v);
    }

    // Face normals
    writer.Write(GetCollectionSize(mesh.faceNormals.size()));

    for (const auto& normal : mesh.faceNormals)
        WriteVec3(writer, normal);

    // Vertex normals
    writer.Write(GetCollectionSize(mesh.vertexNormals.size()));

    for (const auto& normal : mesh.vertexNormals)
        WriteVec3(writer, normal);

    return writer.getBuffer();
}

// ------------------------------------------------------------
// Deserialization
// ------------------------------------------------------------

Mesh MeshSerializer::DeserializeMesh(const Bytes& data)
{
    BinaryReader reader(data);
    Mesh mesh;

    // Material tags
    const std::uint32_t materialCount =
        reader.Read<std::uint32_t>();

    CheckCollectionSize<MaterialTag>(materialCount, reader, 4);

    mesh.materialTags.reserve(materialCount);

    for (std::uint32_t i = 0; i < materialCount; ++i)
        mesh.materialTags.emplace_back(reader.ReadString());

    // Vertices
    const std::uint32_t vertexCount =
        reader.Read<std::uint32_t>();

    CheckCollectionSize<MathsSIMD::Vec3_SIMD>(vertexCount, reader, 12);

    mesh.vertices.reserve(vertexCount);

    for (std::uint32_t i = 0; i < vertexCount; ++i)
        mesh.vertices.push_back(ReadVec3(reader));

    // Faces
    const std::uint32_t faceCount =
        reader.Read<std::uint32_t>();

    CheckCollectionSize<Face>(faceCount, reader, 28);

    mesh.faces.reserve(faceCount);

    for (std::uint32_t i = 0; i < faceCount; ++i)
        mesh.faces.push_back(ReadFace(reader));

    // UV coordinates
    const std::uint32_t uvCount =
        reader.Read<std::uint32_t>();

    CheckCollectionSize<Vec2>(uvCount, reader, 8);

    mesh.uvs.reserve(uvCount);

    for (std::uint32_t i = 0; i < uvCount; ++i)
    {
        Vec2 uv{};

        uv.u = reader.Read<float>();
        uv.v = reader.Read<float>();

        mesh.uvs.push_back(uv);
    }

    // Face normals
    const std::uint32_t faceNormalCount =
        reader.Read<std::uint32_t>();

    CheckCollectionSize<MathsSIMD::Vec3_SIMD>(
        faceNormalCount, reader, 12);

    mesh.faceNormals.reserve(faceNormalCount);

    for (std::uint32_t i = 0; i < faceNormalCount; ++i)
        mesh.faceNormals.push_back(ReadVec3(reader));

    // Vertex normals
    const std::uint32_t vertexNormalCount =
        reader.Read<std::uint32_t>();

    CheckCollectionSize<MathsSIMD::Vec3_SIMD>(
        vertexNormalCount, reader, 12);

    mesh.vertexNormals.reserve(vertexNormalCount);

    for (std::uint32_t i = 0; i < vertexNormalCount; ++i)
        mesh.vertexNormals.push_back(ReadVec3(reader));

    // Check that the entire buffer was consumed.
    if (!reader.isAtEnd())
        throw std::runtime_error("Mesh contains trailing data");

    // Check indices and collection consistency.
    ValidateMesh(mesh);

    return mesh;
}
