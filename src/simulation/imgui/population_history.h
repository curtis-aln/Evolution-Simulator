#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <numeric>

#include "imgui.h"   // ImVec4


struct PopulationEvent
{
    float       time = 0.f;
    std::string label = {};
    ImVec4      color = { 1.f, 0.8f, 0.2f, 1.f };
};

struct PopulationSample
{
    float time = 0.f;
    int   protozoa = 0;
    int   food = 0;
    float avg_gen = 0.f;
};
struct PopulationHistory
{
    static constexpr size_t k_max_samples = 160384;
    static constexpr int    k_spike_threshold = 20;

    std::vector<float> time = {};
    std::vector<float> protozoa = {};
    std::vector<float> food = {};
    std::vector<float> total = {};
    std::vector<float> avg_gen = {};

    std::vector<PopulationEvent> events = {};

    void push(float t, int p, int f, float gen)
    {
        if (time.size() >= k_max_samples)
            _evict();

        const int prev_p = protozoa.empty() ? p : static_cast<int>(protozoa.back());
        const bool on_cooldown = !events.empty() && (t - events.back().time) < 5.f;
        const int  delta = std::abs(p - prev_p);

        time.push_back(t);
        protozoa.push_back(static_cast<float>(p));
        food.push_back(static_cast<float>(f));
        total.push_back(static_cast<float>(p + f));
        avg_gen.push_back(gen);

        if (!on_cooldown && delta > k_spike_threshold)
        {
            const bool  die_off = p < prev_p;
            std::string lbl = die_off
                ? "die-off (-" + std::to_string(delta) + ")"
                : "boom (+" + std::to_string(delta) + ")";
            ImVec4 col = die_off
                ? ImVec4{ 1.f, 0.3f, 0.3f, 1.f }
            : ImVec4{ 0.3f, 1.f, 0.5f, 1.f };
            events.push_back({ t, std::move(lbl), col });
        }
    }

    void add_manual_event(float t, std::string label,
        ImVec4 col = ImVec4{ 1.f, 0.8f, 0.2f, 1.f })
    {
        events.push_back({ t, std::move(label), col });
    }

    void export_csv(const std::string& path) const
    {
        std::ofstream f(path);
        f << "time,protozoa,food,total,avg_gen\n";
        for (size_t i = 0; i < time.size(); ++i)
            f << time[i] << ','
            << protozoa[i] << ','
            << food[i] << ','
            << total[i] << ','
            << avg_gen[i] << '\n';
    }

    size_t size() const { return time.size(); }

private:
    void _evict()
    {
        auto drop = [](std::vector<float>& v) {
            if (!v.empty()) v.erase(v.begin());
            };
        drop(time); drop(protozoa); drop(food);
        drop(total); drop(avg_gen);
    }
};