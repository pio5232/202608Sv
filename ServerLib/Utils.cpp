#include "LibraryPch.h"
#include "Utils.h"
#include <random>
#include <math.h>


void ExecuteProcess(const WCHAR* path, const WCHAR* currentDirectory)//, const std::wstring& args)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	bool ret = CreateProcess(path, nullptr, nullptr, nullptr, FALSE, CREATE_NEW_CONSOLE, NULL, currentDirectory, &si, &pi);

	if (!ret)
	{
		printf("Create Process Failed [%d]\n", GetLastError());
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

}

double GetRandDouble(double min, double max, int roundPlaceValue)
{
	static std::random_device randomDevice;

	static std::mt19937 gen(randomDevice());

	std::uniform_real_distribution<double> dist(min, max);

	double d = dist(gen);

	double powValue = pow(10, roundPlaceValue);
	d *= powValue;

	d = round(d);
	d /= powValue;

	return d;
}

int GetRand(int min, int max)
{
	static std::random_device randomDevice;

	static std::mt19937 gen(randomDevice());

	std::uniform_int_distribution<int> dist(min, max);

	return dist(gen);
}

bool CheckChance(int percentage)
{
	return static_cast<int>(GetRandDouble(0.0, 100.0)) < percentage;
}

