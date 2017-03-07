#include <fstream>
#include <vector>
#include <iostream>

#include "deps\glm\glm.hpp"
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

