#include "Stdafx.h"
#include "Data/ICaseStringUTF8Map.hpp"
#include "Text/EncodingFactory.h"
#include "Text/MyStringW.h"
#if defined(_MSC_VER)
#include <windows.h>
#endif

#define MAX_SHORT_LEN 45

// http://www.iana.org/assignments/character-sets
Text::EncodingFactory::EncodingInfo Text::EncodingFactory::encInfo[] = {
	{  037,                  "IBM037", "IBM EBCDIC US-Canada", {0}},
	{  437,                  "IBM437", "OEM United States", {0}},
	{  500,                  "IBM500", "IBM EBCDIC International", {0}},
	{  708,                "ASMO-708", "Arabic (ASMO 708)", {0}},
	{  709,                         0, "Arabic (ASMO-449+, BCON V4)", {0}},
	{  710,                         0, "Arabic - Transparent Arabic", {0}},
	{  720,                 "DOS-720", "Arabic (Transparent ASMO); Arabic (DOS)", {0}},
	{  737,                  "ibm737", "OEM Greek (formerly 437G); Greek (DOS)", {0}},
	{  775,                  "ibm775", "OEM Baltic; Baltic (DOS)", {0}},
	{  850,                  "ibm850", "OEM Multilingual Latin 1; Western European (DOS)", {0}},
	{  852,                  "ibm852", "OEM Latin 2; Central European (DOS)", {0}},
	{  855,                  "IBM855", "OEM Cyrillic (primarily Russian)", {0}},
	{  857,                  "ibm857", "OEM Turkish; Turkish (DOS)", {0}},
	{  858,                "IBM00858", "OEM Multilingual Latin 1 + Euro symbol", {0}},
	{  860,                  "IBM860", "OEM Portuguese; Portuguese (DOS)", {0}},
	{  861,                  "ibm861", "OEM Icelandic; Icelandic (DOS)", {0}},
	{  862,                 "DOS-862", "OEM Hebrew; Hebrew (DOS)", {0}},
	{  863,                  "IBM863", "OEM French Canadian; French Canadian (DOS)", {0}},
	{  864,                  "IBM864", "OEM Arabic; Arabic (864)", {0}},
	{  865,                  "IBM865", "OEM Nordic; Nordic (DOS)", {0}},
	{  866,                   "cp866", "OEM Russian; Cyrillic (DOS)", {0}},
	{  869,                  "ibm869", "OEM Modern Greek; Greek, Modern (DOS)", {0}},
	{  870,                  "IBM870", "IBM EBCDIC Multilingual/ROECE (Latin 2); IBM EBCDIC Multilingual Latin 2", {0}},
	{  874,             "windows-874", "ANSI/OEM Thai (same as 28605, ISO 8859-15); Thai (Windows)", {0}},
	{  875,                   "cp875", "IBM EBCDIC Greek Modern", {0}},
	{  932,               "shift_jis", "ANSI/OEM Japanese; Japanese (Shift-JIS)",				{"Shift_JIS", "MS_Kanji", "csShiftJIS", 0}},
	{  936,                  "gb2312", "ANSI/OEM Simplified Chinese (PRC, Singapore); Chinese Simplified (GB2312)", {"GB2312", "csGB2312", 0}},
	{  949,          "ks_c_5601-1987", "ANSI/OEM Korean (Unified Hangul Code)", {0}},
	{  950,                    "big5", "ANSI/OEM Traditional Chinese (Taiwan; Hong Kong SAR, PRC); Chinese Traditional (Big5)", {"Big5", "csBig5", "Big5-HKSCS", 0}},
	{ 1026,                 "IBM1026", "IBM EBCDIC Turkish (Latin 5)", {0}},
	{ 1047,                "IBM01047", "IBM EBCDIC Latin 1/Open System", {0}},
	{ 1140,                "IBM01140", "IBM EBCDIC US-Canada (037 + Euro symbol); IBM EBCDIC (US-Canada-Euro)", {0}},
	{ 1141,                "IBM01141", "IBM EBCDIC Germany (20273 + Euro symbol); IBM EBCDIC (Germany-Euro)", {0}},
	{ 1142,                "IBM01142", "IBM EBCDIC Denmark-Norway (20277 + Euro symbol); IBM EBCDIC (Denmark-Norway-Euro)", {0}},
	{ 1143,                "IBM01143", "IBM EBCDIC Finland-Sweden (20278 + Euro symbol); IBM EBCDIC (Finland-Sweden-Euro)", {0}},
	{ 1144,                "IBM01144", "IBM EBCDIC Italy (20280 + Euro symbol); IBM EBCDIC (Italy-Euro)", {0}},
	{ 1145,                "IBM01145", "IBM EBCDIC Latin America-Spain (20284 + Euro symbol); IBM EBCDIC (Spain-Euro)", {0}},
	{ 1146,                "IBM01146", "IBM EBCDIC United Kingdom (20285 + Euro symbol); IBM EBCDIC (UK-Euro)", {0}},
	{ 1147,                "IBM01147", "IBM EBCDIC France (20297 + Euro symbol); IBM EBCDIC (France-Euro)", {0}},
	{ 1148,                "IBM01148", "IBM EBCDIC International (500 + Euro symbol); IBM EBCDIC (International-Euro)", {0}},
	{ 1149,                "IBM01149", "IBM EBCDIC Icelandic (20871 + Euro symbol); IBM EBCDIC (Icelandic-Euro)", {0}},
	{ 1200,                  "utf-16", "Unicode UTF-16, little endian byte order (BMP of ISO 10646); available only to managed applications", {"UTF-16", "UTF-16LE", 0}},
	{ 1201,             "unicodeFFFE", "Unicode UTF-16, big endian byte order; available only to managed applications", {"UTF-16BE", 0}},
	{ 1250,            "windows-1250", "ANSI Central European; Central European (Windows)", {0}},
	{ 1251,            "windows-1251", "ANSI Cyrillic; Cyrillic (Windows)", {0}},
	{ 1252,            "windows-1252", "ANSI Latin 1; Western European (Windows)", {0}},
	{ 1253,            "windows-1253", "ANSI Greek; Greek (Windows)", {0}},
	{ 1254,            "windows-1254", "ANSI Turkish; Turkish (Windows)", {0}},
	{ 1255,            "windows-1255", "ANSI Hebrew; Hebrew (Windows)", {0}},
	{ 1256,            "windows-1256", "ANSI Arabic; Arabic (Windows)", {0}},
	{ 1257,            "windows-1257", "ANSI Baltic; Baltic (Windows)", {0}},
	{ 1258,            "windows-1258", "ANSI/OEM Vietnamese; Vietnamese (Windows)", {0}},
	{ 1361,                   "Johab", "Korean (Johab)", {0}},
	{10000,               "macintosh", "MAC Roman; Western European (Mac)",					{"macintosh", "csMacintosh", 0}},
	{10001,          "x-mac-japanese", "Japanese (Mac)", {0}},
	{10002,       "x-mac-chinesetrad", "MAC Traditional Chinese (Big5); Chinese Traditional (Mac)", {0}},
	{10003,            "x-mac-korean", "Korean (Mac)", {0}},
	{10004,            "x-mac-arabic", "Arabic (Mac)", {0}},
	{10005,            "x-mac-hebrew", "Hebrew (Mac)", {0}},
	{10006,             "x-mac-greek", "Greek (Mac)", {0}},
	{10007,          "x-mac-cyrillic", "Cyrillic (Mac)", {0}},
	{10008,       "x-mac-chinesesimp", "MAC Simplified Chinese (GB 2312); Chinese Simplified (Mac)", {0}},
	{10010,          "x-mac-romanian", "Romanian (Mac)", {0}},
	{10017,         "x-mac-ukrainian", "Ukrainian (Mac)", {0}},
	{10021,              "x-mac-thai", "Thai (Mac)", {0}},
	{10029,                "x-mac-ce", "MAC Latin 2; Central European (Mac)", {0}},
	{10079,         "x-mac-icelandic", "Icelandic (Mac)", {0}},
	{10081,           "x-mac-turkish", "Turkish (Mac)", {0}},
	{10082,          "x-mac-croatian", "Croatian (Mac)", {0}},
	{12000,                  "utf-32", "Unicode UTF-32, little endian byte order; available only to managed applications", {"UTF-32LE", "UTF-32", 0}},
	{12001,                "utf-32BE", "Unicode UTF-32, big endian byte order; available only to managed applications", {"UTF-32BE", 0}},
	{20000,           "x-Chinese_CNS", "CNS Taiwan; Chinese Traditional (CNS)", {0}},
	{20001,               "x-cp20001", "TCA Taiwan", {0}},
	{20002,          "x_Chinese-Eten", "Eten Taiwan; Chinese Traditional (Eten)", {0}},
	{20003,               "x-cp20003", "IBM5550 Taiwan", {0}},
	{20004,               "x-cp20004", "TeleText Taiwan", {0}},
	{20005,               "x-cp20005", "Wang Taiwan", {0}},
	{20105,                   "x-IA5", "IA5 (IRV International Alphabet No. 5, 7-bit); Western European (IA5)", {0}},
	{20106,            "x-IA5-German", "IA5 German (7-bit)", {0}},
	{20107,           "x-IA5-Swedish", "IA5 Swedish (7-bit)", {0}},
	{20108,         "x-IA5-Norwegian", "IA5 Norwegian (7-bit)", {0}},
	{20127,                "us-ascii", "US-ASCII (7-bit)",									{"US-ASCII", "ANSI_X3.4-1968", "iso-ir-6", "ANSI_X3.4-1986", "ISO_646.irv:1991", "ASCII", "ISO646-US", "us", "IBM367", "cp367", "csASCII", 0}},
	{20261,               "x-cp20261", "T.61", {0}},
	{20269,               "x-cp20269", "ISO 6937 Non-Spacing Accent", {0}},
	{20273,                  "IBM273", "IBM EBCDIC Germany", {0}},
	{20277,                  "IBM277", "IBM EBCDIC Denmark-Norway", {0}},
	{20278,                  "IBM278", "IBM EBCDIC Finland-Sweden", {0}},
	{20280,                  "IBM280", "IBM EBCDIC Italy", {0}},
	{20284,                  "IBM284", "IBM EBCDIC Latin America-Spain", {0}},
	{20285,                  "IBM285", "IBM EBCDIC United Kingdom", {0}},
	{20290,                  "IBM290", "IBM EBCDIC Japanese Katakana Extended", {0}},
	{20297,                  "IBM297", "IBM EBCDIC France", {0}},
	{20420,                  "IBM420", "IBM EBCDIC Arabic", {0}},
	{20423,                  "IBM423", "IBM EBCDIC Greek", {0}},
	{20424,                  "IBM424", "IBM EBCDIC Hebrew", {0}},
	{20833, "x-EBCDIC-KoreanExtended", "IBM EBCDIC Korean Extended", {0}},
	{20838,                "IBM-Thai", "IBM EBCDIC Thai", {0}},
	{20866,                  "koi8-r", "Russian (KOI8-R); Cyrillic (KOI8-R)", {0}},
	{20871,                  "IBM871", "IBM EBCDIC Icelandic", {0}},
	{20880,                  "IBM880", "IBM EBCDIC Cyrillic Russian", {0}},
	{20905,                  "IBM905", "IBM EBCDIC Turkish", {0}},
	{20924,                "IBM00924", "IBM EBCDIC Latin 1/Open System (1047 + Euro symbol)", {0}},
	{20932,                  "EUC-JP", "Japanese (JIS 0208-1990 and 0212-1990)",				{"EUC-JP", "Extended_UNIX_Code_Packed_Format_for_Japanese", "csEUCPkdFmtJapanese", 0}},
	{20936,               "x-cp20936", "Simplified Chinese (GB2312); Chinese Simplified (GB2312-80)", {0}},
	{20949,               "x-cp20949", "Korean Wansung", {0}},
	{21025,                  "cp1025", "IBM EBCDIC Cyrillic Serbian-Bulgarian", {0}},
	{21027,                         0, "(deprecated)", {0}},
	{21866,                  "koi8-u", "Ukrainian (KOI8-U); Cyrillic (KOI8-U)", {0}},
	{28591,              "iso-8859-1", "ISO 8859-1 Latin 1; Western European (ISO)",            {"ISO-8859-1", "ISO_8859-1:1987", "iso-ir-100", "ISO_8859-1", "latin1", "l1", "IBM819", "CP819", "csISOLatin1", 0}},
	{28592,              "iso-8859-2", "ISO 8859-2 Central European; Central European (ISO)",   {"ISO-8859-2", "ISO_8859-2:1987", "iso-ir-101", "ISO_8859-2", "latin2", "l2", "csISOLatin2", 0}},
	{28593,              "iso-8859-3", "ISO 8859-3 Latin 3",                                    {"ISO-8859-3", "ISO_8859-3:1988", "iso-ir-109", "ISO_8859-3", "latin3", "l3", "csISOLatin3", 0}},
	{28594,              "iso-8859-4", "ISO 8859-4 Baltic",                                     {"ISO-8859-4", "ISO_8859-4:1988", "iso-ir-110", "ISO_8859-4", "latin4", "l4", "csISOLatin4", 0}},
	{28595,              "iso-8859-5", "ISO 8859-5 Cyrillic",                                   {"ISO-8859-5", "ISO_8859-5:1988", "iso-ir-144", "ISO_8859-5", "cyrillic", "csISOLatinCyrillic", 0}},
	{28596,              "iso-8859-6", "ISO 8859-6 Arabic",                                     {"ISO-8859-6", "ISO_8859-6:1987", "iso-ir-127", "ISO_8859-6", "ECMA-114", "ASMO-708", "arabic", "csISOLatinArabic", 0}},
	{28597,              "iso-8859-7", "ISO 8859-7 Greek",                                      {"ISO-8859-7", "ISO_8859-7:1987", "iso-ir-126", "ISO_8859-7", "ELOT_928", "ECMA-118", "greek", "greek8", "csISOLatinGreek", 0}},
	{28598,              "iso-8859-8", "ISO 8859-8 Hebrew; Hebrew (ISO-Visual)",				{"ISO-8859-8", "ISO_8859-8:1988", "iso-ir-138", "ISO_8859-8", "hebrew", "csISOLatinHebrew", 0}},
	{28599,              "iso-8859-9", "ISO 8859-9 Turkish",									{"ISO-8859-9", "ISO_8859-9:1989", "iso-ir-148", "ISO_8859-9", "latin5", "l5", "csISOLatin5", 0}},
	{28603,             "iso-8859-13", "ISO 8859-13 Estonian", {0}},
	{28605,             "iso-8859-15", "ISO 8859-15 Latin 9", {0}},
	{29001,                "x-Europa", "Europa 3", {0}},
	{38598,            "iso-8859-8-i", "ISO 8859-8 Hebrew; Hebrew (ISO-Logical)", {0}},
	{50220,             "iso-2022-jp", "ISO 2022 Japanese with no halfwidth Katakana; Japanese (JIS)", {"JIS_Encoding", "csJISEncoding", 0}},
	{50221,             "csISO2022JP", "ISO 2022 Japanese with halfwidth Katakana; Japanese (JIS-Allow 1 byte Kana)", {0}},
	{50222,             "iso-2022-jp", "ISO 2022 Japanese JIS X 0201-1989; Japanese (JIS-Allow 1 byte Kana - SO/SI)", {"ISO-2022-JP", "csISO2022JP", 0}},
	{50225,             "iso-2022-kr", "ISO 2022 Korean",										{"ISO-2022-KR", "csISO2022KR", 0}},
	{50227,               "x-cp50227", "ISO 2022 Simplified Chinese; Chinese Simplified (ISO 2022)", {0}},
	{50229,                         0, "ISO 2022 Traditional Chinese", {0}},
	{50930,                         0, "EBCDIC Japanese (Katakana) Extended", {0}},
	{50931,                         0, "EBCDIC US-Canada and Japanese", {0}},
	{50933,                         0, "EBCDIC Korean Extended and Korean", {0}},
	{50935,                         0, "EBCDIC Simplified Chinese Extended and Simplified Chinese", {0}},
	{50936,                         0, "EBCDIC Simplified Chinese", {0}},
	{50937,                         0, "EBCDIC US-Canada and Traditional Chinese", {0}},
	{50939,                         0, "EBCDIC Japanese (Latin) Extended and Japanese", {0}},
	{51932,	      	                0, "EUC Japanese", {0}},
	{51936,                  "EUC-CN", "EUC Simplified Chinese; Chinese Simplified (EUC)", {0}},
	{51949,                  "euc-kr", "EUC Korean",											{"EUC-KR", "csEUCKR", 0}},
	{51950,                         0, "EUC Traditional Chinese", {0}},
	{52936,              "hz-gb-2312", "HZ-GB2312 Simplified Chinese; Chinese Simplified (HZ)", {0}},
	{54936,                 "GB18030", "GB18030 Simplified Chinese (4 byte); Chinese Simplified (GB18030)", {0}},
	{57002,              "x-iscii-de", "ISCII Devanagari", {0}},
	{57003,              "x-iscii-be", "ISCII Bengali", {0}},
	{57004,              "x-iscii-ta", "ISCII Tamil", {0}},
	{57005,              "x-iscii-te", "ISCII Telugu", {0}},
	{57006,              "x-iscii-as", "ISCII Assamese", {0}},
	{57007,              "x-iscii-or", "ISCII Oriya", {0}},
	{57008,              "x-iscii-ka", "ISCII Kannada", {0}},
	{57009,              "x-iscii-ma", "ISCII Malayalam", {0}},
	{57010,              "x-iscii-gu", "ISCII Gujarati", {0}},
	{57011,              "x-iscii-pa", "ISCII Punjabi", {0}},
	{65000,                   "utf-7", "Unicode (UTF-7)",										{"UTF-7", 0}},
	{65001,                   "utf-8", "Unicode (UTF-8)",										{"UTF-8", 0}}
};

