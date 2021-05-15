#pragma once

#include "ProcessRunner.g.h"

namespace winrt::TestComponent::implementation
{
    struct ProcessRunner
    {
        static ProcessResult CallFileHandlingApp(hstring const& inputFilePath, hstring const& outputFileName);
    };
}
namespace winrt::TestComponent::factory_implementation
{
    struct ProcessRunner : ProcessRunnerT<ProcessRunner, implementation::ProcessRunner>
    {
    };
}
