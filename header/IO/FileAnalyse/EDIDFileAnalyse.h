#ifndef _SM_IO_FILEANALYSE_EDIDFILEANALYSE
#define _SM_IO_FILEANALYSE_EDIDFILEANALYSE
#include "IO/StreamData.h"
#include "IO/FileAnalyse/FileAnalyser.h"

namespace IO
{
	namespace FileAnalyse
	{
		class EDIDFileAnalyse : public IO::FileAnalyse::FileAnalyser
		{
		private:
			UIntOS blockCnt;
			Optional<IO::StreamData> fd;

			static void ParseDescriptor(NN<FrameDetail> frame, UnsafeArray<const UInt8> buff, UIntOS ofst);
			static void RemoveNonASCII(UnsafeArray<UTF8Char> sbuff, UnsafeArray<UTF8Char> sbuffEnd);
		public:
			EDIDFileAnalyse(NN<IO::StreamData> fd);
			virtual ~EDIDFileAnalyse();

			virtual Text::CStringNN GetFormatName();
			virtual UIntOS GetFrameCount();
			virtual Bool GetFrameName(UIntOS index, NN<Text::StringBuilderUTF8> sb);
			virtual UIntOS GetFrameIndex(UInt64 ofst);
			virtual Optional<FrameDetail> GetFrameDetail(UIntOS index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CStringNN outputFile);

			static Text::CString DigitalBitDepthGetName(UIntOS val);
			static Text::CString DigitalVideoInterfaceGetName(UIntOS val);
			static Text::CString DigitalDisplayTypeGetName(UIntOS val);
			static Text::CString AnalogLevelGetName(UIntOS val);
			static Text::CString AnalogDisplayTypeGetName(UIntOS val);
			static Text::CString ImageAspectRatioGetName(UIntOS val);
			static Text::CString DescriptorTypeGetName(UInt8 val);
			static Text::CString ExtensionTagGetName(UInt8 val);
			static Text::CString AspectRatioPreferenceGetName(UIntOS val);
			static Text::CString AudioFormatGetName(UIntOS val);
			static Text::CString AudioExtFormatGetName(UIntOS val);
		};
	}
}
#endif
