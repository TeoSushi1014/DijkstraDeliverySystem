#pragma once

#include "Graph.h"

#include <vector>

namespace DijkstraDeliverySystem
{
    class MatrixGraph
    {
    public:
        explicit MatrixGraph(const Graph& graph);

        int VertexCount() const noexcept;
        bool IsDirected() const noexcept;
        bool HasEdge(int from, int to) const;
        Graph::Weight WeightAt(int from, int to) const;

    private:
        static constexpr Graph::Weight kNoEdge = -1;

        int m_vertexCount{};
        bool m_directed{};
        std::vector<std::vector<Graph::Weight>> m_weights;
    };
}
