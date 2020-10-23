#include "Matrix.h"

// parameter constructor
template<typename T>
Matrix<T>::Matrix( unsigned int rows, unsigned int cols, const T& initial )
{
	this->matrix.resize( rows );
	for ( int i = 0; i < this->matrix.size(); i++ )
		this->matrix[i].resize( cols, initial );

	this->rows = rows;
	this->cols = cols;
}

// copy constructor
template<typename T>
Matrix<T>::Matrix( const Matrix<T>& rhs )
{
	this->matrix = rhs.matrix;
	this->rows = rhs.GetRows();
	this->cols = rhs.GetCols();
}

// virtual destructor
template<typename T>
Matrix<T>::~Matrix() { }

// assignment operator
template<typename T>
Matrix<T>& Matrix<T>::operator=( const Matrix<T>&  rhs )
{
	if ( &rhs == this )
		return *this;

	unsigned int newRows = rhs.GetRows();
	unsigned int newCols = rhs.GetCols();

	this->matrix.resize( newRows );
	for ( int i = 0; i < this->matrix.size(); i++ )
		this->matrix[i].resize( newCols );

	for ( int i = 0; i < newRows; i++ )
	{
		for ( int j = 0; j < newCols; j++ )
			this->matrix[i][j] = rhs( i, j );
	}

	this->rows = newRows;
	this->cols = newCols;

	return *this;
}

// matrix addition
template<typename T>
Matrix<T> Matrix<T>::operator+( const Matrix<T>& rhs )
{
	Matrix result( this->rows, this->cols, 0.0 );
	for ( int i = 0; i < this->rows; i++ )
	{
		for ( int j = 0; j < this->cols; j++ )
			result( i, j ) = this->matrix[i][j] + rhs( i, j );
	}
	return result;
}

// cumulative adddition of 'this' matrix and another
template<typename T>
Matrix<T>& Matrix<T>::operator+=( const Matrix<T>& rhs )
{
	unsigned int rows = rhs.GetCols();
	unsigned int cols = rhs.GetRows();

	for ( int i = 0; i < rows; i ++ )
	{
		for ( int j = 0; j < cols; j++ )
			this->matrix[i][j] += rhs( i, j );
	}

	return *this;
}

// subtraction of 'this' matrix and another
template<typename T>
Matrix<T> Matrix<T>::operator-( const Matrix<T>& rhs )
{
	Matrix result( this->rows, this->cols, 0.0 );
	unsigned int rows = rhs.GetRows();
	unsigned int cols = rhs.GetCols();
	for ( int i = 0; i < rows; i++ )
	{
		for ( int j = 0; j < cols; j++ )
			result( i, j ) = this->matrix[i][j] - rhs( i, j );
	}
	return result;
}

// cumulative subtraction of 'this' matrix and another
template<typename T>
Matrix<T>& Matrix<T>::operator-=( const Matrix<T>& rhs )
{
	unsigned int rows = rhs.GetRows();
	unsigned int cols = rhs.GetCols();
	for ( int i = 0; i < rows; i++ )
	{
		for ( int j = 0; j < cols; j++ )
			this->matrix[i][j] -= rhs( i, j );
	}
	return *this;
}

// left multiplication of 'this' matrix and another
template<typename T>
Matrix<T> Matrix<T>::operator*( const Matrix<T>& rhs )
{
	Matrix result( this->rows, this->cols, 0.0 );
	unsigned int rows = rhs.GetRows();
	unsigned int cols = rhs.GetCols();
	for ( int i = 0; i < rows; i++ )
	{
		for ( int j = 0; j < cols; j++ )
		{
			for ( int k = 0; k < rows, k++ )
				result( i, j ) += this->matrix[i][k] * rhs( k, j );
		}
	}
	return result;
}

// cumulative left multiplication of 'this' matrix and another
template<typename T>
Matrix<T>& Matrix<T>::operator*=( const Matrix<T>& rhs )
{
	Matrix result = ( *this ) * rhs;
	( *this ) = result;
	return *this;
}

// calculate the transpose of 'this' matrix
template<typename T>
Matrix<T> Matrix<T>::Transpose()
{
	Matrix result( this->rows, this->cols, 0.0 );
	for ( int i = 0; i < this->rows; i++ )
	{
		for ( int j = 0; j < this->cols; j++ )
			result( i, j ) = this->matrix[j][i];
	}
	return this->matrix;
}

// matrix / scalar addition
template<typename T>
Matrix<T> Matrix<T>::operator+( const T& rhs )
{
	Matrix result( this->rows, this->cols, 0.0 );
	for ( int i = 0; i < this->rows; i++  )
	{
		for ( int j = 0; j < this->cols; j++ )
			result( i, j ) = this->matrix[i][j] + rhs;
	}
	return result;
}

// matrix / scalar subtraction
template<typename T>
Matrix<T> Matrix<T>::operator-( const T& rhs )
{
	Matrix result( this->rows, this->cols, 0.0 );
	for ( int i = 0; i < this->rows; i++  )
	{
		for ( int j = 0; j < this->rows; j++ )
			result( i, j ) = this->matrix[i][j] - rhs;
	}
	return result;
}

// matrix / scalar multiplication
template<typename T>
Matrix<T> Matrix<T>::operator*( const T& rhs )
{
	Matrix result( this->rows, this->cols, 0.0 );
	for ( int i = 0; i < this->rows; i++  )
	{
		for ( int j = 0; j < this->rows; j++ )
			result( i, j ) = this->matrix[i][j] * rhs;
	}
	return result;
}

// matrix / scalar division
template<typename T>
Matrix<T> Matrix<T>::operator/( const T& rhs )
{
	Matrix result( this->rows, this->cols, 0.0 );
	for ( int i = 0; i < this->rows; i++  )
	{
		for ( int j = 0; j < this->rows; j++ )
			result( i, j ) = this->matrix[i][j] / rhs;
	}
	return result;
}

// multiply a matrix with a vector
template<typename T>
std::vector<T> Matrix<T>::operator*( const std::vector<T>& rhs )
{
	std::vector<T> result( rhs.size(), 0.0 );
	for ( int i = 0; i < this->row; i++ )
	{
		for ( int j = 0; j < this->cols; j++ )
			result[i] += this->matrix[i][j] * rhs[j];
	}
	return result;
}

// get vector of the diagonal elements
template<typename T>
std::vector<T> Matrix<T>::DiagonalVector()
{
	std::vector<T> result( this->rows, 0.0 );
	for ( int i = 0; i < this->rows; i++ )
		diagonalElements[i] = this->matrix[i][i];
	return result;
}

// access the individual elements
template<typename T>
T& Matrix<T>::operator() ( const unsigned int& row, const unsigned int& col )
{
	return this->matrix[row][col];
}

// access the individual elements - const
template<typename T>
const T& Matrix<T>::operator() ( const unsigned int& row, const unsigned int& col ) const
{
	return this->matrix[row][col];
}

// get the number of rows in the matrix
template<typename T>
unsigned int Matrix<T>::GetRows() const
{
	return this->rows;
}

// get the number of cols in the matrix
template<typename T>
unsigned int Matrix<T>::GetCols() const
{
	return this->cols;
}