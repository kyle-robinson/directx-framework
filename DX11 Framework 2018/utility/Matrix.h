#pragma once
#ifndef MATRIX_H
#define MATRIX_H

#include <vector>

template<typename T>
class Matrix
{
public:
	Matrix( unsigned int rows, unsigned int cols, const T& initial );
	Matrix( const Matrix<T>& rhs );
	virtual ~Matrix();
public:
	// operator overloading
	Matrix<T>& operator=( const Matrix<T>& rhs );
	// matrix mathematical operations
	Matrix<T> operator+( const Matrix<T>& rhs );
	Matrix<T>& operator+=( const Matrix<T>& rhs );
	Matrix<T> operator-( const Matrix<T>& rhs );
	Matrix<T>& operator-=( const Matrix<T>& rhs );
	Matrix<T> operator*( const Matrix<T>& rhs );
	Matrix<T>& operator*=( const Matrix<T>& rhs );
	Matrix<T> Transpose();
public:
	// matrix scalar operations
	Matrix<T> operator+( const T& rhs );
	Matrix<T> operator-( const T& rhs );
	Matrix<T> operator*( const T& rhs );
	Matrix<T> operator/( const T& rhs );
public:
	// matrix / vector operations
	std::vector<T> operator*( const std::vector<T>& rhs );
	std::vector<T> DiagonalVector();
	// access the individual elements
	T& operator() ( const unsigned int& row, const unsigned int& col );
	const T& operator() ( const unsigned int& row, const unsigned int& col ) const;
	// access the row and column sizes
	unsigned GetRows() const;
	unsigned GetCols() const;
private:
	std::vector<std::vector<T>> matrix;
	unsigned int rows, cols;
};

//#include "Matrix.cpp"
#endif