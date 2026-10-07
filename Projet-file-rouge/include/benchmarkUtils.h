#pragma once
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdint>

inline constexpr std::int64_t ITERATIONS = 10;
inline constexpr std::int64_t REP = 10;


inline double ComputeP95(const std::vector<double>& samples) {
    if (samples.empty()) return 0.0;
    std::vector<double> sorted = samples;
    std::sort(sorted.begin(), sorted.end());
    const auto index = static_cast<std::size_t>(std::ceil(0.95 * static_cast<double>(sorted.size()))) - 1;
    return sorted[index];
}