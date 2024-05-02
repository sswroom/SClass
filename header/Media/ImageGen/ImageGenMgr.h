#ifndef _SM_MEDIA_IMAGEGEN_IMAGEGENMGR
#define _SM_MEDIA_IMAGEGEN_IMAGEGENMGR
#include "Data/ArrayListNN.h"
#include "Media/ImageGenerator.h"

namespace Media
{
	namespace ImageGen
	{
		class ImageGenMgr : public Data::ReadingListNN<Media::ImageGenerator>
		{
		private:
			Data::ArrayListNN<Media::ImageGenerator> imgGenArr;
		public:
			ImageGenMgr();
			virtual ~ImageGenMgr();

			virtual UOSInt GetCount() const;
			virtual NN<Media::ImageGenerator> GetItemNoCheck(UOSInt index) const;
			virtual Optional<Media::ImageGenerator> GetItem(UOSInt index) const;
		};
	}
}
#endif
