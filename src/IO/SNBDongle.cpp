#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/SNBDongle.h"

void __stdcall IO::SNBDongle::OnProtocolRecv(void *userObj, UInt8 cmdType, UOSInt cmdSize, UInt8 *cmd)
{
	IO::SNBDongle *me = (IO::SNBDongle*)userObj;
	DeviceInfo *dev;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Double readingVals[16];
	ReadingType readingTypes[16];
	SensorType sensorType;
	if (me->protoHdlr)
	{
		me->protoHdlr(me->protoObj, cmdType, cmdSize, cmd);
	}
	switch (cmdType)
	{
	case 0xb3:
		if (cmdSize >= 12)
		{
			Bool valid = true;
			dev = me->GetDevice(ReadUInt64(&cmd[0]));
			sensorType = ST_CUSTOM;
			readingTypes[0] = RT_ONOFF;
			dev->param = cmd[8];
			if (cmd[8] == 2)
			{
				readingVals[0] = 0;
			}
			else if (cmd[8] == 1)
			{
				readingVals[0] = 1;
			}
			else
			{
				valid = false;
			}
			k = 1;
			if (dev->handType == SNBDongle::HT_MOBILEPLUG)
			{
				readingTypes[1] = RT_UNKNOWN;
				readingVals[1] = 0;
				k = 2;
			}

			if (valid)
			{
				me->hdlr->DeviceSensor(dev->devId, sensorType, k, readingTypes, readingVals);
			}
		}
		break;
	case 0xc3:
		if (cmdSize >= 17)
		{
			me->dongleId = ReadUInt64(&cmd[3]);
			if (cmd[16] == 4)
			{
				me->dongleBaudRate = 115200;
			}
			else if (cmd[16] == 1)
			{
				me->dongleBaudRate = 19200;
			}
			else if (cmd[16] == 2)
			{
				me->dongleBaudRate = 38400;
			}
			else if (cmd[16] == 3)
			{
				me->dongleBaudRate = 57600;
			}
			else if (cmd[16] == 5)
			{
				me->dongleBaudRate = 9600;
			}
			else if (cmd[16] == 6)
			{
				me->dongleBaudRate = 4800;
			}
			else if (cmd[16] == 7)
			{
				me->dongleBaudRate = 2400;
			}
			else if (cmd[16] == 8)
			{
				me->dongleBaudRate = 14400;
			}
			else if (cmd[16] == 9)
			{
				me->dongleBaudRate = 28800;
			}
			else if (cmd[16] == 10)
			{
				me->dongleBaudRate = 230400;
			}
			else if (cmd[16] == 11)
			{
				me->dongleBaudRate = 76800;
			}
			else
			{
				me->dongleBaudRate = 0;
			}
			me->hdlr->DongleInfoUpdated();
		}
		break;
	case 0xc6:
		if (cmdSize >= 17)
		{
			dev = me->GetDevice(ReadUInt64(cmd));
			if (dev->shortAddr != ReadUInt16(&cmd[8]))
			{
				dev->shortAddr = ReadUInt16(&cmd[8]);
				me->hdlr->DeviceUpdated(dev->devId, dev->shortAddr);
			}
			dev->devType = (DeviceType)cmd[10];
			dev->sensorCount = cmd[11];
			dev->versionMajor = cmd[12];
			dev->versionMinor = cmd[13];
			dev->param = ReadInt16(&cmd[14]);
			dev->sensorType = cmd[16];
		}
		else if (cmdSize >= 14)
		{
			dev = me->GetDevice(ReadUInt64(cmd));
			if (dev->shortAddr != ReadUInt16(&cmd[8]))
			{
				dev->shortAddr = ReadUInt16(&cmd[8]);
				me->hdlr->DeviceUpdated(dev->devId, dev->shortAddr);
			}
			dev->devType = (DeviceType)cmd[10];
			dev->sensorCount = cmd[11];
			dev->versionMajor = cmd[12];
			dev->versionMinor = cmd[13];
		}
		break;
	case 0xdc:
		if (cmdSize >= 10)
		{
			dev = me->GetDevice(ReadUInt64(cmd));
			if (dev->shortAddr != ReadUInt16(&cmd[8]))
			{
				dev->shortAddr = ReadUInt16(&cmd[8]);
				me->hdlr->DeviceUpdated(dev->devId, dev->shortAddr);
			}
			dev->devType = IO::SNBDongle::DT_UNKNOWN;
			dev->sensorCount = 0;
			dev->versionMajor = 0;
			dev->versionMinor = 0;
		}
		break;
	case 0xdd:
		if (cmdSize >= 14)
		{
			Bool valid = false;
			dev = me->GetDevice(ReadUInt64(&cmd[1]));
			switch (cmd[0])
			{
			case 3:
				sensorType = ST_DOOR_CONTACT;
				readingTypes[0] = RT_DOOR_CONTACT;
				if (cmd[9] == 1)
				{
					readingVals[0] = 1;
				}
				else
				{
					readingVals[0] = 0;
				}
				readingTypes[1] = RT_VOLTAGE;
				readingVals[1] = ReadUInt16(&cmd[10]) * 0.001;
				k = 2;
				valid = true;
				break;
			case 4:
				sensorType = ST_MOTION;
				readingTypes[0] = RT_VOLTAGE;
				readingVals[0] = ReadUInt16(&cmd[10]) * 0.001;
				readingTypes[1] = RT_MOVING_SENSOR;
				readingVals[1] = cmd[9];
				k = 2;
				valid = true;
				break;
			case 32:
				sensorType = ST_WATERSENSOR;
				readingTypes[0] = RT_VOLTAGE;
				readingVals[0] = ReadUInt16(&cmd[10]) * 0.001;
				readingTypes[1] = RT_ALERT;
				readingVals[1] = 1;
				k = 2;
				valid = true;
				break;
			case 0xFA:
			case 0xFB:
				sensorType = (SensorType)cmd[0];
				k = 0;
				i = 9;
				j = cmdSize - 2;
				while (i < j)
				{
					readingTypes[k] = (ReadingType)cmd[i];
					switch (readingTypes[k])
					{
					case RT_TEMPERATURE:
					case RT_AIR_HUMIDITY:
						readingVals[k] = ReadInt16(&cmd[i + 1]) * 0.01;
						break;
					case RT_AIR_HCHO:
					case RT_AIR_VOC:
						readingVals[k] = ReadInt16(&cmd[i + 1]) * 0.001;
						break;
					default:
						readingVals[k] = ReadInt16(&cmd[i + 1]);
						break;
					}
					i += 3;
					k++;
				}
				valid = true;
				break;
			case 0xFC:
				sensorType = ST_RETURN_NORMAL;
				if (dev->handType == SNBDongle::HT_MOTION)
				{
					readingTypes[0] = RT_VOLTAGE;
					readingVals[0] = ReadUInt16(&cmd[10]) * 0.001;
					readingTypes[1] = RT_MOVING_SENSOR;
					readingVals[1] = cmd[9];
					k = 2;
					valid = true;
				}
				else if (dev->handType == SNBDongle::HT_WATERLEAKAGE)
				{
					readingTypes[0] = RT_VOLTAGE;
					readingVals[0] = ReadUInt16(&cmd[10]) * 0.001;
					readingTypes[1] = RT_ALERT;
					readingVals[1] = 0;
					k = 2;
					valid = true;
				}
				break;
			case 0xFE:
				sensorType = ST_CUSTOM;
				if (dev->handType == SNBDongle::HT_DOOR)
				{
					if (cmdSize == 14)
					{
						valid = true;
						readingTypes[0] = RT_UNKNOWN;
						readingVals[0] = cmd[9];
						readingTypes[1] = RT_VOLTAGE;
						readingVals[1] = ReadUInt16(&cmd[10]) * 0.001;
						k = 2;
					}
				}
				else if (dev->handType == SNBDongle::HT_TEMPERATURE1)
				{
					if (cmdSize == 14)
					{
						valid = true;
						readingTypes[0] = RT_TEMPERATURE;
						readingVals[0] = ReadInt16(&cmd[9]) * 0.1;
						readingTypes[1] = RT_AIR_HUMIDITY;
						readingVals[1] = cmd[13];
						if (readingVals[1] > 100)
						{
							readingTypes[0] = RT_UNKNOWN;
							readingTypes[1] = RT_UNKNOWN;
						}
						k = 2;
					}
				}
				else if (dev->handType == SNBDongle::HT_TEMPERATURE2)
				{
					if (cmdSize == 22)
					{
						valid = true;
						readingTypes[0] = RT_VOLTAGE;
						readingVals[0] = ReadInt16(&cmd[10]) * 0.001;
						readingTypes[1] = RT_TEMPERATURE;
						readingVals[1] = ReadInt16(&cmd[14]) * 0.01;
						readingTypes[2] = RT_AIR_HUMIDITY;
						readingVals[2] = cmd[17];
						if (readingVals[2] > 100)
						{
							readingTypes[1] = RT_UNKNOWN;
							readingTypes[2] = RT_UNKNOWN;
						}
						k = 3;
					}
				}
				else if (dev->handType == SNBDongle::HT_TEMPERATURE2V2)
				{
					if (cmdSize == 22)
					{
						valid = true;
						readingTypes[0] = RT_VOLTAGE;
						readingVals[0] = ReadInt16(&cmd[10]) * 0.001;
						readingTypes[1] = RT_TEMPERATURE;
						readingVals[1] = ReadInt16(&cmd[14]) * 0.01;
						readingTypes[2] = RT_AIR_HUMIDITY;
						readingVals[2] = ReadInt16(&cmd[17]) * 0.01;
						if (readingVals[2] > 100)
						{
							readingTypes[1] = RT_UNKNOWN;
							readingTypes[2] = RT_UNKNOWN;
						}
						k = 3;
					}
					else if (cmdSize == 28)
					{
						valid = true;
						readingTypes[0] = RT_VOLTAGE;
						readingVals[0] = ReadInt16(&cmd[10]) * 0.001;
						readingTypes[1] = RT_TEMPERATURE;
						readingVals[1] = ReadInt16(&cmd[14]) * 0.01;
						readingTypes[2] = RT_AIR_HUMIDITY;
						readingVals[2] = ReadInt16(&cmd[17]) * 0.01;
						if (readingVals[2] > 100)
						{
							readingTypes[1] = RT_UNKNOWN;
							readingTypes[2] = RT_UNKNOWN;
						}
						k = 3;
					}
				}
				else if (dev->handType == SNBDongle::HT_WATERLEAKAGE)
				{
					if (cmdSize == 19)
					{
						valid = true;
						readingTypes[0] = RT_VOLTAGE;
						readingVals[0] = ReadInt16(&cmd[10]) * 0.001;
						readingTypes[1] = RT_ALERT;
						readingVals[1] = ReadInt16(&cmd[14]) != 2;
						k = 2;
					}
				}
				break;
			}
			if (valid)
			{
				me->hdlr->DeviceSensor(dev->devId, sensorType, k, readingTypes, readingVals);
			}
		}
		break;
	case 0xe7:
		if (cmdSize >= 24)
		{
			dev = me->GetDevice(ReadUInt64(cmd));
			sensorType = ST_CUSTOM;
			if (dev->handType == IO::SNBDongle::HT_MOBILEPLUG)
			{
				if (dev->param == 1)
				{
					readingTypes[0] = RT_ONOFF;
					readingVals[0] = 1;
				}
				else if (dev->param == 2)
				{
					readingTypes[0] = RT_ONOFF;
					readingVals[0] = 0;
				}
				else
				{
					readingTypes[0] = RT_UNKNOWN;
					readingVals[0] = 0;
				}
				readingTypes[1] = RT_POWER;
				readingVals[1] = ReadInt32(&cmd[16]) * 0.1;
				me->hdlr->DeviceSensor(dev->devId, sensorType, 2, readingTypes, readingVals);
			}
		}
		break;
	}
}

