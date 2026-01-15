#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "IO/BTController.h"
#include "Text/MyString.h"

IO::BTController::BTDevice::BTDevice(void *internalData, void *hRadio, void *devInfo)
{
}

IO::BTController::BTDevice::~BTDevice()
{
}

NN<Text::String> IO::BTController::BTDevice::GetName() const
{
	return Text::String::NewEmpty();
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

Bool IO::BTController::BTDevice::Pair(const UTF8Char *key)
{
	///////////////////////////////
	return false;
}

Bool IO::BTController::BTDevice::Unpair()
{
	///////////////////////////////
	return false;
}

UIntOS IO::BTController::BTDevice::QueryServices(Data::ArrayList<void*> *guidList)
{
	///////////////////////////////
	return 0;
}

void IO::BTController::BTDevice::FreeServices(Data::ArrayList<void*> *guidList)
{
	///////////////////////////////
}

Bool IO::BTController::BTDevice::EnableService(void *guid, Bool toEnable)
{
	///////////////////////////////
	return false;
}

IO::BTController::BTController(void *internalData, void *hand)
{
	this->addr[0] = 0;
	this->addr[1] = 0;
	this->addr[2] = 0;
	this->addr[3] = 0;
	this->addr[4] = 0;
	this->addr[5] = 0;
	this->name = Text::String::NewEmpty();
	this->devClass = 0;
	this->manufacturer = 0;
	this->subversion = 0;
}

IO::BTController::~BTController()
{
	this->name->Release();
}

IntOS IO::BTController::CreateDevices(Data::ArrayList<BTDevice*> *devList, Bool toSearch)
{
	return 0;
}

UInt8 *IO::BTController::GetAddress()
{
	return this->addr;
}

NN<Text::String> IO::BTController::GetName() const
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

void IO::BTController::LEScanHandleResult2(LEScanHandler leHdlr, AnyType leHdlrObj)
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
