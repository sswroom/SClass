#ifndef _SM_TEXT_LOCALE
#define _SM_TEXT_LOCALE
#include "Data/FastStringMapNN.hpp"

namespace Text
{
	class Locale
	{
	public:
		typedef struct
		{
			UInt32 lcid;
			UnsafeArray<const UTF8Char> shortName;
			UOSInt shortNameLen;
			UnsafeArray<const UTF8Char> desc;
			UOSInt descLen;
			UInt32 defCodePage;
		} LocaleEntry;

	private:
		static LocaleEntry locales[];
	public:
		static Optional<LocaleEntry> GetLocaleEntry(UInt32 lcid); //see Text::EncodingFactory::GetSystemLCID()
		static Optional<LocaleEntry> GetLocaleEntryByCodePage(UInt32 codePage);

	private:
		Data::FastStringMapNN<LocaleEntry> names;
	public:
		Locale();
		~Locale();

		Optional<LocaleEntry> GetLocaleEntryByName(Text::CStringNN name);
	};
}
#endif
