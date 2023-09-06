#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/Resizer/LanczosResizerH8_8.h"
#include "Media/Resizer/LanczosResizer8_C8.h"
#include "Sync/ThreadUtil.h"
#include "DotNet/MyPictureBox.h"

#include <windows.h>
#include <ddraw.h>

void DotNet::MyPictureBox::GetDrawingRect(void *rc)
{
	GetClientRect((HWND)hwnd, (RECT*)rc);
	ClientToScreen((HWND)hwnd, (POINT*)&((RECT*)rc)->left);
	ClientToScreen((HWND)hwnd, (POINT*)&((RECT*)rc)->right);
}

void DotNet::MyPictureBox::DrawToScreen()
{
	RECT rcSrc;
	RECT rcDest;
	GetDrawingRect(&rcDest);
	rcSrc.left = 0;
	rcSrc.top = 0;
	rcSrc.right = this->surfaceW;
	rcSrc.bottom = this->surfaceH;
	if (this->pSurface && this->surfaceBuff)
	{
		((LPDIRECTDRAWSURFACE7)this->pSurface)->Blt(&rcDest, (LPDIRECTDRAWSURFACE7)this->surfaceBuff, &rcSrc, 0, 0);
	}
}

void DotNet::MyPictureBox::SizeChg(System::Object *sender, System::EventArgs *e)
{
	this->ReleaseSubSurface();
	this->CreateSubSurface();
	this->UpdateSubSurface();
	this->formCtrl->Invalidate();
}

void DotNet::MyPictureBox::OnPaint(System::Object *sender, System::Windows::Forms::PaintEventArgs *e)
{
	DrawToScreen();
}

void DotNet::MyPictureBox::CreateSurface()
{
	LPDIRECTDRAW7 lpDD = (LPDIRECTDRAW7)ddObj;
	DDSURFACEDESC2 ddsd;
	LPDIRECTDRAWSURFACE7 primarySurface;

	this->ReleaseSurface();
	this->ReleaseSubSurface();

	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags        = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	HRESULT res = lpDD->CreateSurface( &ddsd, &primarySurface, NULL );
	pSurface = primarySurface;
	if (primarySurface)
	{
		CreateSubSurface();
		if (clipper)
			primarySurface->SetClipper((LPDIRECTDRAWCLIPPER)clipper);
	}

	return;
}

void DotNet::MyPictureBox::ReleaseSurface()
{
	if (this->pSurface)
	{
		((LPDIRECTDRAWSURFACE7)this->pSurface)->Release();
		this->pSurface = 0;
	}
}

void DotNet::MyPictureBox::CreateSubSurface()
{
	LPDIRECTDRAW7 lpDD = (LPDIRECTDRAW7)ddObj;
	DDSURFACEDESC2 ddsd;
	RECT rc;
	GetDrawingRect(&rc);
	if (rc.right <= rc.left || rc.bottom <= rc.top)
	{
	}
	else
	{
		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags        = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
		ddsd.dwWidth = rc.right - rc.left;
		ddsd.dwHeight = rc.bottom - rc.top;
		this->surfaceW = ddsd.dwWidth;
		this->surfaceH = ddsd.dwHeight;
		void *surface;
		HRESULT res = lpDD->CreateSurface( &ddsd, (LPDIRECTDRAWSURFACE7*)&surface, NULL );
		this->surfaceBuff = surface;
	}
}

void DotNet::MyPictureBox::ReleaseSubSurface()
{
	if (this->surfaceBuff)
	{
		((LPDIRECTDRAWSURFACE7)this->surfaceBuff)->Release();
		this->surfaceBuff = 0;
	}
}

