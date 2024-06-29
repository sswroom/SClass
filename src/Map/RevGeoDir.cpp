#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "IO/Path.h"
#include "Map/RevGeoDir.h"

Map::RevGeoDir::RevGeoDir(Text::CStringNN cfgDir, UInt32 defLCID, NN<IO::Writer> errWriter)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	Data::Timestamp modTime;
	NN<IO::Path::FindFileSession> sess;
	IO::Path::PathType pt;

	this->defLCID = defLCID;

	sptr = cfgDir.ConcatTo(sbuff);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr2 = Text::StrConcatC(sptr, UTF8STRC("RevGeo_*.*"));
	if (!IO::Path::FindFile(CSTRP(sbuff, sptr2)).SetTo(sess))
	{
		return;
	}
	while (IO::Path::FindNextFile(sptr, sess, modTime, pt, 0).SetTo(sptr2))
	{
		if (Text::StrStartsWithICaseC(sptr, (UOSInt)(sptr2 - sptr), UTF8STRC("REVGEO_")))
		{
			errWriter->Write(CSTR("Loading search file "));
			errWriter->Write(CSTRP(sptr, sptr2));
			errWriter->Write(CSTR("..."));
			
			Map::RevGeoCfg *revGeo;
			NN<RevGeoFile> file;
			UOSInt i;
			NEW_CLASS(revGeo, Map::RevGeoCfg(CSTRP(sbuff, sptr2), &this->mapSrchMgr));
			file = MemAllocNN(RevGeoFile);
			file->cfg = revGeo;
			i = Text::StrIndexOfChar(sptr, '.');
			if (i != INVALID_INDEX && i >= 7)
			{
				sptr[i] = 0;
				file->lcid = Text::StrToUInt32(&sptr[7]);
			}
			else
			{
				file->lcid = 0;
			}
			this->files.Add(file);
			errWriter->WriteLine(CSTR("Success"));

		}
	}
	IO::Path::FindFileClose(sess);

}

Map::RevGeoDir::~RevGeoDir()
{
	NN<RevGeoFile> file;
	UOSInt i = this->files.GetCount();
	while (i-- > 0)
	{
		file = this->files.GetItemNoCheck(i);
		DEL_CLASS(file->cfg);
		MemFreeNN(file);
	}
}

UnsafeArrayOpt<UTF8Char> Map::RevGeoDir::SearchName(UnsafeArray<UTF8Char> buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid)
{
	UOSInt i;
	Optional<RevGeoFile> file;
	NN<RevGeoFile> tmpFile;
	file = this->files.GetItem(0);
	i = this->files.GetCount();
	while (i-- > 0)
	{
		tmpFile = this->files.GetItemNoCheck(i);
		if (tmpFile->lcid == lcid)
		{
			file = tmpFile;
			break;
		}
		else if (tmpFile->lcid == this->defLCID)
		{
			file = tmpFile;
		}
	}
	if (!file.SetTo(tmpFile))
		return 0;
	return tmpFile->cfg->GetStreetName(buff, buffSize, pos);
}

UnsafeArrayOpt<UTF8Char> Map::RevGeoDir::CacheName(UnsafeArray<UTF8Char> buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid)
{
	return SearchName(buff, buffSize, pos, lcid);
}
