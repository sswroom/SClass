#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "Math/Math_C.h"
#include "Net/TETRALRRP.h"

Bool Net::TETRALRRP::ParseProtocol(UnsafeArray<UInt8> buff, UIntOS buffSize, NN<Data::DateTime> recvTime, NN<Map::GPSTrack::GPSRecord3> record, OutParam<Int32> requestId, OutParam<Int32> resultCode)
{
	if (buff[0] != 0x80)
	{
		return false;
	}

	Bool constTab = false;
	Bool isRequest = false;
	switch (buff[1])
	{
	case 3:
	case 5:
	case 9:
	case 0xf:
	case 0x14:
	case 0x17:
	case 0x1b:
	case 0x21:
	case 0x26:
		constTab = false;
		isRequest = true;
		break;
	case 7:
	case 0xb:
	case 0xd:
	case 0x11:
	case 0x13:
	case 0x15:
	case 0x19:
	case 0x1d:
	case 0x1f:
	case 0x23:
	case 0x25:
	case 0x27:
		constTab = false;
		isRequest = false;
		break;
	case 2:
	case 4:
	case 8:
	case 0xe:
	case 0x16:
	case 0x1a:
	case 0x20:
		constTab = true;
		isRequest = true;
		break;
	case 6:
	case 0xa:
	case 0xc:
	case 0x10:
	case 0x12:
	case 0x18:
	case 0x1c:
	case 0x1e:
	case 0x22:
	case 0x24:
		constTab = true;
		isRequest = false;
		break;
	default:
		return false;
	}
	if (buff[2] > buffSize - 3)
		return false;
	UInt8 *endPtr = &buff[3] + buff[2];
	UnsafeArray<UInt8> ptr = &buff[3];
	Data::DateTime dt;
	Int32 reqId = -1;
	Int32 res = -1;

	record->pos = Math::Coord2DDbl(0, 0);
	record->altitude = 0;
	record->recTime = recvTime->ToInstant();
	record->valid = false;
	record->speed = 0;
	record->heading = 0;
	record->nSateUsed = -1;
	record->nSateUsedGPS = -1;
	record->nSateUsedSBAS = -1;
	record->nSateUsedGLO = -1; //GLONASS
	record->nSateViewGPS = -1;
	record->nSateViewGLO = -1;
	record->nSateViewGA = -1;
	record->nSateViewQZSS = -1;
	record->nSateViewBD = -1;

	if (isRequest)
	{
		return false;
	}
	else
	{
		while (ptr < endPtr)
		{
			switch (*ptr)
			{
			case 0x22: //request-id (server request)
				ptr += ptr[1] + 2;
				reqId = 32;
				break;
			case 0x23: //request-id
				if (endPtr - ptr >= 2)
				{
					reqId = ptr[1];
				}
				ptr += 2;
				break;
			case 0x2A:
				ptr += 8;
				break;
			case 0x30: //current-time
			case 0x34: //info-time
				if (endPtr - ptr >= 6)
				{
					dt.ToUTCTime();
					dt.SetValue((ptr[1] << 6) | (ptr[2] >> 2), ((ptr[2] & 3) << 2) | (ptr[3] >> 6), (ptr[3] >> 1) & 0x1f, ((ptr[3] & 1) << 4) | (ptr[4] >> 4), ((ptr[4] & 0xf) << 2) | (ptr[5] >> 6), ptr[5] & 0x3f, 0);
					record->recTime = dt.ToInstant();
					ptr += 6;
				}
				break;
			//case 0x31: //current-time
			case 0x56: //direction-hor
			case 0x60: //heading-hor
				if (endPtr - ptr >= 2)
				{
					record->heading = ptr[1] * 2;
				}
				ptr += 2;
				break;
			case 0x90: //direction-hor
			case 0x94: //heading-hor
				ptr++;
				record->heading = ReadUFloatVar(ptr) * 2.0;
				break;
			case 0x66: //point-2d (lat + long)
				if (endPtr - ptr >= 9)
				{
					ptr++;
					record->pos.y = ReadLat(ptr);
					record->pos.x = ReadLong(ptr);
					record->valid = true;
				}
				else
				{
					ptr += 9;
				}
				break;
			case 0x67: //point-3d (lat + long + altitude/sintvar)
				ptr++;
				record->pos.y = ReadLat(ptr);
				record->pos.x = ReadLong(ptr);
				record->valid = true;
				record->altitude = ReadSIntVar(ptr);
				break;
			case 0x68: //point-3d (lat + long + altitude/sintvar + altitude-acc/uintvar)
				ptr++;
				record->pos.y = ReadLat(ptr);
				record->pos.x = ReadLong(ptr);
				record->valid = true;
				record->altitude = ReadSIntVar(ptr);
				ReadUIntVar(ptr);
				break;
			case 0x69: //point-3d (lat + long + altitude/sfloatvar)
				ptr++;
				record->pos.y = ReadLat(ptr);
				record->pos.x = ReadLong(ptr);
				record->valid = true;
				record->altitude = ReadSFloatVar(ptr);
				break;
			case 0x6A: //point-3d (lat + long + altitude/sfloatvar + altitude-acc/ufloatvar)
				ptr++;
				record->pos.y = ReadLat(ptr);
				record->pos.x = ReadLong(ptr);
				record->valid = true;
				record->altitude = ReadSFloatVar(ptr);
				ReadUFloatVar(ptr);
				break;

			case 0x5A: //ellipse-2d (lat + long + angle + semi-major + semi-minor /UIntVar)
				record->pos.y = ReadLat(ptr);
				record->pos.x = ReadLong(ptr);
				record->valid = true;
				ReadUIntVar(ptr);
				ReadUIntVar(ptr);
				ReadUIntVar(ptr);
				break;
			case 0x5B: //ellipse-2d (lat + long + angle + semi-major + semi-minor /UFloatVar)
				record->pos.y = ReadLat(ptr);
				record->pos.x = ReadLong(ptr);
				record->valid = true;
				ReadUFloatVar(ptr);
				ReadUFloatVar(ptr);
				ReadUFloatVar(ptr);
				break;
			case 0x5C: //ellipse-3d (lat + long + angle + semi-major + semi-minor + altitude /UIntVar)
				record->pos.y = ReadLat(ptr);
				record->pos.x = ReadLong(ptr);
				record->valid = true;
				ReadUIntVar(ptr);
				ReadUIntVar(ptr);
				ReadUIntVar(ptr);
				record->altitude = ReadSIntVar(ptr);
				break;
			case 0x5D: //ellipse-3d (lat + long + angle + semi-major + semi-minor + altitude + altitude-acc /UIntVar)
				record->pos.y = ReadLat(ptr);
				record->pos.x = ReadLong(ptr);
				record->valid = true;
				ReadUIntVar(ptr);
				ReadUIntVar(ptr);
				ReadUIntVar(ptr);
				record->altitude = ReadSIntVar(ptr);
				ReadUIntVar(ptr);
				break;
			case 0x5E: //ellipse-3d (lat + long + angle + semi-major + semi-minor + altitude /UFloatVar)
				record->pos.y = ReadLat(ptr);
				record->pos.x = ReadLong(ptr);
				record->valid = true;
				ReadUFloatVar(ptr);
				ReadUFloatVar(ptr);
				ReadUFloatVar(ptr);
				record->altitude = ReadSFloatVar(ptr);
				break;
			case 0x5F: //ellipse-3d (lat + long + angle + semi-major + semi-minor + altitude + altitude-acc /UFloatVar)
				record->pos.y = ReadLat(ptr);
				record->pos.x = ReadLong(ptr);
				record->valid = true;
				ReadUFloatVar(ptr);
				ReadUFloatVar(ptr);
				ReadUFloatVar(ptr);
				record->altitude = ReadSFloatVar(ptr);
				ReadUFloatVar(ptr);
				break;
			case 0x37: //result
				ptr++;
				res = ReadUIntVar(ptr);
				break;
			//case 0x38: //result
			case 0x6B: //speed-hor
				ptr += 1;
				record->speed = ReadUIntVar(ptr) * 3.6 / 1.852;
				break;
			case 0x6C: //speed-hor
				ptr += 1;
				record->speed = ReadUFloatVar(ptr) * 3.6 / 1.852;
				break;
			default:
				return false;
			}
		}
	}
	
	requestId.Set(reqId);
	resultCode.Set(res);
	return true;
}