void DotNet::MyPictureBox::UpdateSubSurface()
{
	if (this->imgBuff && this->currImage && this->surfaceBuff)
	{
		RECT rc;
		DDSURFACEDESC2 ddsd;
		LPDIRECTDRAWSURFACE7 surface = (LPDIRECTDRAWSURFACE7)this->surfaceBuff;

		DDBLTFX ddbltfx;
		ZeroMemory( &ddbltfx, sizeof(ddbltfx) );
		ddbltfx.dwSize      = sizeof(ddbltfx);
		ddbltfx.dwFillColor = 0;
		surface->Blt(0, 0, 0, DDBLT_COLORFILL, &ddbltfx);

		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);

		rc.left = 0;
		rc.top = 0;
		rc.right = this->surfaceW;
		rc.bottom = this->surfaceH;

		if (DD_OK == surface->Lock(&rc, &ddsd, DDLOCK_WAIT, 0))
		{
			Int32 outW;
			Int32 outH;
			UInt8 *ptr = (UInt8*)ddsd.lpSurface;
			((Media::Resizer::LanczosResizer8_C8*)resizer)->SetSrcRGBType(this->currImage->info->rgbType, this->currImage->info->rgbGamma);
			if (this->surfaceW >= this->currImage->info->width && this->surfaceH >= this->currImage->info->height && !this->allowEnlarge)
			{
				if (this->currImage->info->par > 1)
				{
					outW = this->currImage->info->width;
					outH = Math::Double2Int(this->currImage->info->height / this->currImage->info->par);
				}
				else if (this->currImage->info->par < 1)
				{
					outW = Math::Double2Int(this->currImage->info->width * this->currImage->info->par);
					outH = this->currImage->info->height;
				}
				else
				{
					outW = this->currImage->info->width;
					outH = this->currImage->info->height;
				}
				ptr = ptr + ((this->surfaceH - outH) >> 1) * ddsd.lPitch + ((this->surfaceW - outW) >> 1) * 4;
				resizer->Resize(this->imgBuff, this->currImage->info->width << 2, this->currImage->info->width, this->currImage->info->height, 0, 0, ptr, ddsd.lPitch, outW, outH);
			}
			else
			{
				Int32 srcW = this->currImage->info->width;
				Int32 srcH = this->currImage->info->height;
				if (srcW * this->currImage->info->par * this->surfaceH > this->surfaceW * (Double)srcH)
				{
					outW = this->surfaceW;
					outH = Math::Double2Int(this->surfaceW / this->currImage->info->par * srcH / srcW);
				}
				else
				{
					outW = Math::Double2Int(this->surfaceH * this->currImage->info->par * srcW / srcH);
					outH = this->surfaceH;
				}
				ptr = ptr + ((this->surfaceH - outH) >> 1) * ddsd.lPitch + ((this->surfaceW - outW) >> 1) * 4;
				resizer->Resize(this->imgBuff, this->currImage->info->width << 2, this->currImage->info->width, this->currImage->info->height, 0, 0, ptr, ddsd.lPitch, outW, outH);
			}
			surface->Unlock(0);
		}
	}
}

DotNet::MyPictureBox::MyPictureBox(System::Windows::Forms::Control *formCtrl, DotNet::DNColorManager *dnColorMgr, Bool allowEnlarge)
{
	this->formCtrl = formCtrl;
	this->dnColorMgr = dnColorMgr;
	this->dnColorMgr->AddHandler(this);
	this->allowEnlarge = allowEnlarge;
	this->hwnd = formCtrl->Handle.ToPointer();
	this->formCtrl->Paint += new System::Windows::Forms::PaintEventHandler(this, OnPaint);
	this->formCtrl->SizeChanged += new System::EventHandler(this, SizeChg);
	this->formCtrl->BackColor = System::Drawing::Color::Transparent;
	this->clipper = 0;
	this->pSurface = 0;
	this->surfaceBuff = 0;
	this->imgBuff = 0;
	this->currImage = 0;
	this->csconv = 0;
	NEW_CLASS(resizer, Media::Resizer::LanczosResizer8_C8(3, Media::CS::TRANodeType::Unknown, Media::CS::TRANT_DISPLAY, 2.2, this->dnColorMgr->GetColorMgr()));

	LPDIRECTDRAW7 lpDD;
	if (DirectDrawCreateEx( NULL, (VOID**)&lpDD, IID_IDirectDraw7, NULL ) != DD_OK )
	{
	}
	else
	{
		this->ddObj = lpDD;
		lpDD->SetCooperativeLevel((HWND)this->hwnd, DDSCL_NORMAL);
		LPDIRECTDRAWCLIPPER pcClipper;
		lpDD->CreateClipper( 0, &pcClipper, NULL );
		if (pcClipper->SetHWnd( 0, (HWND)this->hwnd ) != DD_OK)
		{
			pcClipper->Release();
			pcClipper = 0;
			return;
		}
		this->clipper = pcClipper;

		CreateSurface();
	}
}

DotNet::MyPictureBox::~MyPictureBox()
{
	LPDIRECTDRAW7 lpDD = (LPDIRECTDRAW7)ddObj;

	this->ReleaseSurface();
	this->ReleaseSubSurface();

	if (clipper)
	{
		((LPDIRECTDRAWCLIPPER)clipper)->Release();
		clipper = 0;
	}
	if (lpDD)
	{
		lpDD->Release();
		ddObj = 0;
	}
	if (this->imgBuff)
	{
		MemFree(this->imgBuff);
		this->imgBuff = 0;
	}
	if (this->csconv)
	{
		DEL_CLASS(this->csconv);
		this->csconv = 0;
	}
	DEL_CLASS(resizer);
	this->dnColorMgr->RemoveHandler(this);
}

