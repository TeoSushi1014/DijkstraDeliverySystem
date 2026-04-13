#pragma once

#include <stdexcept>
#include <utility>
#include <vector>

namespace DijkstraDeliverySystem
{
    class Graph
    {
    public:
        using Weight = long long;
        using Neighbor = std::pair<int, Weight>;

        Graph(int vertexCount = 0, bool directed = false);

        void AddEdge(int from, int to, Weight weight);

        int VertexCount() const noexcept;
        bool IsDirected() const noexcept;
        const std::vector<Neighbor>& Neighbors(int vertex) const;

    private:
        void ValidateVertex(int vertex) const;

        int m_vertexCount;
        bool m_directed;
        std::vector<std::vector<Neighbor>> m_adjacency;
    };
}
