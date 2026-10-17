#ifndef _SM_IO_FILEANALYSE_CFBFILEANALYSE
#define _SM_IO_FILEANALYSE_CFBFILEANALYSE
#include "Data/SyncArrayListNN.hpp"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/FileAnalyser.h"
#include "Sync/Thread.h"

namespace IO
{
	namespace FileAnalyse
	{
		class CFBFileAnalyse : public IO::FileAnalyse::FileAnalyser
		{
		private:
			struct FrameInfo
			{
				UInt64 ofst;
				UIntOS size;
				UInt32 sectorId;
				UInt8 type;
			};

			Optional<IO::StreamData> fd;
			Data::SyncArrayListNN<FrameInfo> frames;
			UIntOS sectorSize;
			UInt32 sectorCount;

			Bool pauseParsing;
			Sync::Thread thread;

			static Text::CString SIDName(UInt32 sid);
			static Text::CString DirTypeName(UInt8 type);
			static Text::CString ColorName(UInt8 color);

			static void __stdcall ParseThread(NN<Sync::Thread> thread);
		public:
			CFBFileAnalyse(NN<IO::StreamData> fd);
			virtual ~CFBFileAnalyse();

			virtual Text::CStringNN GetFormatName();
			virtual UIntOS GetFrameCount();
			virtual Bool GetFrameName(UIntOS index, NN<Text::StringBuilderUTF8> sb);
			virtual UIntOS GetFrameIndex(UInt64 ofst);
			virtual Optional<FrameDetail> GetFrameDetail(UIntOS index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CStringNN outputFile);
		};
	}
}
#endif
