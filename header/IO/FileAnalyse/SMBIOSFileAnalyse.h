#ifndef _SM_IO_FILEANALYSE_SMBIOSFILEANALYSE
#define _SM_IO_FILEANALYSE_SMBIOSFILEANALYSE
#include "IO/IStreamData.h"
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
			IO::IStreamData *fd;
			Data::ArrayList<PackInfo*> packs;

		public:
			SMBIOSFileAnalyse(IO::IStreamData *fd);
			virtual ~SMBIOSFileAnalyse();

			virtual Text::CString GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, Text::StringBuilderUTF8 *sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual FrameDetail *GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CString outputFile);

			static Text::CString SMBIOSTypeGetName(UInt8 type);
			static Text::CString MemoryLocationGetName(UInt8 location);
			static void AddString(FrameDetail *frame, UOSInt ofst, const UInt8 *packBuff, Text::CString *carr, Text::CString name);
			static void AddHex8(FrameDetail *frame, UOSInt ofst, const UInt8 *packBuff, Text::CString *carr, Text::CString name);
			static void AddHex16(FrameDetail *frame, UOSInt ofst, const UInt8 *packBuff, Text::CString *carr, Text::CString name);
			static void AddHex32(FrameDetail *frame, UOSInt ofst, const UInt8 *packBuff, Text::CString *carr, Text::CString name);
			static void AddHex64(FrameDetail *frame, UOSInt ofst, const UInt8 *packBuff, Text::CString *carr, Text::CString name);
			static void AddUInt8(FrameDetail *frame, UOSInt ofst, const UInt8 *packBuff, Text::CString *carr, Text::CString name);
			static void AddUInt16(FrameDetail *frame, UOSInt ofst, const UInt8 *packBuff, Text::CString *carr, Text::CString name);
			static void AddUInt32(FrameDetail *frame, UOSInt ofst, const UInt8 *packBuff, Text::CString *carr, Text::CString name);
			static void AddUInt64(FrameDetail *frame, UOSInt ofst, const UInt8 *packBuff, Text::CString *carr, Text::CString name);
			static void AddUUID(FrameDetail *frame, UOSInt ofst, const UInt8 *packBuff, Text::CString *carr, Text::CString name);
			static void AddDate(FrameDetail *frame, UOSInt ofst, const UInt8 *packBuff, Text::CString *carr, Text::CString name);
			static void AddBits(FrameDetail *frame, UOSInt ofst, const UInt8 *packBuff, Text::CString *carr, const Char *bitNames[]);
			static void AddEnum(FrameDetail *frame, UOSInt ofst, const UInt8 *packBuff, Text::CString *carr, Text::CString name, const Char *names[], UOSInt namesCnt);
			static void AddEnum(FrameDetail *frame, UOSInt ofst, UInt8 val, Text::CString *carr, Text::CString name, const Char *names[], UOSInt namesCnt);
		};
	}
}
#endif
