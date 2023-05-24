#include "Stdafx.h"
#include "IO/Device/BYDC9R.h"
#include "Sync/MutexUsage.h"

IO::Device::BYDC9R::BYDC9R()
{
	this->status.speedkmHr = 0;
	this->status.door1 = DoorStatus::Closed;
	this->status.door2 = DoorStatus::Closed;
	this->status.batteryLevelPercent = 0;
	this->status.motorRPM = 0;
	this->status.batteryChargedStart = 0;
	this->status.leftMotorMode = MotorMode::DriveMode;
	this->status.rightMotorMode = MotorMode::DriveMode;
	this->status.carbinDoorBack = false;
	this->status.carbinDoorLeft = false;
	this->status.carbinDoorRight = false;
	this->status.powerMode = PowerStatus::Invalid;
	this->status.batteryCharging = ChargingStatus::NotCharging;
	this->status.okLED = false;
}

IO::Device::BYDC9R::~BYDC9R()
{

}

void IO::Device::BYDC9R::VehicleSpeed(Double speedkmHr)
{
	Sync::MutexUsage mutUsage(&this->statusMut);
	this->status.speedkmHr = speedkmHr;
}

void IO::Device::BYDC9R::VehicleDoor(DoorStatus door1, DoorStatus door2)
{
	Sync::MutexUsage mutUsage(&this->statusMut);
	this->status.door1 = door1;
	this->status.door2 = door2;
}

void IO::Device::BYDC9R::BatteryLevel(Double percent)
{
	Sync::MutexUsage mutUsage(&this->statusMut);
	this->status.batteryLevelPercent = percent;
}

void IO::Device::BYDC9R::MotorRPM(Int32 rpm)
{
	Sync::MutexUsage mutUsage(&this->statusMut);
	this->status.motorRPM = rpm;
}

void IO::Device::BYDC9R::BatteryChargedTime(UInt32 minutes)
{
	Sync::MutexUsage mutUsage(&this->statusMut);
	this->status.batteryChargedStart = Data::Timestamp::Now().AddMinute(-(OSInt)minutes);
}

void IO::Device::BYDC9R::LeftMotorMode(MotorMode mode)
{
	Sync::MutexUsage mutUsage(&this->statusMut);
	this->status.leftMotorMode = mode;
}

void IO::Device::BYDC9R::RightMotorMode(MotorMode mode)
{
	Sync::MutexUsage mutUsage(&this->statusMut);
	this->status.rightMotorMode = mode;
}

void IO::Device::BYDC9R::CarbinDoor(Bool backOpened, Bool leftOpened, Bool rightOpened)
{
	Sync::MutexUsage mutUsage(&this->statusMut);
	this->status.carbinDoorBack = backOpened;
	this->status.carbinDoorLeft = leftOpened;
	this->status.carbinDoorRight = rightOpened;
}

void IO::Device::BYDC9R::PowerMode(PowerStatus status)
{
	Sync::MutexUsage mutUsage(&this->statusMut);
	this->status.powerMode = status;
}

void IO::Device::BYDC9R::BatteryCharging(ChargingStatus status)
{
	Sync::MutexUsage mutUsage(&this->statusMut);
	this->status.batteryCharging = status;
}

void IO::Device::BYDC9R::OKLED(Bool ledOn)
{
	Sync::MutexUsage mutUsage(&this->statusMut);
	this->status.okLED = ledOn;
}

void IO::Device::BYDC9R::GetStatus(DeviceStatus *status)
{
	Sync::MutexUsage mutUsage(&this->statusMut);
	MemCopyNO(status, &this->status, sizeof(DeviceStatus));
}

IO::CANHandler *IO::Device::BYDC9R::GetCANHandler()
{
	return this;
}
