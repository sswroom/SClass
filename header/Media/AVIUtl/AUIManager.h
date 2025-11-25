#ifndef _SM_MEDIA_AVIUTL_AUIMANAGER
#define _SM_MEDIA_AVIUTL_AUIMANAGER
#include "Data/ArrayListNN.hpp"
#include "IO/FileSelector.h"
#include "Media/AVIUtl/AUIPlugin.h"
#include "Media/MediaSource.h"
#include "Sync/Mutex.h"

namespace Media
{
	namespace AVIUtl
	{
		class AUIManager
		{
		private:
			Data::ArrayListNN<Media::AVIUtl::AUIPlugin> plugins;

		public:
			AUIManager();
			~AUIManager();
			UOSInt LoadFile(UnsafeArray<const UTF8Char> fileName, NN<Data::ArrayListNN<Media::MediaSource>> outArr);
			void PrepareSelector(NN<IO::FileSelector> selector);
		};
	}
}
#endif
