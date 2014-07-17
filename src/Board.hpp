#ifndef BOARD_HPP
#define BOARD_HPP

#include <string>
#include "Coords.hpp"

#define BOARD_WIDTH 8
#define BOARD_HEIGHT BOARD_WIDTH
#define EMPTY '.'

#define max(x, y) ((x) > (y) ? (x) : (y))
#define abs(x) ((x) < 0 ? -(x) : (x))

class Board {
public:
	Board();
	void move(char player, CoordsLine movement);
	int count(char c);
	std::string str();
	std::string prettyStr();

private:
	char data[BOARD_WIDTH][BOARD_HEIGHT];
	bool inBounds(Coords location);
	void addSlime(Coords location, char player);
	void killSlime(Coords location);
};

#endif
