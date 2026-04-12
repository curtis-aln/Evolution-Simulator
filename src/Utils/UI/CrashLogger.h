#pragma once

#include <fstream>
#include <chrono>
#include <format>
#include <filesystem>
#include <string>
#include <vector>

#if defined(__cpp_lib_stacktrace) && __cpp_lib_stacktrace >= 202011L
#include <stacktrace>
#endif

#ifdef _WIN32
#include <Windows.h>
#endif

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/Event.hpp>

#ifndef PROJECT_ROOT
#define PROJECT_ROOT "."
#endif


// ============================================================
//  Settings — tweak everything here
// ============================================================

struct CrashLoggerSettings
{
    // --- file settings ---
    inline static constexpr bool        write_to_file = true;
    inline static constexpr bool        write_to_console = true;
    inline static const std::string     log_filename = "crash.log";
    inline static const std::string     log_directory = std::string(PROJECT_ROOT);

    // --- stack trace settings ---
    inline static constexpr bool        show_stack_trace = true;
    inline static constexpr bool        your_frames_only = true;    // filter out system/runtime frames
    inline static constexpr bool        show_system_frame_count = true;    // show count of omitted system frames
    inline static constexpr bool        show_full_file_path = false;   // false = src\file.cpp, true = full path
    inline static const std::string     source_root = std::string(PROJECT_ROOT);

    // --- error window settings ---
    inline static constexpr bool        show_error_window = true;
    inline static constexpr unsigned    window_width = 700;
    inline static constexpr unsigned    window_height = 480;
    inline static const std::string     window_title = "Project ARIA - Crash";
    inline static const std::string     font_path = "media/fonts/Roboto-Regular.ttf";

    // --- window colours ---
    inline static const sf::Color       window_background = sf::Color(30, 30, 30);
    inline static const sf::Color       color_title = sf::Color(220, 60, 60);
    inline static const sf::Color       color_message = sf::Color(255, 255, 255);
    inline static const sf::Color       color_info = sf::Color(160, 160, 160);
    inline static const sf::Color       color_build = sf::Color(120, 120, 120);
    inline static const sf::Color       color_stack_yours = sf::Color(180, 220, 100);
    inline static const sf::Color       color_footer = sf::Color(100, 100, 100);

    // --- font sizes ---
    inline static constexpr unsigned    font_size_title = 20;
    inline static constexpr unsigned    font_size_message = 14;
    inline static constexpr unsigned    font_size_info = 13;
    inline static constexpr unsigned    font_size_build = 12;
    inline static constexpr unsigned    font_size_stack = 12;
    inline static constexpr unsigned    font_size_footer = 12;
};


// ============================================================
//  CrashLogger
// ============================================================

class CrashLogger : public CrashLoggerSettings
{
    inline static auto program_start_ = std::chrono::steady_clock::now();

public:

    // ----------------------------------------------------------
    // Call once at the top of main() before anything else.
    // Registers a Windows SEH filter so hardware exceptions
    // (null deref, access violations, stack overflows) are caught
    // and reported with a full stack trace at the crash site.
    // ----------------------------------------------------------
    static void set_exception_translator()
    {
#ifdef _WIN32
        SetUnhandledExceptionFilter([](EXCEPTION_POINTERS* info) -> LONG
            {
                std::string reason;
                switch (info->ExceptionRecord->ExceptionCode)
                {
                case EXCEPTION_ACCESS_VIOLATION:
                    reason = "Access Violation (null pointer dereference or invalid memory access)";
                    break;
                case EXCEPTION_STACK_OVERFLOW:
                    reason = "Stack Overflow (infinite recursion or oversized stack allocation)";
                    break;
                case EXCEPTION_INT_DIVIDE_BY_ZERO:
                    reason = "Integer Divide by Zero";
                    break;
                case EXCEPTION_ILLEGAL_INSTRUCTION:
                    reason = "Illegal Instruction";
                    break;
                case EXCEPTION_IN_PAGE_ERROR:
                    reason = "In-Page Error (failed to load page from disk)";
                    break;
                default:
                    reason = std::format("Hardware Exception (code: {:#010x})",
                        static_cast<unsigned>(info->ExceptionRecord->ExceptionCode));
                    break;
                }

#if defined(__cpp_lib_stacktrace) && __cpp_lib_stacktrace >= 202011L
                const std::string stack = format_stack(std::stacktrace::current());
#else
                const std::string stack = "(unavailable — requires C++23)\n";
#endif
                if (write_to_file)
                    write_log(reason, "Hardware exception — see stack trace", stack);

                if (show_error_window)
                    show_window(reason, "Hardware exception — see stack trace", stack);

                return EXCEPTION_EXECUTE_HANDLER;
            });
#endif
    }


