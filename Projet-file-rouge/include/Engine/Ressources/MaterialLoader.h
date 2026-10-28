#pragma once

#include <map>
#include <memory>

#include "Engine/Ressources/Material.h"
#include "Engine/utils/Filepath.h"

class MaterialLoader final
{
public:
    [[nodiscard]] static std::map<MaterialTag, std::shared_ptr<Material>>
        Load(const Filepath& filepath);
};