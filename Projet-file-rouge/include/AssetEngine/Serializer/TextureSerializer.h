#pragma once
#include "AssetEngine/Binary/BinaryTypes.h"

class Texture;

class TextureSerializer
{
public:
    static Bytes SerializeTexture(const Texture& texture);
    static void DeserializeTexture(const Bytes& data, Texture& texture);
};
