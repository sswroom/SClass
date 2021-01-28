#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/ImageCmp/ImageMeanSqrErr.h"

Media::ImageCmp::ImageMeanSqrErr::ImageMeanSqrErr()
{
}

Media::ImageCmp::ImageMeanSqrErr::~ImageMeanSqrErr()
{
}

Double Media::ImageCmp::ImageMeanSqrErr::CompareImage(Media::Image *oriImage, Media::Image *cmpImage)
{
	if (oriImage->info->fourcc != cmpImage->info->fourcc)
		return -1;
	if (oriImage->info->bpp != cmpImage->info->bpp)
		return -1;
	if (oriImage->info->width != cmpImage->info->width || oriImage->info->height != cmpImage->info->height)
		return -1;
	if (oriImage->info->fourcc == 0 || oriImage->info->fourcc == *(Int32*)"DIB")
	{
		Int64 diffSum = 0;
		if (oriImage->info->bpp == 32)
		{
			Int32 w = oriImage->info->width;
			Int32 bpl = w << 2;
			UInt8 *srcImg;
			UInt8 *destImg;
			Int32 i = oriImage->info->height;
			srcImg = MemAlloc(UInt8, bpl);
			destImg = MemAlloc(UInt8, bpl);
			while (i-- > 0)
			{
				oriImage->GetImageData(srcImg, 0, i, w, 1, bpl);
				cmpImage->GetImageData(destImg, 0, i, w, 1, bpl);
				_asm
				{
					mov esi,srcImg
					mov edi,destImg
					mov ecx,w
cilop:
					movzx eax,byte ptr [esi]
					movzx edx,byte ptr [edi]
					sub eax,edx
					imul eax
					mov ebx,eax

					movzx eax,byte ptr [esi+1]
					movzx edx,byte ptr [edi+1]
					sub eax,edx
					imul eax
					add ebx,eax

					movzx eax,byte ptr [esi+2]
					movzx edx,byte ptr [edi+2]
					sub eax,edx
					imul eax
					add ebx,eax

					add dword ptr diffSum, ebx
					adc dword ptr diffSum[4], 0
					add esi,4
					add edi,4
					dec ecx
					jnz cilop
				}
			}
			MemFree(srcImg);
			MemFree(destImg);
			return diffSum / (Double)(w * oriImage->info->height * 3);
		}
		return -1;
	}
	else
	{
		return -1;
	}
}
