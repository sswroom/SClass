#ifndef _SM_TEXT_LOCALE
#define _SM_TEXT_LOCALE
#include "Data/FastStringMapNN.h"

namespace Text
{
	class Locale
	{
	public:
		typedef struct
		{
			UInt32 lcid;
			const UTF8Char *shortName;
			UOSInt shortNameLen;
			const UTF8Char *desc;
			UOSInt descLen;
			UInt32 defCodePage;
		} LocaleEntry;

	private:
		static LocaleEntry locales[];
	public:
		static LocaleEntry *GetLocaleEntry(UInt32 lcid); //see Text::EncodingFactory::GetSystemLCID()
		static LocaleEntry *GetLocaleEntryByCodePage(UInt32 codePage);

	private:
		Data::FastStringMapNN<LocaleEntry> names;
	public:
		Locale();
		~Locale();

		Optional<LocaleEntry> GetLocaleEntryByName(Text::CStringNN name);
	};
}
#endif
