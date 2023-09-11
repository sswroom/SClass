#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Device/BYDC9RHandler.h"

IO::Device::BYDC9RHandler::BYDC9RHandler()
{
}

IO::Device::BYDC9RHandler::~BYDC9RHandler()
{
}

void IO::Device::BYDC9RHandler::CANMessage(UInt32 id, Bool rtr, Data::ByteArrayR message)
{
	UInt16 v;
	if (message.GetSize() == 8)
	{
		switch (id)
		{
		case 0x18FEF100:
			v = message.ReadU16(1);
			if (v <= 0xFAFF)
			{
				this->VehicleSpeed(v / 256.0);
			}
			break;
		case 0x18FED925:
			this->VehicleDoor((DoorStatus)((message[5] >> 4) & 3), (DoorStatus)((message[5] >> 6) & 3));
			break;
		case 0x18FC07F4:
			v = message.ReadU16(4);
			if (v <= 0x3E8)
			{
				this->BatteryLevel(v * 0.1);
			}
			break;
		case 0x18FC1721:
			v = message.ReadU16(0);
			if (v <= 0x55F0)
			{
				this->MotorRPM((Int32)v - 11000);
			}
			this->LeftMotorMode((MotorMode)(message[6] & 3));
			break;
		case 0x18FC1521:
			this->RightMotorMode((MotorMode)(message[6] & 3));
			break;
		case 0x181D00F4:
			v = message.ReadU16(0);
			if (v <= 0x258)
			{
				this->BatteryChargedTime(v);
			}
			break;
		case 0x18FAF321:
			this->CarbinDoor(message[6] & 2, message[6] & 4, message[6] & 8);
			break;
		case 0x0CFAE621:
			this->PowerMode((PowerStatus)((message[4] >> 2) & 3));
			break;
		case 0x18FC04F4:
			this->BatteryCharging((ChargingStatus)(message[1] >> 6));
			break;
		case 0x18F31F00:
			v = (UInt16)((message[0] >> 2) & 3);
			if (v <= 1)
			{
				this->OKLED(v != 0);
			}
			break;
		}
	}
}

void IO::Device::BYDC9RHandler::VehicleSpeed(Double speedkmHr)
{
}

void IO::Device::BYDC9RHandler::VehicleDoor(DoorStatus door1, DoorStatus door2)
{
}

void IO::Device::BYDC9RHandler::BatteryLevel(Double percent)
{
}

void IO::Device::BYDC9RHandler::MotorRPM(Int32 rpm)
{
}

void IO::Device::BYDC9RHandler::BatteryChargedTime(UInt32 minutes)
{
}

void IO::Device::BYDC9RHandler::LeftMotorMode(MotorMode mode)
{
}

void IO::Device::BYDC9RHandler::RightMotorMode(MotorMode mode)
{
}

void IO::Device::BYDC9RHandler::CarbinDoor(Bool backOpened, Bool leftOpened, Bool rightOpened)
{
}

void IO::Device::BYDC9RHandler::PowerMode(PowerStatus status)
{
}

void IO::Device::BYDC9RHandler::BatteryCharging(ChargingStatus status)
{
}

void IO::Device::BYDC9RHandler::OKLED(Bool ledOn)
{
}

Text::CStringNN IO::Device::BYDC9RHandler::DoorStatusGetName(DoorStatus doorStatus)
{
	switch (doorStatus)
	{
	case DoorStatus::Closed:
		return CSTR("Closed");
	case DoorStatus::Opened:
		return CSTR("Opened");
	case DoorStatus::Reserved:
		return CSTR("Reserved");
	default:
		return CSTR("Unknown");
	}
}

Text::CStringNN IO::Device::BYDC9RHandler::MotorModeGetName(MotorMode motorMode)
{
	switch (motorMode)
	{
	case MotorMode::DriveMode:
		return CSTR("DriveMode");
	case MotorMode::FeedbackMode:
		return CSTR("FeedbackMode");
	case MotorMode::OffMode:
		return CSTR("OffMode");
	case MotorMode::Reserved:
		return CSTR("Reserved");
	default:
		return CSTR("Unknown");
	}
}

Text::CStringNN IO::Device::BYDC9RHandler::PowerStatusGetName(PowerStatus powerStatus)
{
	switch (powerStatus)
	{
	case PowerStatus::Invalid:
		return CSTR("Invalid");
	case PowerStatus::Off:
		return CSTR("Off");
	case PowerStatus::Reserved:
		return CSTR("Reserved");
	case PowerStatus::On:
		return CSTR("On");	
	default:
		return CSTR("Unknown");
	}
}

Text::CStringNN IO::Device::BYDC9RHandler::ChargingStatusGetName(ChargingStatus chargingStatus)
{
	switch (chargingStatus)
	{
	case ChargingStatus::NotCharging:
		return CSTR("NotCharging");
	case ChargingStatus::Charging:
		return CSTR("Charging");
	case ChargingStatus::ChargeComplete:
		return CSTR("ChargeComplete");
	case ChargingStatus::ChargeTerminated:
		return CSTR("ChargeTerminated");
	default:
		return CSTR("Unknown");
	}
}
