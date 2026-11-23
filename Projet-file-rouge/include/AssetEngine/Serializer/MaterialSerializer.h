#pragma once

#include "AssetEngine/Binary/BinaryTypes.h"

class Material; 

class MaterialSerializer
{
public:
    static Bytes SerializeMaterial(const Material& material);
    static Material DeserializeMaterial(const Bytes& data);
};
