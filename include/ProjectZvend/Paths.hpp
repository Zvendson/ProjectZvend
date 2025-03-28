#pragma once

#include <filesystem>
#include <string>
#include <wtypes.h>


namespace PZvend
{
    namespace Path
    {
        /*
        Creates the path with all its subdirectories.
        */
        bool Create(const std::string& path);



        /*
        Creates the path with all its subdirectories. Easier usage.
        */
        template <typename... Args>
        bool Create(const std::string& root, Args... args)
        {
            std::vector<std::string> parts = { args... };
            std::ostringstream oss;

            oss << root;

            for (size_t i = 0; i < parts.size(); ++i)
            {
                oss << "/" << parts[i];
            }

            return Create(oss.str());
        }



        /*
        Checks if the path exist.
        */
        bool Exist(const std::string& path);



        /*
        Gets the current module path.
        */
        std::string GetModulePath(HMODULE hModule);
    }
}