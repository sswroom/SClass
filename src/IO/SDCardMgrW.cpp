#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "IO/Path.h"
#include "IO/SDCardMgr.h"
#include "Text/StringBuilderUTF16.h"
#include "Win32/WMIQuery.h"
#include <windows.h>

#undef FindNextFile

#define IOCTL_SFFDISK_DEVICE_COMMAND CTL_CODE( FILE_DEVICE_DISK, 0x7a1, METHOD_BUFFERED, FILE_WRITE_ACCESS)
enum SFFDISK_DCMD
{
	SFFDISK_DC_GET_VERSION     ,
	SFFDISK_DC_LOCK_CHANNEL    ,
	SFFDISK_DC_UNLOCK_CHANNEL  ,
	SFFDISK_DC_DEVICE_COMMAND
};

typedef struct _SFFDISK_DEVICE_COMMAND_DATA
{
	USHORT       HeaderSize;
	USHORT       Reserved;
	SFFDISK_DCMD Command;
	USHORT       ProtocolArgumentSize;
	ULONG        DeviceDataBufferSize;
	ULONG_PTR    Information;
	UCHAR        Data[];
} SFFDISK_DEVICE_COMMAND_DATA, *PSFFDISK_DEVICE_COMMAND_DATA;

enum SD_COMMAND_CLASS
{
	SDCC_STANDARD  ,
	SDCC_APP_CMD
};

enum SD_RESPONSE_TYPE
{
	SDRT_UNSPECIFIED  ,
	SDRT_NONE         ,
	SDRT_1            ,
	SDRT_1B           ,
	SDRT_2            ,
	SDRT_3            ,
	SDRT_4            ,
	SDRT_5            ,
	SDRT_5B           ,
	SDRT_6
} ;

enum SD_TRANSFER_TYPE
{
	SDTT_UNSPECIFIED           ,
	SDTT_CMD_ONLY              ,
	SDTT_SINGLE_BLOCK          ,
	SDTT_MULTI_BLOCK           ,
	SDTT_MULTI_BLOCK_NO_CMD12
} ;

typedef enum
{ 
	SDTD_UNSPECIFIED  = 0,
	SDTD_READ         = 1,
	SDTD_WRITE        = 2
} SD_TRANSFER_DIRECTION;

//
// SD device commands codes either refer to the standard command
// set (0-63), or to the "App Cmd" set, which have the same value range,
// but are preceded by the app_cmd escape (55).
//
typedef UCHAR SD_COMMAND_CODE;

typedef struct _SDCMD_DESCRIPTOR
{
	SD_COMMAND_CODE       Cmd;
	SD_COMMAND_CLASS      CmdClass;
	SD_TRANSFER_DIRECTION TransferDirection;
	SD_TRANSFER_TYPE      TransferType;
	SD_RESPONSE_TYPE      ResponseType;
} SDCMD_DESCRIPTOR, *PSDCMD_DESCRIPTOR;

