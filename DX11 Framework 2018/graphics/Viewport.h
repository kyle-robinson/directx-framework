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
		Viewport( Graphics& gfx, Side side ) : Viewport( side,
			static_cast<FLOAT>( gfx.GetWidth() ), static_cast<FLOAT>( gfx.GetHeight() ) ) {}
		Viewport( Side side, float width, float height ) : side( side )
		{
			viewportDesc = CD3D11_VIEWPORT( 0.0f, 0.0f, width, height );
			viewportDesc.MinDepth = 0.0f;
			viewportDesc.MaxDepth = 1.0f;
			
			if ( side == Side::Left )
			{
				viewportDesc.Width = width / 1.4f;
			}
			else if ( side == Side::Right )
			{
				viewportDesc.Width = width / 1.4f;
				viewportDesc.TopLeftX = width / 3.35f;
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