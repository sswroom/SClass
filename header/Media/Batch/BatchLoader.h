#ifndef _SM_MEDIA_BATCH_BATCHLOADER
#define _SM_MEDIA_BATCH_BATCHLOADER
#include "AnyType.h"
#include "Data/SyncCircularBuff.h"
#include "Media/Batch/BatchHandler.h"
#include "Parser/ParserList.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

namespace Media
{
	namespace Batch
	{
		class BatchLoader
		{
		private:
			typedef struct
			{
				NotNullPtr<IO::StreamData> data;
				NotNullPtr<Text::String> fileId;
			} DataInfo;

			typedef struct
			{
				Bool running;
				Bool toStop;
				Bool processing;
				Sync::Event *evt;
				BatchLoader *me;
			} ThreadState;
		private:
			NotNullPtr<Parser::ParserList> parsers;
			Media::Batch::BatchHandler *hdlr;
			Sync::Event mainEvt;
			Sync::Mutex ioMut;
			Sync::Mutex reqMut;
			Data::SyncCircularBuff<Text::String*> fileNames;
			Data::SyncCircularBuff<DataInfo*> datas;
			UOSInt threadCnt;
			ThreadState *threadStates;
			UOSInt nextThread;

			static UInt32 __stdcall ThreadProc(AnyType userObj);
		public:
			BatchLoader(NotNullPtr<Parser::ParserList> parsers, Media::Batch::BatchHandler *hdlr);
			~BatchLoader();
			
			void AddFileName(Text::CString fileName);
			void AddImageData(NotNullPtr<IO::StreamData> data, Text::CStringNN fileId);
			Bool IsProcessing();
		};
	}
}
#endif
