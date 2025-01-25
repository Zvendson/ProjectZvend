#include <ProjectZvend/Logger.hpp>
#include <ProjectZvend/Memory.hpp>
#include <ProjectZvend/Stopwatch.hpp>


int main()
{
    auto logger = PZvend::CreateLogger("Sandbox", true);

    auto scanner = PZvend::Memory::Scanner("kernel32.dll");

    PZvend::Stopwatch watch(10.0f);

    auto result = scanner.FindAll("55", PZvend::Memory::RDATA);
    auto time   = watch.GetElapsedTime();


    logger->info("Scanner = {}", fmt::ptr(&scanner));
    logger->info("Results = {} (Time: {})", result.size(), time);

    return 0;
}
