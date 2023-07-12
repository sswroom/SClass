#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/ImageUtil.h"
#include "Media/StaticImage.h"
#include "Media/Batch/BatchWatermarker.h"

Media::Batch::BatchWatermarker::BatchWatermarker(Media::DrawEngine *deng, Media::Batch::BatchHandler *hdlr)
{
	this->deng = deng;
	this->hdlr = hdlr;
	this->watermark = 0;
}

Media::Batch::BatchWatermarker::~BatchWatermarker()
{
	SDEL_STRING(this->watermark);
}
			
void Media::Batch::BatchWatermarker::SetWatermark(Text::CString watermark)
{
	SDEL_STRING(this->watermark);
	if (watermark.leng > 0)
	{
		this->watermark = Text::String::New(watermark).Ptr();
	}
}

void Media::Batch::BatchWatermarker::SetHandler(Media::Batch::BatchHandler *hdlr)
{
	this->hdlr = hdlr;
}

void Media::Batch::BatchWatermarker::ImageOutput(Media::ImageList *imgList, const UTF8Char *fileId, const UTF8Char *subId)
{
	if (this->hdlr == 0)
		return;
	if (this->watermark == 0 || this->watermark->leng == 0)
	{
		this->hdlr->ImageOutput(imgList, fileId, subId);
		return;
	}
	NotNullPtr<Text::String> watermark;
	if (!watermark.Set(this->watermark))
		return;
	UOSInt i;
	UOSInt j;
	Media::StaticImage *simg;
	Media::DrawImage *tmpImg = this->deng->CreateImage32(Math::Size2D<UOSInt>(16, 16), Media::AT_NO_ALPHA);
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
		Media::DrawImage *gimg2;
		Media::DrawBrush *b = tmpImg->NewBrushARGB(0xffffffff);
		Media::DrawFont *f;

		simg = (Media::StaticImage*)imgList->GetImage(i, 0);
		fontSizePx = UOSInt2Double(simg->info.dispSize.x) / 12.0;

		while (true)
		{
			f = tmpImg->NewFontPx(CSTR("Arial"), fontSizePx, Media::DrawEngine::DFS_NORMAL, 0);
			sz = tmpImg->GetTextSize(f, this->watermark->ToCString());
			if (sz.x == 0 || sz.y == 0)
			{
				tmpImg->DelFont(f);
				break;
			}
			if (sz.x <= UOSInt2Double(simg->info.dispSize.x) && sz.y <= UOSInt2Double(simg->info.dispSize.y))
			{
				xRand = Double2Int32(UOSInt2Double(simg->info.dispSize.x) - sz.x);
				yRand = Double2Int32(UOSInt2Double(simg->info.dispSize.y) - sz.y);
				iWidth = (UInt32)Double2Int32(sz.x);
				iHeight = (UInt32)Double2Int32(sz.y);
				gimg2 = this->deng->CreateImage32(Math::Size2D<UOSInt>(iWidth, iHeight), Media::AT_NO_ALPHA);
				gimg2->DrawString(Math::Coord2DDbl(0, 0), watermark, f, b);
				gimg2->SetAlphaType(Media::AT_ALPHA);
				Bool revOrder;
				UInt8 *bmpBits = gimg2->GetImgBits(&revOrder);
				ImageUtil_ColorReplace32A(bmpBits, iWidth, iHeight, (this->rnd.NextInt30() & 0xffffff) | 0x5f808080);
				if (revOrder)
				{
					this->ablend.Blend(simg->data + (UInt32)Double2Int32(this->rnd.NextDouble() * yRand) * simg->info.storeSize.x * 4 + Double2Int32(this->rnd.NextDouble() * xRand) * 4, (OSInt)simg->info.storeSize.x << 2, bmpBits + iWidth * 4 * (iHeight - 1), -(Int32)iWidth * 4, iWidth, iHeight, Media::AT_ALPHA);
				}
				else
				{
					this->ablend.Blend(simg->data + (UInt32)Double2Int32(this->rnd.NextDouble() * yRand) * simg->info.storeSize.x * 4 + Double2Int32(this->rnd.NextDouble() * xRand) * 4, (OSInt)simg->info.storeSize.x << 2, bmpBits, (Int32)iWidth * 4, iWidth, iHeight, Media::AT_ALPHA);
				}
				this->deng->DeleteImage(gimg2);
				tmpImg->DelFont(f);
				break;
			}
			else
			{
				tmpImg->DelFont(f);
				fontSizePx--;
			}
		}
		tmpImg->DelBrush(b);
		i++;
	}
	this->deng->DeleteImage(tmpImg);
	this->hdlr->ImageOutput(imgList, fileId, subId);
}
