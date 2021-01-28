#ifndef _SM_MEDIA_SHAREDIMAGE
#define _SM_MEDIA_SHAREDIMAGE
#include "Media/ImageList.h"
#include "Sync/Mutex.h"

namespace Media
{
	class SharedImage
	{
	private:
		typedef struct
		{
			Media::ImageList *imgList;
			Data::ArrayList<Media::StaticImage*> *prevList;
			Sync::Mutex *mut;
			Int32 useCnt;

			Int32 imgDelay;
			UOSInt imgIndex;
			Int64 lastTimeTick;
		} ImageStatus;
	private:
		ImageStatus *imgStatus;

		SharedImage(ImageStatus *status);
	public:
		SharedImage(Media::ImageList *imgList, Bool genPreview);
		~SharedImage();

		SharedImage *Clone();
		Media::StaticImage *GetImage(Int32 *imgTimeMS);
		Media::StaticImage *GetPrevImage(Double width, Double height, Int32 *imgTimeMS);
	};
}
#endif
