#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/ImageCopyC.h"
#include "Media/ImageUtil.h"
#include "Media/LRGBLimiter.h"
#include "Media/CS/TransferFunc.h"
#include "Media/Resizer/LanczosResizerLR_C32.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "UI/GUIClientControl.h"
#include "UI/GUIPictureBoxDD.h"
#include "UI/MessageDialog.h"

void UI::GUIPictureBoxDD::UpdateSubSurface()
{
	if (this->imgBuff && this->currImage && this->surfaceBuff)
	{
		UOSInt bpl;
		if (this->drawHdlrs->GetCount() > 0)
		{
			if (this->bgBuff == 0 || this->surfaceW != this->bgBuffW || this->surfaceH != this->bgBuffH)
			{
				if (this->bgBuff)
				{
					MemFreeA(this->bgBuff);
				}
				this->bgBuffW = this->surfaceW;
				this->bgBuffH = this->surfaceH;
				this->bgBuff = MemAllocA(UInt8, this->bgBuffW * this->bgBuffH * 4);
			}

			Double srcRect[4];
			OSInt destRect[4];
			ImageUtil_ImageColorFill32(this->bgBuff, this->bgBuffW, this->bgBuffH, this->bgBuffW << 2, 0);

			if (this->mouseDowned)
			{
				Double oldCenterX = this->zoomCenterX;
				Double oldCenterY = this->zoomCenterY;
				this->zoomCenterX += Math::OSInt2Double(this->mouseDownX - this->mouseCurrX) / this->zoomScale;
				this->zoomCenterY += Math::OSInt2Double(this->mouseDownY - this->mouseCurrY) / this->zoomScale;
				if (this->zoomScale != this->zoomMinScale)
				{
					if (this->zoomCenterX < this->zoomMinX)
						this->zoomCenterX = this->zoomMinX;
					else if (this->zoomCenterX > this->zoomMaxX)
						this->zoomCenterX = this->zoomMaxX;
				}

				if (this->zoomCenterY < this->zoomMinY)
					this->zoomCenterY = this->zoomMinY;
				else if (this->zoomCenterY > this->zoomMaxY)
					this->zoomCenterY = this->zoomMaxY;
				CalDispRect(srcRect, destRect);
				this->zoomCenterX = oldCenterX;
				this->zoomCenterY = oldCenterY;
			}
			else if (this->gzoomDown)
			{
				Double oldCenterX = this->zoomCenterX;
				Double oldCenterY = this->zoomCenterY;
				Double oldScale = this->zoomScale;
				Double oldMinX = this->zoomMinX;
				Double oldMaxX = this->zoomMaxX;
				Double oldMinY = this->zoomMinY;
				Double oldMaxY = this->zoomMaxY;
				this->zoomScale = this->zoomScale * Math::UInt64_Double(this->gzoomCurrDist) / Math::UInt64_Double(this->gzoomDownDist);
				if (this->zoomScale < this->zoomMinScale)
				{
					this->zoomScale = this->zoomMinScale;
				}
				else if (this->zoomScale > 16)
				{
					this->zoomScale = 16.0;
				}
				this->UpdateZoomRange();

				this->zoomCenterX += Math::OSInt2Double(this->gzoomDownX - this->gzoomCurrX) / this->zoomScale;
				this->zoomCenterY += Math::OSInt2Double(this->gzoomDownY - this->gzoomCurrY) / this->zoomScale;
				if (this->zoomCenterX < this->zoomMinX)
					this->zoomCenterX = this->zoomMinX;
				else if (this->zoomCenterX > this->zoomMaxX)
					this->zoomCenterX = this->zoomMaxX;

				if (this->zoomCenterY < this->zoomMinY)
					this->zoomCenterY = this->zoomMinY;
				else if (this->zoomCenterY > this->zoomMaxY)
					this->zoomCenterY = this->zoomMaxY;
				CalDispRect(srcRect, destRect);
				this->zoomCenterX = oldCenterX;
				this->zoomCenterY = oldCenterY;
				this->zoomScale = oldScale;
				this->zoomMinX = oldMinX;
				this->zoomMaxX = oldMaxX;
				this->zoomMinY = oldMinY;
				this->zoomMaxY = oldMaxY;
			}
			else
			{
				CalDispRect(srcRect, destRect);
			}
			UInt8 *dptr = this->bgBuff;
			Int32 iLeft = (Int32)srcRect[0];
			Int32 iTop = (Int32)srcRect[1];
			dptr = dptr + destRect[1] * (OSInt)(this->bgBuffW << 2) + destRect[0] * 4;
			resizer->Resize(this->imgBuff + iLeft * 8 + iTop * (OSInt)this->currImageW * 8, (OSInt)this->currImageW << 3, srcRect[2], srcRect[3], srcRect[0] - iLeft, srcRect[1] - iTop, dptr, (OSInt)this->bgBuffW << 2, (UOSInt)destRect[2], (UOSInt)destRect[3]);

			this->DrawFromBG();
		}
		else
		{
			UInt8 *dptr = this->LockSurfaceBegin(this->surfaceW, this->surfaceH, &bpl);
			if (dptr)
			{
				Double srcRect[4];
				OSInt destRect[4];
				ImageUtil_ImageColorFill32(dptr, this->surfaceW, this->surfaceH, bpl, 0);

				if (this->mouseDowned)
				{
					Double oldCenterX = this->zoomCenterX;
					Double oldCenterY = this->zoomCenterY;
					this->zoomCenterX += Math::OSInt2Double(this->mouseDownX - this->mouseCurrX) / this->zoomScale;
					this->zoomCenterY += Math::OSInt2Double(this->mouseDownY - this->mouseCurrY) / this->zoomScale;
					if (this->zoomScale != this->zoomMinScale)
					{
						if (this->zoomCenterX < this->zoomMinX)
							this->zoomCenterX = this->zoomMinX;
						else if (this->zoomCenterX > this->zoomMaxX)
							this->zoomCenterX = this->zoomMaxX;
					}

					if (this->zoomCenterY < this->zoomMinY)
						this->zoomCenterY = this->zoomMinY;
					else if (this->zoomCenterY > this->zoomMaxY)
						this->zoomCenterY = this->zoomMaxY;
					CalDispRect(srcRect, destRect);
					this->zoomCenterX = oldCenterX;
					this->zoomCenterY = oldCenterY;
				}
				else if (this->gzoomDown)
				{
					Double oldCenterX = this->zoomCenterX;
					Double oldCenterY = this->zoomCenterY;
					Double oldScale = this->zoomScale;
					Double oldMinX = this->zoomMinX;
					Double oldMaxX = this->zoomMaxX;
					Double oldMinY = this->zoomMinY;
					Double oldMaxY = this->zoomMaxY;
					this->zoomScale = this->zoomScale * Math::UInt64_Double(this->gzoomCurrDist) / Math::UInt64_Double(this->gzoomDownDist);
					if (this->zoomScale < this->zoomMinScale)
					{
						this->zoomScale = this->zoomMinScale;
					}
					else if (this->zoomScale > 16)
					{
						this->zoomScale = 16.0;
					}
					this->UpdateZoomRange();

					this->zoomCenterX += Math::OSInt2Double(this->gzoomDownX - this->gzoomCurrX) / this->zoomScale;
					this->zoomCenterY += Math::OSInt2Double(this->gzoomDownY - this->gzoomCurrY) / this->zoomScale;
					if (this->zoomCenterX < this->zoomMinX)
						this->zoomCenterX = this->zoomMinX;
					else if (this->zoomCenterX > this->zoomMaxX)
						this->zoomCenterX = this->zoomMaxX;

					if (this->zoomCenterY < this->zoomMinY)
						this->zoomCenterY = this->zoomMinY;
					else if (this->zoomCenterY > this->zoomMaxY)
						this->zoomCenterY = this->zoomMaxY;
					CalDispRect(srcRect, destRect);
					this->zoomCenterX = oldCenterX;
					this->zoomCenterY = oldCenterY;
					this->zoomScale = oldScale;
					this->zoomMinX = oldMinX;
					this->zoomMaxX = oldMaxX;
					this->zoomMinY = oldMinY;
					this->zoomMaxY = oldMaxY;
				}
				else
				{
					CalDispRect(srcRect, destRect);
				}
				Int32 iLeft = (Int32)srcRect[0];
				Int32 iTop = (Int32)srcRect[1];
				dptr = dptr + destRect[1] * (OSInt)bpl + destRect[0] * 4;
				if (srcRect[2] > 0 && srcRect[3] > 0)
				{
					resizer->Resize(this->imgBuff + iLeft * 8 + iTop * (OSInt)this->currImageW * 8, (OSInt)this->currImageW << 3, srcRect[2], srcRect[3], srcRect[0] - iLeft, srcRect[1] - iTop, dptr, (OSInt)bpl, (UOSInt)destRect[2], (UOSInt)destRect[3]);
				}

				this->LockSurfaceEnd();
			}
		}
	}
}

