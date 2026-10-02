#include <iostream>
#include "Viewer/BenchmarkParser.h"

int main() {
    auto results = LoadBenchmarkResults("results.json");
    
    std::cout << "Nombre de resultats charges : " << results.size() << std::endl;
    for (const auto& res : results) {
        std::cout << "- " << res.name << " | Cycles/op: " << res.cyclesPerOp << std::endl;
    }

    return 0;
}