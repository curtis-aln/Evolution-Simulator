#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <unordered_map>
#include <sstream>
#include <iomanip>

#include "../../settings.h"

template<typename T>
class StatisticVector {
public:
    std::vector<std::pair<std::string, T*>> statistics;

    void add_statistic(const std::string& name, T* statistic) {
        statistics.emplace_back(name, statistic);
    }
};

class TextBox
{
    sf::RenderWindow* m_render_window_ = nullptr;
    sf::Rect<float> m_border_{};
    Font m_title_font_;
    Font m_text_font_;
    TextPacket m_title_{};
    sf::RectangleShape m_render_border_{};

    std::unordered_map<std::string, StatisticVector<int>> int_stats_;
    std::unordered_map<std::string, StatisticVector<unsigned>> unsigned_stats_;
    std::unordered_map<std::string, StatisticVector<bool>> bool_stats_;
    std::unordered_map<std::string, StatisticVector<float>> float_stats_;
    std::unordered_map<std::string, StatisticVector<double>> double_stats_;
    std::unordered_map<std::string, StatisticVector<std::string>> string_stats_;

public:
    TextBox(const sf::Rect<float>& border = {}, sf::RenderWindow* render_window = nullptr) :
        m_render_window_(render_window),
        m_border_(border),
        m_title_font_(render_window, TextSettings::title_font_size, TextSettings::bold_font_location),
        m_text_font_(render_window, TextSettings::regular_font_size, TextSettings::regular_font_location)
    {
        // Reserve capacity for statistics
        int_stats_.reserve(20);
        unsigned_stats_.reserve(20);
        bool_stats_.reserve(20);
        float_stats_.reserve(20);
        double_stats_.reserve(20);
        string_stats_.reserve(20);
    }

    void render()
    {
        render_border();
        render_stats();
    }

    template<typename T>
    void add_statistic(const std::string& type_name, const std::string& name, T* statistic)
    {
        if constexpr (std::is_same_v<T, int>) {
            int_stats_[type_name].add_statistic(name, statistic);
        }
        else if constexpr (std::is_same_v<T, unsigned>) {
            unsigned_stats_[type_name].add_statistic(name, statistic);
        }
        else if constexpr (std::is_same_v<T, bool>) {
            bool_stats_[type_name].add_statistic(name, statistic);
        }
        else if constexpr (std::is_same_v<T, float>) {
            float_stats_[type_name].add_statistic(name, statistic);
        }
        else if constexpr (std::is_same_v<T, double>) {
            double_stats_[type_name].add_statistic(name, statistic);
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            string_stats_[type_name].add_statistic(name, statistic);
        }
    }

    void set_title(const std::string& text)
    {
        const sf::Vector2f border_pos = m_border_.getPosition();
        const sf::Vector2f border_size = m_border_.getSize();
        const sf::Vector2f text_size = m_title_font_.get_text_size(text);
        const sf::Vector2f position = { border_pos.x + border_size.x / 2.f, border_pos.y + text_size.y };

        m_title_ = TextPacket(position, text, 0, true);
    }

    void init_graphics(const sf::Color fill_color, const sf::Color outline_color = {}, const float outline_thickness = 0)
    {
        m_render_border_.setPosition(m_border_.getPosition());
        m_render_border_.setSize(m_border_.getSize());
        m_render_border_.setFillColor(fill_color);
        m_render_border_.setOutlineColor(outline_color);
        m_render_border_.setOutlineThickness(outline_thickness);
    }

private:
    void render_border()
    {
        m_render_window_->draw(m_render_border_);
        m_title_font_.draw(m_title_);
    }

    void render_stats()
    {
        const sf::Vector2f buffer = { 20, m_title_font_.get_text_size(m_title_.text).y * 2 };

        float currentY = buffer.y;

        currentY = render_statistic_type(int_stats_, buffer, currentY);
        currentY = render_statistic_type(unsigned_stats_, buffer, currentY);
        currentY = render_statistic_type(bool_stats_, buffer, currentY);
        currentY = render_statistic_type(float_stats_, buffer, currentY);
        currentY = render_statistic_type(double_stats_, buffer, currentY);
        currentY = render_statistic_type(string_stats_, buffer, currentY);
    }

    template<typename T>
    float render_statistic_type(std::unordered_map<std::string, StatisticVector<T>>& stat_map, const sf::Vector2f& buffer, float startY)
    {
        float currentY = startY;

        for (const auto& [type_name, stats] : stat_map) {
            for (const auto& [name, statistic] : stats.statistics) {
                const sf::Vector2f size = m_text_font_.get_text_size(name);
                const sf::Vector2f pos = { m_border_.left + buffer.x, m_border_.top + currentY };
                std::stringstream ss;
                ss << name << ": ";

                if constexpr (std::is_same_v<T, bool>) {
                    ss << ((*statistic) ? "true" : "false");
                }
                else {
                    ss << std::fixed << std::setprecision(2) << (*statistic);
                }

                m_text_font_.draw(pos, ss.str());
                currentY += size.y; // Move to the next line
            }
        }

        // Add some padding after each type of statistics
        currentY += 10;

        return currentY;
    }
};
