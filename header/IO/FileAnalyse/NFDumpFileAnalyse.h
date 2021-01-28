#ifndef _SM_IO_FILEANALYSE_NFDUMPFILEANALYSE
#define _SM_IO_FILEANALYSE_NFDUMPFILEANALYSE
#include "Data/ArrayList.h"
#include "Data/Integer32Map.h"
#include "IO/IStreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Sync/Mutex.h"
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
				Int64 fileOfst;
				OSInt packSize;
				Int32 packType;
			} PackInfo;
		private:
			IO::IStreamData *fd;
			Data::ArrayList<PackInfo*> *packs;
			Data::Integer32Map<UInt8*> *extMap;
			Sync::Mutex *packMut;
			Bool hasLZODecomp;

			Bool pauseParsing;
			Bool threadRunning;
			Bool threadToStop;
			Bool threadStarted;
			static UInt32 __stdcall ParseThread(void *userObj);
			OSInt LZODecompBlock(UInt8 *srcBlock, OSInt srcSize, UInt8 *outBlock, OSInt maxOutSize);
		public:
			NFDumpFileAnalyse(IO::IStreamData *fd);
			virtual ~NFDumpFileAnalyse();

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