void UI::GUIPictureBoxDD::CalDispRect(Double *srcRect, OSInt *destRect)
{
	if (this->currImage == 0)
		return;

	srcRect[0] = this->zoomCenterX - Math::UOSInt2Double(this->surfaceW) * 0.5 / this->zoomScale;
	srcRect[1] = this->zoomCenterY - Math::UOSInt2Double(this->surfaceH) * 0.5 / this->zoomScale;
	srcRect[2] = Math::UOSInt2Double(this->surfaceW) / this->zoomScale;
	srcRect[3] = Math::UOSInt2Double(this->surfaceH) / this->zoomScale;
	destRect[0] = 0;
	destRect[1] = 0;
	destRect[2] = (OSInt)this->surfaceW;
	destRect[3] = (OSInt)this->surfaceH;

	if (srcRect[0] < 0)
	{
		destRect[0] += Math::Double2OSInt(-srcRect[0] * this->zoomScale);
		destRect[2] += Math::Double2OSInt(srcRect[0] * this->zoomScale);
		srcRect[2] += srcRect[0];
		srcRect[0] = 0;
	}
	if (srcRect[0] + srcRect[2] > Math::UOSInt2Double(this->currImageW))
	{
		srcRect[2] = Math::UOSInt2Double(this->currImageW) - srcRect[0];
		destRect[2] = Math::Double2OSInt(srcRect[2] * this->zoomScale);
	}
	if (srcRect[1] < 0)
	{
		destRect[1] = Math::Double2OSInt(-srcRect[1] * this->zoomScale);
		destRect[3] += Math::Double2OSInt(srcRect[1] * this->zoomScale);
		srcRect[3] += srcRect[1];
		srcRect[1] = 0;
	}
	if (srcRect[1] + srcRect[3] > Math::UOSInt2Double(this->currImageH))
	{
		srcRect[3] = Math::UOSInt2Double(this->currImageH) - srcRect[1];
		destRect[3] = Math::Double2OSInt(srcRect[3] * this->zoomScale);
	}
}

