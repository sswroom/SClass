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
	NEW_CLASS(this->rnd, Data::RandomOS());
	NEW_CLASS(this->ablend, Media::ABlend::AlphaBlend8_8());
	this->watermark = 0;
}

Media::Batch::BatchWatermarker::~BatchWatermarker()
{
	DEL_CLASS(this->rnd);
	DEL_CLASS(this->ablend);
	SDEL_TEXT(this->watermark);
}
			
void Media::Batch::BatchWatermarker::SetWatermark(const UTF8Char *watermark)
{
	SDEL_TEXT(this->watermark);
	if (watermark)
	{
		this->watermark = Text::StrCopyNew(watermark);
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
	if (this->watermark == 0 || this->watermark[0] == 0)
	{
		this->hdlr->ImageOutput(imgList, fileId, subId);
		return;
	}
	OSInt i;
	OSInt j;
	Media::StaticImage *simg;
	Media::DrawImage *tmpImg = this->deng->CreateImage32(16, 16, Media::AT_NO_ALPHA);
	i = 0;
	j = imgList->GetCount();
	while (i < j)
	{
		Int32 xRand;
		Int32 yRand;
		Double fontSize;
		OSInt leng = Text::StrCharCnt(this->watermark);
		Double sz[2];
		Int32 iWidth;
		Int32 iHeight;
		Media::DrawImage *gimg2;
		Media::DrawBrush *b = tmpImg->NewBrushARGB(0xffffffff);
		Media::DrawFont *f;

		simg = (Media::StaticImage*)imgList->GetImage(i, 0);
		fontSize = simg->info->dispWidth / 12.0;

		while (true)
		{
			f = tmpImg->NewFontH(L"Arial", fontSize, Media::DrawEngine::DFS_NORMAL, 0);
			if (!tmpImg->GetTextSizeUTF8(f, this->watermark, leng, sz))
			{
				tmpImg->DelFont(f);
				break;
			}
			if (sz[0] <= simg->info->dispWidth && sz[1] <= simg->info->dispHeight)
			{
				xRand = Math::Double2Int32(simg->info->dispWidth - sz[0]);
				yRand = Math::Double2Int32(simg->info->dispHeight - sz[1]);
				iWidth = Math::Double2Int32(sz[0]);
				iHeight = Math::Double2Int32(sz[1]);
				gimg2 = this->deng->CreateImage32(iWidth, iHeight, Media::AT_NO_ALPHA);
				gimg2->DrawStringUTF8(0, 0, this->watermark, f, b);
				gimg2->SetAlphaType(Media::AT_ALPHA);
				Bool revOrder;
				UInt8 *bmpBits = gimg2->GetImgBits(&revOrder);
				ImageUtil_ColorReplace32A(bmpBits, iWidth, iHeight, (this->rnd->NextInt30() & 0xffffff) | 0x5f808080);
				if (revOrder)
				{
					this->ablend->Blend(simg->data + Math::Double2Int32(this->rnd->NextDouble() * yRand) * simg->info->storeWidth * 4 + Math::Double2Int32(this->rnd->NextDouble() * xRand) * 4, simg->info->storeWidth << 2, bmpBits + iWidth * 4 * (iHeight - 1), -iWidth * 4, iWidth, iHeight, Media::AT_ALPHA);
				}
				else
				{
					this->ablend->Blend(simg->data + Math::Double2Int32(this->rnd->NextDouble() * yRand) * simg->info->storeWidth * 4 + Math::Double2Int32(this->rnd->NextDouble() * xRand) * 4, simg->info->storeWidth << 2, bmpBits, iWidth * 4, iWidth, iHeight, Media::AT_ALPHA);
				}
				this->deng->DeleteImage(gimg2);
				tmpImg->DelFont(f);
				break;
			}
			else
			{
				tmpImg->DelFont(f);
				fontSize--;
			}
		}
		tmpImg->DelBrush(b);
		i++;
	}
	this->deng->DeleteImage(tmpImg);
	this->hdlr->ImageOutput(imgList, fileId, subId);
}
