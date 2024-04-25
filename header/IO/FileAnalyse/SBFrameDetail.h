#ifndef _SM_IO_FILEANALYSE_SBFRAMEDETAIL
#define _SM_IO_FILEANALYSE_SBFRAMEDETAIL
#include "IO/FileAnalyse/FrameDetailHandler.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class SBFrameDetail : public IO::FileAnalyse::FrameDetailHandler
		{
		private:
			NN<Text::StringBuilderUTF8> sb;
		public:
			SBFrameDetail(NN<Text::StringBuilderUTF8> sb);
			virtual ~SBFrameDetail();

			virtual void AddHeader(Text::CStringNN header);
			virtual void AddField(UOSInt ofst, UOSInt size, Text::CStringNN name, Text::CString value);
			virtual void AddSubfield(UOSInt ofst, UOSInt size, Text::CStringNN name, Text::CString value);
			virtual void AddFieldSeperstor(UOSInt ofst, Text::CStringNN name);
			virtual void AddText(UOSInt ofst, Text::CStringNN name);
			virtual void AddSubframe(UOSInt ofst, UOSInt size);
		};
	}
}
#endif
