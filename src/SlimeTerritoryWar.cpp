//#define LINUX_DEBUG

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <time.h>
#include "Player.hpp"
#include "Board.hpp"
#ifdef LINUX_DEBUG
#include <unistd.h>
#endif

void playGame(std::vector<Player>& players);
std::vector< std::vector<Player> > getAllMatchups(std::vector<std::string> commands);

int main() {
	// retrieve list of players
	std::ifstream playerlistFile("playerlist.txt");
	std::string line;
	std::vector<std::string> commands;
	while (std::getline(playerlistFile, line)) {
		commands.push_back(line);
	}

	// start playing!
	std::vector< std::vector<Player> > matchups = getAllMatchups(commands);
	for (auto matchup = matchups.begin(); matchup != matchups.end(); ++matchup) {
		playGame(*matchup);
		for (auto p = matchup->begin(); p != matchup->end(); ++p) {
			std::cout << p->str() << std::endl;
		}
	}

	return 0;
}

void playGame(std::vector<Player>& players) {
	// initialize stuffs
	Board b;

	// main loop
	for (int i = 0; b.count('.'); i = (i + 1) % players.size()) {
		// send input and get output
		std::ostringstream oss;
		oss << (i+1) << "," << b.str();

		// time how long it take to get input to report outliers that take way too long
		clock_t moveClock = clock();
		CoordsLine cl = players[i].getMove(oss.str());
		float time = ((float) clock() - moveClock) / CLOCKS_PER_SEC;
		if (time >= 0.1) std::cerr << players[i].str() << " took too long (" << time << ")" << std::endl;

		// try to move (will do nothing i.e. pass if move is invalid)
		b.move('1' + i, cl);

#ifdef LINUX_DEBUG
		std::cout << b.prettyStr() << cl.from.x << " " << cl.from.y << " " << cl.to.x << " " << cl.to.y << std::endl << std::endl;
		usleep(10000);
#endif
	}

	for (unsigned int i = 0; i < players.size(); ++i) {
		players[i].score = b.count('1' + i);
	}
}

std::vector< std::vector<Player> > getAllMatchups(std::vector<std::string> commands) {
	std::vector< std::vector<Player> > matchups;

	// disclaimer: very ugly code ahead
	// you have been warned
	int cmdCount = commands.size();
	for (int p1 = 0; p1 < cmdCount - 3; ++p1) {
		for (int p2 = p1+1; p2 < cmdCount - 2; ++p2) {
			for (int p3 = p2+1; p3 < cmdCount - 1; ++p3) {
				for (int p4 = p3+1; p4 < cmdCount - 0; ++p4) {
					std::vector<Player> matchup;
					matchup.push_back(Player(commands[p1]));
					matchup.push_back(Player(commands[p2]));
					matchup.push_back(Player(commands[p3]));
					matchup.push_back(Player(commands[p4]));
					matchups.push_back(matchup);
				}
			}
		}
	}

	return matchups;
}