/*
ISO-8859-10
ISO_6937-2-add
JIS_X0201
Extended_UNIX_Code_Fixed_Width_for_Japanese
BS_4730
SEN_850200_C
IT
ES
DIN_66003
NS_4551-1
NF_Z_62-010
ISO-10646-UTF-1
ISO_646.basic:1983
INVARIANT
ISO_646.irv:1983
NATS-SEFI
NATS-SEFI-ADD
NATS-DANO
NATS-DANO-ADD
SEN_850200_B
KS_C_5601-1987
ISO-2022-JP-2
JIS_C6220-1969-jp
JIS_C6220-1969-ro
PT
greek7-old
latin-greek

*/

Text::EncodingFactory::EncodingFactory()
{
	UOSInt i = (sizeof(encInfo) / sizeof(encInfo[0]));
	UOSInt j;
	NN<Text::String> s;
	UOSInt len;
	while (i-- > 0)
	{
		j = 0;
		while (j < 11)
		{
			if (encInfo[i].internetNames[j])
			{
				len = Text::StrCharCntCh(encInfo[i].internetNames[j]);
				s = Text::String::New(len);
				Text::StrToLowerC(s->v, (const UTF8Char*)encInfo[i].internetNames[j], len);
				this->encMap.PutNN(s, encInfo[i]);
				s->Release();
			}
			else
			{
				break;
			}
			j++;
		}
	}
}