void UI::GUIPictureBoxDD::UpdateZoomRange()
{
	if (Math::UOSInt2Double(this->surfaceW) > Math::UOSInt2Double(this->currImageW) * zoomScale)
	{
		this->zoomMaxX = Math::UOSInt2Double(this->surfaceW) * 0.5 / zoomScale;
		this->zoomMinX = Math::UOSInt2Double(this->currImageW) - Math::UOSInt2Double(this->surfaceW) * 0.5 / zoomScale;
	}
	else
	{
		this->zoomMinX = (Math::UOSInt2Double(this->surfaceW) * 0.5) / zoomScale;
		this->zoomMaxX = Math::UOSInt2Double(this->currImageW) - Math::UOSInt2Double(this->surfaceW) * 0.5 / zoomScale;
	}
	if (Math::UOSInt2Double(this->surfaceH) > Math::UOSInt2Double(this->currImageH) * zoomScale)
	{
		this->zoomMaxY = Math::UOSInt2Double(this->surfaceH) * 0.5 / zoomScale;
		this->zoomMinY = Math::UOSInt2Double(this->currImageH) - Math::UOSInt2Double(this->surfaceH) * 0.5 / zoomScale;
	}
	else
	{
		this->zoomMinY = (Math::UOSInt2Double(this->surfaceH) * 0.5) / zoomScale;
		this->zoomMaxY = Math::UOSInt2Double(this->currImageH) - Math::UOSInt2Double(this->surfaceH) * 0.5 / zoomScale;
	}
}

