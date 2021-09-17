#ifndef _SM_IO_FILEANALYSE_PCAPFILEANALYSE
#define _SM_IO_FILEANALYSE_PCAPFILEANALYSE
#include "Data/ArrayList.h"
#include "IO/IStreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Sync/Mutex.h"
#include "Text/StringBuilderUTF.h"

namespace IO
{
	namespace FileAnalyse
	{
		class PCapFileAnalyse : public IO::FileAnalyse::IFileAnalyse
		{
		private:
			IO::IStreamData *fd;
			Bool isBE;
			UInt32 linkType;
			Data::ArrayList<UInt64> *ofstList;
			Data::ArrayList<UInt64> *sizeList;
			Sync::Mutex *dataMut;
			UInt8 *packetBuff;

			Bool pauseParsing;
			Bool threadRunning;
			Bool threadToStop;
			Bool threadStarted;

			static UInt32 __stdcall ParseThread(void *userObj);
		public:
			PCapFileAnalyse(IO::IStreamData *fd);
			virtual ~PCapFileAnalyse();

			virtual const UTF8Char *GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, Text::StringBuilderUTF *sb);
			virtual Bool GetFrameDetail(UOSInt index, Text::StringBuilderUTF *sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(const UTF8Char *outputFile);
		};
	}
}
#endif
