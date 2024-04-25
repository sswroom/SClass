#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/I2CChannelOS.h"
#include "IO/Device/BME280.h"
#include "Sync/SimpleThread.h"

Double IO::Device::BME280::CalcTempRAW(Int32 tRAW)
{
	Double rslt = 0;
	Int32 v1;
	Int32 v2;
    v1 = ((((tRAW >> 3) - ((Int32) this->t1 << 1))) * ((Int32) this->t2)) >> 11;
    v2 = (((((tRAW >> 4) - ((Int32) this->t1)) * ((tRAW >> 4) - ((Int32) this->t1))) >> 12) * ((Int32) this->t3)) >> 14;
    rslt = (v1 + v2) * 5 / 25600.0;
    return rslt;
}

Double IO::Device::BME280::CalcRHRAW(Int32 tRAW, Int32 rhRAW)
{
	Int32 tv1;
	Int32 tv2;
    tv1 = ((((tRAW >> 3) - ((Int32) this->t1 << 1))) * ((Int32) this->t2)) >> 11;
    tv2 = (((((tRAW >> 4) - ((Int32) this->t1)) * ((tRAW >> 4) - ((Int32) this->t1))) >> 12) * ((Int32) this->t3)) >> 14;
	Int32 t_fine = tv1 + tv2;
	Int32 v1;
	v1 = (t_fine - ((Int32) 76800));
	v1 = (((((rhRAW <<14) - (((Int32) this->h4) << 20) - (((Int32) this->h5) * v1)) +
		((Int32) 16384)) >> 15) * (((((((v1 * ((Int32) this->h6)) >> 10) * (((v1 *
		((Int32) this->h3)) >> 11) + ((Int32) 32768))) >> 10) + ((Int32) 2097152)) *
		((Int32) this->h2) + 8192) >> 14));
	v1 = (v1 - (((((v1 >> 15) * (v1 >> 15)) >> 7) * ((Int32) this->h1)) >> 4));
	v1 = (v1 < 0 ? 0 : v1);
	v1 = (v1 > 419430400 ? 419430400 : v1);
	return (v1 >> 12) / 1024.0;
}

Double IO::Device::BME280::CalcPressureRAW(Int32 tRAW, Int32 pressureRAW)
{
	Int32 tv1;
	Int32 tv2;
    tv1 = ((((tRAW >> 3) - ((Int32) this->t1 << 1))) * ((Int32) this->t2)) >> 11;
    tv2 = (((((tRAW >> 4) - ((Int32) this->t1)) * ((tRAW >> 4) - ((Int32) this->t1))) >> 12) * ((Int32) this->t3)) >> 14;
	Int32 t_fine = tv1 + tv2;
	Int64 rslt = 0;
	Int64 v1;
	Int64 v2;
	v1 = ((Int64) t_fine) - 128000;
	v2 = v1 * v1 * (Int64) this->p6;
	v2 = v2 + ((v1 * (Int64) this->p5) << 17);
	v2 = v2 + (((Int64) this->p4) << 35);
	v1 = ((v1 * v1 * (Int64) this->p3) >> 8) + ((v1 * (Int64) this->p2) << 12);
	v1 = (((((Int64) 1) << 47) + v1)) * ((Int64) this->p1) >> 33;
	if (v1 == 0)
		return 0;
	rslt = 1048576 - pressureRAW;
	rslt = (((rslt << 31) - v2) * 3125) / v1;
	v1 = (((Int64) this->p9) * (rslt >> 13) * (rslt >> 13)) >> 25;
	v2 = (((Int64) this->p8) * rslt) >> 19;
	rslt = ((rslt + v1 + v2) >> 8) + (((Int64) this->p7) << 4);
	return (Double)rslt / 256.0;
}

IO::Device::BME280::BME280(NN<IO::I2CChannel> channel, Bool toRelease) : i2c(channel, 0)
{
	this->channel = channel;
	this->toRelease = toRelease;
	this->valid = false;
	UInt8 id;
	if (this->i2c.ReadBuff(0xd0, 1, &id))
	{
		this->valid = (id == 0x60); //BME280
	}
	if (this->valid)
	{
		this->valid = this->Reset();
	}
}

IO::Device::BME280::~BME280()
{
	if (this->toRelease)
	{
		this->channel.Delete();
	}
}

Bool IO::Device::BME280::IsError()
{
	return !this->valid;
}

