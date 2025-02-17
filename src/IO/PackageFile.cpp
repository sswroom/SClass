#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/PackageFile.h"

//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

IO::PackageFile::PackageFile(NN<Text::String> fileName) : IO::ParsedObject(fileName)
{
}

IO::PackageFile::PackageFile(Text::CStringNN fileName) : IO::ParsedObject(fileName)
{
}

IO::PackageFile::~PackageFile()
{
}

IO::ParserType IO::PackageFile::GetParserType() const
{
	return IO::ParserType::PackageFile;
}


Optional<IO::StreamData> IO::PackageFile::GetItemStmDataNew(Text::CStringNN name) const
{
	UOSInt index = GetItemIndex(name);
	if (index == INVALID_INDEX)
	{
		return 0;
	}
#if defined(VERBOSE)
	printf("PackageFile: Found item %s in index %d\r\n", name.v.Ptr(), (UInt32)index);
#endif
	return this->GetItemStmDataNew(index);
}

Optional<IO::PackageFile> IO::PackageFile::GetItemPack(Text::CStringNN path, OutParam<Bool> needRelease) const
{
	UOSInt i = path.IndexOf('\\');
	UOSInt j = path.IndexOf('/');
	if (i == INVALID_INDEX)
		i = j;
	else if (j == INVALID_INDEX)
	{
	}
	else if (i > j)
		i = j;
	if (i == INVALID_INDEX)
	{
		i = GetItemIndex(path);
		if (i == INVALID_INDEX)
			return 0;
		return this->GetItemPack(i, needRelease);
	}
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	if (i < 64)
	{
		sptr = Text::StrConcatC(sbuff, path.v, i);
		path = path.Substring(i + 1);
		j = GetItemIndex(CSTRP(sbuff, sptr));
	}
	else
	{
		UnsafeArray<UTF8Char> tmpBuff = MemAllocArr(UTF8Char, i + 1);
		sptr = Text::StrConcatC(tmpBuff, path.v, i);
		path = path.Substring(i + 1);
		j = GetItemIndex(CSTRP(tmpBuff, sptr));
		MemFreeArr(tmpBuff);
	}
	if (j == INVALID_INDEX)
		return 0;
	Bool thisNeedRelease;
	NN<IO::PackageFile> pkg;
	if (GetItemPack(j, thisNeedRelease).SetTo(pkg))
	{
		Optional<IO::PackageFile> ret = pkg->GetItemPack(path, needRelease);
		if (thisNeedRelease)
		{
			pkg.Delete();
		}
		return ret;
	}
	return 0;
}
