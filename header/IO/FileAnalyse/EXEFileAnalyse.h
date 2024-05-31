#ifndef _SM_IO_FILEANALYSE_EXEFILEANALYSE
#define _SM_IO_FILEANALYSE_EXEFILEANALYSE
#include "Data/ByteBuffer.h"
#include "Data/SyncArrayListNN.h"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Sync/Mutex.h"
#include "Sync/Thread.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class EXEFileAnalyse : public IO::FileAnalyse::IFileAnalyse
		{
		private:
			typedef struct
			{
				UInt64 fileOfst;
				UInt64 packSize;
				Int32 packType;
			} PackInfo;
		private:
			IO::StreamData *fd;
			Data::SyncArrayListNN<PackInfo> packs;
			Data::ByteBuffer imageBuff;
			UOSInt imageSize;

			Bool pauseParsing;
			Sync::Thread thread;
\
			static void __stdcall ParseThread(NN<Sync::Thread> thread);
		public:
			EXEFileAnalyse(NN<IO::StreamData> fd);
			virtual ~EXEFileAnalyse();

			virtual Text::CStringNN GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, NN<Text::StringBuilderUTF8> sb);
			virtual Bool GetFrameDetail(UOSInt index, NN<Text::StringBuilderUTF8> sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual Optional<FrameDetail> GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CStringNN outputFile);

			static Text::CStringNN PackTypeGetName(Int32 packType);
		};
	}
}
#endif
