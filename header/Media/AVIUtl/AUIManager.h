#ifndef _SM_MEDIA_AVIUTL_AUIMANAGER
#define _SM_MEDIA_AVIUTL_AUIMANAGER
#include "Data/ArrayList.h"
#include "IO/IFileSelector.h"
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
			Data::ArrayList<Media::AVIUtl::AUIPlugin *> *plugins;

		public:
			AUIManager();
			~AUIManager();
			UOSInt LoadFile(const UTF8Char *fileName, Data::ArrayList<Media::IMediaSource*> *outArr);
			void PrepareSelector(IO::IFileSelector *selector);
		};
	}
}
#endif
