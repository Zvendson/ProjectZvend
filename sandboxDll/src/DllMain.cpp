#include <ProjectZvend/JSON.hpp>
#include <ProjectZvend/Logger.hpp>
#include <ProjectZvend/Memory.hpp>
#include <ProjectZvend/Paths.hpp>
#include <ProjectZvend/Stopwatch.hpp>


BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    auto logger = PZvend::CreateLogger("DllTest", true, ".");

    logger->info("Hello {}!", "World");
    return 1;
}