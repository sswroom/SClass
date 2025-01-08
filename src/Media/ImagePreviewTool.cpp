#include "Stdafx.h"
#include "Media/ImagePreviewTool.h"
#include "Media/Resizer/LanczosResizer8_C8.h"

Bool Media::ImagePreviewTool::CreatePreviews(NN<Media::ImageList> imgList, NN<Data::ArrayListNN<Media::StaticImage>> prevImgs, UOSInt maxSize)
{
	NN<Media::StaticImage> img;
	imgList->ToStaticImage(0);
	if (Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(0, 0)).SetTo(img) && (img->info.dispSize.x >= maxSize || img->info.dispSize.y >= maxSize))
	{
		UOSInt currWidth = img->info.dispSize.x;
		UOSInt currHeight = img->info.dispSize.y;
		NN<Media::StaticImage> simg;
		Media::Resizer::LanczosResizer8_C8 resizer(3, 3, img->info.color, img->info.color, 0, img->info.atype);
		img->To32bpp();

		while (currWidth >= maxSize || currHeight >= maxSize)
		{
			currWidth >>= 1;
			currHeight >>= 1;
			resizer.SetTargetSize(Math::Size2D<UOSInt>(currWidth, currHeight));
			if (simg.Set(resizer.ProcessToNew(img)))
			{
				prevImgs->Add(simg);
			}
		}
		return true;
	}
	return false;
}
