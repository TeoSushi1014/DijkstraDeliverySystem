#include "pch.h"
#include "App.xaml.h"

#include <appmodel.h>
#include <MddBootstrap.h>
#include <wil/resource.h>

using namespace winrt;

namespace
{
    constexpr uint32_t WindowsAppSdkMajorMinorVersion = 0x00010008;

    bool IsPackagedProcess()
    {
        uint32_t packageFullNameLength = 0;
        const LONG result = ::GetCurrentPackageFullName(&packageFullNameLength, nullptr);
        return result != APPMODEL_ERROR_NO_PACKAGE;
    }
}

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    winrt::init_apartment(winrt::apartment_type::single_threaded);

    bool bootstrapInitialized = false;
    auto bootstrapShutdown = wil::scope_exit([&bootstrapInitialized]()
    {
        if (bootstrapInitialized)
        {
            ::MddBootstrapShutdown();
        }
    });

    if (!IsPackagedProcess())
    {
        PACKAGE_VERSION minVersion{};
        const HRESULT initializeResult = ::MddBootstrapInitialize2(
            WindowsAppSdkMajorMinorVersion,
            nullptr,
            minVersion,
            MddBootstrapInitializeOptions_None);

        if (FAILED(initializeResult))
        {
            return static_cast<int>(initializeResult);
        }

        bootstrapInitialized = true;
    }

    winrt::Microsoft::UI::Xaml::Application::Start(
        [](auto&&)
        {
            make<winrt::DijkstraDeliverySystem::implementation::App>();
        });

    return 0;
}
