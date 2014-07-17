#include "Board.hpp"
#include <iostream>

Board::Board() {
	for (int x = 0; x < BOARD_WIDTH; ++x) {
		for (int y = 0; y < BOARD_HEIGHT; ++y) {
			data[x][y] = EMPTY;
		}
	}

	data[0]            [0]              = '1';
	data[0]            [BOARD_HEIGHT-1] = '2';
	data[BOARD_WIDTH-1][0]              = '3';
	data[BOARD_WIDTH-1][BOARD_HEIGHT-1] = '4';
}

void Board::move(char player, CoordsLine movement) {
	// bounds check
	if (!(inBounds(movement.from) && inBounds(movement.to))) return;

	// player can only move his own slimes to only empty squares or his own slimes
	if (data[movement.from.x][movement.from.y] != player) return;
	if (data[movement.to.x][movement.to.y] != EMPTY && data[movement.to.x][movement.to.y] != player) return;

	// distance check
	int maxDelta = max(abs(movement.from.x - movement.to.x), abs(movement.from.y - movement.to.y));
	if (maxDelta <= 0 || maxDelta > 2) return;

	// move the slime and convert adjacent slimes
	if (data[movement.to.x][movement.to.y] == player) {
		// one more distance check
		if (maxDelta == 2) return;

		for (int dx = -1; dx <= 1; ++dx) {
			for (int dy = -1; dy <= 1; ++dy) {
				if (inBounds({movement.to.x + dx, movement.to.y + dy}) && data[movement.to.x + dx][movement.to.y + dy] == EMPTY) {
					data[movement.to.x + dx][movement.to.y + dy] = player;
				}
			}
		}
		killSlime(movement.from);
	} else {
		addSlime(movement.to, player);
		if (maxDelta == 2) killSlime(movement.from);
	}

}

int Board::count(char c) {
	int counter = 0;
	for (int x = 0; x < BOARD_WIDTH; ++x) {
		for (int y = 0; y < BOARD_HEIGHT; ++y) {
			if (data[x][y] == c) ++counter;
		}
	}
	return counter;
}

bool Board::inBounds(Coords location) {
	return (location.x >= 0 && location.x < BOARD_WIDTH && location.y >= 0 && location.y < BOARD_HEIGHT);
}

void Board::addSlime(Coords location, char player) {
	data[location.x][location.y] = player;
	for (int dx = -1; dx <= 1; ++dx) {
		for (int dy = -1; dy <= 1; ++dy) {
			if (inBounds({location.x + dx, location.y + dy}) && data[location.x + dx][location.y + dy] != EMPTY) {
				data[location.x + dx][location.y + dy] = player;
			}
		}
	}
}

void Board::killSlime(Coords location) {
	data[location.x][location.y] = EMPTY;
}

std::string Board::str() {
	std::string s = "";
	for (int x = 0; x < BOARD_WIDTH; ++x) {
		for (int y = 0; y < BOARD_HEIGHT; ++y) {
			s += data[x][y];
		}
		s += ',';
	}
	s.erase(s.size() - 1);
	return s;
}

std::string Board::prettyStr() {
	std::string s = "";
	for (int x = 0; x < BOARD_WIDTH; ++x) {
		for (int y = 0; y < BOARD_HEIGHT; ++y) {
			s += data[x][y];
		}
		s += '\n';
	}
	return s;
}