Double Net::TETRALRRP::ReadLat(InOutParam<UnsafeArray<UInt8>> buff)
{
	Int32 i;
	UnsafeArray<UInt8> buffPtr = buff.Get();
	i = ReadMInt32(&buffPtr[0]);
	buff.Set(buffPtr + 4);
	if (i & 0x80000000)
	{
		return (i & 0x7fffffff) * -0.000000041909515857696533203125;
	}
	else
	{
		return (i & 0x7fffffff) * 0.000000041909515857696533203125;
	}
}

Double Net::TETRALRRP::ReadLong(InOutParam<UnsafeArray<UInt8>> buff)
{
	Int32 i;
	UnsafeArray<UInt8> buffPtr = buff.Get();
	i = ReadMInt32(&buffPtr[0]);
	buff.Set(buffPtr + 4);
	Double v = i * 0.00000008381903171539306640625;
	if (v < 0)
	{
		v += 360;
	}
	return v;
}

Int32 Net::TETRALRRP::ReadSIntVar(InOutParam<UnsafeArray<UInt8>> buff)
{
	UInt32 v = 0;
	UInt8 b;
	Bool sign;
	UnsafeArray<UInt8> buffPtr = buff.Get();
	b = buffPtr[0];
	buffPtr++;
	sign = (b & 0x40) != 0;
	b = b & ~0x40;
	while (true)
	{
		v = (v << 7) | (b & 0x7f);
		if ((b & 0x80) == 0)
			break;
		b = buffPtr[0];
		buffPtr++;
	}
	buff.Set(buffPtr);
	return v;
}

