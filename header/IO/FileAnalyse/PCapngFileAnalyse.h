#ifndef _SM_IO_FILEANALYSE_PCAPNGFILEANALYSE
#define _SM_IO_FILEANALYSE_PCAPNGFILEANALYSE
#include "Data/ByteBuffer.h"
#include "Data/SyncArrayList.h"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class PCapngFileAnalyse : public IO::FileAnalyse::IFileAnalyse
		{
		private:
			typedef struct
			{
				UInt64 ofst;
				UInt32 blockType;
				UInt32 blockLength;
				UInt16 linkType;
				Int8 timeResol;
			} BlockInfo;
		private:
			IO::StreamData *fd;
			Bool isBE;
			Data::SyncArrayList<BlockInfo *> blockList;
			Data::ByteBuffer packetBuff;

			Bool pauseParsing;
			Bool threadRunning;
			Bool threadToStop;
			Bool threadStarted;

			static UInt32 __stdcall ParseThread(void *userObj);
		public:
			PCapngFileAnalyse(NotNullPtr<IO::StreamData> fd);
			virtual ~PCapngFileAnalyse();

			virtual Text::CString GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, Text::StringBuilderUTF8 *sb);
			virtual Bool GetFrameDetail(UOSInt index, Text::StringBuilderUTF8 *sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual FrameDetail *GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CString outputFile);

			static void SetTime(Data::DateTime *dt, Int64 ts, Int8 timeResol);
		};
	}
}
#endif