IO::SNBDongle::DeviceInfo *IO::SNBDongle::GetDevice(UInt64 devId)
{
	DeviceInfo *dev;
	Bool upd = false;
	this->devMut->LockWrite();
	dev = this->devMap->Get(devId);
	if (dev == 0)
	{
		dev = MemAlloc(DeviceInfo, 1);
		upd = true;
		dev->devId = devId;
		dev->devType = DT_UNKNOWN;
		dev->handType = HT_UNKNOWN;
		dev->param = 0;
		dev->sensorCount = 0;
		dev->sensorType = 0;
		dev->shortAddr = 0;
		dev->versionMinor = 0;
		dev->versionMajor = 0;
		this->devMap->Put(devId, dev);
	}
	this->devMut->UnlockWrite();
	if (upd)
	{
		this->hdlr->DeviceAdded(devId);
	}
	return dev;
}

IO::SNBDongle::SNBDongle(IO::Stream *stm, SNBHandler *hdlr)
{
	this->protoHdlr = 0;
	this->protoObj = 0;
	this->hdlr = hdlr;
	this->dongleBaudRate = 0;
	this->dongleId = 0;
	NEW_CLASS(this->proto, IO::SNBProtocol(stm, OnProtocolRecv, this));
	NEW_CLASS(this->devMap, Data::UInt64Map<DeviceInfo*>());
	NEW_CLASS(this->devMut, Sync::RWMutex());
}

