#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "IO/Path.h"
#include "Map/RevGeoDir.h"

Map::RevGeoDir::RevGeoDir(Text::CString cfgDir, UInt32 defLCID, IO::Writer *errWriter)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	Data::Timestamp modTime;
	IO::Path::FindFileSession *sess;
	IO::Path::PathType pt;

	this->defLCID = defLCID;

	sptr = cfgDir.ConcatTo(sbuff);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr2 = Text::StrConcatC(sptr, UTF8STRC("RevGeo_*.*"));
	sess = IO::Path::FindFile(CSTRP(sbuff, sptr2));
	if (sess == 0)
	{
		return;
	}
	while ((sptr2 = IO::Path::FindNextFile(sptr, sess, &modTime, &pt, 0)) != 0)
	{
		if (Text::StrStartsWithICaseC(sptr, (UOSInt)(sptr2 - sptr), UTF8STRC("REVGEO_")))
		{
			errWriter->WriteStrC(UTF8STRC("Loading search file "));
			errWriter->WriteStrC(sptr, (UOSInt)(sptr2 - sptr));
			errWriter->WriteStrC(UTF8STRC("..."));
			
			Map::RevGeoCfg *revGeo;
			RevGeoFile *file;
			UOSInt i;
			NEW_CLASS(revGeo, Map::RevGeoCfg(CSTRP(sbuff, sptr2), &this->mapSrchMgr));
			file = MemAlloc(RevGeoFile, 1);
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
			errWriter->WriteLineC(UTF8STRC("Success"));

		}
	}
	IO::Path::FindFileClose(sess);

}

Map::RevGeoDir::~RevGeoDir()
{
	RevGeoFile *file;
	UOSInt i = this->files.GetCount();
	while (i-- > 0)
	{
		file = this->files.GetItem(i);
		DEL_CLASS(file->cfg);
		MemFree(file);
	}
}

UTF8Char *Map::RevGeoDir::SearchName(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid)
{
	UOSInt i;
	RevGeoFile *file;
	RevGeoFile *tmpFile;
	file = this->files.GetItem(0);
	i = this->files.GetCount();
	while (i-- > 0)
	{
		tmpFile = this->files.GetItem(i);
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
	if (file == 0)
		return 0;
	return file->cfg->GetStreetName(buff, buffSize, pos);
}

UTF8Char *Map::RevGeoDir::CacheName(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid)
{
	return SearchName(buff, buffSize, pos, lcid);
}
