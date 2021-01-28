#ifndef _SM_MEDIA_IIMGROTATE
#define _SM_MEDIA_IIMGROTATE
namespace Media
{
	class IImgRotate
	{
	public:
		virtual Media::Image *Rotate(Media::Image *srcImg, Single centerX, Single centerY, Single angleRad, Bool keepCoord, Bool keepSize) = 0;
	};
};
#endif
