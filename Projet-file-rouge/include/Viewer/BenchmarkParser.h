#pragma once

#include <string>
#include <vector>
#include <unordered_map>

struct BenchmarkMetrics {
    double realTime = 0.0;       
    double cpuTime = 0.0;       
    double cyclesPerOp = 0.0;    
    double itemsPerSecond = 0.0;  
    long long iterations = 0;     
};

struct BenchmarkDataGroup {
    std::string functionName;    
    bool selected = true;        
    
    // Métriques agrégées p95
    BenchmarkMetrics p95Metrics;
    
    // Marge d'erreur (écart-type / stddev)
    double stdDevCpuTime = 0.0;
    double stdDevCycles = 0.0;

    // Historique des itérations pour le tracé graphique
    std::vector<double> iterationCycles;
    std::vector<double> iterationCpuTimes;
};

// Charge et regroupe les résultats par nom de fonction
std::unordered_map<std::string, BenchmarkDataGroup> LoadBenchmarkDataGroups(const std::string& filepath);