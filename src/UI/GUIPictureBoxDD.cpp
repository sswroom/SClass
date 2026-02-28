#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Media/ImageCopy_C.h"
#include "Media/ImageUtil_C.h"
#include "Media/LRGBLimiter_C.h"
#include "Media/VectorGraph.h"
#include "Media/CS/TransferFunc.h"
#include "Media/Resizer/LanczosResizerLR_C32.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "UI/GUIClientControl.h"
#include "UI/GUIPictureBoxDD.h"

void UI::GUIPictureBoxDD::UpdateSubSurface()
{
	NN<Media::ImageResizer> resizer;
	UnsafeArray<UInt8> imgBuff;
	NN<Media::Image> img;
	if (this->currImage.SetTo(img))
	{
		Math::RectAreaDbl srcRect;
		Math::RectArea<IntOS> destRect;

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
			CalDispRect(srcRect, destRect);
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
			CalDispRect(srcRect, destRect);
			this->zoomCenter = oldCenter;
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
		if (!this->IsSurfaceReady())
			return;

		IntOS bpl;
		UnsafeArray<UInt8> dptr;
		if (!img->IsRaster())
		{
			NN<Media::VectorGraph> vimg = NN<Media::VectorGraph>::ConvertFrom(img);
			NN<Media::DrawEngine> deng = vimg->GetDrawEngine();
			Double scale = IntOS2Double(destRect.GetWidth()) / srcRect.GetWidth();
			NN<Media::DrawImage> dimg;
			if (!Math::IsNAN(scale) && deng->CreateImage32(this->bkBuffSize, Media::AlphaType::AT_IGNORE_ALPHA).SetTo(dimg))
			{
				//dimg->SetClip(destRect.ToDouble());
				vimg->DrawTo(-srcRect.min + destRect.min.ToDouble() / scale, scale, dimg, nullptr);
				//dimg->ClearClip();
				if (this->LockSurfaceBegin(this->bkBuffSize.x, this->bkBuffSize.y, bpl).SetTo(dptr))
				{
					NN<Media::RasterImage> rimg;
					if (dimg->AsRasterImage().SetTo(rimg))
					{
						rimg->GetRasterData(dptr, 0, 0, this->bkBuffSize.x, this->bkBuffSize.y, this->bkBuffSize.x << 2, false, Media::RotateType::None);
					}
					this->LockSurfaceEnd();
				}
				deng->DeleteImage(dimg);
			}
		}
		else if (this->imgBuff.SetTo(imgBuff) && !this->currImage.IsNull())
		{
			if (this->drawHdlrs.GetCount() > 0)
			{
				UnsafeArray<UInt8> bgBuff;
				if (!this->bgBuff.SetTo(bgBuff) || this->bkBuffSize != this->bgBuffSize)
				{
					if (this->bgBuff.SetTo(bgBuff))
					{
						MemFreeAArr(bgBuff);
					}
					this->bgBuffSize = this->bkBuffSize;
					this->bgBuff = bgBuff = MemAllocAArr(UInt8, this->bgBuffSize.CalcArea() * 4);
				}
				ImageUtil_ImageColorFill32(bgBuff.Ptr(), this->bgBuffSize.x, this->bgBuffSize.y, this->bgBuffSize.x << 2, 0);

				dptr = bgBuff;
				Int32 iLeft = (Int32)srcRect.min.x;
				Int32 iTop = (Int32)srcRect.min.y;
				dptr = dptr + destRect.min.y * (IntOS)(this->bgBuffSize.x << 2) + destRect.min.x * 4;
				Math::Size2DDbl srcSize = srcRect.GetSize();
				Math::Size2D<IntOS> destSize = destRect.GetSize();
				if (this->resizer.SetTo(resizer)) resizer->Resize(imgBuff + iLeft * 8 + iTop * (IntOS)this->currImageSize.x * 8, (IntOS)this->currImageSize.x << 3, srcSize.x, srcSize.y, srcRect.min.x - iLeft, srcRect.min.y - iTop, dptr, (IntOS)this->bgBuffSize.x << 2, (UIntOS)destSize.x, (UIntOS)destSize.y);

				this->DrawFromBG();
			}
			else
			{
				if (this->LockSurfaceBegin(this->bkBuffSize.x, this->bkBuffSize.y, bpl).SetTo(dptr))
				{
					ImageUtil_ImageColorFill32(dptr.Ptr(), this->bkBuffSize.x, this->bkBuffSize.y, (UIntOS)bpl, 0);

					Int32 iLeft = (Int32)srcRect.min.x;
					Int32 iTop = (Int32)srcRect.min.y;
					dptr = dptr + destRect.min.y * (IntOS)bpl + destRect.min.x * 4;
					Math::Size2DDbl srcSize = srcRect.GetSize();
					if (srcSize.x > 0 && srcSize.y > 0)
					{
						if (this->resizer.SetTo(resizer)) resizer->Resize(imgBuff + iLeft * 8 + iTop * (IntOS)this->currImageSize.x * 8, (IntOS)this->currImageSize.x << 3, srcSize.x, srcSize.y, srcRect.min.x - iLeft, srcRect.min.y - iTop, dptr, (IntOS)bpl, (UIntOS)destRect.GetWidth(), (UIntOS)destRect.GetHeight());
					}

					this->LockSurfaceEnd();
				}
			}
		}
	}
}

