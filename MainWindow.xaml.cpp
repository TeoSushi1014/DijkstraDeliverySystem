#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

using namespace winrt;
using namespace Windows::Foundation::Numerics;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Media;
using namespace Microsoft::UI::Xaml::Shapes;
using namespace Windows::Foundation;

namespace winrt::DijkstraDeliverySystem::implementation
{
    MainWindow::MainWindow()
    {
        m_isUiReady = false;
        InitializeComponent();
        BuildSampleGraph();
        RenderGraph({});
        StatusTextBlock().Text(L"Enter source and target nodes, then run Dijkstra.");
        m_isUiReady = true;
    }

    void MainWindow::BuildSampleGraph()
    {
        const int testCase = SelectedTestCase();
        LoadGraphByTestCase(testCase);
        SetInputsForTestCase(testCase);
    }

    void MainWindow::LoadGraphByTestCase(int testCase)
    {
        m_expectedDistance = -1;

        if (testCase == 2)
        {
            m_graph = ::DijkstraDeliverySystem::Graph(5, false);
            m_graph.AddEdge(0, 1, 2);
            m_graph.AddEdge(0, 2, 2);
            m_graph.AddEdge(1, 3, 2);
            m_graph.AddEdge(2, 3, 2);
            m_graph.AddEdge(3, 4, 2);
            m_graph.AddEdge(1, 2, 1);
            m_expectedDistance = 6;
        }
        else if (testCase == 3)
        {
            m_graph = ::DijkstraDeliverySystem::Graph(6, false);
            m_graph.AddEdge(0, 1, 1);
            m_graph.AddEdge(1, 2, 2);
            m_graph.AddEdge(3, 4, 1);
            m_graph.AddEdge(4, 5, 2);
        }
        else if (testCase == 4)
        {
            m_graph = ::DijkstraDeliverySystem::Graph(8, false);
            m_graph.AddEdge(0, 1, 4);
            m_graph.AddEdge(0, 2, 3);
            m_graph.AddEdge(1, 2, 1);
            m_graph.AddEdge(1, 3, 2);
            m_graph.AddEdge(2, 4, 6);
            m_graph.AddEdge(3, 4, 1);
            m_graph.AddEdge(3, 5, 5);
            m_graph.AddEdge(4, 6, 2);
            m_graph.AddEdge(5, 6, 1);
            m_graph.AddEdge(5, 7, 7);
            m_graph.AddEdge(6, 7, 3);
            m_expectedDistance = 0;
        }
        else if (testCase == 5)
        {
            constexpr int vertexCount = 50;
            m_graph = ::DijkstraDeliverySystem::Graph(vertexCount, false);

            for (int i = 0; i < vertexCount - 1; ++i)
            {
                m_graph.AddEdge(i, i + 1, 1 + (i % 4));
            }

            m_graph.AddEdge(0, 5, 3);
            m_graph.AddEdge(3, 9, 4);
            m_graph.AddEdge(6, 12, 2);
            m_graph.AddEdge(10, 15, 3);
            m_graph.AddEdge(14, 19, 4);
            m_graph.AddEdge(2, 11, 6);
            m_graph.AddEdge(8, 18, 5);

            for (int i = 20; i < vertexCount; i += 5)
            {
                if (i + 4 < vertexCount)
                {
                    m_graph.AddEdge(i, i + 2, 2 + (i % 3));
                    m_graph.AddEdge(i + 1, i + 4, 3 + ((i + 1) % 4));
                }

                if (i - 7 >= 0)
                {
                    m_graph.AddEdge(i, i - 7, 5);
                }
            }

            m_graph.AddEdge(0, 25, 8);
            m_graph.AddEdge(10, 32, 7);
            m_graph.AddEdge(21, 40, 6);
            m_graph.AddEdge(33, 49, 5);
        }
        else
        {
            m_graph = ::DijkstraDeliverySystem::Graph(8, false);
            m_graph.AddEdge(0, 1, 4);
            m_graph.AddEdge(0, 2, 3);
            m_graph.AddEdge(1, 2, 1);
            m_graph.AddEdge(1, 3, 2);
            m_graph.AddEdge(2, 4, 6);
            m_graph.AddEdge(3, 4, 1);
            m_graph.AddEdge(3, 5, 5);
            m_graph.AddEdge(4, 6, 2);
            m_graph.AddEdge(5, 6, 1);
            m_graph.AddEdge(5, 7, 7);
            m_graph.AddEdge(6, 7, 3);
            m_expectedDistance = 12;
        }

        GenerateNodePositions(m_graph.VertexCount());
    }

