#ifndef _SM_IO_DEVICE_BYDC9R
#define _SM_IO_DEVICE_BYDC9R
#include "Data/Timestamp.h"
#include "IO/Device/BYDC9RHandler.h"
#include "Sync/Mutex.h"

namespace IO
{
	namespace Device
	{
		class BYDC9R : public BYDC9RHandler
		{
		public:
			struct DeviceStatus
			{
				Double speedkmHr;
				DoorStatus door1;
				DoorStatus door2;
				Double batteryLevelPercent;
				Int32 motorRPM;
				Data::Timestamp batteryChargedStart;
				MotorMode leftMotorMode;
				MotorMode rightMotorMode;
				Bool carbinDoorBack;
				Bool carbinDoorLeft;
				Bool carbinDoorRight;
				PowerStatus powerMode;
				ChargingStatus batteryCharging;
				Bool okLED;
			};
		private:
			Sync::Mutex statusMut;
			DeviceStatus status;
		public:
			BYDC9R();
			virtual ~BYDC9R();

			virtual void VehicleSpeed(Double speedkmHr);
			virtual void VehicleDoor(DoorStatus door1, DoorStatus door2);
			virtual void BatteryLevel(Double percent);
			virtual void MotorRPM(Int32 rpm);
			virtual void BatteryChargedTime(UInt32 minutes);
			virtual void LeftMotorMode(MotorMode mode);
			virtual void RightMotorMode(MotorMode mode);
			virtual void CarbinDoor(Bool backOpened, Bool leftOpened, Bool rightOpened);
			virtual void PowerMode(PowerStatus status);
			virtual void BatteryCharging(ChargingStatus status);
			virtual void OKLED(Bool ledOn);

			void GetStatus(DeviceStatus *status);
			CANHandler *GetCANHandler();
		};
	}
}
#endif
