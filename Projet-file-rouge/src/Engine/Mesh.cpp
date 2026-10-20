#include "Engine/Mesh.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <cmath>

void Mesh::ComputeFaceNormals() {
    faceNormals.clear();
    faceNormals.reserve(faces.size());

    for (const auto& face : faces) {
        const auto& v0 = vertices[face.v0];
        const auto& v1 = vertices[face.v1];
        const auto& v2 = vertices[face.v2];

        MathsSIMD::Vec3_SIMD e1 = v1 - v0;
        MathsSIMD::Vec3_SIMD e2 = v2 - v0;

        MathsSIMD::Vec3_SIMD normal = e1.Cross(e2).Normalize();
        faceNormals.push_back(normal);
    }
}
bool Texture::texture_load(const char* path, Texture* texture)
{
    int w, h, channels;

    uint8_t* data = stbi_load(path, &w, &h, &channels, 4);

    if (!data)
    {
        std::cerr << "texture_load: failed on '"
                  << path << "': "
                  << stbi_failure_reason() << '\n';
        return false;
    }

    texture->width = w;
    texture->height = h;
    texture->pixels.resize(
        static_cast<size_t>(w) * static_cast<size_t>(h)
    );

    for (int i = 0; i < w * h; ++i)
    {
        uint8_t r = data[i * 4 + 0];
        uint8_t g = data[i * 4 + 1];
        uint8_t b = data[i * 4 + 2];
        uint8_t a = data[i * 4 + 3];

        texture->pixels[i] =
            (static_cast<uint32_t>(a) << 24) |
            (static_cast<uint32_t>(r) << 16) |
            (static_cast<uint32_t>(g) << 8)  |
             static_cast<uint32_t>(b);
    }

    stbi_image_free(data);
    return true;
}

void Texture::texture_free(Texture* texture)
{
    if (!texture)
        return;

    texture->pixels.clear();
    texture->width = 0;
    texture->height = 0;
}

uint32_t Texture::texture_sample(
    const Texture* texture, float u, float v)
{
    if (!texture || texture->pixels.empty() ||
        texture->width <= 0 || texture->height <= 0)
        return 0;

    // Répétition de la texture
    u -= std::floor(u);
    v -= std::floor(v);

    int x = static_cast<int>(u * texture->width);
    int y = static_cast<int>((1.0f - v) * texture->height);

    if (x < 0) x = 0;
    if (x >= texture->width) x = texture->width - 1;

    if (y < 0) y = 0;
    if (y >= texture->height) y = texture->height - 1;

    return texture->pixels[y * texture->width + x];
}