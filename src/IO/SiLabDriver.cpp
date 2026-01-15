#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SiLabDriver.h"
#include "IO/SiLabSerialPort.h"
#include "Text/MyString.h"

IO::SiLabDriver::SI_STATUS IO::SiLabDriver::SI_GetNumDevices(UInt32 *NumDevices)
{
	void *fPtr = this->driver->GetFunc("SI_GetNumDevices");
	if (fPtr == 0)
	{
		return SI_FUNC_NOT_FOUND;
	}
	return (SI_STATUS)(((int(__stdcall*)(UInt32*))fPtr)(NumDevices));
}

IO::SiLabDriver::SI_STATUS IO::SiLabDriver::SI_GetProductString(UInt32 DeviceNum, void *DeviceString, SI_RETURN Options)
{
	void *fPtr = this->driver->GetFunc("SI_GetProductString");
	if (fPtr == 0)
	{
		return SI_FUNC_NOT_FOUND;
	}
	return (SI_STATUS)(((int(__stdcall*)(UInt32, void*, UInt32))fPtr)(DeviceNum, DeviceString, Options));
}

IO::SiLabDriver::SI_STATUS IO::SiLabDriver::SI_Open(UInt32 DeviceNum, void **Handle)
{
	void *fPtr = this->driver->GetFunc("SI_Open");
	if (fPtr == 0)
	{
		return SI_FUNC_NOT_FOUND;
	}
	return (SI_STATUS)(((int(__stdcall*)(UInt32, void**))fPtr)(DeviceNum, Handle));
}

IO::SiLabDriver::SI_STATUS IO::SiLabDriver::SI_Close(void *Handle)
{
	void *fPtr = this->driver->GetFunc("SI_Close");
	if (fPtr == 0)
	{
		return SI_FUNC_NOT_FOUND;
	}
	return (SI_STATUS)(((int(__stdcall*)(void*))fPtr)(Handle));
}

IO::SiLabDriver::SI_STATUS IO::SiLabDriver::SI_Read(void *Handle, void *Buffer, UInt32 NumBytesToRead, UInt32 *NumBytesReturned, void* o)
{
	void *fPtr = this->driver->GetFunc("SI_Read");
	if (fPtr == 0)
	{
		return SI_FUNC_NOT_FOUND;
	}
	return (SI_STATUS)(((int(__stdcall*)(void*, void*, UInt32, UInt32*, void*))fPtr)(Handle, Buffer, NumBytesToRead, NumBytesReturned, o));
}

IO::SiLabDriver::SI_STATUS IO::SiLabDriver::SI_Write(void *Handle, void *Buffer, UInt32 NumBytesToWrite, UInt32 *NumBytesWritten, void* o)
{
	void *fPtr = this->driver->GetFunc("SI_Write");
	if (fPtr == 0)
	{
		return SI_FUNC_NOT_FOUND;
	}
	return (SI_STATUS)(((int(__stdcall*)(void*, void *, UInt32, UInt32*, void *))fPtr)(Handle, Buffer, NumBytesToWrite, NumBytesWritten, o));
}

IO::SiLabDriver::SI_STATUS IO::SiLabDriver::SI_CancelIo(void *Handle)
{
	void *fPtr = this->driver->GetFunc("SI_CancelIo");
	if (fPtr == 0)
	{
		return SI_FUNC_NOT_FOUND;
	}
	return (SI_STATUS)(((int(__stdcall*)(void*))fPtr)(Handle));
}

IO::SiLabDriver::SI_STATUS IO::SiLabDriver::SI_FlushBuffers(void *Handle, UInt8 FlushTransmit, UInt8 FlushReceive)
{
	void *fPtr = this->driver->GetFunc("SI_FlushBuffers");
	if (fPtr == 0)
	{
		return SI_FUNC_NOT_FOUND;
	}
	return (SI_STATUS)(((int(__stdcall*)(void*, UInt8, UInt8))fPtr)(Handle, FlushTransmit, FlushReceive));
}

IO::SiLabDriver::SI_STATUS IO::SiLabDriver::SI_SetBaudRate(void *Handle, UInt32 dwBaudRate)
{
	void *fPtr = this->driver->GetFunc("SI_SetBaudRate");
	if (fPtr == 0)
	{
		return SI_FUNC_NOT_FOUND;
	}
	return (SI_STATUS)(((int(__stdcall*)(void*, UInt32))fPtr)(Handle, dwBaudRate));
}

IO::SiLabDriver::SI_STATUS IO::SiLabDriver::SI_GetDLLVersion(UInt32* HighVersion, UInt32* LowVersion)
{
	void *fPtr = this->driver->GetFunc("SI_GetDLLVersion");
	if (fPtr == 0)
	{
		return SI_FUNC_NOT_FOUND;
	}
	return (SI_STATUS)(((int(__stdcall*)(UInt32*, UInt32*))fPtr)(HighVersion, LowVersion));
}

