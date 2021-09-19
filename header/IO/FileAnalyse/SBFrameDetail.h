#ifndef _SM_IO_FILEANALYSE_SBFRAMEDETAIL
#define _SM_IO_FILEANALYSE_SBFRAMEDETAIL
#include "IO/FileAnalyse/FrameDetailHandler.h"
#include "Text/StringBuilderUTF.h"

namespace IO
{
	namespace FileAnalyse
	{
		class SBFrameDetail : public IO::FileAnalyse::FrameDetailHandler
		{
		private:
			Text::StringBuilderUTF *sb;
		public:
			SBFrameDetail(Text::StringBuilderUTF *sb);
			virtual ~SBFrameDetail();

			virtual void AddHeader(const UTF8Char *header);
			virtual void AddField(UInt32 ofst, UInt32 size, const UTF8Char *name, const UTF8Char *value);
			virtual void AddSubfield(UInt32 ofst, UInt32 size, const UTF8Char *name, const UTF8Char *value);
			virtual void AddFieldSeperstor(UInt32 ofst, const UTF8Char *name);
			virtual void AddText(UInt32 ofst, const UTF8Char *name);
		};
	}
}
#endif
