#pragma once
#ifndef GRAPHICSRESOURCE_H
#define GRAPHICSRESOURCE_H

#include "Graphics.h"
class Graphics;

class GraphicsResource
{
protected:
	static ID3D11DeviceContext* GetContext( Graphics& gfx ) noexcept;
	static ID3D11Device* GetDevice( Graphics& gfx ) noexcept;
};

#endif