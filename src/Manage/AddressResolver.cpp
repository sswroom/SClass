#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/AddressResolver.h"
Manage::AddressResolver::AddressResolver()
{
}

Manage::AddressResolver::~AddressResolver()
{
}

Bool Manage::AddressResolver::ResolveNameSB(NN<Text::StringBuilderUTF8> sb, UInt64 address)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr = this->ResolveName(sbuff, address);
	if (sptr)
	{
		sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
		return true;
	}
	else
	{
		return false;
	}
}
