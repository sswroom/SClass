#ifndef _SM_IO_FILEANALYSE_DWGFILEANALYSE
#define _SM_IO_FILEANALYSE_DWGFILEANALYSE
#include "Data/SyncArrayListNN.hpp"
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
		class DWGFileAnalyse : public IO::FileAnalyse::FileAnalyser
		{
		public:
			enum class PackType
			{
				Unknown,
				FileHeaderV1,
				PreviewImage,
				HeaderVariables,
				ClassSection,
				ObjectMap,
				UnknownTable,
				Measurement,
				R2004FileHeader
			};

			typedef struct
			{
				UInt64 fileOfst;
				UInt64 packSize;
				PackType packType;
			} PackInfo;
		private:
			IO::StreamData *fd;
			Data::SyncArrayListNN<PackInfo> packs;

			UInt32 fileVer;

			Bool pauseParsing;
			Sync::Thread thread;

			static void __stdcall ParseThread(NN<Sync::Thread> thread);
		public:
			DWGFileAnalyse(NN<IO::StreamData> fd);
			virtual ~DWGFileAnalyse();

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
