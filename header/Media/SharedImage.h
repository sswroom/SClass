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
			NotNullPtr<Media::ImageList> imgList;
			Data::ArrayListNN<Media::StaticImage> *prevList;
			Sync::Mutex mut;
			Int32 useCnt;

			UInt32 imgDelay;
			UOSInt imgIndex;
			Int64 lastTimeTick;
		};
	private:
		NotNullPtr<ImageStatus> imgStatus;

		SharedImage(NotNullPtr<ImageStatus> status);
	public:
		SharedImage(NotNullPtr<Media::ImageList> imgList, Bool genPreview);
		~SharedImage();

		NotNullPtr<SharedImage> Clone() const;
		Media::StaticImage *GetImage(OptOut<UInt32> imgTimeMS) const;
		Optional<Media::StaticImage> GetPrevImage(Double width, Double height, OptOut<UInt32> imgTimeMS) const;
	};
}
#endif
