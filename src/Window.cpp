#include "Window.hpp"
#include <SFML/Graphics.hpp>



Window::Window(const int xSize, const int ySize, const std::string& title)
{
	m_handle = new sf::RenderWindow();
	
	sf::VideoMode vidMode(800, 600);
	m_handle->create(vidMode, 
									 title, 
									 sf::Style::Default, 
									 sf::ContextSettings(24));
}



Window::~Window()
{
	delete m_handle;
}



void Window::display()
{
	m_handle->display();
}



bool Window::pollEvent(sf::Event& e)
{
	return m_handle->pollEvent(e);
}
