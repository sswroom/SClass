#ifndef _SM_IO_DEVICE_MTKGPSNMEA
#define _SM_IO_DEVICE_MTKGPSNMEA
#include "Data/SyncArrayListNN.hpp"
#include "IO/GPSNMEA.h"
#include "Text/String.h"

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
			Sync::Mutex cmdMut;
			Sync::Event cmdEvt;
			Data::SyncArrayListNN<Text::String> cmdWResults;

			Optional<Text::String> firmwareRel;
			Optional<Text::String> firmwareBuild;
			Optional<Text::String> productMode;
			Optional<Text::String> sdkVer;
		private:
			virtual void ParseUnknownCmd(UnsafeArray<const UTF8Char> cmd, UIntOS cmdLen);
		public:
			MTKGPSNMEA(NN<IO::Stream> stm, Bool relStm);
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
			UIntOS CalLogBlockCount(UIntOS logSize);
			Bool ReadLogPart(UIntOS addr, UnsafeArray<UInt8> buff); //1024 bytes
			Bool ReadLogBlock(UIntOS addr, UnsafeArray<UInt8> buff); //65536 bytes
			Bool ParseLog(NN<Map::GPSTrack> gps);
			Bool DelLogData();
			Bool SetLogFormat(LogFormat lf);
			Bool SetLogInterval(UInt32 sec);
			Bool SetLogDistance(UInt32 meter);
			Bool SetLogSpeed(UInt32 meter);
			Bool SetLogMode(LogMode lm);
			LogFormat GetLogFormat();
			UInt32 GetLogInterval();
			UInt32 GetLogDistance();
			UInt32 GetLogSpeed();
			LogMode GetLogMode();
			UIntOS GetLogSize(); //Bytes
			Optional<Text::String> SendMTKCommand(UnsafeArray<const UInt8> cmdBuff, UIntOS cmdSize, UnsafeArray<const UTF8Char> resultStart, UIntOS resultStartLen, Data::Duration timeout);

			Optional<Text::String> GetFirmwareRel();
			Optional<Text::String> GetFirmwareBuild();
			Optional<Text::String> GetProductMode();
			Optional<Text::String> GetSDKVer();

			static UIntOS GetMTKSerialPort();
			static Bool ParseBlock(UnsafeArray<UInt8> block, NN<Map::GPSTrack> gps);
		};
	}
}
#endif