    // ----------------------------------------------------------
    // Call from catch blocks in main()
    // ----------------------------------------------------------

    static void handle(const std::exception& e)
    {
        handle_impl("std::exception", e.what());
    }

    static void handle()
    {
        handle_impl("Unknown exception", "No details available.");
    }


private:

    static void handle_impl(const std::string& reason, const std::string& detail)
    {
        if (write_to_console)
            std::cout << "[CRASH] " << reason << ": " << detail << "\n";

#if defined(__cpp_lib_stacktrace) && __cpp_lib_stacktrace >= 202011L
        const std::string stack = format_stack(std::stacktrace::current());
#else
        const std::string stack = "(unavailable — requires C++23)\n";
#endif

        if (write_to_file)
            write_log(reason, detail, stack);

        if (show_error_window)
            show_window(reason, detail, stack);
    }


    // ----------------------------------------------------------
    // Time helpers
    // ----------------------------------------------------------

    static std::string get_timestamp()
    {
        auto now = std::chrono::system_clock::now();
        return std::format("{:%Y-%m-%d %H:%M:%S}", now);
    }

    static std::string get_uptime()
    {
        auto elapsed = std::chrono::steady_clock::now() - program_start_;
        auto h = std::chrono::duration_cast<std::chrono::hours>(elapsed);
        auto m = std::chrono::duration_cast<std::chrono::minutes>(elapsed % std::chrono::hours(1));
        auto s = std::chrono::duration_cast<std::chrono::seconds>(elapsed % std::chrono::minutes(1));
        return std::format("{}h {}m {}s", h.count(), m.count(), s.count());
    }


    // ----------------------------------------------------------
    // Build info
    // ----------------------------------------------------------

    static std::string get_build_info()
    {
        std::string compiler;
#if defined(_MSC_VER)
        compiler = "MSVC " + std::to_string(_MSC_VER);
#elif defined(__clang__)
        compiler = "Clang " + std::to_string(__clang_major__) + "." + std::to_string(__clang_minor__);
#elif defined(__GNUC__)
        compiler = "GCC " + std::to_string(__GNUC__) + "." + std::to_string(__GNUC_MINOR__);
#else
        compiler = "Unknown compiler";
#endif
        return std::format("Compiled: {} {}  |  C++{}  |  {}",
            __DATE__, __TIME__, __cplusplus, compiler);
    }


    // ----------------------------------------------------------
    // Stack trace formatting — shared by both C++ and SEH paths
    // ----------------------------------------------------------

#if defined(__cpp_lib_stacktrace) && __cpp_lib_stacktrace >= 202011L
    static std::string format_stack(const std::stacktrace& trace)
    {
        if (!show_stack_trace)
            return "(stack trace disabled in CrashLoggerSettings)\n";

        std::vector<std::string> your_frames;
        std::vector<std::string> sys_frames;

        for (const auto& frame : trace)
        {
            const std::string file = frame.source_file();
            const std::string desc = frame.description();

            // skip the logger's own frames
            if (desc.find("CrashLogger") != std::string::npos) continue;
            if (desc.find("write_crash_log") != std::string::npos) continue;
            if (desc.find("handle_impl") != std::string::npos) continue;
            if (desc.find("format_stack") != std::string::npos) continue;

            const bool is_yours = !file.empty() &&
                file.find(source_root) != std::string::npos;

            std::string entry;
            if (!file.empty())
            {
                std::string display_file = file;
                if (!show_full_file_path)
                {
                    const size_t src_pos = file.find("src");
                    if (src_pos != std::string::npos)
                        display_file = file.substr(src_pos);
                }
                entry = std::format("  {:<50} line {}", display_file, frame.source_line());
            }
            else
            {
                entry = std::format("  {}", desc);
            }

            if (is_yours) your_frames.push_back(entry);
            else          sys_frames.push_back(entry);
        }

        std::string result;

        result += "--- Your Code (most recent call first) ---\n";
        if (your_frames.empty())
            result += "  (none found — ensure /Zi and /DEBUG flags are set in CMake)\n";
        else
            for (const auto& f : your_frames)
                result += f + "\n";

        if (!your_frames_only)
        {
            result += "\n--- System Frames ---\n";
            for (const auto& f : sys_frames)
                result += f + "\n";
        }
        else if (show_system_frame_count && !sys_frames.empty())
        {
            result += std::format("\n  ({} system/runtime frames omitted)\n", sys_frames.size());
        }

        return result;
    }
#endif


