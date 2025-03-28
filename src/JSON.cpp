#include <ProjectZvend/JSON.hpp>
#include <ProjectZvend/Paths.hpp>



namespace
{
    bool SaveJSON(
        const std::string&                      filepath,
        const nlohmann::ordered_json&           json,
        const int                               indent,
        const char                              indent_char,
        const bool                              ensure_ascii,
        const nlohmann::detail::error_handler_t error_handler)
    {
        if (filepath.empty())
            return false;

        if (!PZvend::Path::Create(filepath))
            return false;

        std::ofstream savefile(filepath);
        if (!savefile.is_open())
            return false;

        try
        {
            savefile << json.dump(indent, indent_char, ensure_ascii, error_handler);
            savefile.close();
        }
        catch (const nlohmann::json::exception&)
        {
            return false;
        }

        return true;
    }
}


PZvend::JSON::JSON()
{}

PZvend::JSON::JSON(const std::string& filepath) : m_Filepath(filepath)
{
    Path::Create(filepath);

    if (Path::Exist(filepath))
        Load(filepath);
}

bool PZvend::JSON::Load(const std::string& filepath)
{
    if (!filepath.ends_with(".json"))
        return false;

    std::ifstream loadfile(filepath);
    if (loadfile.is_open())
    {
        try
        {
            loadfile >> m_Json;
            loadfile.close();
        }
        catch (const nlohmann::json::exception& e)
        {
            loadfile.close();
            throw e;
            return false;
        }

        m_Filepath = filepath;
        return true;
    }

    return false;
}

bool PZvend::JSON::Reload()
{
    if (m_Filepath.empty())
        return false;

    std::ifstream loadfile(m_Filepath);
    if (!loadfile.is_open())
        return false;

    try
    {
        loadfile >> m_Json;
        loadfile.close();
    }
    catch (const nlohmann::json::exception&)
    {
        return false;
    }

    return true;
}

bool PZvend::JSON::Save(
    const int indent,
    const char indent_char,
    const bool ensure_ascii,
    const ErrorHandler error_handler)
{
    return SaveJSON(m_Filepath, m_Json, indent, indent_char, ensure_ascii, error_handler);
}

bool PZvend::JSON::SaveTo(const std::string& filepath,
                          const int          indent,
                          const char         indent_char,
                          const bool         ensure_ascii,
                          const ErrorHandler error_handler)
{
    return SaveJSON(filepath, m_Json, indent, indent_char, ensure_ascii, error_handler);
}