    void MainWindow::SetInputsForTestCase(int testCase)
    {
        auto sourceNodeBox = SourceNodeBox();
        auto targetNodeBox = TargetNodeBox();

        if (sourceNodeBox == nullptr || targetNodeBox == nullptr)
        {
            return;
        }

        if (testCase == 3)
        {
            sourceNodeBox.Text(L"0");
            targetNodeBox.Text(L"5");
        }
        else if (testCase == 2)
        {
            sourceNodeBox.Text(L"0");
            targetNodeBox.Text(L"4");
        }
        else if (testCase == 4)
        {
            sourceNodeBox.Text(L"2");
            targetNodeBox.Text(L"2");
        }
        else if (testCase == 5)
        {
            sourceNodeBox.Text(L"0");
            targetNodeBox.Text(L"49");
        }
        else
        {
            sourceNodeBox.Text(L"0");
            targetNodeBox.Text(L"7");
        }
    }

    int MainWindow::SelectedTestCase()
    {
        auto testCaseBox = TestCaseBox();
        if (testCaseBox == nullptr)
        {
            return 1;
        }

        const int index = static_cast<int>(testCaseBox.SelectedIndex());
        return (index >= 0) ? (index + 1) : 1;
    }

    void MainWindow::GenerateNodePositions(int vertexCount)
    {
        m_nodePositions.clear();
        m_nodePositions.reserve(static_cast<size_t>(vertexCount));

        constexpr double pi = 3.14159265358979323846;
        constexpr float centerX = 360.0f;
        constexpr float centerY = 200.0f;
        constexpr float radius = 160.0f;

        if (vertexCount <= 0)
        {
            return;
        }

        for (int i = 0; i < vertexCount; ++i)
        {
            const double angle = (2.0 * pi * static_cast<double>(i)) / static_cast<double>(vertexCount);
            const float x = centerX + static_cast<float>(std::cos(angle) * radius);
            const float y = centerY + static_cast<float>(std::sin(angle) * radius);
            m_nodePositions.emplace_back(x, y);
        }
    }

    void MainWindow::TestCaseBox_SelectionChanged(IInspectable const&, SelectionChangedEventArgs const&)
    {
        if (!m_isUiReady)
        {
            return;
        }

        auto sourceNodeBox = SourceNodeBox();
        auto targetNodeBox = TargetNodeBox();
        auto statusTextBlock = StatusTextBlock();
        if (sourceNodeBox == nullptr || targetNodeBox == nullptr || statusTextBlock == nullptr)
        {
            return;
        }

        const int testCase = SelectedTestCase();
        LoadGraphByTestCase(testCase);
        SetInputsForTestCase(testCase);
        RenderGraph({});
        statusTextBlock.Text(L"Test case loaded. Press Run Dijkstra to evaluate.");
    }

    void MainWindow::StartHighlightAnimation(UIElement const& element, int delayMs, bool forNode)
    {
        using namespace Microsoft::UI::Xaml::Media::Animation;
        using namespace std::chrono;

        element.Opacity(0.30);

        const auto begin = duration_cast<Windows::Foundation::TimeSpan>(milliseconds(delayMs));
        const auto duration = duration_cast<Windows::Foundation::TimeSpan>(forNode ? milliseconds(520) : milliseconds(380));

        DoubleAnimation animation{};
        animation.From(0.30);
        animation.To(1.0);
        animation.BeginTime(begin);
        animation.Duration(DurationHelper::FromTimeSpan(duration));

        Storyboard storyboard{};
        Storyboard::SetTarget(animation, element);
        Storyboard::SetTargetProperty(animation, L"Opacity");
        storyboard.Children().Append(animation);

        m_runningAnimations.push_back(storyboard);
        m_runningAnimations.back().Begin();
    }

