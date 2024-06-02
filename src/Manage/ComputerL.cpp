#include "Stdafx.h"
#include "Manage/Computer.h"
#include <unistd.h>

UnsafeArrayOpt<UTF8Char> Manage::Computer::GetHostName(UnsafeArray<UTF8Char> sbuff)
{
	int len = gethostname((Char*)sbuff.Ptr(), 256);
	return &sbuff[len];
}