void UI::GUIPictureBoxDD::CalDispRect(NN<Math::RectAreaDbl> srcRect, NN<Math::RectArea<IntOS>> destRect)
{
	if (this->currImage.IsNull())
	{
		srcRect->min = Math::Coord2DDbl(0, 0);
		srcRect->max = Math::Coord2DDbl(0, 0);
		destRect->min = Math::Coord2D<IntOS>(0, 0);
		destRect->max = Math::Coord2D<IntOS>(0, 0);
		return;
	}

	srcRect->min = this->zoomCenter - this->bkBuffSize.ToDouble() * 0.5 / this->zoomScale;
	srcRect->max = srcRect->min + this->bkBuffSize.ToDouble() / this->zoomScale;
	destRect->min = Math::Coord2D<IntOS>(0, 0);
	destRect->max = Math::Coord2D<IntOS>((IntOS)this->bkBuffSize.x, (IntOS)this->bkBuffSize.y);

	if (srcRect->min.x < 0)
	{
		destRect->min.x += Double2IntOS(-srcRect->min.x * this->zoomScale);
		srcRect->min.x = 0;
	}
	if (srcRect->max.x > UIntOS2Double(this->currImageSize.x))
	{
		srcRect->max.x = UIntOS2Double(this->currImageSize.x);
		destRect->max.x = Double2IntOS(srcRect->GetWidth() * this->zoomScale) + destRect->min.x;
	}
	if (srcRect->min.y < 0)
	{
		destRect->min.y = Double2IntOS(-srcRect->min.y * this->zoomScale);
		srcRect->min.y = 0;
	}
	if (srcRect->max.y > UIntOS2Double(this->currImageSize.y))
	{
		srcRect->max.y = UIntOS2Double(this->currImageSize.y);
		destRect->max.y = Double2IntOS(srcRect->GetHeight() * this->zoomScale) + destRect->min.y;
	}
}

void UI::GUIPictureBoxDD::UpdateZoomRange()
{
	if (UIntOS2Double(this->bkBuffSize.x) > UIntOS2Double(this->currImageSize.x) * zoomScale)
	{
		this->zoomMaxX = UIntOS2Double(this->bkBuffSize.x) * 0.5 / zoomScale;
		this->zoomMinX = UIntOS2Double(this->currImageSize.x) - UIntOS2Double(this->bkBuffSize.x) * 0.5 / zoomScale;
	}
	else
	{
		this->zoomMinX = (UIntOS2Double(this->bkBuffSize.x) * 0.5) / zoomScale;
		this->zoomMaxX = UIntOS2Double(this->currImageSize.x) - UIntOS2Double(this->bkBuffSize.x) * 0.5 / zoomScale;
	}
	if (UIntOS2Double(this->bkBuffSize.y) > UIntOS2Double(this->currImageSize.y) * zoomScale)
	{
		this->zoomMaxY = UIntOS2Double(this->bkBuffSize.y) * 0.5 / zoomScale;
		this->zoomMinY = UIntOS2Double(this->currImageSize.y) - UIntOS2Double(this->bkBuffSize.y) * 0.5 / zoomScale;
	}
	else
	{
		this->zoomMinY = (UIntOS2Double(this->bkBuffSize.y) * 0.5) / zoomScale;
		this->zoomMaxY = UIntOS2Double(this->currImageSize.y) - UIntOS2Double(this->bkBuffSize.y) * 0.5 / zoomScale;
	}
}

void UI::GUIPictureBoxDD::UpdateMinScale()
{
	NN<Media::RasterImage> rimg;
	NN<Media::Image> img;
	if (this->currImage.SetTo(img))
	{
		Double outZoomScale;
		Double outW;
	//	Double outH;
		Double srcW = UIntOS2Double(this->currImageSize.x);
		Double srcH = UIntOS2Double(this->currImageSize.y);
		if (img->IsRaster())
		{
			rimg = NN<Media::RasterImage>::ConvertFrom(img);
			if (srcW * rimg->info.CalcPAR() * UIntOS2Double(this->bkBuffSize.y) > UIntOS2Double(this->bkBuffSize.x) * srcH)
			{
				outW = UIntOS2Double(this->bkBuffSize.x);
		//		outH = this->surfaceSize.x / this->currImage->info.par2 * srcH / srcW;
			}
			else
			{
				outW = UIntOS2Double(this->bkBuffSize.y) * rimg->info.CalcPAR() * srcW / srcH;
		//		outH = IntOS2Double(this->surfaceSize.y);
			}
		}
		else
		{
			if (srcW * UIntOS2Double(this->bkBuffSize.y) > UIntOS2Double(this->bkBuffSize.x) * srcH)
			{
				outW = UIntOS2Double(this->bkBuffSize.x);
			}
			else
			{
				outW = UIntOS2Double(this->bkBuffSize.y) * srcW / srcH;
			}
		}
		outZoomScale = outW / srcW;
		if (outZoomScale > 1.0)
		{
			outZoomScale = 1.0;
		}
		this->zoomMinScale = outZoomScale;
	}
}

