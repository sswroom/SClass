#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/ImageCopyC.h"
#include "Media/ImageUtil.h"
#include "Media/LRGBLimiterC.h"
#include "Media/CS/TransferFunc.h"
#include "Media/Resizer/LanczosResizerLR_C32.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "UI/GUIClientControl.h"
#include "UI/GUIPictureBoxDD.h"
#include "UI/MessageDialog.h"

void UI::GUIPictureBoxDD::UpdateSubSurface()
{
	if (this->imgBuff && this->currImage && this->IsSurfaceReady())
	{
		UOSInt bpl;
		if (this->drawHdlrs.GetCount() > 0)
		{
			if (this->bgBuff == 0 || this->bkBuffSize != this->bgBuffSize)
			{
				if (this->bgBuff)
				{
					MemFreeA(this->bgBuff);
				}
				this->bgBuffSize = this->bkBuffSize;
				this->bgBuff = MemAllocA(UInt8, this->bgBuffSize.CalcArea() * 4);
			}

			Math::RectAreaDbl srcRect;
			Math::RectArea<OSInt> destRect;
			ImageUtil_ImageColorFill32(this->bgBuff, this->bgBuffSize.x, this->bgBuffSize.y, this->bgBuffSize.x << 2, 0);

			if (this->mouseDowned)
			{
				Math::Coord2DDbl oldCenter = this->zoomCenter;
				this->zoomCenter += (this->mouseDownPos - this->mouseCurrPos).ToDouble() / this->zoomScale;
				if (this->zoomScale != this->zoomMinScale)
				{
					if (this->zoomCenter.x < this->zoomMinX)
						this->zoomCenter.x = this->zoomMinX;
					else if (this->zoomCenter.x > this->zoomMaxX)
						this->zoomCenter.x = this->zoomMaxX;
				}

				if (this->zoomCenter.y < this->zoomMinY)
					this->zoomCenter.y = this->zoomMinY;
				else if (this->zoomCenter.y > this->zoomMaxY)
					this->zoomCenter.y = this->zoomMaxY;
				CalDispRect(&srcRect, &destRect);
				this->zoomCenter = oldCenter;
			}
			else if (this->gzoomDown)
			{
				Math::Coord2DDbl oldCenter = this->zoomCenter;
				Double oldScale = this->zoomScale;
				Double oldMinX = this->zoomMinX;
				Double oldMaxX = this->zoomMaxX;
				Double oldMinY = this->zoomMinY;
				Double oldMaxY = this->zoomMaxY;
				this->zoomScale = this->zoomScale * UInt64_Double(this->gzoomCurrDist) / UInt64_Double(this->gzoomDownDist);
				if (this->zoomScale < this->zoomMinScale)
				{
					this->zoomScale = this->zoomMinScale;
				}
				else if (this->zoomScale > 16)
				{
					this->zoomScale = 16.0;
				}
				this->UpdateZoomRange();

				this->zoomCenter += (this->gzoomDownPos - this->gzoomCurrPos).ToDouble() / this->zoomScale;
				if (this->zoomCenter.x < this->zoomMinX)
					this->zoomCenter.x = this->zoomMinX;
				else if (this->zoomCenter.x > this->zoomMaxX)
					this->zoomCenter.x = this->zoomMaxX;

				if (this->zoomCenter.y < this->zoomMinY)
					this->zoomCenter.y = this->zoomMinY;
				else if (this->zoomCenter.y > this->zoomMaxY)
					this->zoomCenter.y = this->zoomMaxY;
				CalDispRect(&srcRect, &destRect);
				this->zoomCenter = oldCenter;
				this->zoomScale = oldScale;
				this->zoomMinX = oldMinX;
				this->zoomMaxX = oldMaxX;
				this->zoomMinY = oldMinY;
				this->zoomMaxY = oldMaxY;
			}
			else
			{
				CalDispRect(&srcRect, &destRect);
			}
			UInt8 *dptr = this->bgBuff;
			Int32 iLeft = (Int32)srcRect.tl.x;
			Int32 iTop = (Int32)srcRect.tl.y;
			dptr = dptr + destRect.tl.y * (OSInt)(this->bgBuffSize.x << 2) + destRect.tl.x * 4;
			Math::Size2DDbl srcSize = srcRect.GetSize();
			Math::Size2D<OSInt> destSize = destRect.GetSize();
			resizer->Resize(this->imgBuff + iLeft * 8 + iTop * (OSInt)this->currImageSize.x * 8, (OSInt)this->currImageSize.x << 3, srcSize.x, srcSize.y, srcRect.tl.x - iLeft, srcRect.tl.y - iTop, dptr, (OSInt)this->bgBuffSize.x << 2, (UOSInt)destSize.x, (UOSInt)destSize.y);

			this->DrawFromBG();
		}
		else
		{
			UInt8 *dptr = this->LockSurfaceBegin(this->bkBuffSize.x, this->bkBuffSize.y, &bpl);
			if (dptr)
			{
				Math::RectAreaDbl srcRect;
				Math::RectArea<OSInt> destRect;
				ImageUtil_ImageColorFill32(dptr, this->bkBuffSize.x, this->bkBuffSize.y, bpl, 0);

				if (this->mouseDowned)
				{
					Math::Coord2DDbl oldCenter = this->zoomCenter;
					this->zoomCenter += (this->mouseDownPos - this->mouseCurrPos).ToDouble() / this->zoomScale;
					if (this->zoomScale != this->zoomMinScale)
					{
						if (this->zoomCenter.x < this->zoomMinX)
							this->zoomCenter.x = this->zoomMinX;
						else if (this->zoomCenter.x > this->zoomMaxX)
							this->zoomCenter.x = this->zoomMaxX;
					}

					if (this->zoomCenter.y < this->zoomMinY)
						this->zoomCenter.y = this->zoomMinY;
					else if (this->zoomCenter.y > this->zoomMaxY)
						this->zoomCenter.y = this->zoomMaxY;
					CalDispRect(&srcRect, &destRect);
					this->zoomCenter = oldCenter;
				}
				else if (this->gzoomDown)
				{
					Math::Coord2DDbl oldCenter = this->zoomCenter;
					Double oldScale = this->zoomScale;
					Double oldMinX = this->zoomMinX;
					Double oldMaxX = this->zoomMaxX;
					Double oldMinY = this->zoomMinY;
					Double oldMaxY = this->zoomMaxY;
					this->zoomScale = this->zoomScale * UInt64_Double(this->gzoomCurrDist) / UInt64_Double(this->gzoomDownDist);
					if (this->zoomScale < this->zoomMinScale)
					{
						this->zoomScale = this->zoomMinScale;
					}
					else if (this->zoomScale > 16)
					{
						this->zoomScale = 16.0;
					}
					this->UpdateZoomRange();

					this->zoomCenter += (this->gzoomDownPos - this->gzoomCurrPos).ToDouble() / this->zoomScale;
					if (this->zoomCenter.x < this->zoomMinX)
						this->zoomCenter.x = this->zoomMinX;
					else if (this->zoomCenter.x > this->zoomMaxX)
						this->zoomCenter.x = this->zoomMaxX;

					if (this->zoomCenter.y < this->zoomMinY)
						this->zoomCenter.y = this->zoomMinY;
					else if (this->zoomCenter.y > this->zoomMaxY)
						this->zoomCenter.y = this->zoomMaxY;
					CalDispRect(&srcRect, &destRect);
					this->zoomCenter = oldCenter;
					this->zoomScale = oldScale;
					this->zoomMinX = oldMinX;
					this->zoomMaxX = oldMaxX;
					this->zoomMinY = oldMinY;
					this->zoomMaxY = oldMaxY;
				}
				else
				{
					CalDispRect(&srcRect, &destRect);
				}
				Int32 iLeft = (Int32)srcRect.tl.x;
				Int32 iTop = (Int32)srcRect.tl.y;
				dptr = dptr + destRect.tl.y * (OSInt)bpl + destRect.tl.x * 4;
				Math::Size2DDbl srcSize = srcRect.GetSize();
				if (srcSize.x > 0 && srcSize.y > 0)
				{
					resizer->Resize(this->imgBuff + iLeft * 8 + iTop * (OSInt)this->currImageSize.x * 8, (OSInt)this->currImageSize.x << 3, srcSize.x, srcSize.y, srcRect.tl.x - iLeft, srcRect.tl.y - iTop, dptr, (OSInt)bpl, (UOSInt)destRect.GetWidth(), (UOSInt)destRect.GetHeight());
				}

				this->LockSurfaceEnd();
			}
		}
	}
}

