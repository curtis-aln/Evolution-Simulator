#include <iostream>
#include <fstream>   // std::ofstream
#include <chrono>    // std::chrono::system_clock
#include <format>    // std::format
#include "simulation/simulation.h"

int main()
{
    try
    {
        Simulation().run_simulation();
    }

    catch (const std::exception& e)
    {
        // write to crash log
        std::ofstream crash_log("crash.log");
        if (crash_log.is_open())
        {
            auto now = std::chrono::system_clock::now();
            crash_log << "[CRASH] " << std::format("{:%Y-%m-%d %H:%M:%S}", now) << "\n";
            crash_log << "Exception: " << e.what() << "\n";
        }

        // show error window
        sf::RenderWindow error_window(sf::VideoMode({ 600, 200 }), "Project ARIA - Crash");
        sf::Font font;
        font.openFromFile("media/fonts/Roboto-Regular.ttf");

        sf::Text title(font, "Project ARIA has crashed", 20);
        sf::Text message(font, e.what(), 15);
        sf::Text footer(font, "A crash.log file has been saved.", 13);

        title.setPosition({ 20, 20 });
        message.setPosition({ 20, 60 });
        footer.setPosition({ 20, 120 });

        title.setFillColor(sf::Color(220, 60, 60));
        message.setFillColor(sf::Color::White);
        footer.setFillColor(sf::Color(160, 160, 160));

        while (error_window.isOpen())
        {
            while (const std::optional event = error_window.pollEvent())
            {
                if (event->is<sf::Event::Closed>())
                    error_window.close();
            }
            error_window.clear(sf::Color(30, 30, 30));
            error_window.draw(title);
            error_window.draw(message);
            error_window.draw(footer);
            error_window.display();
        }
    }
    catch (...)
    {
        std::ofstream crash_log("crash.log");
        if (crash_log.is_open())
            crash_log << "[CRASH] Unknown exception — no details available.\n";
    }
}