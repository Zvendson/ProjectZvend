#pragma once

#include <functional>
#include <wtypes.h>



namespace PZvend
{
    namespace Memory
    {

/*************\
*    Types    *
\*************/
        using DllModule    = HMODULE;
        using ScanCallback = std::function<uint8_t* (uint8_t* address)>;

        enum ScanSection : uint16_t
        {
            TEXT,  // Contains the executable code of the program or library.
            RDATA, // Stores read-only data, such as constants or string literals.
            DATA,  // Contains initialized data, which is readable and writable.
            BSS,   // Contains uninitialized data. BSS (Block Started by Symbol) doesn't exist in the file itself but is allocated in memory during execution.
            EDATA, // Contains export directory information for functions and variables that the DLL exports for use by other modules.
            IDATA, // Stores import directory information, listing external functions or variables that the DLL imports from other modules.
            RELOC, // Contains relocation information, which is used to adjust memory addresses when the file is loaded into memory.
            RSRC,  // Contains resources like icons, bitmaps, dialogs, and other user interface elements.
            TLS,   // Contains thread-local storage data for variables that have unique values for each thread.
            PDATA, // Contains exception handling data, particularly for 64-bit Windows platforms.
            DEBUG, // Stores debug information (if any) such as symbol tables and line number information, used for debugging the executable or library.
            MAX
        };

        struct SectionData
        {
            /* 0x0000 */ uint8_t* Start = nullptr;
            /* 0x0004 */ uint8_t* End   = nullptr;
        };



/*************\
*  Functions  *
\*************/
        /*
        !! Spaces are required that the combo-pattern will work !!
        @enhancement: allow spaces

        Examples of combo pattern:
            1. "55 48 89 E5 48 C7 05 ? ? ? ? 69 00 00 00 48 89 EC C3"
            2. "55 48 89 E5 48 C7 05 * * * * 69 00 00 00 48 89 EC C3"
            3. "55 48 89 E5 48 C7 05 _ _ _ _ 69 00 00 00 48 89 EC C3"
            4. "55 48 89 E5 48 C7 05 ?? ?? ?? ?? 69 00 00 00 48 89 EC C3"
            5. "55 48 89 E5 48 C7 05 ** ** ** ** 69 00 00 00 48 89 EC C3"
            6. "55 48 89 E5 48 C7 05 __ __ __ __ 69 00 00 00 48 89 EC C3"
            7. "55 48 89 E5 48 C7 05 ?? ** __ ?* 69 00 00 00 48 89 EC C3"  // Not recommended but technical possible.
        */
        [[nodiscard]] uint64_t ConvertComboPattern(const char* combo, uint8_t* out_bytes, char* out_mask) noexcept;


        /*
        Dereferences E8, E9 and EB instructions and retrieves the address. It checks for nested JMP instructions.
        */
        [[nodiscard]] uint8_t* DereferenceCall(uint8_t* call_addr) noexcept;


        /*
        Dereferences E8, E9 and EB instructions and retrieves the address. It checks for nested JMP instructions.
        */
        template<class T>
        [[nodiscard]] T DereferenceCall(uint8_t* call_addr) noexcept
        {
            return reinterpret_cast<T>(DereferenceCall(call_addr));
        }


        /*
        Dereferences an pointer address from target address.
        */
        [[nodiscard]] uint8_t* DereferencePointer(uint8_t* address) noexcept;


        /*
        Dereferences an pointer address from target address.
        */
        template<class T>
        [[nodiscard]] inline T DereferencePointer(uint8_t* address) noexcept
        {
            return reinterpret_cast<T>(DereferencePointer(address));
        }



/*************\
*   Classes   *
\*************/
        class Scanner
        {
        public:
            Scanner(const char* modulename);

            [[nodiscard]] uint8_t* operator[](size_t RVA) const noexcept
            {
                return reinterpret_cast<uint8_t*>(m_Module) + RVA;
            }