UInt32 Net::TETRALRRP::ReadUIntVar(InOutParam<UnsafeArray<UInt8>> buff)
{
	UInt32 v = 0;
	UInt8 b;
	UnsafeArray<UInt8> buffPtr = buff.Get();
	while (true)
	{
		b = buffPtr[0];
		buffPtr++;
		v = (v << 7) | (b & 0x7f);
		if ((b & 0x80) == 0)
			break;
	}
	buff.Set(buffPtr);
	return v;
}

Double Net::TETRALRRP::ReadSFloatVar(InOutParam<UnsafeArray<UInt8>> buff)
{
	Int32 v1;
	UnsafeArray<UInt8> ptr;
	UInt32 v2;
	v1 = ReadSIntVar(buff);
	ptr = buff.Get();
	v2 = ReadUIntVar(buff);
	return v1 + v2 / (1 << (7 * (buff.Get() - ptr)));
}

Double Net::TETRALRRP::ReadUFloatVar(InOutParam<UnsafeArray<UInt8>>buff)
{
	UInt32 v1;
	UnsafeArray<UInt8> ptr;
	UInt32 v2;
	v1 = ReadUIntVar(buff);
	ptr = buff.Get();
	v2 = ReadUIntVar(buff);
	return v1 + v2 / (1 << (7 * (buff.Get() - ptr)));
}

UnsafeArray<UInt8> Net::TETRALRRP::WriteUIntVar(UnsafeArray<UInt8> buff, UInt32 val)
{
	UInt8 b;
	while (true)
	{
		b = val & 0x7f;
		val = val >> 7;
		if (val)
		{
			b = b | 0x80;
			*buff++ = b;
		}
		else
		{
			*buff++ = b;
			break;
		}
	}
	return buff;
}

UnsafeArray<UInt8> Net::TETRALRRP::WriteLat(UnsafeArray<UInt8> buff, Double lat)
{
	Int32 i = Double2Int32(lat * 23860929.422222222222222222222222);
	if (i < 0)
	{
		i = (-i) | 0x80000000;
	}
	WriteMInt32(&buff[0], i);
	return buff + 4;
}

UnsafeArray<UInt8> Net::TETRALRRP::WriteLong(UnsafeArray<UInt8> buff, Double lon)
{
	WriteMInt32(&buff[0], Double2Int32(lon * 11930464.711111111111111111111111));
	return buff + 4;
}

UnsafeArray<UInt8> Net::TETRALRRP::WriteDateTime(UnsafeArray<UInt8> buff, NN<Data::DateTime> dt)
{
	Int32 v = dt->GetMinute();
	Int32 v2 = dt->GetHour();
	Int32 v3;
	buff[4] = dt->GetSecond() + (UInt8)(v << 6);
	buff[3] = (UInt8)((v >> 2) | (v2 << 4));
	v = dt->GetDay();
	v3 = dt->GetMonth();
	buff[2] = (UInt8)((v2 >> 4) | (v << 1) | (v3 << 6));
	v = dt->GetYear();
	buff[1] = (UInt8)((v3 >> 2) | (v << 2));
	buff[0] = (UInt8)(v >> 6);
	
	return buff + 5;
}

UIntOS Net::TETRALRRP::GenLocReq(UnsafeArray<UInt8> buff)
{
	///////////////////////////
	buff[0] = 0x05;
	buff[1] = 0x0F;
	buff[2] = 0x22;
	buff[3] = 0x04;
	buff[4] = 0x24;
	buff[5] = 0x68;
	buff[6] = 0xAC;
	buff[7] = 0xE0;
	buff[8] = 0x51;
	buff[9] = 0x63;
	buff[10] = 0x32;
	buff[11] = 0x38;
	buff[12] = 0x04;
	buff[13] = 0x08;
	buff[14] = 0x22;
	buff[15] = 0x10;
	buff[16] = 0x12;
	return 17;
}

UIntOS Net::TETRALRRP::BuildPacket(UnsafeArray<UInt8> buff, NN<Map::GPSTrack::GPSRecord3> record, Int32 requestId, Int32 resultCode)
{
	UIntOS retSize;
	UnsafeArray<UInt8> ptr;
	Data::DateTime dt;
	ptr = &buff[3];
	retSize = 0;
	buff[0] = 0x80;
	buff[1] = 0x13;
	
	if (requestId < 0)
	{
	}
	else
	{
		ptr[0] = 0x23;
		ptr[1] = (UInt8)requestId;
		ptr += 2;
	}

	if (resultCode >= 0)
	{
		ptr[0] = 0x37;
		ptr = WriteUIntVar(ptr + 1, resultCode);
	}

	ptr[0] = 0x34;
	dt.ToUTCTime();
	dt.SetInstant(record->recTime);
	ptr = WriteDateTime(ptr + 1, dt);

	if (record->valid)
	{
		ptr[0] = 0x66;
		ptr = WriteLat(ptr + 1, record->pos.GetLat());
		ptr = WriteLong(ptr, record->pos.GetLon());

		ptr[0] = 0x56;
		ptr[1] = (UInt8)(record->heading * 0.5);
		ptr += 2;
	}

	retSize = ptr - buff;
	buff[2] = (UInt8)(retSize - 3);
	return retSize;
}
