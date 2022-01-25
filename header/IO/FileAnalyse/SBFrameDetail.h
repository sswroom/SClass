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
			Text::StringBuilderUTF8 *sb;
		public:
			SBFrameDetail(Text::StringBuilderUTF8 *sb);
			virtual ~SBFrameDetail();

			virtual void AddHeader(const UTF8Char *header);
			virtual void AddField(UOSInt ofst, UOSInt size, const UTF8Char *name, const UTF8Char *value);
			virtual void AddSubfield(UOSInt ofst, UOSInt size, const UTF8Char *name, const UTF8Char *value);
			virtual void AddFieldSeperstor(UOSInt ofst, const UTF8Char *name);
			virtual void AddText(UOSInt ofst, const UTF8Char *name);
			virtual void AddSubframe(UOSInt ofst, UOSInt size);
		};
	}
}
#endif
