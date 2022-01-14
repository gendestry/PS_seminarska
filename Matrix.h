
#pragma once
#include <vector>

template<class T>
class Matrix {
public:
	std::vector<std::vector<T>> matrix;

	Matrix() = default;
	Matrix(int size);
	Matrix(std::vector<std::vector<T>> mat);
	~Matrix() = default;

	void add(Matrix<T>& other);
	void sub(Matrix<T>& other);
	void scale(T scalar);
	Matrix<T> mul(Matrix<T>& other);

	unsigned int numRows() const { return matrix.size(); }
	unsigned int numCols() const { return matrix[0].size(); }

	void resize(int size) { matrix.resize(size); }

	std::vector<T>& operator[](int index) {
		return matrix[index];
	}
};