void DotNet::MyPictureBox::SetImage(Media::Image *currImage)
{
	this->currImage = currImage;
	if (this->imgBuff)
	{
		MemFree(this->imgBuff);
		this->imgBuff = 0;
	}
	if (this->csconv)
	{
		DEL_CLASS(this->csconv);
		this->csconv = 0;
	}
	if (this->currImage)
	{
		this->csconv = Media::CS::CSConverter::NewConverter(this->currImage->info->fourcc, this->currImage->info->bpp, this->currImage->info->rgbType, 0, 32, Media::CS::TRANodeType::Unknown, this->currImage->info->rgbGamma, this->currImage->info->yuvType, this->dnColorMgr->GetColorMgr());
		if (this->csconv)
		{
			if (this->currImage->pal)
			{
				this->csconv->SetPalette(this->currImage->pal);
			}
			this->imgBuff = MemAlloc(UInt8, this->currImage->info->width * this->currImage->info->height * 4);
			if (this->currImage->GetImageType() == Media::Image::ImageType::Static)
			{
				this->csconv->Convert(((Media::StaticImage*)this->currImage)->data, this->imgBuff, this->currImage->info->width, this->currImage->info->height, this->currImage->GetBpl(), this->currImage->info->width << 2);
			}
			else
			{
				UInt8 *imgData = MemAlloc(UInt8, this->currImage->GetBpl() * this->currImage->info->height);
				this->currImage->GetImageData(imgData, 0, 0, this->currImage->info->width, this->currImage->info->height, this->currImage->GetBpl(), false, Media::RotateType::None);
				this->csconv->Convert(imgData, this->imgBuff, this->currImage->info->width, this->currImage->info->height, this->currImage->GetBpl(), this->currImage->info->width << 2);
				MemFree(imgData);
			}
			this->UpdateSubSurface();
			this->formCtrl->Refresh();
		}
	}
}

void DotNet::MyPictureBox::YUVParamChanged(NotNullPtr<const Media::IColorHandler::YUVPARAM> yuvParam)
{
	if (this->currImage && this->csconv)
	{
		if (this->currImage->GetImageType() == Media::Image::ImageType::Static)
		{
			this->csconv->Convert(((Media::StaticImage*)this->currImage)->data, this->imgBuff, this->currImage->info->width, this->currImage->info->height, this->currImage->info->width * (this->currImage->info->bpp >> 3), this->currImage->info->width << 2);
		}
		else
		{
			UInt8 *imgData = MemAlloc(UInt8, this->currImage->GetBpl() * this->currImage->info->height);
			this->currImage->GetImageData(imgData, 0, 0, this->currImage->info->width, this->currImage->info->height, this->currImage->GetBpl(), false, Media::RotateType::None);
			this->csconv->Convert(imgData, this->imgBuff, this->currImage->info->width, this->currImage->info->height, this->currImage->info->width * (this->currImage->info->bpp >> 3), this->currImage->info->width << 2);
			MemFree(imgData);
		}
		UpdateSubSurface();
		DrawToScreen();
	}
}

void DotNet::MyPictureBox::RGBParamChanged(const Media::IColorHandler::RGBPARAM *rgbParam)
{
	if (this->currImage && this->csconv)
	{
		if (this->currImage->GetImageType() == Media::Image::ImageType::Static)
		{
			this->csconv->Convert(((Media::StaticImage*)this->currImage)->data, this->imgBuff, this->currImage->info->width, this->currImage->info->height, this->currImage->info->width * (this->currImage->info->bpp >> 3), this->currImage->info->width << 2);
		}
		else
		{
			UInt8 *imgData = MemAlloc(UInt8, this->currImage->GetBpl() * this->currImage->info->height);
			this->currImage->GetImageData(imgData, 0, 0, this->currImage->info->width, this->currImage->info->height, this->currImage->GetBpl(), false, Media::RotateType::None);
			this->csconv->Convert(imgData, this->imgBuff, this->currImage->info->width, this->currImage->info->height, this->currImage->info->width * (this->currImage->info->bpp >> 3), this->currImage->info->width << 2);
			MemFree(imgData);
		}
		UpdateSubSurface();
		DrawToScreen();
	}
}

void DotNet::MyPictureBox::SetAllowEnlarge(Bool allowEnlarge)
{
	if (this->allowEnlarge != allowEnlarge)
	{
		this->allowEnlarge = allowEnlarge;
		UpdateSubSurface();
		DrawToScreen();
	}
}
