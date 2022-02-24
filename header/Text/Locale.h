#ifndef _SM_TEXT_LOCALE
#define _SM_TEXT_LOCALE
#include "Data/FastStringMap.h"

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
		Data::FastStringMap<LocaleEntry*> *names;
	public:
		Locale();
		~Locale();

		LocaleEntry *GetLocaleEntryByName(Text::CString name);
	};
}
#endif
