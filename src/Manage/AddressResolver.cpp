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
	UnsafeArray<UTF8Char> sptr;
	if (this->ResolveName(sbuff, address).SetTo(sptr))
	{
		sb->AppendC(sbuff, (UIntOS)(sptr - sbuff));
		return true;
	}
	else
	{
		return false;
	}
}
