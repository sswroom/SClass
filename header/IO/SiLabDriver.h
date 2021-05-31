#ifndef _SM_IO_SILABDRIVER
#define _SM_IO_SILABDRIVER
#include "IO/Library.h"
#include "IO/Stream.h"

namespace IO
{
	class SiLabDriver
	{
	public:
		typedef enum
		{
			SI_SUCCESS = 0x00,
			SI_DEVICE_NOT_FOUND = 0xFF,
			SI_INVALID_HANDLE = 0x01,
			SI_READ_ERROR = 0x02,
			SI_WRITE_ERROR = 0x04,
			SI_RESET_ERROR = 0x05,
			SI_INVALID_PARAMETER = 0x06,
			SI_INVALID_REQUEST_LENGTH = 0x07,
			SI_DEVICE_IO_FAILED = 0x08,
			SI_INVALID_BAUDRATE = 0x09,
			SI_FUNCTION_NOT_SUPPORTED = 0x0a,
			SI_GLOBAL_DATA_ERROR = 0x0b,
			SI_SYSTEM_ERROR_CODE = 0x0c,
			SI_READ_TIMED_OUT = 0x0d,
			SI_WRITE_TIMED_OUT = 0x0e,
			SI_IO_PENDING = 0x0f,
			SI_FUNC_NOT_FOUND = 0xffffffff
		} SI_STATUS;

		typedef enum
		{
			SI_RETURN_SERIAL_NUMBER = 0x00,
			SI_RETURN_DESCRIPTION = 0x01,
			SI_RETURN_LINK_NAME = 0x02,
			SI_RETURN_VID = 0x03,
			SI_RETURN_PID = 0x04
		} SI_RETURN;

	private:
		IO::Library *driver;

	public:
		SI_STATUS SI_GetNumDevices(UInt32 *NumDevices);
		SI_STATUS SI_GetProductString(UInt32 DeviceNum, void *DeviceString, SI_RETURN Options); //UInt32 Options);
		SI_STATUS SI_Open(UInt32 DeviceNum, void **Handle);
		SI_STATUS SI_Close(void *Handle);
		SI_STATUS SI_Read(void *Handle, void *Buffer, UInt32 NumBytesToRead, UInt32 *NumBytesReturned, void* o = 0);
		SI_STATUS SI_Write(void *Handle, void *Buffer, UInt32 NumBytesToWrite, UInt32 *NumBytesWritten, void* o = 0);
		SI_STATUS SI_CancelIo(void *Handle);
		SI_STATUS SI_FlushBuffers(void *Handle, UInt8 FlushTransmit, UInt8 FlushReceive);
		SI_STATUS SI_SetTimeouts(UInt32 ReadTimeout, UInt32 WriteTimeout);
		SI_STATUS SI_GetTimeouts(UInt32 *ReadTimeout, UInt32 *WriteTimeout);
		SI_STATUS SI_CheckRXQueue(void *Handle, UInt32 *NumBytesInQueue, UInt32 *QueueStatus);
		SI_STATUS SI_SetBaudRate(void *Handle, UInt32 dwBaudRate);
		SI_STATUS SI_SetBaudDivisor(void *Handle, UInt16 wBaudDivisor);
		SI_STATUS SI_SetLineControl(void *Handle, UInt16 wLineControl);
		SI_STATUS SI_SetFlowControl(void *Handle, UInt8 bCTS_MaskCode, UInt8 bRTS_MaskCode, UInt8 bDTR_MaskCode, UInt8 bDSRMaskCode, UInt8 bDCD_MaskCode, UInt8 bFlowXonXoff);
		SI_STATUS SI_GetModemStatus(void *Handle, UInt8 *ModemStatus);
		SI_STATUS SI_SetBreak(void *cyHandle, UInt16 wBreakState);
		SI_STATUS SI_ReadLatch(void *Handle, UInt8 *Latch);
		SI_STATUS SI_WriteLatch(void *Handle, UInt8 Mask, UInt8 Latch);
		SI_STATUS SI_GetPartNumber(void *Handle, UInt8 *PartNum);
		SI_STATUS SI_DeviceIOControl(void *Handle, UInt32 IOControlCode, void *InBuffer, UInt32 BytesToRead, void *OutBuffer, UInt32 BytesToWrite, UInt32 *BytesSucceeded);
		SI_STATUS SI_GetDLLVersion(UInt32* HighVersion, UInt32* LowVersion);
		SI_STATUS SI_GetDriverVersion(UInt32* HighVersion, UInt32* LowVersion);

	public:
		SiLabDriver();
		~SiLabDriver();

		Bool IsError();

		Bool GetDLLVersion(UInt16 *ver1, UInt16 *ver2, UInt16 *ver3, UInt16 *ver4);
		Bool GetDriverVersion(UInt16 *ver1, UInt16 *ver2, UInt16 *ver3, UInt16 *ver4);
		UInt32 GetNumDevices();
		Bool GetDeviceVID(UInt32 devId, UInt32 *vid);
		Bool GetDevicePID(UInt32 devId, UInt32 *pid);
		UTF8Char *GetDeviceSN(UInt32 devId, UTF8Char *buff);
		UTF8Char *GetDeviceDesc(UInt32 devId, UTF8Char *buff);
		UTF8Char *GetDeviceLink(UInt32 devId, UTF8Char *buff);
		IO::Stream *OpenPort(UInt32 devId, UInt32 baudRate);
	};
}
#endif