void UI::GUIPictureBoxDD::UpdateMinScale()
{
	Double outZoomScale;
	Double outW;
//	Double outH;
	Double srcW = Math::UOSInt2Double(this->currImageW);
	Double srcH = Math::UOSInt2Double(this->currImageH);
	if (srcW * this->currImage->info->par2 * Math::UOSInt2Double(this->surfaceH) > Math::UOSInt2Double(this->surfaceW) * srcH)
	{
		outW = Math::UOSInt2Double(this->surfaceW);
//		outH = this->surfaceW / this->currImage->info->par2 * srcH / srcW;
	}
	else
	{
		outW = Math::UOSInt2Double(this->surfaceH) * this->currImage->info->par2 * srcW / srcH;
//		outH = Math::OSInt2Double(this->surfaceH);
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
		refLuminance = Media::CS::TransferFunc::GetRefLuminance(this->currImage->info->color->rtransfer);
	}
	if (this->curr10Bit)
	{
		NEW_CLASS(this->resizer, Media::Resizer::LanczosResizerLR_C32(4, 3, &destColor, this->colorSess, Media::AT_NO_ALPHA, refLuminance, Media::PF_LE_A2B10G10R10));
	}
	else
	{
		NEW_CLASS(this->resizer, Media::Resizer::LanczosResizerLR_C32(4, 3, &destColor, this->colorSess, Media::AT_NO_ALPHA, refLuminance, this->GetPixelFormat()));
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
	UInt8 *dptr = this->LockSurfaceBegin(this->bgBuffW, this->bgBuffH, &bpl);
	if (dptr)
	{
		UOSInt i;
		UOSInt j;
		ImageCopy_ImgCopy(this->bgBuff, dptr, this->bgBuffW << 2, this->bgBuffH, (OSInt)this->bgBuffW << 2, (OSInt)bpl);
		i = 0;
		j = this->drawHdlrs->GetCount();
		while (i < j)
		{
			this->drawHdlrs->GetItem(i)(this->drawObjs->GetItem(i), dptr, this->bgBuffW, this->bgBuffH, bpl);
			i++;
		}
		this->LockSurfaceEnd();
	}
}

void UI::GUIPictureBoxDD::OnPaint()
{
	if (this->currScnMode != SM_FS && this->currScnMode != SM_VFS)
	{
		if (this->bgBuff && this->drawHdlrs->GetCount() > 0)
		{
			DrawFromBG();
		}

		Sync::MutexUsage mutUsage(this->surfaceMut);
		DrawToScreen();
		mutUsage.EndUse();
	}
	else
	{
//		PAINTSTRUCT ps;
//		BeginPaint((HWND)this->hwnd, &ps);
//		EndPaint((HWND)this->hwnd, &ps);
	}
}

UI::GUIPictureBoxDD::GUIPictureBoxDD(UI::GUICore *ui, UI::GUIClientControl *parent, Media::ColorManagerSess *colorSess, Bool allowEnlarge, Bool directMode) : UI::GUIDDrawControl(ui, parent, directMode, colorSess)
{
	this->colorSess = colorSess;
	this->colorSess->AddHandler(this);
	NEW_CLASS(this->mouseDownHdlrs, Data::ArrayList<MouseEventHandler>());
	NEW_CLASS(this->mouseDownObjs, Data::ArrayList<void *>());
	NEW_CLASS(this->mouseUpHdlrs, Data::ArrayList<MouseEventHandler>());
	NEW_CLASS(this->mouseUpObjs, Data::ArrayList<void *>());
	NEW_CLASS(this->mouseMoveHdlrs, Data::ArrayList<MouseEventHandler>());
	NEW_CLASS(this->mouseMoveObjs, Data::ArrayList<void *>());
	NEW_CLASS(this->drawHdlrs, Data::ArrayList<DrawHandler32>());
	NEW_CLASS(this->drawObjs, Data::ArrayList<void *>());
	NEW_CLASS(this->moveToNextHdlrs, Data::ArrayList<UI::UIEvent>());
	NEW_CLASS(this->moveToNextObjs, Data::ArrayList<void *>());
	NEW_CLASS(this->moveToPrevHdlrs, Data::ArrayList<UI::UIEvent>());
	NEW_CLASS(this->moveToPrevObjs, Data::ArrayList<void *>());
	this->enableLRGBLimit = false;

	this->bgBuff = 0;
	this->bgBuffW = 0;
	this->bgBuffH = 0;
	this->allowEnlarge = allowEnlarge;
	this->currImage = 0;
	this->currImageW = 0;
	this->currImageH = 0;
	this->csconv = 0;
	this->imgBuff = 0;
	this->zoomScale = 1.0;
	this->mouseDowned = false;
	this->curr10Bit = false;
	this->resizer = 0;
	this->zoomCenterX = 0;
	this->zoomCenterY = 0;
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
	DEL_CLASS(this->mouseDownHdlrs);
	DEL_CLASS(this->mouseDownObjs);
	DEL_CLASS(this->mouseUpHdlrs);
	DEL_CLASS(this->mouseUpObjs);
	DEL_CLASS(this->mouseMoveHdlrs);
	DEL_CLASS(this->mouseMoveObjs);
	DEL_CLASS(this->drawHdlrs);
	DEL_CLASS(this->drawObjs);
	DEL_CLASS(this->moveToNextHdlrs);
	DEL_CLASS(this->moveToNextObjs);
	DEL_CLASS(this->moveToPrevHdlrs);
	DEL_CLASS(this->moveToPrevObjs);
	if (this->bgBuff)
	{
		MemFreeA(this->bgBuff);
		this->bgBuff = 0;
	}
	this->colorSess->RemoveHandler(this);
}

const UTF8Char *UI::GUIPictureBoxDD::GetObjectClass()
{
	return (const UTF8Char*)"PictureBoxDD";
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
	UOSInt oriW = this->currImageW;
	UOSInt oriH = this->currImageH;
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
		Media::EXIFData::RotateType rotType = Media::EXIFData::RT_NONE;
		this->currImageW = this->currImage->info->dispWidth;
		this->currImageH = this->currImage->info->dispHeight;
		if (this->currImage->exif)
		{
			rotType = this->currImage->exif->GetRotateType();
			if (rotType == Media::EXIFData::RT_CW90)
			{
				this->currImageH = this->currImage->info->dispWidth;
				this->currImageW = this->currImage->info->dispHeight;
			}
			else if (rotType == Media::EXIFData::RT_CW180)
			{
			}
			else if (rotType == Media::EXIFData::RT_CW270)
			{
				this->currImageH = this->currImage->info->dispWidth;
				this->currImageW = this->currImage->info->dispHeight;
			}
		}
		if (!sameImg || oriW != this->currImageW || oriH != this->currImageH)
		{
			this->zoomCenterX = Math::UOSInt2Double(this->currImageW) * 0.5;
			this->zoomCenterY = Math::UOSInt2Double(this->currImageH) * 0.5;
			this->UpdateMinScale();
			this->zoomScale = this->zoomMinScale;
			this->UpdateZoomRange();
		}
		Media::ColorProfile color(Media::ColorProfile::CPT_PDISPLAY);
		this->csconv = Media::CS::CSConverter::NewConverter(this->currImage->info->fourcc, this->currImage->info->storeBPP, this->currImage->info->pf, this->currImage->info->color, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, &color, this->currImage->info->yuvType, this->colorSess);
		if (this->csconv)
		{
			if (this->currImage->pal)
			{
				this->csconv->SetPalette(this->currImage->pal);
			}
			this->imgBuff = MemAllocA(UInt8, this->currImageW * this->currImageH * 8);
			if (this->currImage->GetImageType() == Media::Image::IT_STATIC)
			{
				this->csconv->ConvertV2(&((Media::StaticImage*)this->currImage)->data, this->imgBuff, this->currImage->info->dispWidth, this->currImage->info->dispHeight, this->currImage->info->storeWidth, this->currImage->info->storeHeight, (OSInt)this->currImage->info->dispWidth << 3, this->currImage->info->ftype, this->currImage->info->ycOfst);
			}
			else
			{
				UInt8 *imgData = MemAllocA(UInt8, this->currImage->GetDataBpl() * this->currImage->info->storeHeight);
				this->currImage->GetImageData(imgData, 0, 0, this->currImage->info->storeWidth, this->currImage->info->dispHeight, this->currImage->GetDataBpl(), this->currImage->IsUpsideDown());
				this->csconv->ConvertV2(&imgData, this->imgBuff, this->currImage->info->dispWidth, this->currImage->info->dispHeight, this->currImage->info->storeWidth, this->currImage->info->storeHeight, (OSInt)this->currImage->info->dispWidth << 3, this->currImage->info->ftype, this->currImage->info->ycOfst);
				MemFreeA(imgData);
			}
			if (this->enableLRGBLimit)
			{
				LRGBLimiter_LimitImageLRGB(this->imgBuff, this->currImage->info->dispWidth, this->currImage->info->dispHeight);
			}

			if (rotType == Media::EXIFData::RT_CW90)
			{
				UInt8 *tmpBuff = MemAllocA(UInt8, this->currImageW * this->currImageH * 8);
				ImageUtil_Rotate64_CW90(this->imgBuff, tmpBuff, this->currImageH, this->currImageW, this->currImageH << 3, this->currImageW << 3);
				MemFreeA(this->imgBuff);
				this->imgBuff = tmpBuff;
			}
			else if (rotType == Media::EXIFData::RT_CW180)
			{
				UInt8 *tmpBuff = MemAllocA(UInt8, this->currImageW * this->currImageH * 8);
				ImageUtil_Rotate64_CW180(this->imgBuff, tmpBuff, this->currImageW, this->currImageH, this->currImageW << 3, this->currImageW << 3);
				MemFreeA(this->imgBuff);
				this->imgBuff = tmpBuff;
			}
			else if (rotType == Media::EXIFData::RT_CW270)
			{
				UInt8 *tmpBuff = MemAllocA(UInt8, this->currImageW * this->currImageH * 8);
				ImageUtil_Rotate64_CW270(this->imgBuff, tmpBuff, this->currImageH, this->currImageW, this->currImageH << 3, this->currImageW << 3);
				MemFreeA(this->imgBuff);
				this->imgBuff = tmpBuff;
			}
			this->UpdateSubSurface();
			DrawToScreen();
		}
	}
	else
	{
		this->currImageW = 0;
		this->currImageH = 0;
	}
}