void UI::GUIPictureBoxDD::CalDispRect(Math::RectAreaDbl *srcRect, Math::RectArea<OSInt> *destRect)
{
	if (this->currImage == 0)
	{
		srcRect->tl = Math::Coord2DDbl(0, 0);
		srcRect->br = Math::Coord2DDbl(0, 0);
		destRect->tl = Math::Coord2D<OSInt>(0, 0);
		destRect->br = Math::Coord2D<OSInt>(0, 0);
		return;
	}

	srcRect->tl = this->zoomCenter - this->bkBuffSize.ToDouble() * 0.5 / this->zoomScale;
	srcRect->br = srcRect->tl + this->bkBuffSize.ToDouble() / this->zoomScale;
	destRect->tl = Math::Coord2D<OSInt>(0, 0);
	destRect->br = Math::Coord2D<OSInt>((OSInt)this->bkBuffSize.x, (OSInt)this->bkBuffSize.y);

	if (srcRect->tl.x < 0)
	{
		destRect->tl.x += Double2OSInt(-srcRect->tl.x * this->zoomScale);
		srcRect->tl.x = 0;
	}
	if (srcRect->br.x > UOSInt2Double(this->currImageSize.x))
	{
		srcRect->br.x = UOSInt2Double(this->currImageSize.x);
		destRect->br.x = Double2OSInt(srcRect->GetWidth() * this->zoomScale) + destRect->tl.x;
	}
	if (srcRect->tl.y < 0)
	{
		destRect->tl.y = Double2OSInt(-srcRect->tl.y * this->zoomScale);
		srcRect->tl.y = 0;
	}
	if (srcRect->br.y > UOSInt2Double(this->currImageSize.y))
	{
		srcRect->br.y = UOSInt2Double(this->currImageSize.y);
		destRect->br.y = Double2OSInt(srcRect->GetHeight() * this->zoomScale) + destRect->tl.y;
	}
}

void UI::GUIPictureBoxDD::UpdateZoomRange()
{
	if (UOSInt2Double(this->bkBuffSize.x) > UOSInt2Double(this->currImageSize.x) * zoomScale)
	{
		this->zoomMaxX = UOSInt2Double(this->bkBuffSize.x) * 0.5 / zoomScale;
		this->zoomMinX = UOSInt2Double(this->currImageSize.x) - UOSInt2Double(this->bkBuffSize.x) * 0.5 / zoomScale;
	}
	else
	{
		this->zoomMinX = (UOSInt2Double(this->bkBuffSize.x) * 0.5) / zoomScale;
		this->zoomMaxX = UOSInt2Double(this->currImageSize.x) - UOSInt2Double(this->bkBuffSize.x) * 0.5 / zoomScale;
	}
	if (UOSInt2Double(this->bkBuffSize.y) > UOSInt2Double(this->currImageSize.y) * zoomScale)
	{
		this->zoomMaxY = UOSInt2Double(this->bkBuffSize.y) * 0.5 / zoomScale;
		this->zoomMinY = UOSInt2Double(this->currImageSize.y) - UOSInt2Double(this->bkBuffSize.y) * 0.5 / zoomScale;
	}
	else
	{
		this->zoomMinY = (UOSInt2Double(this->bkBuffSize.y) * 0.5) / zoomScale;
		this->zoomMaxY = UOSInt2Double(this->currImageSize.y) - UOSInt2Double(this->bkBuffSize.y) * 0.5 / zoomScale;
	}
}

