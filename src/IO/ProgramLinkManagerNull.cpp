#include "Stdafx.h"
#include "IO/ProgramLinkManager.h"

UOSInt IO::ProgramLinkManager::GetLinkNamesDir(Data::ArrayList<Text::String*> *nameList, UTF8Char *linkPath, UTF8Char *linkPathEnd, UTF8Char *filePath, UTF8Char *filePathEnd)
{
	return 0;
}

IO::ProgramLinkManager::ProgramLinkManager()
{
}

IO::ProgramLinkManager::~ProgramLinkManager()
{
}

UOSInt IO::ProgramLinkManager::GetLinkNames(Data::ArrayList<Text::String*> *nameList, Bool allUser, Bool thisUser)
{
	return 0;
}

Bool IO::ProgramLinkManager::GetLinkDetail(Text::CString linkName, IO::ProgramLink *link)
{
	return false;
}
