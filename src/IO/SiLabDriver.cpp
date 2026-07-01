#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SiLabDriver.h"
#include "IO/SiLabSerialPort.h"
#include "Text/MyString.h"

IO::SiLabDriver::SI_STATUS IO::SiLabDriver::SI_GetNumDevices(UInt32 *NumDevices)
{
	NN<IO::Library> driver;
	void *fPtr;
	if (!this->driver.SetTo(driver) || (fPtr = driver->GetFunc("SI_GetNumDevices")) == 0)
	{
		return SI_FUNC_NOT_FOUND;
	}
	return (SI_STATUS)(((int(__stdcall*)(UInt32*))fPtr)(NumDevices));
}

IO::SiLabDriver::SI_STATUS IO::SiLabDriver::SI_GetProductString(UInt32 DeviceNum, void *DeviceString, SI_RETURN Options)
{
	NN<IO::Library> driver;
	void *fPtr;
	if (!this->driver.SetTo(driver) || (fPtr = driver->GetFunc("SI_GetProductString")) == 0)
	{
		return SI_FUNC_NOT_FOUND;
	}
	return (SI_STATUS)(((int(__stdcall*)(UInt32, void*, UInt32))fPtr)(DeviceNum, DeviceString, Options));
}

IO::SiLabDriver::SI_STATUS IO::SiLabDriver::SI_Open(UInt32 DeviceNum, void **Handle)
{
	NN<IO::Library> driver;
	void *fPtr;
	if (!this->driver.SetTo(driver) || (fPtr = driver->GetFunc("SI_Open")) == 0)
	{
		return SI_FUNC_NOT_FOUND;
	}
	return (SI_STATUS)(((int(__stdcall*)(UInt32, void**))fPtr)(DeviceNum, Handle));
}

IO::SiLabDriver::SI_STATUS IO::SiLabDriver::SI_Close(void *Handle)
{
	NN<IO::Library> driver;
	void *fPtr;
	if (!this->driver.SetTo(driver) || (fPtr = driver->GetFunc("SI_Close")) == 0)
	{
		return SI_FUNC_NOT_FOUND;
	}
	return (SI_STATUS)(((int(__stdcall*)(void*))fPtr)(Handle));
}

IO::SiLabDriver::SI_STATUS IO::SiLabDriver::SI_Read(void *Handle, void *Buffer, UInt32 NumBytesToRead, UInt32 *NumBytesReturned, void* o)
{
	NN<IO::Library> driver;
	void *fPtr;
	if (!this->driver.SetTo(driver) || (fPtr = driver->GetFunc("SI_Read")) == 0)
	{
		return SI_FUNC_NOT_FOUND;
	}
	return (SI_STATUS)(((int(__stdcall*)(void*, void*, UInt32, UInt32*, void*))fPtr)(Handle, Buffer, NumBytesToRead, NumBytesReturned, o));
}

IO::SiLabDriver::SI_STATUS IO::SiLabDriver::SI_Write(void *Handle, void *Buffer, UInt32 NumBytesToWrite, UInt32 *NumBytesWritten, void* o)
{
	NN<IO::Library> driver;
	void *fPtr;
	if (!this->driver.SetTo(driver) || (fPtr = driver->GetFunc("SI_Write")) == 0)
	{
		return SI_FUNC_NOT_FOUND;
	}
	return (SI_STATUS)(((int(__stdcall*)(void*, void *, UInt32, UInt32*, void *))fPtr)(Handle, Buffer, NumBytesToWrite, NumBytesWritten, o));
}

IO::SiLabDriver::SI_STATUS IO::SiLabDriver::SI_CancelIo(void *Handle)
{
	NN<IO::Library> driver;
	void *fPtr;
	if (!this->driver.SetTo(driver) || (fPtr = driver->GetFunc("SI_CancelIo")) == 0)
	{
		return SI_FUNC_NOT_FOUND;
	}
	return (SI_STATUS)(((int(__stdcall*)(void*))fPtr)(Handle));
}

IO::SiLabDriver::SI_STATUS IO::SiLabDriver::SI_FlushBuffers(void *Handle, UInt8 FlushTransmit, UInt8 FlushReceive)
{
	NN<IO::Library> driver;
	void *fPtr;
	if (!this->driver.SetTo(driver) || (fPtr = driver->GetFunc("SI_FlushBuffers")) == 0)
	{
		return SI_FUNC_NOT_FOUND;
	}
	return (SI_STATUS)(((int(__stdcall*)(void*, UInt8, UInt8))fPtr)(Handle, FlushTransmit, FlushReceive));
}

