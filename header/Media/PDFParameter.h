#ifndef _SM_MEDIA_PDFPARAMETER
#define _SM_MEDIA_PDFPARAMETER
#include "Data/ArrayListNN.hpp"
#include "Text/String.h"

namespace Media
{
	class PDFParameter
	{
	public:
		struct ParamEntry
		{
			NN<Text::String> type;
			Optional<Text::String> value;
		};
	private:
		Data::ArrayListNN<ParamEntry> entries;
	public:
		PDFParameter();
		~PDFParameter();

		void AddEntry(Text::CStringNN type, Text::CString value);
		Optional<Text::String> GetEntryValue(Text::CStringNN type) const;
		Optional<Text::String> GetEntryType(UOSInt index) const;
		Optional<Text::String> GetEntryValue(UOSInt index) const;
		Bool ContainsEntry(Text::CStringNN type) const;
		Optional<ParamEntry> GetEntry(Text::CStringNN type) const;
		UOSInt GetEntryIndex(Text::CStringNN type) const;
		UOSInt GetCount() const;
		NN<ParamEntry> GetItemNoCheck(UOSInt index) const;
		Optional<ParamEntry> GetItem(UOSInt index) const;

		static Optional<PDFParameter> Parse(Text::CStringNN parameter);
	};
}
#endif
