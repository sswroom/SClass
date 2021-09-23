#ifndef _SM_IO_FILEANALYSE_EBMLFILEANALYSE
#define _SM_IO_FILEANALYSE_EBMLFILEANALYSE
#include "Data/SyncArrayList.h"
#include "IO/IStreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Sync/Mutex.h"
#include "Text/StringBuilderUTF.h"

namespace IO
{
	namespace FileAnalyse
	{
		class EBMLFileAnalyse : public IO::FileAnalyse::IFileAnalyse
		{
		private:
			typedef struct
			{
				UOSInt lev;
				UInt64 fileOfst;
				UOSInt packSize;
				UOSInt hdrSize;
				UInt8 packType[4];
			} PackInfo;

			typedef enum
			{
				ET_UNKNOWN,
				ET_MASTER,
				ET_SINT,
				ET_UINT,
				ET_FLOAT,
				ET_STRING,
				ET_UTF8,
				ET_DATE,
				ET_BINARY
			} ElementType;

			typedef struct
			{
				UInt32 elementId;
				ElementType type;
				const Char *elementName;
			} ElementInfo;
		private:
			IO::IStreamData *fd;
			Data::SyncArrayList<PackInfo*> *packs;

			Bool pauseParsing;
			Bool threadRunning;
			Bool threadToStop;
			Bool threadStarted;
			UOSInt maxLev;

			static ElementInfo elements[];

			static const UInt8 *ReadInt(const UInt8 *buffPtr, UInt64 *val, UInt32 *intSize);
			static const ElementInfo *GetElementInfo(UInt32 elementId);
			void ParseRange(UOSInt lev, UInt64 ofst, UInt64 size);
			static UInt32 __stdcall ParseThread(void *userObj);
			UOSInt GetFrameIndex(UOSInt lev, UInt64 ofst);
		public:
			EBMLFileAnalyse(IO::IStreamData *fd);
			virtual ~EBMLFileAnalyse();

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
