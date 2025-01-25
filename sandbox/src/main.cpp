#include <ProjectZvend/Memory.hpp>
#include <ProjectZvend/Stopwatch.hpp>


int main()
{
    auto scanner = PZvend::Memory::Scanner("kernel32.dll");

    PZvend::Stopwatch watch(10.0f);

    auto result = scanner.FindAll("55", PZvend::Memory::RDATA);
    auto time   = watch.GetElapsedTime();

    printf("Scanner = %p\n", (void*)&scanner);
    printf("Results = %d (Time: %f)\n", result.size(), time);

    return 0;
}
