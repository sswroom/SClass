#ifndef _SM_IO_FILEANALYSE_TIFFFILEANALYSE
#define _SM_IO_FILEANALYSE_TIFFFILEANALYSE
#include "Data/ByteOrder.h"
#include "Data/SyncArrayList.h"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class TIFFFileAnalyse : public IO::FileAnalyse::IFileAnalyse
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
				UInt64 fileOfst;
				UOSInt packSize;
				PackType packType;
				Text::String *fileName;
			} PackInfo;
		private:
			IO::StreamData *fd;
			Data::SyncArrayList<PackInfo*> packs;
			Data::ByteOrder *bo;

			Bool pauseParsing;
			Bool threadRunning;
			Bool threadToStop;
			Bool threadStarted;

			void ParseV1Directory(UInt64 dirOfst, UInt64 dirSize);
			void ParseV2Directory(UInt64 dirOfst, UInt64 dirSize);
			static UInt32 __stdcall ParseThread(void *userObj);
			static void FreePackInfo(PackInfo *pack);
		public:
			TIFFFileAnalyse(IO::StreamData *fd);
			virtual ~TIFFFileAnalyse();

			virtual Text::CString GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, Text::StringBuilderUTF8 *sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual FrameDetail *GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CString outputFile);
		};
	}
}
#endif