    void MainWindow::StartNodePulseAndShadow(UIElement const& element, int delayMs, Windows::UI::Color const& glowColor, NodeAnimationRole role)
    {
        using namespace Microsoft::UI::Xaml::Hosting;
        using namespace std::chrono;

        const auto baseVisual = ElementCompositionPreview::GetElementVisual(element);
        const auto compositor = baseVisual.Compositor();

        baseVisual.CenterPoint(float3{ 18.0f, 18.0f, 0.0f });

        const auto delay = duration_cast<Windows::Foundation::TimeSpan>(milliseconds(delayMs));

        float peakScale = 1.06f;
        int32_t pulseIterations = 1;
        float shadowOpacityValue = 0.28f;
        float shadowBlur = 14.0f;

        auto pulseDuration = duration_cast<Windows::Foundation::TimeSpan>(milliseconds(560));
        auto shadowDuration = duration_cast<Windows::Foundation::TimeSpan>(milliseconds(1000));

        if (role == NodeAnimationRole::Source)
        {
            peakScale = 1.10f;
            pulseIterations = 2;
            shadowOpacityValue = 0.38f;
            shadowBlur = 18.0f;
            pulseDuration = duration_cast<Windows::Foundation::TimeSpan>(milliseconds(900));
            shadowDuration = duration_cast<Windows::Foundation::TimeSpan>(milliseconds(1800));
        }
        else if (role == NodeAnimationRole::Target)
        {
            peakScale = 1.16f;
            pulseIterations = 2;
            shadowOpacityValue = 0.58f;
            shadowBlur = 24.0f;
            pulseDuration = duration_cast<Windows::Foundation::TimeSpan>(milliseconds(620));
            shadowDuration = duration_cast<Windows::Foundation::TimeSpan>(milliseconds(1500));
        }

        auto pulse = compositor.CreateVector3KeyFrameAnimation();
        pulse.InsertKeyFrame(0.0f, float3{ 1.0f, 1.0f, 1.0f });
        pulse.InsertKeyFrame(0.5f, float3{ peakScale, peakScale, 1.0f });
        pulse.InsertKeyFrame(1.0f, float3{ 1.0f, 1.0f, 1.0f });
        pulse.DelayTime(delay);
        pulse.Duration(pulseDuration);
        pulse.IterationCount(pulseIterations);
        baseVisual.StartAnimation(L"Scale", pulse);

        auto shadow = compositor.CreateDropShadow();
        shadow.Color(glowColor);
        shadow.BlurRadius(shadowBlur);
        shadow.Opacity(shadowOpacityValue);

        auto shadowVisual = compositor.CreateSpriteVisual();
        shadowVisual.Size(float2{ 36.0f, 36.0f });
        shadowVisual.Shadow(shadow);
        shadowVisual.Opacity(0.0f);

        auto shadowOpacityAnimation = compositor.CreateScalarKeyFrameAnimation();
        shadowOpacityAnimation.InsertKeyFrame(0.0f, 0.0f);
        shadowOpacityAnimation.InsertKeyFrame(0.30f, 1.0f);
        shadowOpacityAnimation.InsertKeyFrame(1.0f, 0.0f);
        shadowOpacityAnimation.DelayTime(delay);
        shadowOpacityAnimation.Duration(shadowDuration);

        shadowVisual.StartAnimation(L"Opacity", shadowOpacityAnimation);
        ElementCompositionPreview::SetElementChildVisual(element, shadowVisual);
    }

    void MainWindow::SolveButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        const int testCase = SelectedTestCase();
        const int source = ParseIntOrDefault(SourceNodeBox().Text(), 0);
        const int target = ParseIntOrDefault(TargetNodeBox().Text(), m_graph.VertexCount() - 1);
        int iterations = ParseIntOrDefault(IterationBox().Text(), 30);
        if (iterations <= 0)
        {
            iterations = 30;
        }

        if (source < 0 || source >= m_graph.VertexCount() || target < 0 || target >= m_graph.VertexCount())
        {
            RenderGraph({});
            StatusTextBlock().Text(L"Invalid input: source or target is out of range for the current graph.");
            return;
        }

