# pragma once

#include <vector>
#include "Engine/Mesh.h"

namespace ECS
{
    struct MeshComponent
    {
        Mesh mesh;
        Texture texture;
    };
}