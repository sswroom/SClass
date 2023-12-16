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
		static void ToCPPString(NotNullPtr<Text::StringBuilderUTF8> sb, const UTF8Char *str);
		static void ToCPPString(NotNullPtr<Text::StringBuilderUTF8> sb, const UTF8Char *str, UOSInt leng);
		static void FromCPPString(NotNullPtr<Text::StringBuilderUTF8> sb, const UTF8Char *str);

		static Bool ParseEnum(Data::ArrayListStringNN *enumEntries, Text::CString cppEnumStr, NotNullPtr<Text::StringBuilderUTF8> sbPrefix);
		static Text::CString GetCppType(Data::VariItem::ItemType itemType);
	};
}
#endif