Text::EncodingFactory::~EncodingFactory()
{
}

UInt32 Text::EncodingFactory::GetCodePage(Text::CStringNN shortName)
{
	if (shortName.leng > MAX_SHORT_LEN)
	{
		return 0;
	}
	UTF8Char sbuff[MAX_SHORT_LEN + 1];
	Text::StrToLowerC(sbuff, shortName.v, shortName.leng);
	NN<Text::EncodingFactory::EncodingInfo> encInfo;
	if (this->encMap.GetC({sbuff, shortName.leng}).SetTo(encInfo))
	{
		return encInfo->codePage;
	}
	else
	{
		return 0;
	}
}

UnsafeArray<UTF8Char> Text::EncodingFactory::GetName(UnsafeArray<UTF8Char> buff, UInt32 codePage)
{
	OSInt i = 0;
	OSInt j = (OSInt)(sizeof(encInfo) / sizeof(encInfo[0])) - 1;
	OSInt k;
	UInt32 l;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = encInfo[k].codePage;
		if (codePage > l)
		{
			i = k + 1;
		}
		else if (codePage < l)
		{
			j = k - 1;
		}
		else
		{
			return Text::StrConcat(buff, (const UTF8Char*)encInfo[k].desc);
		}
	}
	return Text::StrConcatC(buff, UTF8STRC("Unknown"));
}

