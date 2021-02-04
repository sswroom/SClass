#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Media/DDCReader.h"
#include "Text/MyString.h"

UInt8 *DDCReader_GetMonitorEDID(void *hMon, UOSInt *edidSizeRet)
{
	UTF8Char sbuff[512];
	IO::FileStream *fs;
	UOSInt edidSize;
	UInt8 edid[1025];
	UInt8 *ret = 0;

	// Intel GPU
	UTF8Char *sptr;
	UTF8Char *sptr2;
	UTF8Char *sptr3;
	UTF8Char *sptr4;
	IO::Path::FindFileSession *sess;
	IO::Path::FindFileSession *sess2;
	IO::Path::FindFileSession *sess3;
	IO::Path::PathType pt;
	sptr = Text::StrConcat(sbuff, (const UTF8Char*)"/sys/devices/pci0000:00/");
	Text::StrConcat(sptr, IO::Path::ALL_FILES);
	sess = IO::Path::FindFile(sbuff);
	if (sess)
	{
		while ((sptr2 = IO::Path::FindNextFile(sptr, sess, 0, &pt, 0)) != 0)
		{
			Text::StrConcat(sptr2, (const UTF8Char*)"/drm");
			if (sptr[0] != '0')
			{

			}
			else if (IO::Path::GetPathType(sbuff) == IO::Path::PT_DIRECTORY)
			{
				sptr2 = Text::StrConcat(sptr2, (const UTF8Char*)"/drm/card0/");
				Text::StrConcat(sptr2, (const UTF8Char*)"card0-*");
				sess2 = IO::Path::FindFile(sbuff);
				if (sess2)
				{
					while ((sptr3 = IO::Path::FindNextFile(sptr2, sess2, 0, &pt, 0)) != 0)
					{
						sptr3 = Text::StrConcat(sptr3, (const UTF8Char*)"/edid");
						NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
						if (!fs->IsError())
						{
							edidSize = fs->Read(edid, 1024);
							if (edidSize > 0)
							{
								ret = MemAlloc(UInt8, edidSize);
								MemCopyNO(ret, edid, edidSize);
								*edidSizeRet = edidSize;
							}
						}
						DEL_CLASS(fs);
						if (ret)
							break;
					}
					IO::Path::FindFileClose(sess2);
				}
				if (ret)
					break;
			}
			else
			{
				*sptr2++ = IO::Path::PATH_SEPERATOR;
				Text::StrConcat(sptr2, IO::Path::ALL_FILES);
				sess2 = IO::Path::FindFile(sbuff);
				if (sess2)
				{
					while ((sptr3 = IO::Path::FindNextFile(sptr2, sess2, 0, &pt, 0)) != 0)
					{
						if (sptr2[0] == '0')
						{
							sptr3 = Text::StrConcat(sptr3, (const UTF8Char*)"/drm/card0/");
							Text::StrConcat(sptr3, (const UTF8Char*)"card0-*");
							sess3 = IO::Path::FindFile(sbuff);
							if (sess3)
							{
								while ((sptr4 = IO::Path::FindNextFile(sptr3, sess3, 0, &pt, 0)) != 0)
								{
									sptr4 = Text::StrConcat(sptr4, (const UTF8Char*)"/edid");
									NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
									if (!fs->IsError())
									{
										edidSize = fs->Read(edid, 1024);
										if (edidSize > 0)
										{
											ret = MemAlloc(UInt8, edidSize);
											MemCopyNO(ret, edid, edidSize);
											*edidSizeRet = edidSize;
										}
									}
									DEL_CLASS(fs);
									if (ret)
										break;
								}
								IO::Path::FindFileClose(sess3);
							}
						}
						if (ret)
							break;
					}
					IO::Path::FindFileClose(sess2);
				}
				if (ret)
					break;
			}
		}
		IO::Path::FindFileClose(sess);
	}
	else
	{
//		wprintf(L"Error in founding %ls\r\n", sbuff);
	}
	if (ret)
		return ret;

	// hdmi
	NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"/sys/class/hdmi/hdmi/attr/edid", IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (!fs->IsError())
	{
		edidSize = fs->Read(edid, 1024);
		if (edidSize > 0)
		{
			*edidSizeRet = edidSize;
			ret = MemAlloc(UInt8, edidSize);
			MemCopyNO(ret, edid, edidSize);
		}
	}
	DEL_CLASS(fs);
	if (ret)
		return ret;

	// Amlogic
	if (ret == 0)
	{
		NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"/sys/class/amhdmitx/amhdmitx0/rawedid", IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
		if (!fs->IsError())
		{
			edidSize = fs->Read(edid, 1024);
			if (edidSize > 0)
			{
				ret = MemAlloc(UInt8, edidSize >> 1);
				edid[edidSize] = 0;
				edidSize = Text::StrHex2Bytes((const Char*)edid, ret);
				*edidSizeRet = edidSize;
			}
		}
		DEL_CLASS(fs);
	}
	return ret;
}

Media::DDCReader::DDCReader(void *hMon)
{
	this->edid = 0;
	this->edidSize = 0;
	this->hMon = hMon;
	this->edid = DDCReader_GetMonitorEDID(hMon, &edidSize);	
}

Media::DDCReader::DDCReader(const UTF8Char *monitorId)
{
	this->edid = 0;
	this->edidSize = 0;
	this->hMon = 0;
	this->edid = DDCReader_GetMonitorEDID(hMon, &edidSize);	
}

