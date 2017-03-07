#pragma once


class FileIO {

public:

	static void writeRotational(std::vector<glm::vec3> vertices, int numSweeps = 90);

	static void writeTranslational(std::vector<glm::vec3> profile, std::vector<glm::vec3> trajectory);

};