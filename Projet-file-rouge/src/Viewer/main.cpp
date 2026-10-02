#include <iostream>
#include <vector>
#include <unordered_map>

#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"

#include "Viewer/BenchmarkParser.h"

int main() {
    if (!glfwInit()) {
        std::cerr << "Erreur : Échec de l'initialisation de GLFW." << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1400, 800, "Benchmark Viewer", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    auto benchmarkGroups = LoadBenchmarkDataGroups("results.json");

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        int display_w = 0;
        int display_h = 0;
        glfwGetFramebufferSize(window, &display_w, &display_h);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

        // --- Panneau 1 : Filtres de sélection ---
        ImGui::Begin("Selection des Fonctions");
        ImGui::Text("Cochez les fonctions a afficher :");
        ImGui::Separator();

        for (auto& [name, group] : benchmarkGroups) {
            ImGui::Checkbox(name.c_str(), &group.selected);
        }
        ImGui::End();

        // --- Panneau 2 : Dashboard Métriques & Graphiques ---
        ImGui::Begin("Analyse de Performance");

        // Tableau récapitulatif des métriques p95
        if (ImGui::BeginTable("MetricsTable", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            ImGui::TableSetupColumn("Fonction");
            ImGui::TableSetupColumn("Cycles/op (p95)");
            ImGui::TableSetupColumn("Marge Erreur (Cycles)");
            ImGui::TableSetupColumn("Vitesse (Ops/s)");
            ImGui::TableSetupColumn("Temps CPU p95 (ns)");
            ImGui::TableSetupColumn("Iterations");
            ImGui::TableHeadersRow();

            for (const auto& [name, group] : benchmarkGroups) {
                if (!group.selected) continue;

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", group.functionName.c_str());

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%.3f", group.p95Metrics.cyclesPerOp);

                ImGui::TableSetColumnIndex(2);
                ImGui::Text("+/- %.3f", group.stdDevCycles);

                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%.2e", group.p95Metrics.itemsPerSecond);

                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%.2f", group.p95Metrics.cpuTime);

                ImGui::TableSetColumnIndex(5);
                ImGui::Text("%lld", group.p95Metrics.iterations);
            }
            ImGui::EndTable();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Graphique ImPlot des itérations
        if (ImPlot::BeginPlot("Cycles par Operation (Iteratif)", ImVec2(-1.0f, 400.0f))) {
            ImPlot::SetupAxes("Index Iteration", "Cycles / Op");

            for (const auto& [name, group] : benchmarkGroups) {
                if (!group.selected || group.iterationCycles.empty()) continue;

                std::vector<double> xData(group.iterationCycles.size());
                for (size_t i = 0; i < xData.size(); ++i) {
                    xData[i] = static_cast<double>(i);
                }

                ImPlot::PlotLine(group.functionName.c_str(), xData.data(), group.iterationCycles.data(), static_cast<int>(xData.size()));
                ImPlot::PlotScatter((group.functionName + " (points)").c_str(), xData.data(), group.iterationCycles.data(), static_cast<int>(xData.size()));
            }

            ImPlot::EndPlot();
        }

        ImGui::End();

        // Rendu OpenGL
        ImGui::Render();
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}