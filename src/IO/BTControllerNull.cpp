#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/BTController.h"
#include "Text/MyString.h"

IO::BTController::BTDevice::BTDevice(void *internalData, void *hRadio, void *devInfo)
{
}

IO::BTController::BTDevice::~BTDevice()
{
}

const UTF8Char *IO::BTController::BTDevice::GetName()
{
	return 0;
}

UInt8 *IO::BTController::BTDevice::GetAddress()
{
	return 0;
}

UInt32 IO::BTController::BTDevice::GetDevClass()
{
	return 0;
}

Bool IO::BTController::BTDevice::IsConnected()
{
	return false;
}

Bool IO::BTController::BTDevice::IsRemembered()
{
	return false;
}

Bool IO::BTController::BTDevice::IsAuthenticated()
{
	return false;
}

void IO::BTController::BTDevice::GetLastSeen(Data::DateTime *dt)
{
	dt->SetTicks(0);
}

void IO::BTController::BTDevice::GetLastUsed(Data::DateTime *dt)
{
	dt->SetTicks(0);
}

IO::BTController::BTController(void *internalData, void *hand)
{
	this->addr[0] = 0;
	this->addr[1] = 0;
	this->addr[2] = 0;
	this->addr[3] = 0;
	this->addr[4] = 0;
	this->addr[5] = 0;
	this->name = 0;
	this->devClass = 0;
	this->manufacturer = 0;
	this->subversion = 0;
}

IO::BTController::~BTController()
{
}

OSInt IO::BTController::CreateDevices(Data::ArrayList<BTDevice*> *devList, Bool toSearch)
{
	return 0;
}

UInt8 *IO::BTController::GetAddress()
{
	return this->addr;
}

Text::String *IO::BTController::GetName()
{
	return this->name;
}

UInt32 IO::BTController::GetDevClass()
{
	return this->devClass;
}

UInt16 IO::BTController::GetManufacturer()
{
	return this->manufacturer;
}

UInt16 IO::BTController::GetSubversion()
{
	return this->subversion;
}

void IO::BTController::LEScanHandleResult(LEScanHandler leHdlr, void *leHdlrObj)
{

}

Bool IO::BTController::LEScanBegin()
{
	return false;
}

Bool IO::BTController::LEScanEnd()
{
	return false;
}
