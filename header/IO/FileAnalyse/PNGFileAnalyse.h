#ifndef _SM_IO_FILEANALYSE_PNGFILEANALYSE
#define _SM_IO_FILEANALYSE_PNGFILEANALYSE
#include "Data/ArrayList.h"
#include "IO/IStreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Sync/Mutex.h"
#include "Text/StringBuilderUTF.h"

namespace IO
{
	namespace FileAnalyse
	{
		class PNGFileAnalyse : public IO::FileAnalyse::IFileAnalyse
		{
		private:
			typedef struct
			{
				Int32 tagType;
				Int32 crc;
				Int64 ofst;
				OSInt size;
			} PNGTag;
		private:
			IO::IStreamData *fd;
			Data::ArrayList<PNGTag*> *tags;
			Sync::Mutex *tagsMut;
			OSInt hdrSize;

			Bool pauseParsing;
			Bool threadRunning;
			Bool threadToStop;
			Bool threadStarted;

			static UInt32 __stdcall ParseThread(void *userObj);
		public:
			PNGFileAnalyse(IO::IStreamData *fd);
			virtual ~PNGFileAnalyse();

			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, Text::StringBuilderUTF *sb);
			virtual Bool GetFrameDetail(UOSInt index, Text::StringBuilderUTF *sb);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(const UTF8Char *outputFile);
		};
	}
}
#endif
