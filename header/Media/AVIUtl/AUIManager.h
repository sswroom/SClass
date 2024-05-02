#ifndef _SM_MEDIA_AVIUTL_AUIMANAGER
#define _SM_MEDIA_AVIUTL_AUIMANAGER
#include "Data/ArrayListNN.h"
#include "IO/FileSelector.h"
#include "Media/AVIUtl/AUIPlugin.h"
#include "Media/IMediaSource.h"
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
			UOSInt LoadFile(const UTF8Char *fileName, NN<Data::ArrayListNN<Media::IMediaSource>> outArr);
			void PrepareSelector(NN<IO::FileSelector> selector);
		};
	}
}
#endif
