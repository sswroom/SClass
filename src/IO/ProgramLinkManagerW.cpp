#include "Stdafx.h"
#include "IO/Path.h"
#include "IO/ProgramLinkManager.h"
#include "Manage/EnvironmentVar.h"

UOSInt IO::ProgramLinkManager::GetLinkNamesDir(Data::ArrayList<Text::String*> *nameList, UTF8Char *linkPath, UTF8Char *linkPathEnd, UTF8Char *filePath, UTF8Char *filePathEnd)
{
	UTF8Char *sptr;
	UTF8Char* sptr2;
	UOSInt ret = 0;
	IO::Path::FindFileSession *sess;
	*filePathEnd++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcatC(filePathEnd, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	sess = IO::Path::FindFile(CSTRP(filePath, sptr));
	if (sess)
	{
		IO::Path::PathType pt;
		while ((sptr = IO::Path::FindNextFile(filePathEnd, sess, 0, &pt, 0)) != 0)
		{
			if (filePathEnd[0] != '.')
			{
				if (pt == IO::Path::PathType::Directory)
				{
					sptr2 = Text::StrConcatC(linkPathEnd, filePathEnd, (UOSInt)(sptr - filePathEnd));
					*sptr2++ = IO::Path::PATH_SEPERATOR;
					ret += GetLinkNamesDir(nameList, linkPath, sptr2, filePath, sptr);
				}
				else
				{
					if (Text::StrEndsWithICaseC(filePathEnd, (UOSInt)(sptr - filePathEnd), UTF8STRC(".LNK")))
					{
						sptr2 = Text::StrConcatC(linkPathEnd, filePathEnd, (UOSInt)(sptr - filePathEnd));
						nameList->Add(Text::String::NewP(linkPath, sptr2));
						ret++;
					}
				}
			}
		}
	}
	return ret;
}

IO::ProgramLinkManager::ProgramLinkManager()
{
}

IO::ProgramLinkManager::~ProgramLinkManager()
{
}

UOSInt IO::ProgramLinkManager::GetLinkNames(Data::ArrayList<Text::String*> *nameList, Bool allUser, Bool thisUser)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char path[512];
	UOSInt ret = 0;
	if (allUser)
	{
		sptr = Manage::EnvironmentVar::GetEnvValue(sbuff, CSTR("ProgramData"));
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		sptr = Text::StrConcatC(sptr, UTF8STRC("\\Microsoft\\Windows\\Start Menu\\Programs"));
		ret += GetLinkNamesDir(nameList, path, path, sbuff, sptr);
	}
	if (thisUser)
	{
		sptr = IO::Path::GetUserHome(sbuff);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		sptr = Text::StrConcatC(sptr, UTF8STRC("AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs"));
		path[0] = '*';
		ret += GetLinkNamesDir(nameList, path, path + 1, sbuff, sptr);
	}
	return 0;
}

Bool IO::ProgramLinkManager::GetLinkDetail(Text::CString linkName, IO::ProgramLink *link)
{
	return false;
}

Bool IO::ProgramLinkManager::CreateLink(Bool thisUser, Text::CString shortName, Text::CString linkName, Text::CString comment, Text::CString categories, Text::CString cmdLine)
{
	return false;
}

Bool IO::ProgramLinkManager::DeleteLink(Text::CString linkName)
{
	return false;
}
