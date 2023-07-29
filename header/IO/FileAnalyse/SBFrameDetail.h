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
			NotNullPtr<Text::StringBuilderUTF8> sb;
		public:
			SBFrameDetail(NotNullPtr<Text::StringBuilderUTF8> sb);
			virtual ~SBFrameDetail();

			virtual void AddHeader(Text::CString header);
			virtual void AddField(UOSInt ofst, UOSInt size, Text::CString name, Text::CString value);
			virtual void AddSubfield(UOSInt ofst, UOSInt size, Text::CString name, Text::CString value);
			virtual void AddFieldSeperstor(UOSInt ofst, Text::CString name);
			virtual void AddText(UOSInt ofst, Text::CString name);
			virtual void AddSubframe(UOSInt ofst, UOSInt size);
		};
	}
}
#endif
