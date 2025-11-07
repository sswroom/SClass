#ifndef _SM_IO_FILEANALYSE_DBF3FILEANALYSE
#define _SM_IO_FILEANALYSE_DBF3FILEANALYSE
#include "Data/ByteBuffer.h"
#include "Data/SyncArrayListNN.h"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/FileAnalyser.h"
#include "Sync/Mutex.h"
#include "Sync/Thread.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class DBF3FileAnalyse : public IO::FileAnalyse::FileAnalyser
		{
		private:
			enum class PackType
			{
				TableHeader,
				FieldSubrecords,
				Terminator,
				TableRecord
			};

			typedef struct
			{
				UInt64 fileOfst;
				UInt64 packSize;
				PackType packType;
			} PackInfo;

			struct DBFCol
			{
				NN<Text::String> name;
				UTF8Char type;
				UOSInt colOfst;
				UOSInt colSize;
				UOSInt colDP;
			};
		private:
			Optional<IO::StreamData> fd;
			Data::SyncArrayListNN<PackInfo> packs;
			Data::ArrayListNN<DBFCol> cols;

			Bool pauseParsing;
			Sync::Thread thread;
\
			static void __stdcall ParseThread(NN<Sync::Thread> thread);
			static void __stdcall FreeCol(NN<DBFCol> col);
		public:
			DBF3FileAnalyse(NN<IO::StreamData> fd);
			virtual ~DBF3FileAnalyse();

			virtual Text::CStringNN GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, NN<Text::StringBuilderUTF8> sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual Optional<FrameDetail> GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CStringNN outputFile);

			static Text::CStringNN PackTypeGetName(PackType packType);
		};
	}
}
#endif
