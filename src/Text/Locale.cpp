#include "Stdafx.h"
#include "Data/ICaseStringUTF8Map.h"
#include "Text/Locale.h"

// http://msdn.microsoft.com/en-us/library/0h88fahh%28VS.85%29.aspx
Text::Locale::LocaleEntry Text::Locale::locales[] = {
	{0x0009, UTF8STRC("en"),    UTF8STRC("English"), 1252},
	{0x0401, UTF8STRC("ar-sa"), UTF8STRC("Arabic - Saudi Arabia"), 0},
	{0x0402, UTF8STRC("bg"),    UTF8STRC("Bulgarian"), 0},
	{0x0403, UTF8STRC("ca"),    UTF8STRC("Catalan"), 0},
	{0x0404, UTF8STRC("zh-tw"), UTF8STRC("Chinese - Taiwan"), 950},
	{0x0405, UTF8STRC("cs"),    UTF8STRC("Czech"), 1250},
	{0x0406, UTF8STRC("da"),    UTF8STRC("Danish"), 0},
	{0x0407, UTF8STRC("de-de"), UTF8STRC("German - Germany"), 0},
	{0x0408, UTF8STRC("el"),    UTF8STRC("Greek"), 0},
	{0x0409, UTF8STRC("en-us"), UTF8STRC("English - United States"), 1252},
	{0x040A, UTF8STRC(""),      UTF8STRC("Spanish - Spain (Traditional)"), 0},
	{0x040B, UTF8STRC("fi"),    UTF8STRC("Finnish"), 0},
	{0x040C, UTF8STRC("fr-fr"), UTF8STRC("French - France"), 0},
	{0x040D, UTF8STRC("he"),    UTF8STRC("Hebrew"), 0},
	{0x040E, UTF8STRC("hu"),    UTF8STRC("Hungarian"), 1250},
	{0x040F, UTF8STRC("is"),    UTF8STRC("Icelandic"), 0},
	{0x0410, UTF8STRC("it-it"), UTF8STRC("Italian - Italy"), 0},
	{0x0411, UTF8STRC("ja"),    UTF8STRC("Japanese"), 932},
	{0x0412, UTF8STRC("ko"),    UTF8STRC("Korean"), 0},
	{0x0413, UTF8STRC("nl-nl"), UTF8STRC("Dutch - Netherlands"), 0},
	{0x0414, UTF8STRC("nb-no"), UTF8STRC("Norwegian - Bokmal"), 0},
	{0x0415, UTF8STRC("pl"),    UTF8STRC("Polish"), 1250},
	{0x0416, UTF8STRC("pt-br"), UTF8STRC("Portuguese - Brazil"), 0},
	{0x0417, UTF8STRC("rm"),    UTF8STRC("Raeto-Romance"), 0},
	{0x0418, UTF8STRC("ro"),    UTF8STRC("Romanian - Romania"), 1250},
	{0x0419, UTF8STRC("ru"),    UTF8STRC("Russian"), 1251},
	{0x041A, UTF8STRC("hr"),    UTF8STRC("Croatian"), 1250},
	{0x041B, UTF8STRC("sk"),    UTF8STRC("Slovak"), 1250},
	{0x041C, UTF8STRC("sq"),    UTF8STRC("Albanian"), 1250},
	{0x041D, UTF8STRC("sv-se"), UTF8STRC("Swedish - Sweden"), 0},
	{0x041E, UTF8STRC("th"),    UTF8STRC("Thai"), 0},
	{0x041F, UTF8STRC("tr"),    UTF8STRC("Turkish"), 0},
	{0x0420, UTF8STRC("ur"),    UTF8STRC("Urdu"), 0},
	{0x0421, UTF8STRC("id"),    UTF8STRC("Indonesian"), 0},
	{0x0422, UTF8STRC("uk"),    UTF8STRC("Ukrainian"), 0},
	{0x0423, UTF8STRC("be"),    UTF8STRC("Belarusian"), 0},
	{0x0424, UTF8STRC("sl"),    UTF8STRC("Slovenian"), 1250},
	{0x0425, UTF8STRC("et"),    UTF8STRC("Estonian"), 0},
	{0x0426, UTF8STRC("lv"),    UTF8STRC("Latvian"), 0},
	{0x0427, UTF8STRC("lt"),    UTF8STRC("Lithuanian"), 0},
	{0x0429, UTF8STRC("fa"),    UTF8STRC("Farsi"), 0},
	{0x042A, UTF8STRC("vi"),    UTF8STRC("Vietnamese"), 0},
	{0x042B, UTF8STRC("hy"),    UTF8STRC("Armenian"), 0},
	{0x042C, UTF8STRC("az-az"), UTF8STRC("Azeri - Latin"), 0},
	{0x042D, UTF8STRC("eu"),    UTF8STRC("Basque (Basque)"), 0},
	{0x042E, UTF8STRC("sb"),    UTF8STRC("Sorbian"), 0},
	{0x042F, UTF8STRC("mk"),    UTF8STRC("F.Y.R.O. Macedonia"), 0},
	{0x0430, UTF8STRC("st"),    UTF8STRC("Southern Sotho"), 0},
	{0x0431, UTF8STRC("ts"),    UTF8STRC("Tsonga"), 0},
	{0x0432, UTF8STRC("tn"),    UTF8STRC("Setsuana"), 0},
	{0x0433, UTF8STRC(""),      UTF8STRC("Venda"), 0},
	{0x0434, UTF8STRC("xh"),    UTF8STRC("Xhosa"), 0},
	{0x0435, UTF8STRC("zu"),    UTF8STRC("Zulu"), 0},
	{0x0436, UTF8STRC("af"),    UTF8STRC("Afrikaans"), 0},
	{0x0437, UTF8STRC(""),      UTF8STRC("Georgian"), 0},
	{0x0438, UTF8STRC("fo"),    UTF8STRC("Faroese"), 0},
	{0x0439, UTF8STRC("hi"),    UTF8STRC("Hindi"), 0},
	{0x043A, UTF8STRC("mt"),    UTF8STRC("Maltese"), 0},
	{0x043B, UTF8STRC(""),      UTF8STRC("Sami (Lappish)"), 0},
	{0x043C, UTF8STRC("gd"),    UTF8STRC("Scottish Gaelic (United Kingdom)"), 0},
	{0x043D, UTF8STRC("yi"),    UTF8STRC("Yiddish"), 0},
	{0x043E, UTF8STRC("ms-my"), UTF8STRC("Malay - Malaysia"), 0},
	{0x043F, UTF8STRC(""),      UTF8STRC("Kazakh"), 0},
	{0x0440, UTF8STRC(""),      UTF8STRC("Kyrgyz (Cyrillic)"), 0},
	{0x0441, UTF8STRC("sw"),    UTF8STRC("Swahili"), 0},
	{0x0443, UTF8STRC("uz-uz"), UTF8STRC("Uzbek (Latin)"), 0},
	{0x0444, UTF8STRC("tt"),    UTF8STRC("Tatar"), 0},
	{0x0449, UTF8STRC("ta"),    UTF8STRC("Tamil"), 0},
	{0x044A, UTF8STRC(""),      UTF8STRC("Telugu"), 0},
	{0x044E, UTF8STRC("mr"),    UTF8STRC("Marathi"), 0},
	{0x044F, UTF8STRC("sa"),    UTF8STRC("Sanskrit"), 0},
	{0x0800, UTF8STRC(""),      UTF8STRC("Default"), 0},
	{0x0801, UTF8STRC("ar-iq"), UTF8STRC("Arabic - Iraq"), 0},
	{0x0804, UTF8STRC("zh-cn"), UTF8STRC("Chinese - China"), 936},
	{0x0807, UTF8STRC("de-ch"), UTF8STRC("German - Switzerland"), 0},
	{0x0809, UTF8STRC("en-gb"), UTF8STRC("English - Great Britain"), 1252},
	{0x080A, UTF8STRC("es-mx"), UTF8STRC("Spanish - Mexico"), 0},
	{0x080C, UTF8STRC("fr-be"), UTF8STRC("French - Belgium"), 0},
	{0x0810, UTF8STRC("it-ch"), UTF8STRC("Italian - Switzerland"), 0},
	{0x0813, UTF8STRC("nl-be"), UTF8STRC("Dutch - Belgium"), 0},
	{0x0814, UTF8STRC("nn-no"), UTF8STRC("Norwegian - Nynorsk"), 0},
	{0x0816, UTF8STRC("pt-pt"), UTF8STRC("Portuguese - Portugal"), 0},
	{0x0818, UTF8STRC("ro-mo"), UTF8STRC("Romanian - Republic of Moldova"), 0},
	{0x0819, UTF8STRC("ru-mo"), UTF8STRC("Russian - Republic of Moldova"), 0},
	{0x081A, UTF8STRC("sr-sp"), UTF8STRC("Serbian - Latin"), 1250},
	{0x081D, UTF8STRC("sv-fi"), UTF8STRC("Swedish - Finland"), 0},
	{0x082C, UTF8STRC("az-az"), UTF8STRC("Azeri - Cyrillic"), 1251},
	{0x083C, UTF8STRC("gd-ie"), UTF8STRC("Irish - Ireland"), 0},
	{0x083E, UTF8STRC("ms-bn"), UTF8STRC("Malay (Brunei)"), 0},
	{0x0843, UTF8STRC("uz-uz"), UTF8STRC("Uzbek - Cyrillic"), 0},
	{0x0C01, UTF8STRC("ar-eg"), UTF8STRC("Arabic - Egypt"), 0},
	{0x0C04, UTF8STRC("zh-hk"), UTF8STRC("Chinese - Hong Kong SAR"), 950},
	{0x0C07, UTF8STRC("de-at"), UTF8STRC("German - Austria"), 0},
	{0x0C09, UTF8STRC("en-au"), UTF8STRC("English - Australia"), 1252},
	{0x0C0A, UTF8STRC("es-es"), UTF8STRC("Spanish - Spain (Modern)"), 0},
	{0x0C0C, UTF8STRC("fr-ca"), UTF8STRC("French - Canada"), 0},
	{0x0C1A, UTF8STRC("sr-sp"), UTF8STRC("Serbian - Cyrillic"), 1251},
	{0x1001, UTF8STRC("ar-ly"), UTF8STRC("Arabic - Libya"), 0},
	{0x1004, UTF8STRC("zh-sg"), UTF8STRC("Chinese - Singapore"), 950},
	{0x1007, UTF8STRC("de-lu"), UTF8STRC("German - Luxembourg"), 0},
	{0x1009, UTF8STRC("en-ca"), UTF8STRC("English - Canada"), 1252},
	{0x100A, UTF8STRC("es-gt"), UTF8STRC("Spanish - Guatemala"), 0},
	{0x100C, UTF8STRC("fr-ch"), UTF8STRC("French - Switzerland"), 0},
	{0x1401, UTF8STRC("ar-dz"), UTF8STRC("Arabic - Algeria"), 0},
	{0x1404, UTF8STRC("zh-mo"), UTF8STRC("Chinese - Macau SAR"), 950},
	{0x1407, UTF8STRC("de-li"), UTF8STRC("German - Liechtenstein"), 0},
	{0x1409, UTF8STRC("en-nz"), UTF8STRC("English - New Zealand"), 1252},
	{0x140A, UTF8STRC("es-cr"), UTF8STRC("Spanish - Costa Rica"), 0},
	{0x140C, UTF8STRC("fr-lu"), UTF8STRC("French - Luxembourg"), 0},
	{0x1801, UTF8STRC("ar-ma"), UTF8STRC("Arabic - Morocco"), 0},
	{0x1809, UTF8STRC("en-ie"), UTF8STRC("English - Ireland"), 1252},
	{0x180A, UTF8STRC("es-pa"), UTF8STRC("Spanish - Panama"), 0},
	{0x1C01, UTF8STRC("ar-tn"), UTF8STRC("Arabic - Tunisia"), 0},
	{0x1C09, UTF8STRC("en-za"), UTF8STRC("English - Southern Africa"), 1252},
	{0x1C0A, UTF8STRC("es-do"), UTF8STRC("Spanish - Dominican Republic"), 0},
	{0x2001, UTF8STRC("ar-om"), UTF8STRC("Arabic - Oman"), 0},
	{0x2009, UTF8STRC("en-jm"), UTF8STRC("English - Jamaica"), 1252},
	{0x200A, UTF8STRC("es-ve"), UTF8STRC("Spanish - Venezuela"), 0},
	{0x2401, UTF8STRC("ar-ye"), UTF8STRC("Arabic - Yemen"), 0},
	{0x2409, UTF8STRC("en-cb"), UTF8STRC("English - Caribbean"), 1252},
	{0x240A, UTF8STRC("es-co"), UTF8STRC("Spanish - Colombia"), 0},
	{0x2801, UTF8STRC("ar-sy"), UTF8STRC("Arabic - Syria"), 0},
	{0x2809, UTF8STRC("en-bz"), UTF8STRC("English - Belize"), 1252},
	{0x280A, UTF8STRC("es-pe"), UTF8STRC("Spanish - Peru"), 0},
	{0x2C01, UTF8STRC("ar-jo"), UTF8STRC("Arabic - Jordan"), 0},
	{0x2C09, UTF8STRC("en-tt"), UTF8STRC("English - Trinidad"), 1252},
	{0x2C0A, UTF8STRC("es-ar"), UTF8STRC("Spanish - Argentina"), 0},
	{0x3001, UTF8STRC("ar-lb"), UTF8STRC("Arabic - Lebanon"), 0},
	{0x3009, UTF8STRC("en-zw"), UTF8STRC("English - Zimbabwe"), 1252},
	{0x300A, UTF8STRC("es-ec"), UTF8STRC("Spanish - Ecuador"), 0},
	{0x3401, UTF8STRC("ar-kw"), UTF8STRC("Arabic - Kuwait"), 0},
	{0x3409, UTF8STRC("en-ph"), UTF8STRC("English - Phillippines"), 1252},
	{0x340A, UTF8STRC("es-cl"), UTF8STRC("Spanish - Chile"), 0},
	{0x3801, UTF8STRC("ar-ae"), UTF8STRC("Arabic - United Arab Emirates"), 0},
	{0x380A, UTF8STRC("es-uy"), UTF8STRC("Spanish - Uruguay"), 0},
	{0x3C01, UTF8STRC("ar-bh"), UTF8STRC("Arabic - Bahrain"), 0},
	{0x3C0A, UTF8STRC("es-py"), UTF8STRC("Spanish - Paraguay"), 0},
	{0x4001, UTF8STRC("ar-qa"), UTF8STRC("Arabic - Qatar"), 0},
	{0x4009, UTF8STRC("en-in"), UTF8STRC("English - India"), 1252},
	{0x400A, UTF8STRC("es-bo"), UTF8STRC("Spanish - Bolivia"), 0},
	{0x4409, UTF8STRC("en-my"), UTF8STRC("English - Malaysia"), 1252},
	{0x440A, UTF8STRC("es-sv"), UTF8STRC("Spanish - El Salvador"), 0},
	{0x4809, UTF8STRC("en-sg"), UTF8STRC("English - Singapore"), 1252},
	{0x480A, UTF8STRC("es-hn"), UTF8STRC("Spanish - Honduras"), 0},
	{0x4C0A, UTF8STRC("es-ni"), UTF8STRC("Spanish - Nicaragua"), 0},
	{0x500A, UTF8STRC("es-pr"), UTF8STRC("Spanish - Puerto Rico"), 0}};

Text::Locale::LocaleEntry *Text::Locale::GetLocaleEntry(UInt32 lcid)
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

Text::Locale::LocaleEntry *Text::Locale::GetLocaleEntryByCodePage(UInt32 codePage)
{
	UOSInt j = (sizeof(locales) / sizeof(locales[0]));
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
	UOSInt i = sizeof(locales) / sizeof(locales[0]);
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
