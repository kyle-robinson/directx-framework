#pragma once
#ifndef DEPTHSTENCIL_H
#define DEPTHSTENCIL_H

#include "GraphicsResource.h"

namespace Bind
{
	class DepthStencil : public GraphicsResource
	{
	public:
		DepthStencil( Graphics& gfx ) : DepthStencil( gfx, static_cast<FLOAT>( gfx.GetWidth() ), static_cast<FLOAT>( gfx.GetHeight() ) ) {}
		DepthStencil( Graphics& gfx, float width, float height )
		{
			try
			{
				CD3D11_TEXTURE2D_DESC depthStencilDesc( DXGI_FORMAT_D24_UNORM_S8_UINT, width, height );
				depthStencilDesc.MipLevels = 1;
				depthStencilDesc.SampleDesc.Count = 1;
				depthStencilDesc.SampleDesc.Quality = 0;
				depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

				Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
				HRESULT hr = GetDevice( gfx )->CreateTexture2D( &depthStencilDesc, NULL, depthStencilBuffer.GetAddressOf() );
				hr = GetDevice( gfx )->CreateDepthStencilView( depthStencilBuffer.Get(), NULL, depthStencilView.GetAddressOf() );
				COM_ERROR_IF_FAILED( hr, "Failed to create depth stencil!" );
			}
			catch ( COMException& exception )
			{
				ErrorLogger::Log( exception );
				return;
			}
		}
		void Bind( Graphics& gfx ) noexcept override { }
		void ClearDepthStencil( Graphics& gfx ) noexcept
		{
			GetContext( gfx )->ClearDepthStencilView( depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );
		}
		ID3D11DepthStencilView* GetDepthStencilView() noexcept
		{
			return depthStencilView.Get();
		}
	private:
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	};
}

#endif