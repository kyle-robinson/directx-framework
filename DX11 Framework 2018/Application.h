#pragma once
#include "graphics\\Vertex.h"
#include "graphics\\ConstantBufferTypes.h"
#include "utility\\ErrorLogger.h"
#include "resource.h"
#include <vector>
#include <windows.h>
#include <d3d11_1.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <directxcolors.h>
#pragma comment( lib, "DXGI.lib" )
#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "D3DCompiler.lib" )

using namespace DirectX;

class Application
{
public:
	Application() {};
	HRESULT Initialise( HINSTANCE hInstance, int nCmdShow, int windowWidth, int windowHeight );
	void Update();
	void Draw();
private:
	HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow );
	HRESULT InitDevice();
	HRESULT CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut );
	HRESULT InitShadersAndInputLayout();
	HRESULT InitVertexBuffer();
	HRESULT InitIndexBuffer();
private:
	HINSTANCE _hInst;
	HWND _hWnd;
	D3D_DRIVER_TYPE _driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL _featureLevel = D3D_FEATURE_LEVEL_11_0;
	Microsoft::WRL::ComPtr<ID3D11Device> _pd3dDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> _pImmediateContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> _pSwapChain;
private:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _pRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> _pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> _pPixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> _pVertexLayout;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> _depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> _depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> _rasterizerState;
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> _pVertexBufferCube;
	Microsoft::WRL::ComPtr<ID3D11Buffer> _pIndexBufferCube;
	Microsoft::WRL::ComPtr<ID3D11Buffer> _pVertexBufferPyramid;
	Microsoft::WRL::ComPtr<ID3D11Buffer> _pIndexBufferPyramid;
	Microsoft::WRL::ComPtr<ID3D11Buffer> _pConstantBuffer;
private:
	XMFLOAT4X4 _view;
	XMFLOAT4X4 _projection;
	std::vector<XMFLOAT4X4> _worldMatricesCube;
	std::vector<XMFLOAT4X4> _worldMatricesPyramid;
private:
	float gTime;
	UINT _WindowWidth;
	UINT _WindowHeight;
	D3D11_RASTERIZER_DESC rasterizerDesc;
};

