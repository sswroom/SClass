#ifndef _SM_IO_FILEANALYSE_SMTCFILEANALYSE
#define _SM_IO_FILEANALYSE_SMTCFILEANALYSE
#include "Data/ArrayList.h"
#include "Data/ByteBuffer.h"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Sync/Mutex.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class SMTCFileAnalyse : public IO::FileAnalyse::IFileAnalyse
		{
		private:
			struct DataInfo
			{
				UInt64 ofst;
				UOSInt size;
				UInt8 type;
			};
			
		private:
			IO::StreamData *fd;
			Data::ArrayList<DataInfo*> dataList;
			Sync::Mutex dataMut;
			Data::ByteBuffer packetBuff;

			Bool pauseParsing;
			Bool threadRunning;
			Bool threadToStop;
			Bool threadStarted;

			static UInt32 __stdcall ParseThread(void *userObj);
		public:
			SMTCFileAnalyse(NotNullPtr<IO::StreamData> fd);
			virtual ~SMTCFileAnalyse();

			virtual Text::CString GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual FrameDetail *GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CString outputFile);
		};
	}
}
#endif
