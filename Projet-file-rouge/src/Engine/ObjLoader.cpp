#include "Engine/ObjLoader.h"

#include <iostream>
#include <fstream>
#include <sstream>

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
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v") {
            float x, y, z;
            if (ss >> x >> y >> z) {
                mesh.vertices.push_back(MathsSIMD::Vec3_SIMD(x, y, z)); //mal au perf 
            }
        }
        else if (type == "vt") {
            float u, v;
            if (ss >> u >> v) {
                mesh.uvs.push_back(Vec2{ u, v });
            }
        }
        else if (type == "f") {
            Face face{};
            face.materialIndex = currentMaterialIndex;

            bool valid = true;

            for (int i = 0; i < 3; ++i) {
                std::string token;
                if (ss >> token) {
                    std::stringstream token_stream(token);
                    std::string v_str, vt_str;
                    
                    std::getline(token_stream, v_str, '/');
                    std::getline(token_stream, vt_str, '/');

                    if (!v_str.empty()) {
                        int v_idx = std::stoi(v_str) - 1; // OBJ est basé sur 1
                        int vt_idx = !vt_str.empty() ? std::stoi(vt_str) - 1 : 0;

                        if (i == 0) { face.v0 = v_idx; face.vt0 = vt_idx; }
                        else if (i == 1) { face.v1 = v_idx; face.vt1 = vt_idx; }
                        else if (i == 2) { face.v2 = v_idx; face.vt2 = vt_idx; }
                    } else {
                        valid = false;
                    }
                } else {
                    valid = false;
                }
            }

            if (valid) {
                mesh.faces.push_back(face);
            }
        }
        
        else if (type == "usemtl") {
            std::string materialName;
            ss >> materialName;

            MaterialTag tag(materialName);

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