Media::DDCReader::DDCReader(UInt8 *edid, UOSInt edidSize)
{
	this->edid = MemAlloc(UInt8, edidSize);
	this->edidSize = edidSize;
	this->hMon = 0;
	MemCopyNO(this->edid, edid, edidSize);
}

Media::DDCReader::~DDCReader()
{
	if (this->edid)
	{
		MemFree(this->edid);
		this->edid = 0;
	}
}

UInt8 *Media::DDCReader::GetEDID(UOSInt *size)
{
	if (size)
	{
		*size = this->edidSize;
	}
	return this->edid;
}

UOSInt Media::DDCReader::CreateDDCReaders(Data::ArrayList<DDCReader*> *readerList)
{
	UTF8Char sbuff[512];
	IO::FileStream *fs;
	UOSInt edidSize;
	UInt8 edid[1025];
	Media::DDCReader *reader;
	UOSInt ret = 0;

	// Intel GPU
	UTF8Char *sptr;
	UTF8Char *sptr2;
	UTF8Char *sptr3;
	UTF8Char *sptr4;
	IO::Path::FindFileSession *sess;
	IO::Path::FindFileSession *sess2;
	IO::Path::FindFileSession *sess3;
	IO::Path::PathType pt;
	sptr = Text::StrConcat(sbuff, (const UTF8Char*)"/sys/devices/pci0000:00/");
	Text::StrConcat(sptr, IO::Path::ALL_FILES);
	sess = IO::Path::FindFile(sbuff);
	if (sess)
	{
		while ((sptr2 = IO::Path::FindNextFile(sptr, sess, 0, &pt, 0)) != 0)
		{
			Text::StrConcat(sptr2, (const UTF8Char*)"/drm");
			if (sptr[0] != '0')
			{

			}
			else if (IO::Path::GetPathType(sbuff) == IO::Path::PT_DIRECTORY)
			{
				sptr2 = Text::StrConcat(sptr2, (const UTF8Char*)"/drm/card0/");
				Text::StrConcat(sptr2, (const UTF8Char*)"card0-*");
				sess2 = IO::Path::FindFile(sbuff);
				if (sess2)
				{
					while ((sptr3 = IO::Path::FindNextFile(sptr2, sess2, 0, &pt, 0)) != 0)
					{
						sptr3 = Text::StrConcat(sptr3, (const UTF8Char*)"/edid");
						NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
						if (!fs->IsError())
						{
							edidSize = fs->Read(edid, 1024);
							if (edidSize > 0)
							{
								NEW_CLASS(reader, Media::DDCReader(edid, edidSize));
								readerList->Add(reader);
								ret++;
							}
						}
						DEL_CLASS(fs);
					}
					IO::Path::FindFileClose(sess2);
				}
			}
			else
			{
				*sptr2++ = IO::Path::PATH_SEPERATOR;
				Text::StrConcat(sptr2, IO::Path::ALL_FILES);
				sess2 = IO::Path::FindFile(sbuff);
				if (sess2)
				{
					while ((sptr3 = IO::Path::FindNextFile(sptr2, sess2, 0, &pt, 0)) != 0)
					{
						if (sptr2[0] == '0')
						{
							sptr3 = Text::StrConcat(sptr3, (const UTF8Char*)"/drm/card0/");
							Text::StrConcat(sptr3, (const UTF8Char*)"card0-*");
							sess3 = IO::Path::FindFile(sbuff);
							if (sess3)
							{
								while ((sptr4 = IO::Path::FindNextFile(sptr3, sess3, 0, &pt, 0)) != 0)
								{
									sptr4 = Text::StrConcat(sptr4, (const UTF8Char*)"/edid");
									NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
									if (!fs->IsError())
									{
										edidSize = fs->Read(edid, 1024);
										if (edidSize > 0)
										{
											NEW_CLASS(reader, Media::DDCReader(edid, edidSize));
											readerList->Add(reader);
											ret++;
										}
									}
									DEL_CLASS(fs);
								}
								IO::Path::FindFileClose(sess3);
							}
						}
					}
					IO::Path::FindFileClose(sess2);
				}
			}
		}
		IO::Path::FindFileClose(sess);
	}
	else
	{
//		wprintf(L"Error in founding %ls\r\n", sbuff);
	}

	// hdmi
	NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"/sys/class/hdmi/hdmi/attr/edid", IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (!fs->IsError())
	{
		edidSize = fs->Read(edid, 1024);
		if (edidSize > 0)
		{
			NEW_CLASS(reader, Media::DDCReader(edid, edidSize));
			readerList->Add(reader);
			ret++;
		}
	}
	DEL_CLASS(fs);

	// Amlogic
	if (ret == 0)
	{
		NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"/sys/class/amhdmitx/amhdmitx0/rawedid", IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
		if (!fs->IsError())
		{
			edidSize = fs->Read(edid, 1024);
			if (edidSize > 0)
			{
				UInt8 *edidData = MemAlloc(UInt8, edidSize >> 1);
				edid[edidSize] = 0;
				edidSize = Text::StrHex2Bytes((const Char*)edid, edidData);
				NEW_CLASS(reader, Media::DDCReader(edidData, edidSize));
				readerList->Add(reader);
				MemFree(edidData);
				ret++;
			}
		}
		DEL_CLASS(fs);
	}
	return ret;
}
