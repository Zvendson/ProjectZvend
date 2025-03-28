#pragma once


#include "ProjectZvend/Paths.hpp"

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>



namespace PZvend
{
    namespace LogColor
    {
        /* Windows colors (Linux has same?) */
        constexpr uint16_t Foreground_Black     = 0x0000;
        constexpr uint16_t Foreground_Blue      = 0x0001;
        constexpr uint16_t Foreground_Green     = 0x0002;
        constexpr uint16_t Foreground_Cyan      = 0x0003;
        constexpr uint16_t Foreground_Red       = 0x0004;
        constexpr uint16_t Foreground_Magenta   = 0x0005;
        constexpr uint16_t Foreground_Yellow    = 0x0006;
        constexpr uint16_t Foreground_White     = 0x0007;
        constexpr uint16_t Foreground_Intensity = 0x0008;

        constexpr uint16_t Foreground_Intense_Blue      = Foreground_Blue    | Foreground_Intensity;
        constexpr uint16_t Foreground_Intense_Green     = Foreground_Green   | Foreground_Intensity;
        constexpr uint16_t Foreground_Intense_Cyan      = Foreground_Cyan    | Foreground_Intensity;
        constexpr uint16_t Foreground_Intense_Red       = Foreground_Red     | Foreground_Intensity;
        constexpr uint16_t Foreground_Intense_Magenta   = Foreground_Magenta | Foreground_Intensity;
        constexpr uint16_t Foreground_Intense_Yellow    = Foreground_Yellow  | Foreground_Intensity;
        constexpr uint16_t Foreground_Intense_White     = Foreground_White   | Foreground_Intensity;

        constexpr uint16_t Background_Black     = 0x0000;
        constexpr uint16_t Background_Blue      = 0x0100;
        constexpr uint16_t Background_Green     = 0x0200;
        constexpr uint16_t Background_Cyan      = 0x0300;
        constexpr uint16_t Background_Red       = 0x0400;
        constexpr uint16_t Background_Magenta   = 0x0500;
        constexpr uint16_t Background_Yellow    = 0x0600;
        constexpr uint16_t Background_White     = 0x0700;
        constexpr uint16_t Background_Intensity = 0x0800;

        constexpr uint16_t Background_Intense_Blue      = Background_Blue    | Background_Intensity;
        constexpr uint16_t Background_Intense_Green     = Background_Green   | Background_Intensity;
        constexpr uint16_t Background_Intense_Cyan      = Background_Cyan    | Background_Intensity;
        constexpr uint16_t Background_Intense_Red       = Background_Red     | Background_Intensity;
        constexpr uint16_t Background_Intense_Magenta   = Background_Magenta | Background_Intensity;
        constexpr uint16_t Background_Intense_Yellow    = Background_Yellow  | Background_Intensity;
        constexpr uint16_t Background_Intense_White     = Background_White   | Background_Intensity;
    }

    /*
    Creates a pre defined logger for simplification.
    Creates sub directories if non existing.
    No checks for the path.

    @enhancement: verify path.
    */
    std::shared_ptr<spdlog::logger> CreateLogger(
        const std::string& name,
        bool               has_console   = false,
        const std::string& path          = "",
        std::size_t        max_file_size = 20 * 1024 * 1024,
        std::size_t        max_files     = 3)
    {
    #ifdef _DEBUG
        auto level = spdlog::level::debug;
    #else
        auto level = spdlog::level::info;
    #endif

        std::vector<spdlog::sink_ptr> sinks;

        if (has_console)
        {
            auto stdoutSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

            stdoutSink->set_color(spdlog::level::trace   , LogColor::Foreground_Intense_Cyan);
            stdoutSink->set_color(spdlog::level::debug   , LogColor::Foreground_Cyan);
            stdoutSink->set_color(spdlog::level::info    , LogColor::Foreground_Green);
            stdoutSink->set_color(spdlog::level::warn    , LogColor::Foreground_Intense_Yellow);
            stdoutSink->set_color(spdlog::level::err     , LogColor::Foreground_Intense_Red);
            stdoutSink->set_color(spdlog::level::critical, LogColor::Foreground_Intense_White | LogColor::Background_Red);

            stdoutSink->set_pattern("[%T][%n][%^%l%$] %v");
            stdoutSink->set_level(level);
            sinks.push_back(stdoutSink);
        }

        if (!path.empty())
        {
            std::tm     timeinfo;
            std::time_t current_time = std::time(nullptr);
            localtime_s(&timeinfo, &current_time);

            if (!Path::Create(path))
                throw std::exception("Path could not be created.");

            std::ostringstream oss;
            oss << path;
            if (!(path.ends_with('/')))
                oss << '/';
            oss << std::put_time(&timeinfo, "%Y-%m-%d.log");

            auto rotatingSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(oss.str(), max_file_size, max_files);
            rotatingSink->set_pattern("[%T][%n][%l] %v");
            rotatingSink->set_level(level);
            sinks.push_back(rotatingSink);
        }

        auto logger = std::make_shared<spdlog::logger>(name, sinks.begin(), sinks.end());
        logger->set_level(level);
        logger->flush_on(level);
        return logger;
    }
}