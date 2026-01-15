#ifndef _SM_IO_FILEANALYSE_SMBIOSFILEANALYSE
#define _SM_IO_FILEANALYSE_SMBIOSFILEANALYSE
#include "IO/StreamData.h"
#include "IO/FileAnalyse/FileAnalyser.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class SMBIOSFileAnalyse : public IO::FileAnalyse::FileAnalyser
		{
		private:
			typedef struct
			{
				UIntOS fileOfst;
				UIntOS packSize;
				UInt8 packType;
			} PackInfo;
		private:
			Optional<IO::StreamData> fd;
			Data::ArrayListNN<PackInfo> packs;

		public:
			SMBIOSFileAnalyse(NN<IO::StreamData> fd);
			virtual ~SMBIOSFileAnalyse();

			virtual Text::CStringNN GetFormatName();
			virtual UIntOS GetFrameCount();
			virtual Bool GetFrameName(UIntOS index, NN<Text::StringBuilderUTF8> sb);
			virtual UIntOS GetFrameIndex(UInt64 ofst);
			virtual Optional<FrameDetail> GetFrameDetail(UIntOS index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CStringNN outputFile);

			static Text::CStringNN SMBIOSTypeGetName(UInt8 type);
			static Text::CString MemoryLocationGetName(UInt8 location);
			static Text::CString PointingDeviceInterfaceGetName(UInt8 v);
			static Text::CString SlotTypeGetName(UInt8 v);
			static void AddString(NN<FrameDetail> frame, UIntOS ofst, Data::ByteArrayR packBuff, UnsafeArray<Text::CString> carr, Text::CStringNN name);
			static void AddHex8(NN<FrameDetail> frame, UIntOS ofst, Data::ByteArrayR packBuff, UnsafeArray<Text::CString> carr, Text::CStringNN name);
			static void AddHex16(NN<FrameDetail> frame, UIntOS ofst, Data::ByteArrayR packBuff, UnsafeArray<Text::CString> carr, Text::CStringNN name);
			static void AddHex32(NN<FrameDetail> frame, UIntOS ofst, Data::ByteArrayR packBuff, UnsafeArray<Text::CString> carr, Text::CStringNN name);
			static void AddHex64(NN<FrameDetail> frame, UIntOS ofst, Data::ByteArrayR packBuff, UnsafeArray<Text::CString> carr, Text::CStringNN name);
			static void AddUInt8(NN<FrameDetail> frame, UIntOS ofst, Data::ByteArrayR packBuff, UnsafeArray<Text::CString> carr, Text::CStringNN name);
			static void AddUInt16(NN<FrameDetail> frame, UIntOS ofst, Data::ByteArrayR packBuff, UnsafeArray<Text::CString> carr, Text::CStringNN name);
			static void AddUInt32(NN<FrameDetail> frame, UIntOS ofst, Data::ByteArrayR packBuff, UnsafeArray<Text::CString> carr, Text::CStringNN name);
			static void AddUInt64(NN<FrameDetail> frame, UIntOS ofst, Data::ByteArrayR packBuff, UnsafeArray<Text::CString> carr, Text::CStringNN name);
			static void AddUUID(NN<FrameDetail> frame, UIntOS ofst, Data::ByteArrayR packBuff, UnsafeArray<Text::CString> carr, Text::CStringNN name);
			static void AddDate(NN<FrameDetail> frame, UIntOS ofst, Data::ByteArrayR packBuff, UnsafeArray<Text::CString> carr, Text::CStringNN name);
			static void AddBits(NN<FrameDetail> frame, UIntOS ofst, Data::ByteArrayR packBuff, UnsafeArray<Text::CString> carr, const Char *bitNames[]);
			static void AddEnum(NN<FrameDetail> frame, UIntOS ofst, Data::ByteArrayR packBuff, UnsafeArray<Text::CString> carr, Text::CStringNN name, const Char *names[], UIntOS namesCnt);
			static void AddEnum(NN<FrameDetail> frame, UIntOS ofst, UInt8 val, UnsafeArray<Text::CString> carr, Text::CStringNN name, const Char *names[], UIntOS namesCnt);
		};
	}
}
#endif
