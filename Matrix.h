#pragma once
#include<iostream>
#include<windows.h>
#include<fstream>
#include<cstdlib>
#include<cstdio>
#include<cmath>
#include<iomanip>
#include<vector>
#include <stdarg.h>
using namespace std;
class Matrix
{
public:
	vector<vector<double>> value;
	vector<double>& operator[](int i)
	{
		return value[i];
	}
	double& operator()(int i)
	{
		return value[i / colomn][i % colomn];
	}
	Matrix& operator=(std::initializer_list<double> il)
	{
		std::size_t count = 0;

		for (const auto& e : il)
		{
			value[count / colomn][count % colomn] = e;
			count++;
		}
		return *this;
	}
	Matrix(int a = 0, int b = 0);
	Matrix(int row, int colomn, std::initializer_list<double> il);
	std::size_t Row();
	std::size_t Colomn();
	~Matrix();
private:
	std::size_t row;
	std::size_t colomn;
};
