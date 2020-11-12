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
		enum class Side
		{
			Full,
			Left,
			Right
		};
		Viewport( Graphics& gfx, Side side ) : Viewport( side, gfx.GetWidth(), gfx.GetHeight() ) {}
		Viewport( Side side, UINT width, UINT height ) : side( side )
		{
			viewportDesc = CD3D11_VIEWPORT( 0.0f, 0.0f, width, height );
			if ( side == Side::Left )
			{
				viewportDesc.Width = width / 1.385f;
			}
			if ( side == Side::Right )
			{
				viewportDesc.TopLeftX = width / 3.3f;
				viewportDesc.Width = width / 1.385f;
			}
		}
		void Bind( Graphics& gfx ) noexcept override
		{
			GetContext( gfx )->RSSetViewports( 1u, &viewportDesc );
		}
	private:
		Side side;
		CD3D11_VIEWPORT viewportDesc = {};
	};
}

#endif