void UI::GUIPictureBoxDD::UpdateMinScale()
{
	Double outZoomScale;
	Double outW;
//	Double outH;
	Double srcW = UOSInt2Double(this->currImageSize.x);
	Double srcH = UOSInt2Double(this->currImageSize.y);
	if (srcW * this->currImage->info.par2 * UOSInt2Double(this->bkBuffSize.y) > UOSInt2Double(this->bkBuffSize.x) * srcH)
	{
		outW = UOSInt2Double(this->bkBuffSize.x);
//		outH = this->surfaceSize.x / this->currImage->info.par2 * srcH / srcW;
	}
	else
	{
		outW = UOSInt2Double(this->bkBuffSize.y) * this->currImage->info.par2 * srcW / srcH;
//		outH = OSInt2Double(this->surfaceSize.y);
	}
	outZoomScale = outW / srcW;
	if (outZoomScale > 1.0)
	{
		outZoomScale = 1.0;
	}
	this->zoomMinScale = outZoomScale;
}

void UI::GUIPictureBoxDD::CreateResizer()
{
	Media::ColorProfile destColor(Media::ColorProfile::CPT_PDISPLAY);

	SDEL_CLASS(this->resizer);
	Double refLuminance = 0;
	if (this->currImage)
	{
		refLuminance = Media::CS::TransferFunc::GetRefLuminance(this->currImage->info.color.rtransfer);
	}
	if (this->curr10Bit)
	{
		NEW_CLASS(this->resizer, Media::Resizer::LanczosResizerLR_C32(4, 3, destColor, this->colorSess, Media::AT_NO_ALPHA, refLuminance, Media::PF_LE_A2B10G10R10));
	}
	else
	{
		NEW_CLASS(this->resizer, Media::Resizer::LanczosResizerLR_C32(4, 3, destColor, this->colorSess, Media::AT_NO_ALPHA, refLuminance, this->GetPixelFormat()));
	}
}

void __stdcall UI::GUIPictureBoxDD::OnSizeChg(void *userObj)
{
	UI::GUIPictureBoxDD *me = (UI::GUIPictureBoxDD*)userObj;
	if (me->switching)
		return;
	Bool new10Bit = false;
	if (me->currScnMode == SM_VFS || me->currScnMode == SM_FS || me->currScnMode == SM_WINDOWED_DIR)
	{
		new10Bit = me->colorSess->Get10BitColor();
	}
	if (me->currImage)
	{
		me->UpdateMinScale();
		me->UpdateZoomRange();
	}
	if (me->curr10Bit != new10Bit)
	{
		me->curr10Bit = new10Bit;
		me->CreateResizer();
		me->UpdateSubSurface();
		me->DrawToScreen();
	}
}

void UI::GUIPictureBoxDD::DrawFromBG()
{
	UOSInt bpl;
	UInt8 *dptr = this->LockSurfaceBegin(this->bgBuffSize.x, this->bgBuffSize.y, &bpl);
	if (dptr)
	{
		UOSInt i;
		UOSInt j;
		ImageCopy_ImgCopy(this->bgBuff, dptr, this->bgBuffSize.x << 2, this->bgBuffSize.y, (OSInt)this->bgBuffSize.x << 2, (OSInt)bpl);
		i = 0;
		j = this->drawHdlrs.GetCount();
		while (i < j)
		{
			this->drawHdlrs.GetItem(i)(this->drawObjs.GetItem(i), dptr, this->bgBuffSize.x, this->bgBuffSize.x, bpl);
			i++;
		}
		this->LockSurfaceEnd();
	}
}

void UI::GUIPictureBoxDD::OnPaint()
{
	if (this->currScnMode != SM_FS && this->currScnMode != SM_VFS)
	{
		if (this->bgBuff && this->drawHdlrs.GetCount() > 0)
		{
			DrawFromBG();
		}

		DrawToScreen();
	}
	else
	{
//		PAINTSTRUCT ps;
//		BeginPaint((HWND)this->hwnd, &ps);
//		EndPaint((HWND)this->hwnd, &ps);
	}
}

UI::GUIPictureBoxDD::GUIPictureBoxDD(NotNullPtr<UI::GUICore> ui, UI::GUIClientControl *parent, Media::ColorManagerSess *colorSess, Bool allowEnlarge, Bool directMode) : UI::GUIDDrawControl(ui, parent, directMode, colorSess)
{
	this->colorSess = colorSess;
	this->colorSess->AddHandler(this);
	this->enableLRGBLimit = false;

	this->bgBuff = 0;
	this->bgBuffSize = Math::Size2D<UOSInt>(0, 0);
	this->allowEnlarge = allowEnlarge;
	this->currImage = 0;
	this->currImageSize = Math::Size2D<UOSInt>(0, 0);
	this->csconv = 0;
	this->imgBuff = 0;
	this->zoomScale = 1.0;
	this->zoomMinScale = 1.0;
	this->mouseDowned = false;
	this->curr10Bit = false;
	this->resizer = 0;
	this->zoomCenter = Math::Coord2DDbl(0, 0);
	this->gzoomDown = false;

	CreateResizer();
	this->HandleSizeChanged(OnSizeChg, this);
}

UI::GUIPictureBoxDD::~GUIPictureBoxDD()
{
	if (this->imgBuff)
	{
		MemFreeA(this->imgBuff);
		this->imgBuff = 0;
	}
	if (this->csconv)
	{
		DEL_CLASS(this->csconv);
		this->csconv = 0;
	}
	DEL_CLASS(this->resizer);
	if (this->bgBuff)
	{
		MemFreeA(this->bgBuff);
		this->bgBuff = 0;
	}
	this->colorSess->RemoveHandler(this);
}

Text::CString UI::GUIPictureBoxDD::GetObjectClass()
{
	return CSTR("PictureBoxDD");
}

OSInt UI::GUIPictureBoxDD::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GUIPictureBoxDD::ChangeMonitor(MonitorHandle *hMon)
{
	this->currMon = hMon;

	Media::MonitorInfo *monInfo;
	NEW_CLASS(monInfo, Media::MonitorInfo(hMon));
	this->scnX = monInfo->GetLeft();
	this->scnY = monInfo->GetTop();
	DEL_CLASS(monInfo);

	if (this->currScnMode == SM_WINDOWED_DIR)
	{
		this->CreateSurface();
		this->UpdateSubSurface();
		this->DrawToScreen();
	}
}

