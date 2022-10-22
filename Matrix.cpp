#include "Matrix.h"

Matrix::Matrix(int a, int b)
{
	row = a;
	colomn = b;
	vector<double> temp(colomn);
	value.resize(row, temp);
}
Matrix::Matrix(int row, int colomn, std::initializer_list<double> il)
{
	std::size_t count = 0;

	this->row = row;
	this->colomn = colomn;
	vector<double> temp(colomn);
	value.resize(row, temp);
	for (const auto& e : il)
	{
		value[count / colomn][count % colomn] = e;
		count++;
	}
}
std::size_t Matrix::Row()
{
	return row;
}
std::size_t Matrix::Colomn()
{
	return colomn;
}
Matrix::~Matrix()
{

}
