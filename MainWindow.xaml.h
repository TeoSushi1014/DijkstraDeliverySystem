#pragma once

#include "Graph.h"
#include "DijkstraSolver.h"
#include "MainWindow.g.h"

#include <atomic>
#include <chrono>
#include <unordered_map>
#include <vector>

namespace winrt::DijkstraDeliverySystem::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();

        void TestCaseBox_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& args);
        winrt::fire_and_forget SolveButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
        void CancelButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);

    private:
        enum class NodeAnimationRole
        {
            Source,
            Target,
            Path,
        };

        void BuildSampleGraph();
        void LoadGraphByTestCase(int testCase);
        void SetInputsForTestCase(int testCase);
        int SelectedTestCase();
        void GenerateNodePositions(int vertexCount);
        void RenderGraph(std::vector<int> const& highlightedPath, int source = -1, int target = -1, bool enableEffects = true);
        void PathCanvas_SizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& args);
        void EnsureResizeTimer();
        void ResizeTimer_Tick(winrt::Microsoft::UI::Dispatching::DispatcherQueueTimer const& sender, winrt::Windows::Foundation::IInspectable const& args);
        void StartHighlightAnimation(winrt::Microsoft::UI::Xaml::UIElement const& element, int delayMs, bool forNode);
        void StartNodePulseAndShadow(winrt::Microsoft::UI::Xaml::UIElement const& element, int delayMs, winrt::Windows::UI::Color const& glowColor, NodeAnimationRole role);
        void SetInteractionEnabled(bool isEnabled);
        void UpdateProgressUI(int completedIterations, int totalIterations);
        void ResetProgressUI();
        winrt::Microsoft::UI::Xaml::Media::SolidColorBrush GetCachedBrush(winrt::Windows::UI::Color const& color);
        bool TryParseInt(winrt::hstring const& text, int& parsedValue) const;
        bool TryParseIterations(int& parsedValue);
        winrt::hstring BuildResultText(::DijkstraDeliverySystem::DijkstraResult const& result, int source, int target, int testCase) const;

        ::DijkstraDeliverySystem::Graph m_graph;
        ::DijkstraDeliverySystem::DijkstraSolver m_solver;
        std::vector<winrt::Windows::Foundation::Point> m_nodePositions;
        std::vector<int> m_lastHighlightedPath;
        std::vector<winrt::Microsoft::UI::Xaml::Media::Animation::Storyboard> m_runningAnimations;
        std::unordered_map<uint32_t, winrt::Microsoft::UI::Xaml::Media::SolidColorBrush> m_brushCache;
        winrt::Microsoft::UI::Xaml::Style m_captionTextStyle{ nullptr };
        winrt::Microsoft::UI::Xaml::Style m_bodyStrongTextStyle{ nullptr };
        winrt::Microsoft::UI::Dispatching::DispatcherQueueTimer m_resizeTimer{ nullptr };
        std::chrono::steady_clock::time_point m_lastResizeSignal{};
        int m_lastHighlightedSource{ -1 };
        int m_lastHighlightedTarget{ -1 };
        double m_lastCanvasWidth{ -1.0 };
        double m_lastCanvasHeight{ -1.0 };
        int m_expectedDistance{ -1 };
        bool m_isUiReady{ false };
        bool m_isSolving{ false };
        bool m_resizePending{ false };
        bool m_resizeNeedsFinalRender{ false };
        bool m_resizeActive{ false };
        std::atomic_bool m_cancelRequested{ false };
    };
}

namespace winrt::DijkstraDeliverySystem::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
