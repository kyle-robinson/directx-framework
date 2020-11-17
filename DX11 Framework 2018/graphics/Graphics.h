#pragma once
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <map>
#include "Cube.h"
#include "Plane.h"
#include "Sprite.h"
#include "Shaders.h"
#include "Camera2D.h"
#include "ImGuiManager.h"
#include "RenderableGameObject.h"
#include <dxtk/SpriteFont.h>
#include <dxtk/SpriteBatch.h>
#include <dxtk/WICTextureLoader.h>

namespace Bind
{
	class Sampler;
	class Stencil;
	class Blender;
	class Viewport;
	class SwapChain;
	class Rasterizer;
	class DepthStencil;
	class RenderTarget;
}

class Graphics
{
	friend class GraphicsResource;
public:
	struct SceneParameters
	{
		bool useMask = false;
		bool useTexture = true;
		bool circleMask = true;
		bool multiView = false;
		bool rasterizerSolid = true;
		bool cameraCollision = false;
		float alphaFactor = 1.0f;
		float clearColor[4] = { 0.0f, 0.75f, 1.0f, 1.0f };
	} sceneParams;
	struct ViewportParameters
	{
		bool useFull = true;
		bool useLeft = false;
		bool useRight = false;
		bool useSplit = false;
	} viewportParams;
	struct SamplerParameters
	{
		bool useAnisotropic = true;
		bool useBilinear = false;
		bool usePoint = false;
	} samplerParams;
	struct SpawnWindow
	{
		bool sceneWindow = false;
		bool lightWindow = false;
		bool fogWindow = false;
		bool modelWindow = false;
		bool cameraWindow = false;
		bool stencilWindow = false;
	} spawnWindow;

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
	bool flyCamera = true;
	std::string cameraToUse = "Main";
	std::vector<RenderableGameObject> renderables;
	std::map<std::string, std::shared_ptr<Camera3D>> cameras;
	std::map<std::string, std::shared_ptr<Bind::Viewport>> viewports;
private:
	bool InitializeDirectX( HWND hWnd );
	bool InitializeShaders();
	bool InitializeScene();

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> boxTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> grassTexture;

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
	PixelShader pixelShader_full;
	PixelShader pixelShader_light;
	PixelShader pixelShader_noLight;
	PixelShader pixelShader_2D_discard;

	ConstantBuffer<CB_VS_fog> cb_vs_fog;
	ConstantBuffer<CB_PS_scene> cb_ps_scene;
	ConstantBuffer<CB_PS_light> cb_ps_light;
	ConstantBuffer<CB_VS_matrix> cb_vs_matrix;
	ConstantBuffer<CB_VS_matrix_2D> cb_vs_matrix_2d;
	ConstantBuffer<CB_VS_fullscreen> cb_vs_fullscreen;

	UINT windowWidth;
	UINT windowHeight;
	ImGuiManager imgui;

	Cube cube;
	Camera2D camera2D;
	PlaneInstanced ground;
	PlaneFullscreen fullscreen;
	std::unique_ptr<SpriteFont> spriteFont;
	std::unique_ptr<SpriteBatch> spriteBatch;
};

#endif