#ifndef _SM_IO_FILEANALYSE_RAR5FILEANALYSE
#define _SM_IO_FILEANALYSE_RAR5FILEANALYSE
#include "Data/SyncArrayList.h"
#include "IO/IStreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
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
			IO::IStreamData *fd;
			Data::SyncArrayList<BlockInfo*> packs;

			Bool pauseParsing;
			Bool threadRunning;
			Bool threadToStop;
			Bool threadStarted;

			static const UInt8 *ReadVInt(const UInt8 *buffPtr, UInt64 *val);
			static const UInt8 *AddVInt(IO::FileAnalyse::FrameDetail *frame, UOSInt ofst, Text::CString name, const UInt8 *buffPtr);
			static const UInt8 *AddVInt(IO::FileAnalyse::FrameDetail *frame, UOSInt ofst, Text::CString name, const UInt8 *buffPtr, UInt64 *val);
			static const UInt8 *AddVHex(IO::FileAnalyse::FrameDetail *frame, UOSInt ofst, Text::CString name, const UInt8 *buffPtr, UInt64 *val);
			static UInt32 __stdcall ParseThread(void *userObj);
		public:
			RAR5FileAnalyse(IO::IStreamData *fd);
			virtual ~RAR5FileAnalyse();

			virtual Text::CString GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, Text::StringBuilderUTF8 *sb);
			virtual Bool GetFrameDetail(UOSInt index, Text::StringBuilderUTF8 *sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual FrameDetail *GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CString outputFile);
		};
	}
}
#endif
