#include "Stdafx.h"
#include "Manage/Computer.h"
#include <unistd.h>

UTF8Char *Manage::Computer::GetHostName(UTF8Char *sbuff)
{
	int len = gethostname((Char*)sbuff, 256);
	return &sbuff[len];
}
