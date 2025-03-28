#include <ProjectZvend/JSON.hpp>
#include <ProjectZvend/Logger.hpp>
#include <ProjectZvend/Memory.hpp>
#include <ProjectZvend/Paths.hpp>
#include <ProjectZvend/Stopwatch.hpp>

static auto g_Logger = PZvend::CreateLogger("Sandbox", true, ".");

using log_hello_FUNC = void(*)(const std::string&);
static PZvend::Memory::THook<log_hello_FUNC> g_Hook;

void log_hello(const std::string& name)
{
    g_Logger->info("Hello {}", name);
}

void hook_log_hello(const std::string& name)
{
    if (name == "TAZBEAST")
        return;

    g_Hook.Call(name);
}


int main()
{
    PZvend::Memory::Initialize();

    auto scanner = PZvend::Memory::Scanner("kernel32.dll");

    PZvend::Stopwatch watch(10.0f);

    auto result = scanner.FindAll("55", PZvend::Memory::RDATA);
    auto time   = watch.GetElapsedTime();

    g_Logger->info("Scanner = {}", fmt::ptr(&scanner));
    g_Logger->info("Results = {} (Time: {})", result.size(), time);

    PZvend::JSON json("./Hello/config.json");
    std::vector<uint32_t> list = { 21, 42, 84, 168 };
    json.Set(42, "The", "Answer", "To", "Everything");
    json.Set(list, "The", "Result", "Nobody", "Wanted");
    json.Save();


    PZvend::JSON json2("./Hello/config.json");

    uint32_t magic_number = 0;
    if (!json2.Get(magic_number, "The", "Answer", "To", "Everything"))
    {
        g_Logger->error("MagicNumber does not exist in the JSON.");
    }

    std::vector<uint32_t> out_list;
    if (!json2.Get(out_list, "The", "Result", "Nobody", "Wanted"))
    {
        g_Logger->error("The List does not exist in the JSON.");
    }

    g_Logger->info("Magic Number = {}", magic_number);
    for (const auto& val : out_list)
    {
        g_Logger->info("List Value = {}", val);
    }


    g_Logger->info("BEFORE:");
    log_hello("Steven");
    log_hello("TAZBEAST");
    log_hello("Svend");
    log_hello("Kotomi");

    g_Hook = PZvend::Memory::THook<log_hello_FUNC>("LogHello", log_hello, hook_log_hello);
    g_Hook.Create();
    g_Hook.Enable();

    g_Logger->info("AFTER:");
    log_hello("Steven");
    log_hello("TAZBEAST");
    log_hello("Svend");
    log_hello("Kotomi");
    g_Hook.Remove();

    return 0;
}