void UI::GUIPictureBoxDD::SetDPI(Double hdpi, Double ddpi)
{
	Double oldHDPI = this->hdpi;
	Double oldDDPI = this->ddpi;
	this->hdpi = hdpi;
	this->ddpi = ddpi;
	this->zoomScale = this->zoomScale * hdpi / ddpi * oldDDPI / oldHDPI;
	this->UpdatePos(true);
	if (this->zoomScale < this->zoomMinScale)
	{
		this->zoomScale = this->zoomMinScale;
		this->UpdateSubSurface();
		this->DrawToScreen();
	}
	if (this->hFont)
	{
		this->InitFont();
	}
	else
	{
		this->UpdateFont();
	}
}

void UI::GUIPictureBoxDD::EnableLRGBLimit(Bool enable)
{
	if (this->enableLRGBLimit != enable)
	{
		this->enableLRGBLimit = enable;
	}
}

void UI::GUIPictureBoxDD::SetImage(Media::Image *currImage, Bool sameImg)
{
	Math::Size2D<UOSInt> oriSize = this->currImageSize;
	this->currImage = currImage;
	if (this->imgBuff)
	{
		MemFreeA(this->imgBuff);
		this->imgBuff = 0;
	}
	if (this->csconv)
	{
		DEL_CLASS(this->csconv);
		this->csconv = 0;
	}
	if (this->currImage)
	{
		Media::RotateType rotType = Media::RotateType::None;
		this->currImageSize = this->currImage->info.dispSize;
		if (this->currImage->exif)
		{
			rotType = this->currImage->exif->GetRotateType();
			if (rotType == Media::RotateType::CW_90)
			{
				this->currImageSize = this->currImage->info.dispSize.SwapXY();
			}
			else if (rotType == Media::RotateType::CW_180)
			{
			}
			else if (rotType == Media::RotateType::CW_270)
			{
				this->currImageSize = this->currImage->info.dispSize.SwapXY();
			}
		}
		if (!sameImg || oriSize != this->currImageSize)
		{
			this->zoomCenter = this->currImageSize.ToDouble() * 0.5;
			this->UpdateMinScale();
			this->zoomScale = this->zoomMinScale;
			this->UpdateZoomRange();
		}
		Media::ColorProfile color(Media::ColorProfile::CPT_PDISPLAY);
		this->csconv = Media::CS::CSConverter::NewConverter(this->currImage->info.fourcc, this->currImage->info.storeBPP, this->currImage->info.pf, this->currImage->info.color, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, color, this->currImage->info.yuvType, this->colorSess);
		if (this->csconv)
		{
			if (this->currImage->pal)
			{
				this->csconv->SetPalette(this->currImage->pal);
			}
			this->imgBuff = MemAllocA(UInt8, this->currImageSize.CalcArea() * 8);
			if (this->currImage->GetImageType() == Media::Image::ImageType::Static)
			{
				this->csconv->ConvertV2(&((Media::StaticImage*)this->currImage)->data, this->imgBuff, this->currImage->info.dispSize.x, this->currImage->info.dispSize.y, this->currImage->info.storeSize.x, this->currImage->info.storeSize.y, (OSInt)this->currImage->info.dispSize.x << 3, this->currImage->info.ftype, this->currImage->info.ycOfst);
			}
			else
			{
				UInt8 *imgData = MemAllocA(UInt8, this->currImage->GetDataBpl() * this->currImage->info.storeSize.y);
				this->currImage->GetImageData(imgData, 0, 0, this->currImage->info.storeSize.x, this->currImage->info.dispSize.y, this->currImage->GetDataBpl(), this->currImage->IsUpsideDown(), this->currImage->info.rotateType);
				this->csconv->ConvertV2(&imgData, this->imgBuff, this->currImage->info.dispSize.x, this->currImage->info.dispSize.y, this->currImage->info.storeSize.x, this->currImage->info.storeSize.y, (OSInt)this->currImage->info.dispSize.x << 3, this->currImage->info.ftype, this->currImage->info.ycOfst);
				MemFreeA(imgData);
			}
			if (this->enableLRGBLimit)
			{
				LRGBLimiter_LimitImageLRGB(this->imgBuff, this->currImage->info.dispSize.x, this->currImage->info.dispSize.y);
			}

			if (rotType == Media::RotateType::CW_90)
			{
				UInt8 *tmpBuff = MemAllocA(UInt8, this->currImageSize.CalcArea() * 8);
				ImageUtil_Rotate64_CW90(this->imgBuff, tmpBuff, this->currImageSize.y, this->currImageSize.x, this->currImageSize.y << 3, this->currImageSize.x << 3);
				MemFreeA(this->imgBuff);
				this->imgBuff = tmpBuff;
			}
			else if (rotType == Media::RotateType::CW_180)
			{
				UInt8 *tmpBuff = MemAllocA(UInt8, this->currImageSize.CalcArea() * 8);
				ImageUtil_Rotate64_CW180(this->imgBuff, tmpBuff, this->currImageSize.x, this->currImageSize.y, this->currImageSize.x << 3, this->currImageSize.x << 3);
				MemFreeA(this->imgBuff);
				this->imgBuff = tmpBuff;
			}
			else if (rotType == Media::RotateType::CW_270)
			{
				UInt8 *tmpBuff = MemAllocA(UInt8, this->currImageSize.CalcArea() * 8);
				ImageUtil_Rotate64_CW270(this->imgBuff, tmpBuff, this->currImageSize.y, this->currImageSize.x, this->currImageSize.y << 3, this->currImageSize.x << 3);
				MemFreeA(this->imgBuff);
				this->imgBuff = tmpBuff;
			}
			this->UpdateSubSurface();
			DrawToScreen();
		}
	}
	else
	{
		this->currImageSize = Math::Size2D<UOSInt>(0, 0);
	}
}

