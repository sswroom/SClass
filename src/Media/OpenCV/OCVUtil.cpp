#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "Media/OpenCV/OCVUtil.h"
#include "Text/MyString.h"

const Char *OCVUtil_LinuxPaths[] = {
	"/usr/local/share/OpenCV",
	"/usr/share/OpenCV",
	"/usr/share/opencv",
	"/usr/share/opencv4",
	0
};

const Char *OCVUtil_WinPaths[] = {
	0
};

UTF8Char *OCVUtil_TestDataPath(UTF8Char *sbuff, const UTF8Char *basePath, const UTF8Char *dataFile)
{
	UTF8Char *sptr;
	sptr = Text::StrConcat(sbuff, basePath);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcat(sptr, dataFile);
	if (IO::Path::GetPathType(CSTRP(sbuff, sptr)) != IO::Path::PathType::Unknown)
	{
		return sptr;
	}
	return 0;
}

UTF8Char *Media::OpenCV::OCVUtil::GetDataPath(UTF8Char *sbuff, const UTF8Char *dataFile)
{
	const UTF8Char **paths;
	UTF8Char *sptr;
	const UTF8Char *path;
	OSInt i;
	if (IO::Path::PATH_SEPERATOR == '/')
	{
		paths = (const UTF8Char**)OCVUtil_LinuxPaths;
	}
	else
	{
		paths = (const UTF8Char**)OCVUtil_WinPaths;
	}
	i = 0;
	while (true)
	{
		path = paths[i++];
		if (path == 0)
			break;
		sptr = OCVUtil_TestDataPath(sbuff, path, dataFile);
		if (sptr)
			return sptr;
	}
	return 0;
}