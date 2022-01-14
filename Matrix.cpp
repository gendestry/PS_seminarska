#include "Matrix.h"

template<class T>
Matrix<T>::Matrix(int size) {
	matrix.resize(size);
}

template<class T>
Matrix<T>::Matrix(std::vector<std::vector<T>> mat) {
	matrix = mat;
}

template<class T>
void Matrix<T>::add(Matrix<T>& other) {
	if(numRows() != other.numRows() || numCols != other.numCols)
		return;
	
	for(int i = 0; i < matrix.size(); i++) {
		for(int j = 0; j < matrix[i].size(); j++) {
			matrix[i][j] += other[i][j];
		}
	}
}

template<class T>
void Matrix<T>::sub(Matrix<T>& other) {
	if(numRows() != other.numRows() || numCols != other.numCols)
		return;
	
	for(int i = 0; i < matrix.size(); i++) {
		for(int j = 0; j < matrix[i].size(); j++) {
			matrix[i][j] -= other[i][j];
		}
	}
}

template<class T>
Matrix<T> Matrix<T>::mul(Matrix<T>& other) {
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

template<class T>
void Matrix<T>::scale(T scalar) {
	for(auto& vec : matrix)
		for(auto& el : vec) 
			el *= scale;
}