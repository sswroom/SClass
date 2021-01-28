#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/AddressResolver.h"
Manage::AddressResolver::AddressResolver()
{
}

Manage::AddressResolver::~AddressResolver()
{
}

Bool Manage::AddressResolver::ResolveNameSB(Text::StringBuilderUTF *sb, UInt64 address)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr = this->ResolveName(sbuff, address);
	if (sptr)
	{
		sb->Append(sbuff);
		return true;
	}
	else
	{
		return false;
	}
}
