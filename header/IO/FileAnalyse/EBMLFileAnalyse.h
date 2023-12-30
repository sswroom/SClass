#ifndef _SM_IO_FILEANALYSE_EBMLFILEANALYSE
#define _SM_IO_FILEANALYSE_EBMLFILEANALYSE
#include "Data/SyncArrayList.h"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Sync/Mutex.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

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
				Text::CStringNN elementName;
			} ElementInfo;
		private:
			IO::StreamData *fd;
			Data::SyncArrayList<PackInfo*> packs;

			Bool pauseParsing;
			Sync::Thread thread;
			UOSInt maxLev;

			static ElementInfo elements[];

			static const UInt8 *ReadInt(const UInt8 *buffPtr, UInt64 *val, UInt32 *intSize);
			static const ElementInfo *GetElementInfo(UInt32 elementId);
			void ParseRange(UOSInt lev, UInt64 ofst, UInt64 size);
			static void __stdcall ParseThread(NotNullPtr<Sync::Thread> thread);
			UOSInt GetFrameIndex(UOSInt lev, UInt64 ofst);
		public:
			EBMLFileAnalyse(NotNullPtr<IO::StreamData> fd);
			virtual ~EBMLFileAnalyse();

			virtual Text::CStringNN GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb);
			virtual Bool GetFrameDetail(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual Optional<FrameDetail> GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CStringNN outputFile);
		};
	}
}
#endif
