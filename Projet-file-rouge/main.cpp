#include "Engine/DisplayWindow.h"
#include "Engine/Pipeline.h"
#include "Engine/ObjLoader.h"
#include "Engine/ParticleGenerator.h"

#include "ECS/component.h"
#include "ECS/Registry.h"
#include "ECS/System/RenderSystem.h"
#include "ECS/System/SpinSystem.h"

#include <iostream>

int main() {
    // =========================================================================
    // 1. INITIALISATION DU CONTEXTE ET DU MOTEUR DE RENDU
    // =========================================================================
    DisplayWindow window(WIDTH, HEIGHT, "Rasterizer 3D - Smooth Phong & Near Clipping");
    if (!window.Init()) {
        std::cerr << "Erreur : Impossible d'initialiser la fenetre OpenGL/GLFW." << std::endl;
        return -1;
    }

    // Pipeline de rendu logiciel (Color Buffer & Z-Buffer)
    Pipeline pipeline(WIDTH, HEIGHT);
    ParticleGenerator particleGenerator(500);

    // =========================================================================
    // 2. ECS : TYPES DE COMPOSANTS, ENTITES, SYSTEMES
    // =========================================================================
    ECS::Registry registry;

    registry.RegisterComponent<ECS::TransformComponent>();
    registry.RegisterComponent<ECS::SpinComponent>();
    registry.RegisterComponent<ECS::MeshComponent>();
    registry.RegisterComponent<ECS::CameraComponent>();
    registry.RegisterComponent<ECS::LightComponent>();

    // --- Spot : maillage + texture + rotation ---
    ECS::Entity spot = registry.CreateEntity();
    registry.AddComponent(spot, ECS::TransformComponent{});
    registry.AddComponent(spot, ECS::SpinComponent{});
    registry.AddComponent(spot, ECS::MeshComponent{});          // vide : on le remplit juste après, sans copie

    {
        ECS::MeshComponent& meshComp = registry.GetComponent<ECS::MeshComponent>(spot);

        meshComp.mesh = ObjLoader::Load("C:/ProjectFileRouge/Projet-file-rouge/assets/spot.obj");
        if (meshComp.mesh.vertices.empty()) {
            std::cerr << "Erreur : Maillage vide ou chemin introuvable." << std::endl;
            return -1;
        }
        if (!Texture::texture_load("C:/ProjectFileRouge/Projet-file-rouge/assets/spot_texture.png", &meshComp.texture)) {
            std::cerr << "Erreur : Impossible de charger la texture." << std::endl;
            return 1;
        }
    }

    // --- Camera (Position Eye, Target, Up Vector) ---
    ECS::Entity camera = registry.CreateEntity();
    registry.AddComponent(camera, ECS::CameraComponent{
        MathsSIMD::Vec3_SIMD(0.0f, 0.0f, 3.0f),
        MathsSIMD::Vec3_SIMD(0.0f, 0.0f, 0.0f),
        MathsSIMD::Vec3_SIMD(0.0f, 1.0f, 0.0f)
    });

    // --- Source lumineuse directionnelle (valeurs par défaut de LightComponent) ---
    ECS::Entity sun = registry.CreateEntity();
    registry.AddComponent(sun, ECS::LightComponent{});

    // --- Systèmes : exécutés dans cet ordre à chaque registry.Update() ---
    float dt = 0.0f;
    registry.AddSystem([&dt](ECS::EntityManager& em) { ECS::SpinSystem(em, dt); });
    registry.AddSystem([&pipeline](ECS::EntityManager& em) { ECS::RenderSystem(em, pipeline); });

    double lastFrameTime = glfwGetTime();

    // =========================================================================
    // 3. BOUCLE PRINCIPALE DE RENDU (RENDER LOOP)
    // =========================================================================
    while (!window.ShouldClose()) {
        window.PollEvents();
        pipeline.Clear(); // Reinitialise les buffers de couleur et de profondeur

        // Delta
        double currentFrameTime = glfwGetTime();
        dt = static_cast<float>(currentFrameTime - lastFrameTime);
        lastFrameTime = currentFrameTime;

        // Spin puis Render (ordre d'ajout des systèmes)
        registry.Update();

        // Particules (pas encore dans l'ECS)
        float centerX = static_cast<float>(WIDTH) * 0.5f;
        float centerY = static_cast<float>(HEIGHT) * 0.5f;
        Vec2 emitterPos{centerX, centerY};

        particleGenerator.Update(dt, ParticleType::Snow);
        particleGenerator.Update(dt, ParticleType::firework, emitterPos);
        particleGenerator.Draw(pipeline);

        // Transfert du Framebuffer vers la fenêtre et affichage
        window.RenderBuffer(pipeline.GetColorBuffer());
    }

    return 0;
}