#include "Stdafx.h"
#include "Data/ICaseStringUTF8Map.h"
#include "Text/Locale.h"

// http://msdn.microsoft.com/en-us/library/0h88fahh%28VS.85%29.aspx
Text::Locale::LocaleEntry Text::Locale::locales[] = {
	{0x0009, (const UTF8Char*)"en",    (const UTF8Char*)"English", 1252},
	{0x0401, (const UTF8Char*)"ar-sa", (const UTF8Char*)"Arabic - Saudi Arabia", 0},
	{0x0402, (const UTF8Char*)"bg",    (const UTF8Char*)"Bulgarian", 0},
	{0x0403, (const UTF8Char*)"ca",    (const UTF8Char*)"Catalan", 0},
	{0x0404, (const UTF8Char*)"zh-tw", (const UTF8Char*)"Chinese - Taiwan", 950},
	{0x0405, (const UTF8Char*)"cs",    (const UTF8Char*)"Czech", 1250},
	{0x0406, (const UTF8Char*)"da",    (const UTF8Char*)"Danish", 0},
	{0x0407, (const UTF8Char*)"de-de", (const UTF8Char*)"German - Germany", 0},
	{0x0408, (const UTF8Char*)"el",    (const UTF8Char*)"Greek", 0},
	{0x0409, (const UTF8Char*)"en-us", (const UTF8Char*)"English - United States", 1252},
	{0x040A, (const UTF8Char*)"",      (const UTF8Char*)"Spanish - Spain (Traditional)", 0},
	{0x040B, (const UTF8Char*)"fi",    (const UTF8Char*)"Finnish", 0},
	{0x040C, (const UTF8Char*)"fr-fr", (const UTF8Char*)"French - France", 0},
	{0x040D, (const UTF8Char*)"he",    (const UTF8Char*)"Hebrew", 0},
	{0x040E, (const UTF8Char*)"hu",    (const UTF8Char*)"Hungarian", 1250},
	{0x040F, (const UTF8Char*)"is",    (const UTF8Char*)"Icelandic", 0},
	{0x0410, (const UTF8Char*)"it-it", (const UTF8Char*)"Italian - Italy", 0},
	{0x0411, (const UTF8Char*)"ja",    (const UTF8Char*)"Japanese", 932},
	{0x0412, (const UTF8Char*)"ko",    (const UTF8Char*)"Korean", 0},
	{0x0413, (const UTF8Char*)"nl-nl", (const UTF8Char*)"Dutch - Netherlands", 0},
	{0x0414, (const UTF8Char*)"nb-no", (const UTF8Char*)"Norwegian - Bokmal", 0},
	{0x0415, (const UTF8Char*)"pl",    (const UTF8Char*)"Polish", 1250},
	{0x0416, (const UTF8Char*)"pt-br", (const UTF8Char*)"Portuguese - Brazil", 0},
	{0x0417, (const UTF8Char*)"rm",    (const UTF8Char*)"Raeto-Romance", 0},
	{0x0418, (const UTF8Char*)"ro",    (const UTF8Char*)"Romanian - Romania", 1250},
	{0x0419, (const UTF8Char*)"ru",    (const UTF8Char*)"Russian", 1251},
	{0x041A, (const UTF8Char*)"hr",    (const UTF8Char*)"Croatian", 1250},
	{0x041B, (const UTF8Char*)"sk",    (const UTF8Char*)"Slovak", 1250},
	{0x041C, (const UTF8Char*)"sq",    (const UTF8Char*)"Albanian", 1250},
	{0x041D, (const UTF8Char*)"sv-se", (const UTF8Char*)"Swedish - Sweden", 0},
	{0x041E, (const UTF8Char*)"th",    (const UTF8Char*)"Thai", 0},
	{0x041F, (const UTF8Char*)"tr",    (const UTF8Char*)"Turkish", 0},
	{0x0420, (const UTF8Char*)"ur",    (const UTF8Char*)"Urdu", 0},
	{0x0421, (const UTF8Char*)"id",    (const UTF8Char*)"Indonesian", 0},
	{0x0422, (const UTF8Char*)"uk",    (const UTF8Char*)"Ukrainian", 0},
	{0x0423, (const UTF8Char*)"be",    (const UTF8Char*)"Belarusian", 0},
	{0x0424, (const UTF8Char*)"sl",    (const UTF8Char*)"Slovenian", 1250},
	{0x0425, (const UTF8Char*)"et",    (const UTF8Char*)"Estonian", 0},
	{0x0426, (const UTF8Char*)"lv",    (const UTF8Char*)"Latvian", 0},
	{0x0427, (const UTF8Char*)"lt",    (const UTF8Char*)"Lithuanian", 0},
	{0x0429, (const UTF8Char*)"fa",    (const UTF8Char*)"Farsi", 0},
	{0x042A, (const UTF8Char*)"vi",    (const UTF8Char*)"Vietnamese", 0},
	{0x042B, (const UTF8Char*)"hy",    (const UTF8Char*)"Armenian", 0},
	{0x042C, (const UTF8Char*)"az-az", (const UTF8Char*)"Azeri - Latin", 0},
	{0x042D, (const UTF8Char*)"eu",    (const UTF8Char*)"Basque (Basque)", 0},
	{0x042E, (const UTF8Char*)"sb",    (const UTF8Char*)"Sorbian", 0},
	{0x042F, (const UTF8Char*)"mk",    (const UTF8Char*)"F.Y.R.O. Macedonia", 0},
	{0x0430, (const UTF8Char*)"st",    (const UTF8Char*)"Southern Sotho", 0},
	{0x0431, (const UTF8Char*)"ts",    (const UTF8Char*)"Tsonga", 0},
	{0x0432, (const UTF8Char*)"tn",    (const UTF8Char*)"Setsuana", 0},
	{0x0433, (const UTF8Char*)"",      (const UTF8Char*)"Venda", 0},
	{0x0434, (const UTF8Char*)"xh",    (const UTF8Char*)"Xhosa", 0},
	{0x0435, (const UTF8Char*)"zu",    (const UTF8Char*)"Zulu", 0},
	{0x0436, (const UTF8Char*)"af",    (const UTF8Char*)"Afrikaans", 0},
	{0x0437, (const UTF8Char*)"",      (const UTF8Char*)"Georgian", 0},
	{0x0438, (const UTF8Char*)"fo",    (const UTF8Char*)"Faroese", 0},
	{0x0439, (const UTF8Char*)"hi",    (const UTF8Char*)"Hindi", 0},
	{0x043A, (const UTF8Char*)"mt",    (const UTF8Char*)"Maltese", 0},
	{0x043B, (const UTF8Char*)"",      (const UTF8Char*)"Sami (Lappish)", 0},
	{0x043C, (const UTF8Char*)"gd",    (const UTF8Char*)"Scottish Gaelic (United Kingdom)", 0},
	{0x043D, (const UTF8Char*)"yi",    (const UTF8Char*)"Yiddish", 0},
	{0x043E, (const UTF8Char*)"ms-my", (const UTF8Char*)"Malay - Malaysia", 0},
	{0x043F, (const UTF8Char*)"",      (const UTF8Char*)"Kazakh", 0},
	{0x0440, (const UTF8Char*)"",      (const UTF8Char*)"Kyrgyz (Cyrillic)", 0},
	{0x0441, (const UTF8Char*)"sw",    (const UTF8Char*)"Swahili", 0},
	{0x0443, (const UTF8Char*)"uz-uz", (const UTF8Char*)"Uzbek (Latin)", 0},
	{0x0444, (const UTF8Char*)"tt",    (const UTF8Char*)"Tatar", 0},
	{0x0449, (const UTF8Char*)"ta",    (const UTF8Char*)"Tamil", 0},
	{0x044A, (const UTF8Char*)"",      (const UTF8Char*)"Telugu", 0},
	{0x044E, (const UTF8Char*)"mr",    (const UTF8Char*)"Marathi", 0},
	{0x044F, (const UTF8Char*)"sa",    (const UTF8Char*)"Sanskrit", 0},
	{0x0800, (const UTF8Char*)"",      (const UTF8Char*)"Default", 0},
	{0x0801, (const UTF8Char*)"ar-iq", (const UTF8Char*)"Arabic - Iraq", 0},
	{0x0804, (const UTF8Char*)"zh-cn", (const UTF8Char*)"Chinese - China", 936},
	{0x0807, (const UTF8Char*)"de-ch", (const UTF8Char*)"German - Switzerland", 0},
	{0x0809, (const UTF8Char*)"en-gb", (const UTF8Char*)"English - Great Britain", 1252},
	{0x080A, (const UTF8Char*)"es-mx", (const UTF8Char*)"Spanish - Mexico", 0},
	{0x080C, (const UTF8Char*)"fr-be", (const UTF8Char*)"French - Belgium", 0},
	{0x0810, (const UTF8Char*)"it-ch", (const UTF8Char*)"Italian - Switzerland", 0},
	{0x0813, (const UTF8Char*)"nl-be", (const UTF8Char*)"Dutch - Belgium", 0},
	{0x0814, (const UTF8Char*)"nn-no", (const UTF8Char*)"Norwegian - Nynorsk", 0},
	{0x0816, (const UTF8Char*)"pt-pt", (const UTF8Char*)"Portuguese - Portugal", 0},
	{0x0818, (const UTF8Char*)"ro-mo", (const UTF8Char*)"Romanian - Republic of Moldova", 0},
	{0x0819, (const UTF8Char*)"ru-mo", (const UTF8Char*)"Russian - Republic of Moldova", 0},
	{0x081A, (const UTF8Char*)"sr-sp", (const UTF8Char*)"Serbian - Latin", 1250},
	{0x081D, (const UTF8Char*)"sv-fi", (const UTF8Char*)"Swedish - Finland", 0},
	{0x082C, (const UTF8Char*)"az-az", (const UTF8Char*)"Azeri - Cyrillic", 1251},
	{0x083C, (const UTF8Char*)"gd-ie", (const UTF8Char*)"Irish - Ireland", 0},
	{0x083E, (const UTF8Char*)"ms-bn", (const UTF8Char*)"Malay (Brunei)", 0},
	{0x0843, (const UTF8Char*)"uz-uz", (const UTF8Char*)"Uzbek - Cyrillic", 0},
	{0x0C01, (const UTF8Char*)"ar-eg", (const UTF8Char*)"Arabic - Egypt", 0},
	{0x0C04, (const UTF8Char*)"zh-hk", (const UTF8Char*)"Chinese - Hong Kong SAR", 950},
	{0x0C07, (const UTF8Char*)"de-at", (const UTF8Char*)"German - Austria", 0},
	{0x0C09, (const UTF8Char*)"en-au", (const UTF8Char*)"English - Australia", 1252},
	{0x0C0A, (const UTF8Char*)"es-es", (const UTF8Char*)"Spanish - Spain (Modern)", 0},
	{0x0C0C, (const UTF8Char*)"fr-ca", (const UTF8Char*)"French - Canada", 0},
	{0x0C1A, (const UTF8Char*)"sr-sp", (const UTF8Char*)"Serbian - Cyrillic", 1251},
	{0x1001, (const UTF8Char*)"ar-ly", (const UTF8Char*)"Arabic - Libya", 0},
	{0x1004, (const UTF8Char*)"zh-sg", (const UTF8Char*)"Chinese - Singapore", 950},
	{0x1007, (const UTF8Char*)"de-lu", (const UTF8Char*)"German - Luxembourg", 0},
	{0x1009, (const UTF8Char*)"en-ca", (const UTF8Char*)"English - Canada", 1252},
	{0x100A, (const UTF8Char*)"es-gt", (const UTF8Char*)"Spanish - Guatemala", 0},
	{0x100C, (const UTF8Char*)"fr-ch", (const UTF8Char*)"French - Switzerland", 0},
	{0x1401, (const UTF8Char*)"ar-dz", (const UTF8Char*)"Arabic - Algeria", 0},
	{0x1404, (const UTF8Char*)"zh-mo", (const UTF8Char*)"Chinese - Macau SAR", 950},
	{0x1407, (const UTF8Char*)"de-li", (const UTF8Char*)"German - Liechtenstein", 0},
	{0x1409, (const UTF8Char*)"en-nz", (const UTF8Char*)"English - New Zealand", 1252},
	{0x140A, (const UTF8Char*)"es-cr", (const UTF8Char*)"Spanish - Costa Rica", 0},
	{0x140C, (const UTF8Char*)"fr-lu", (const UTF8Char*)"French - Luxembourg", 0},
	{0x1801, (const UTF8Char*)"ar-ma", (const UTF8Char*)"Arabic - Morocco", 0},
	{0x1809, (const UTF8Char*)"en-ie", (const UTF8Char*)"English - Ireland", 1252},
	{0x180A, (const UTF8Char*)"es-pa", (const UTF8Char*)"Spanish - Panama", 0},
	{0x1C01, (const UTF8Char*)"ar-tn", (const UTF8Char*)"Arabic - Tunisia", 0},
	{0x1C09, (const UTF8Char*)"en-za", (const UTF8Char*)"English - Southern Africa", 1252},
	{0x1C0A, (const UTF8Char*)"es-do", (const UTF8Char*)"Spanish - Dominican Republic", 0},
	{0x2001, (const UTF8Char*)"ar-om", (const UTF8Char*)"Arabic - Oman", 0},
	{0x2009, (const UTF8Char*)"en-jm", (const UTF8Char*)"English - Jamaica", 1252},
	{0x200A, (const UTF8Char*)"es-ve", (const UTF8Char*)"Spanish - Venezuela", 0},
	{0x2401, (const UTF8Char*)"ar-ye", (const UTF8Char*)"Arabic - Yemen", 0},
	{0x2409, (const UTF8Char*)"en-cb", (const UTF8Char*)"English - Caribbean", 1252},
	{0x240A, (const UTF8Char*)"es-co", (const UTF8Char*)"Spanish - Colombia", 0},
	{0x2801, (const UTF8Char*)"ar-sy", (const UTF8Char*)"Arabic - Syria", 0},
	{0x2809, (const UTF8Char*)"en-bz", (const UTF8Char*)"English - Belize", 1252},
	{0x280A, (const UTF8Char*)"es-pe", (const UTF8Char*)"Spanish - Peru", 0},
	{0x2C01, (const UTF8Char*)"ar-jo", (const UTF8Char*)"Arabic - Jordan", 0},
	{0x2C09, (const UTF8Char*)"en-tt", (const UTF8Char*)"English - Trinidad", 1252},
	{0x2C0A, (const UTF8Char*)"es-ar", (const UTF8Char*)"Spanish - Argentina", 0},
	{0x3001, (const UTF8Char*)"ar-lb", (const UTF8Char*)"Arabic - Lebanon", 0},
	{0x3009, (const UTF8Char*)"en-zw", (const UTF8Char*)"English - Zimbabwe", 1252},
	{0x300A, (const UTF8Char*)"es-ec", (const UTF8Char*)"Spanish - Ecuador", 0},
	{0x3401, (const UTF8Char*)"ar-kw", (const UTF8Char*)"Arabic - Kuwait", 0},
	{0x3409, (const UTF8Char*)"en-ph", (const UTF8Char*)"English - Phillippines", 1252},
	{0x340A, (const UTF8Char*)"es-cl", (const UTF8Char*)"Spanish - Chile", 0},
	{0x3801, (const UTF8Char*)"ar-ae", (const UTF8Char*)"Arabic - United Arab Emirates", 0},
	{0x380A, (const UTF8Char*)"es-uy", (const UTF8Char*)"Spanish - Uruguay", 0},
	{0x3C01, (const UTF8Char*)"ar-bh", (const UTF8Char*)"Arabic - Bahrain", 0},
	{0x3C0A, (const UTF8Char*)"es-py", (const UTF8Char*)"Spanish - Paraguay", 0},
	{0x4001, (const UTF8Char*)"ar-qa", (const UTF8Char*)"Arabic - Qatar", 0},
	{0x4009, (const UTF8Char*)"en-in", (const UTF8Char*)"English - India", 1252},
	{0x400A, (const UTF8Char*)"es-bo", (const UTF8Char*)"Spanish - Bolivia", 0},
	{0x4409, (const UTF8Char*)"en-my", (const UTF8Char*)"English - Malaysia", 1252},
	{0x440A, (const UTF8Char*)"es-sv", (const UTF8Char*)"Spanish - El Salvador", 0},
	{0x4809, (const UTF8Char*)"en-sg", (const UTF8Char*)"English - Singapore", 1252},
	{0x480A, (const UTF8Char*)"es-hn", (const UTF8Char*)"Spanish - Honduras", 0},
	{0x4C0A, (const UTF8Char*)"es-ni", (const UTF8Char*)"Spanish - Nicaragua", 0},
	{0x500A, (const UTF8Char*)"es-pr", (const UTF8Char*)"Spanish - Puerto Rico", 0}};