IO::SiLabDriver::SI_STATUS IO::SiLabDriver::SI_GetDriverVersion(UInt32* HighVersion, UInt32* LowVersion)
{
	void *fPtr = this->driver->GetFunc("SI_GetDriverVersion");
	if (fPtr == 0)
	{
		return SI_FUNC_NOT_FOUND;
	}
	return (SI_STATUS)(((int(__stdcall*)(UInt32*, UInt32*))fPtr)(HighVersion, LowVersion));
}

IO::SiLabDriver::SiLabDriver()
{
#ifdef _WIN32
	NEW_CLASS(this->driver, IO::Library((const UTF8Char*)"SiUSBXP.dll"));
#else
	NEW_CLASS(this->driver, IO::Library((const UTF8Char*)"SiUSBXP.so"));
#endif
	if (this->driver->IsError())
	{
		DEL_CLASS(this->driver);
		this->driver = 0;
	}
}

IO::SiLabDriver::~SiLabDriver()
{
	SDEL_CLASS(this->driver);
}

Bool IO::SiLabDriver::IsError()
{
	return this->driver == 0;
}

Bool IO::SiLabDriver::GetDLLVersion(UInt16 *ver1, UInt16 *ver2, UInt16 *ver3, UInt16 *ver4)
{
	UInt32 hiVer;
	UInt32 loVer;
	if (SI_GetDLLVersion(&hiVer, &loVer) == SI_SUCCESS)
	{
		*ver1 = (UInt16)(hiVer >> 16);
		*ver2 = (UInt16)(hiVer & 0xffff);
		*ver3 = (UInt16)(loVer >> 16);
		*ver4 = (UInt16)(loVer & 0xffff);
		return true;
	}
	return false;
}

Bool IO::SiLabDriver::GetDriverVersion(UInt16 *ver1, UInt16 *ver2, UInt16 *ver3, UInt16 *ver4)
{
	UInt32 hiVer;
	UInt32 loVer;
	if (SI_GetDriverVersion(&hiVer, &loVer) == SI_SUCCESS)
	{
		*ver1 = (UInt16)(hiVer >> 16);
		*ver2 = (UInt16)(hiVer & 0xffff);
		*ver3 = (UInt16)(loVer >> 16);
		*ver4 = (UInt16)(loVer & 0xffff);
		return true;
	}
	return false;
}

UInt32 IO::SiLabDriver::GetNumDevices()
{
	UInt32 cnt;
	if (this->SI_GetNumDevices(&cnt) == SI_SUCCESS)
		return cnt;
	return 0;
}

Bool IO::SiLabDriver::GetDeviceVID(UInt32 devId, UInt32 *vid)
{
	Char buff[32];
	if (this->SI_GetProductString(devId, buff, IO::SiLabDriver::SI_RETURN_VID) == SI_SUCCESS)
	{
		*vid = Text::StrHex2UInt32ChC(buff);
		return true;
	}
	return false;
}

Bool IO::SiLabDriver::GetDevicePID(UInt32 devId, UInt32 *pid)
{
	Char buff[32];
	if (this->SI_GetProductString(devId, buff, IO::SiLabDriver::SI_RETURN_PID) == SI_SUCCESS)
	{
		*pid = Text::StrHex2UInt32ChC(buff);
		return true;
	}
	return false;
}

UnsafeArrayOpt<UTF8Char> IO::SiLabDriver::GetDeviceSN(UInt32 devId, UnsafeArray<UTF8Char> buff)
{
	Char cbuff[256];
	if (this->SI_GetProductString(devId, cbuff, IO::SiLabDriver::SI_RETURN_SERIAL_NUMBER) == SI_SUCCESS)
	{
		return Text::StrConcat(buff, (const UTF8Char*)cbuff);
	}
	return nullptr;
}

UnsafeArrayOpt<UTF8Char> IO::SiLabDriver::GetDeviceDesc(UInt32 devId, UnsafeArray<UTF8Char> buff)
{
	Char cbuff[256];
	if (this->SI_GetProductString(devId, cbuff, IO::SiLabDriver::SI_RETURN_DESCRIPTION) == SI_SUCCESS)
	{
		return Text::StrConcat(buff, (const UTF8Char*)cbuff);
	}
	return nullptr;
}

UnsafeArrayOpt<UTF8Char> IO::SiLabDriver::GetDeviceLink(UInt32 devId, UnsafeArray<UTF8Char> buff)
{
	Char cbuff[256];
	if (this->SI_GetProductString(devId, cbuff, IO::SiLabDriver::SI_RETURN_LINK_NAME) == SI_SUCCESS)
	{
		return Text::StrConcat(buff, (const UTF8Char*)cbuff);
	}
	return nullptr;
}

IO::Stream *IO::SiLabDriver::OpenPort(UInt32 devId, UInt32 baudRate)
{
	IO::SiLabSerialPort *port;
	void *hand;
	if (this->SI_Open(devId, &hand) != SI_SUCCESS)
		return 0;
	if (this->SI_SetBaudRate(hand, baudRate) != SI_SUCCESS)
	{
		this->SI_Close(hand);
		return 0;
	}
	NEW_CLASS(port, IO::SiLabSerialPort(hand, this));
	return port;
}
