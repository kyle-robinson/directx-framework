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
	class Blender;
	class SwapChain;
	class Rasterizer;
	class DepthStencil;
	class RenderTarget;
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

	Light light;
	Sprite circle;
	Sprite square;
	Camera2D camera2D;
	Camera3D camera3D;
	std::vector<RenderableGameObject> renderables;
private:
	bool InitializeDirectX( HWND hWnd );
	bool InitializeShaders();
	bool InitializeScene();

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> boxTexture;
	//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> grassTexture;

	std::shared_ptr<Bind::Blender> blendState;
	std::shared_ptr<Bind::SwapChain> swapChain;
	std::shared_ptr<Bind::DepthStencil> depthStencil;
	std::shared_ptr<Bind::RenderTarget> backBuffer;
	std::shared_ptr<Bind::RenderTarget> renderTarget;
	std::map<std::string, std::shared_ptr<Bind::Sampler>> samplerStates;
	std::map<std::string, std::shared_ptr<Bind::Stencil>> stencilStates;
	std::map<std::string, std::shared_ptr<Bind::Rasterizer>> rasterizerStates;

	VertexShader vertexShader_2D;
	VertexShader vertexShader_full;
	//VertexShader vertexShader_quad;
	VertexShader vertexShader_light;
	VertexShader vertexShader_noLight;
	VertexShader vertexShader_lightDirect;

	PixelShader pixelShader_2D;
	PixelShader pixelShader_full;
	//PixelShader pixelShader_quad;
	PixelShader pixelShader_light;
	PixelShader pixelShader_noLight;
	PixelShader pixelShader_2D_discard;
	PixelShader pixelShader_lightDirect;

	VertexBuffer<Vertex3D> vertexBufferCube;
	//VertexBuffer<Vertex3D> vertexBufferQuad;
	VertexBuffer<Vertex_Pos> vertexBufferFullscreen;

	IndexBuffer indexBufferCube;
	//IndexBuffer indexBufferQuad;
	IndexBuffer indexBufferFullscreen;

	ConstantBuffer<CB_VS_fog> cb_vs_fog;
	ConstantBuffer<CB_PS_scene> cb_ps_scene;
	ConstantBuffer<CB_PS_light> cb_ps_light;
	ConstantBuffer<CB_VS_matrix> cb_vs_matrix;
	ConstantBuffer<CB_VS_matrix_2D> cb_vs_matrix_2d;
	ConstantBuffer<CB_VS_fullscreen> cb_vs_fullscreen;
	ConstantBuffer<CB_PS_lightDirect> cb_ps_lightDirect;

	UINT windowWidth;
	UINT windowHeight;
	ImGuiManager imgui;
	bool useMask = false;
	bool useTexture = true;
	bool circleMask = true;
	bool multiView = false;
	bool usePointLight = false;
	bool rasterizerSolid = true;
	bool samplerAnisotropic = true;
	float alphaFactor = 1.0f;
	float clearColor[4] = { 0.0f, 0.75f, 1.0f, 1.0f };
	std::vector<DirectX::XMFLOAT4X4> worldMatricesCube;
};

#endif