#include "Stdafx.h"
#include "IO/LNKFile.h"
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

UTF8Char *IO::ProgramLinkManager::GetLinkPath(UTF8Char *buff, Bool thisUser)
{
	UTF8Char *sptr;	
	if (thisUser)
	{
		sptr = IO::Path::GetUserHome(buff);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		sptr = Text::StrConcatC(sptr, UTF8STRC("AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs"));
	}
	else
	{
		sptr = Manage::EnvironmentVar::GetEnvValue(buff, CSTR("ProgramData"));
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		sptr = Text::StrConcatC(sptr, UTF8STRC("\\Microsoft\\Windows\\Start Menu\\Programs"));
	}
	return sptr;
}

UOSInt IO::ProgramLinkManager::GetLinkNames(Data::ArrayList<Text::String*> *nameList, Bool allUser, Bool thisUser)
{
	UTF8Char linkPath[512];
	UTF8Char *linkPathEnd;
	UTF8Char filePath[512];
	UOSInt ret = 0;
	if (allUser)
	{
		linkPathEnd = GetLinkPath(linkPath, false);
		ret += GetLinkNamesDir(nameList, linkPath, linkPathEnd, filePath, filePath);
	}
	if (allUser)
	{
		linkPathEnd = GetLinkPath(linkPath, true);
		*filePath = '*';
		ret += GetLinkNamesDir(nameList, linkPath, linkPathEnd, filePath, filePath + 1);
	}
	return ret;
}

Bool IO::ProgramLinkManager::GetLinkDetail(Text::CString linkName, IO::ProgramLink *link)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	if (linkName.v[0] == '*')
	{
		sptr = this->GetLinkPath(sbuff, this);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = linkName.Substring(1).ConcatTo(sptr);
	}
	else
	{
		sptr = this->GetLinkPath(sbuff, false);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = linkName.ConcatTo(sptr);
	}
	IO::LNKFile lnk(CSTRP(sbuff, sptr));
	if (lnk.IsError())
		return false;
	if ((sptr = lnk.GetNameString(sbuff)) != 0)
		link->SetComment(CSTRP(sbuff, sptr));
	if ((sptr = lnk.GetLocalBasePath(sbuff)) != 0 || (sptr = lnk.GetRelativePath(sbuff)) != 0)
	{
		sptr2 = sptr;
		*sptr2++ = ' ';
		sptr2 = lnk.GetCommandLineArguments(sptr2);
		if (sptr2)
		{
			link->SetCmdLine(CSTRP(sbuff, sptr2));
		}
		else
		{
			*sptr = 0;
			link->SetCmdLine(CSTRP(sbuff, sptr));
		}
	}
	if ((sptr = lnk.GetIconLocation(sbuff)) != 0)
		link->SetIcon(CSTRP(sbuff, sptr));
	return true;
}

Bool IO::ProgramLinkManager::CreateLink(Bool thisUser, Text::CString shortName, Text::CString linkName, Text::CString comment, Text::CString categories, Text::CString cmdLine)
{
	return false;
}

Bool IO::ProgramLinkManager::DeleteLink(Text::CString linkName)
{
	return false;
}