IO::SNBDongle::~SNBDongle()
{
	DEL_CLASS(this->proto);
	DEL_CLASS(this->devMut);
	DeviceInfo *dev;
	Data::ArrayList<DeviceInfo*> *devList = this->devMap->GetValues();
	UOSInt i;
	i = devList->GetCount();
	while (i-- > 0)
	{
		dev = devList->GetItem(i);
		MemFree(dev);
	}
	DEL_CLASS(this->devMap);
}

void IO::SNBDongle::SetDevHandleType(UInt64 devId, HandleType handType)
{
	DeviceInfo *dev;
	dev = this->GetDevice(devId);
	dev->handType = handType;
}

void IO::SNBDongle::SetDevShortAddr(UInt64 devId, UInt16 shortAddr)
{
	DeviceInfo *dev;
	dev = this->GetDevice(devId);
	dev->shortAddr = shortAddr;
}

UInt64 IO::SNBDongle::GetDongleId()
{
	return this->dongleId;
}

UInt32 IO::SNBDongle::GetBaudRate()
{
	return this->dongleBaudRate;
}

void IO::SNBDongle::HandleProtocolReceived(IO::SNBProtocol::ProtocolHandler protoHdlr, void *userObj)
{
	this->protoHdlr = protoHdlr;
	this->protoObj = userObj;
}

