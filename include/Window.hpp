#pragma once

#include <string>
#include <SFML/Window.hpp>


class Window {
public:
	Window(const int xSize, const int ySize, const std::string& title);
	~Window();
	
	void display();
	bool pollEvent(sf::Event& e);

private:
	sf::Window m_handle;
};
