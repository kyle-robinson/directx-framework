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
	RenderableGameObject mill;
	RenderableGameObject home;
	RenderableGameObject town;
	RenderableGameObject building;
	RenderableGameObject lighthouse;
	RenderableGameObject nanosuit;
	std::vector<RenderableGameObject> renderableObjects;
	ConstantBuffer<CB_PS_light> cb_ps_light;
private:
	bool InitializeDirectX( HWND hWnd );
	bool InitializeShaders();
	bool InitializeScene();

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> boxTexture;

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