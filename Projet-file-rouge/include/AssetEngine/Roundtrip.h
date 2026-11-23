#pragma once 
#include "Binary/BinaryWriter.h"
#include "Binary/BinaryReader.h"
#include "HeaderInit/AssetHeader.h"
#include <vector>

#include <iomanip>
#include <iostream>

#include "AssetEngine/AssetFile/AssetFileWriter.h"
#include "AssetEngine/AssetFile/AssetFileReader.h"
#include <filesystem>


#include "AssetEngine/Serializer/TextureSerializer.h"
#include "Engine/Ressources/Texture.h"
class Roundtrip {

    private : 
        BinaryWriter writer;
        BinaryReader reader;
        AssetHeader header;
    
    // all type : 
    // bool
    // int8
    // int32
    // int16
    // int64  
    // float      
    // double
    // UTF-8 String
    // Collection / Vector

    public : 
        Roundtrip() = default;

    void testAssetFileWriter()
    {
        // 1. Préparer le chemin du fichier
        std::filesystem::path path = "test.singe";

        // 2. Préparer le Header
        AssetHeader header;

        // 3. Préparer les données binaires
        Bytes data = {
            std::byte{0x53},
            std::byte{0x49},
            std::byte{0x4E},
            std::byte{0x47},
            std::byte{0x45}
        };

        // 4. Créer le Writer
        AssetFileWriter writer;

        // 5. Écrire le fichier
        writer.Write(path, header, data);

        std::cout << "Asset file successfully written!" << std::endl;
    }
    
    void verifyAll() {

        writer = BinaryWriter();
        reader = BinaryReader();
        // init all type with a value 
        bool boolValue = true;
        std::uint8_t int8Value = -128;
        std::uint16_t int16Value = -32768;
        std::uint32_t int32Value = -2147483648;
        std::uint64_t int64Value = -9223372036854775807;
        float floatValue = 3.14f;
        double doubleValue = 3.141592653589793;
        std::string stringValue = "Hello, World!";
        std::vector<std::uint32_t> vectorValue = {1, 2, 3, 4, 5};

        // write all type to buffer
        writer.Write(boolValue);
        writer.Write(static_cast<std::uint8_t>(int8Value));
        writer.Write(static_cast<std::uint16_t>(int16Value));
        writer.Write(static_cast<std::uint32_t>(int32Value));
        writer.Write(static_cast<std::uint64_t>(int64Value));
        writer.Write(floatValue);
        writer.Write(doubleValue);
        writer.Write(stringValue);  
        writer.Write(vectorValue); 

        // read all type from buffer
        Bytes buffer = writer.getBuffer();
        reader = BinaryReader(buffer);
        bool readBool = reader.ReadBool();  
        std::uint8_t readInt8 = reader.ReadU8();
        std::uint16_t readInt16 = reader.ReadU16();
        std::uint32_t readInt32 = reader.ReadU32();
        std::uint64_t readInt64 = reader.ReadU64();
        float readFloat = reader.ReadFloat();   
        double readDouble = reader.ReadDouble();  
        std::string readString = reader.ReadString();
        std::vector<std::uint32_t> readVector = reader.ReadVector<std::uint32_t>(); 

        // verify all type
        if (boolValue != readBool) { throw std::runtime_error("Bool value mismatch"); }
        if (int8Value != readInt8) { throw std::runtime_error("Int8 value mismatch"); }
        if (int16Value != readInt16) { throw std::runtime_error("Int16 value mismatch"); }
        if (int32Value != readInt32) { throw std::runtime_error("Int32 value mismatch"); }
        if (int64Value != readInt64) { throw std::runtime_error("Int64 value mismatch"); }
        if (floatValue != readFloat) { throw std::runtime_error("Float value mismatch"); }
        if (doubleValue != readDouble) { throw std::runtime_error("Double value mismatch"); }
        if (stringValue != readString) { throw std::runtime_error("String value mismatch"); }
        if (vectorValue != readVector) { throw std::runtime_error("Vector value mismatch"); }

        TestTextureSerializer();
        
    }
    
    
    void TestTextureSerializer()
    {
        // 1. Création de la texture originale
        Texture originalTexture;

        std::vector<std::uint32_t> originalPixels =
        {
            0xFFFF0000, // Rouge
            0xFF00FF00, // Vert
            0xFF0000FF, // Bleu
            0xFFFFFFFF  // Blanc
        };

        originalTexture.SetData(2, 2, originalPixels);

        // 2. Sérialisation
        Bytes serializedData =
            TextureSerializer::SerializeTexture(originalTexture);

        if (serializedData.size() != 28)
        {
            throw std::runtime_error("Texture serialized data size mismatch");
        }
        
        // 3. Création d'une texture vide
        Texture reconstructedTexture;

        // 4. Désérialisation
        TextureSerializer::DeserializeTexture(
            serializedData,
            reconstructedTexture
        );

        // 5. Vérification des dimensions
        if (originalTexture.getWidth() != reconstructedTexture.getWidth() ||
            originalTexture.getHeight() != reconstructedTexture.getHeight())
        {
            throw std::runtime_error("Texture dimensions mismatch");
        }

        // 6. Vérification des pixels
        if (originalTexture.getPixels() != reconstructedTexture.getPixels())
        {
            throw std::runtime_error("Texture pixels mismatch");
        }
    }

};
