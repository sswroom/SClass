#include "Stdafx.h"
#include "IO/ProgramLinkManager.h"

UOSInt IO::ProgramLinkManager::GetLinkNamesDir(Data::ArrayListStringNN *nameList, UnsafeArray<UTF8Char> linkPath, UnsafeArray<UTF8Char> linkPathEnd, UnsafeArray<UTF8Char> filePath, UnsafeArray<UTF8Char> filePathEnd)
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

Bool IO::ProgramLinkManager::GetLinkDetail(Text::CStringNN linkName, IO::ProgramLink *link)
{
	return false;
}

Bool IO::ProgramLinkManager::CreateLink(Bool thisUser, Text::CStringNN shortName, Text::CStringNN linkName, Text::CString comment, Text::CString categories, Text::CStringNN cmdLine)
{
	return false;
}

Bool IO::ProgramLinkManager::DeleteLink(Text::CStringNN linkName)
{
	return false;
}
