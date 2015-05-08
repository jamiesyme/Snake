#pragma once

#include <string>
namespace sf { class Window; class Event; }


class Window {
public:
	Window(const int xSize, const int ySize, const std::string& title);
	~Window();
	
	void display();
	bool pollEvent(sf::Event& e);

private:
	sf::Window* m_handle;
};
