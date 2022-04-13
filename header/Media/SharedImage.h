#ifndef _SM_MEDIA_SHAREDIMAGE
#define _SM_MEDIA_SHAREDIMAGE
#include "Media/ImageList.h"
#include "Sync/Mutex.h"

namespace Media
{
	class SharedImage
	{
	private:
		class ImageStatus
		{
		public:
			Media::ImageList *imgList;
			Data::ArrayList<Media::StaticImage*> *prevList;
			Sync::Mutex mut;
			Int32 useCnt;

			UInt32 imgDelay;
			UOSInt imgIndex;
			Int64 lastTimeTick;
		};
	private:
		ImageStatus *imgStatus;

		SharedImage(ImageStatus *status);
	public:
		SharedImage(Media::ImageList *imgList, Bool genPreview);
		~SharedImage();

		SharedImage *Clone();
		Media::StaticImage *GetImage(UInt32 *imgTimeMS);
		Media::StaticImage *GetPrevImage(Double width, Double height, UInt32 *imgTimeMS);
	};
}
#endif
