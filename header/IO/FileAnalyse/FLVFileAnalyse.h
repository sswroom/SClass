#ifndef _SM_IO_FILEANALYSE_FLVFILEANALYSE
#define _SM_IO_FILEANALYSE_FLVFILEANALYSE
#include "Data/SyncArrayListNN.h"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Sync/Mutex.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class FLVFileAnalyse : public IO::FileAnalyse::IFileAnalyse
		{
		private:
			typedef struct
			{
				UInt8 tagType;
				UInt64 ofst;
				UOSInt size;
			} FLVTag;
		private:
			IO::StreamData *fd;
			Data::SyncArrayListNN<FLVTag> tags;
			UOSInt hdrSize;

			Bool pauseParsing;
			Sync::Thread thread;

			UOSInt ParseScriptDataVal(UnsafeArray<UInt8> data, UOSInt ofst, UOSInt endOfst, NN<Text::StringBuilderUTF8> sb);
			void ParseScriptData(UnsafeArray<UInt8> data, UOSInt ofst, UOSInt endOfst, UOSInt frameOfst, NN<IO::FileAnalyse::FrameDetailHandler> frame);
			static void __stdcall ParseThread(NN<Sync::Thread> thread);
		public:
			FLVFileAnalyse(NN<IO::StreamData> fd);
			virtual ~FLVFileAnalyse();

			virtual Text::CStringNN GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, NN<Text::StringBuilderUTF8> sb);
			virtual Bool GetFrameDetail(UOSInt index, NN<Text::StringBuilderUTF8> sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual Optional<FrameDetail> GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CStringNN outputFile);
		};
	}
}
#endif
