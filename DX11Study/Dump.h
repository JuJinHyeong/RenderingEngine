#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <assimp/scene.h>

class Dump {
public:
	template<typename... Args>
	static void Print(const Args&... args) {
		std::cout << std::fixed << std::setprecision(6);
		PrintRecursive(args...);
	}

	// Dump args to Dumps/{fileName}
	template<typename... Args>
	static void WriteToFile(const std::string& fileName, const Args&... args) {
		std::ofstream os("Dumps/" + fileName, std::ios::app);
		if (!os.is_open()) {
			std::cerr << "Failed to open the file: " << fileName << std::endl;
			return;
		}
		os << std::fixed << std::setprecision(5);
		WriteToFileRecursive(os, args...);
		os.close();
	}

	static void ClearFile(const std::string& fileName) {
		std::ofstream os("Dumps/" + fileName);
		os.close();
	}


	static std::string MatrixToString(const aiMatrix4x4& m, unsigned int space = 0) {
		std::stringstream ss;
		std::string pad(space, ' ');
		ss << std::fixed << std::setprecision(5)
			<< pad << m.a1 << " " << m.a2 << " " << m.a3 << " " << m.a4 << "\n"
			<< pad << m.b1 << " " << m.b2 << " " << m.b3 << " " << m.b4 << "\n"
			<< pad << m.c1 << " " << m.c2 << " " << m.c3 << " " << m.c4 << "\n"
			<< pad << m.d1 << " " << m.d2 << " " << m.d3 << " " << m.d4 << "\n";
		return ss.str();
	}

	static std::string MatrixToString(const DirectX::XMFLOAT4X4& m, unsigned int space = 0) {
		std::stringstream ss;
		std::string pad(space, ' ');
		ss << std::fixed << std::setprecision(5)
			<< pad << m._11 << " " << m._12 << " " << m._13 << " " << m._14 << "\n"
			<< pad << m._21 << " " << m._22 << " " << m._23 << " " << m._24 << "\n"
			<< pad << m._31 << " " << m._32 << " " << m._33 << " " << m._34 << "\n"
			<< pad << m._41 << " " << m._42 << " " << m._43 << " " << m._44 << "\n";
		return ss.str();
	}

	static std::string MatrixToString(const DirectX::FXMMATRIX& m, unsigned int space = 0) {
		DirectX::XMFLOAT4X4 mat;
		DirectX::XMStoreFloat4x4(&mat, m);
		return MatrixToString(mat, space);
	}
	
	static std::string ArrayToString(const std::vector<int>& arr) {
		std::stringstream ss;
		ss << "[";
		for (int i = 0; i < arr.size(); i++) {
			ss << " " << arr[i] << " ";
		}
		ss << "]";
		return ss.str();
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