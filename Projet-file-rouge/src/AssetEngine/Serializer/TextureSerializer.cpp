#include "AssetEngine/Serializer/TextureSerializer.h"

#include "Engine/Ressources/Texture.h"

#include "AssetEngine/Binary/BinaryWriter.h"
#include "AssetEngine/Binary/BinaryReader.h"
#include "AssetEngine/AssetType/PixelFormat.h"

#include <stdexcept>
#include <limits>



Bytes TextureSerializer::SerializeTexture(const Texture& texture)
{
    if (!texture.isLoaded())
        throw std::runtime_error("Cannot serialize an unloaded texture");

    BinaryWriter writer;

    const auto width = static_cast<std::uint32_t>(texture.getWidth());
    const auto height = static_cast<std::uint32_t>(texture.getHeight());

    writer.Write(width);
    writer.Write(height);

    writer.Write(static_cast<std::uint32_t>(PixelFormat::RGBA8));

    const auto& pixels = texture.getPixels();

    const std::size_t expectedSize =
        static_cast<std::size_t>(width) * height;

    if (pixels.size() != expectedSize)
        throw std::runtime_error("Invalid texture pixel count");

    for (const std::uint32_t pixel : pixels)
    {
        const std::uint8_t r = (pixel >> 16) & 0xFF;
        const std::uint8_t g = (pixel >> 8) & 0xFF;
        const std::uint8_t b = pixel & 0xFF;
        const std::uint8_t a = (pixel >> 24) & 0xFF;

        writer.Write(r);
        writer.Write(g);
        writer.Write(b);
        writer.Write(a);
    }

    return writer.getBuffer();
}




void TextureSerializer::DeserializeTexture(const Bytes& data, Texture& texture)
{
    constexpr std::uint32_t metadataSize = 12;
    constexpr std::uint32_t bytesPerPixel = 4;

    if (data.size() < metadataSize)
        throw std::runtime_error("Texture data is too small");

    BinaryReader reader(data);

    const std::uint32_t width = reader.ReadU32();
    const std::uint32_t height = reader.ReadU32();

    const auto pixelFormat =
        static_cast<PixelFormat>(reader.ReadU32());

    if (width == 0 || height == 0)
        throw std::runtime_error("Invalid texture dimensions");

    if (width > static_cast<std::uint32_t>(std::numeric_limits<int>::max()) ||
        height > static_cast<std::uint32_t>(std::numeric_limits<int>::max()))
        throw std::runtime_error("Texture dimensions are too large");

    if (pixelFormat != PixelFormat::RGBA8)
        throw std::runtime_error("Unsupported texture pixel format");

    const std::uint64_t pixelCount =
        static_cast<std::uint64_t>(width) * height;

    const std::uint64_t expectedSize =
        metadataSize + pixelCount * bytesPerPixel;

    if (expectedSize != data.size())
        throw std::runtime_error("Invalid texture data size");

    std::vector<std::uint32_t> pixels;
    pixels.reserve(static_cast<std::size_t>(pixelCount));

    for (std::uint64_t i = 0; i < pixelCount; ++i)
    {
        const std::uint32_t r = reader.ReadU8();
        const std::uint32_t g = reader.ReadU8();
        const std::uint32_t b = reader.ReadU8();
        const std::uint32_t a = reader.ReadU8();

        const std::uint32_t pixel =
            (a << 24) |
            (r << 16) |
            (g << 8) |
            b;

        pixels.push_back(pixel);
    }

    texture.SetData(
        static_cast<int>(width),
        static_cast<int>(height),
        std::move(pixels)
    );
}
