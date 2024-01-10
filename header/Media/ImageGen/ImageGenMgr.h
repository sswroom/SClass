#ifndef _SM_MEDIA_IMAGEGEN_IMAGEGENMGR
#define _SM_MEDIA_IMAGEGEN_IMAGEGENMGR
#include "Media/ImageGenerator.h"
#include "Data/ArrayList.h"

namespace Media
{
	namespace ImageGen
	{
		class ImageGenMgr
		{
		private:
			Data::ArrayList<Media::ImageGenerator*> *imgGenArr;
		public:
			ImageGenMgr();
			~ImageGenMgr();

			UOSInt GetCount();
			Media::ImageGenerator *GetGenerator(UOSInt index);
		};
	}
}
#endif
