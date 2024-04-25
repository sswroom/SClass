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
			NN<Media::ImageList> imgList;
			Data::ArrayListNN<Media::StaticImage> *prevList;
			Sync::Mutex mut;
			Int32 useCnt;

			UInt32 imgDelay;
			UOSInt imgIndex;
			Int64 lastTimeTick;
		};
	private:
		NN<ImageStatus> imgStatus;

		SharedImage(NN<ImageStatus> status);
	public:
		SharedImage(NN<Media::ImageList> imgList, Bool genPreview);
		~SharedImage();

		NN<SharedImage> Clone() const;
		Media::StaticImage *GetImage(OptOut<UInt32> imgTimeMS) const;
		Optional<Media::StaticImage> GetPrevImage(Double width, Double height, OptOut<UInt32> imgTimeMS) const;
	};
}
#endif
