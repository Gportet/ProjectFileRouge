#include "Engine/ObjLoader.h"

#include <iostream>
#include <fstream>

// Observe les token sans copier les caractère
#include <string_view>

// Permet from_char qui convertit les chaine de caractère en value sans créé de chaine temporaire
#include <charconv>

// Permet std::errc
#include <system_error>

// Permet std::distance
#include <iterator>

#include <limits>
#include <algorithm>
#include <cstdint>

namespace  
{
    bool ReadToken(std::string_view& line, std::string_view& token)
    {
        const size_t start = line.find_first_not_of(" \t\r");
        
        if (start == std::string_view::npos)
            return false;
        line.remove_prefix(start);
        
        const size_t end = line.find_first_of(" \t\r");
        
        token = line.substr(0, end);
        
        if (end == std::string_view::npos)
            line.remove_prefix(line.size());
        else
            line.remove_prefix(end);
        
        return true;
    }
    
    template<typename T>
    bool ParseNumber(const std::string_view& token, T& value)
    {
        const auto [ptr, ec] = std::from_chars(
            token.data(), 
            token.data() + token.size(), 
            value
            );
        
        return ec == std::errc() && ptr == token.data() + token.size();
    }
    
    bool ParseFaceVertex(
        std::string_view token,
        int& vertexIndex,
        int& uvIndex)
    {
        const size_t firstSlash = token.find('/');
        
        std::string_view vertexPart;
        std::string_view uvPart;
        
        if (firstSlash == std::string_view::npos)
        {
            vertexPart = token;
        }
        else
        {
            vertexPart = token.substr(0, firstSlash);
            
            const size_t secondSlash = token.find('/', firstSlash + 1);
            
            if (secondSlash == std::string_view::npos)
            {
                uvPart = token.substr(firstSlash + 1);
            }
            else
            {
                uvPart = token.substr(
                    firstSlash + 1, 
                    secondSlash - firstSlash - 1
                    );
            }
        }
        
        if (!ParseNumber(vertexPart, vertexIndex))
            return false;
        
        uvIndex = 0;
        
        if (!uvPart.empty() && !ParseNumber(uvPart, uvIndex))
            return false;
        
        --vertexIndex;
        
        if (!uvPart.empty())
            --uvIndex;
        
        return true;
    }
}


Mesh ObjLoader::Load(const Filepath& filepath) {
    Mesh mesh;
    std::ifstream file(filepath.str());

    if (!file.is_open()) {
        std::cerr << "Impossible d'ouvrir le fichier : " << filepath.str() << std::endl;
        return mesh;
    }

    uint32_t currentMaterialIndex = std::numeric_limits<uint32_t>::max();
    
    std::string line;
    while (std::getline(file, line))
    {
        std::string_view lineView(line);
        std::string_view type;
        
        if (!ReadToken(lineView, type))
            continue;
        
        std::string_view token;
        float value[3];
        bool valid = true;
        
        int count = 0;
        if (type == "v")
            count = 3;
        
        else if (type == "vt")
            count = 2;
        
        else if (type == "vn")
            count = 3;
        
        
        for (int i = 0; i < count; ++i)
        {
            if (!ReadToken(lineView, token) || !ParseNumber(token, value[i]))
            {
                valid = false;
                break;
            }
            
            
        }
        if (valid)
        {
            if (type == "v") {
                mesh.vertices.push_back(MathsSIMD::Vec3_SIMD(value[0], value[1], value[2]));
            }
            
            else if (type == "vt")
            {
                mesh.uvs.push_back(Vec2(value[0], value[1]));
            }
            
            // Pour l'ajout des normales plus tard
            /*else if (type == "vn")
            {
                mesh.vertices.push_back(MathsSIMD::Vec3_SIMD(value[0], value[1], value[2]));
            }*/
        }
        
        if (type == "f")
        {
            Face face{};
            face.materialIndex = currentMaterialIndex;
                
            bool validFace = true;
            std::string_view tokenFace;
            int vertexIndex = 0, vertexUVIndex = 0;
                
            for (int i = 0; i < 3; ++i)
            {
                if (!ReadToken(lineView, tokenFace) || !ParseFaceVertex(tokenFace, vertexIndex, vertexUVIndex))
                {
                    validFace = false;
                    break;
                }
                    
                if (i == 0)
                {
                    face.v0 = vertexIndex;
                    face.vt0 = vertexUVIndex;
                }
                else if (i == 1)
                {
                    face.v1 = vertexIndex;
                    face.vt1 = vertexUVIndex;
                }
                else if (i == 2)
                {
                    face.v2 = vertexIndex;
                    face.vt2 = vertexUVIndex;
                }
            }
            if (validFace)
            {
                mesh.faces.push_back(face);
            }
        }
        
        else if (type == "usemtl") {
            std::string_view materialName;
            if (!ReadToken(lineView, materialName))
                continue;

            MaterialTag tag{std::string(materialName)};

            auto it = std::find(
                mesh.materialTags.begin(),
                mesh.materialTags.end(),
                tag
            );

            if (it == mesh.materialTags.end()) {
                mesh.materialTags.push_back(tag);
                currentMaterialIndex =
                    static_cast<uint32_t>(mesh.materialTags.size() - 1);
            }
            else {
                currentMaterialIndex =
                    static_cast<uint32_t>(
                        std::distance(mesh.materialTags.begin(), it)
                    );
            }
        }
    }
    
    if (!mesh.faces.empty()) {
        mesh.ComputeFaceNormals();
        mesh.ComputeVertexNormals();
    }

    return mesh;
}