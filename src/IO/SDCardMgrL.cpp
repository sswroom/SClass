#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/SDCardMgr.h"
#include "Text/MyString.h"

Bool SDCardMgr_ReadId(const UTF8Char *fileName, UInt8 *buff)
{
	UOSInt i;
	Bool ret = false;
	UInt8 fileBuff[64];
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	i = fs->Read(fileBuff, 64);
	if (i >= 32)
	{
		fileBuff[32] = 0;
		if (Text::StrHex2Bytes((const UTF8Char*)fileBuff, buff) == 16)
		{
			ret = true;
		}
	}
	DEL_CLASS(fs);
	return ret;
}

UOSInt IO::SDCardMgr::GetCardList(Data::ArrayList<IO::SDCardInfo*> *cardList)
{
	UTF8Char nameBuff[16];
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	UTF8Char *sptr3;
	UInt8 cid[16];
	UInt8 csd[16];
	IO::Path::PathType pt;
	UOSInt ret = 0;
	IO::SDCardInfo *sdcard;

	sptr = Text::StrConcat(sbuff, (const UTF8Char*)"/sys/class/mmc_host/");
	Text::StrConcat(sptr, IO::Path::ALL_FILES);
	IO::Path::FindFileSession *sess = IO::Path::FindFile(sbuff);
	if (sess)
	{
		while ((sptr2 = IO::Path::FindNextFile(sptr, sess, 0, &pt, 0)) != 0)
		{
			if (sptr[0] != '.' && pt != IO::Path::PathType::File)
			{
				sptr2 = Text::StrConcat(sptr2, (const UTF8Char*)"/");
				Text::StrConcat(sptr2, IO::Path::ALL_FILES);
				IO::Path::FindFileSession *sess2 = IO::Path::FindFile(sbuff);
				if (sess2)
				{
					while ((sptr3 = IO::Path::FindNextFile(sptr2, sess2, 0, &pt, 0)) != 0)
					{
						if (sptr2[0] != '.' && pt != IO::Path::PathType::File && (sptr3 - sptr2) <= 15 && Text::StrIndexOf(sptr2, ':') != INVALID_INDEX)
						{
							Bool valid = true;
							Text::StrConcat(nameBuff, sptr2);

							Text::StrConcat(sptr3, (const UTF8Char*)"/cid");
							if (!SDCardMgr_ReadId(sbuff, cid))
								valid = false;
							Text::StrConcat(sptr3, (const UTF8Char*)"/csd");
							if (!SDCardMgr_ReadId(sbuff, csd))
								valid = false;
							if (valid)
							{
								NEW_CLASS(sdcard, IO::SDCardInfo(nameBuff, cid, csd));
								cardList->Add(sdcard);
								ret++;
							}
						}
					}
					IO::Path::FindFileClose(sess2);
				}
			}
		}
		IO::Path::FindFileClose(sess);
	}
	return ret;
}
