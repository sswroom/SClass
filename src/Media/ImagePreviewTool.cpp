#include "Stdafx.h"
#include "Media/ImagePreviewTool.h"
#include "Media/Resizer/LanczosResizerRGB_C8.h"
#include "Media/Resizer/LanczosResizerW8_8.h"

Bool Media::ImagePreviewTool::CreatePreviews(NN<Media::ImageList> imgList, NN<Data::ArrayListNN<Media::StaticImage>> prevImgs, UOSInt maxSize)
{
	NN<Media::StaticImage> img;
	imgList->ToStaticImage(0);
	if (Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(0, 0)).SetTo(img) && (img->info.dispSize.x >= maxSize || img->info.dispSize.y >= maxSize))
	{
		UOSInt currWidth = img->info.dispSize.x;
		UOSInt currHeight = img->info.dispSize.y;
		NN<Media::StaticImage> simg;
		Media::PixelFormat pf = img->info.pf;
		if (pf == Media::PF_PAL_W8)
		{
			Media::Resizer::LanczosResizerW8_8 resizer(3, 3, img->info.color, img->info.color, nullptr);
			while (currWidth >= maxSize || currHeight >= maxSize)
			{
				currWidth >>= 1;
				currHeight >>= 1;
				resizer.SetTargetSize(Math::Size2D<UOSInt>(currWidth, currHeight));
				img->info.pf;
				if (resizer.ProcessToNew(img).SetTo(simg))
				{
					prevImgs->Add(simg);
				}
			}
		}
		else
		{
			if (pf == Media::PF_PAL_1)
			{
				img->ToPal8();
				pf = img->info.pf;
			}
			Media::Resizer::LanczosResizerRGB_C8 resizer(3, 3, img->info.color, img->info.color, nullptr, img->info.atype);
			if (resizer.IsSupported(img->info))
			{
				if (pf == Media::PF_B8G8R8)
				{
					resizer.SetSrcPixelFormat(pf, img->pal);
					resizer.SetDestPixelFormat(pf);
				}
				else
				{
					resizer.SetSrcPixelFormat(pf, img->pal);
				}
			}
			else
			{
				printf("ImagePreviewTool: PixelFormat not supported: %s\r\n", Media::PixelFormatGetName(pf).v.Ptr());
				img->ToB8G8R8A8();
			}
	
			while (currWidth >= maxSize || currHeight >= maxSize)
			{
				currWidth >>= 1;
				currHeight >>= 1;
				resizer.SetTargetSize(Math::Size2D<UOSInt>(currWidth, currHeight));
				img->info.pf;
				if (resizer.ProcessToNew(img).SetTo(simg))
				{
					prevImgs->Add(simg);
				}
			}
		}
		return true;
	}
	return false;
}