Text::Locale::LocaleEntry *Text::Locale::GetLocaleEntry(Int32 lcid)
{
	OSInt i = 0;
	OSInt j = (sizeof(locales) / sizeof(locales[0])) - 1;
	OSInt k;
	while (i <= j)
	{
		k = (i + j) >> 1;
		if (lcid > locales[k].lcid)
		{
			i = k + 1;
		}
		else if (lcid < locales[k].lcid)
		{
			j = k - 1;
		}
		else
		{
			return &locales[k];
		}
	}
	return 0;
}

Text::Locale::LocaleEntry *Text::Locale::GetLocaleEntryByCodePage(Int32 codePage)
{
	OSInt j = (sizeof(locales) / sizeof(locales[0]));
	while (j-- > 0)
	{
		if (locales[j].defCodePage == codePage)
			return &locales[j];
	}
	return 0;
}

Text::Locale::Locale()
{
	NEW_CLASS(this->names, Data::ICaseStringUTF8Map<Text::Locale::LocaleEntry*>());
	OSInt i = sizeof(locales) / sizeof(locales[0]);
	while (i-- > 0)
	{
		if (this->locales[i].shortName[0] != 0)
		{
			this->names->Put(this->locales[i].shortName, &this->locales[i]);
		}
	}
}

Text::Locale::~Locale()
{
	DEL_CLASS(this->names);
}

Text::Locale::LocaleEntry *Text::Locale::GetLocaleEntryByName(const UTF8Char *name)
{
	return this->names->Get(name);
}
