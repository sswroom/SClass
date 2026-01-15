#ifndef _SM_MEDIA_IMAGEGEN_IMAGEGENMGR
#define _SM_MEDIA_IMAGEGEN_IMAGEGENMGR
#include "Data/ArrayListNN.hpp"
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

			virtual UIntOS GetCount() const;
			virtual NN<Media::ImageGenerator> GetItemNoCheck(UIntOS index) const;
			virtual Optional<Media::ImageGenerator> GetItem(UIntOS index) const;
		};
	}
}
#endif
