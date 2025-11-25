#ifndef _SM_IO_FILEANALYSE_RAR5FILEANALYSE
#define _SM_IO_FILEANALYSE_RAR5FILEANALYSE
#include "Data/SyncArrayListNN.hpp"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/FileAnalyser.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class RAR5FileAnalyse : public IO::FileAnalyse::FileAnalyser
		{
		private:
			typedef struct
			{
				UInt64 fileOfst;
				UInt32 headerSize;
				UInt32 headerType;
				UInt64 dataSize;
			} BlockInfo;
		private:
			IO::StreamData *fd;
			Data::SyncArrayListNN<BlockInfo> packs;

			Bool pauseParsing;
			Sync::Thread thread;

			static const UInt8 *ReadVInt(const UInt8 *buffPtr, UInt64 *val);
			static Data::ByteArray ReadVInt(Data::ByteArray buffPtr, UInt64 *val);
			static const UInt8 *AddVInt(NN<IO::FileAnalyse::FrameDetail> frame, UOSInt ofst, Text::CStringNN name, const UInt8 *buffPtr);
			static const UInt8 *AddVInt(NN<IO::FileAnalyse::FrameDetail> frame, UOSInt ofst, Text::CStringNN name, const UInt8 *buffPtr, OptOut<UInt64> val);
			static Data::ByteArray AddVInt(NN<IO::FileAnalyse::FrameDetail> frame, UOSInt ofst, Text::CStringNN name, Data::ByteArray buffPtr);
			static Data::ByteArray AddVInt(NN<IO::FileAnalyse::FrameDetail> frame, UOSInt ofst, Text::CStringNN name, Data::ByteArray buffPtr, OptOut<UInt64> val);
			static const UInt8 *AddVHex(NN<IO::FileAnalyse::FrameDetail> frame, UOSInt ofst, Text::CStringNN name, const UInt8 *buffPtr, OptOut<UInt64> val);
			static Data::ByteArray AddVHex(NN<IO::FileAnalyse::FrameDetail> frame, UOSInt ofst, Text::CStringNN name, Data::ByteArray buffPtr, OptOut<UInt64> val);
			static void __stdcall ParseThread(NN<Sync::Thread> thread);
		public:
			RAR5FileAnalyse(NN<IO::StreamData> fd);
			virtual ~RAR5FileAnalyse();

			virtual Text::CStringNN GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, NN<Text::StringBuilderUTF8> sb);
			virtual Bool GetFrameDetail(UOSInt index, NN<Text::StringBuilderUTF8> sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual Optional<FrameDetail> GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CStringNN outputFile);
		};
	}
}
#endif
