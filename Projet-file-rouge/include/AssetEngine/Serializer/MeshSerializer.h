#pragma once

#include "AssetEngine/Binary/BinaryTypes.h"

struct Mesh;

class MeshSerializer
{
public:
    static Bytes SerializeMesh(const Mesh& mesh);
    static Mesh DeserializeMesh(const Bytes& data);
};