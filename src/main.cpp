#include "Window.hpp"
#include <SFML/OpenGL.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <cstdlib>


class Tile {
public:
	static float size;
	float x;
	float y;
	float velX;
	float velY;
	float velDelay;
	std::string type;
	bool isActive;
	
	Tile()
	{
		x = 0.0f;
		y = 0.0f;
		const float velMax = 400.0f;
		velX = (float)rand() / (float)RAND_MAX * velMax - velMax / 2.0f;
		velY = -(float)rand() / (float)RAND_MAX * velMax;
		if (velX < velMax / 2.0f && velX >= 0)
			velX += velMax / 2.0f;
		if (velX > -velMax / 2.0f && velX <= 0)
			velX -= velMax / 2.0f;
		if (velY < velMax / 2.0f && velY >= 0)
			velY += velMax / 2.0f;
		if (velY > -velMax / 2.0f && velY <= 0)
			velY -= velMax / 2.0f;
		velDelay = (float)rand() / (float)RAND_MAX;
		velDelay *= velDelay * velDelay;
		velDelay = 0.2f + (1.0f - velDelay) * 2.0f;
		type = "empty";
		isActive = false;
	}
	
	void tick(float delta)
	{
		if (velDelay > 0.0f)
		{
			velDelay -= delta;
			return;
		}
		isActive = true;
		x += velX * delta;
		y += velY * delta;
		velY += 981.0f * delta;
	}
	void draw()
	{
		float mult = 1.0f;
		if (isActive)
			mult = 0.9f;
		glColor3f(0.9f * mult, 0.9f * mult, 0.9f * mult);
		glBegin(GL_TRIANGLES);
			glVertex2f(x       , y       );
			glVertex2f(x + size, y       );
			glVertex2f(x + size, y + size);
			glVertex2f(x + size, y + size);
			glVertex2f(x       , y + size);
			glVertex2f(x       , y       );
		glEnd();
		
		if (type.compare("empty") == 0) return;
		if (type.compare("body") == 0)  glColor3f(0.3f * mult, 0.3f * mult, 0.9f * mult);
		if (type.compare("head") == 0)  glColor3f(0.1f * mult, 0.1f * mult, 0.9f * mult);
		if (type.compare("food") == 0)  glColor3f(0.9f * mult, 0.3f * mult, 0.3f * mult);
		const float e = 2.0f;
		glBegin(GL_TRIANGLES);
			glVertex2f(x        + e, y        + e);
			glVertex2f(x + size - e, y        + e);
			glVertex2f(x + size - e, y + size - e);
			glVertex2f(x + size - e, y + size - e);
			glVertex2f(x        + e, y + size - e);
			glVertex2f(x        + e, y        + e);
		glEnd();
	}
};
float Tile::size;


class Tiles {
public:
	Tiles(int sizeX, int sizeY)
	{
		m_x = sizeX;
		m_y = sizeY;
		m_tiles = new Tile*[m_x];
		for (int x = 0; x < m_x; x++)
			m_tiles[x] = new Tile[m_y];
			
		for (int x = 0; x < m_x; x++) {
			for (int y = 0; y < m_y; y++) {
				Tile& t = m_tiles[x][y];
				t.x = (float)x * Tile::size;
				t.y = (float)y * Tile::size;
				t.type = "empty";
			}
		}
	}
	~Tiles()
	{
		delete[] m_tiles;
	}
	
	void tick(float delta)
	{
		for (int x = 0; x < m_x; x++)
			for (int y = 0; y < m_y; y++)
				m_tiles[x][y].tick(delta);
	}
	
	void draw()
	{
		for (int x = 0; x < m_x; x++)
			for (int y = 0; y < m_y; y++)
				m_tiles[x][y].draw();
	}
	
	Tile* getTile(int x, int y)
	{
		if (x < 0 || x >= m_x || y < 0 || y >= m_y)
			return 0;
		return &m_tiles[x][y];
	}
	
	void placeFood()
	{
		std::vector<int> emptyTiles;
		for (int x = 0; x < m_x; x++)
			for (int y = 0; y < m_y; y++)
				if (m_tiles[x][y].type.compare("empty") == 0)
				{
					emptyTiles.push_back(x);
					emptyTiles.push_back(y);
				}
				
		int random = rand() % ((int)emptyTiles.size() / 2) * 2;
		m_tiles[emptyTiles[random + 0]][emptyTiles[random + 1]].type = "food";
	}

private:
	Tile** m_tiles;
	int m_x;
	int m_y;
};


class SnakeBody {
public:
	SnakeBody(Tiles* tiles)
	{
		m_tiles = tiles;
	}
	void tick()
	{
		for (unsigned int i = 0; i < m_parts.size(); i++)
		{
			m_parts[i].ttl -= 1;
			if (m_parts[i].ttl < 0) {
				m_tiles->getTile(m_parts[i].x, m_parts[i].y)->type = "empty";
				m_parts.erase(m_parts.begin() + i--);
			}
		}
	}
	
	void add(int x, int y, int ttl)
	{
		Part p;
		p.x = x;
		p.y = y;
		p.ttl = ttl;
		m_tiles->getTile(x, y)->type = "body";
		m_parts.push_back(p);
	}
	