Bool IO::Device::BME280::Reset()
{
	UInt8 buff[26];
	buff[0] = 0xB6;
	if (!this->i2c.WriteBuff(0xE0, 1, buff)) //Reset
	{
		return false;
	}
	Sync::SimpleThread::Sleep(300);
	if (!this->i2c.ReadBuff(0x88, 0xa2 - 0x88, buff))
	{
		return false;
	}
	else
	{
		this->t1 = ReadUInt16(&buff[0]);
		this->t2 = ReadInt16(&buff[2]);
		this->t3 = ReadInt16(&buff[4]);
		this->p1 = ReadUInt16(&buff[6]);
		this->p2 = ReadInt16(&buff[8]);
		this->p3 = ReadInt16(&buff[10]);
		this->p4 = ReadInt16(&buff[12]);
		this->p5 = ReadInt16(&buff[14]);
		this->p6 = ReadInt16(&buff[16]);
		this->p7 = ReadInt16(&buff[18]);
		this->p8 = ReadInt16(&buff[20]);
		this->p9 = ReadInt16(&buff[22]);
		this->h1 = buff[25];
	}

	if (!this->i2c.ReadBuff(0xE1, 7, buff))
	{
		return false;
	}
	else
	{
		this->h2 = ReadUInt16(&buff[0]);
		this->h3 = buff[2];
		this->h4 = (UInt16)((buff[3] << 4) | (buff[4] & 0xf));
		this->h5 = (UInt16)((buff[4] >> 4) | (buff[5] << 4));
		this->h6 = buff[6];
	}
	
	buff[0] = 0x40;
	this->i2c.WriteBuff(0xF5, 1, buff); //config: t_sb = 125ms, filter = off
	buff[0] = 0x93;
	this->i2c.WriteBuff(0xF4, 1, buff); //ctrl_meas = tx8, px8, normal mode
	buff[0] = 0x4;
	this->i2c.WriteBuff(0xF2, 1, buff); //ctrl_humi = hx8
	return true;
}

Bool IO::Device::BME280::ReadTemperature(OutParam<Double> temp)
{
	UInt8 buff[3];
	if (!this->valid)
		return false;
	if (this->i2c.ReadBuff(0xFA, 3, buff))
	{
		temp.Set(this->CalcTempRAW((buff[0] << 12) | (buff[1] << 4) | (buff[2] >> 4)));
		return true;
	}
	else
	{
		return false;
	}
}

Bool IO::Device::BME280::ReadRH(OutParam<Double> rh)
{
	UInt8 buff[5];
	if (!this->valid)
		return false;
	if (this->i2c.ReadBuff(0xFA, 5, buff))
	{
		rh.Set(this->CalcRHRAW((buff[0] << 12) | (buff[1] << 4) | (buff[2] >> 4), ReadMUInt16(&buff[3])));
		return true;
	}
	else
	{
		return false;
	}
}

Bool IO::Device::BME280::ReadPressure(OutParam<Double> pressure)
{
	UInt8 buff[6];
	if (!this->valid)
		return false;
	if (this->i2c.ReadBuff(0xF7, 6, buff))
	{
		pressure.Set(this->CalcPressureRAW((buff[3] << 12) | (buff[4] << 4) | (buff[5] >> 4), (buff[0] << 12) | (buff[1] << 4) | (buff[2] >> 4)));
		return true;
	}
	else
	{
		return false;
	}
}

Bool IO::Device::BME280::ReadAll(OutParam<Double> temp, OutParam<Double> rh, OutParam<Double> pressure)
{
	UInt8 buff[8];
	if (!this->valid)
		return false;
	if (this->i2c.ReadBuff(0xF7, 8, buff))
	{
		Int32 tVal = (buff[3] << 12) | (buff[4] << 4) | (buff[5] >> 4);
		rh.Set(this->CalcRHRAW(tVal, ReadMUInt16(&buff[6])));
		temp.Set(this->CalcTempRAW(tVal));
		pressure.Set(this->CalcPressureRAW(tVal, (buff[0] << 12) | (buff[1] << 4) | (buff[2] >> 4)));
		return true;
	}
	else
	{
		return false;
	}

}

IO::I2CChannel *IO::Device::BME280::CreateDefChannel(Int32 i2cBusNo)
{
	IO::I2CChannel *channel;
	NEW_CLASS(channel, IO::I2CChannelOS(i2cBusNo, 0x76));
	if (channel->IsError())
	{
		DEL_CLASS(channel);
		return 0;
	}
	return channel;
}