void UI::GUIPictureBoxDD::YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuvParam)
{
	if (this->currImage && this->csconv)
	{
		Media::EXIFData::RotateType rotType = Media::EXIFData::RT_NONE;
		UInt8 *imgBuff;
		if (this->currImage->exif)
		{
			rotType = this->currImage->exif->GetRotateType();
		}
		if (rotType == Media::EXIFData::RT_NONE)
		{
			imgBuff = this->imgBuff;
		}
		else
		{
			imgBuff = MemAllocA(UInt8, this->currImageW * this->currImageH * 8);
		}
		if (this->currImage->GetImageType() == Media::Image::IT_STATIC)
		{
			this->csconv->ConvertV2(&((Media::StaticImage*)this->currImage)->data, imgBuff, this->currImage->info->dispWidth, this->currImage->info->dispHeight, this->currImage->info->storeWidth, this->currImage->info->storeHeight, (OSInt)this->currImage->info->dispWidth << 3, this->currImage->info->ftype, this->currImage->info->ycOfst);
		}
		else
		{
			UInt8 *imgData = MemAllocA(UInt8, this->currImage->GetDataBpl() * this->currImage->info->storeHeight);
			this->currImage->GetImageData(imgData, 0, 0, this->currImage->info->storeWidth, this->currImage->info->dispHeight, this->currImage->GetDataBpl(), this->currImage->IsUpsideDown());
			this->csconv->ConvertV2(&imgData, imgBuff, this->currImage->info->dispWidth, this->currImage->info->dispHeight, this->currImage->info->storeWidth, this->currImage->info->storeHeight, (OSInt)this->currImage->info->dispWidth << 3, this->currImage->info->ftype, this->currImage->info->ycOfst);
			MemFreeA(imgData);
		}
		if (this->enableLRGBLimit)
		{
			LRGBLimiter_LimitImageLRGB(imgBuff, this->currImage->info->dispWidth, this->currImage->info->dispHeight);
		}
		if (rotType == Media::EXIFData::RT_CW90)
		{
			ImageUtil_Rotate64_CW90(imgBuff, this->imgBuff, this->currImageH, this->currImageW, this->currImageH << 3, this->currImageW << 3);
			MemFreeA(imgBuff);
		}
		else if (rotType == Media::EXIFData::RT_CW180)
		{
			ImageUtil_Rotate64_CW180(imgBuff, this->imgBuff, this->currImageW, this->currImageH, this->currImageW << 3, this->currImageW << 3);
			MemFreeA(imgBuff);
		}
		else if (rotType == Media::EXIFData::RT_CW270)
		{
			ImageUtil_Rotate64_CW270(imgBuff, this->imgBuff, this->currImageH, this->currImageW, this->currImageH << 3, this->currImageW << 3);
			MemFreeA(imgBuff);
		}
		UpdateSubSurface();
		DrawToScreen();
	}
}

