#ifndef _SM_IO_FILEANALYSE_RAR5FILEANALYSE
#define _SM_IO_FILEANALYSE_RAR5FILEANALYSE
#include "Data/SyncArrayList.h"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class RAR5FileAnalyse : public IO::FileAnalyse::IFileAnalyse
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
			Data::SyncArrayList<BlockInfo*> packs;

			Bool pauseParsing;
			Sync::Thread thread;

			static const UInt8 *ReadVInt(const UInt8 *buffPtr, UInt64 *val);
			static Data::ByteArray ReadVInt(Data::ByteArray buffPtr, UInt64 *val);
			static const UInt8 *AddVInt(IO::FileAnalyse::FrameDetail *frame, UOSInt ofst, Text::CStringNN name, const UInt8 *buffPtr);
			static const UInt8 *AddVInt(IO::FileAnalyse::FrameDetail *frame, UOSInt ofst, Text::CStringNN name, const UInt8 *buffPtr, OptOut<UInt64> val);
			static Data::ByteArray AddVInt(IO::FileAnalyse::FrameDetail *frame, UOSInt ofst, Text::CStringNN name, Data::ByteArray buffPtr);
			static Data::ByteArray AddVInt(IO::FileAnalyse::FrameDetail *frame, UOSInt ofst, Text::CStringNN name, Data::ByteArray buffPtr, OptOut<UInt64> val);
			static const UInt8 *AddVHex(IO::FileAnalyse::FrameDetail *frame, UOSInt ofst, Text::CStringNN name, const UInt8 *buffPtr, OptOut<UInt64> val);
			static Data::ByteArray AddVHex(IO::FileAnalyse::FrameDetail *frame, UOSInt ofst, Text::CStringNN name, Data::ByteArray buffPtr, OptOut<UInt64> val);
			static void __stdcall ParseThread(NotNullPtr<Sync::Thread> thread);
		public:
			RAR5FileAnalyse(NotNullPtr<IO::StreamData> fd);
			virtual ~RAR5FileAnalyse();

			virtual Text::CStringNN GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb);
			virtual Bool GetFrameDetail(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual FrameDetail *GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CStringNN outputFile);
		};
	}
}
#endif
