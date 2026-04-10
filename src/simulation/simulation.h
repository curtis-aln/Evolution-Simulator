#pragma once
#include <SFML/Graphics.hpp>
#include "../settings.h"
#include "../world/world.h"
#include "imgui/population_history.h"
#include "imgui/control_panel.h"

#include "../Utils/time.h"
#include "../Utils/fps_manager.h"
#include "../Utils/UI/Camera.hpp"

#include <imgui.h>
#include <imgui-SFML.h>
#include <implot.h>

class Simulation : SimulationSettings, TextSettings
{
    static sf::VideoMode getAdjustedVideoMode()
    {
        sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
        return sf::VideoMode({ (unsigned)(desktop.size.x * resize_shrinkage),
                               (unsigned)(desktop.size.y * resize_shrinkage) });
    }

    Font title_font{ nullptr, title_font_size, bold_font_location };
    Font regular_font{ nullptr, regular_font_size, regular_font_location };
    Font cell_statistic_font{ nullptr, cell_statistic_font_size, regular_font_location };

    sf::VideoMode   videoMode = full_screen ? sf::VideoMode::getDesktopMode() : getAdjustedVideoMode();
    std::uint32_t   windowStyle = sf::Style::None;
    sf::RenderWindow m_window_{ videoMode, "Project ARIA", windowStyle };

    FrameRateSmoothing<frame_smoothing> m_clock_{};
    Camera camera_{ &m_window_, 1.f };

    StopWatch m_delta_time_{};
    float    m_total_time_elapsed_ = 0.f;
    unsigned m_ticks_ = 0;

    World           m_world_{};
    PopulationHistory m_history_;
    ControlPanel    m_control_panel_;

    bool  m_rendering_ = true;
    bool  hide_panels = false;
    bool  open_extinction_window = false;
    bool  tracking = false;
    bool  m_tick_frame_time = false;
    bool  mouse_pressed_event = false;
    bool  running = true;
    float fps_ = 0.f;

    ImPlotColormap m_plot_colormap_{};

public:
    Simulation();
    void run_simulation();

private:
    void init_imGUI();
    void update_one_frame();
    void camera_follow_selected_protozoa();
    void update_line_graphs();
    void handle_imGUI();
    void extinction_popup();
    void render();
    void manage_frame_rate();

    // events
    void handle_events();
    bool try_select_protozoa(const sf::Vector2f& cam_pos);
    void handle_mouse_press(const sf::Vector2f& cam_pos);
    void handle_mouse_release();
    void handle_pause_toggle();
    void handle_keyboard_events(const sf::Keyboard::Key& event_key_code);
    void dispatch_event(const sf::Event& event, const sf::Vector2f& cam_pos);
};