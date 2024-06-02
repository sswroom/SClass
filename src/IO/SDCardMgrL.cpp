#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/SDCardMgr.h"
#include "Text/MyString.h"

Bool SDCardMgr_ReadId(Text::CStringNN fileName, UInt8 *buff)
{
	UOSInt i;
	Bool ret = false;
	UInt8 fileBuff[64];
	IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	i = fs.Read(BYTEARR(fileBuff));
	if (i >= 32)
	{
		fileBuff[32] = 0;
		if (Text::StrHex2Bytes((const UTF8Char*)fileBuff, buff) == 16)
		{
			ret = true;
		}
	}
	return ret;
}

UOSInt IO::SDCardMgr::GetCardList(NN<Data::ArrayListNN<IO::SDCardInfo>> cardList)
{
	UTF8Char nameBuff[16];
	UnsafeArray<UTF8Char> namePtr;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	UnsafeArray<UTF8Char> sptr3;
	UnsafeArray<UTF8Char> sptr4;
	UInt8 cid[16];
	UInt8 csd[16];
	IO::Path::PathType pt;
	UOSInt ret = 0;
	NN<IO::SDCardInfo> sdcard;

	sptr = Text::StrConcatC(sbuff, UTF8STRC("/sys/class/mmc_host/"));
	sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	IO::Path::FindFileSession *sess = IO::Path::FindFile(CSTRP(sbuff, sptr2));
	if (sess)
	{
		while (IO::Path::FindNextFile(sptr, sess, 0, &pt, 0).SetTo(sptr2))
		{
			if (sptr[0] != '.' && pt != IO::Path::PathType::File)
			{
				sptr2 = Text::StrConcatC(sptr2, UTF8STRC("/"));
				sptr3 = Text::StrConcatC(sptr2, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
				IO::Path::FindFileSession *sess2 = IO::Path::FindFile(CSTRP(sbuff, sptr3));
				if (sess2)
				{
					while (IO::Path::FindNextFile(sptr2, sess2, 0, &pt, 0).SetTo(sptr3))
					{
						if (sptr2[0] != '.' && pt != IO::Path::PathType::File && (sptr3 - sptr2) <= 15 && Text::StrIndexOfChar(sptr2, ':') != INVALID_INDEX)
						{
							Bool valid = true;
							namePtr = Text::StrConcat(nameBuff, sptr2);

							sptr4 = Text::StrConcatC(sptr3, UTF8STRC("/cid"));
							if (!SDCardMgr_ReadId(CSTRP(sbuff, sptr4), cid))
								valid = false;
							sptr4 = Text::StrConcatC(sptr3, UTF8STRC("/csd"));
							if (!SDCardMgr_ReadId(CSTRP(sbuff, sptr4), csd))
								valid = false;
							if (valid)
							{
								NEW_CLASSNN(sdcard, IO::SDCardInfo(CSTRP(nameBuff, namePtr), cid, csd));
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