        try
        {
            const auto result = m_solver.Solve(m_graph, source, target, iterations);
            RenderGraph(result.Path, source, target);
            StatusTextBlock().Text(BuildResultText(result, source, target, testCase));
        }
        catch (std::exception const& ex)
        {
            RenderGraph({});
            StatusTextBlock().Text(winrt::to_hstring(std::string("Invalid input: ") + ex.what()));
        }
    }

    void MainWindow::RenderGraph(std::vector<int> const& highlightedPath, int source, int target)
    {
        for (auto& storyboard : m_runningAnimations)
        {
            storyboard.Stop();
        }
        m_runningAnimations.clear();

        PathCanvas().Children().Clear();

        auto makeColor = [](uint8_t a, uint8_t r, uint8_t g, uint8_t b)
        {
            return winrt::Windows::UI::Color{ a, r, g, b };
        };

        auto edgeKey = [](int left, int right)
        {
            const int low = (std::min)(left, right);
            const int high = (std::max)(left, right);
            return (static_cast<long long>(low) << 32) | static_cast<unsigned int>(high);
        };

        std::unordered_set<long long> highlightedEdges;
        std::vector<bool> highlightedNodes(static_cast<size_t>(m_graph.VertexCount()), false);
        std::unordered_map<int, int> pathOrder;
        for (size_t i = 0; i < highlightedPath.size(); ++i)
        {
            const int node = highlightedPath[i];
            if (node >= 0 && node < m_graph.VertexCount())
            {
                highlightedNodes[static_cast<size_t>(node)] = true;
                pathOrder[node] = static_cast<int>(i);
            }

            if (i > 0)
            {
                highlightedEdges.insert(edgeKey(highlightedPath[i - 1], highlightedPath[i]));
            }
        }

        for (int from = 0; from < m_graph.VertexCount(); ++from)
        {
            for (const auto& [to, weight] : m_graph.Neighbors(from))
            {
                if (!m_graph.IsDirected() && from > to)
                {
                    continue;
                }

                const auto& start = m_nodePositions[static_cast<size_t>(from)];
                const auto& end = m_nodePositions[static_cast<size_t>(to)];
                const bool onPath = highlightedEdges.contains(edgeKey(from, to));

                Line line;
                line.X1(start.X);
                line.Y1(start.Y);
                line.X2(end.X);
                line.Y2(end.Y);
                line.Stroke(SolidColorBrush(onPath ? makeColor(255, 220, 38, 38) : makeColor(255, 148, 163, 184)));
                line.StrokeThickness(onPath ? 4.0 : 2.0);
                PathCanvas().Children().Append(line);

                if (onPath)
                {
                    const int fromOrder = pathOrder.contains(from) ? pathOrder[from] : 0;
                    const int toOrder = pathOrder.contains(to) ? pathOrder[to] : fromOrder;
                    const int delay = (std::min)(fromOrder, toOrder) * 120;
                    StartHighlightAnimation(line, delay, false);
                }

                TextBlock weightLabel;
                weightLabel.Text(winrt::to_hstring(weight));
                weightLabel.Style(Application::Current().Resources().Lookup(box_value(L"CaptionTextBlockStyle")).as<Style>());
                Canvas::SetLeft(weightLabel, (start.X + end.X) / 2.0 + 4.0);
                Canvas::SetTop(weightLabel, (start.Y + end.Y) / 2.0 - 16.0);
                PathCanvas().Children().Append(weightLabel);
            }
        }

        constexpr double nodeDiameter = 36.0;
        for (int node = 0; node < m_graph.VertexCount(); ++node)
        {
            const auto& position = m_nodePositions[static_cast<size_t>(node)];
            const bool onPath = highlightedNodes[static_cast<size_t>(node)];
            const bool isSource = (node == source);
            const bool isTarget = (node == target);
            const auto nodeColor = isSource
                ? makeColor(255, 34, 197, 94)
                : (isTarget
                    ? makeColor(255, 245, 158, 11)
                    : (onPath ? makeColor(255, 220, 38, 38) : makeColor(255, 14, 116, 144)));

            Microsoft::UI::Xaml::Shapes::Ellipse circle;
            circle.Width(nodeDiameter);
            circle.Height(nodeDiameter);
            circle.Fill(SolidColorBrush(nodeColor));
            circle.Stroke(SolidColorBrush(makeColor(255, 241, 245, 249)));
            circle.StrokeThickness(2.0);

            Canvas::SetLeft(circle, position.X - (nodeDiameter / 2.0));
            Canvas::SetTop(circle, position.Y - (nodeDiameter / 2.0));
            PathCanvas().Children().Append(circle);

            if (onPath || isSource || isTarget)
            {
                const int delay = pathOrder.contains(node) ? (pathOrder[node] * 120 + 60) : 0;
                StartHighlightAnimation(circle, delay, true);
                const auto role = isSource
                    ? NodeAnimationRole::Source
                    : (isTarget ? NodeAnimationRole::Target : NodeAnimationRole::Path);
                StartNodePulseAndShadow(circle, delay, nodeColor, role);
            }

            TextBlock nodeLabel;
            nodeLabel.Text(winrt::to_hstring(node));
            nodeLabel.Foreground(SolidColorBrush(makeColor(255, 255, 255, 255)));
            nodeLabel.Style(Application::Current().Resources().Lookup(box_value(L"BodyStrongTextBlockStyle")).as<Style>());
            Canvas::SetLeft(nodeLabel, position.X - 5.0);
            Canvas::SetTop(nodeLabel, position.Y - 10.0);
            PathCanvas().Children().Append(nodeLabel);
        }
    }

    int MainWindow::ParseIntOrDefault(hstring const& text, int fallback) const
    {
        try
        {
            const std::string utf8 = winrt::to_string(text);
            size_t parsedLength = 0;
            const int parsedValue = std::stoi(utf8, &parsedLength);
            return (parsedLength == utf8.size()) ? parsedValue : fallback;
        }
        catch (...)
        {
            return fallback;
        }
    }

    hstring MainWindow::BuildResultText(::DijkstraDeliverySystem::DijkstraResult const& result, int source, int target, int testCase) const
    {
        std::wstringstream stream;
        stream << L"Test case: TC" << testCase << L"\n";
        stream << L"Source: " << source << L", Target: " << target << L"\n";

        if (result.Found)
        {
            stream << L"Shortest distance: " << result.Distance << L"\n";
            stream << L"Path: ";

            for (size_t i = 0; i < result.Path.size(); ++i)
            {
                if (i > 0)
                {
                    stream << L" -> ";
                }
                stream << result.Path[i];
            }
            stream << L"\n";
        }
        else
        {
            stream << L"No path found.\n";
        }

        if (m_expectedDistance >= 0)
        {
            const bool pass = result.Found && result.Distance == m_expectedDistance;
            stream << L"Expected distance: " << m_expectedDistance
                   << L" (" << (pass ? L"PASS" : L"CHECK") << L")\n";
        }

        stream << L"\nLegend: source (subtle pulse), target (strong pulse), path node (light pulse), teal = normal node";
        stream << L"\n";
         stream << L"Benchmark adjacency list + heap (" << result.Benchmark.Iterations << L" runs): "
               << L"best=" << result.Benchmark.BestMicroseconds << L"us, "
               << L"avg=" << std::fixed << std::setprecision(2) << result.Benchmark.AverageMicroseconds << L"us, "
               << L"worst=" << result.Benchmark.WorstMicroseconds << L"us";

         stream << L"\nBenchmark adjacency matrix + linear scan (" << result.MatrixBenchmark.Iterations << L" runs): "
             << L"best=" << result.MatrixBenchmark.BestMicroseconds << L"us, "
             << L"avg=" << std::fixed << std::setprecision(2) << result.MatrixBenchmark.AverageMicroseconds << L"us, "
             << L"worst=" << result.MatrixBenchmark.WorstMicroseconds << L"us";

         stream << L"\nCross-check (distance equality): "
             << (result.CrossCheckPassed ? L"PASS" : L"MISMATCH");

        return hstring(stream.str());
    }
}
