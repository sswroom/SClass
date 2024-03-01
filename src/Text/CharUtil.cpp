#include "Stdafx.h"
#include "Text/CharUtil.h"

Text::CharUtil::UnicodeBlock Text::CharUtil::ublk[] = {
	{0x0000, 0x007F, false, "C0 Controls and Basic Latin"},
	{0x0080, 0x00FF, false, "C1 Controls and Latin-1 Supplement"},
	{0x0100, 0x017F, false, "Latin Extended-A"},
	{0x0180, 0x024F, false, "Latin Extended-B"},
	{0x0250, 0x02AF, false, "IPA Extensions"},
	{0x02B0, 0x02FF, false, "Spacing Modifier Letters"},
	{0x0300, 0x036F, false, "Combining Diacritical Marks"},
	{0x0370, 0x03FF, false, "Greek and Coptic"},
	{0x0400, 0x04FF, false, "Cyrillic"},
	{0x0500, 0x052F, false, "Cyrillic Supplement"},
	{0x0530, 0x058F, false, "Armenian"},
	{0x0590, 0x05FF, false, "Hebrew"},
	{0x0600, 0x06FF, false, "Arabic"},
	{0x0700, 0x074F, false, "Syriac"},
	{0x0750, 0x077F, false, "Arabic Supplement"},
	{0x0780, 0x07BF, false, "Thaana"},
	{0x07C0, 0x07FF, false, "NKo"},
	{0x0800, 0x083F, false, "Samaritan"},
	{0x0840, 0x085F, false, "Mandaic"},
	{0x0860, 0x086F, false, "Syriac Supplement"},
	{0x0870, 0x089F, false, "Arabic Extended-B"},
	{0x08A0, 0x08FF, false, "Arabic Extended-A"},
	{0x0900, 0x097F, false, "Devanagari"},
	{0x0980, 0x09FF, false, "Bengali"},
	{0x0A00, 0x0A7F, false, "Gurmukhi"},
	{0x0A80, 0x0AFF, false, "Gujarati"},
	{0x0B00, 0x0B7F, false, "Oriya"},
	{0x0B80, 0x0BFF, false, "Tamil"},
	{0x0C00, 0x0C7F, false, "Telugu"},
	{0x0C80, 0x0CFF, false, "Kannada"},
	{0x0D00, 0x0D7F, false, "Malayalam"},
	{0x0D80, 0x0DFF, false, "Sinhala"},
	{0x0E00, 0x0E7F, false, "Thai"},
	{0x0E80, 0x0EFF, false, "Lao"},
	{0x0F00, 0x0FFF, false, "Tibetan"},
	{0x1000, 0x109F, false, "Myanmar"},
	{0x10A0, 0x10FF, false, "Georgian"},
	{0x1100, 0x11FF, false, "Hangul Jamo"},
	{0x1200, 0x137F, false, "Ethiopic"},
	{0x1380, 0x139F, false, "Ethiopic Supplement"},
	{0x13A0, 0x13FF, false, "Cherokee"},
	{0x1400, 0x167F, false, "Unified Canadian Aboriginal Syllabics"},
	{0x1680, 0x169F, false, "Ogham"},
	{0x16A0, 0x16FF, false, "Runic"},
	{0x1700, 0x171F, false, "Tagalog"},
	{0x1720, 0x173F, false, "Hanunoo"},
	{0x1740, 0x175F, false, "Buhid"},
	{0x1760, 0x177F, false, "Tagbanwa"},
	{0x1780, 0x17FF, false, "Khmer"},
	{0x1800, 0x18AF, false, "Mongolian"},
	{0x18B0, 0x18FF, false, "Unified Canadian Aboriginal Syllabics Extended"},
	{0x1900, 0x194F, false, "Limbu"},
	{0x1950, 0x197F, false, "Tai Le"},
	{0x1980, 0x19DF, false, "New Tai Lue"},
	{0x19E0, 0x19FF, false, "Khmer Symbols"},
	{0x1A00, 0x1A1F, false, "Buginese"},
	{0x1A20, 0x1AAF, false, "Tai Tham"},
	{0x1AB0, 0x1AFF, false, "Combining Diacritical Marks Extended"},
	{0x1B00, 0x1B7F, false, "Balinese"},
	{0x1B80, 0x1BBF, false, "Sundanese"},
	{0x1BC0, 0x1BFF, false, "Batak"},
	{0x1C00, 0x1C4F, false, "Lepcha"},
	{0x1C50, 0x1C7F, false, "Ol Chiki"},
	{0x1C80, 0x1C8F, false, "Cyrillic Extended-C"},
	{0x1C90, 0x1CBF, false, "Georgian Extended"},
	{0x1CC0, 0x1CCF, false, "Sundanese Supplement"},
	{0x1CD0, 0x1CFF, false, "Vedic Extensions"},
	{0x1D00, 0x1D7F, false, "Phonetic Extensions"},
	{0x1D80, 0x1DBF, false, "Phonetic Extensions Supplement"},
	{0x1DC0, 0x1DFF, false, "Combining Diacritical Marks Supplement"},
	{0x1E00, 0x1EFF, false, "Latin Extended Additional"},
	{0x1F00, 0x1FFF, false, "Greek Extended"},
	{0x2000, 0x206F, false, "General Punctuation"},
	{0x2070, 0x209F, false, "Superscripts and Subscripts"},
	{0x20A0, 0x20CF, false, "Currency Symbols"},
	{0x20D0, 0x20FF, false, "Combining Diacritical Marks for Symbols"},
	{0x2100, 0x214F, false, "Letterlike Symbols"},
	{0x2150, 0x218F, false, "Number Forms"},
	{0x2190, 0x21FF, false, "Arrows"},
	{0x2200, 0x22FF, false, "Mathematical Operators"},
	{0x2300, 0x23FF, false, "Miscellaneous Technical"},
	{0x2400, 0x243F, false, "Control Pictures"},
	{0x2440, 0x245F, false, "Optical Character Recognition"},
	{0x2460, 0x24FF, false, "Enclosed Alphanumerics"},
	{0x2500, 0x257F, false, "Box Drawing"},
	{0x2580, 0x259F, false, "Block Elements"},
	{0x25A0, 0x25FF, false, "Geometric Shapes"},
	{0x2600, 0x26FF, false, "Miscellaneous Symbols"},
	{0x2700, 0x27BF, false, "Dingbats"},
	{0x27C0, 0x27EF, false, "Miscellaneous Mathematical Symbols-A"},
	{0x27F0, 0x27FF, false, "Supplemental Arrows-A"},
	{0x2800, 0x28FF, false, "Braille Patterns"},
	{0x2900, 0x297F, false, "Supplemental Arrows-B"},
	{0x2980, 0x29FF, false, "Miscellaneous Mathematical Symbols-B"},
	{0x2A00, 0x2AFF, false, "Supplemental Mathematical Operators"},
	{0x2B00, 0x2BFF, false, "Miscellaneous Symbols and Arrows"},
	{0x2C00, 0x2C5F, false, "Glagolitic"},
	{0x2C60, 0x2C7F, false, "Latin Extended-C"},
	{0x2C80, 0x2CFF, false, "Coptic"},
	{0x2D00, 0x2D2F, false, "Georgian Supplement"},
	{0x2D30, 0x2D7F, false, "Tifinagh"},
	{0x2D80, 0x2DDF, false, "Ethiopic Extended"},
	{0x2DE0, 0x2DFF, false, "Cyrillic Extended-A"},
	{0x2E00, 0x2E7F, false, "Supplemental Punctuation"},
	{0x2E80, 0x2EFF, true,  "CJK Radicals Supplement"},
	{0x2F00, 0x2FDF, true,  "Kangxi Radicals"},
	{0x2FF0, 0x2FFF, true,  "Ideographic Description Characters"},
	{0x3000, 0x303F, true,  "CJK Symbols and Punctuation"},
	{0x3040, 0x309F, true,  "Hiragana"},
	{0x30A0, 0x30FF, true,  "Katakana"},
	{0x3100, 0x312F, true,  "Bopomofo"},
	{0x3130, 0x318F, true,  "Hangul Compatibility Jamo"},
	{0x3190, 0x319F, true,  "Kanbun"},
	{0x31A0, 0x31BF, true,  "Bopomofo Extended"},
	{0x31C0, 0x31EF, true,  "CJK Strokes"},
	{0x31F0, 0x31FF, true,  "Katakana Phonetic Extensions"},
	{0x3200, 0x32FF, true,  "Enclosed CJK Letters and Months"},
	{0x3300, 0x33FF, true,  "CJK Compatibility"},
	{0x3400, 0x4DBF, true,  "CJK Unified Ideographs Extension A"},
	{0x4DC0, 0x4DFF, false, "Yijing Hexagram Symbols"},
	{0x4E00, 0x9FFF, true,  "CJK Unified Ideographs"},
	{0xA000, 0xA48F, false, "Yi Syllables"},
	{0xA490, 0xA4CF, false, "Yi Radicals"},
	{0xA4D0, 0xA4FF, false, "Lisu"},
	{0xA500, 0xA63F, false, "Vai"},
	{0xA640, 0xA69F, false, "Cyrillic Extended-B"},
	{0xA6A0, 0xA6FF, false, "Bamum"},
	{0xA700, 0xA71F, false, "Modifier Tone Letters"},
	{0xA720, 0xA7FF, false, "Latin Extended-D"},
	{0xA800, 0xA82F, false, "Syloti Nagri"},
	{0xA830, 0xA83F, false, "Common Indic Number Forms"},
	{0xA840, 0xA87F, false, "Phags-pa"},
	{0xA880, 0xA8DF, false, "Saurashtra"},
	{0xA8E0, 0xA8FF, false, "Devanagari Extended"},
	{0xA900, 0xA92F, false, "Kayah Li"},
	{0xA930, 0xA95F, false, "Rejang"},
	{0xA960, 0xA97F, false, "Hangul Jamo Extended-A"},
	{0xA980, 0xA9DF, false, "Javanese"},
	{0xA9E0, 0xA9FF, false, "Myanmar Extended-B"},
	{0xAA00, 0xAA5F, false, "Cham"},
	{0xAA60, 0xAA7F, false, "Myanmar Extended-A"},
	{0xAA80, 0xAADF, false, "Tai Viet"},
	{0xAAE0, 0xAAFF, false, "Meetei Mayek Extensions"},
	{0xAB00, 0xAB2F, false, "Ethiopic Extended-A"},
	{0xAB30, 0xAB6F, false, "Latin Extended-E"},
	{0xAB70, 0xABBF, false, "Cherokee Supplement"},
	{0xABC0, 0xABFF, false, "Meetei Mayek"},
	{0xAC00, 0xD7AF, true,  "Hangul Syllables"},
	{0xD7B0, 0xD7FF, true,  "Hangul Jamo Extended-B"},
	{0xD800, 0xDB7F, false, "High Surrogates"},
	{0xDB80, 0xDBFF, false, "High Private Use Surrogates"},
	{0xDC00, 0xDFFF, false, "Low Surrogates"},
	{0xE000, 0xF8FF, false, "Private Use Area"},
	{0xF900, 0xFAFF, true,  "CJK Compatibility Ideographs"},
	{0xFB00, 0xFB4F, false, "Alphabetic Presentation Forms"},
	{0xFB50, 0xFDFF, false, "Arabic Presentation Forms-A"},
	{0xFE00, 0xFE0F, false, "Variation Selectors"},
	{0xFE10, 0xFE1F, true,  "Vertical Forms"},
	{0xFE20, 0xFE2F, false, "Combining Half Marks"},
	{0xFE30, 0xFE4F, true,  "CJK Compatibility Forms"},
	{0xFE50, 0xFE6F, true,  "Small Form Variants"},
	{0xFE70, 0xFEFF, false, "Arabic Presentation Forms-B"},
	{0xFF00, 0xFFEF, false, "Halfwidth and Fullwidth Forms"},
	{0xFFF0, 0xFFFF, false, "Specials"},
	{0x10000, 0x1007F, false, "Linear B Syllabary"},
	{0x10080, 0x100FF, false, "Linear B Ideograms"},
	{0x10100, 0x1013F, false, "Aegean Numbers"},
	{0x10140, 0x1018F, false, "Ancient Greek Numbers"},
	{0x10190, 0x101CF, false, "Ancient Symbols"},
	{0x101D0, 0x101FF, false, "Phaistos Disc"},
	{0x10280, 0x1029F, false, "Lycian"},
	{0x102A0, 0x102DF, false, "Carian"},
	{0x102E0, 0x102FF, false, "Coptic Epact Numbers"},
	{0x10300, 0x1032F, false, "Old Italic"},
	{0x10330, 0x1034F, false, "Gothic"},
	{0x10350, 0x1037F, false, "Old Permic"},
	{0x10380, 0x1039F, false, "Ugaritic"},
	{0x103A0, 0x103DF, false, "Old Persian"},
	{0x10400, 0x1044F, false, "Deseret"},
	{0x10450, 0x1047F, false, "Shavian"},
	{0x10480, 0x104AF, false, "Osmanya"},
	{0x104B0, 0x104FF, false, "Osage"},
	{0x10500, 0x1052F, false, "Elbasan"},
	{0x10530, 0x1056F, false, "Caucasian Albanian"},
	{0x10570, 0x105BF, false, "Vithkuqi"},
	{0x10600, 0x1077F, false, "Linear A"},
	{0x10780, 0x107BF, false, "Latin Extended-F"},
	{0x10800, 0x1083F, false, "Cypriot Syllabary"},
	{0x10840, 0x1085F, false, "Imperial Aramaic"},
	{0x10860, 0x1087F, false, "Palmyrene"},
	{0x10880, 0x108AF, false, "Nabataean"},
	{0x108E0, 0x108FF, false, "Hatran"},
	{0x10900, 0x1091F, false, "Phoenician"},
	{0x10920, 0x1093F, false, "Lydian"},
	{0x10980, 0x1099F, false, "Meroitic Hieroglyphs"},
	{0x109A0, 0x109FF, false, "Meroitic Cursive"},
	{0x10A00, 0x10A5F, false, "Kharoshthi"},
	{0x10A60, 0x10A7F, false, "Old South Arabian"},
	{0x10A80, 0x10A9F, false, "Old North Arabian"},
	{0x10AC0, 0x10AFF, false, "Manichaean"},
	{0x10B00, 0x10B3F, false, "Avestan"},
	{0x10B40, 0x10B5F, false, "Inscriptional Parthian"},
	{0x10B60, 0x10B7F, false, "Inscriptional Pahlavi"},
	{0x10B80, 0x10BAF, false, "Psalter Pahlavi"},
	{0x10C00, 0x10C4F, false, "Old Turkic"},
	{0x10C80, 0x10CFF, false, "Old Hungarian"},
	{0x10D00, 0x10D3F, false, "Hanifi Rohingya"},
	{0x10E60, 0x10E7F, false, "Rumi Numeral Symbols"},
	{0x10E80, 0x10EBF, false, "Yezidi"},
	{0x10F00, 0x10F2F, false, "Old Sogdian"},
	{0x10F30, 0x10F6F, false, "Sogdian"},
	{0x10F70, 0x10FAF, false, "Old Uyghur"},
	{0x10FB0, 0x10FDF, false, "Chorasmian"},
	{0x10FE0, 0x10FFF, false, "Elymaic"},
	{0x11000, 0x1107F, false, "Brahmi"},
	{0x11080, 0x110CF, false, "Kaithi"},
	{0x110D0, 0x110FF, false, "Sora Sompeng"},
	{0x11100, 0x1114F, false, "Chakma"},
	{0x11150, 0x1117F, false, "Mahajani"},
	{0x11180, 0x111DF, false, "Sharada"},
	{0x111E0, 0x111FF, false, "Sinhala Archaic Numbers"},
	{0x11200, 0x1124F, false, "Khojki"},
	{0x11280, 0x112AF, false, "Multani"},
	{0x112B0, 0x112FF, false, "Khudawadi"},
	{0x11300, 0x1137F, false, "Grantha"},
	{0x11400, 0x1147F, false, "Newa"},
	{0x11480, 0x114DF, false, "Tirhuta"},
	{0x11580, 0x115FF, false, "Siddham"},
	{0x11600, 0x1165F, false, "Modi"},
	{0x11660, 0x1167F, false, "Mongolian Supplement"},
	{0x11680, 0x116CF, false, "Takri"},
	{0x11700, 0x1174F, false, "Ahom"},
	{0x11800, 0x1184F, false, "Dogra"},
	{0x118A0, 0x118FF, false, "Warang Citi"},
	{0x11900, 0x1195F, false, "Dives Akuru"},
	{0x119A0, 0x119FF, false, "Nandinagari"},
	{0x11A00, 0x11A4F, false, "Zanabazar Square"},
	{0x11A50, 0x11AAF, false, "Soyombo"},
	{0x11AB0, 0x11ABF, false, "Unified Canadian Aboriginal Syllabics Extended-A"},
	{0x11AC0, 0x11AFF, false, "Pau Cin Hau"},
	{0x11C00, 0x11C6F, false, "Bhaiksuki"},
	{0x11C70, 0x11CBF, false, "Marchen"},
	{0x11D00, 0x11D5F, false, "Masaram Gondi"},
	{0x11D60, 0x11DAF, false, "Gunjala Gondi"},
	{0x11EE0, 0x11EFF, false, "Makasar"},
	{0x11FB0, 0x11FBF, false, "Lisu Supplement"},
	{0x11FC0, 0x11FFF, false, "Tamil Supplement"},
	{0x12000, 0x123FF, false, "Cuneiform"},
	{0x12400, 0x1247F, false, "Cuneiform Numbers and Punctuation"},
	{0x12480, 0x1254F, false, "Early Dynastic Cuneiform"},
	{0x12F90, 0x12FFF, false, "Cypro-Minoan"},
	{0x13000, 0x1342F, false, "Egyptian Hieroglyphs"},
	{0x13430, 0x1343F, false, "Egyptian Hieroglyph Format Controls"},
	{0x14400, 0x1467F, false, "Anatolian Hieroglyphs"},
	{0x16800, 0x16A3F, false, "Bamum Supplement"},
	{0x16A40, 0x16A6F, false, "Mro"},
	{0x16A70, 0x16ACF, false, "Tangsa"},
	{0x16AD0, 0x16AFF, false, "Bassa Vah"},
	{0x16B00, 0x16B8F, false, "Pahawh Hmong"},
	{0x16E40, 0x16E9F, false, "Medefaidrin"},
	{0x16F00, 0x16F9F, false, "Miao"},
	{0x16FE0, 0x16FFF, true,  "Ideographic Symbols and Punctuation"},
	{0x17000, 0x187FF, true,  "Tangut"},
	{0x18800, 0x18AFF, false, "Tangut Components"},
	{0x18B00, 0x18CFF, false, "Khitan Small Script"},
	{0x18D00, 0x18D7F, false, "Tangut Supplement"},
	{0x1AFF0, 0x1AFFF, false, "Kana Extended-B"},
	{0x1B000, 0x1B0FF, true,  "Kana Supplement"},
	{0x1B100, 0x1B12F, true,  "Kana Extended-A"},
	{0x1B130, 0x1B16F, false, "Small Kana Extension"},
	{0x1B170, 0x1B2FF, false, "Nushu"},
	{0x1BC00, 0x1BC9F, false, "Duployan"},
	{0x1BCA0, 0x1BCAF, false, "Shorthand Format Controls"},
	{0x1CF00, 0x1CFCF, false, "Znamenny Musical Notation"},
	{0x1D000, 0x1D0FF, false, "Byzantine Musical Symbols"},
	{0x1D100, 0x1D1FF, false, "Musical Symbols"},
	{0x1D200, 0x1D24F, false, "Ancient Greek Musical Notation"},
	{0x1D2E0, 0x1D2FF, false, "Mayan Numerals"},
	{0x1D300, 0x1D35F, true,  "Tai Xuan Jing Symbols"},
	{0x1D360, 0x1D37F, false, "Counting Rod Numerals"},
	{0x1D400, 0x1D7FF, false, "Mathematical Alphanumeric Symbols"},
	{0x1D800, 0x1DAAF, false, "Sutton SignWriting"},
	{0x1DF00, 0x1DFFF, false, "Latin Extended-G"},
	{0x1E000, 0x1E02F, false, "Glagolitic Supplement"},
	{0x1E100, 0x1E14F, false, "Nyiakeng Puachue Hmong"},
	{0x1E290, 0x1E2BF, false, "Toto"},
	{0x1E2C0, 0x1E2FF, false, "Wancho"},
	{0x1E7E0, 0x1E7FF, false, "Ethiopic Extended-B"},
	{0x1E800, 0x1E8DF, false, "Mende Kikakui"},
	{0x1E900, 0x1E95F, false, "Adlam"},
	{0x1EC70, 0x1ECBF, false, "Indic Siyaq Numbers"},
	{0x1ED00, 0x1ED4F, false, "Ottoman Siyaq Numbers"},
	{0x1EE00, 0x1EEFF, false, "Arabic Mathematical Alphabetic Symbols"},
	{0x1F000, 0x1F02F, false, "Mahjong Tiles"},
	{0x1F030, 0x1F09F, false, "Domino Tiles"},
	{0x1F0A0, 0x1F0FF, false, "Playing Cards"},
	{0x1F100, 0x1F1FF, false, "Enclosed Alphanumeric Supplement"},
	{0x1F200, 0x1F2FF, false, "Enclosed Ideographic Supplement"},
	{0x1F300, 0x1F5FF, false, "Miscellaneous Symbols and Pictographs"},
	{0x1F600, 0x1F64F, true,  "Emoticons"},
	{0x1F650, 0x1F67F, false, "Ornamental Dingbats"},
	{0x1F680, 0x1F6FF, false, "Transport and Map Symbols"},
	{0x1F700, 0x1F77F, false, "Alchemical Symbols"},
	{0x1F780, 0x1F7FF, false, "Geometric Shapes Extended"},
	{0x1F800, 0x1F8FF, false, "Supplemental Arrows-C"},
	{0x1F900, 0x1F9FF, false, "Supplemental Symbols and Pictographs"},
	{0x1FA00, 0x1FA6F, false, "Chess Symbols"},
	{0x1FA70, 0x1FAFF, false, "Symbols and Pictographs Extended-A"},
	{0x1FB00, 0x1FBFF, false, "Symbols for Legacy Computing"},
	{0x20000, 0x2A6DF, false, "CJK Unified Ideographs Extension B"},
	{0x2A700, 0x2B73F, false, "CJK Unified Ideographs Extension C"},
	{0x2B740, 0x2B81F, false, "CJK Unified Ideographs Extension D"},
	{0x2B820, 0x2CEAF, false, "CJK Unified Ideographs Extension E"},
	{0x2CEB0, 0x2EBEF, false, "CJK Unified Ideographs Extension F"},
	{0x2F800, 0x2FA1F, false, "CJK Compatibility Ideographs Supplement"},
	{0x30000, 0x3134F, false, "CJK Unified Ideographs Extension G"},
	{0xE0000, 0xE007F, false, "Tags"},
	{0xE0100, 0xE01EF, false, "Variation Selectors Supplement"},
	{0xF0000, 0xFFFFF, false, "Supplementary Private Use Area-A"},
	{0x100000, 0x10FFFF, false, "Supplementary Private Use Area-B"}
};

