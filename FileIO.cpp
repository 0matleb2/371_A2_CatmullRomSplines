#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>

#include <GLM\glm.hpp>
#include "FileIO.h"


void FileIO::writeRotational(std::vector<glm::vec3> vertices, int numSweeps) {

	std::ofstream outfile("mesh_data.txt");

	if (outfile.is_open()) {

		outfile << 1 << std::endl;
		outfile << numSweeps << std::endl;

		outfile << vertices.size() << std::endl;
		for (int i = 0, n = vertices.size(); i < n; ++i) {
			outfile << vertices[i].x << " " << vertices[i].z << " " << vertices[i].y << std::endl;
		}

	}
	else {
		std::cout << "Error writing to file" << std::endl;
	}
}

void FileIO::writeTranslational(std::vector<glm::vec3> profile, std::vector<glm::vec3> trajectory) {

	std::ofstream outfile("mesh_data.txt");

	if (outfile.is_open()) {

		outfile << 0 << std::endl;

		outfile << profile.size() << std::endl;
		for (int i = 0, n = profile.size(); i < n; ++i) {
			outfile << profile[i].x << " " << profile[i].z << " " << profile[i].y << std::endl;
		}

		outfile << trajectory.size() << std::endl;
		for (int i = 0, n = trajectory.size(); i < n; ++i) {
			outfile << trajectory[i].x << " " << trajectory[i].y << " " << trajectory[i].z << std::endl;
		}

	}
	else {
		std::cout << "Error writing to file" << std::endl;
	}
}

std::vector<std::string> FileIO::readFile() {


	// Read input file
	std::string input_file_path = "mesh_data.txt";
	std::vector<std::string> input_file_lines;
	std::ifstream input_file_stream(input_file_path, std::ios::in);

	if (input_file_stream.is_open()) {
		std::string line = "";
		while (getline(input_file_stream, line))
			input_file_lines.push_back(line);
		input_file_stream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ?\n", input_file_path.c_str());
		getchar();
		exit(-1);
	}

	return input_file_lines;

}

