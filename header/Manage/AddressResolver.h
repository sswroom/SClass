#ifndef _SM_MANAGE_ADDRESSRESOLVER
#define _SM_MANAGE_ADDRESSRESOLVER
#include "Text/StringBuilderUTF8.h"

namespace Manage
{
	class AddressResolver
	{
	public:
		AddressResolver();
		virtual ~AddressResolver();

		virtual UTF8Char *ResolveName(UTF8Char *buff, UInt64 address) = 0;
		Bool ResolveNameSB(NotNullPtr<Text::StringBuilderUTF8> sb, UInt64 address);
	};
}
#endif