void UI::GUIPictureBoxDD::YUVParamChanged(NotNullPtr<const Media::IColorHandler::YUVPARAM> yuvParam)
{
	if (this->currImage && this->csconv)
	{
		Media::RotateType rotType = Media::RotateType::None;
		UInt8 *imgBuff;
		if (this->currImage->exif)
		{
			rotType = this->currImage->exif->GetRotateType();
		}
		if (rotType == Media::RotateType::None)
		{
			imgBuff = this->imgBuff;
		}
		else
		{
			imgBuff = MemAllocA(UInt8, this->currImageSize.CalcArea() * 8);
		}
		if (this->currImage->GetImageType() == Media::Image::ImageType::Static)
		{
			this->csconv->ConvertV2(&((Media::StaticImage*)this->currImage)->data, imgBuff, this->currImage->info.dispSize.x, this->currImage->info.dispSize.y, this->currImage->info.storeSize.x, this->currImage->info.storeSize.y, (OSInt)this->currImage->info.dispSize.x << 3, this->currImage->info.ftype, this->currImage->info.ycOfst);
		}
		else
		{
			UInt8 *imgData = MemAllocA(UInt8, this->currImage->GetDataBpl() * this->currImage->info.storeSize.y);
			this->currImage->GetImageData(imgData, 0, 0, this->currImage->info.storeSize.x, this->currImage->info.dispSize.y, this->currImage->GetDataBpl(), this->currImage->IsUpsideDown(), this->currImage->info.rotateType);
			this->csconv->ConvertV2(&imgData, imgBuff, this->currImage->info.dispSize.x, this->currImage->info.dispSize.y, this->currImage->info.storeSize.x, this->currImage->info.storeSize.y, (OSInt)this->currImage->info.dispSize.x << 3, this->currImage->info.ftype, this->currImage->info.ycOfst);
			MemFreeA(imgData);
		}
		if (this->enableLRGBLimit)
		{
			LRGBLimiter_LimitImageLRGB(imgBuff, this->currImage->info.dispSize.x, this->currImage->info.dispSize.y);
		}
		if (rotType == Media::RotateType::CW_90)
		{
			ImageUtil_Rotate64_CW90(imgBuff, this->imgBuff, this->currImageSize.y, this->currImageSize.x, this->currImageSize.y << 3, this->currImageSize.x << 3);
			MemFreeA(imgBuff);
		}
		else if (rotType == Media::RotateType::CW_180)
		{
			ImageUtil_Rotate64_CW180(imgBuff, this->imgBuff, this->currImageSize.x, this->currImageSize.y, this->currImageSize.x << 3, this->currImageSize.x << 3);
			MemFreeA(imgBuff);
		}
		else if (rotType == Media::RotateType::CW_270)
		{
			ImageUtil_Rotate64_CW270(imgBuff, this->imgBuff, this->currImageSize.y, this->currImageSize.x, this->currImageSize.y << 3, this->currImageSize.x << 3);
			MemFreeA(imgBuff);
		}
		UpdateSubSurface();
		DrawToScreen();
	}
}

void UI::GUIPictureBoxDD::RGBParamChanged(NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgbParam)
{
	if (this->currImage && this->csconv)
	{
		Media::RotateType rotType = Media::RotateType::None;
		UInt8 *imgBuff;
		if (this->currImage->exif)
		{
			rotType = this->currImage->exif->GetRotateType();
		}
		if (rotType == Media::RotateType::None)
		{
			imgBuff = this->imgBuff;
		}
		else
		{
			imgBuff = MemAllocA(UInt8, this->currImageSize.CalcArea() * 8);
		}
		if (this->currImage->GetImageType() == Media::Image::ImageType::Static)
		{
			this->csconv->ConvertV2(&((Media::StaticImage*)this->currImage)->data, imgBuff, this->currImage->info.dispSize.x, this->currImage->info.dispSize.y, this->currImage->info.storeSize.x, this->currImage->info.storeSize.y, (OSInt)this->currImage->info.dispSize.x << 3, this->currImage->info.ftype, this->currImage->info.ycOfst);
		}
		else
		{
			UInt8 *imgData = MemAllocA(UInt8, this->currImage->GetDataBpl() * this->currImage->info.dispSize.y);
			this->currImage->GetImageData(imgData, 0, 0, this->currImage->info.storeSize.x, this->currImage->info.dispSize.y, this->currImage->GetDataBpl(), this->currImage->IsUpsideDown(), this->currImage->info.rotateType);
			this->csconv->ConvertV2(&imgData, imgBuff, this->currImage->info.dispSize.x, this->currImage->info.dispSize.y, this->currImage->info.storeSize.x, this->currImage->info.storeSize.y, (OSInt)this->currImage->info.dispSize.x << 3, this->currImage->info.ftype, this->currImage->info.ycOfst);
			MemFreeA(imgData);
		}
		if (this->enableLRGBLimit)
		{
			LRGBLimiter_LimitImageLRGB(imgBuff, this->currImage->info.dispSize.x, this->currImage->info.dispSize.y);
		}
		if (rotType == Media::RotateType::CW_90)
		{
			ImageUtil_Rotate64_CW90(imgBuff, this->imgBuff, this->currImageSize.y, this->currImageSize.x, this->currImageSize.y << 3, this->currImageSize.x << 3);
			MemFreeA(imgBuff);
		}
		else if (rotType == Media::RotateType::CW_180)
		{
			ImageUtil_Rotate64_CW180(imgBuff, this->imgBuff, this->currImageSize.x, this->currImageSize.y, this->currImageSize.x << 3, this->currImageSize.x << 3);
			MemFreeA(imgBuff);
		}
		else if (rotType == Media::RotateType::CW_270)
		{
			ImageUtil_Rotate64_CW270(imgBuff, this->imgBuff, this->currImageSize.y, this->currImageSize.x, this->currImageSize.y << 3, this->currImageSize.x << 3);
			MemFreeA(imgBuff);
		}
		UpdateSubSurface();
		DrawToScreen();
	}
}

void UI::GUIPictureBoxDD::SetAllowEnlarge(Bool allowEnlarge)
{
	if (this->allowEnlarge != allowEnlarge)
	{
		this->allowEnlarge = allowEnlarge;
		UpdateSubSurface();
		DrawToScreen();
	}
}

void UI::GUIPictureBoxDD::OnSurfaceCreated()
{
	this->UpdateSubSurface();
	this->DrawToScreen();
}

