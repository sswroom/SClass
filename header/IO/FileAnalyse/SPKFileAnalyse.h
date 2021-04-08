#ifndef _SM_IO_FILEANALYSE_SPKFILEANALYSE
#define _SM_IO_FILEANALYSE_SPKFILEANALYSE
#include "Data/SyncArrayList.h"
#include "Data/Int32Map.h"
#include "IO/IStreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Text/StringBuilderUTF.h"

namespace IO
{
	namespace FileAnalyse
	{
		class SPKFileAnalyse : public IO::FileAnalyse::IFileAnalyse
		{
		private:
			typedef enum
			{
				PT_HEADER,
				PT_V1DIRECTORY,
				PT_V2DIRECTORY,
				PT_FILE
			} PackType;
			typedef struct
			{
				Int64 fileOfst;
				OSInt packSize;
				PackType packType;
				const UTF8Char *fileName;
			} PackInfo;
		private:
			IO::IStreamData *fd;
			Data::SyncArrayList<PackInfo*> *packs;
			Data::Int32Map<UInt8*> *extMap;

			Bool pauseParsing;
			Bool threadRunning;
			Bool threadToStop;
			Bool threadStarted;

			void ParseV1Directory(Int64 dirOfst, Int64 dirSize);
			void ParseV2Directory(Int64 dirOfst, Int64 dirSize);
			static UInt32 __stdcall ParseThread(void *userObj);
			static void FreePackInfo(PackInfo *pack);
		public:
			SPKFileAnalyse(IO::IStreamData *fd);
			virtual ~SPKFileAnalyse();

			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, Text::StringBuilderUTF *sb);
			virtual Bool GetFrameDetail(UOSInt index, Text::StringBuilderUTF *sb);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(const UTF8Char *outputFile);

			void GetDetailDirs(const UInt8 *dirBuff, OSInt dirSize, Text::StringBuilderUTF *sb);
		};
	}
}
#endif
