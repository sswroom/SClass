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
			NotNullPtr<Text::String> type;
			Optional<Text::String> value;
		};
	private:
		Data::ArrayList<ParamEntry*> entries;
	public:
		PDFParameter();
		~PDFParameter();

		void AddEntry(Text::CString type, Text::CString value);
		Optional<Text::String> GetEntryValue(Text::CString type) const;
		Optional<Text::String> GetEntryType(UOSInt index) const;
		Optional<Text::String> GetEntryValue(UOSInt index) const;
		Bool ContainsEntry(Text::CString type) const;
		ParamEntry *GetEntry(Text::CString type) const;
		UOSInt GetEntryIndex(Text::CString type) const;
		UOSInt GetCount() const;
		ParamEntry *GetItem(UOSInt index) const;

		static PDFParameter *Parse(Text::CString parameter);
	};
}
#endif
