#pragma once
#include <vector>

template<typename T>
using Matrix = std::vector<std::vector<T>>;

template<typename T>
T matsumel(const Matrix<T>& a) {
	T sum = 0.0;
	for(auto& vec : a)
		for(auto& el : vec)
			sum += el;

	return sum;
}

template<typename T>
Matrix<T>& matscale(Matrix<T>& a, T scale) {
	for(auto& vec : a)
		for(auto& el : vec) 
			el *= scale;

	return a;
}

template<typename T>
Matrix<T> matsum(const Matrix<T>& a, const Matrix<T>& b) noexcept {
	Matrix<T> c(a.size());

	for(int i = 0; i < a.size(); i++) {
		c[i].resize(a[i].size());
		for(int j = 0; j < a[i].size(); j++) {
			c[i][j] = a[i][j] + b[i][j]; 
		}
	}

	return c;
}

template<typename T>
Matrix<T> matmul(const Matrix<T>& a, const Matrix<T>& b) noexcept {
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