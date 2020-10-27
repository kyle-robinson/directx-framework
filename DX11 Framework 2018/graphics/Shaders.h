#pragma once
#ifndef SHADERS_H
#define SHADERS_H

#include <d3d11.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include "../utility/ErrorLogger.h"

class Shaders
{
protected:
	HRESULT CompileShaderFromFile(
		std::wstring szFileName,
		LPCSTR szEntryPoint,
		LPCSTR szShaderModel,
		ID3DBlob** ppBlobOut );
};

class VertexShader : Shaders
{
public:
	HRESULT Initialize(
		Microsoft::WRL::ComPtr<ID3D11Device> &device,
		std::wstring shaderPath,
		D3D11_INPUT_ELEMENT_DESC* layoutDesc,
		UINT numElements
	);
	ID3D11VertexShader* GetShader() const noexcept;
	ID3D10Blob* GetBuffer() const noexcept;
	ID3D11InputLayout* GetInputLayout() const noexcept;
private:
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;
	Microsoft::WRL::ComPtr<ID3D10Blob> shaderBuffer;
};

class PixelShader : Shaders
{
public:
	HRESULT Initialize( Microsoft::WRL::ComPtr<ID3D11Device> &device, std::wstring shaderPath );
	ID3D11PixelShader* GetShader() const noexcept;
	ID3D10Blob* GetBuffer() const noexcept;
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> shader;
	Microsoft::WRL::ComPtr<ID3D10Blob> shaderBuffer;
};

#endif