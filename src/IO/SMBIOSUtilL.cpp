#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "IO/SMBIOSUtil.h"
#include "Text/MyString.h"

Optional<IO::SMBIOS> IO::SMBIOSUtil::GetSMBIOS()
{
	IO::SMBIOS *smbios;
	UInt8 *dataBuff = 0;
	UOSInt buffSize = 0;
	UInt8 buffTmp[1024];

	{
		IO::FileStream entryPointFS(CSTR("/sys/firmware/dmi/tables/smbios_entry_point"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!entryPointFS.IsError())
		{
			buffSize = entryPointFS.Read(Data::ByteArray(buffTmp, 128));
			entryPointFS.Close();
			UInt32 ofst = 0;

			if (Text::StrStartsWithC(buffTmp, buffSize, UTF8STRC("_SM_")))
			{
				if (buffSize >= 30 && Text::StrStartsWithC(&buffTmp[16], buffSize - 16, UTF8STRC("_DMI_")))
				{
					ofst = 0;//ReadUInt32(&buffTmp[0x18]);
					buffSize = ReadUInt16(&buffTmp[0x16]);
				}
			}
			else if (Text::StrStartsWithC(buffTmp, buffSize, UTF8STRC("_SM3_")))
			{
				if (buffSize >= 24)
				{
					ofst = 0;//ReadInt64(&buffTmp[0x10]);
					buffSize = ReadUInt32(&buffTmp[0x0c]);
				}
			}
			else if (Text::StrStartsWithC(buffTmp, buffSize, UTF8STRC("_DMI_")))
			{
				if (buffSize >= 16)
				{
					ofst = 0;//ReadUInt32(&buffTmp[0x08]);
					buffSize = ReadUInt16(&buffTmp[0x06]);
				}
			}

			if (buffSize != 0)
			{
				IO::FileStream fs(CSTR("/sys/firmware/dmi/tables/DMI"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				if (!fs.IsError())
				{
					dataBuff = MemAlloc(UInt8, buffSize);
					fs.SeekFromBeginning(ofst);
					UOSInt totalRead = 0;
					UOSInt thisRead;
					while (totalRead < buffSize)
					{
						thisRead = fs.Read(Data::ByteArray(&dataBuff[totalRead], buffSize - totalRead));
						if (thisRead == 0)
						{
							MemFree(dataBuff);
							dataBuff = 0;
							break;
						}
						totalRead += thisRead;
					}
				}				
			}
		}
	}

	if (dataBuff == 0)
	{
		UnsafeArray<UTF8Char> sptr;
		UnsafeArray<UTF8Char> sptr2;
		UTF8Char sbuff[512];
		NN<IO::Path::FindFileSession> sess;
		UOSInt readSize;
		IO::MemoryStream mstm;
		sptr = Text::StrConcatC(sbuff, UTF8STRC("/sys/firmware/dmi/entries/"));
		sptr2 = Text::StrConcatC(sptr, UTF8STRC("*"));
		if (IO::Path::FindFile(CSTRP(sbuff, sptr2)).SetTo(sess))
		{
			while (IO::Path::FindNextFile(sptr, sess, 0, 0, 0).SetTo(sptr2))
			{
				if (sptr[0] != '.')
				{
					sptr2 = Text::StrConcatC(sptr2, UTF8STRC("/raw"));
					IO::FileStream fs(CSTRP(sbuff, sptr2), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
					if (!fs.IsError())
					{
						readSize = fs.Read(Data::ByteArray(buffTmp, 1024));
						if (readSize > 0)
						{
							mstm.Write(Data::ByteArrayR(buffTmp, readSize));
						}
					}
				}
			}
			IO::Path::FindFileClose(sess);
		}
		UnsafeArray<UInt8> mstmBuff = mstm.GetBuff(readSize);
		if (readSize > 0)
		{
			dataBuff = MemAlloc(UInt8, readSize);
			buffSize = (UInt32)readSize;
			MemCopyNO(dataBuff, mstmBuff.Ptr(), readSize);
		}
	}
	if (dataBuff)
	{
		NEW_CLASS(smbios, IO::SMBIOS(dataBuff, buffSize, dataBuff));
		return smbios;
	}

	return 0;
}
