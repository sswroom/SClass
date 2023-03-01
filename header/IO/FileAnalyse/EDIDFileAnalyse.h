#ifndef _SM_IO_FILEANALYSE_EDIDFILEANALYSE
#define _SM_IO_FILEANALYSE_EDIDFILEANALYSE
#include "IO/StreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"

namespace IO
{
	namespace FileAnalyse
	{
		class EDIDFileAnalyse : public IO::FileAnalyse::IFileAnalyse
		{
		private:
			UOSInt blockCnt;
			IO::StreamData *fd;

			static void ParseDescriptor(FrameDetail *frame, const UInt8 *buff, UOSInt ofst);
			static void RemoveNonASCII(UTF8Char *sbuff, UTF8Char *sbuffEnd);
		public:
			EDIDFileAnalyse(IO::StreamData *fd);
			virtual ~EDIDFileAnalyse();

			virtual Text::CString GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, Text::StringBuilderUTF8 *sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual FrameDetail *GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CString outputFile);

			static Text::CString DigitalBitDepthGetName(UOSInt val);
			static Text::CString DigitalVideoInterfaceGetName(UOSInt val);
			static Text::CString DigitalDisplayTypeGetName(UOSInt val);
			static Text::CString AnalogLevelGetName(UOSInt val);
			static Text::CString AnalogDisplayTypeGetName(UOSInt val);
			static Text::CString ImageAspectRatioGetName(UOSInt val);
			static Text::CString DescriptorTypeGetName(UInt8 val);
			static Text::CString ExtensionTagGetName(UInt8 val);
			static Text::CString AspectRatioPreferenceGetName(UOSInt val);
			static Text::CString AudioFormatGetName(UOSInt val);
			static Text::CString AudioExtFormatGetName(UOSInt val);
		};
	}
}
#endif