UnsafeArray<UTF8Char> Text::EncodingFactory::GetInternetName(UnsafeArray<UTF8Char> buff, UInt32 codePage)
{
	OSInt i = 0;
	OSInt j = (OSInt)(sizeof(encInfo) / sizeof(encInfo[0])) - 1;
	OSInt k;
	UInt32 l;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = encInfo[k].codePage;
		if (codePage > l)
		{
			i = k + 1;
		}
		else if (codePage < l)
		{
			j = k - 1;
		}
		else
		{
			if (encInfo[k].internetNames[0])
				return Text::StrConcat(buff, (const UTF8Char*)encInfo[k].internetNames[0]);
			break;
		}
	}
	return Text::StrConcatC(buff, UTF8STRC("UTF-8"));
}

UnsafeArray<UTF8Char> Text::EncodingFactory::GetDotNetName(UnsafeArray<UTF8Char> buff, UInt32 codePage)
{
	OSInt i = 0;
	OSInt j = (OSInt)(sizeof(encInfo) / sizeof(encInfo[0])) - 1;
	OSInt k;
	UInt32 l;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = encInfo[k].codePage;
		if (codePage > l)
		{
			i = k + 1;
		}
		else if (codePage < l)
		{
			j = k - 1;
		}
		else
		{
			if (encInfo[k].dotNetName)
				return Text::StrConcat(buff, (const UTF8Char*)encInfo[k].dotNetName);
			break;
		}
	}
	return Text::StrConcatC(buff, UTF8STRC("UTF-8"));
}

