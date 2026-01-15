#ifndef _SM_MEDIA_BATCH_BATCHLOADER
#define _SM_MEDIA_BATCH_BATCHLOADER
#include "AnyType.h"
#include "Data/SyncCircularBuffNN.hpp"
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
				NN<IO::StreamData> data;
				NN<Text::String> fileId;
			} DataInfo;

			typedef struct
			{
				Bool running;
				Bool toStop;
				Bool processing;
				NN<Sync::Event> evt;
				NN<BatchLoader> me;
			} ThreadState;
		private:
			NN<Parser::ParserList> parsers;
			NN<Media::Batch::BatchHandler> hdlr;
			Sync::Event mainEvt;
			Sync::Mutex ioMut;
			Sync::Mutex reqMut;
			Data::SyncCircularBuffNN<Text::String> fileNames;
			Data::SyncCircularBuffNN<DataInfo> datas;
			UIntOS threadCnt;
			UnsafeArray<ThreadState> threadStates;
			UIntOS nextThread;

			static UInt32 __stdcall ThreadProc(AnyType userObj);
		public:
			BatchLoader(NN<Parser::ParserList> parsers, NN<Media::Batch::BatchHandler> hdlr);
			~BatchLoader();
			
			void AddFileName(Text::CStringNN fileName);
			void AddImageData(NN<IO::StreamData> data, Text::CStringNN fileId);
			Bool IsProcessing();
		};
	}
}
#endif
