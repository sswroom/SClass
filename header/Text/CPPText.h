#ifndef _SM_TEXT_CPPTEXT
#define _SM_TEXT_CPPTEXT
#include "Data/ArrayList.h"
#include "Data/VariItem.h"
#include "Text/StringBuilderUTF.h"

namespace Text
{
	class CPPText
	{
	public:
		static void ToCPPString(Text::StringBuilderUTF8 *sb, const UTF8Char *str);
		static void FromCPPString(Text::StringBuilderUTF8 *sb, const UTF8Char *str);

		static Bool ParseEnum(Data::ArrayList<const UTF8Char*> *enumEntries, const UTF8Char *cppEnumStr, Text::StringBuilderUTF8 *sbPrefix);
		static const UTF8Char *GetCppType(Data::VariItem::ItemType itemType);
	};
}
#endif
