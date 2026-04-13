#pragma once

#include "Graph.h"

#include <vector>

namespace DijkstraDeliverySystem
{
    struct BenchmarkStats
    {
        long long BestMicroseconds{};
        double AverageMicroseconds{};
        long long WorstMicroseconds{};
        int Iterations{};
    };

    struct DijkstraResult
    {
        bool Found{};
        Graph::Weight Distance{};
        std::vector<int> Path;
        std::vector<Graph::Weight> Distances;
        std::vector<int> Parents;
        BenchmarkStats Benchmark;
        BenchmarkStats MatrixBenchmark;
        bool MatrixFound{};
        Graph::Weight MatrixDistance{};
        bool CrossCheckPassed{};
    };

    class DijkstraSolver
    {
    public:
        DijkstraResult Solve(const Graph& graph, int source, int target, int benchmarkIterations = 30) const;
    };
}
