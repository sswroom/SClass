#include "Stdafx.h"
#include "IO/LNKFile.h"
#include "IO/Path.h"
#include "IO/ProgramLinkManager.h"
#include "Manage/EnvironmentVar.h"
#include "Text/MyStringW.h"
#include <windows.h>
#include <shobjidl.h>
#include <shlguid.h>
#undef FindNextFile
#undef DeleteFile

UOSInt IO::ProgramLinkManager::GetLinkNamesDir(NN<Data::ArrayListStringNN> nameList, UnsafeArray<UTF8Char> linkPath, UnsafeArray<UTF8Char> linkPathEnd, UnsafeArray<UTF8Char> filePath, UnsafeArray<UTF8Char> filePathEnd)
{
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	UOSInt ret = 0;
	NN<IO::Path::FindFileSession> sess;
	*linkPathEnd++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcatC(linkPathEnd, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	if (IO::Path::FindFile(CSTRP(linkPath, sptr)).SetTo(sess))
	{
		IO::Path::PathType pt;
		while (IO::Path::FindNextFile(linkPathEnd, sess, 0, pt, 0).SetTo(sptr))
		{
			if (linkPathEnd[0] != '.')
			{
				if (pt == IO::Path::PathType::Directory)
				{
					sptr2 = Text::StrConcatC(filePathEnd, linkPathEnd, (UOSInt)(sptr - linkPathEnd));
					*sptr2++ = IO::Path::PATH_SEPERATOR;
					ret += GetLinkNamesDir(nameList, linkPath, sptr, filePath, sptr2);
				}
				else
				{
					if (Text::StrEndsWithICaseC(linkPathEnd, (UOSInt)(sptr - linkPathEnd), UTF8STRC(".LNK")))
					{
						sptr2 = Text::StrConcatC(filePathEnd, linkPathEnd, (UOSInt)(sptr - linkPathEnd));
						nameList->Add(Text::String::NewP(filePath, sptr2));
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

UnsafeArray<UTF8Char> IO::ProgramLinkManager::GetLinkPath(UnsafeArray<UTF8Char> buff, Bool thisUser)
{
	UnsafeArray<UTF8Char> sptr;	
	if (thisUser)
	{
		sptr = IO::Path::GetUserHome(buff).Or(buff);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		sptr = Text::StrConcatC(sptr, UTF8STRC("AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs"));
	}
	else
	{
		sptr = Manage::EnvironmentVar::GetEnvValue(buff, CSTR("ProgramData")).Or(buff);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		sptr = Text::StrConcatC(sptr, UTF8STRC("\\Microsoft\\Windows\\Start Menu\\Programs"));
	}
	return sptr;
}

UOSInt IO::ProgramLinkManager::GetLinkNames(NN<Data::ArrayListStringNN> nameList, Bool allUser, Bool thisUser)
{
	UTF8Char linkPath[512];
	UnsafeArray<UTF8Char> linkPathEnd;
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

Bool IO::ProgramLinkManager::GetLinkDetail(Text::CStringNN linkName, NN<IO::ProgramLink> link)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
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
	UOSInt i = linkName.LastIndexOf(IO::Path::PATH_SEPERATOR);
	if (i != INVALID_INDEX)
	{
		sptr = linkName.Substring(i + 1).ConcatTo(sbuff);
	}
	else if (linkName.v[0] == '*')
	{
		sptr = linkName.Substring(1).ConcatTo(sbuff);
	}
	else
	{
		sptr = linkName.ConcatTo(sbuff);
	}
	i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '.');
	if (i != INVALID_INDEX)
	{
		sbuff[i] = 0;
		sptr = &sbuff[i];
	}
	link->SetName(CSTRP(sbuff, sptr));
	if (lnk.GetNameString(sbuff).SetTo(sptr))
		link->SetComment(CSTRP(sbuff, sptr));
	if (lnk.GetTarget(sbuff).SetTo(sptr) || lnk.GetLocalBasePath(sbuff).SetTo(sptr) || lnk.GetRelativePath(sbuff).SetTo(sptr))
	{
		sptr2 = sptr;
		*sptr2++ = ' ';
		if (lnk.GetCommandLineArguments(sptr2).SetTo(sptr2))
		{
			link->SetCmdLine(CSTRP(sbuff, sptr2));
		}
		else
		{
			*sptr = 0;
			link->SetCmdLine(CSTRP(sbuff, sptr));
		}
	}
	if (lnk.GetIconLocation(sbuff).SetTo(sptr))
		link->SetIcon(CSTRP(sbuff, sptr));
	return true;
}

Bool IO::ProgramLinkManager::CreateLink(Bool thisUser, Text::CStringNN shortName, Text::CStringNN linkName, Text::CString comment, Text::CString categories, Text::CStringNN cmdLine)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	HRESULT hres;
	IShellLink* psl;
	UnsafeArray<const WChar> wptr;
	UOSInt i;

	hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
	if (SUCCEEDED(hres))
	{
		Bool succ = false;
		IPersistFile* ppf;

		if (cmdLine.v[0] == '"')
		{
			i = cmdLine.IndexOf('"', 1);
			if (i != INVALID_INDEX)
			{
				sptr = Text::StrConcatC(sbuff, &cmdLine.v[1], i - 1);
				i++;
			}
			else
			{
				sptr = cmdLine.ConcatTo(sbuff);
			}
		}
		else
		{
			i = cmdLine.IndexOf(' ');
			if (i == INVALID_INDEX)
			{
				sptr = cmdLine.ConcatTo(sbuff);
			}
			else
			{
				sptr = Text::StrConcatC(sbuff, cmdLine.v, i);
			}
		}
		wptr = Text::StrToWCharNew(sbuff);
		psl->SetPath(wptr.Ptr());
		Text::StrDelNew(wptr);

		if (i != INVALID_INDEX)
		{
			while (i < cmdLine.leng)
			{
				if (cmdLine.v[i] != ' ')
					break;
				i++;
			}
			if (i < cmdLine.leng)
			{
				wptr = Text::StrToWCharNew(&cmdLine.v[i]);
				psl->SetArguments(wptr.Ptr());
				Text::StrDelNew(wptr);
			}
		}
		Text::CStringNN nncomment;
		if (comment.SetTo(nncomment) && nncomment.leng > 0)
		{
			wptr = Text::StrToWCharNew(nncomment.v);
			psl->SetDescription(wptr.Ptr());
			Text::StrDelNew(wptr);
		}

		hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);

		if (SUCCEEDED(hres))
		{
			WCHAR wsz[MAX_PATH];
			sptr = GetLinkPath(sbuff, thisUser);
			*sptr++ = IO::Path::PATH_SEPERATOR;
			sptr = Text::StrConcatC(linkName.ConcatTo(sptr), UTF8STRC(".lnk"));
			Text::StrUTF8_WChar(wsz, sbuff, 0);

			hres = ppf->Save(wsz, TRUE);
			ppf->Release();
			if (SUCCEEDED(hres))
			{
				succ = true;
			}
		}
		psl->Release();
		return succ;
	}
	return false;
}

Bool IO::ProgramLinkManager::DeleteLink(Text::CStringNN linkName)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
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
	if (linkName.IndexOf(UTF8STRC("..")) != INVALID_INDEX)
		return false;
	return IO::Path::DeleteFile(sbuff);
}
