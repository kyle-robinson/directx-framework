#pragma once
#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "GraphicsResource.h"
class Graphics;

namespace Bind
{
	class Viewport : public GraphicsResource
	{
	public:
		Viewport( Graphics& gfx ) : Viewport( gfx, static_cast<FLOAT>( gfx.GetWidth() ), static_cast<FLOAT>( gfx.GetHeight() ) ) {}
		Viewport( Graphics& gfx, float width, float height )
		{
			viewportDesc = CD3D11_VIEWPORT( 0.0f, 0.0f, width, height );
			viewportDesc.MinDepth = 0.0f;
			viewportDesc.MaxDepth = 1.0f;
		}
		void Bind( Graphics& gfx ) noexcept override
		{
			GetContext( gfx )->RSSetViewports( 1u, &viewportDesc );
		}
	private:
		CD3D11_VIEWPORT viewportDesc = {};
	};
}

#endif