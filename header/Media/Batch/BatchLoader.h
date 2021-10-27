#ifndef _SM_MEDIA_BATCH_BATCHLOADER
#define _SM_MEDIA_BATCH_BATCHLOADER
#include "Data/SyncLinkedList.h"
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
				IO::IStreamData *data;
				const UTF8Char *fileId;
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
			Parser::ParserList *parsers;
			Media::Batch::BatchHandler *hdlr;
			Sync::Event *mainEvt;
			Sync::Mutex *ioMut;
			Sync::Mutex *reqMut;
			Data::SyncLinkedList *fileNames;
			Data::SyncLinkedList *datas;
			UOSInt threadCnt;
			ThreadState *threadStates;
			UOSInt nextThread;

			static UInt32 __stdcall ThreadProc(void *userObj);
		public:
			BatchLoader(Parser::ParserList *parsers, Media::Batch::BatchHandler *hdlr);
			~BatchLoader();
			
			void AddFileName(const UTF8Char *fileName);
			void AddImageData(IO::IStreamData *data, const UTF8Char *fileId);
			Bool IsProcessing();
		};
	};
};
#endif
