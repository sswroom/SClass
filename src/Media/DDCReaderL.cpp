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
	UOSInt edidSize;
	UInt8 edid[1025];
	UInt8 *ret = 0;

	// Intel GPU
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	UnsafeArray<UTF8Char> sptr3;
	UnsafeArray<UTF8Char> sptr4;
	IO::Path::FindFileSession *sess;
	IO::Path::FindFileSession *sess2;
	IO::Path::FindFileSession *sess3;
	IO::Path::PathType pt;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("/sys/devices/pci0000:00/"));
	sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	sess = IO::Path::FindFile(CSTRP(sbuff, sptr2));
	if (sess)
	{
		while (IO::Path::FindNextFile(sptr, sess, 0, &pt, 0).SetTo(sptr2))
		{
			Text::StrConcatC(sptr2, UTF8STRC("/drm"));
			if (sptr[0] != '0')
			{

			}
			else if (IO::Path::GetPathType(CSTRP(sbuff, sptr2 + 4)) == IO::Path::PathType::Directory)
			{
				sptr2 = Text::StrConcatC(sptr2, UTF8STRC("/drm/card0/"));
				sptr3 = Text::StrConcatC(sptr2, UTF8STRC("card0-*"));
				sess2 = IO::Path::FindFile(CSTRP(sbuff, sptr3));
				if (sess2)
				{
					while (IO::Path::FindNextFile(sptr2, sess2, 0, &pt, 0).SetTo(sptr3))
					{
						sptr3 = Text::StrConcatC(sptr3, UTF8STRC("/edid"));
						IO::FileStream fs(CSTRP(sbuff, sptr3), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
						if (!fs.IsError())
						{
							edidSize = fs.Read(Data::ByteArray(edid, 1024));
							if (edidSize > 0)
							{
								ret = MemAlloc(UInt8, edidSize);
								MemCopyNO(ret, edid, edidSize);
								*edidSizeRet = edidSize;
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
			else
			{
				*sptr2++ = IO::Path::PATH_SEPERATOR;
				sptr3 = Text::StrConcatC(sptr2, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
				sess2 = IO::Path::FindFile(CSTRP(sbuff, sptr3));
				if (sess2)
				{
					while (IO::Path::FindNextFile(sptr2, sess2, 0, &pt, 0).SetTo(sptr3))
					{
						if (sptr2[0] == '0')
						{
							sptr3 = Text::StrConcatC(sptr3, UTF8STRC("/drm/card0/"));
							sptr4 = Text::StrConcatC(sptr3, UTF8STRC("card0-*"));
							sess3 = IO::Path::FindFile(CSTRP(sbuff, sptr4));
							if (sess3)
							{
								while (IO::Path::FindNextFile(sptr3, sess3, 0, &pt, 0).SetTo(sptr4))
								{
									sptr4 = Text::StrConcatC(sptr4, UTF8STRC("/edid"));
									IO::FileStream fs(CSTRP(sbuff, sptr4), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
									if (!fs.IsError())
									{
										edidSize = fs.Read(Data::ByteArray(edid, 1024));
										if (edidSize > 0)
										{
											ret = MemAlloc(UInt8, edidSize);
											MemCopyNO(ret, edid, edidSize);
											*edidSizeRet = edidSize;
										}
									}
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
	{
		IO::FileStream fs(CSTR("/sys/class/hdmi/hdmi/attr/edid"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!fs.IsError())
		{
			edidSize = fs.Read(Data::ByteArray(edid, 1024));
			if (edidSize > 0)
			{
				*edidSizeRet = edidSize;
				ret = MemAlloc(UInt8, edidSize);
				MemCopyNO(ret, edid, edidSize);
			}
		}
	}
	if (ret)
		return ret;

	// Amlogic
	if (ret == 0)
	{
		IO::FileStream fs(CSTR("/sys/class/amhdmitx/amhdmitx0/rawedid"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!fs.IsError())
		{
			edidSize = fs.Read(Data::ByteArray(edid, 1024));
			if (edidSize > 0)
			{
				ret = MemAlloc(UInt8, edidSize >> 1);
				edid[edidSize] = 0;
				edidSize = Text::StrHex2Bytes(edid, ret);
				*edidSizeRet = edidSize;
			}
		}
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

Media::DDCReader::DDCReader(UnsafeArray<const UTF8Char> monitorId)
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

UInt8 *Media::DDCReader::GetEDID(OutParam<UOSInt> size)
{
	size.Set(this->edidSize);
	return this->edid;
}

UOSInt Media::DDCReader::CreateDDCReaders(NN<Data::ArrayListNN<DDCReader>> readerList)
{
	UTF8Char sbuff[512];
	UOSInt edidSize;
	UInt8 edid[1025];
	NN<Media::DDCReader> reader;
	UOSInt ret = 0;

	// Intel GPU
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	UnsafeArray<UTF8Char> sptr3;
	UnsafeArray<UTF8Char> sptr4;
	IO::Path::FindFileSession *sess;
	IO::Path::FindFileSession *sess2;
	IO::Path::FindFileSession *sess3;
	IO::Path::PathType pt;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("/sys/devices/pci0000:00/"));
	sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	sess = IO::Path::FindFile(CSTRP(sbuff, sptr2));
	if (sess)
	{
		while (IO::Path::FindNextFile(sptr, sess, 0, &pt, 0).SetTo(sptr2))
		{
			Text::StrConcatC(sptr2, UTF8STRC("/drm"));
			if (sptr[0] != '0')
			{

			}
			else if (IO::Path::GetPathType(CSTRP(sbuff, sptr2 + 4)) == IO::Path::PathType::Directory)
			{
				sptr2 = Text::StrConcatC(sptr2, UTF8STRC("/drm/card0/"));
				sptr3 = Text::StrConcatC(sptr2, UTF8STRC("card0-*"));
				sess2 = IO::Path::FindFile(CSTRP(sbuff, sptr3));
				if (sess2)
				{
					while (IO::Path::FindNextFile(sptr2, sess2, 0, &pt, 0).SetTo(sptr3))
					{
						sptr3 = Text::StrConcatC(sptr3, UTF8STRC("/edid"));
						IO::FileStream fs(CSTRP(sbuff, sptr3), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
						if (!fs.IsError())
						{
							edidSize = fs.Read(Data::ByteArray(edid, 1024));
							if (edidSize > 0)
							{
								NEW_CLASSNN(reader, Media::DDCReader(edid, edidSize));
								readerList->Add(reader);
								ret++;
							}
						}
					}
					IO::Path::FindFileClose(sess2);
				}
			}
			else
			{
				*sptr2++ = IO::Path::PATH_SEPERATOR;
				sptr3 = Text::StrConcatC(sptr2, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
				sess2 = IO::Path::FindFile(CSTRP(sbuff, sptr3));
				if (sess2)
				{
					while (IO::Path::FindNextFile(sptr2, sess2, 0, &pt, 0).SetTo(sptr3))
					{
						if (sptr2[0] == '0')
						{
							sptr3 = Text::StrConcatC(sptr3, UTF8STRC("/drm/card0/"));
							sptr4 = Text::StrConcatC(sptr3, UTF8STRC("card0-*"));
							sess3 = IO::Path::FindFile(CSTRP(sbuff, sptr4));
							if (sess3)
							{
								while (IO::Path::FindNextFile(sptr3, sess3, 0, &pt, 0).SetTo(sptr4))
								{
									sptr4 = Text::StrConcatC(sptr4, UTF8STRC("/edid"));
									IO::FileStream fs(CSTRP(sbuff, sptr4), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
									if (!fs.IsError())
									{
										edidSize = fs.Read(Data::ByteArray(edid, 1024));
										if (edidSize > 0)
										{
											NEW_CLASSNN(reader, Media::DDCReader(edid, edidSize));
											readerList->Add(reader);
											ret++;
										}
									}
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
	{
		IO::FileStream fs(CSTR("/sys/class/hdmi/hdmi/attr/edid"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!fs.IsError())
		{
			edidSize = fs.Read(Data::ByteArray(edid, 1024));
			if (edidSize > 0)
			{
				NEW_CLASSNN(reader, Media::DDCReader(edid, edidSize));
				readerList->Add(reader);
				ret++;
			}
		}
	}

	// Amlogic
	if (ret == 0)
	{
		IO::FileStream fs(CSTR("/sys/class/amhdmitx/amhdmitx0/rawedid"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!fs.IsError())
		{
			edidSize = fs.Read(Data::ByteArray(edid, 1024));
			if (edidSize > 0)
			{
				UInt8 *edidData = MemAlloc(UInt8, edidSize >> 1);
				edid[edidSize] = 0;
				edidSize = Text::StrHex2Bytes(edid, edidData);
				NEW_CLASSNN(reader, Media::DDCReader(edidData, edidSize));
				readerList->Add(reader);
				MemFree(edidData);
				ret++;
			}
		}
	}
	return ret;
}