IO::SDCardInfo *SDCardMgr_ReadInfo(const WChar *path)
{
    BOOL bResult=FALSE;                 // results flag
    UInt32 nSizeofCmd = 0;
    DWORD dwBytesReturned = 0;
	UInt8 cid[16];
	UInt8 csd[16];
	Bool valid = true;
	IO::SDCardInfo *sdcard = 0;

	HANDLE hDevice = CreateFileW(path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_DELETE | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	SFFDISK_DEVICE_COMMAND_DATA* commandData = NULL;
	nSizeofCmd = sizeof (SFFDISK_DEVICE_COMMAND_DATA) + sizeof(SDCMD_DESCRIPTOR) + 16;
	commandData = (SFFDISK_DEVICE_COMMAND_DATA*)MAlloc(nSizeofCmd);
	MemClear(commandData, nSizeofCmd);
	commandData->HeaderSize = sizeof(SFFDISK_DEVICE_COMMAND_DATA);
	commandData->Command = SFFDISK_DC_DEVICE_COMMAND;
	commandData->ProtocolArgumentSize = sizeof(SDCMD_DESCRIPTOR);
	commandData->Information = 0;
	commandData->DeviceDataBufferSize = 16;

	SDCMD_DESCRIPTOR *commandDescriptor = (SDCMD_DESCRIPTOR*)&commandData[1];
	commandDescriptor->Cmd = 10;   //SFFDISK_SC_GET_CARD_VERSION
	commandDescriptor->CmdClass = SDCC_STANDARD;
	commandDescriptor->TransferDirection = SDTD_READ;
	commandDescriptor->TransferType = SDTT_CMD_ONLY;
	commandDescriptor->ResponseType = SDRT_2;

	dwBytesReturned = 0;
	bResult = DeviceIoControl(hDevice, IOCTL_SFFDISK_DEVICE_COMMAND, commandData, nSizeofCmd, commandData, nSizeofCmd, &dwBytesReturned, NULL);
	if (bResult == 0)
	{
		valid = false;
		DWORD result = GetLastError();
		if (result == 5)
		{
//			wprintf(L"Error: Access Denied\r\n");
		}
		else
		{
//			wprintf(L"Error: %d\n", result);
		}
	}
	else
	{
		UInt8 *sptr = 14 + (UInt8*)&commandDescriptor[1];
		UInt8 *dptr = cid;
		OSInt i = 15;
		while (i-- > 0)
		{
			*dptr++ = *sptr--;
		}
	}

	commandDescriptor->Cmd = 9;   //SEND_CSD
	dwBytesReturned = 0;
	bResult = DeviceIoControl(hDevice, IOCTL_SFFDISK_DEVICE_COMMAND, commandData, nSizeofCmd, commandData, nSizeofCmd, &dwBytesReturned, NULL);
	if (bResult == 0)
	{
		valid = false;
		DWORD result = GetLastError();
		if (result == 5)
		{
//			wprintf(L"Error: Access Denied\r\n");
		}
		else
		{
//			wprintf(L"Error: %d\n", result);
		}
	}
	else
	{
		UInt8 *sptr = 14 + (UInt8*)&commandDescriptor[1];
		UInt8 *dptr = csd;
		OSInt i = 15;
		while (i-- > 0)
		{
			*dptr++ = *sptr--;
		}
	}
	
	if (valid)
	{
		NotNullPtr<Text::String> s = Text::String::NewNotNull(path);
		NEW_CLASS(sdcard, IO::SDCardInfo(s->ToCString(), cid, csd));
		s->Release();
	}

	MemFree(commandData);
	CloseHandle(hDevice);

	return sdcard;
}

Bool SDCardMgr_ReadId(Text::CString fileName, UInt8 *buff)
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

UOSInt IO::SDCardMgr::GetCardList(Data::ArrayList<IO::SDCardInfo*> *cardList)
{
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UOSInt ret = 0;
	IO::SDCardInfo *sdcard;
	Win32::WMIQuery qry(L"ROOT\\CIMV2");
	DB::DBReader *r = qry.QueryTableData(CSTR_NULL, CSTR("CIM_MediaAccessDevice"), 0, 0, 0, CSTR_NULL, 0);
	if (r)
	{
		Bool valid = true;
		sbuff[0] = 0;
		sptr = r->GetName(11, sbuff);
		if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("DeviceID")))
		{
			valid = false;
		}
		sptr = r->GetName(31, sbuff);
		if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("PNPDeviceID")))
		{
			valid = false;
		}
		if (valid)
		{
			while (r->ReadNext())
			{
				sb.ClearStr();
				r->GetStr(31, sb);
				if (sb.StartsWith(UTF8STRC("SD\\DISK")))
				{
					sb.ClearStr();
					r->GetStr(11, sb);
					const WChar *wptr = Text::StrToWCharNew(sb.ToString());
					sdcard = SDCardMgr_ReadInfo(wptr);
					Text::StrDelNew(wptr);
					if (sdcard)
					{
						cardList->Add(sdcard);
						ret++;
					}
				}
			}
		}
		qry.CloseReader(r);
	}
	else //wine
	{
		UTF8Char nameBuff[16];
		UTF8Char *namePtr;
		UTF8Char *sptr;
		UTF8Char *sptr2;
		UTF8Char *sptr3;
		UTF8Char *sptr3End;
		UInt8 cid[16];
		UInt8 csd[16];
		IO::Path::PathType pt;

		sptr = Text::StrConcatC(sbuff, UTF8STRC("Z:\\sys\\class\\mmc_host\\"));
		sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
		IO::Path::FindFileSession *sess = IO::Path::FindFile(CSTRP(sbuff, sptr2));
		if (sess)
		{
			while ((sptr2 = IO::Path::FindNextFile(sptr, sess, 0, &pt, 0)) != 0)
			{
				if (sptr[0] != '.' && pt != IO::Path::PathType::File)
				{
					sptr2 = Text::StrConcatC(sptr2, UTF8STRC("\\"));
					sptr3 = Text::StrConcatC(sptr2, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
					IO::Path::FindFileSession *sess2 = IO::Path::FindFile(CSTRP(sbuff, sptr3));
					if (sess2)
					{
						while ((sptr3 = IO::Path::FindNextFile(sptr2, sess2, 0, &pt, 0)) != 0)
						{
							if (sptr2[0] != '.' && pt != IO::Path::PathType::File && (sptr3 - sptr2) <= 15 && Text::StrIndexOfChar(sptr2, ':') != INVALID_INDEX)
							{
								Bool valid = true;
								namePtr = Text::StrConcat(nameBuff, sptr2);

								sptr3End = Text::StrConcatC(sptr3, UTF8STRC("\\cid"));
								if (!SDCardMgr_ReadId(CSTRP(sbuff, sptr3End), cid))
									valid = false;
								sptr3End = Text::StrConcatC(sptr3, UTF8STRC("\\csd"));
								if (!SDCardMgr_ReadId(CSTRP(sbuff, sptr3End), csd))
									valid = false;
								if (valid)
								{
									NEW_CLASS(sdcard, IO::SDCardInfo(CSTRP(nameBuff, namePtr), cid, csd));
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
	}
	return ret;
}
