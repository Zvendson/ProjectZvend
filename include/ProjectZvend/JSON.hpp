#pragma once

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include <type_traits>

namespace PZvend
{

    class JSON
    {
    public:
        using ErrorHandler = nlohmann::detail::error_handler_t;

        JSON();
        JSON(const std::string& filepath);



        /*
        Loads a new json file. If a new path is assigned, it will update the path internally.
        */
        bool Load(const std::string& filepath);



        /*
        Reloads the json file from the current path.
        */
        bool Reload();



        /*
        Saves the json file.
        */
        bool Save(const int          indent        = 4,
                  const char         indent_char   = ' ',
                  const bool         ensure_ascii  = false,
                  const ErrorHandler error_handler = ErrorHandler::strict);



        /*
        Saves the json file to another path. (Does not update the path internally.)
        */
        bool SaveTo(const std::string& filepath,
                    const int          indent        = 4,
                    const char         indent_char   = ' ',
                    const bool         ensure_ascii  = false,
                    const ErrorHandler error_handler = ErrorHandler::strict);



        template <typename T>
        bool Get(T& out_data, const std::string& key) const
        {
            if (m_Json.contains(key))
            {
                try
                {
                    out_data = m_Json[key].get<T>();
                    return true;
                }
                catch (nlohmann::detail::type_error e)
                {
                    SPDLOG_ERROR("[JSON] {}", e.what());
                    return false;
                }
            }

            SPDLOG_WARN("[JSON] Key '{}' does not exist.", key);
            return false;
        }



        template <typename T, typename... Keys>
        bool Get(T& out_data, const std::string& key, const Keys&... keys) const
        {
            return IGet<T>(m_Json, out_data, key, keys...);
        }



        template <typename T>
        void Set(const std::string& key, const T& value)
        {
            m_Json[key] = value;
        }



        template <typename T, typename... Keys>
        void Set(const T& value, const std::string& key, const Keys&... keys)
        {
            ISet(m_Json, value, key, keys...);
        }

    private:
        template <typename T>
        static bool IGet(const nlohmann::ordered_json& json, T& out_data, const std::string& key)
        {
            if (json.contains(key))
            {
                try
                {
                    out_data = json[key].get<T>();
                    return true;
                }
                catch (nlohmann::detail::type_error e)
                {
                    SPDLOG_ERROR("[JSON] {}", e.what());
                    return false;
                }
            }

            SPDLOG_WARN("[JSON] Key '{}' does not exist.", key);
            return false;
        }



        template <typename T, typename... Keys>
        static bool IGet(const nlohmann::ordered_json& json, T& out_data, const std::string& key, const Keys&... keys)
        {
            if (json.contains(key))
                return IGet<T>(json[key], out_data, keys...);

            SPDLOG_WARN("[JSON] Key '{}' does not exist.", key);
            return false;
        }



        template <typename T>
        static void ISet(nlohmann::ordered_json& json, const T& value, const std::string& key)
        {
            json[key] = value;
        }



        template <typename T, typename... Keys>
        static void ISet(nlohmann::ordered_json& json, const T& value, const std::string& key, const Keys&... keys)
        {
            // If the key does not exist, create a new nested object
            if (!json.contains(key))
            {
                json[key] = nlohmann::ordered_json{};  // Create a nested object if key doesn't exist
            }

            ISet(json[key], value, keys...);
        }

    private:
        std::string            m_Filepath;
        nlohmann::ordered_json m_Json;
    };

}