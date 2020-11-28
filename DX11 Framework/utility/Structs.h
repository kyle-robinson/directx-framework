#pragma once
#ifndef STRUCTS_H
#define STRUCTS_H

struct SceneParameters
{
	bool useMask = false;
	bool useTexture = true;
	bool circleMask = true;
	bool multiView = false;
	bool rasterizerSolid = true;
	bool cameraCollision = false;
	bool useBillboarding = false;
	float alphaFactor = 1.0f;
	float clearColor[4] = { 0.0f, 0.75f, 1.0f, 1.0f };
} sceneParams;

struct ViewportParameters
{
	bool useFull = true;
	bool useLeft = false;
	bool useRight = false;
	bool useSplit = false;
	bool controlLeftSide = true;
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

struct LightParameters
{
	float alphaFactor = 1.0f;
	bool useTexture = true;
	bool lightStuck = false;
	bool lightHover = false;
	bool isEquippable = false;
	bool lightFlicker = false;
	float flickerAmount = 2.0f;
	bool lightIntersection = false;
} lightParams;

struct StencilOutline
{
	XMFLOAT3 outlineColor = { 1.0f, 0.0f, 0.0f };
	float outlineSize = 1.3f;
} outlineParams;

#endif