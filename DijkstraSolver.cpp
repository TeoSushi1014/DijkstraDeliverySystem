#include "pch.h"
#include "DijkstraSolver.h"
#include "MatrixGraph.h"

#include <algorithm>
#include <chrono>
#include <functional>
#include <limits>
#include <queue>
#include <stdexcept>

namespace DijkstraDeliverySystem
{
    namespace
    {
        constexpr Graph::Weight kInfinity = (std::numeric_limits<Graph::Weight>::max)() / 4;

        struct RunState
        {
            std::vector<Graph::Weight> Distances;
            std::vector<int> Parents;
        };

        RunState RunDijkstraOnce(const Graph& graph, int source, int target)
        {
            const int vertexCount = graph.VertexCount();
            std::vector<Graph::Weight> distances(static_cast<size_t>(vertexCount), kInfinity);
            std::vector<int> parents(static_cast<size_t>(vertexCount), -1);

            using QueueEntry = std::pair<Graph::Weight, int>;
            std::priority_queue<QueueEntry, std::vector<QueueEntry>, std::greater<QueueEntry>> minHeap;

            distances[static_cast<size_t>(source)] = 0;
            minHeap.emplace(0, source);

            while (!minHeap.empty())
            {
                const auto [currentDistance, currentNode] = minHeap.top();
                minHeap.pop();

                if (currentDistance != distances[static_cast<size_t>(currentNode)])
                {
                    continue;
                }

                if (currentNode == target)
                {
                    break;
                }

                for (const auto& [nextNode, weight] : graph.Neighbors(currentNode))
                {
                    const Graph::Weight candidateDistance = currentDistance + weight;
                    if (candidateDistance < distances[static_cast<size_t>(nextNode)])
                    {
                        distances[static_cast<size_t>(nextNode)] = candidateDistance;
                        parents[static_cast<size_t>(nextNode)] = currentNode;
                        minHeap.emplace(candidateDistance, nextNode);
                    }
                }
            }

            return RunState{ std::move(distances), std::move(parents) };
        }

        RunState RunDijkstraMatrixOnce(const MatrixGraph& graph, int source, int target)
        {
            const int vertexCount = graph.VertexCount();
            std::vector<Graph::Weight> distances(static_cast<size_t>(vertexCount), kInfinity);
            std::vector<int> parents(static_cast<size_t>(vertexCount), -1);
            std::vector<bool> visited(static_cast<size_t>(vertexCount), false);

            distances[static_cast<size_t>(source)] = 0;

            for (int iteration = 0; iteration < vertexCount; ++iteration)
            {
                int currentNode = -1;
                Graph::Weight bestDistance = kInfinity;

                for (int node = 0; node < vertexCount; ++node)
                {
                    if (!visited[static_cast<size_t>(node)] && distances[static_cast<size_t>(node)] < bestDistance)
                    {
                        bestDistance = distances[static_cast<size_t>(node)];
                        currentNode = node;
                    }
                }

                if (currentNode == -1 || currentNode == target)
                {
                    break;
                }

                visited[static_cast<size_t>(currentNode)] = true;

                for (int nextNode = 0; nextNode < vertexCount; ++nextNode)
                {
                    const Graph::Weight edgeWeight = graph.WeightAt(currentNode, nextNode);
                    if (edgeWeight <= 0)
                    {
                        continue;
                    }

                    const Graph::Weight candidateDistance = distances[static_cast<size_t>(currentNode)] + edgeWeight;
                    if (candidateDistance < distances[static_cast<size_t>(nextNode)])
                    {
                        distances[static_cast<size_t>(nextNode)] = candidateDistance;
                        parents[static_cast<size_t>(nextNode)] = currentNode;
                    }
                }
            }

            return RunState{ std::move(distances), std::move(parents) };
        }

        std::vector<int> ReconstructPath(const std::vector<int>& parents, int source, int target)
        {
            std::vector<int> reversedPath;
            int current = target;

            while (current != -1)
            {
                reversedPath.push_back(current);

                if (current == source)
                {
                    break;
                }

                current = parents[static_cast<size_t>(current)];
            }

            if (reversedPath.empty() || reversedPath.back() != source)
            {
                return {};
            }

            std::reverse(reversedPath.begin(), reversedPath.end());
            return reversedPath;
        }
    }

