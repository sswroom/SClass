#ifndef _SM_IO_DEVICE_BYDC9RHANDLER
#define _SM_IO_DEVICE_BYDC9RHANDLER
#include "IO/CANHandler.h"
#include "Text/CString.h"

namespace IO
{
	namespace Device
	{
		class BYDC9RHandler : public IO::CANHandler
		{
		public:
			enum class DoorStatus
			{
				Closed,
				Opened,
				Reserved
			};
			enum class MotorMode
			{
				DriveMode,
				FeedbackMode,
				OffMode,
				Reserved
			};
			enum class PowerStatus
			{
				Invalid,
				Off,
				Reserved,
				On
			};
			enum class ChargingStatus
			{
				NotCharging,
				Charging,
				ChargeComplete,
				ChargeTerminated
			};
		public:
			BYDC9RHandler();
			virtual ~BYDC9RHandler();

			virtual void CANMessage(UInt32 id, Bool rtr, Data::ByteArrayR message);

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

			static Text::CString DoorStatusGetName(DoorStatus doorStatus);
			static Text::CString MotorModeGetName(MotorMode motorMode);
			static Text::CString PowerStatusGetName(PowerStatus powerStatus);
			static Text::CString ChargingStatusGetName(ChargingStatus chargingStatus);
		};
	}
}
#endif