void UI::GUIPictureBoxDD::OnMouseWheel(Math::Coord2D<OSInt> pos, Int32 amount)
{
	Math::Coord2DDbl mousePoint;
	Math::RectAreaDbl srcRect;
	Math::RectArea<OSInt> destRect;

	if (this->currImage == 0)
		return;
	if (amount > 0)
	{
		if (zoomScale >= 16)
		{
			return;
		}
		CalDispRect(&srcRect, &destRect);
		mousePoint = srcRect.tl + (pos - destRect.tl).ToDouble() * srcRect.GetSize() / destRect.GetSize().ToDouble();
		zoomScale = zoomScale * 2;
		UpdateZoomRange();

		CalDispRect(&srcRect, &destRect);
		this->zoomCenter -= (pos.ToDouble() - (mousePoint - srcRect.tl) * destRect.GetSize().ToDouble() / srcRect.GetSize() - destRect.tl.ToDouble()) / zoomScale;
		if (this->zoomCenter.x < zoomMinX)
			this->zoomCenter.x = zoomMinX;
		if (this->zoomCenter.x > zoomMaxX)
			this->zoomCenter.x = zoomMaxX;
		if (this->zoomCenter.y < zoomMinY)
			this->zoomCenter.y = zoomMinY;
		if (this->zoomCenter.y > zoomMaxY)
			this->zoomCenter.y = zoomMaxY;

		UpdateSubSurface();
		DrawToScreen();
	}
	else
	{
		if (zoomScale == 0)
		{
			return;
		}
		CalDispRect(&srcRect, &destRect);
		mousePoint = srcRect.tl + (pos - destRect.tl).ToDouble() * srcRect.GetSize() / destRect.GetSize().ToDouble();

		zoomScale = zoomScale * 0.5;
		if (zoomScale < this->zoomMinScale)
		{
			zoomScale = this->zoomMinScale;
		}
		UpdateZoomRange();

		CalDispRect(&srcRect, &destRect);
		this->zoomCenter -= (pos.ToDouble() - (mousePoint - srcRect.tl) * destRect.GetSize().ToDouble() / srcRect.GetSize() - destRect.tl.ToDouble()) / zoomScale;

		if (this->zoomCenter.x < zoomMinX)
			this->zoomCenter.x = zoomMinX;
		if (this->zoomCenter.x > zoomMaxX)
			this->zoomCenter.x = zoomMaxX;
		if (this->zoomCenter.y < zoomMinY)
			this->zoomCenter.y = zoomMinY;
		if (this->zoomCenter.y > zoomMaxY)
			this->zoomCenter.y = zoomMaxY;
		UpdateSubSurface();
		DrawToScreen();
	}
}

void UI::GUIPictureBoxDD::OnMouseMove(Math::Coord2D<OSInt> pos)
{
	this->mouseCurrPos = pos;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = this->mouseMoveHdlrs.GetCount();
	while (i < j)
	{
		if (this->mouseMoveHdlrs.GetItem(i)(this->mouseMoveObjs.GetItem(i), pos, UI::GUIControl::MBTN_LEFT))
		{
			return;
		}
		i++;
	}

	if (this->mouseDowned)
	{
		UpdateSubSurface();
		DrawToScreen();
	}
}

void UI::GUIPictureBoxDD::OnMouseDown(Math::Coord2D<OSInt> pos, MouseButton button)
{
	this->Focus();
	UOSInt i;
	UOSInt j;
	i = 0;
	j = this->mouseDownHdlrs.GetCount();
	while (i < j)
	{
		if (this->mouseDownHdlrs.GetItem(i)(this->mouseDownObjs.GetItem(i), pos, button))
		{
			return;
		}
		i++;
	}
	if (button == MBTN_LEFT)
	{
		this->mouseDowned = true;
		this->mouseDownPos = pos;
	}
}

void UI::GUIPictureBoxDD::OnMouseUp(Math::Coord2D<OSInt> pos, MouseButton button)
{
	UOSInt i;
	UOSInt j;
	i = 0;
	j = this->mouseUpHdlrs.GetCount();
	while (i < j)
	{
		if (this->mouseUpHdlrs.GetItem(i)(this->mouseUpObjs.GetItem(i), pos, button))
		{
			return;
		}
		i++;
	}
	if (button == MBTN_LEFT && this->mouseDowned)
	{
		Int32 action = 0;
		this->mouseDowned = false;
		this->zoomCenter += (this->mouseDownPos - pos).ToDouble() / this->zoomScale;
		if (this->zoomCenter.x < this->zoomMinX)
		{
			this->zoomCenter.x = this->zoomMinX;
			if (this->zoomScale == this->zoomMinScale)
			{
				action = 1;
			}
		}
		else if (this->zoomCenter.x > this->zoomMaxX)
		{
			this->zoomCenter.x = this->zoomMaxX;
			if (this->zoomScale == this->zoomMinScale)
			{
				action = 2;
			}
		}

		if (this->zoomCenter.y < this->zoomMinY)
			this->zoomCenter.y = this->zoomMinY;
		else if (this->zoomCenter.y > this->zoomMaxY)
			this->zoomCenter.y = this->zoomMaxY;

		UpdateSubSurface();
		DrawToScreen();

		if (action == 1)
		{
			this->EventMoveToNext();
		}
		else if (action == 2)
		{
			this->EventMoveToPrev();
		}
	}
}

void UI::GUIPictureBoxDD::OnGZoomBegin(Math::Coord2D<OSInt> pos, UInt64 dist)
{
	this->gzoomDown = true;
	this->gzoomDownPos = pos;
	this->gzoomDownDist = dist;
	this->gzoomCurrPos = pos;
	this->gzoomCurrDist = dist;
}

void UI::GUIPictureBoxDD::OnGZoomStep(Math::Coord2D<OSInt> pos, UInt64 dist)
{
	if (this->gzoomDown)
	{
		this->gzoomCurrPos = pos;
		this->gzoomCurrDist = dist;

		UpdateSubSurface();
		DrawToScreen();
	}
}

