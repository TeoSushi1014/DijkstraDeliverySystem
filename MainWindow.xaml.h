#pragma once

#include "Graph.h"
#include "DijkstraSolver.h"
#include "MainWindow.g.h"

#include <atomic>
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
        void RenderGraph(std::vector<int> const& highlightedPath, int source = -1, int target = -1);
        void StartHighlightAnimation(winrt::Microsoft::UI::Xaml::UIElement const& element, int delayMs, bool forNode);
        void StartNodePulseAndShadow(winrt::Microsoft::UI::Xaml::UIElement const& element, int delayMs, winrt::Windows::UI::Color const& glowColor, NodeAnimationRole role);
        void SetInteractionEnabled(bool isEnabled);
        void UpdateProgressUI(int completedIterations, int totalIterations);
        void ResetProgressUI();
        bool TryParseInt(winrt::hstring const& text, int& parsedValue) const;
        bool TryParseIterations(int& parsedValue) const;
        winrt::hstring BuildResultText(::DijkstraDeliverySystem::DijkstraResult const& result, int source, int target, int testCase) const;

        ::DijkstraDeliverySystem::Graph m_graph;
        ::DijkstraDeliverySystem::DijkstraSolver m_solver;
        std::vector<winrt::Windows::Foundation::Point> m_nodePositions;
        std::vector<winrt::Microsoft::UI::Xaml::Media::Animation::Storyboard> m_runningAnimations;
        int m_expectedDistance{ -1 };
        bool m_isUiReady{ false };
        bool m_isSolving{ false };
        std::atomic_bool m_cancelRequested{ false };
    };
}

namespace winrt::DijkstraDeliverySystem::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
