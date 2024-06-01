#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/ProgramLinkManager.h"
#include "Text/UTF8Reader.h"

#include <stdio.h>

UOSInt IO::ProgramLinkManager::GetLinkNamesDir(Data::ArrayListStringNN *nameList, UnsafeArray<UTF8Char> linkPath, UnsafeArray<UTF8Char> linkPathEnd, UnsafeArray<UTF8Char> filePath, UnsafeArray<UTF8Char> filePathEnd)
{
	UOSInt ret = 0;
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	IO::Path::FindFileSession *sess;
	*linkPathEnd++ = '/';
	sptr = Text::StrConcatC(linkPathEnd, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	sess = IO::Path::FindFile(CSTRP(linkPath, sptr));
	if (sess)
	{
		IO::Path::PathType pt;
		while (IO::Path::FindNextFile(linkPathEnd, sess, 0, &pt, 0).SetTo(sptr))
		{
			if (pt == IO::Path::PathType::File)
			{
				if (Text::StrEndsWithC(linkPathEnd, (UOSInt)(sptr - linkPathEnd), UTF8STRC(".desktop")))
				{
					sptr2 = Text::StrConcatC(filePathEnd, linkPathEnd, (UOSInt)(sptr - linkPathEnd));
					nameList->Add(Text::String::NewP(filePath, sptr2));
					ret++;
				}
			}
			else if (pt == IO::Path::PathType::Directory)
			{
				if (linkPathEnd[0] != '.')
				{
					sptr2 = Text::StrConcatC(filePathEnd, linkPathEnd, (UOSInt)(sptr - linkPathEnd));
					*sptr2++ = '/';
					ret += GetLinkNamesDir(nameList, linkPath, sptr, filePath, sptr2);
				}
			}
		}
		IO::Path::FindFileClose(sess);
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
		sptr = IO::Path::AppendPath(buff, sptr, CSTR(".local/share/applications"));
	}
	else
	{
		sptr = Text::StrConcatC(buff, UTF8STRC("/usr/share/applications"));
	}
	return sptr;
}

UOSInt IO::ProgramLinkManager::GetLinkNames(Data::ArrayListStringNN *nameList, Bool allUser, Bool thisUser)
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

Bool IO::ProgramLinkManager::GetLinkDetail(Text::CStringNN linkName, IO::ProgramLink *link)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	if (linkName.v[0] == '*')
	{
		sptr = IO::Path::GetUserHome(sbuff).Or(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR(".local/share/applications/"));
		sptr = linkName.Substring(1).ConcatTo(sptr);
	}
	else
	{
		sptr = Text::StrConcatC(sbuff, UTF8STRC("/usr/share/applications/"));
		sptr = linkName.ConcatTo(sptr);
	}
	{
		IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (fs.IsError())
		{
			return false;
		}
		Text::UTF8Reader reader(fs);
		Text::StringBuilderUTF8 sb;
		if (!reader.ReadLine(sb, 1024))
		{
			return false;
		}
		if (!sb.Equals(UTF8STRC("[Desktop Entry]")))
		{
			printf("ProgramLinkManager: Desktop Entry not found\r\n");
			return false;
		}
		sb.ClearStr();
		while (reader.ReadLine(sb, 1024))
		{
			if (sb.GetLength() == 0)
			{

			}
			else
			{
				UOSInt i = sb.IndexOf('=');
				if (i == INVALID_INDEX)
				{
					printf("ProgramLinkManager: Unknown line: %s\r\n", sb.ToPtr());
				}
				else
				{
					Text::PString name = sb;
					Text::PString val = sb.Substring(i + 1);
					name.TrimToLength(i);
					if (name.Equals(UTF8STRC("Exec")))
					{
						link->SetCmdLine(val.ToCString());
					}
					else if (name.Equals(UTF8STRC("Name")))
					{
						link->SetName(val.ToCString());
					}
					else if (name.Equals(UTF8STRC("NoDisplay")))
					{
						link->SetNoDisplay(val.Equals(UTF8STRC("true")));
					}
					else if (name.Equals(UTF8STRC("StartupNotify")))
					{
						link->SetStartupNotify(val.Equals(UTF8STRC("true")));
					}
					else if (name.Equals(UTF8STRC("Type")))
					{
						link->SetType(val.ToCString());
					}
					else if (name.Equals(UTF8STRC("MimeType")))
					{
						link->SetMimeTypes(val.ToCString());
					}
					else if (name.Equals(UTF8STRC("Icon")))
					{
						link->SetIcon(val.ToCString());
					}
					else if (name.Equals(UTF8STRC("GenericName")))
					{
						link->SetGenericName(val.ToCString());
					}
					else if (name.Equals(UTF8STRC("Comment")))
					{
						link->SetComment(val.ToCString());
					}
					else if (name.Equals(UTF8STRC("Version")))
					{
						link->SetVersion(val.ToCString());
					}
					else if (name.Equals(UTF8STRC("Categories")))
					{
						link->SetCategories(val.ToCString());
					}
					else if (name.Equals(UTF8STRC("Keywords")))
					{
						link->SetKeywords(val.ToCString());
					}
					else if (name.Equals(UTF8STRC("Terminal")))
					{
						link->SetTerminal(val.Equals(UTF8STRC("true")));
					}
					else if (name.StartsWith(UTF8STRC("GenericName[")) && name.EndsWith(']'))
					{
						if (name.Equals(UTF8STRC("GenericName[en]")))
						{
							link->SetGenericName(val.ToCString());
						}
					}
					else if (name.StartsWith(UTF8STRC("Comment[")) && name.EndsWith(']'))
					{
						if (name.Equals(UTF8STRC("Comment[en]")))
						{
							link->SetComment(val.ToCString());
						}
					}
					else if (name.StartsWith(UTF8STRC("Keywords[")) && name.EndsWith(']'))
					{
						if (name.Equals(UTF8STRC("Keywords[en]")))
						{
							link->SetKeywords(val.ToCString());
						}
					}
					else if (name.StartsWith(UTF8STRC("Name[")) && name.EndsWith(']'))
					{
						if (name.Equals(UTF8STRC("Name[en]")))
						{
							link->SetName(val.ToCString());
						}
					}
					else
					{
						printf("ProgramLinkManager: Unknown Item: %s=%s\r\n", name.v.Ptr(), val.v.Ptr());
					}
				}
			}
			sb.ClearStr();
		}
		return true;
	}
}

