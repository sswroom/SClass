#ifndef _SM_IO_FILEANALYSE_FLVFILEANALYSE
#define _SM_IO_FILEANALYSE_FLVFILEANALYSE
#include "Data/SyncArrayList.h"
#include "IO/IStreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Sync/Mutex.h"
#include "Text/StringBuilderUTF.h"

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
			IO::IStreamData *fd;
			Data::SyncArrayList<FLVTag*> *tags;
			UOSInt hdrSize;

			Bool pauseParsing;
			Bool threadRunning;
			Bool threadToStop;
			Bool threadStarted;

			UOSInt ParseScriptDataVal(UInt8 *data, UOSInt ofst, UOSInt endOfst, Text::StringBuilderUTF *sb);
			void ParseScriptData(UInt8 *data, UOSInt ofst, UOSInt endOfst, UOSInt frameOfst, IO::FileAnalyse::FrameDetailHandler *frame);
			static UInt32 __stdcall ParseThread(void *userObj);
		public:
			FLVFileAnalyse(IO::IStreamData *fd);
			virtual ~FLVFileAnalyse();

			virtual const UTF8Char *GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, Text::StringBuilderUTF *sb);
			virtual Bool GetFrameDetail(UOSInt index, Text::StringBuilderUTF *sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual FrameDetail *GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(const UTF8Char *outputFile);
		};
	}
}
#endif
