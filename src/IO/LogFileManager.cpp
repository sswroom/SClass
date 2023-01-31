#include "Stdafx.h"
#include "IO/LogFileManager.h"
#include "IO/Path.h"

IO::LogFileManager::LogFileManager(Text::String *logPath)
{
	this->logPath = logPath->Clone();
}

IO::LogFileManager::~LogFileManager()
{
	this->logPath->Release();
}

void IO::LogFileManager::QueryLogMonths(Data::ArrayList<UInt32> *months)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = this->logPath->ConcatTo(sbuff);
	sptr = Text::StrConcatC(sptr, UTF8STRC("??????"));
	IO::Path::FindFileSession *sess = IO::Path::FindFile(CSTRP(sbuff, sptr));
	if (sess)
	{
		IO::Path::PathType pt;
		while ((sptr = IO::Path::FindNextFile(sbuff, sess, 0, &pt, 0)) != 0)
		{
			if (pt == IO::Path::PathType::Directory)
			{
				UInt32 month = Text::StrToUInt32(&sptr[-6]);
				if (month >= 200000)
				{
					months->Add(month);
				}
			}
		}
		IO::Path::FindFileClose(sess);
	}
}
