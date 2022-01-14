
#pragma once
#include <vector>
#include <iostream>

template<class T>
class Matrix {
public:
	std::vector<std::vector<T>> matrix;

	Matrix() = default;
	~Matrix() = default;

	Matrix(int size) {
		matrix.resize(size);
	}

	Matrix(std::vector<std::vector<T>> mat) {
		matrix = mat;
	}

	unsigned int numRows() const { return matrix.size(); }
	unsigned int numCols() const { return matrix[0].size(); }

	void resize(int size) { matrix.resize(size); }
	

	Matrix<T>& add(Matrix<T>& other) {
		if(numRows() != other.numRows() || numCols() != other.numCols())
			return *this;
		
		for(int i = 0; i < matrix.size(); i++) {
			for(int j = 0; j < matrix[i].size(); j++) {
				matrix[i][j] += other[i][j];
			}
		}

		return *this;
	}

	Matrix<T>& sub(Matrix<T>& other) {
		if(numRows() != other.numRows() || numCols() != other.numCols())
			return *this;
		
		for(int i = 0; i < matrix.size(); i++) {
			for(int j = 0; j < matrix[i].size(); j++) {
				matrix[i][j] -= other[i][j];
			}
		}

		return *this;
	}
	
	Matrix<T>& scale(T scalar) {
		for(auto& vec : matrix)
			for(auto& el : vec) 
				el *= scalar;
		return *this;
	}

	Matrix<T> mul(Matrix<T>& other) {
		if(numCols() != other.numRows())
			return Matrix<T>();
		
		auto& a = matrix;
		auto& b = other;

		Matrix<T> c(a.size());

		for(int i = 0; i < a.size(); i++) {
			c[i].resize(b[i].size());
			for(int j = 0; j < a[i].size(); j++) {
				T product = 0.0;
				for(int k = 0; k < a.size(); k++) {
					product += a[i][k] * b[k][j];
				}
				c[i][j] = product;
			}
		}

		return c;
	}

	Matrix<T>& abs() {
		for(auto& vec : matrix)
			for(auto& el : vec)
				el = fabs(el);
		return *this;
	}

	T sumElements() const {
		T sum = 0;
		for(auto& vec : matrix)
			for(auto& el : vec)
				sum += el;

		return sum;
	}

	// operators

	Matrix<T> operator+(Matrix<T>& other) {
		Matrix<T> res;
		res.matrix = matrix;
		res += other;
		return res;
	}

	Matrix<T> operator-(Matrix<T>& other) {
		Matrix<T> res;
		res.matrix = matrix;
		res -= other;
		return res;
	}

	Matrix<T> operator*(Matrix<T>& other) {
		Matrix<T> res;
		res.matrix = matrix;
		res *= other;
		return res;
	}

	Matrix<T> operator*(T scalar) {
		Matrix<T> res;
		res.matrix = matrix;
		res *= scalar;
		return res;
	}

	Matrix& operator+= (Matrix<T>& other) {
		return add(other);
	}

	Matrix& operator-= (Matrix<T>& other) {
		return sub(other);
	}

	Matrix& operator*= (Matrix<T>& other) {
		matrix = mul(other).matrix;
		return *this;
	}

	Matrix& operator*= (T scalar) {
		return scale(scalar);
	}


	std::vector<T>& operator[](int index) {
		return matrix[index];
	}

	friend std::ostream& operator<<(std::ostream& out, Matrix& mat) {
		for(int i = 0; i < mat.numRows(); i++) {
			for(int j = 0; j < mat.numCols(); j++) {
				out << mat[i][j] << "  ";
			}
			out << std::endl;
		}
		return out;
	}
};