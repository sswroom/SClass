#ifndef _SM_IO_DEVICE_MTKGPSNMEA
#define _SM_IO_DEVICE_MTKGPSNMEA
#include "Data/SyncArrayList.h"
#include "IO/GPSNMEA.h"

namespace IO
{
	namespace Device
	{
		class MTKGPSNMEA : public IO::GPSNMEA
		{
		public:
			typedef enum
			{
				LM_UNKNOWN,
				LM_OVERWRITE,
				LM_STOP
			} LogMode;

			typedef enum
			{
				LF_UNKNOWN = 0,
				LF_UTC = 1,
				LF_VALID = 2,
				LF_LATITUDE = 4,
				LF_LONGITUDE = 8,
				LF_HEIGHT = 16,
				LF_SPEED = 32,
				LF_HEADING = 64,
				LF_DSTA = 0x80,
				LF_DAGE = 0x100,
				LF_PDOP = 0x200,
				LF_HDOP = 0x400,
				LF_VDOP = 0x800,
				LF_NSAT = 0x1000,
				LF_SID = 0x2000,
				LF_ELEVATION = 0x4000,
				LF_AZIMUTH = 0x8000,
				LF_SNR = 0x10000,
				LF_RCR = 0x20000,
				LF_MILISECOND = 0x40000,
				LF_DISTANCE = 0x80000
			} LogFormat;
		private:
			Sync::Mutex *cmdMut;
			Sync::Event *cmdEvt;

			Data::SyncArrayList<const UTF8Char *> *cmdWResults;

			const UTF8Char *firmwareRel;
			const UTF8Char *firmwareBuild;
			const UTF8Char *productMode;
			const UTF8Char *sdkVer;
		private:
			virtual void ParseUnknownCmd(const UTF8Char *cmd);
		public:
			MTKGPSNMEA(IO::Stream *stm, Bool relStm);
			virtual ~MTKGPSNMEA();

			virtual ServiceType GetServiceType();

			void HotStart();
			void WarmStart();
			void ColdStart();
			void FactoryReset();
			Bool IsMTKDevice();
			Bool QueryFirmware();
			Bool IsLogEnabled();
			Bool DisableLog();
			Bool EnableLog();
			UOSInt CalLogBlockCount(UOSInt logSize);
			Bool ReadLogPart(UOSInt addr, UInt8 *buff); //1024 bytes
			Bool ReadLogBlock(UOSInt addr, UInt8 *buff); //65536 bytes
			Bool ParseLog(Map::GPSTrack *gps);
			Bool DelLogData();
			Bool SetLogFormat(LogFormat lf);
			Bool SetLogInterval(Int32 sec);
			Bool SetLogDistance(Int32 meter);
			Bool SetLogSpeed(Int32 meter);
			Bool SetLogMode(LogMode lm);
			LogFormat GetLogFormat();
			Int32 GetLogInterval();
			Int32 GetLogDistance();
			Int32 GetLogSpeed();
			LogMode GetLogMode();
			UOSInt GetLogSize(); //Bytes
			const UTF8Char *SendMTKCommand(const UInt8 *cmdBuff, UOSInt cmdSize, const UTF8Char *resultStart, Int32 timeoutMS);

			const UTF8Char *GetFirmwareRel();
			const UTF8Char *GetFirmwareBuild();
			const UTF8Char *GetProductMode();
			const UTF8Char *GetSDKVer();

			static UOSInt GetMTKSerialPort();
			static Bool ParseBlock(UInt8 *block, Map::GPSTrack *gps);
		};
	}
}
#endif
