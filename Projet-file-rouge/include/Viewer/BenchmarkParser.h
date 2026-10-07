#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

struct Metrics {
    double realTime = 0.0;
    double cpuTime = 0.0;
    double cyclesPerOp = 0.0;
    double itemsPerSecond = 0.0;
    int64_t iterations = 0;
};

struct BenchmarkDataGroup {
    std::string functionName;
    std::string category;
    bool selected = true;
    
    std::vector<double> iterationCycles;
    std::vector<double> iterationCpuTimes;
    
    Metrics p95Metrics;
    double stdDevCpuTime = 0.0;
    double stdDevCycles = 0.0;
};

using BenchmarkCategories = std::unordered_map<std::string, std::unordered_map<std::string, BenchmarkDataGroup>>;

BenchmarkCategories LoadBenchmarkDataGroups(const std::string& filepath);