void UI::GUIPictureBoxDD::CreateResizer()
{
	Media::ColorProfile destColor(Media::ColorProfile::CPT_PDISPLAY);

	this->resizer.Delete();
	Double refLuminance = 0;
	NN<Media::Image> img;
	if (this->currImage.SetTo(img))
	{
		refLuminance = Media::CS::TransferFunc::GetRefLuminance(NN<Media::RasterImage>::ConvertFrom(img)->info.color.rtransfer);
	}
	if (this->curr10Bit)
	{
		NEW_CLASSOPT(this->resizer, Media::Resizer::LanczosResizerLR_C32(4, 3, destColor, this->colorSess.Ptr(), Media::AT_IGNORE_ALPHA, refLuminance, Media::PF_LE_A2B10G10R10));
	}
	else
	{
		NEW_CLASSOPT(this->resizer, Media::Resizer::LanczosResizerLR_C32(4, 3, destColor, this->colorSess.Ptr(), Media::AT_IGNORE_ALPHA, refLuminance, this->GetPixelFormat()));
	}
}

void __stdcall UI::GUIPictureBoxDD::OnSizeChg(AnyType userObj)
{
	NN<UI::GUIPictureBoxDD> me = userObj.GetNN<UI::GUIPictureBoxDD>();
	if (me->switching)
		return;
	Bool new10Bit = false;
	if (me->currScnMode == SM_VFS || me->currScnMode == SM_FS || me->currScnMode == SM_WINDOWED_DIR)
	{
		new10Bit = me->colorSess->Get10BitColor();
	}
	if (!me->currImage.IsNull())
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
	UnsafeArray<UInt8> bgBuff;
	IntOS bpl;
	UnsafeArray<UInt8> dptr;
	if (this->bgBuff.SetTo(bgBuff) && this->LockSurfaceBegin(this->bgBuffSize.x, this->bgBuffSize.y, bpl).SetTo(dptr))
	{
		UIntOS i;
		UIntOS j;
		ImageCopy_ImgCopy(bgBuff.Ptr(), dptr.Ptr(), this->bgBuffSize.x << 2, this->bgBuffSize.y, (IntOS)this->bgBuffSize.x << 2, (IntOS)bpl);
		i = 0;
		j = this->drawHdlrs.GetCount();
		while (i < j)
		{
			Data::CallbackStorage<DrawHandler32> cb = this->drawHdlrs.GetItem(i);
			cb.func(cb.userObj, dptr, this->bgBuffSize.x, this->bgBuffSize.x, (UIntOS)bpl);
			i++;
		}
		this->LockSurfaceEnd();
	}
}