            /*
            Find a specicifc pattern based on the mask in the desired module's section.
            An offset can be applied when pattern is found.
            */
            template<class T = uint8_t*>
            [[nodiscard]] T Find(const uint8_t* pattern, const char* mask, ScanSection section, int32_t offset = 0x0000) const noexcept
            {
                uint8_t* address = IFind(pattern, mask, m_Sections[section].Start, m_Sections[section].End);

                return reinterpret_cast<T>(address ? address + offset : address);
            }
            /*
            Find a specicifc pattern based on the mask between a start and end address.
            It will scan backwards, if start address is bigger than its end address.
            An offset can be applied when pattern is found.
            */
            template<class T = uint8_t*>
            [[nodiscard]] T Find(const uint8_t* pattern, const char* mask, uint8_t* start, uint8_t* end, int32_t offset = 0x0000) const noexcept
            {
                uint8_t* address = IFind(pattern, mask, start, end);

                return reinterpret_cast<T>(address ? address + offset : address);
            }


            /*
            Find a specific combo-pattern in the desired module's section.
            An offset can be applied when pattern is found.
            */
            template<class T = uint8_t*, size_t maxLen = 256>
            [[nodiscard]] T Find(const char* combo, ScanSection section, int32_t offset = 0x0000) const noexcept
            {
                uint8_t pattern[maxLen];
                char    mask[maxLen];

                if (!ConvertComboPattern(combo, pattern, mask))
                    return reinterpret_cast<T>(nullptr);

                uint8_t* address = IFind(pattern, mask, m_Sections[section].Start, m_Sections[section].End);

                return reinterpret_cast<T>(address ? address + offset : address);
            }
            /*
            Find a specific combo-pattern between a start and end address.
            It will scan backwards, if start address is bigger than its end address.
            An offset can be applied when pattern is found.
            */
            template<class T = uint8_t*, size_t maxLen = 256>
            [[nodiscard]] T Find(const char* combo, uint8_t* start, uint8_t* end, int32_t offset = 0x0000) const noexcept
            {
                uint8_t pattern[maxLen];
                char    mask[maxLen];

                if (!ConvertComboPattern(combo, pattern, mask))
                    return reinterpret_cast<T>(nullptr);

                uint8_t* address = IFind(pattern, mask, start, end);

                return reinterpret_cast<T>(address ? address + offset : address);
            }


            /*
            Find a specific combo-pattern in the desired module's section.
            Allows a callback to trigger when the pattern has been found, so you can do pointer arithmetic on it.
            */
            template<class T = uint8_t*, size_t maxLen = 256>
            [[nodiscard]] T Find(const char* combo, ScanSection section, ScanCallback scan_cb) const noexcept
            {
                uint8_t pattern[maxLen];
                char    mask[maxLen];

                if (!ConvertComboPattern(combo, pattern, mask))
                    return reinterpret_cast<T>(nullptr);

                uint8_t* address = IFind(pattern, mask, m_Sections[section].Start, m_Sections[section].End);
                return reinterpret_cast<T>(scan_cb ? scan_cb(address) : address);
            }
            /*
            Find a specific combo-pattern between a start and end address.
            It will scan backwards, if start address is bigger than its end address.
            Allows a callback to trigger when the pattern has been found, so you can do pointer arithmetic on it.
            */
            template<class T = uint8_t*, size_t maxLen = 256>
            [[nodiscard]] T Find(const char* combo, uint8_t* start, uint8_t* end, ScanCallback scan_cb) const noexcept
            {
                uint8_t pattern[maxLen];
                char    mask[maxLen];

                if (!ConvertComboPattern(combo, pattern, mask))
                    return reinterpret_cast<T>(nullptr);

                uint8_t* address = IFind(pattern, mask, start, end);
                return reinterpret_cast<T>(scan_cb ? scan_cb(address) : address);
            }