void UI::GUIPictureBoxDD::OnGZoomEnd(Math::Coord2D<OSInt> pos, UInt64 dist)
{
	if (this->gzoomDown)
	{
		this->gzoomDown = false;
		this->gzoomCurrPos = pos;
		this->gzoomCurrDist = dist;

		this->zoomScale = this->zoomScale * UInt64_Double(this->gzoomCurrDist) / UInt64_Double(this->gzoomDownDist);
		if (this->zoomScale < this->zoomMinScale)
		{
			this->zoomScale = this->zoomMinScale;
		}
		else if (this->zoomScale > 16)
		{
			this->zoomScale = 16.0;
		}
		this->UpdateZoomRange();
		this->zoomCenter += (this->gzoomDownPos - this->gzoomCurrPos).ToDouble() / this->zoomScale;

		if (this->zoomCenter.x < this->zoomMinX)
			this->zoomCenter.x = this->zoomMinX;
		else if (this->zoomCenter.x > this->zoomMaxX)
			this->zoomCenter.x = this->zoomMaxX;

		if (this->zoomCenter.y < this->zoomMinY)
			this->zoomCenter.y = this->zoomMinY;
		else if (this->zoomCenter.y > this->zoomMaxY)
			this->zoomCenter.y = this->zoomMaxY;

		UpdateSubSurface();
		DrawToScreen();
	}
}

void UI::GUIPictureBoxDD::OnJSButtonDown(OSInt buttonId)
{
	if (buttonId == 4)
	{
		this->zoomScale = this->zoomScale * 2.0;
		if (this->zoomScale > 16.0)
		{
			this->zoomScale = 16.0;
		}
		this->UpdateZoomRange();
		UpdateSubSurface();
		DrawToScreen();
	}
	else if (buttonId == 3)
	{
		this->zoomScale = this->zoomScale * 0.5;
		if (this->zoomScale < this->zoomMinScale)
		{
			this->zoomScale = this->zoomMinScale;
		}
		this->UpdateZoomRange();
		UpdateSubSurface();
		DrawToScreen();
	}
	else if (buttonId == 1)
	{
		this->EventMoveToPrev();
	}
	else if (buttonId == 2)
	{
		this->EventMoveToNext();
	}
	else if (buttonId == 5)
	{
		this->SwitchFullScreen(!this->IsFullScreen(), true);
	}
}

void UI::GUIPictureBoxDD::OnJSButtonUp(OSInt buttonId)
{
}

void UI::GUIPictureBoxDD::OnJSAxis(OSInt axis1, OSInt axis2, OSInt axis3, OSInt axis4)
{
	axis1 >>= 10;
	axis2 >>= 10;
	if (axis1 < 0)
		axis1++;
	if (axis2 < 0)
		axis2++;
	if (axis1 != 0 || axis2 != 0)
	{
		this->zoomCenter.x += OSInt2Double(axis1) / this->zoomScale;
		this->zoomCenter.y += OSInt2Double(axis2) / this->zoomScale;
		if (this->zoomCenter.x < this->zoomMinX)
		{
			this->zoomCenter.x = this->zoomMinX;
		}
		else if (this->zoomCenter.x > this->zoomMaxX)
		{
			this->zoomCenter.x = this->zoomMaxX;
		}

		if (this->zoomCenter.y < this->zoomMinY)
			this->zoomCenter.y = this->zoomMinY;
		else if (this->zoomCenter.y > this->zoomMaxY)
			this->zoomCenter.y = this->zoomMaxY;

		UpdateSubSurface();
		DrawToScreen();
	}
}


void UI::GUIPictureBoxDD::EventMoveToNext()
{
	UOSInt i;
	UOSInt j;
	i = 0;
	j = this->moveToNextHdlrs.GetCount();
	while (i < j)
	{
		this->moveToNextHdlrs.GetItem(i)(this->moveToNextObjs.GetItem(i));
		i++;
	}
}

void UI::GUIPictureBoxDD::EventMoveToPrev()
{
	UOSInt i;
	UOSInt j;
	i = 0;
	j = this->moveToPrevHdlrs.GetCount();
	while (i < j)
	{
		this->moveToPrevHdlrs.GetItem(i)(this->moveToPrevObjs.GetItem(i));
		i++;
	}
}

Bool UI::GUIPictureBoxDD::GetImageViewSize(Math::Size2D<UOSInt> *viewSize, Math::Size2D<UOSInt> imageSize)
{
	Double par = 1;
	if (this->bkBuffSize.x >= imageSize.x && this->bkBuffSize.y >= imageSize.y)
	{
		if (par > 1)
		{
			viewSize->x = imageSize.x;
			viewSize->y = (UOSInt)Double2OSInt(UOSInt2Double(imageSize.y) / par);
		}
		else if (par < 1)
		{
			viewSize->x = (UOSInt)Double2OSInt(UOSInt2Double(imageSize.x) * par);
			viewSize->y = imageSize.y;
		}
		else
		{
			*viewSize = imageSize;
		}
	}
	else
	{
		UOSInt srcW = imageSize.x;
		UOSInt srcH = imageSize.y;
		if (UOSInt2Double(srcW) * par * UOSInt2Double(this->bkBuffSize.y) > UOSInt2Double(this->bkBuffSize.x) * UOSInt2Double(srcH))
		{
			viewSize->x = this->bkBuffSize.x;
			viewSize->y = (UOSInt)Double2OSInt(UOSInt2Double(this->bkBuffSize.x) / par * UOSInt2Double(srcH) / UOSInt2Double(srcW));
		}
		else
		{
			viewSize->x = (UOSInt)Double2OSInt(UOSInt2Double(this->bkBuffSize.y) * par * UOSInt2Double(srcW) / UOSInt2Double(srcH));
			viewSize->y = this->bkBuffSize.y;
		}
	}
	return true;
}

