#ifndef _SM_TEXT_CPPTEXT
#define _SM_TEXT_CPPTEXT
#include "Data/ArrayListStringNN.h"
#include "Data/VariItem.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace Text
{
	class CPPText
	{
	public:
		static void ToCPPString(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UTF8Char> str);
		static void ToCPPString(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UTF8Char> str, UOSInt leng);
		static void FromCPPString(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UTF8Char> str);

		static Bool ParseEnum(Data::ArrayListStringNN *enumEntries, Text::CStringNN cppEnumStr, NN<Text::StringBuilderUTF8> sbPrefix);
		static Text::CStringNN GetCppType(Data::VariItem::ItemType itemType, Bool notNull);
	};
}
#endif
