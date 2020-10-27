#pragma once
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Camera.h"
#include "Shaders.h"
#include "ImGuiManager.h"
#include "ObjectIndices.h"
#include "ObjectVertices.h"
#include "RenderableGameObject.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include <dxtk/WICTextureLoader.h>

class Graphics
{
public:
	bool Initialize( HWND hWnd, int width, int height );
	void BeginFrame();
	void RenderFrame();
	void EndFrame();
	void Update( float dt );
private:
	bool InitializeDirectX( HWND hWnd );
	bool InitializeShaders();
	bool InitializeScene();
public:
	Light light;
	Camera camera;
	RenderableGameObject nanosuit;
	ConstantBuffer<CB_PS_light> cb_ps_light;
	ConstantBuffer<CB_VS_matrix> cb_vs_matrix;
private:
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> boxTexture;
protected:
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState_Solid;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState_Wireframe;
private:
	VertexShader vertexShader;
	VertexShader vertexShader_noLight;
	PixelShader pixelShader;
	PixelShader pixelShader_noLight;
	VertexBuffer<Vertex> vertexBufferCube;
	IndexBuffer indexBufferCube;
private:
	UINT windowWidth;
	UINT windowHeight;
	ImGuiManager imgui;
	float clearColor[4];
	bool useTexture = true;
	std::vector<DirectX::XMFLOAT4X4> worldMatricesCube;
};

#endif