Bool Text::CharUtil::PtrIsWS(const UTF8Char **sptr)
{
	UTF8Char c = **sptr;
	switch (c)
	{
	case ' ':
	case '\t':
	case '\r':
	case '\n':
		++*sptr;
		return true;
	}
	return false;
}

Bool Text::CharUtil::IsWS(const UTF8Char *sptr)
{
	UTF8Char c = *sptr;
	switch (c)
	{
	case ' ':
	case '\t':
	case '\r':
	case '\n':
		return true;
	}
	return false;
}

Bool Text::CharUtil::IsPunctuation(UTF8Char c)
{
	switch (c)
	{
	case '[':
	case ']':
	case '-':
	case '\'':
	case ',':
	case '(':
	case ')':
		return true;
	default:
		return false;
	}
}

Bool Text::CharUtil::IsAlphaNumeric(UTF8Char c)
{
	return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

Bool Text::CharUtil::IsDigit(UTF8Char c)
{
	return (c >= '0' && c <= '9');
}

Bool Text::CharUtil::IsAlphabet(UTF8Char c)
{
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

Bool Text::CharUtil::UTF8CharValid(const UTF8Char *sptr)
{
	UTF8Char c = *sptr;
	UInt32 code;
	if (c < 0x80)
	{
		return true;
	}
	else if ((c & 0xE0) == 0xC0)
	{
		if ((sptr[1] & 0xC0) == 0x80)
		{
			code = (((UInt32)c & 0x1f) << 6) | (sptr[1] & 0x3f);
		}
		else
		{
			return false;
		}
	}
	else if ((c & 0xF0) == 0xE0)
	{
		if (((sptr[1] & 0xC0) == 0x80) && ((sptr[2] & 0xC0) == 0x80))
		{
			code = (((UInt32)c & 0xf) << 12) | (((UInt32)sptr[1] & 0x3f) << 6) | (sptr[2] & 0x3f);
		}
		else
		{
			return false;
		}
	}
	else if ((c & 0xF8) == 0xF0)
	{
		if (((sptr[1] & 0xC0) == 0x80) && ((sptr[2] & 0xC0) == 0x80) && ((sptr[3] & 0xC0) == 0x80))
		{
			code = (((UInt32)sptr[0] & 0x7) << 18) | (((UInt32)sptr[1] & 0x3f) << 12) | (((UInt32)sptr[2] & 0x3f) << 6) | (sptr[3] & 0x3f);
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	if (code >= 0x110000)
	{
		return false;
	}
	if (code >= 0xD800 && code < 0xE000)
	{
		return false;
	}
	return true;

/*	else if ((c & 0xFC) == 0xF8)
	{
		return ((sptr[1] & 0xC0) == 0x80) && ((sptr[2] & 0xC0) == 0x80) && ((sptr[3] & 0xC0) == 0x80) && ((sptr[4] & 0xC0) == 0x80);
	}
	else if ((c & 0xFE) == 0xFC)
	{
		return ((sptr[1] & 0xC0) == 0x80) && ((sptr[2] & 0xC0) == 0x80) && ((sptr[3] & 0xC0) == 0x80) && ((sptr[4] & 0xC0) == 0x80) && ((sptr[5] & 0xC0) == 0x80);
	}*/
	return false;
}

const Text::CharUtil::UnicodeBlock *Text::CharUtil::GetUnicodeBlock(UTF32Char c)
{
	UnicodeBlock *blk;
	OSInt i = 0;
	OSInt j = (sizeof(ublk) / sizeof(ublk[0])) - 1;
	OSInt k;
	while (i <= j)
	{
		k = (i + j) >> 1;
		blk = &ublk[k];
		if ((UInt32)c < blk->firstCode)
		{
			j = k - 1;
		}
		else if ((UInt32)c > blk->lastCode)
		{
			i = k + 1;
		}
		else
		{
			return blk;
		}
	}
	return 0;
}

Bool Text::CharUtil::IsDoubleSize(UTF32Char c)
{
	if (c == 0x3000)
	{
		return true;
	}
	if (c >= 0xff01 && c <= 0xff5e)
	{
		return true;
	}
	const UnicodeBlock *blk = GetUnicodeBlock(c);
	if (blk == 0)
	{
		return false;
	}
	return blk->dblWidth;
}

UTF8Char Text::CharUtil::ToUpper(UTF8Char c)
{
	if (c >= 'a' && c <= 'z')
	{
		return (UTF8Char)(c - 0x20);
	}
	else
	{
		return c;
	}
}

UTF8Char Text::CharUtil::ToLower(UTF8Char c)
{
	if (c >= 'A' && c <= 'Z')
	{
		return (UTF8Char)(c + 0x20);
	}
	else
	{
		return c;
	}
}

Bool Text::CharUtil::IsUpperCase(UTF8Char c)
{
	return c >= 'A' && c <= 'Z';
}

Bool Text::CharUtil::IsLowerCase(UTF8Char c)
{
	return c >= 'a' && c <= 'z';
}
