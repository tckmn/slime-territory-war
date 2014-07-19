#include "Player.hpp"

Player::Player(std::string cmd, int id): id(id), score(0), cmd(cmd) {
	// nothing to do here
}

CoordsLine Player::getMove(std::string input) {
	std::string ERR_STR = "Problem in submission #" + std::to_string(id) + " (getMove): ";
	CoordsLine ERR = {{-1, -1}, {-1, -1}};

	// time how long it takes to get output to report outliers that take way too long
	clock_t moveClock = clock();

	FILE* pipe = popen((cmd + " " + input).c_str(), "r");
	if (!pipe) {
		std::cout << ERR_STR << "command not found" << std::endl;
		return ERR;
	}

	char buffer[128];
	std::string result = "";
	if (fgets(buffer, 128, pipe) != NULL) {
		result = buffer;
		fclose(pipe);
	} else {
		if (feof(pipe)) {
			std::cout << ERR_STR << "no output received" << std::endl;
		} else if (ferror(pipe)) {
			std::cout << ERR_STR << "read error" << std::endl;
		} else {
			std::cout << ERR_STR << "something very weird happened (I/O error)" << std::endl;
		}
		fclose(pipe);
		return ERR;
	}

	// finish timing
	float time = ((float) clock() - moveClock) / CLOCKS_PER_SEC;
	if (time >= 0.1) std::cout << ERR_STR << "took too long (" << time << "s)" << std::endl;

	std::istringstream iss(result);
	int outputCoords[4];
	for (int i = 0; i < 4; ++i) {
		iss >> outputCoords[i];
		if (iss.eof()) {
			std::cout << ERR_STR << "not enough input (" << i << " of 4): " << result << std::endl;
			return ERR;
		} else if (iss.bad()) {
			std::cout << ERR_STR << "error in istringstream (something weird happened): " << result << std::endl;
			return ERR;
		} else if (iss.fail()) {
			std::cout << ERR_STR << "parse error: " << result << std::endl;
			return ERR;
		}
	}

	return CoordsLine{{outputCoords[0], outputCoords[1]}, {outputCoords[2], outputCoords[3]}};
}

std::string Player::str() {
	std::ostringstream oss;
	oss << id;
	if (score) oss << " " << score;
	return oss.str();
}
