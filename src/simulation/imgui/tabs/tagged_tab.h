#pragma once
#include "i_tab.h"
#include "../helpers/toast_system.h"
#include <unordered_set>
#include <string>

class TaggedTab : public ITab
{
public:
    const char* label() const override { return "Tagged"; }
    void        draw(UIContext& ctx)   override;

    void toggle_tag(int id);
    bool is_tagged(int id) const;
    void notify_death(int id, const std::string& cause);
    void draw_toasts(float dt);  // called from ControlPanel every frame

private:
    std::unordered_set<int> m_tagged_ids_;
    ToastSystem             m_toasts_;

    void draw_tag_input(UIContext& ctx);
    void draw_list(UIContext& ctx);
};