#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Media/ImageUtil_C.h"
#include "Media/StaticImage.h"
#include "Media/Batch/BatchWatermarker.h"

Media::Batch::BatchWatermarker::BatchWatermarker(NN<Media::DrawEngine> deng, Optional<Media::Batch::BatchHandler> hdlr)
{
	this->deng = deng;
	this->hdlr = hdlr;
	this->watermark = nullptr;
}

Media::Batch::BatchWatermarker::~BatchWatermarker()
{
	OPTSTR_DEL(this->watermark);
}
			
void Media::Batch::BatchWatermarker::SetWatermark(Text::CString watermark)
{
	OPTSTR_DEL(this->watermark);
	Text::CStringNN nnwatermark;
	if (watermark.SetTo(nnwatermark) && nnwatermark.leng > 0)
	{
		this->watermark = Text::String::New(nnwatermark);
	}
}

void Media::Batch::BatchWatermarker::SetHandler(Optional<Media::Batch::BatchHandler> hdlr)
{
	this->hdlr = hdlr;
}

void Media::Batch::BatchWatermarker::ImageOutput(NN<Media::ImageList> imgList, Text::CStringNN fileId, Text::CStringNN subId)
{
	NN<Media::Batch::BatchHandler> hdlr;
	if (!this->hdlr.SetTo(hdlr))
		return;

	NN<Text::String> watermark;
	if (!this->watermark.SetTo(watermark) || watermark->leng == 0)
	{
		hdlr->ImageOutput(imgList, fileId, subId);
		return;
	}
	UIntOS i;
	UIntOS j;
	NN<Media::StaticImage> simg;
	NN<Media::DrawImage> tmpImg;
	if (this->deng->CreateImage32(Math::Size2D<UIntOS>(16, 16), Media::AT_ALPHA_ALL_FF).SetTo(tmpImg))
	{
		i = 0;
		j = imgList->GetCount();
		while (i < j)
		{
			Int32 xRand;
			Int32 yRand;
			Double fontSizePx;
			Math::Size2DDbl sz;
			UInt32 iWidth;
			UInt32 iHeight;
			NN<Media::DrawImage> gimg2;
			NN<Media::DrawBrush> b = tmpImg->NewBrushARGB(0xffffffff);
			NN<Media::DrawFont> f;

			if (Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(i, 0)).SetTo(simg))
			{
				fontSizePx = UIntOS2Double(simg->info.dispSize.x) / 12.0;

				while (true)
				{
					f = tmpImg->NewFontPx(CSTR("Arial"), fontSizePx, Media::DrawEngine::DFS_NORMAL, 0);
					sz = tmpImg->GetTextSize(f, watermark->ToCString());
					if (sz.x == 0 || sz.y == 0)
					{
						tmpImg->DelFont(f);
						break;
					}
					if (sz.x <= UIntOS2Double(simg->info.dispSize.x) && sz.y <= UIntOS2Double(simg->info.dispSize.y))
					{
						xRand = Double2Int32(UIntOS2Double(simg->info.dispSize.x) - sz.x);
						yRand = Double2Int32(UIntOS2Double(simg->info.dispSize.y) - sz.y);
						iWidth = (UInt32)Double2Int32(sz.x);
						iHeight = (UInt32)Double2Int32(sz.y);
						if (this->deng->CreateImage32(Math::Size2D<UIntOS>(iWidth, iHeight), Media::AT_ALPHA_ALL_FF).SetTo(gimg2))
						{
							gimg2->DrawString(Math::Coord2DDbl(0, 0), watermark, f, b);
							gimg2->SetAlphaType(Media::AT_ALPHA);
							Bool revOrder;
							UnsafeArray<UInt8> bmpBits;
							if (gimg2->GetImgBits(revOrder).SetTo(bmpBits))
							{
								ImageUtil_ColorReplace32A(bmpBits.Ptr(), iWidth, iHeight, (this->rnd.NextInt30() & 0xffffff) | 0x5f808080);
								if (revOrder)
								{
									this->ablend.Blend(simg->data + (UInt32)Double2Int32(this->rnd.NextDouble() * yRand) * simg->info.storeSize.x * 4 + Double2Int32(this->rnd.NextDouble() * xRand) * 4, (IntOS)simg->info.storeSize.x << 2, bmpBits + iWidth * 4 * (iHeight - 1), -(Int32)iWidth * 4, iWidth, iHeight, Media::AT_ALPHA);
								}
								else
								{
									this->ablend.Blend(simg->data + (UInt32)Double2Int32(this->rnd.NextDouble() * yRand) * simg->info.storeSize.x * 4 + Double2Int32(this->rnd.NextDouble() * xRand) * 4, (IntOS)simg->info.storeSize.x << 2, bmpBits, (Int32)iWidth * 4, iWidth, iHeight, Media::AT_ALPHA);
								}
							}
							this->deng->DeleteImage(gimg2);
						}
						tmpImg->DelFont(f);
						break;
					}
					else
					{
						tmpImg->DelFont(f);
						fontSizePx--;
					}
				}
			}
			tmpImg->DelBrush(b);
			i++;
		}
		this->deng->DeleteImage(tmpImg);
		hdlr->ImageOutput(imgList, fileId, subId);
	}
}