IO::SiLabDriver::SI_STATUS IO::SiLabDriver::SI_SetBaudRate(void *Handle, UInt32 dwBaudRate)
{
	NN<IO::Library> driver;
	void *fPtr;
	if (!this->driver.SetTo(driver) || (fPtr = driver->GetFunc("SI_SetBaudRate")) == 0)
	{
		return SI_FUNC_NOT_FOUND;
	}
	return (SI_STATUS)(((int(__stdcall*)(void*, UInt32))fPtr)(Handle, dwBaudRate));
}

IO::SiLabDriver::SI_STATUS IO::SiLabDriver::SI_GetDLLVersion(UInt32* HighVersion, UInt32* LowVersion)
{
	NN<IO::Library> driver;
	void *fPtr;
	if (!this->driver.SetTo(driver) || (fPtr = driver->GetFunc("SI_GetDLLVersion")) == 0)
	{
		return SI_FUNC_NOT_FOUND;
	}
	return (SI_STATUS)(((int(__stdcall*)(UInt32*, UInt32*))fPtr)(HighVersion, LowVersion));
}

IO::SiLabDriver::SI_STATUS IO::SiLabDriver::SI_GetDriverVersion(UInt32* HighVersion, UInt32* LowVersion)
{
	NN<IO::Library> driver;
	void *fPtr;
	if (!this->driver.SetTo(driver) || (fPtr = driver->GetFunc("SI_GetDriverVersion")) == 0)
	{
		return SI_FUNC_NOT_FOUND;
	}
	return (SI_STATUS)(((int(__stdcall*)(UInt32*, UInt32*))fPtr)(HighVersion, LowVersion));
}

IO::SiLabDriver::SiLabDriver()
{
	NN<IO::Library> driver;
#ifdef _WIN32
	NEW_CLASSNN(driver, IO::Library((const UTF8Char*)"SiUSBXP.dll"));
#else
	NEW_CLASSNN(driver, IO::Library((const UTF8Char*)"SiUSBXP.so"));
#endif
	if (driver->IsError())
	{
		driver.Delete();
		this->driver = nullptr;
	}
	else
	{
		this->driver = driver;
	}
}

IO::SiLabDriver::~SiLabDriver()
{
	this->driver.Delete();
}

Bool IO::SiLabDriver::IsError()
{
	return this->driver.IsNull();
}

Bool IO::SiLabDriver::GetDLLVersion(OutParam<UInt16> ver1, OutParam<UInt16> ver2, OutParam<UInt16> ver3, OutParam<UInt16> ver4)
{
	UInt32 hiVer;
	UInt32 loVer;
	if (SI_GetDLLVersion(&hiVer, &loVer) == SI_SUCCESS)
	{
		ver1.Set((UInt16)(hiVer >> 16));
		ver2.Set((UInt16)(hiVer & 0xffff));
		ver3.Set((UInt16)(loVer >> 16));
		ver4.Set((UInt16)(loVer & 0xffff));
		return true;
	}
	return false;
}

Bool IO::SiLabDriver::GetDriverVersion(OutParam<UInt16> ver1, OutParam<UInt16> ver2, OutParam<UInt16> ver3, OutParam<UInt16> ver4)
{
	UInt32 hiVer;
	UInt32 loVer;
	if (SI_GetDriverVersion(&hiVer, &loVer) == SI_SUCCESS)
	{
		ver1.Set((UInt16)(hiVer >> 16));
		ver2.Set((UInt16)(hiVer & 0xffff));
		ver3.Set((UInt16)(loVer >> 16));
		ver4.Set((UInt16)(loVer & 0xffff));
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

Bool IO::SiLabDriver::GetDeviceVID(UInt32 devId, OutParam<UInt32> vid)
{
	Char buff[32];
	if (this->SI_GetProductString(devId, buff, IO::SiLabDriver::SI_RETURN_VID) == SI_SUCCESS)
	{
		vid.Set(Text::StrHex2UInt32ChC(buff));
		return true;
	}
	return false;
}

Bool IO::SiLabDriver::GetDevicePID(UInt32 devId, OutParam<UInt32> pid)
{
	Char buff[32];
	if (this->SI_GetProductString(devId, buff, IO::SiLabDriver::SI_RETURN_PID) == SI_SUCCESS)
	{
		pid.Set(Text::StrHex2UInt32ChC(buff));
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

Optional<IO::Stream> IO::SiLabDriver::OpenPort(UInt32 devId, UInt32 baudRate)
{
	NN<IO::SiLabSerialPort> port;
	void *hand;
	if (this->SI_Open(devId, &hand) != SI_SUCCESS)
		return nullptr;
	if (this->SI_SetBaudRate(hand, baudRate) != SI_SUCCESS)
	{
		this->SI_Close(hand);
		return nullptr;
	}
	NEW_CLASSNN(port, IO::SiLabSerialPort(hand, this));
	return port;
}
