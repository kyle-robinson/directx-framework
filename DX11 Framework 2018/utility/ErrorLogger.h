#pragma once
#include <Windows.h>
#include "StringConverter.h"

class ErrorLogger
{
public:
	static void Log( const std::string& message ) noexcept;
	static void Log( HRESULT hr, const std::string& message ) noexcept;
	static void Log( HRESULT hr, const std::wstring& message ) noexcept;
};