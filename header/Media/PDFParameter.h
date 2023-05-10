#ifndef _SM_MEDIA_PDFPARAMETER
#define _SM_MEDIA_PDFPARAMETER
#include "Data/ArrayList.h"
#include "Text/String.h"

namespace Media
{
	class PDFParameter
	{
	private:
		struct ParamEntry
		{
			Text::String *type;
			Text::String *value;
		};
	private:
		Data::ArrayList<ParamEntry*> entries;
	public:
		PDFParameter();
		~PDFParameter();

		void AddEntry(Text::CString type, Text::CString value);
		Text::String *GetEntry(Text::CString type);

		static PDFParameter *Parse(Text::CString parameter);
	};
}
#endif
