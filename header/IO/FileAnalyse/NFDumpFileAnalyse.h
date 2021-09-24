#ifndef _SM_IO_FILEANALYSE_NFDUMPFILEANALYSE
#define _SM_IO_FILEANALYSE_NFDUMPFILEANALYSE
#include "Data/SyncArrayList.h"
#include "Data/Int32Map.h"
#include "IO/IStreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Text/StringBuilderUTF.h"

namespace IO
{
	namespace FileAnalyse
	{
		class NFDumpFileAnalyse : public IO::FileAnalyse::IFileAnalyse
		{
		private:
			typedef struct
			{
				UInt64 fileOfst;
				UOSInt packSize;
				Int32 packType;
			} PackInfo;
		private:
			IO::IStreamData *fd;
			Data::SyncArrayList<PackInfo*> *packs;
			Data::Int32Map<UInt8*> *extMap;
			Bool hasLZODecomp;

			Bool pauseParsing;
			Bool threadRunning;
			Bool threadToStop;
			Bool threadStarted;
			static UInt32 __stdcall ParseThread(void *userObj);
			UOSInt LZODecompBlock(UInt8 *srcBlock, UOSInt srcSize, UInt8 *outBlock, UOSInt maxOutSize);
		public:
			NFDumpFileAnalyse(IO::IStreamData *fd);
			virtual ~NFDumpFileAnalyse();

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
