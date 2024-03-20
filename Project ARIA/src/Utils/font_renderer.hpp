#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

class FontManager
{
	sf::Font m_font{};
	sf::Text text{};
	sf::RenderWindow* m_window;

public:
	FontManager(sf::RenderWindow* window, const unsigned font_size, const std::string& font_loc) : m_window(window)
	{
		if (!m_font.loadFromFile(font_loc))
		{
			std::cout << "[ERROR]: failed to load font \n";
			return;
		}

		text.setCharacterSize(font_size);
		text.setFont(m_font);
		text.setFillColor(sf::Color::White);
	}

	void draw(const sf::Vector2f& position, const std::string& string_text, const bool centered = false)
	{
		text.setString(string_text);


		// Calculate the position for centering the text
		const sf::FloatRect textBounds = text.getLocalBounds();

		sf::Vector2f textPosition = position;
		if (centered)
			textPosition = { position.x - textBounds.width / 2, position.y - textBounds.height / 2 };


		text.setPosition(textPosition);


		m_window->draw(text);
	}

};