	int getLength()
	{
		return (int)m_parts.size();
	}
	
private:
	class Part {
	public:
		int x;
		int y;
		int ttl;
	};
	std::vector<Part> m_parts;
	Tiles* m_tiles;
};


class SnakeHead {
public:
	int x;
	int y;
	int dir;
	Tiles* tiles;
	SnakeBody* body;
	int bodySafeFor;
	
	
	SnakeHead(Tiles* tiles)
	{
		this->tiles = tiles;
		body = new SnakeBody(tiles);
		bodySafeFor = 3;
	}
	
	~SnakeHead() { delete body; }
	
	bool tick() // Returns 'if dead'
	{
		// Are we dead
		int futureX = x + getDirX();
		int futureY = y + getDirY();
		Tile* futureTile = tiles->getTile(futureX, futureY);
		if (futureTile == 0) {
			std::cout << "Died from wall." << std::endl;
			return true;
		}
		if (futureTile->type.compare("body") == 0) {
			std::cout << "Died from self-implosion." << std::endl;
			return true;
		}
			
		// We aren't dead, so extend our body
		Tile* lastTile = tiles->getTile(x, y);
		if (lastTile != 0) {
			lastTile->type = "empty";
			body->add(x, y, body->getLength());
		}
		
		// Check for food
		if (futureTile->type.compare("food") == 0)
		{
			bodySafeFor = 3;
			tiles->placeFood();
		}
		
		// Fix us
		futureTile->type = "head";
		x = futureX;
		y = futureY;
		
		// Update our body
		if (bodySafeFor > 0)
			bodySafeFor -= 1;
		else
			body->tick();
		
		// We're still alive
		return false;
	}
	int getDirX() { 
		if (dir == 0) return 1;
		if (dir == 1) return 0;
		if (dir == 2) return -1;
		if (dir == 3)	return 0;
	}
	int getDirY() { 
		if (dir == 0) return 0;
		if (dir == 1) return -1;
		if (dir == 2) return 0;
		if (dir == 3) return 1;
	}
	
	void setDir(int dir)
	{
		if (this->dir == 0 && dir == 2) return;
		if (this->dir == 1 && dir == 3) return;
		if (this->dir == 2 && dir == 0) return;
		if (this->dir == 3 && dir == 1) return;
		this->dir = dir;
	}
};


int main()
{
	// Create the window
	const int winSizeX = 800;
	const int winSizeY = 600;
	Window window(winSizeX, winSizeY, "Snake");
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	// Create the tiles and snake and food
	Tile::size = 50.0f;
	Tiles* tiles = new Tiles(winSizeX / Tile::size, winSizeY / Tile::size);
	SnakeHead* head = new SnakeHead(tiles);
	head->x = 1;
	head->y = 1;
	head->dir = 0;
	tiles->placeFood();
	
	
	// MAIN LOOP
	sf::Clock clock;
	float timeLastTicked = 0.0f;
	bool isRunning = true;
	bool isDead = false;
	int move2 = -1;
	int move1 = -1;
	float deltaForTick = 0.1f;
	while (isRunning)
	{
		float deltaTime = clock.getElapsedTime().asSeconds() - timeLastTicked;
		if (!isDead && deltaTime > deltaForTick) {
			timeLastTicked = clock.getElapsedTime().asSeconds();
			if (move1 != -1) {
				head->setDir(move1);
				move1 = -1;
			}
			if (move2 != -1) {
				move1 = move2;
				move2 = -1;
			}
			if (head->tick())
				isDead = true;
		}
		if (isDead) {
			tiles->tick(deltaTime);
			timeLastTicked = clock.getElapsedTime().asSeconds();
		}
	
		glClear(GL_COLOR_BUFFER_BIT);
		tiles->draw();
		window.display();
	
		sf::Event e;
		while (window.pollEvent(e))
		{
			if (e.type == sf::Event::Closed)
				isRunning = false;
			if (e.type == sf::Event::KeyPressed)
			{
				if (e.key.code == sf::Keyboard::Add)
					deltaForTick *= 0.9f;
				if (e.key.code == sf::Keyboard::Subtract)
					deltaForTick /= 0.9f;
				if (e.key.code == sf::Keyboard::Up) {
					if (move1 == -1)
						move1 = 1;
					else
					if (move2 == -1)
						move2 = 1;
				}
				if (e.key.code == sf::Keyboard::Down) {
					if (move1 == -1)
						move1 = 3;
					else
					if (move2 == -1)
						move2 = 3;
				}
				if (e.key.code == sf::Keyboard::Right) {
					if (move1 == -1)
						move1 = 0;
					else
					if (move2 == -1)
						move2 = 0;
				}
				if (e.key.code == sf::Keyboard::Left) {
					if (move1 == -1)
						move1 = 2;
					else
					if (move2 == -1)
						move2 = 2;
				}
				if (e.key.code == sf::Keyboard::R)
				{
					delete head;
					delete tiles;
					tiles = new Tiles(winSizeX / Tile::size, winSizeY / Tile::size);
					head = new SnakeHead(tiles);
					head->x = 1;
					head->y = 1;
					head->dir = 0;
					tiles->placeFood();
					isDead = false;
					move1 = -1;
					move2 = -1;
				}
			}
		}
	}
	
	delete head;
	delete tiles;
	
	
	return 0;
}
