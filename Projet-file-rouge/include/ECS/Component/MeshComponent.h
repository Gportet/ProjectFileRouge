# pragma once

#include <vector>
#include "Engine/Mesh.h"
#include <map>
namespace ECS
{
    struct MeshComponent
    {
        Mesh mesh;
        std::map<MaterialTag, std::shared_ptr<Material>> materials;
    };
}