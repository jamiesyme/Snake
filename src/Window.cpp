#include "Window.hpp"



Window::Window(const int xSize, const int ySize, const std::string& title)
  : m_handle(sf::VideoMode(xSize, ySize), title)
{
}



Window::~Window()
{
}



void Window::display()
{
	m_handle.display();
}



bool Window::pollEvent(sf::Event& e)
{
	return m_handle.pollEvent(e);
}
