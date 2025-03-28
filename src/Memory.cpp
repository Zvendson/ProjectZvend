#include "ProjectZvend/Memory.hpp"

#include "Macros.hpp"

#include <DbgHelp.h>
#include <Psapi.h>
#include <spdlog/spdlog.h>


namespace
{
    static bool g_MemoryInitialized = false;
}


bool PZvend::Memory::Initialize()

{
    if (g_MemoryInitialized)
        return true;

    auto mh_status = MH_Initialize();
    if (mh_status != MH_OK)
    {
        SPDLOG_CRITICAL("Memory could not be initialized, because: {}.", MH_StatusToString(mh_status));
        return false;
    }

    g_MemoryInitialized = true;
    return true;
}



uint64_t PZvend::Memory::ConvertComboPattern(const char* combo, uint8_t* out_bytes, char* out_mask) noexcept
{
    char lastChar = ' ';
    uint32_t bytesLength = 0;

    for (unsigned int i = 0; i < strlen(combo); i++)
    {
        if ((combo[i] == '?' || combo[i] == '*' || combo[i] == '_') &&
            (lastChar != '?' && lastChar != '*' && lastChar != '_'))
        {
            out_bytes[bytesLength] = '\0';
            out_mask[bytesLength] = '?';
            bytesLength++;
        }

        else if (isspace(lastChar))
        {
            char repr = static_cast<uint8_t>(strtol(&combo[i], 0, 16));

            out_bytes[bytesLength] = repr;
            out_mask[bytesLength] = 'x';

            bytesLength++;
        }

        lastChar = combo[i];
    }

    out_bytes[bytesLength] = '\0';
    out_mask[bytesLength]  = '\0';

    return bytesLength;
}



uint8_t* PZvend::Memory::DereferenceCall(uint8_t* call_addr) noexcept
{
    constexpr uint8_t CALL = 0xE8;
    constexpr uint8_t JMPL = 0xE9; // long jump
    constexpr uint8_t JMPS = 0xEB; // short jump

    if (!call_addr)
        return nullptr;

    uint8_t* address = nullptr;
    uint8_t  opcode  = *reinterpret_cast<uint8_t*>(call_addr);

    switch (opcode)
    {
        case CALL:
        case JMPL:
            {
                const auto offset = *reinterpret_cast<int32_t*>(call_addr + 0x0001);
                address = call_addr + 0x0005 + offset;
            }
            break;

        case JMPS:
                {
                const auto offset = *reinterpret_cast<uint8_t*>(call_addr + 0x0001);
                address = call_addr + 0x0002 + offset;
            }
                break;

        default: // Invalid opcode
            return nullptr;
    }

    // Check for nested JMPs or CALLs
    if (const auto nested_call = DereferenceCall(address))
        return nested_call;

    return address;
}



uint8_t* PZvend::Memory::DereferencePointer(uint8_t* address) noexcept
{
#ifdef PZVEND_IS_X32
    int32_t rva = *reinterpret_cast<uint32_t*>(address);
    return address + rva + 0x0004;
#else
    auto deref = *reinterpret_cast<uintptr_t*>(address);
    return reinterpret_cast<uint8_t*>(deref);
#endif
}



PZvend::Memory::Scanner::Scanner(const char* modulename)
{
    m_Module = static_cast<DllModule>(GetModuleHandleA(modulename));
    auto module_base = reinterpret_cast<uint64_t>(m_Module);

    IMAGE_NT_HEADERS*     nt_header      = ImageNtHeader(m_Module);
    IMAGE_SECTION_HEADER* section_header = reinterpret_cast<IMAGE_SECTION_HEADER*>(nt_header + 1);

    for (uint64_t i = 0; i < nt_header->FileHeader.NumberOfSections; i++)
    {
        auto name = reinterpret_cast<char*>(section_header->Name);

        uint8_t section = 0xFF;
        if (memcmp(name, ".text", 5) == 0)
            section = ScanSection::TEXT;

        else if (memcmp(name, ".rdata", 6) == 0)
            section = ScanSection::RDATA;

        else if (memcmp(name, ".data", 5) == 0)
            section = ScanSection::DATA;

        else if (memcmp(name, ".idata", 6) == 0)
            section = ScanSection::IDATA;

        else if (memcmp(name, ".reloc", 6) == 0)
            section = ScanSection::RELOC;

        else if (memcmp(name, ".pdata", 6) == 0)
            section = ScanSection::PDATA;

        else if (memcmp(name, ".bss", 4) == 0)
            section = ScanSection::BSS;

        else if (memcmp(name, ".edata", 6) == 0)
            section = ScanSection::EDATA;

        else if (memcmp(name, ".rsrc", 5) == 0)
            section = ScanSection::RSRC;

        else if (memcmp(name, ".tls", 4) == 0)
            section = ScanSection::TLS;

        else if (memcmp(name, ".debug", 6) == 0)
            section = ScanSection::DEBUG;

        if (section != 0xFF)
        {
            uint8_t* start = reinterpret_cast<uint8_t*>(module_base + section_header->VirtualAddress);
            uint8_t* end   = start + section_header->Misc.VirtualSize;

            m_Sections[section].Start = start;
            m_Sections[section].End   = end;
        }

        section_header++;
    }
}



uint8_t* PZvend::Memory::Scanner::IFind(const uint8_t* pattern, const char* mask, uint8_t* start, uint8_t* end) const noexcept
{
    uint64_t len = strlen(mask);
    end -= len;

    auto check_pattern = [&](uint8_t* addr) -> bool
    {
        if (*reinterpret_cast<uint8_t*>(addr) != pattern[0])
            return false;

        for (uint64_t index = 1; index < len; index++)
        {
            if (mask && mask[index] != 'x')
                continue;

            uint8_t byte = *reinterpret_cast<uint8_t*>(addr + index);
            if (byte != pattern[index])
                return false;
        }

        return true;
    };

    if (start > end) // Scan backwards
    {
        for (uint8_t* address = start; address >= end; address--)
            if (check_pattern(address))
                return address;
    }
    else             // Scan forwards
    {
        for (uint8_t* address = start; address < end; address++)
            if (check_pattern(address))
                return address;
    }

    return nullptr;
}



std::vector<uint8_t*> PZvend::Memory::Scanner::IFindAll(const uint8_t* pattern, const char* mask, uint8_t* start, uint8_t* end) const noexcept
{
    uint64_t len = strlen(mask);
    end -= len;
    std::vector<uint8_t*> results;

    auto check_pattern = [&](uint8_t* addr) -> bool
    {
        if (*reinterpret_cast<uint8_t*>(addr) != pattern[0])
            return false;

        for (uint64_t index = 1; index < len; index++)
        {
            if (mask && mask[index] != 'x')
                continue;

            uint8_t byte = *reinterpret_cast<uint8_t*>(addr + index);
            if (byte != pattern[index])
                return false;
        }

        return true;
    };

    if (start > end) // Scan backwards
    {
        for (uint8_t* address = start; address >= end; address--)
            if (check_pattern(address))
                results.push_back(address);
    }
    else             // Scan forwards
    {
        for (uint8_t* address = start; address < end; address++)
            if (check_pattern(address))
                results.push_back(address);
    }

    return results;
}