    DijkstraResult DijkstraSolver::Solve(
        const Graph& graph,
        int source,
        int target,
        int benchmarkIterations,
        ProgressCallback progressCallback) const
    {
        if (graph.VertexCount() <= 0)
        {
            throw std::invalid_argument("graph must contain at least one vertex");
        }

        if (source < 0 || source >= graph.VertexCount())
        {
            throw std::out_of_range("source vertex is out of range");
        }

        if (target < 0 || target >= graph.VertexCount())
        {
            throw std::out_of_range("target vertex is out of range");
        }

        if (source == target)
        {
            BenchmarkStats stats{};
            stats.Iterations = (std::max)(1, benchmarkIterations);
            stats.BestMicroseconds = 0;
            stats.AverageMicroseconds = 0.0;
            stats.WorstMicroseconds = 0;

            std::vector<Graph::Weight> distances(static_cast<size_t>(graph.VertexCount()), kInfinity);
            std::vector<int> parents(static_cast<size_t>(graph.VertexCount()), -1);
            distances[static_cast<size_t>(source)] = 0;

            return DijkstraResult{
                true,
                0,
                std::vector<int>{ source },
                std::move(distances),
                std::move(parents),
                stats,
                stats,
                true,
                0,
                true,
                false
            };
        }

        const int iterations = (std::max)(1, benchmarkIterations);
        const MatrixGraph matrixGraph(graph);

        BenchmarkStats stats{};
        stats.Iterations = iterations;
        stats.BestMicroseconds = (std::numeric_limits<long long>::max)();
        stats.WorstMicroseconds = 0;

        BenchmarkStats matrixStats{};
        matrixStats.Iterations = iterations;
        matrixStats.BestMicroseconds = (std::numeric_limits<long long>::max)();
        matrixStats.WorstMicroseconds = 0;

        RunState finalState{};
        RunState finalMatrixState{};
        long long totalMicroseconds = 0;
        long long totalMatrixMicroseconds = 0;
        int completedIterations = 0;
        bool cancelled = false;

        for (int i = 0; i < iterations; ++i)
        {
            const auto startedAt = std::chrono::high_resolution_clock::now();
            RunState state = RunDijkstraOnce(graph, source, target);
            const auto endedAt = std::chrono::high_resolution_clock::now();

            const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(endedAt - startedAt).count();
            totalMicroseconds += elapsed;
            stats.BestMicroseconds = (std::min)(stats.BestMicroseconds, elapsed);
            stats.WorstMicroseconds = (std::max)(stats.WorstMicroseconds, elapsed);

            if (i == 0)
            {
                finalState = std::move(state);
            }

            const auto matrixStartedAt = std::chrono::high_resolution_clock::now();
            RunState matrixState = RunDijkstraMatrixOnce(matrixGraph, source, target);
            const auto matrixEndedAt = std::chrono::high_resolution_clock::now();

            const auto matrixElapsed = std::chrono::duration_cast<std::chrono::microseconds>(matrixEndedAt - matrixStartedAt).count();
            totalMatrixMicroseconds += matrixElapsed;
            matrixStats.BestMicroseconds = (std::min)(matrixStats.BestMicroseconds, matrixElapsed);
            matrixStats.WorstMicroseconds = (std::max)(matrixStats.WorstMicroseconds, matrixElapsed);

            if (i == 0)
            {
                finalMatrixState = std::move(matrixState);
            }

            completedIterations = i + 1;

            if (progressCallback && !progressCallback(completedIterations, iterations))
            {
                cancelled = true;
                break;
            }
        }

        if (completedIterations == 0)
        {
            BenchmarkStats emptyStats{};
            emptyStats.Iterations = 0;
            emptyStats.BestMicroseconds = 0;
            emptyStats.AverageMicroseconds = 0.0;
            emptyStats.WorstMicroseconds = 0;

            std::vector<Graph::Weight> distances(static_cast<size_t>(graph.VertexCount()), kInfinity);
            std::vector<int> parents(static_cast<size_t>(graph.VertexCount()), -1);

            return DijkstraResult{
                false,
                -1,
                {},
                std::move(distances),
                std::move(parents),
                emptyStats,
                emptyStats,
                false,
                -1,
                true,
                cancelled
            };
        }

        stats.Iterations = completedIterations;
        matrixStats.Iterations = completedIterations;

        stats.AverageMicroseconds = static_cast<double>(totalMicroseconds) / static_cast<double>(completedIterations);
        matrixStats.AverageMicroseconds = static_cast<double>(totalMatrixMicroseconds) / static_cast<double>(completedIterations);

        const Graph::Weight distance = finalState.Distances[static_cast<size_t>(target)];
        const bool found = (distance != kInfinity);
        std::vector<int> path = found ? ReconstructPath(finalState.Parents, source, target) : std::vector<int>{};

        const Graph::Weight matrixDistance = finalMatrixState.Distances[static_cast<size_t>(target)];
        const bool matrixFound = (matrixDistance != kInfinity);
        const bool crossCheckPassed = (found == matrixFound) && (!found || distance == matrixDistance);

        return DijkstraResult{
            found,
            found ? distance : -1,
            std::move(path),
            std::move(finalState.Distances),
            std::move(finalState.Parents),
            stats,
            matrixStats,
            matrixFound,
            matrixFound ? matrixDistance : -1,
            crossCheckPassed,
            cancelled
        };
    }
}
