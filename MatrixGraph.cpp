#include "pch.h"
#include "MatrixGraph.h"

#include <stdexcept>

namespace DijkstraDeliverySystem
{
    MatrixGraph::MatrixGraph(const Graph& graph)
        : m_vertexCount(graph.VertexCount()),
          m_directed(graph.IsDirected()),
          m_weights(static_cast<size_t>(graph.VertexCount()), std::vector<Graph::Weight>(static_cast<size_t>(graph.VertexCount()), kNoEdge))
    {
        for (int from = 0; from < m_vertexCount; ++from)
        {
            m_weights[static_cast<size_t>(from)][static_cast<size_t>(from)] = 0;

            for (const auto& [to, weight] : graph.Neighbors(from))
            {
                auto& current = m_weights[static_cast<size_t>(from)][static_cast<size_t>(to)];
                if (current == kNoEdge || weight < current)
                {
                    current = weight;
                }
            }
        }
    }

    int MatrixGraph::VertexCount() const noexcept
    {
        return m_vertexCount;
    }

    bool MatrixGraph::IsDirected() const noexcept
    {
        return m_directed;
    }

    bool MatrixGraph::HasEdge(int from, int to) const
    {
        if (from < 0 || from >= m_vertexCount || to < 0 || to >= m_vertexCount)
        {
            throw std::out_of_range("vertex index is out of range");
        }

        return m_weights[static_cast<size_t>(from)][static_cast<size_t>(to)] != kNoEdge;
    }

    Graph::Weight MatrixGraph::WeightAt(int from, int to) const
    {
        if (from < 0 || from >= m_vertexCount || to < 0 || to >= m_vertexCount)
        {
            throw std::out_of_range("vertex index is out of range");
        }

        return m_weights[static_cast<size_t>(from)][static_cast<size_t>(to)];
    }
}