#ifdef _MSC_VER
UInt32 Text::EncodingFactory::GetSystemCodePage()
{
	return GetACP();
}

UInt32 Text::EncodingFactory::GetSystemLCID()
{
	return GetUserDefaultLCID();
}

Data::ArrayList<UInt32> *EncodingFactory_codePageResult;
Int32 __stdcall EncodingFactory_CodePageResult(WChar *codePageName);

void Text::EncodingFactory::GetCodePages(NN<Data::ArrayList<UInt32>> codePages)
{
	EncodingFactory_codePageResult = codePages.Ptr();
	EnumSystemCodePagesW(EncodingFactory_CodePageResult, CP_INSTALLED);
}

Int32 __stdcall EncodingFactory_CodePageResult(WChar *codePageName)
{
	EncodingFactory_codePageResult->Add(Text::StrToInt32W((const WChar*)codePageName));
	return 1;
}
#else

UInt32 Text::EncodingFactory::GetSystemCodePage()
{
	return 65001;
}

UInt32 Text::EncodingFactory::GetSystemLCID()
{
	return 0x0409;
}

void Text::EncodingFactory::GetCodePages(NN<Data::ArrayList<UInt32>> codePages)
{
	codePages->Add(1200);
	codePages->Add(1201);
	codePages->Add(65001);
}
#endif