            /*
            Find a specicifc pattern based on the mask in the desired module's section.
            Allows a callback to trigger when the pattern has been found, so you can do pointer arithmetic on it.
            */
            template<class T = uint8_t*>
            [[nodiscard]] T Find(const uint8_t* pattern, const char* mask, ScanSection section, ScanCallback scan_cb) const noexcept
            {
                uint8_t* address = IFind(pattern, mask, m_Sections[section].Start, m_Sections[section].End);
                return reinterpret_cast<T>(scan_cb ? scan_cb(address) : address);
            }
            /*
            Find a specicifc pattern based on the mask between a start and end address.
            It will scan backwards, if start address is bigger than its end address.
            Allows a callback to trigger when the pattern has been found, so you can do pointer arithmetic on it.
            */
            template<class T = uint8_t*>
            [[nodiscard]] T Find(const uint8_t* pattern, const char* mask, uint8_t* start, uint8_t* end, ScanCallback scan_cb) const noexcept
            {
                uint8_t* address = IFind(pattern, mask, start, end);
                return reinterpret_cast<T>(scan_cb ? scan_cb(address) : address);
            }


            /*
            Finds all occurrences of a specicifc combo-pattern in the desired module's section.
            */
            template<class T = uint8_t*, size_t maxLen = 256>
            [[nodiscard]] std::vector<T> FindAll(const char* combo, ScanSection section) const noexcept
            {
                uint8_t pattern[maxLen];
                char    mask[maxLen];

                if (!ConvertComboPattern(combo, pattern, mask))
                    return {};

                std::vector<uint8_t*> result = IFindAll(pattern, mask, m_Sections[section].Start, m_Sections[section].End);

                std::vector<T> out(result.size());
                for (size_t i = 0; i < result.size(); ++i)
                    out[i] = reinterpret_cast<T>(result[i]);

                return out;
            }
            /*
            Finds all occurrences of a specific combo-pattern between a start and end address.
            It will scan backwards, if start address is bigger than its end address.
            */
            template<class T = uint8_t*, size_t maxLen = 256>
            [[nodiscard]] std::vector<T> FindAll(const char* combo, uint8_t* start, uint8_t* end) const noexcept
            {
                uint8_t pattern[maxLen];
                char    mask[maxLen];

                if (!ConvertComboPattern(combo, pattern, mask))
                    return {};

                std::vector<uint8_t*> result = IFindAll(pattern, mask, start, end);

                std::vector<T> out(result.size());
                for (size_t i = 0; i < result.size(); ++i)
                    out[i] = reinterpret_cast<T>(result[i]);

                return out;
            }


            /*
            Finds all occurrences of a specicifc pattern based on the mask in the desired module's section.
            */
            template<class T = uint8_t*>
            [[nodiscard]] std::vector<T> FindAll(const uint8_t* pattern, const char* mask, ScanSection section) const noexcept
            {
                std::vector<uint8_t*> result = IFindAll(pattern, mask, m_Sections[section].Start, m_Sections[section].End);

                std::vector<T> out(result.size());
                for (size_t i = 0; i < result.size(); ++i)
                    out[i] = reinterpret_cast<T>(result[i]);

                return out;
            }
            /*
            Finds all occurrences of a specicifc pattern based on the mask between a start and end address.
            It will scan backwards, if start address is bigger than its end address.
            */
            template<class T = uint8_t*>
            [[nodiscard]] std::vector<T> FindAll(const uint8_t* pattern, const char* mask, uint8_t* start, uint8_t* end) const noexcept
            {
                std::vector<uint8_t*> result = IFindAll(pattern, mask, start, end);

                std::vector<T> out(result.size());
                for (size_t i = 0; i < result.size(); ++i)
                    out[i] = reinterpret_cast<T>(result[i]);

                return out;
            }


        protected: /* (I)nternal functions */
            [[nodiscard]] uint8_t* IFind(const uint8_t* pattern, const char* mask, uint8_t* start, uint8_t* end) const noexcept;
            [[nodiscard]] std::vector<uint8_t*> IFindAll(const uint8_t* pattern, const char* mask, uint8_t* start, uint8_t* end) const noexcept;


        protected: /* Variables */
            /* 0x0000 */ SectionData m_Sections[ScanSection::MAX] = {};
            /* 0x0008 */ DllModule   m_Module                     = nullptr;
        };
    }
}

