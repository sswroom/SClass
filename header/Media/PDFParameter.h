#ifndef _SM_MEDIA_PDFPARAMETER
#define _SM_MEDIA_PDFPARAMETER
#include "Data/ArrayList.h"
#include "Text/String.h"

namespace Media
{
	class PDFParameter
	{
	public:
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
		Text::String *GetEntryValue(Text::CString type) const;
		Bool ContainsEntry(Text::CString type) const;
		ParamEntry *GetEntry(Text::CString type) const;
		UOSInt GetCount() const;
		ParamEntry *GetItem(UOSInt index) const;

		static PDFParameter *Parse(Text::CString parameter);
	};
}
#endif