NotNullPtr<Media::StaticImage> UI::GUIPictureBoxDD::CreatePreviewImage(NotNullPtr<const Media::StaticImage> image)
{
	NotNullPtr<Media::StaticImage> outImage;
	Math::Size2D<UOSInt> prevSize;
	this->GetImageViewSize(&prevSize, image->info.dispSize);

	UInt8 *prevImgData = MemAllocA(UInt8, image->info.dispSize.CalcArea() * 8);
	Media::ColorProfile color(NotNullPtr<const Media::ColorProfile>(image->info.color));
	color.GetRTranParam()->Set(Media::CS::TRANT_LINEAR, 1.0);
	color.GetGTranParam()->Set(Media::CS::TRANT_LINEAR, 1.0);
	color.GetBTranParam()->Set(Media::CS::TRANT_LINEAR, 1.0);
	Media::CS::CSConverter *csConv = Media::CS::CSConverter::NewConverter(image->info.fourcc, image->info.storeBPP, image->info.pf, image->info.color, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, color, Media::ColorProfile::YUVT_UNKNOWN, this->colorSess);
	Media::Resizer::LanczosResizerLR_C32 *resizer;
	Media::PixelFormat pf = Media::PF_B8G8R8A8;
	NEW_CLASS(resizer, Media::Resizer::LanczosResizerLR_C32(4, 4, image->info.color, this->colorSess, Media::AT_NO_ALPHA, Media::CS::TransferFunc::GetRefLuminance(image->info.color.rtransfer), pf));
	csConv->ConvertV2(&image->data, prevImgData, image->info.dispSize.x, image->info.dispSize.y, image->info.storeSize.x, image->info.storeSize.y, (OSInt)image->info.dispSize.x * 8, Media::FT_NON_INTERLACE, Media::YCOFST_C_TOP_LEFT);

	NEW_CLASSNN(outImage, Media::StaticImage(image->info.dispSize, 0, 32, pf, 0, image->info.color, Media::ColorProfile::YUVT_UNKNOWN, image->info.atype, image->info.ycOfst));
	resizer->Resize(prevImgData, (OSInt)image->info.dispSize.x * 8, UOSInt2Double(image->info.dispSize.x), UOSInt2Double(image->info.dispSize.y), 0, 0, outImage->data, (OSInt)outImage->GetDataBpl(), outImage->info.dispSize.x, outImage->info.dispSize.y);

	DEL_CLASS(resizer);
	DEL_CLASS(csConv);
	MemFreeA(prevImgData);
	return outImage;
}

void UI::GUIPictureBoxDD::HandleMouseDown(MouseEventHandler hdlr, void *userObj)
{
	this->mouseDownHdlrs.Add(hdlr);
	this->mouseDownObjs.Add(userObj);
}

void UI::GUIPictureBoxDD::HandleMouseMove(MouseEventHandler hdlr, void *userObj)
{
	this->mouseMoveHdlrs.Add(hdlr);
	this->mouseMoveObjs.Add(userObj);
}

void UI::GUIPictureBoxDD::HandleMouseUp(MouseEventHandler hdlr, void *userObj)
{
	this->mouseUpHdlrs.Add(hdlr);
	this->mouseUpObjs.Add(userObj);
}

void UI::GUIPictureBoxDD::HandleDraw(DrawHandler32 hdlr, void *userObj)
{
	this->drawHdlrs.Add(hdlr);
	this->drawObjs.Add(userObj);
}

void UI::GUIPictureBoxDD::HandleMoveToNext(UI::UIEvent hdlr, void *userObj)
{
	this->moveToNextHdlrs.Add(hdlr);
	this->moveToNextObjs.Add(userObj);
}

void UI::GUIPictureBoxDD::HandleMoveToPrev(UI::UIEvent hdlr, void *userObj)
{
	this->moveToPrevHdlrs.Add(hdlr);
	this->moveToPrevObjs.Add(userObj);
}

Math::Coord2DDbl UI::GUIPictureBoxDD::Scn2ImagePos(Math::Coord2D<OSInt> scnPos)
{
	if (this->currImage == 0)
		return Math::Coord2DDbl(0, 0);
	Math::RectAreaDbl srcRect;
	Math::RectArea<OSInt> destRect;
	CalDispRect(&srcRect, &destRect);
/*	if (x < destRect[0])
	{
		x = destRect[0];
	}
	else if (x > destRect[0] + destRect[2])
	{
		x = destRect[0] + destRect[2];
	}
	if (y < destRect[1])
	{
		y = destRect[1];
	}
	else if (y > destRect[1] + destRect[3])
	{
		y = destRect[1] + destRect[3];
	}*/

	return srcRect.tl + (scnPos - destRect.tl).ToDouble() * srcRect.GetSize() / destRect.GetSize().ToDouble();
}

Math::Coord2DDbl UI::GUIPictureBoxDD::Image2ScnPos(Math::Coord2DDbl imgPos)
{
	Math::RectAreaDbl srcRect;
	Math::RectArea<OSInt> destRect;
	CalDispRect(&srcRect, &destRect);

	if (this->mouseDowned)
	{
		Math::Coord2DDbl currCenter = this->zoomCenter + (this->mouseDownPos - this->mouseCurrPos).ToDouble() / this->zoomScale;
		if (this->zoomScale != this->zoomMinScale)
		{
			if (currCenter.x < this->zoomMinX)
				currCenter.x = this->zoomMinX;
			else if (currCenter.x > this->zoomMaxX)
				currCenter.x = this->zoomMaxX;
		}

		if (currCenter.y < this->zoomMinY)
			currCenter.y = this->zoomMinY;
		else if (currCenter.y > this->zoomMaxY)
			currCenter.y = this->zoomMaxY;
		
		Math::Coord2DDbl sub = (currCenter - this->zoomCenter) * this->zoomScale;
		return destRect.tl.ToDouble() + (imgPos - srcRect.tl) * destRect.GetSize().ToDouble() / srcRect.GetSize() - sub;
	}
	else
	{
		return destRect.tl.ToDouble() + (imgPos - srcRect.tl) * destRect.GetSize().ToDouble() / srcRect.GetSize();
	}
}

void UI::GUIPictureBoxDD::ZoomToFit()
{
	this->zoomCenter = this->currImageSize.ToDouble() * 0.5;
	Double outZoomScale;
	Double outW;
//	Double outH;
	Math::Size2DDbl srcSize = this->currImageSize.ToDouble();
	if (srcSize.x * this->currImage->info.par2 * UOSInt2Double(this->bkBuffSize.y) > UOSInt2Double(this->bkBuffSize.x) * srcSize.y)
	{
		outW = UOSInt2Double(this->bkBuffSize.x);
//		outH = this->surfaceW / this->currImage->info.par2 * srcH / srcW;
	}
	else
	{
		outW = UOSInt2Double(this->bkBuffSize.y) * this->currImage->info.par2 * srcSize.x / srcSize.y;
//		outH = OSInt2Double(this->surfaceH);
	}
	outZoomScale = outW / srcSize.x;
	this->zoomScale = outZoomScale;
	this->UpdateSubSurface();
	DrawToScreen();
}

void UI::GUIPictureBoxDD::UpdateBufferImage()
{
	this->UpdateSubSurface();
	DrawToScreen();
}
