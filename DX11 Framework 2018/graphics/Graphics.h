#include <vector>
#include <windows.h>
#include <directxcolors.h>
#include "Vertex.h"
#include "Shaders.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include "ConstantBufferTypes.h"
#pragma comment( lib, "DXGI.lib" )
#pragma comment( lib, "d3d11.lib" )

class Graphics
{
public:
	bool Initialize( HWND hWnd, int width, int height );
	void BeginFrame( float clearColor[4] );
	void RenderFrame();
	void EndFrame();
public:
	float gTime;
	D3D11_RASTERIZER_DESC rasterizerDesc;
	D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_NULL;
	std::vector<DirectX::XMFLOAT4X4> worldMatricesCube;
	std::vector<DirectX::XMFLOAT4X4> worldMatricesPyramid;
private:
	HRESULT InitializeDirectX( HWND hWnd );
	HRESULT InitializeShaders();
	HRESULT InitializeScene();
private:
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
private:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
private:
	VertexShader vertexShader;
	PixelShader pixelShader;
	ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader;
private:
	VertexBuffer<Vertex_Pos_Col> vertexBufferCube;
	IndexBuffer indexBufferCube;
	VertexBuffer<Vertex_Pos_Col> vertexBufferPyramid;
	IndexBuffer indexBufferPyramid;
private:
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
private:
	UINT windowWidth;
	UINT windowHeight;
};