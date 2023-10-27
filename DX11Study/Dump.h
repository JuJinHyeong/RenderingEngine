#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <assimp/scene.h>

class Dump {
public:
	template<typename... Args>
	static void Print(const Args&... args) {
#ifndef NO_DEBUG
		std::cout << std::fixed << std::setprecision(3);
		PrintRecursive(args...);
#endif
	}

	// Dump args to Dumps/{fileName}
	template<typename... Args>
	static void WriteToFile(const std::string& fileName, const Args&... args) {
#ifndef NO_DEBUG
		std::ofstream os("Dumps/" + fileName, std::ios::app);
		if (!os.is_open()) {
			std::cerr << "Failed to open the file: " << fileName << std::endl;
			return;
		}
		os << std::fixed << std::setprecision(3);
		WriteToFileRecursive(os, args...);
		os.close();
#endif
	}

	static void ClearFile(const std::string& fileName) {
		std::ofstream os("Dumps/" + fileName);
		os.close();
	}
private:
	template<typename T>
	static void WriteToFileRecursive(std::ofstream& os, const T& value) {
		os << value;
	}
	template<typename T, typename... Args>
	static void WriteToFileRecursive(std::ofstream& os, const T& value, const Args&... args) {
		os << value << " ";
		WriteToFileRecursive(os, args...);
	}

	template<typename T>
	static void PrintRecursive(const T& value) {
		std::cout << value;
	}
	template<typename T, typename... Args>
	static void PrintRecursive(const T& value, const Args&... args) {
		std::cout << value << " ";
		PrintRecursive(args...);
	}
};