void IO::SNBDongle::SendCheckDongle()
{
	this->proto->SendCommand(0xc2, 0, 0);
}

void IO::SNBDongle::SendGetDongleInfo()
{
	this->proto->SendCommand(0xc3, 0, 0);
}

void IO::SNBDongle::SendCheckDevices()
{
	this->proto->SendCommand(0xc6, 0, 0);
}

void IO::SNBDongle::SendResetNetwork()
{
	this->proto->SendCommand(0xc4, 0, 0);
}

void IO::SNBDongle::SendAddDevice(UInt8 timeout)
{
	this->proto->SendCommand(0xc7, 1, &timeout);
}

void IO::SNBDongle::SendSetReportTime(UInt64 devId, Int32 interval)
{
	UInt8 buff[20];
	DeviceInfo *dev;
	dev = this->GetDevice(devId);
	if (dev->handType == IO::SNBDongle::HT_MOBILEPLUG)
	{
		interval = interval / 10;
		if (interval <= 0)
			return;

		WriteUInt64(&buff[0], dev->devId);
		buff[8] = 0xe3;
		buff[9] = 2;
		buff[10] = 0x2a;
		buff[11] = 0x0a;
		buff[12] = 0xff;
		buff[13] = (UInt8)interval;
		this->proto->SendCommand(0xb1, 14, buff);
	}
/*	else
	{
		interval = interval / 10;
		if (interval <= 0)
			return;

		WriteInt64(&buff[0], devId);
		buff[8] = 0xe0;
		WriteInt16(&buff[9], interval | 0x4000);
		this->proto->SendCommand(0xb1, 11, buff);
	}*/
}

void IO::SNBDongle::SendGetReportTime(UInt64 devId)
{
	UInt8 buff[9];
	WriteUInt64(&buff[0], devId);
	buff[8] = 0xe0;
	this->proto->SendCommand(0xb2, 9, buff);
}

Bool IO::SNBDongle::SendDevTurnOn(UInt64 devId)
{
	UInt8 buff[16];
	DeviceInfo *dev;
	dev = this->GetDevice(devId);
	if (dev->handType == IO::SNBDongle::HT_MOBILEPLUG)
	{
		if (dev->shortAddr == 0)
		{
			return false;
		}
		WriteUInt16(&buff[0], dev->shortAddr);
		WriteUInt64(&buff[2], dev->devId);
		buff[10] = 1;
		buff[11] = 1;
		buff[12] = 0;
		this->proto->SendCommand(0xcb, 13, buff);
		return true;
	}
	return false;
}

Bool IO::SNBDongle::SendDevTurnOff(UInt64 devId)
{
	UInt8 buff[16];
	DeviceInfo *dev;
	dev = this->GetDevice(devId);
	if (dev->handType == IO::SNBDongle::HT_MOBILEPLUG)
	{
		if (dev->shortAddr == 0)
		{
			return false;
		}
		WriteUInt16(&buff[0], dev->shortAddr);
		WriteUInt64(&buff[2], dev->devId);
		buff[10] = 1;
		buff[11] = 2;
		buff[12] = 0;
		this->proto->SendCommand(0xcb, 13, buff);
		return true;
	}
	return false;
}

Bool IO::SNBDongle::SendDevGetStatus(UInt64 devId)
{
	UInt8 buff[16];
	DeviceInfo *dev;
	dev = this->GetDevice(devId);
	if (dev->handType == IO::SNBDongle::HT_MOBILEPLUG)
	{
		if (dev->shortAddr == 0)
		{
			return false;
		}
		WriteUInt16(&buff[0], dev->shortAddr);
		this->proto->SendCommand(0xe7, 2, buff);
		return true;
	}
	return false;
}

