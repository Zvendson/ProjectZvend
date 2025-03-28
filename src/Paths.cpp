#include "ProjectZvend/Paths.hpp"

bool PZvend::Path::Create(const std::string& path)
{
    bool result = true;
    std::filesystem::path dirpath = std::filesystem::absolute(path);

    if (dirpath.has_extension())
        dirpath = dirpath.parent_path();

    try
    {
        if (!std::filesystem::exists(dirpath))
            result = std::filesystem::create_directories(dirpath);
    }
    catch (const std::filesystem::filesystem_error&)
    {
        result = false;
    }

    return result;
}



bool PZvend::Path::Exist(const std::string& path)
{
    std::filesystem::path dirpath(path);

    try
    {
        return std::filesystem::exists(dirpath);
    }
    catch (const std::filesystem::filesystem_error&)
    {
        return false;
    }
}



std::string PZvend::Path::GetModulePath(HMODULE hModule)
{
    char path[MAX_PATH];

    if (0 == GetModuleFileNameA(hModule, path, MAX_PATH))
    {
        return "";
    }

    return std::filesystem::path(path).parent_path().string();
}
