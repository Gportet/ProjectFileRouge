#include "Engine/Ressources/MaterialLoader.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <string>

std::map<MaterialTag, std::shared_ptr<Material>>
MaterialLoader::Load(const Filepath& filepath)
{
    std::map<MaterialTag, std::shared_ptr<Material>> materials;

    // Ouverture du fichier MTL
    std::ifstream file(filepath.str());

    if (!file.is_open())
    {
        std::cerr << "Impossible d'ouvrir le fichier MTL : "
                  << filepath.str() << std::endl;
        return materials;
    }

    // Matériau actuellement en cours de lecture
    std::shared_ptr<Material> currentMaterial = nullptr;

    std::string line;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string type;

        ss >> type;

        // Ignorer les lignes vides et les commentaires
        if (type.empty() || type[0] == '#')
            continue;

        // Création d'un nouveau matériau
        if (type == "newmtl")
        {
            std::string name;
            std::getline(ss >> std::ws, name);

            if (name.empty())
                continue;

            MaterialTag tag(name);

            currentMaterial = std::make_shared<Material>(
                tag,
                nullptr
            );

            materials[tag] = currentMaterial;
        }

        // Les propriétés suivantes nécessitent un matériau courant
        else if (currentMaterial)
        {
            float r, g, b;

            if (type == "Ka")
            {
                if (ss >> r >> g >> b)
                {
                    currentMaterial->setAmbient(
                        MathsSIMD::Vec4_SIMD(
                            _mm_setr_ps(r, g, b, 1.0f)
                        )
                    );
                }
            }
            else if (type == "Kd")
            {
                if (ss >> r >> g >> b)
                {
                    currentMaterial->setDiffuse(
                        MathsSIMD::Vec4_SIMD(
                            _mm_setr_ps(r, g, b, 1.0f)
                        )
                    );
                }
            }
            else if (type == "Ks")
            {
                if (ss >> r >> g >> b)
                {
                    currentMaterial->setSpecular(
                        MathsSIMD::Vec4_SIMD(
                            _mm_setr_ps(r, g, b, 1.0f)
                        )
                    );
                }
            }
            else if (type == "Ns")
            {
                float shininess;

                if (ss >> shininess)
                    currentMaterial->setShininess(shininess);
            }
            else if (type == "illum")
            {
                uint16_t illum;

                if (ss >> illum)
                    currentMaterial->setIllum(illum);
            }
            else if (type == "d")
            {
                float opacity;

                if (ss >> opacity)
                    currentMaterial->setOpacity(
                        std::clamp(opacity, 0.0f, 1.0f)
                    );
            }
            else if (type == "Tr")
            {
                float transparency;

                if (ss >> transparency)
                {
                    currentMaterial->setOpacity(
                        1.0f - std::clamp(transparency, 0.0f, 1.0f)
                    );
                }
            }
            else if (type == "map_Kd")
            {
                std::string textureName;
                std::getline(ss >> std::ws, textureName);

                if (!textureName.empty())
                {
                    // Le chemin de la texture est relatif au fichier MTL
                    Filepath texturePath(
                        filepath.parentPath(),
                        textureName
                    );

                    auto texture = std::make_shared<Texture>();

                    if (texture->Load(texturePath))
                    {
                        currentMaterial->setTexture(texture);
                    }
                    else
                    {
                        std::cerr << "Impossible de charger la texture : "
                                  << texturePath.str() << std::endl;
                    }
                }
            }
        }
    }

    return materials;
}
