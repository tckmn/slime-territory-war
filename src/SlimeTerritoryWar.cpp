//#define DEBUG
#define MULTITHREADING
#define MULTITHREADING_COUNT 4

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <time.h>
#include "Player.hpp"
#include "Board.hpp"
#ifdef DEBUG
#ifndef MULTITHREADING
#include <chrono>
#include <thread>
#endif
#endif
#ifdef MULTITHREADING
#include <chrono>
#include <thread>
#include <mutex>
#include <signal.h>
#ifndef MULTITHREADING_COUNT
#define MULTITHREADING_COUNT std::thread::hardware_concurrency()
#endif
std::mutex matchupsMtx, coutMtx, cacheMtx;
#endif

std::ofstream cache;
void playGame(std::vector<Player>& players);
#ifdef MULTITHREADING
void mt_playGame(std::vector< std::vector<Player> >& matchups);
void mt_onSIGINT(int _);
#endif
std::vector< std::vector<Player> > getAllMatchups(std::vector<std::string> commands);

bool FINISHED = false;

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
	cache.open("cache.txt", std::ios::out | std::ios::app);

#ifdef MULTITHREADING
	// initialize and start threads
	std::thread threads[MULTITHREADING_COUNT];
	for (unsigned int i = 0; i < MULTITHREADING_COUNT; ++i) {
		threads[i] = std::thread(mt_playGame, std::ref(matchups));
	}

	// handle ctrl+c for lots of data
	struct sigaction handleSIGINT;
	handleSIGINT.sa_handler = mt_onSIGINT;
	sigemptyset(&handleSIGINT.sa_mask);
	handleSIGINT.sa_flags = 0;
	sigaction(SIGINT, &handleSIGINT, NULL);

	// have main thread wait until other threads finished
	while (!FINISHED) {
		// do something just to avoid eating CPU
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	// completely finish other threads
	for (unsigned int i = 0; i < MULTITHREADING_COUNT; ++i) {
		threads[i].join();
	}
#else
	for (auto matchup = matchups.begin(); matchup != matchups.end(); ++matchup) {
		playGame(*matchup);
		for (auto p = matchup->begin(); p != matchup->end(); ++p) {
			std::cout << p->str() << std::endl;
		}
	}
#endif

	cache.close();
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

#ifdef DEBUG
		std::cout << b.prettyStr() << cl.from.x << " " << cl.from.y << " " << cl.to.x << " " << cl.to.y << std::endl << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
#endif
	}

	for (unsigned int i = 0; i < players.size(); ++i) {
		players[i].score = b.count('1' + i);
	}
#ifdef MULTITHREADING
	std::ostringstream cacheTxt;

	coutMtx.lock();
	for (auto p = players.begin(); p != players.end(); ++p) {
		std::cout << p->str() << std::endl;
		cacheTxt << p->id << " ";
	}
	coutMtx.unlock();

	std::string cacheStr = cacheTxt.str();
	cacheStr.pop_back();

	cacheMtx.lock();
	cache << cacheStr << std::endl;
	cacheMtx.unlock();
#endif
}

#ifdef MULTITHREADING
void mt_playGame(std::vector< std::vector<Player> >& mt_matchups) {
	for (;;) {
		// ctrl+c
		if (FINISHED) return;

		// get matchup with thread safety
		matchupsMtx.lock();
		if (mt_matchups.empty()) {
			FINISHED = true;
			matchupsMtx.unlock();
			return;
		}
		std::vector<Player> matchup = mt_matchups.back();
		mt_matchups.pop_back();
		matchupsMtx.unlock();

		playGame(matchup);
	}
}

void mt_onSIGINT(int _) {
	FINISHED = true;
}
#endif

std::vector< std::vector<Player> > getAllMatchups(std::vector<std::string> commands) {
	std::vector< std::vector<Player> > matchups;

	std::ifstream iCache("cache.txt");
	std::string line;
	std::set<std::string> cached;
	while (std::getline(iCache, line)) {
		cached.insert(line);
	}
	std::cout << cached.size() << std::endl;

	// disclaimer: very ugly code ahead
	// you have been warned
	int cmdCount = commands.size();
	for (int p1 = 0; p1 < cmdCount - 3; ++p1) {
		for (int p2 = p1+1; p2 < cmdCount - 2; ++p2) {
			for (int p3 = p2+1; p3 < cmdCount - 1; ++p3) {
				for (int p4 = p3+1; p4 < cmdCount - 0; ++p4) {
					int p1i = std::stoi(commands[p1].substr(0, 2)), p2i = std::stoi(commands[p2].substr(0, 2)),
						p3i = std::stoi(commands[p3].substr(0, 2)), p4i = std::stoi(commands[p4].substr(0, 2));
					std::ostringstream cacheTxt;
					cacheTxt << p1i << " " << p2i << " " << p3i << " " << p4i;

					if (cached.find(cacheTxt.str()) == cached.end()) {
						std::vector<Player> matchup;
						matchup.push_back(Player(commands[p1].substr(2), p1i));
						matchup.push_back(Player(commands[p2].substr(2), p2i));
						matchup.push_back(Player(commands[p3].substr(2), p3i));
						matchup.push_back(Player(commands[p4].substr(2), p4i));
						matchups.push_back(matchup);
					} else {
						std::cout << "cached!" << std::endl;
					}
				}
			}
		}
	}

	return matchups;
}