const UTF8Char *IO::SNBDongle::GetHandleName(HandleType handType)
{
	switch (handType)
	{
	case HT_DOOR:
		return (const UTF8Char*)"Door";
	case HT_TEMPERATURE1:
		return (const UTF8Char*)"Temperature type1";
	case HT_TEMPERATURE2:
		return (const UTF8Char*)"Temperature type2";
	case HT_MOTION:
		return (const UTF8Char*)"Motion";
	case HT_TEMPERATURE2V2:
		return (const UTF8Char*)"Temperature type2 v2/v3";
	case HT_MOBILEPLUG:
		return (const UTF8Char*)"Mobile Plug";
	case HT_AIR_QUALITY:
		return (const UTF8Char*)"Air Quality";
	case HT_WATERLEAKAGE:
		return (const UTF8Char*)"Water Leakage";
	case HT_UNKNOWN:
	default:
		return (const UTF8Char*)"Unknown";
	}
}

const UTF8Char *IO::SNBDongle::GetReadingName(ReadingType readingType)
{
	switch (readingType)
	{
	case IO::SNBDongle::RT_TEMPERATURE:
		return (const UTF8Char*)"Temperature";
	case IO::SNBDongle::RT_AIR_HUMIDITY:
		return (const UTF8Char*)"Humidity";
	case IO::SNBDongle::RT_AIR_CO:
		return (const UTF8Char*)"CO Level";
	case IO::SNBDongle::RT_AIR_PM25:
		return (const UTF8Char*)"PM2.5";
	case IO::SNBDongle::RT_AIR_PM10:
		return (const UTF8Char*)"PM10";
	case IO::SNBDongle::RT_AIR_HCHO:
		return (const UTF8Char*)"HCHO";
	case IO::SNBDongle::RT_AIR_VOC:
		return (const UTF8Char*)"VOC";
	case IO::SNBDongle::RT_AIR_CO2:
		return (const UTF8Char*)"CO2 Level";
	case IO::SNBDongle::RT_VOLTAGE:
		return (const UTF8Char*)"Voltage";
	case IO::SNBDongle::RT_DOOR_CONTACT:
		return (const UTF8Char*)"Door";
	case IO::SNBDongle::RT_MOVING_SENSOR:
		return (const UTF8Char*)"Motion";
	case IO::SNBDongle::RT_ONOFF:
		return (const UTF8Char*)"OnOff";
	case IO::SNBDongle::RT_POWER:
		return (const UTF8Char*)"Power";
	case IO::SNBDongle::RT_PARTICLE_0_3UM:
		return (const UTF8Char*)"Particle 0.3um";
	case IO::SNBDongle::RT_PARTICLE_0_5UM:
		return (const UTF8Char*)"Particle 0.5um";
	case IO::SNBDongle::RT_PARTICLE_1_0UM:
		return (const UTF8Char*)"Particle 1.0um";
	case IO::SNBDongle::RT_PARTICLE_2_5UM:
		return (const UTF8Char*)"Particle 2.5um";
	case IO::SNBDongle::RT_PARTICLE_5_0UM:
		return (const UTF8Char*)"Particle 5.0um";
	case IO::SNBDongle::RT_PARTICLE_10UM:
		return (const UTF8Char*)"Particle 10um";
	case IO::SNBDongle::RT_ALERT:
		return (const UTF8Char*)"Alert";
	case IO::SNBDongle::RT_GAS_CH4:
		return (const UTF8Char*)"Gas CH4 Level";
	case IO::SNBDongle::RT_UNKNOWN:
	default:
		return (const UTF8Char*)"Value";
	}
}

const UTF8Char *IO::SNBDongle::SensorTypeGetName(SensorType sensorType)
{
	switch (sensorType)
	{
	case IO::SNBDongle::ST_DOOR_CONTACT:
		return (const UTF8Char*)"Door Contact";
	case IO::SNBDongle::ST_MOTION:
		return (const UTF8Char*)"Motion";
	case IO::SNBDongle::ST_SENSOR:
		return (const UTF8Char*)"Mixed";
	case IO::SNBDongle::ST_TEMPERATURE:
		return (const UTF8Char*)"Temperature";
	case IO::SNBDongle::ST_RETURN_NORMAL:
		return (const UTF8Char*)"Return normal";
	default:
		return (const UTF8Char*)"Unknown";
	}

}
