#pragma once
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <map>
#include "Sprite.h"
#include "Shaders.h"
#include "Camera2D.h"
#include "Camera3D.h"
#include "ImGuiManager.h"
#include "ObjectIndices.h"
#include "ObjectVertices.h"
#include "RenderableGameObject.h"
#include <dxtk/WICTextureLoader.h>

namespace Bind
{
	class Sampler;
	class Stencil;
	class Rasterizer;
	class DepthStencil;
}

class Graphics
{
	friend class GraphicsResource;
public:
	virtual ~Graphics( void ) = default;
	bool Initialize( HWND hWnd, int width, int height );
	void BeginFrame();
	void RenderFrame();
	void EndFrame();
	void Update( float dt );
	UINT GetWidth() const noexcept;
	UINT GetHeight() const noexcept;
private:
	bool InitializeDirectX( HWND hWnd );
	bool InitializeShaders();
	bool InitializeScene();
private:
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> backBuffer;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> nullRenderTarget = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> nullShaderResourceView = nullptr;

	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> boxTexture;
	
	std::shared_ptr<Bind::DepthStencil> depthStencil;
	std::map<std::string, std::shared_ptr<Bind::Sampler>> samplerStates;
	std::map<std::string, std::shared_ptr<Bind::Stencil>> stencilStates;
	std::map<std::string, std::shared_ptr<Bind::Rasterizer>> rasterizerStates;
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

	ConstantBuffer<CB_VS_fog> cb_vs_fog;
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
	bool rasterizerSolid = true;
	bool samplerAnisotropic = true;
	std::vector<DirectX::XMFLOAT4X4> worldMatricesCube;
};

#endif