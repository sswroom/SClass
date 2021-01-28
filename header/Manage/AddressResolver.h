#ifndef _SM_MANAGE_ADDRESSRESOLVER
#define _SM_MANAGE_ADDRESSRESOLVER
#include "Text/StringBuilderUTF.h"

namespace Manage
{
	class AddressResolver
	{
	public:
		AddressResolver();
		virtual ~AddressResolver();

		virtual UTF8Char *ResolveName(UTF8Char *buff, UInt64 address) = 0;
		Bool ResolveNameSB(Text::StringBuilderUTF *sb, UInt64 address);
	};
};
#endif
