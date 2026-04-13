#include "pch.h"
#include "Graph.h"

namespace DijkstraDeliverySystem
{
    Graph::Graph(int vertexCount, bool directed)
        : m_vertexCount(vertexCount), m_directed(directed), m_adjacency(static_cast<size_t>(vertexCount))
    {
        if (vertexCount < 0)
        {
            throw std::invalid_argument("vertexCount must be non-negative");
        }
    }

    void Graph::AddEdge(int from, int to, Weight weight)
    {
        ValidateVertex(from);
        ValidateVertex(to);

        if (weight <= 0)
        {
            throw std::invalid_argument("Dijkstra requires positive edge weights");
        }

        m_adjacency[static_cast<size_t>(from)].emplace_back(to, weight);

        if (!m_directed)
        {
            m_adjacency[static_cast<size_t>(to)].emplace_back(from, weight);
        }
    }

    int Graph::VertexCount() const noexcept
    {
        return m_vertexCount;
    }

    bool Graph::IsDirected() const noexcept
    {
        return m_directed;
    }

    const std::vector<Graph::Neighbor>& Graph::Neighbors(int vertex) const
    {
        ValidateVertex(vertex);
        return m_adjacency[static_cast<size_t>(vertex)];
    }

    void Graph::ValidateVertex(int vertex) const
    {
        if (vertex < 0 || vertex >= m_vertexCount)
        {
            throw std::out_of_range("vertex index is out of range");
        }
    }
}
