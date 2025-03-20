class Camera
{
public:
    float m_currentScroll = 1.f;
    float m_targetScroll = 1.f;  // Target zoom level
    float m_zoom_velocity = 0.f; // Controls the smoothness of zooming

    sf::View m_view_{};

private:
    sf::RenderWindow* m_window_ptr_ = nullptr;
    sf::Vector2f m_mouse_position_before_{};
    sf::Vector2f m_delta_{};
    float m_zoom_strength_ = 0.08f;
    float m_smooth_zoom_speed = 5.0f;  // Adjust smoothness

public:
    explicit Camera(sf::RenderWindow* window_ptr, const float scale = 1.f)
        : m_window_ptr_(window_ptr)
    {
        const auto dims = sf::Vector2f(m_window_ptr_->getSize());
        m_view_ = sf::View({ 0, 0, dims.x, dims.y });

        m_currentScroll *= scale;
        m_targetScroll = m_currentScroll;
        zoom(0);
    }

    

    sf::Vector2f get_world_mouse_pos() const
    {
        return map_window_position_to_world_position(sf::Mouse::getPosition(*m_window_ptr_));
    }

    template<typename T>
    sf::Vector2f map_window_position_to_world_position(const sf::Vector2<T> window_position) const
    {
        return m_window_ptr_->mapPixelToCoords(static_cast<sf::Vector2i>(window_position), m_view_);
    }

    void translate(sf::Vector2f delta = { 0.f, 0.f })
    {
        if (delta.x == 0.f && delta.y == 0.f)
        {
            delta = m_delta_;
        }

        // Invert the signs of the components to move in the correct direction
        const sf::Vector2f final_translation = { -delta.x / m_currentScroll, -delta.y / m_currentScroll };
        m_view_.move(final_translation);

        update_window_view();
    }

    void update(float deltaTime)
    {
        const auto mouse_pos = sf::Vector2f(sf::Mouse::getPosition(*m_window_ptr_));
        m_delta_ = mouse_pos - m_mouse_position_before_;
        m_mouse_position_before_ = mouse_pos;

        // Smooth zoom
        smooth_zoom(deltaTime);
    }

    void zoom(float delta_scroll)
    {
        const sf::Vector2f before_mouse_pos = m_window_ptr_->mapPixelToCoords(sf::Mouse::getPosition(*m_window_ptr_), m_view_);

        float scale = get_zoom_scale(delta_scroll);
        m_targetScroll *= scale;  // Set the target zoom level

        const sf::Vector2f after_mouse_pos = m_window_ptr_->mapPixelToCoords(sf::Mouse::getPosition(*m_window_ptr_), m_view_);
        const sf::Vector2f offset = before_mouse_pos - after_mouse_pos;

        m_view_.move(offset);
    }

    void smooth_zoom(float deltaTime)
    {
        if (std::abs(m_currentScroll - m_targetScroll) < 0.001f)
            return; // Prevent unnecessary updates

        // Get the mouse world position before zoom
        const sf::Vector2f before_mouse_pos = get_world_mouse_pos();

        // Interpolate zoom velocity
        m_zoom_velocity = (m_targetScroll - m_currentScroll) * m_smooth_zoom_speed;

        // Apply velocity
        m_currentScroll += m_zoom_velocity * deltaTime;

        // Update the view size
        m_view_.setSize(m_window_ptr_->getSize().x / m_currentScroll, m_window_ptr_->getSize().y / m_currentScroll);

        // Get the mouse world position after zoom
        const sf::Vector2f after_mouse_pos = get_world_mouse_pos();

        // Adjust the view to compensate for the shift in mouse position
        const sf::Vector2f offset = before_mouse_pos - after_mouse_pos;
        m_view_.move(offset);

        update_window_view();
    }

    void update_window_view() const
    {
        m_window_ptr_->setView(m_view_);
    }

private:
    [[nodiscard]] float get_zoom_scale(float delta) const
    {
        return delta > 0 ? 1.0f + m_zoom_strength_ : 1.0f - m_zoom_strength_;
    }
};
