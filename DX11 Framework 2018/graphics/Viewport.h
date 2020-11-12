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
			if ( side == Side::Full )
			{
				//viewportDesc = CD3D11_VIEWPORT( 0.0f, 0.0f, width, height );
				viewportDesc.TopLeftX = 0.0f;
				viewportDesc.TopLeftY = 0.0f;
				viewportDesc.Width = width;
				viewportDesc.Height = height;
				viewportDesc.MinDepth = 0.0f;
				viewportDesc.MaxDepth = 1.0f;
			}
			if ( side == Side::Left )
			{
				//viewportDescLeft = CD3D11_VIEWPORT( 0.0f, 0.0f, width, height );
				//viewportDescLeft.Width = width / 2.0f;
				//viewportDescLeft.TopLeftX = 0.0f - width;
				viewportDescLeft.TopLeftX = 0.0f;
				viewportDescLeft.TopLeftY = 0.0f;
				viewportDescLeft.Width = width / 1.385f;
				viewportDescLeft.Height = height;
				viewportDescLeft.MinDepth = 0.0f;
				viewportDescLeft.MaxDepth = 1.0f;
			}
			if ( side == Side::Right )
			{
				//viewportDescRight = CD3D11_VIEWPORT( 0.0f, 0.0f, width / 2.0f, height );
				viewportDescRight.TopLeftX = width / 3.3f;
				viewportDescRight.TopLeftY = 0.0f;
				viewportDescRight.Width = width / 1.385f;
				viewportDescRight.Height = height;
				viewportDescRight.MinDepth = 0.0f;
				viewportDescRight.MaxDepth = 1.0f;
			}
		}
		void Bind( Graphics& gfx ) noexcept override
		{
			GetContext( gfx )->RSSetViewports( 1u, &viewportDesc );
		}
		void BindLeft( Graphics& gfx ) noexcept
		{
			GetContext( gfx )->RSSetViewports( 1u, &viewportDescLeft );
		}
		void BindRight( Graphics& gfx ) noexcept
		{
			GetContext( gfx )->RSSetViewports( 1u, &viewportDescRight );
		}
	private:
		Side side;
		D3D11_VIEWPORT viewportDesc = {};
		D3D11_VIEWPORT viewportDescLeft = {};
		D3D11_VIEWPORT viewportDescRight = {};
	};
}

#endif