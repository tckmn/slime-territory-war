#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <string>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include "Coords.hpp"

class Player {
public:
	Player(std::string cmd, int id);
	CoordsLine getMove(std::string input);
	int id;
	int score;
	std::string str();

private:
	std::string cmd;
};

#endif