Bool IO::ProgramLinkManager::CreateLink(Bool thisUser, Text::CStringNN shortName, Text::CStringNN linkName, Text::CString comment, Text::CString categories, Text::CStringNN cmdLine)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	Text::CStringNN nns;
	sptr = GetLinkPath(sbuff, thisUser);
	*sptr++ = '/';
	sptr = shortName.ConcatTo(sptr);
	sptr = Text::StrConcatC(sptr, UTF8STRC(".desktop"));
	if (IO::Path::GetPathType(CSTRP(sbuff, sptr)) != IO::Path::PathType::Unknown)
		return false;
	IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
		return false;
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("[Desktop Entry]"));
	sb.AppendC(UTF8STRC("\r\nName="));
	sb.Append(linkName);
	if (comment.SetTo(nns) && nns.leng > 0)
	{
		sb.AppendC(UTF8STRC("\r\nComment="));
		sb.Append(nns);
	}
	sb.AppendC(UTF8STRC("\r\nType=Appplication"));
	if (categories.SetTo(nns) && nns.leng > 0)
	{
		sb.AppendC(UTF8STRC("\r\nCategories="));
		sb.Append(nns);
	}
	sb.AppendC(UTF8STRC("\r\nExec="));
	sb.Append(cmdLine);
	sb.AppendC(UTF8STRC("\r\n"));
	fs.Write(sb.ToString(), sb.GetLength());
	return true;
}

Bool IO::ProgramLinkManager::DeleteLink(Text::CStringNN linkName)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	if (linkName.v[0] == '*')
	{
		sptr = GetLinkPath(sbuff, true);
		*sptr++ = '/';
		sptr = linkName.Substring(1).ConcatTo(sptr);
	}
	else
	{
		sptr = GetLinkPath(sbuff, false);
		*sptr++ = '/';
		sptr = linkName.ConcatTo(sptr);
	}
	if (IO::Path::GetPathType(CSTRP(sbuff, sptr)) == IO::Path::PathType::File)
	{
		return IO::Path::DeleteFile(sbuff);
	}
	return false;
}
