#pragma once
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Light.h"
#include "Camera.h"
#include "Shaders.h"
#include "ImGuiManager.h"
#include "RenderableGameObject.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include <dxtk/WICTextureLoader.h>

class Graphics
{
public:
	bool Initialize( HWND hWnd, int width, int height );
	void BeginFrame( float clearColor[4] );
	void RenderFrame();
	void EndFrame();
	void Update();
public:
	Light light;
	Camera camera;
	RenderableGameObject gameObject;
	float gTime, multiplier = 1.0f;
	D3D11_RASTERIZER_DESC rasterizerDesc;
public:
	//ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader;
	//ConstantBuffer<CB_VS_vertexshader_water> cb_vs_vertexshader_water;
	//ConstantBuffer<CB_VS_vertexshader_normal> cb_vs_vertexshader_normal;
	//ConstantBuffer<CB_PS_pixelshader_normal> cb_ps_pixelshader_normal;
public:
	D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_NULL;
	//DirectX::XMFLOAT4X4 worldMatrixLightCube;
	//std::vector<DirectX::XMFLOAT4X4> worldMatricesQuad;
	//std::vector<DirectX::XMFLOAT4X4> worldMatricesCube;
	//std::vector<DirectX::XMFLOAT4X4> worldMatricesPyramid;
private:
	bool InitializeDirectX( HWND hWnd );
	bool InitializeShaders();
	bool InitializeScene();
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
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> waterTexture;
private:
	VertexShader vertexShader;
	VertexShader vertexShader_noLight;
	PixelShader pixelShader;
	PixelShader pixelShader_noLight;
	ConstantBuffer<CB_VS_vertexShader> cb_vs_vertexshader;
	ConstantBuffer<CB_PS_light> cb_ps_light;
	/*VertexShader vertexShaderPrimitive;
	PixelShader pixelShaderPrimitive;
	VertexShader vertexShaderWater;
	PixelShader pixelShaderWater;
	VertexShader vertexShaderNormal;
	PixelShader pixelShaderNormal;*/
private:
	/*VertexBuffer<Vertex_Pos_Nrm> vertexBufferLightCube;
	VertexBuffer<Vertex_Pos_Col> vertexBufferCube;
	VertexBuffer<Vertex_Pos_Col> vertexBufferPyramid;
	VertexBuffer<Vertex_Pos_Tex> vertexBufferQuad;
	IndexBuffer indexBufferCube;
	IndexBuffer indexBufferPyramid;
	IndexBuffer indexBufferQuad;*/
private:
	UINT windowWidth;
	UINT windowHeight;
	ImGuiManager imgui;
};

#endif