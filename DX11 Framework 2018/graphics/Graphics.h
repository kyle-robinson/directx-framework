#pragma once
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Sprite.h"
#include "Shaders.h"
#include "Camera2D.h"
#include "Camera3D.h"
#include "ImGuiManager.h"
#include "ObjectIndices.h"
#include "ObjectVertices.h"
#include "RenderableGameObject.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
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
private:
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> backBuffer;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> nullRenderTarget = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> nullShaderResourceView = nullptr;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState_drawMask;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState_writeMask;

	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> boxTexture;
protected:
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState_Solid;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState_Wireframe;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState_Anisotropic;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState_Point;
public:
	Light light;
	Sprite circle;
	Sprite square;
	Camera2D camera2D;
	Camera3D camera3D;
	RenderableGameObject nanosuit;
	ConstantBuffer<CB_PS_light> cb_ps_light;
private:
	VertexShader vertexShader_2D;
	VertexShader vertexShader_full;
	VertexShader vertexShader_light;
	VertexShader vertexShader_noLight;

	PixelShader pixelShader_2D;
	PixelShader pixelShader_2D_discard;
	PixelShader pixelShader_full;
	PixelShader pixelShader_light;
	PixelShader pixelShader_noLight;

	VertexBuffer<Vertex3D> vertexBufferCube;
	VertexBuffer<Vertex_Pos> vertexBufferFullscreen;

	IndexBuffer indexBufferCube;
	IndexBuffer indexBufferFullscreen;

	ConstantBuffer<CB_VS_matrix> cb_vs_matrix;
	ConstantBuffer<CB_VS_matrix_2D> cb_vs_matrix_2d;
	ConstantBuffer<CB_VS_fullscreen> cb_vs_fullscreen;
private:
	UINT windowWidth;
	UINT windowHeight;
	ImGuiManager imgui;
	float clearColor[4];
	bool useMask = false;
	bool circleMask = true;
	bool multiView = false;
	bool useTexture = true;
	float alphaFactor = 1.0f;
	bool rasterizerSolid = true;
	bool samplerAnisotropic = true;
	std::vector<DirectX::XMFLOAT4X4> worldMatricesCube;
};

#endif