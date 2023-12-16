#include "Stdafx.h"
#include "IO/ProgramLinkManager.h"

UOSInt IO::ProgramLinkManager::GetLinkNamesDir(Data::ArrayListStringNN *nameList, UTF8Char *linkPath, UTF8Char *linkPathEnd, UTF8Char *filePath, UTF8Char *filePathEnd)
{
	return 0;
}

IO::ProgramLinkManager::ProgramLinkManager()
{
}

IO::ProgramLinkManager::~ProgramLinkManager()
{
}

UOSInt IO::ProgramLinkManager::GetLinkNames(Data::ArrayListStringNN *nameList, Bool allUser, Bool thisUser)
{
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
