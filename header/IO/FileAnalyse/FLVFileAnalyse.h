#ifndef _SM_IO_FILEANALYSE_FLVFILEANALYSE
#define _SM_IO_FILEANALYSE_FLVFILEANALYSE
#include "Data/SyncArrayList.h"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Sync/Mutex.h"
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
			Data::SyncArrayList<FLVTag*> tags;
			UOSInt hdrSize;

			Bool pauseParsing;
			Bool threadRunning;
			Bool threadToStop;
			Bool threadStarted;

			UOSInt ParseScriptDataVal(UInt8 *data, UOSInt ofst, UOSInt endOfst, NotNullPtr<Text::StringBuilderUTF8> sb);
			void ParseScriptData(UInt8 *data, UOSInt ofst, UOSInt endOfst, UOSInt frameOfst, IO::FileAnalyse::FrameDetailHandler *frame);
			static UInt32 __stdcall ParseThread(void *userObj);
		public:
			FLVFileAnalyse(NotNullPtr<IO::StreamData> fd);
			virtual ~FLVFileAnalyse();

			virtual Text::CString GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb);
			virtual Bool GetFrameDetail(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual FrameDetail *GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CString outputFile);
		};
	}
}
#endif
