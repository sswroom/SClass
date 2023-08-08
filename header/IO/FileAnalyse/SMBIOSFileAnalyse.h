#ifndef _SM_IO_FILEANALYSE_SMBIOSFILEANALYSE
#define _SM_IO_FILEANALYSE_SMBIOSFILEANALYSE
#include "IO/StreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class SMBIOSFileAnalyse : public IO::FileAnalyse::IFileAnalyse
		{
		private:
			typedef struct
			{
				UOSInt fileOfst;
				UOSInt packSize;
				UInt8 packType;
			} PackInfo;
		private:
			IO::StreamData *fd;
			Data::ArrayList<PackInfo*> packs;

		public:
			SMBIOSFileAnalyse(NotNullPtr<IO::StreamData> fd);
			virtual ~SMBIOSFileAnalyse();

			virtual Text::CStringNN GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual FrameDetail *GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CStringNN outputFile);

			static Text::CString SMBIOSTypeGetName(UInt8 type);
			static Text::CString MemoryLocationGetName(UInt8 location);
			static Text::CString PointingDeviceInterfaceGetName(UInt8 v);
			static Text::CString SlotTypeGetName(UInt8 v);
			static void AddString(FrameDetail *frame, UOSInt ofst, Data::ByteArrayR packBuff, Text::CString *carr, Text::CString name);
			static void AddHex8(FrameDetail *frame, UOSInt ofst, Data::ByteArrayR packBuff, Text::CString *carr, Text::CString name);
			static void AddHex16(FrameDetail *frame, UOSInt ofst, Data::ByteArrayR packBuff, Text::CString *carr, Text::CString name);
			static void AddHex32(FrameDetail *frame, UOSInt ofst, Data::ByteArrayR packBuff, Text::CString *carr, Text::CString name);
			static void AddHex64(FrameDetail *frame, UOSInt ofst, Data::ByteArrayR packBuff, Text::CString *carr, Text::CString name);
			static void AddUInt8(FrameDetail *frame, UOSInt ofst, Data::ByteArrayR packBuff, Text::CString *carr, Text::CString name);
			static void AddUInt16(FrameDetail *frame, UOSInt ofst, Data::ByteArrayR packBuff, Text::CString *carr, Text::CString name);
			static void AddUInt32(FrameDetail *frame, UOSInt ofst, Data::ByteArrayR packBuff, Text::CString *carr, Text::CString name);
			static void AddUInt64(FrameDetail *frame, UOSInt ofst, Data::ByteArrayR packBuff, Text::CString *carr, Text::CString name);
			static void AddUUID(FrameDetail *frame, UOSInt ofst, Data::ByteArrayR packBuff, Text::CString *carr, Text::CString name);
			static void AddDate(FrameDetail *frame, UOSInt ofst, Data::ByteArrayR packBuff, Text::CString *carr, Text::CString name);
			static void AddBits(FrameDetail *frame, UOSInt ofst, Data::ByteArrayR packBuff, Text::CString *carr, const Char *bitNames[]);
			static void AddEnum(FrameDetail *frame, UOSInt ofst, Data::ByteArrayR packBuff, Text::CString *carr, Text::CString name, const Char *names[], UOSInt namesCnt);
			static void AddEnum(FrameDetail *frame, UOSInt ofst, UInt8 val, Text::CString *carr, Text::CString name, const Char *names[], UOSInt namesCnt);
		};
	}
}
#endif
