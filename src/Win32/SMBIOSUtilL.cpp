#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "Text/MyString.h"
#include "Win32/SMBIOSUtil.h"

Win32::SMBIOS *Win32::SMBIOSUtil::GetSMBIOS()
{
	Win32::SMBIOS *smbios;
	UInt8 *dataBuff = 0;
	UOSInt buffSize = 0;
	UInt8 buffTmp[1024];

	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"/sys/firmware/dmi/tables/smbios_entry_point", IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (!fs->IsError())
	{
		buffSize = fs->Read(buffTmp, 128);
		DEL_CLASS(fs);
		if (Text::StrStartsWith(buffTmp, (const UTF8Char*)"_SM_"))
		{
			if (Text::StrStartsWith(&buffTmp[16], (const UTF8Char*)"_DMI_") && buffSize >= 30)
			{
				UInt32 ofst = 0;//ReadUInt32(&buffTmp[0x18]);
				buffSize = ReadUInt16(&buffTmp[0x16]);
//				UInt32 cnt = ReadUInt16(&buffTmp[0x1c]);

				NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"/sys/firmware/dmi/tables/DMI", IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
				if (!fs->IsError())
				{
					dataBuff = MemAlloc(UInt8, buffSize);
					fs->SeekFromBeginning(ofst);
					if (buffSize != fs->Read(dataBuff, buffSize))
					{
						MemFree(dataBuff);
						dataBuff = 0;
					}
				}
				DEL_CLASS(fs);
			}
		}
		else if (Text::StrStartsWith(buffTmp, (const UTF8Char*)"_SM3_"))
		{
			if (buffSize >= 24)
			{
				UInt64 ofst = 0;//ReadInt64(&buffTmp[0x10]);
				buffSize = ReadUInt32(&buffTmp[0x0c]);

				NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"/sys/firmware/dmi/tables/DMI", IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
				if (!fs->IsError())
				{
					dataBuff = MemAlloc(UInt8, buffSize);
					fs->SeekFromBeginning(ofst);
					if (buffSize != fs->Read(dataBuff, buffSize))
					{
						MemFree(dataBuff);
						dataBuff = 0;
					}
				}
				DEL_CLASS(fs);
			}
		}
		else if (Text::StrStartsWith(buffTmp, (const UTF8Char*)"_DMI_"))
		{
			if (buffSize >= 16)
			{
				UInt32 ofst = 0;//ReadUInt32(&buffTmp[0x08]);
				buffSize = ReadUInt16(&buffTmp[0x06]);
//				UInt32 cnt = ReadUInt16(&buffTmp[0x0c]);

				NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"/sys/firmware/dmi/tables/DMI", IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
				if (!fs->IsError())
				{
					dataBuff = MemAlloc(UInt8, buffSize);
					fs->SeekFromBeginning(ofst);
					if (buffSize != fs->Read(dataBuff, buffSize))
					{
						MemFree(dataBuff);
						dataBuff = 0;
					}
				}
				DEL_CLASS(fs);
			}
		}
	}
	else
	{
		DEL_CLASS(fs);
	}

	if (dataBuff == 0)
	{
	}

	if (dataBuff == 0)
	{
		UTF8Char *sptr;
		UTF8Char *sptr2;
		UTF8Char sbuff[512];
		IO::Path::FindFileSession *sess;
		UOSInt readSize;
		IO::MemoryStream *mstm;
		NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"Win32.SMBIOS.GetSMBIOS.mstm"));
		sptr = Text::StrConcat(sbuff, (const UTF8Char*)"/sys/firmware/dmi/entries/");
		Text::StrConcat(sptr, (const UTF8Char*)"*");
		sess = IO::Path::FindFile(sbuff);
		if (sess)
		{
			while ((sptr2 = IO::Path::FindNextFile(sptr, sess, 0, 0, 0)) != 0)
			{
				if (sptr[0] != '.')
				{
					sptr2 = Text::StrConcat(sptr2, (const UTF8Char*)"/raw");
					NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
					if (!fs->IsError())
					{
						readSize = fs->Read(buffTmp, 1024);
						if (readSize > 0)
						{
							mstm->Write(buffTmp, readSize);
						}
					}
					else
					{
					}
					DEL_CLASS(fs);
				}
			}
			IO::Path::FindFileClose(sess);
		}
		UInt8 *mstmBuff = mstm->GetBuff(&readSize);
		if (readSize > 0)
		{
			dataBuff = MemAlloc(UInt8, readSize);
			buffSize = (UInt32)readSize;
			MemCopyNO(dataBuff, mstmBuff, readSize);
		}
		DEL_CLASS(mstm);
	}
	if (dataBuff)
	{
		NEW_CLASS(smbios, Win32::SMBIOS(dataBuff, buffSize, dataBuff));
		return smbios;
	}

	return 0;
}
