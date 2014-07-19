#include "Player.hpp"

Player::Player(std::string cmd, int id): id(id), score(0), cmd(cmd) {
	// nothing to do here
}

CoordsLine Player::getMove(std::string input) {
	std::string ERR_STR = "Error in submission #" + std::to_string(id) + ": ";
	CoordsLine ERR = {{-1, -1}, {-1, -1}};

	FILE* pipe = popen((cmd + " " + input).c_str(), "r");
	if (!pipe) {
		std::cerr << ERR_STR << "command not found" << std::endl;
		return ERR;
	}

	char buffer[128];
	std::string result = "";
	if (fgets(buffer, 128, pipe) != NULL) {
		result = buffer;
		fclose(pipe);
	} else {
		if (feof(pipe)) {
			std::cerr << ERR_STR << "no output received" << std::endl;
		} else if (ferror(pipe)) {
			std::cerr << ERR_STR << "read error" << std::endl;
		} else {
			std::cerr << ERR_STR << "something very weird happened (I/O error)" << std::endl;
		}
		fclose(pipe);
		return ERR;
	}

	std::istringstream iss(result);
	int outputCoords[4];
	for (int i = 0; i < 4; ++i) {
		iss >> outputCoords[i];
		if (iss.eof()) {
			std::cerr << ERR_STR << "not enough input (" << i << " of 4): " << result << std::endl;
			return ERR;
		} else if (iss.bad()) {
			std::cerr << ERR_STR << "error in istringstream (something weird happened): " << result << std::endl;
			return ERR;
		} else if (iss.fail()) {
			std::cerr << ERR_STR << "parse error: " << result << std::endl;
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