void UI::GUIPictureBoxDD::RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgbParam)
{
	if (this->currImage && this->csconv)
	{
		Media::EXIFData::RotateType rotType = Media::EXIFData::RT_NONE;
		UInt8 *imgBuff;
		if (this->currImage->exif)
		{
			rotType = this->currImage->exif->GetRotateType();
		}
		if (rotType == Media::EXIFData::RT_NONE)
		{
			imgBuff = this->imgBuff;
		}
		else
		{
			imgBuff = MemAllocA(UInt8, this->currImageW * this->currImageH * 8);
		}
		if (this->currImage->GetImageType() == Media::Image::IT_STATIC)
		{
			this->csconv->ConvertV2(&((Media::StaticImage*)this->currImage)->data, imgBuff, this->currImage->info->dispWidth, this->currImage->info->dispHeight, this->currImage->info->storeWidth, this->currImage->info->storeHeight, (OSInt)this->currImage->info->dispWidth << 3, this->currImage->info->ftype, this->currImage->info->ycOfst);
		}
		else
		{
			UInt8 *imgData = MemAllocA(UInt8, this->currImage->GetDataBpl() * this->currImage->info->dispHeight);
			this->currImage->GetImageData(imgData, 0, 0, this->currImage->info->storeWidth, this->currImage->info->dispHeight, this->currImage->GetDataBpl(), this->currImage->IsUpsideDown());
			this->csconv->ConvertV2(&imgData, imgBuff, this->currImage->info->dispWidth, this->currImage->info->dispHeight, this->currImage->info->storeWidth, this->currImage->info->storeHeight, (OSInt)this->currImage->info->dispWidth << 3, this->currImage->info->ftype, this->currImage->info->ycOfst);
			MemFreeA(imgData);
		}
		if (this->enableLRGBLimit)
		{
			LRGBLimiter_LimitImageLRGB(imgBuff, this->currImage->info->dispWidth, this->currImage->info->dispHeight);
		}
		if (rotType == Media::EXIFData::RT_CW90)
		{
			ImageUtil_Rotate64_CW90(imgBuff, this->imgBuff, this->currImageH, this->currImageW, this->currImageH << 3, this->currImageW << 3);
			MemFreeA(imgBuff);
		}
		else if (rotType == Media::EXIFData::RT_CW180)
		{
			ImageUtil_Rotate64_CW180(imgBuff, this->imgBuff, this->currImageW, this->currImageH, this->currImageW << 3, this->currImageW << 3);
			MemFreeA(imgBuff);
		}
		else if (rotType == Media::EXIFData::RT_CW270)
		{
			ImageUtil_Rotate64_CW270(imgBuff, this->imgBuff, this->currImageH, this->currImageW, this->currImageH << 3, this->currImageW << 3);
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

void UI::GUIPictureBoxDD::OnMouseWheel(OSInt x, OSInt y, Int32 amount)
{
	Double mousePointX;
	Double mousePointY;
	Double srcRect[4];
	OSInt destRect[4];

	if (this->currImage == 0)
		return;
	if (amount > 0)
	{
		if (zoomScale >= 16)
		{
			return;
		}
		CalDispRect(srcRect, destRect);
		mousePointX = srcRect[0] + Math::OSInt2Double(x - destRect[0]) * srcRect[2] / Math::OSInt2Double(destRect[2]);
		mousePointY = srcRect[1] + Math::OSInt2Double(y - destRect[1]) * srcRect[3] / Math::OSInt2Double(destRect[3]);
		zoomScale = zoomScale * 2;
		UpdateZoomRange();

		CalDispRect(srcRect, destRect);
		this->zoomCenterX -= (Math::OSInt2Double(x) - (mousePointX - srcRect[0]) * Math::OSInt2Double(destRect[2]) / srcRect[2] - Math::OSInt2Double(destRect[0])) / zoomScale;
		this->zoomCenterY -= (Math::OSInt2Double(y) - (mousePointY - srcRect[1]) * Math::OSInt2Double(destRect[3]) / srcRect[3] - Math::OSInt2Double(destRect[1])) / zoomScale;
		if (this->zoomCenterX < zoomMinX)
			this->zoomCenterX = zoomMinX;
		if (this->zoomCenterX > zoomMaxX)
			this->zoomCenterX = zoomMaxX;
		if (this->zoomCenterY < zoomMinY)
			this->zoomCenterY = zoomMinY;
		if (this->zoomCenterY > zoomMaxY)
			this->zoomCenterY = zoomMaxY;

		UpdateSubSurface();
		DrawToScreen();
	}
	else
	{
		if (zoomScale == 0)
		{
			return;
		}
		CalDispRect(srcRect, destRect);
		mousePointX = srcRect[0] + Math::OSInt2Double(x - destRect[0]) * srcRect[2] / Math::OSInt2Double(destRect[2]);
		mousePointY = srcRect[1] + Math::OSInt2Double(y - destRect[1]) * srcRect[3] / Math::OSInt2Double(destRect[3]);

		zoomScale = zoomScale * 0.5;
		if (zoomScale < this->zoomMinScale)
		{
			zoomScale = this->zoomMinScale;
		}
		UpdateZoomRange();

		CalDispRect(srcRect, destRect);
		this->zoomCenterX -= (Math::OSInt2Double(x) - (mousePointX - srcRect[0]) * Math::OSInt2Double(destRect[2]) / srcRect[2] - Math::OSInt2Double(destRect[0])) / zoomScale;
		this->zoomCenterY -= (Math::OSInt2Double(y) - (mousePointY - srcRect[1]) * Math::OSInt2Double(destRect[3]) / srcRect[3] - Math::OSInt2Double(destRect[1])) / zoomScale;

		if (this->zoomCenterX < zoomMinX)
			this->zoomCenterX = zoomMinX;
		if (this->zoomCenterX > zoomMaxX)
			this->zoomCenterX = zoomMaxX;
		if (this->zoomCenterY < zoomMinY)
			this->zoomCenterY = zoomMinY;
		if (this->zoomCenterY > zoomMaxY)
			this->zoomCenterY = zoomMaxY;
		UpdateSubSurface();
		DrawToScreen();
	}
}

void UI::GUIPictureBoxDD::OnMouseMove(OSInt x, OSInt y)
{
	this->mouseCurrX = x;
	this->mouseCurrY = y;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = this->mouseMoveHdlrs->GetCount();
	while (i < j)
	{
		if (this->mouseMoveHdlrs->GetItem(i)(this->mouseMoveObjs->GetItem(i), x, y, UI::GUIControl::MBTN_LEFT))
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

void UI::GUIPictureBoxDD::OnMouseDown(OSInt x, OSInt y, MouseButton button)
{
	this->Focus();
	UOSInt i;
	UOSInt j;
	i = 0;
	j = this->mouseDownHdlrs->GetCount();
	while (i < j)
	{
		if (this->mouseDownHdlrs->GetItem(i)(this->mouseDownObjs->GetItem(i), x, y, button))
		{
			return;
		}
		i++;
	}
	if (button == MBTN_LEFT)
	{
		this->mouseDowned = true;
		this->mouseDownX = x;
		this->mouseDownY = y;
	}
}

void UI::GUIPictureBoxDD::OnMouseUp(OSInt x, OSInt y, MouseButton button)
{
	UOSInt i;
	UOSInt j;
	i = 0;
	j = this->mouseUpHdlrs->GetCount();
	while (i < j)
	{
		if (this->mouseUpHdlrs->GetItem(i)(this->mouseUpObjs->GetItem(i), x, y, button))
		{
			return;
		}
		i++;
	}
	if (button == MBTN_LEFT && this->mouseDowned)
	{
		Int32 action = 0;
		this->mouseDowned = false;
		this->zoomCenterX += Math::OSInt2Double(this->mouseDownX - x) / this->zoomScale;
		this->zoomCenterY += Math::OSInt2Double(this->mouseDownY - y) / this->zoomScale;
		if (this->zoomCenterX < this->zoomMinX)
		{
			this->zoomCenterX = this->zoomMinX;
			if (this->zoomScale == this->zoomMinScale)
			{
				action = 1;
			}
		}
		else if (this->zoomCenterX > this->zoomMaxX)
		{
			this->zoomCenterX = this->zoomMaxX;
			if (this->zoomScale == this->zoomMinScale)
			{
				action = 2;
			}
		}

		if (this->zoomCenterY < this->zoomMinY)
			this->zoomCenterY = this->zoomMinY;
		else if (this->zoomCenterY > this->zoomMaxY)
			this->zoomCenterY = this->zoomMaxY;

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

void UI::GUIPictureBoxDD::OnGZoomBegin(OSInt x, OSInt y, UInt64 dist)
{
	this->gzoomDown = true;
	this->gzoomDownX = x;
	this->gzoomDownY = y;
	this->gzoomDownDist = dist;
	this->gzoomCurrX = x;
	this->gzoomCurrY = y;
	this->gzoomCurrDist = dist;
}

void UI::GUIPictureBoxDD::OnGZoomStep(OSInt x, OSInt y, UInt64 dist)
{
	if (this->gzoomDown)
	{
		this->gzoomCurrX = x;
		this->gzoomCurrY = y;
		this->gzoomCurrDist = dist;

		UpdateSubSurface();
		DrawToScreen();
	}
}

void UI::GUIPictureBoxDD::OnGZoomEnd(OSInt x, OSInt y, UInt64 dist)
{
	if (this->gzoomDown)
	{
		this->gzoomDown = false;
		this->gzoomCurrX = x;
		this->gzoomCurrY = y;
		this->gzoomCurrDist = dist;

		this->zoomScale = this->zoomScale * Math::UInt64_Double(this->gzoomCurrDist) / Math::UInt64_Double(this->gzoomDownDist);
		if (this->zoomScale < this->zoomMinScale)
		{
			this->zoomScale = this->zoomMinScale;
		}
		else if (this->zoomScale > 16)
		{
			this->zoomScale = 16.0;
		}
		this->UpdateZoomRange();
		this->zoomCenterX += Math::OSInt2Double(this->gzoomDownX - this->gzoomCurrX) / this->zoomScale;
		this->zoomCenterY += Math::OSInt2Double(this->gzoomDownY - this->gzoomCurrY) / this->zoomScale;

		if (this->zoomCenterX < this->zoomMinX)
			this->zoomCenterX = this->zoomMinX;
		else if (this->zoomCenterX > this->zoomMaxX)
			this->zoomCenterX = this->zoomMaxX;

		if (this->zoomCenterY < this->zoomMinY)
			this->zoomCenterY = this->zoomMinY;
		else if (this->zoomCenterY > this->zoomMaxY)
			this->zoomCenterY = this->zoomMaxY;

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
		this->zoomCenterX += Math::OSInt2Double(axis1) / this->zoomScale;
		this->zoomCenterY += Math::OSInt2Double(axis2) / this->zoomScale;
		if (this->zoomCenterX < this->zoomMinX)
		{
			this->zoomCenterX = this->zoomMinX;
		}
		else if (this->zoomCenterX > this->zoomMaxX)
		{
			this->zoomCenterX = this->zoomMaxX;
		}

		if (this->zoomCenterY < this->zoomMinY)
			this->zoomCenterY = this->zoomMinY;
		else if (this->zoomCenterY > this->zoomMaxY)
			this->zoomCenterY = this->zoomMaxY;

		UpdateSubSurface();
		DrawToScreen();
	}
}


void UI::GUIPictureBoxDD::EventMoveToNext()
{
	UOSInt i;
	UOSInt j;
	i = 0;
	j = this->moveToNextHdlrs->GetCount();
	while (i < j)
	{
		this->moveToNextHdlrs->GetItem(i)(this->moveToNextObjs->GetItem(i));
		i++;
	}
}

void UI::GUIPictureBoxDD::EventMoveToPrev()
{
	UOSInt i;
	UOSInt j;
	i = 0;
	j = this->moveToPrevHdlrs->GetCount();
	while (i < j)
	{
		this->moveToPrevHdlrs->GetItem(i)(this->moveToPrevObjs->GetItem(i));
		i++;
	}
}

Bool UI::GUIPictureBoxDD::GetImageViewSize(UOSInt *viewSize, UOSInt imageWidth, UOSInt imageHeight)
{
	Double par = 1;
	if (this->surfaceW >= imageWidth && this->surfaceH >= imageHeight)
	{
		if (par > 1)
		{
			viewSize[0] = imageWidth;
			viewSize[1] = (UOSInt)Math::Double2OSInt(Math::UOSInt2Double(imageHeight) / par);
		}
		else if (par < 1)
		{
			viewSize[0] = (UOSInt)Math::Double2OSInt(Math::UOSInt2Double(imageWidth) * par);
			viewSize[1] = imageHeight;
		}
		else
		{
			viewSize[0] = imageWidth;
			viewSize[1] = imageHeight;
		}
	}
	else
	{
		UOSInt srcW = imageWidth;
		UOSInt srcH = imageHeight;
		if (Math::UOSInt2Double(srcW) * par * Math::UOSInt2Double(this->surfaceH) > Math::UOSInt2Double(this->surfaceW) * Math::UOSInt2Double(srcH))
		{
			viewSize[0] = this->surfaceW;
			viewSize[1] = (UOSInt)Math::Double2OSInt(Math::UOSInt2Double(this->surfaceW) / par * Math::UOSInt2Double(srcH) / Math::UOSInt2Double(srcW));
		}
		else
		{
			viewSize[0] = (UOSInt)Math::Double2OSInt(Math::UOSInt2Double(this->surfaceH) * par * Math::UOSInt2Double(srcW) / Math::UOSInt2Double(srcH));
			viewSize[1] = this->surfaceH;
		}
	}
	return true;
}

Media::StaticImage *UI::GUIPictureBoxDD::CreatePreviewImage(Media::StaticImage *image)
{
	Media::StaticImage *outImage;
	UOSInt prevSize[2];
	this->GetImageViewSize(prevSize, image->info->dispWidth, image->info->dispHeight);

	UInt8 *prevImgData = MemAllocA(UInt8, image->info->dispWidth * image->info->dispHeight * 8);
	Media::ColorProfile color(image->info->color);
	color.GetRTranParam()->Set(Media::CS::TRANT_LINEAR, 1.0);
	color.GetGTranParam()->Set(Media::CS::TRANT_LINEAR, 1.0);
	color.GetBTranParam()->Set(Media::CS::TRANT_LINEAR, 1.0);
	Media::CS::CSConverter *csConv = Media::CS::CSConverter::NewConverter(image->info->fourcc, image->info->storeBPP, image->info->pf, image->info->color, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, &color, Media::ColorProfile::YUVT_UNKNOWN, this->colorSess);
	Media::Resizer::LanczosResizerLR_C32 *resizer;
	Media::PixelFormat pf = Media::PF_B8G8R8A8;
	NEW_CLASS(resizer, Media::Resizer::LanczosResizerLR_C32(4, 4, image->info->color, this->colorSess, Media::AT_NO_ALPHA, Media::CS::TransferFunc::GetRefLuminance(image->info->color->rtransfer), pf));
	csConv->ConvertV2(&image->data, prevImgData, image->info->dispWidth, image->info->dispHeight, image->info->storeWidth, image->info->storeHeight, (OSInt)image->info->dispWidth * 8, Media::FT_NON_INTERLACE, Media::YCOFST_C_TOP_LEFT);

	NEW_CLASS(outImage, Media::StaticImage(image->info->dispWidth, image->info->dispHeight, 0, 32, pf, 0, image->info->color, Media::ColorProfile::YUVT_UNKNOWN, image->info->atype, image->info->ycOfst));
	resizer->Resize(prevImgData, (OSInt)image->info->dispWidth * 8, Math::UOSInt2Double(image->info->dispWidth), Math::UOSInt2Double(image->info->dispHeight), 0, 0, outImage->data, (OSInt)outImage->GetDataBpl(), outImage->info->dispWidth, outImage->info->dispHeight);

	DEL_CLASS(resizer);
	DEL_CLASS(csConv);
	MemFreeA(prevImgData);
	return outImage;
}

void UI::GUIPictureBoxDD::HandleMouseDown(MouseEventHandler hdlr, void *userObj)
{
	this->mouseDownHdlrs->Add(hdlr);
	this->mouseDownObjs->Add(userObj);
}

void UI::GUIPictureBoxDD::HandleMouseMove(MouseEventHandler hdlr, void *userObj)
{
	this->mouseMoveHdlrs->Add(hdlr);
	this->mouseMoveObjs->Add(userObj);
}

void UI::GUIPictureBoxDD::HandleMouseUp(MouseEventHandler hdlr, void *userObj)
{
	this->mouseUpHdlrs->Add(hdlr);
	this->mouseUpObjs->Add(userObj);
}

void UI::GUIPictureBoxDD::HandleDraw(DrawHandler32 hdlr, void *userObj)
{
	this->drawHdlrs->Add(hdlr);
	this->drawObjs->Add(userObj);
}

void UI::GUIPictureBoxDD::HandleMoveToNext(UI::UIEvent hdlr, void *userObj)
{
	this->moveToNextHdlrs->Add(hdlr);
	this->moveToNextObjs->Add(userObj);
}

void UI::GUIPictureBoxDD::HandleMoveToPrev(UI::UIEvent hdlr, void *userObj)
{
	this->moveToPrevHdlrs->Add(hdlr);
	this->moveToPrevObjs->Add(userObj);
}

void UI::GUIPictureBoxDD::Scn2ImagePos(OSInt x, OSInt y, Double *imgX, Double *imgY)
{
	if (this->currImage == 0)
		return;
	Double srcRect[4];
	OSInt destRect[4];
	CalDispRect(srcRect, destRect);
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

	*imgX = srcRect[0] + Math::OSInt2Double(x - destRect[0]) * srcRect[2] / Math::OSInt2Double(destRect[2]);
	*imgY = srcRect[1] + Math::OSInt2Double(y - destRect[1]) * srcRect[3] / Math::OSInt2Double(destRect[3]);
}

void UI::GUIPictureBoxDD::Image2ScnPos(Double imgX, Double imgY, Double *scnX, Double *scnY)
{
	Double srcRect[4];
	OSInt destRect[4];
	CalDispRect(srcRect, destRect);

	if (this->mouseDowned)
	{
		*scnX = Math::OSInt2Double(destRect[0]) + (imgX - srcRect[0]) * Math::OSInt2Double(destRect[2]) / srcRect[2] - Math::OSInt2Double(this->mouseDownX) + Math::OSInt2Double(this->mouseCurrX);
		*scnY = Math::OSInt2Double(destRect[1]) + (imgY - srcRect[1]) * Math::OSInt2Double(destRect[3]) / srcRect[3] - Math::OSInt2Double(this->mouseDownY) + Math::OSInt2Double(this->mouseCurrY);
	}
	else
	{
		*scnX = Math::OSInt2Double(destRect[0]) + (imgX - srcRect[0]) * Math::OSInt2Double(destRect[2]) / srcRect[2];
		*scnY = Math::OSInt2Double(destRect[1]) + (imgY - srcRect[1]) * Math::OSInt2Double(destRect[3]) / srcRect[3];
	}
}

void UI::GUIPictureBoxDD::ZoomToFit()
{
	this->zoomCenterX = Math::UOSInt2Double(this->currImageW) * 0.5;
	this->zoomCenterY = Math::UOSInt2Double(this->currImageH) * 0.5;
	Double outZoomScale;
	Double outW;
//	Double outH;
	Double srcW = Math::UOSInt2Double(this->currImageW);
	Double srcH = Math::UOSInt2Double(this->currImageH);
	if (srcW * this->currImage->info->par2 * Math::UOSInt2Double(this->surfaceH) > Math::UOSInt2Double(this->surfaceW) * srcH)
	{
		outW = Math::UOSInt2Double(this->surfaceW);
//		outH = this->surfaceW / this->currImage->info->par2 * srcH / srcW;
	}
	else
	{
		outW = Math::UOSInt2Double(this->surfaceH) * this->currImage->info->par2 * srcW / srcH;
//		outH = Math::OSInt2Double(this->surfaceH);
	}
	outZoomScale = outW / srcW;
	this->zoomScale = outZoomScale;
	this->UpdateSubSurface();
	DrawToScreen();
}