    // ----------------------------------------------------------
    // File writing
    // ----------------------------------------------------------

    static void write_log(const std::string& reason, const std::string& detail,
        const std::string& stack)
    {
        const std::string path = log_directory + "/" + log_filename;
        std::filesystem::create_directories(
            std::filesystem::path(path).parent_path());

        std::ofstream log(path);
        if (!log.is_open()) return;

        if (write_to_console)
            std::cout << "Crash log written to: "
            << std::filesystem::absolute(path) << "\n";

        log << "==================== CRASH REPORT ====================\n";
        log << "Time     : " << get_timestamp() << "\n";
        log << "Uptime   : " << get_uptime() << "\n";
        log << "Type     : " << reason << "\n";
        log << "Message  : " << detail << "\n";
        log << "\n--- Build Info ---\n";
        log << get_build_info() << "\n";
        log << "\n--- Stack Trace ---\n";
        log << stack;
        log << "\n=======================================================\n";
    }


    // ----------------------------------------------------------
    // Error window
    // ----------------------------------------------------------

    static void show_window(const std::string& reason, const std::string& detail,
        const std::string& stack)
    {
        sf::RenderWindow window(
            sf::VideoMode({ window_width, window_height }),
            window_title
        );

        sf::Font font;
        
        if (!font.openFromFile(font_path))
        {
            std::cout << "Failed to load font for crash window: " << font_path << "\n"
				<< "Ensure the path is correct and the file is accessible.\n";
        }

        sf::Text t_title(font, "Project ARIA has crashed", font_size_title);
        sf::Text t_reason(font, "Type    : " + reason, font_size_message);
        sf::Text t_detail(font, "Error   : " + detail, font_size_message);
        sf::Text t_uptime(font, "Uptime  : " + get_uptime(), font_size_info);
        sf::Text t_time(font, "Crashed : " + get_timestamp(), font_size_info);
        sf::Text t_build(font, get_build_info(), font_size_build);
        sf::Text t_stack(font, stack, font_size_stack);
        sf::Text t_footer(font, "Full details saved to: " + log_directory + "/" + log_filename,
            font_size_footer);

        t_title.setFillColor(color_title);
        t_reason.setFillColor(color_message);
        t_detail.setFillColor(color_message);
        t_uptime.setFillColor(color_info);
        t_time.setFillColor(color_info);
        t_build.setFillColor(color_build);
        t_stack.setFillColor(color_stack_yours);
        t_footer.setFillColor(color_footer);

        // auto-stack positions with configurable gap
        float y = 15.f;
        auto place = [&](sf::Text& t, float gap_after = 5.f)
            {
                t.setPosition({ 20.f, y });
                y += t.getLocalBounds().size.y + gap_after;
            };

        place(t_title, 15.f);
        place(t_reason, 5.f);
        place(t_detail, 15.f);
        place(t_uptime, 3.f);
        place(t_time, 15.f);
        place(t_build, 15.f);
        place(t_stack, 15.f);
        place(t_footer);

        while (window.isOpen())
        {
            while (const std::optional event = window.pollEvent())
                if (event->is<sf::Event::Closed>())
                    window.close();

            window.clear(window_background);
            window.draw(t_title);
            window.draw(t_reason);
            window.draw(t_detail);
            window.draw(t_uptime);
            window.draw(t_time);
            window.draw(t_build);
            window.draw(t_stack);
            window.draw(t_footer);
            window.display();
        }
    }
};