void UI::GUIPictureBoxDD::OnPaint()
{
	if (this->currScnMode != SM_FS && this->currScnMode != SM_VFS)
	{
		if (this->bgBuff.NotNull() && this->drawHdlrs.GetCount() > 0)
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

UI::GUIPictureBoxDD::GUIPictureBoxDD(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::ColorManagerSess> colorSess, Bool allowEnlarge, Bool directMode) : UI::GUIDDrawControl(ui, parent, directMode, colorSess)
{
	this->colorSess = colorSess;
	this->colorSess->AddHandler(*this);
	this->enableLRGBLimit = false;

	this->bgBuff = nullptr;
	this->bgBuffSize = Math::Size2D<UIntOS>(0, 0);
	this->allowEnlarge = allowEnlarge;
	this->currImage = nullptr;
	this->currImageSize = Math::Size2D<UIntOS>(0, 0);
	this->csconv = nullptr;
	this->imgBuff = nullptr;
	this->zoomScale = 1.0;
	this->zoomMinScale = 1.0;
	this->mouseDowned = false;
	this->mouseDownPos = Math::Coord2D<IntOS>(0, 0);
	this->curr10Bit = false;
	this->resizer = nullptr;
	this->zoomCenter = Math::Coord2DDbl(0, 0);
	this->gzoomDown = false;

	CreateResizer();
	this->HandleSizeChanged(OnSizeChg, this);
}

UI::GUIPictureBoxDD::~GUIPictureBoxDD()
{
	UnsafeArray<UInt8> imgBuff;
	if (this->imgBuff.SetTo(imgBuff))
	{
		MemFreeAArr(imgBuff);
		this->imgBuff = nullptr;
	}
	this->csconv.Delete();
	this->resizer.Delete();
	UnsafeArray<UInt8> bgBuff;
	if (this->bgBuff.SetTo(bgBuff))
	{
		MemFreeAArr(bgBuff);
		this->bgBuff = nullptr;
	}
	this->colorSess->RemoveHandler(*this);
}

Text::CStringNN UI::GUIPictureBoxDD::GetObjectClass() const
{
	return CSTR("PictureBoxDD");
}

IntOS UI::GUIPictureBoxDD::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GUIPictureBoxDD::ChangeMonitor(Optional<MonitorHandle> hMon)
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

void UI::GUIPictureBoxDD::SetImage(Optional<Media::Image> currImage, Bool sameImg)
{
	UnsafeArray<UInt8> imgBuff;
	Math::Size2D<UIntOS> oriSize = this->currImageSize;
	this->currImage = currImage;
	if (this->imgBuff.SetTo(imgBuff))
	{
		MemFreeAArr(imgBuff);
		this->imgBuff = nullptr;
	}
	this->csconv.Delete();
	NN<Media::RasterImage> rimg;
	NN<Media::Image> img;
	if (this->currImage.SetTo(img))
	{
		if (img->IsRaster())
		{
			rimg = NN<Media::RasterImage>::ConvertFrom(img);
			Media::RotateType rotType = Media::RotateType::None;
			NN<Media::EXIFData> exif;
			this->currImageSize = rimg->info.dispSize;
			if (rimg->exif.SetTo(exif))
			{
				rotType = exif->GetRotateType();
				if (rotType == Media::RotateType::CW_90)
				{
					this->currImageSize = rimg->info.dispSize.SwapXY();
				}
				else if (rotType == Media::RotateType::CW_180)
				{
				}
				else if (rotType == Media::RotateType::CW_270)
				{
					this->currImageSize = rimg->info.dispSize.SwapXY();
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
			this->csconv = Media::CS::CSConverter::NewConverter(rimg->info.fourcc, rimg->info.storeBPP, rimg->info.pf, rimg->info.color, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, color, rimg->info.yuvType, this->colorSess.Ptr());
			NN<Media::CS::CSConverter> csconv;
			if (this->csconv.SetTo(csconv))
			{
				UnsafeArray<UInt8> pal;
				if (rimg->pal.SetTo(pal))
				{
					csconv->SetPalette(pal);
				}
				this->imgBuff = imgBuff = MemAllocAArr(UInt8, this->currImageSize.CalcArea() * 8);
				if (rimg->GetImageClass() == Media::RasterImage::ImageClass::StaticImage)
				{
					csconv->ConvertV2(&NN<Media::StaticImage>::ConvertFrom(rimg)->data, imgBuff, rimg->info.dispSize.x, rimg->info.dispSize.y, rimg->info.storeSize.x, rimg->info.storeSize.y, (IntOS)rimg->info.dispSize.x << 3, rimg->info.ftype, rimg->info.ycOfst);
				}
				else
				{
					UnsafeArray<UInt8> imgData = MemAllocAArr(UInt8, rimg->GetDataBpl() * rimg->info.storeSize.y);
					rimg->GetRasterData(imgData, 0, 0, rimg->info.storeSize.x, rimg->info.dispSize.y, rimg->GetDataBpl(), rimg->IsUpsideDown(), rimg->info.rotateType);
					csconv->ConvertV2(&imgData, imgBuff, rimg->info.dispSize.x, rimg->info.dispSize.y, rimg->info.storeSize.x, rimg->info.storeSize.y, (IntOS)rimg->info.dispSize.x << 3, rimg->info.ftype, rimg->info.ycOfst);
					MemFreeAArr(imgData);
				}
				if (this->enableLRGBLimit)
				{
					LRGBLimiter_LimitImageLRGB(imgBuff.Ptr(), rimg->info.dispSize.x, rimg->info.dispSize.y);
				}

				if (rotType == Media::RotateType::CW_90)
				{
					UInt8 *tmpBuff = MemAllocA(UInt8, this->currImageSize.CalcArea() * 8);
					ImageUtil_Rotate64_CW90(imgBuff.Ptr(), tmpBuff, this->currImageSize.y, this->currImageSize.x, this->currImageSize.y << 3, this->currImageSize.x << 3);
					MemFreeAArr(imgBuff);
					this->imgBuff = tmpBuff;
				}
				else if (rotType == Media::RotateType::CW_180)
				{
					UInt8 *tmpBuff = MemAllocA(UInt8, this->currImageSize.CalcArea() * 8);
					ImageUtil_Rotate64_CW180(imgBuff.Ptr(), tmpBuff, this->currImageSize.x, this->currImageSize.y, this->currImageSize.x << 3, this->currImageSize.x << 3);
					MemFreeAArr(imgBuff);
					this->imgBuff = tmpBuff;
				}
				else if (rotType == Media::RotateType::CW_270)
				{
					UInt8 *tmpBuff = MemAllocA(UInt8, this->currImageSize.CalcArea() * 8);
					ImageUtil_Rotate64_CW270(imgBuff.Ptr(), tmpBuff, this->currImageSize.y, this->currImageSize.x, this->currImageSize.y << 3, this->currImageSize.x << 3);
					MemFreeAArr(imgBuff);
					this->imgBuff = tmpBuff;
				}
				this->UpdateSubSurface();
				DrawToScreen();
			}
		}
		else
		{
			NN<Media::VectorGraph> vimg = NN<Media::VectorGraph>::ConvertFrom(img);
			this->currImageSize = vimg->GetSize();
			this->zoomCenter = this->currImageSize.ToDouble() * 0.5;
			this->UpdateMinScale();
			this->zoomScale = this->zoomMinScale;
			this->UpdateZoomRange();
			this->UpdateSubSurface();
			DrawToScreen();

		}
	}
	else
	{
		this->currImageSize = Math::Size2D<UIntOS>(0, 0);
	}
}

void UI::GUIPictureBoxDD::YUVParamChanged(NN<const Media::ColorHandler::YUVPARAM> yuvParam)
{
	NN<Media::Image> img;
	NN<Media::RasterImage> rimg;
	NN<Media::CS::CSConverter> csconv;
	UnsafeArray<UInt8> thisImgBuff;
	if (this->currImage.SetTo(img) && img->IsRaster() && this->csconv.SetTo(csconv) && this->imgBuff.SetTo(thisImgBuff))
	{
		rimg = NN<Media::RasterImage>::ConvertFrom(img);
		Media::RotateType rotType = Media::RotateType::None;
		NN<Media::EXIFData> exif;
		UnsafeArray<UInt8> imgBuff;
		if (rimg->exif.SetTo(exif))
		{
			rotType = exif->GetRotateType();
		}
		if (rotType == Media::RotateType::None)
		{
			imgBuff = thisImgBuff;
		}
		else
		{
			imgBuff = MemAllocA(UInt8, this->currImageSize.CalcArea() * 8);
		}
		if (rimg->GetImageClass() == Media::RasterImage::ImageClass::StaticImage)
		{
			csconv->ConvertV2(&NN<Media::StaticImage>::ConvertFrom(rimg)->data, imgBuff, rimg->info.dispSize.x, rimg->info.dispSize.y, rimg->info.storeSize.x, rimg->info.storeSize.y, (IntOS)rimg->info.dispSize.x << 3, rimg->info.ftype, rimg->info.ycOfst);
		}
		else
		{
			UnsafeArray<UInt8> imgData = MemAllocAArr(UInt8, rimg->GetDataBpl() * rimg->info.storeSize.y);
			rimg->GetRasterData(imgData, 0, 0, rimg->info.storeSize.x, rimg->info.dispSize.y, rimg->GetDataBpl(), rimg->IsUpsideDown(), rimg->info.rotateType);
			csconv->ConvertV2(&imgData, imgBuff, rimg->info.dispSize.x, rimg->info.dispSize.y, rimg->info.storeSize.x, rimg->info.storeSize.y, (IntOS)rimg->info.dispSize.x << 3, rimg->info.ftype, rimg->info.ycOfst);
			MemFreeAArr(imgData);
		}
		if (this->enableLRGBLimit)
		{
			LRGBLimiter_LimitImageLRGB(imgBuff.Ptr(), rimg->info.dispSize.x, rimg->info.dispSize.y);
		}
		if (rotType == Media::RotateType::CW_90)
		{
			ImageUtil_Rotate64_CW90(imgBuff.Ptr(), thisImgBuff.Ptr(), this->currImageSize.y, this->currImageSize.x, this->currImageSize.y << 3, this->currImageSize.x << 3);
			MemFreeAArr(imgBuff);
		}
		else if (rotType == Media::RotateType::CW_180)
		{
			ImageUtil_Rotate64_CW180(imgBuff.Ptr(), thisImgBuff.Ptr(), this->currImageSize.x, this->currImageSize.y, this->currImageSize.x << 3, this->currImageSize.x << 3);
			MemFreeAArr(imgBuff);
		}
		else if (rotType == Media::RotateType::CW_270)
		{
			ImageUtil_Rotate64_CW270(imgBuff.Ptr(), thisImgBuff.Ptr(), this->currImageSize.y, this->currImageSize.x, this->currImageSize.y << 3, this->currImageSize.x << 3);
			MemFreeAArr(imgBuff);
		}
		UpdateSubSurface();
		DrawToScreen();
	}
}

void UI::GUIPictureBoxDD::RGBParamChanged(NN<const Media::ColorHandler::RGBPARAM2> rgbParam)
{
	NN<Media::CS::CSConverter> csconv;
	NN<Media::Image> img;
	NN<Media::RasterImage> rimg;
	UnsafeArray<UInt8> thisImgBuff;
	if (this->currImage.SetTo(img) && img->IsRaster() && this->csconv.SetTo(csconv) && this->imgBuff.SetTo(thisImgBuff))
	{
		rimg = NN<Media::RasterImage>::ConvertFrom(img);
		Media::RotateType rotType = Media::RotateType::None;
		UnsafeArray<UInt8> imgBuff;
		NN<Media::EXIFData> exif;
		if (rimg->exif.SetTo(exif))
		{
			rotType = exif->GetRotateType();
		}
		if (rotType == Media::RotateType::None)
		{
			imgBuff = thisImgBuff;
		}
		else
		{
			imgBuff = MemAllocA(UInt8, this->currImageSize.CalcArea() * 8);
		}
		if (rimg->GetImageClass() == Media::RasterImage::ImageClass::StaticImage)
		{
			csconv->ConvertV2(&NN<Media::StaticImage>::ConvertFrom(rimg)->data, imgBuff, rimg->info.dispSize.x, rimg->info.dispSize.y, rimg->info.storeSize.x, rimg->info.storeSize.y, (IntOS)rimg->info.dispSize.x << 3, rimg->info.ftype, rimg->info.ycOfst);
		}
		else
		{
			UnsafeArray<UInt8> imgData = MemAllocAArr(UInt8, rimg->GetDataBpl() * rimg->info.dispSize.y);
			rimg->GetRasterData(imgData, 0, 0, rimg->info.storeSize.x, rimg->info.dispSize.y, rimg->GetDataBpl(), rimg->IsUpsideDown(), rimg->info.rotateType);
			csconv->ConvertV2(&imgData, imgBuff, rimg->info.dispSize.x, rimg->info.dispSize.y, rimg->info.storeSize.x, rimg->info.storeSize.y, (IntOS)rimg->info.dispSize.x << 3, rimg->info.ftype, rimg->info.ycOfst);
			MemFreeAArr(imgData);
		}
		if (this->enableLRGBLimit)
		{
			LRGBLimiter_LimitImageLRGB(imgBuff.Ptr(), rimg->info.dispSize.x, rimg->info.dispSize.y);
		}
		if (rotType == Media::RotateType::CW_90)
		{
			ImageUtil_Rotate64_CW90(imgBuff.Ptr(), thisImgBuff.Ptr(), this->currImageSize.y, this->currImageSize.x, this->currImageSize.y << 3, this->currImageSize.x << 3);
			MemFreeAArr(imgBuff);
		}
		else if (rotType == Media::RotateType::CW_180)
		{
			ImageUtil_Rotate64_CW180(imgBuff.Ptr(), thisImgBuff.Ptr(), this->currImageSize.x, this->currImageSize.y, this->currImageSize.x << 3, this->currImageSize.x << 3);
			MemFreeAArr(imgBuff);
		}
		else if (rotType == Media::RotateType::CW_270)
		{
			ImageUtil_Rotate64_CW270(imgBuff.Ptr(), thisImgBuff.Ptr(), this->currImageSize.y, this->currImageSize.x, this->currImageSize.y << 3, this->currImageSize.x << 3);
			MemFreeAArr(imgBuff);
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

void UI::GUIPictureBoxDD::OnMouseWheel(Math::Coord2D<IntOS> pos, Int32 amount)
{
	Math::Coord2DDbl mousePoint;
	Math::RectAreaDbl srcRect;
	Math::RectArea<IntOS> destRect;

	if (this->currImage.IsNull())
		return;
	if (amount > 0)
	{
		if (zoomScale >= 16)
		{
			return;
		}
		CalDispRect(srcRect, destRect);
		mousePoint = srcRect.min + (pos - destRect.min).ToDouble() * srcRect.GetSize() / destRect.GetSize().ToDouble();
		zoomScale = zoomScale * 2;
		UpdateZoomRange();

		CalDispRect(srcRect, destRect);
		this->zoomCenter -= (pos.ToDouble() - (mousePoint - srcRect.min) * destRect.GetSize().ToDouble() / srcRect.GetSize() - destRect.min.ToDouble()) / zoomScale;
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
		CalDispRect(srcRect, destRect);
		mousePoint = srcRect.min + (pos - destRect.min).ToDouble() * srcRect.GetSize() / destRect.GetSize().ToDouble();

		zoomScale = zoomScale * 0.5;
		if (zoomScale < this->zoomMinScale)
		{
			zoomScale = this->zoomMinScale;
		}
		UpdateZoomRange();

		CalDispRect(srcRect, destRect);
		this->zoomCenter -= (pos.ToDouble() - (mousePoint - srcRect.min) * destRect.GetSize().ToDouble() / srcRect.GetSize() - destRect.min.ToDouble()) / zoomScale;

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

void UI::GUIPictureBoxDD::OnMouseMove(Math::Coord2D<IntOS> pos)
{
	this->mouseCurrPos = pos;
	UIntOS i;
	UIntOS j;
	i = 0;
	j = this->mouseMoveHdlrs.GetCount();
	while (i < j)
	{
		Data::CallbackStorage<MouseEventHandler> cb = this->mouseMoveHdlrs.GetItem(i);
		if (cb.func(cb.userObj, pos, UI::GUIControl::MBTN_LEFT) == EventState::StopEvent)
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

void UI::GUIPictureBoxDD::OnMouseDown(Math::Coord2D<IntOS> pos, MouseButton button)
{
	this->Focus();
	UIntOS i;
	UIntOS j;
	i = 0;
	j = this->mouseDownHdlrs.GetCount();
	while (i < j)
	{
		Data::CallbackStorage<MouseEventHandler> cb = this->mouseDownHdlrs.GetItem(i);
		if (cb.func(cb.userObj, pos, button) == EventState::StopEvent)
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

void UI::GUIPictureBoxDD::OnMouseUp(Math::Coord2D<IntOS> pos, MouseButton button)
{
	UIntOS i;
	UIntOS j;
	i = 0;
	j = this->mouseUpHdlrs.GetCount();
	while (i < j)
	{
		Data::CallbackStorage<MouseEventHandler> cb = this->mouseUpHdlrs.GetItem(i);
		if (cb.func(cb.userObj, pos, button) == EventState::StopEvent)
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

void UI::GUIPictureBoxDD::OnGZoomBegin(Math::Coord2D<IntOS> pos, UInt64 dist)
{
	this->gzoomDown = true;
	this->gzoomDownPos = pos;
	this->gzoomDownDist = dist;
	this->gzoomCurrPos = pos;
	this->gzoomCurrDist = dist;
}

void UI::GUIPictureBoxDD::OnGZoomStep(Math::Coord2D<IntOS> pos, UInt64 dist)
{
	if (this->gzoomDown)
	{
		this->gzoomCurrPos = pos;
		this->gzoomCurrDist = dist;

		UpdateSubSurface();
		DrawToScreen();
	}
}

void UI::GUIPictureBoxDD::OnGZoomEnd(Math::Coord2D<IntOS> pos, UInt64 dist)
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

void UI::GUIPictureBoxDD::OnJSButtonDown(IntOS buttonId)
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

void UI::GUIPictureBoxDD::OnJSButtonUp(IntOS buttonId)
{
}

void UI::GUIPictureBoxDD::OnJSAxis(IntOS axis1, IntOS axis2, IntOS axis3, IntOS axis4)
{
	axis1 >>= 10;
	axis2 >>= 10;
	if (axis1 < 0)
		axis1++;
	if (axis2 < 0)
		axis2++;
	if (axis1 != 0 || axis2 != 0)
	{
		this->zoomCenter.x += IntOS2Double(axis1) / this->zoomScale;
		this->zoomCenter.y += IntOS2Double(axis2) / this->zoomScale;
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
	UIntOS i;
	UIntOS j;
	i = 0;
	j = this->moveToNextHdlrs.GetCount();
	while (i < j)
	{
		Data::CallbackStorage<UI::UIEvent> cb = this->moveToNextHdlrs.GetItem(i);
		cb.func(cb.userObj);
		i++;
	}
}

void UI::GUIPictureBoxDD::EventMoveToPrev()
{
	UIntOS i;
	UIntOS j;
	i = 0;
	j = this->moveToPrevHdlrs.GetCount();
	while (i < j)
	{
		Data::CallbackStorage<UI::UIEvent> cb = this->moveToPrevHdlrs.GetItem(i);
		cb.func(cb.userObj);
		i++;
	}
}

Bool UI::GUIPictureBoxDD::GetImageViewSize(Math::Size2D<UIntOS> *viewSize, Math::Size2D<UIntOS> imageSize)
{
	Double par = 1;
	if (this->bkBuffSize.x >= imageSize.x && this->bkBuffSize.y >= imageSize.y)
	{
		if (par > 1)
		{
			viewSize->x = imageSize.x;
			viewSize->y = (UIntOS)Double2IntOS(UIntOS2Double(imageSize.y) / par);
		}
		else if (par < 1)
		{
			viewSize->x = (UIntOS)Double2IntOS(UIntOS2Double(imageSize.x) * par);
			viewSize->y = imageSize.y;
		}
		else
		{
			*viewSize = imageSize;
		}
	}
	else
	{
		UIntOS srcW = imageSize.x;
		UIntOS srcH = imageSize.y;
		if (UIntOS2Double(srcW) * par * UIntOS2Double(this->bkBuffSize.y) > UIntOS2Double(this->bkBuffSize.x) * UIntOS2Double(srcH))
		{
			viewSize->x = this->bkBuffSize.x;
			viewSize->y = (UIntOS)Double2IntOS(UIntOS2Double(this->bkBuffSize.x) / par * UIntOS2Double(srcH) / UIntOS2Double(srcW));
		}
		else
		{
			viewSize->x = (UIntOS)Double2IntOS(UIntOS2Double(this->bkBuffSize.y) * par * UIntOS2Double(srcW) / UIntOS2Double(srcH));
			viewSize->y = this->bkBuffSize.y;
		}
	}
	return true;
}

Optional<Media::StaticImage> UI::GUIPictureBoxDD::CreatePreviewImage(NN<const Media::StaticImage> image)
{
	NN<Media::StaticImage> outImage;
	Math::Size2D<UIntOS> prevSize;
	this->GetImageViewSize(&prevSize, image->info.dispSize);

	UInt8 *prevImgData = MemAllocA(UInt8, image->info.dispSize.CalcArea() * 8);
	Media::ColorProfile color(NN<const Media::ColorProfile>(image->info.color));
	color.GetRTranParam()->Set(Media::CS::TRANT_LINEAR, 1.0);
	color.GetGTranParam()->Set(Media::CS::TRANT_LINEAR, 1.0);
	color.GetBTranParam()->Set(Media::CS::TRANT_LINEAR, 1.0);
	NN<Media::CS::CSConverter> csConv;
	if (!Media::CS::CSConverter::NewConverter(image->info.fourcc, image->info.storeBPP, image->info.pf, image->info.color, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, color, Media::ColorProfile::YUVT_UNKNOWN, this->colorSess.Ptr()).SetTo(csConv))
	{
		MemFreeA(prevImgData);
		return nullptr;
	}
	Media::Resizer::LanczosResizerLR_C32 *resizer;
	Media::PixelFormat pf = Media::PF_B8G8R8A8;
	NEW_CLASS(resizer, Media::Resizer::LanczosResizerLR_C32(4, 4, image->info.color, this->colorSess.Ptr(), Media::AT_IGNORE_ALPHA, Media::CS::TransferFunc::GetRefLuminance(image->info.color.rtransfer), pf));
	csConv->ConvertV2(&image->data, prevImgData, image->info.dispSize.x, image->info.dispSize.y, image->info.storeSize.x, image->info.storeSize.y, (IntOS)image->info.dispSize.x * 8, Media::FT_NON_INTERLACE, Media::YCOFST_C_TOP_LEFT);

	NEW_CLASSNN(outImage, Media::StaticImage(image->info.dispSize, 0, 32, pf, 0, image->info.color, Media::ColorProfile::YUVT_UNKNOWN, image->info.atype, image->info.ycOfst));
	resizer->Resize(prevImgData, (IntOS)image->info.dispSize.x * 8, UIntOS2Double(image->info.dispSize.x), UIntOS2Double(image->info.dispSize.y), 0, 0, outImage->data.Ptr(), (IntOS)outImage->GetDataBpl(), outImage->info.dispSize.x, outImage->info.dispSize.y);

	DEL_CLASS(resizer);
	csConv.Delete();
	MemFreeA(prevImgData);
	return outImage;
}

void UI::GUIPictureBoxDD::HandleMouseDown(MouseEventHandler hdlr, AnyType userObj)
{
	this->mouseDownHdlrs.Add({hdlr, userObj});
}

void UI::GUIPictureBoxDD::HandleMouseMove(MouseEventHandler hdlr, AnyType userObj)
{
	this->mouseMoveHdlrs.Add({hdlr, userObj});
}

void UI::GUIPictureBoxDD::HandleMouseUp(MouseEventHandler hdlr, AnyType userObj)
{
	this->mouseUpHdlrs.Add({hdlr, userObj});
}

void UI::GUIPictureBoxDD::HandleDraw(DrawHandler32 hdlr, AnyType userObj)
{
	this->drawHdlrs.Add({hdlr, userObj});
}

void UI::GUIPictureBoxDD::HandleMoveToNext(UI::UIEvent hdlr, AnyType userObj)
{
	this->moveToNextHdlrs.Add({hdlr, userObj});
}

void UI::GUIPictureBoxDD::HandleMoveToPrev(UI::UIEvent hdlr, AnyType userObj)
{
	this->moveToPrevHdlrs.Add({hdlr, userObj});
}

Math::Coord2DDbl UI::GUIPictureBoxDD::Scn2ImagePos(Math::Coord2D<IntOS> scnPos)
{
	if (this->currImage.IsNull())
		return Math::Coord2DDbl(0, 0);
	Math::RectAreaDbl srcRect;
	Math::RectArea<IntOS> destRect;
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

	return srcRect.min + (scnPos - destRect.min).ToDouble() * srcRect.GetSize() / destRect.GetSize().ToDouble();
}

Math::Coord2DDbl UI::GUIPictureBoxDD::Image2ScnPos(Math::Coord2DDbl imgPos)
{
	Math::RectAreaDbl srcRect;
	Math::RectArea<IntOS> destRect;
	CalDispRect(srcRect, destRect);

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
		return destRect.min.ToDouble() + (imgPos - srcRect.min) * destRect.GetSize().ToDouble() / srcRect.GetSize() - sub;
	}
	else
	{
		return destRect.min.ToDouble() + (imgPos - srcRect.min) * destRect.GetSize().ToDouble() / srcRect.GetSize();
	}
}

void UI::GUIPictureBoxDD::ZoomToFit()
{
	NN<Media::Image> img;
	NN<Media::RasterImage> rimg;
	if (this->currImage.SetTo(img))
	{
		this->zoomCenter = this->currImageSize.ToDouble() * 0.5;
		Double outZoomScale;
		Double outW;
	//	Double outH;
		Math::Size2DDbl srcSize = this->currImageSize.ToDouble();
		if (img->IsRaster())
		{
			rimg = NN<Media::RasterImage>::ConvertFrom(img);
			if (srcSize.x * rimg->info.CalcPAR() * UIntOS2Double(this->bkBuffSize.y) > UIntOS2Double(this->bkBuffSize.x) * srcSize.y)
			{
				outW = UIntOS2Double(this->bkBuffSize.x);
		//		outH = this->surfaceW / this->currImage->info.par2 * srcH / srcW;
			}
			else
			{
				outW = UIntOS2Double(this->bkBuffSize.y) * rimg->info.CalcPAR() * srcSize.x / srcSize.y;
		//		outH = IntOS2Double(this->surfaceH);
			}
		}
		else
		{
			if (srcSize.x * UIntOS2Double(this->bkBuffSize.y) > UIntOS2Double(this->bkBuffSize.x) * srcSize.y)
			{
				outW = UIntOS2Double(this->bkBuffSize.x);
			}
			else
			{
				outW = UIntOS2Double(this->bkBuffSize.y) * srcSize.x / srcSize.y;
			}
		}
		outZoomScale = outW / srcSize.x;
		this->zoomScale = outZoomScale;
		this->UpdateSubSurface();
		DrawToScreen();
	}
}

void UI::GUIPictureBoxDD::UpdateBufferImage()
{
	this->UpdateSubSurface();
	DrawToScreen();
}
