#ifndef _SM_MEDIA_BATCH_BATCHLOADER
#define _SM_MEDIA_BATCH_BATCHLOADER
#include "Media/Batch/BatchHandler.h"
#include "Sync/Mutex.h"
#include "Sync/Event.h"
#include "Parser/ParserList.h"
#include "Data/LinkedList.h"

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
			Data::LinkedList *fileNames;
			Data::LinkedList *datas;
			UOSInt threadCnt;
			ThreadState *threadStates;
			OSInt nextThread;

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
