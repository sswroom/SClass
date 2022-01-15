#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/Math.h"
#include "IO/BitReaderMSB.h"
#include "Net/TETRALIP.h"

Bool Net::TETRALIP::ParseProtocol(UInt8 *buff, OSInt buffSize, Data::DateTime *recvTime, Map::GPSTrack::GPSRecord *record, Int32 *reason)
{
	if ((buff[0] & 0xc0) == 0)
	{
		Int64 ts = recvTime->ToUnixTimestamp();
		Int32 ispd;
		Int32 ilon = ((ReadMInt32(buff)) >> 3) & 0x1ffffff;
		Int32 ilat = ((ReadMInt32(&buff[3])) >> 3) & 0xffffff;
		if (ilon & 0x1000000)
		{
			ilon = ilon | 0xff000000;
		}
		if (ilat & 0x800000)
		{
			ilat = ilat | 0xff000000;
		}
		record->lat = ilat * 0.0000107288360595703125;
		record->lon = ilon * 0.0000107288360595703125;
		record->altitude = 0;
		record->utcTimeTS = (Int32)ts;
		record->valid = (record->lat != 0 || record->lon != 0); //true
		if ((buff[6] & 7) == 7)
		{
			record->valid = false;
		}
		ispd = buff[7] >> 1;
		if (ispd <= 28)
		{
			record->speed = ispd / 1.852;
		}
		else if (ispd == 127)
		{
			record->speed = 0;
		}
		else
		{
			record->speed = 16 * Math_Pow(1.038, ispd - 13) / 1.852;
		}
		record->heading = (((buff[7] << 3) | (buff[8] >> 5)) & 0xf) * 22.5;
		record->nSateUsed = -1;
		record->nSateView = -1;
		if ((buff[8] & 0x10) == 0)
		{
			*reason = ((buff[8] << 4) | (buff[9] >> 4)) & 0xff;
		}
		else
		{
			*reason = 0;
		}
		return true;
	}
	else if ((buff[0] & 0xc0) == 0x40)
	{
		if ((buff[0] & 0x3C) == 0xC)
		{
			Int32 v;
			IO::BitReaderMSB reader(buff, buffSize);
			reader.ReadBits(&v, 2);
			reader.ReadBits(&v, 4);
			reader.ReadBits(&v, 2);
			switch (v)
			{
			case 0:
				break;
			case 1:
				reader.ReadBits(&v, 2);
				break;
			case 2:
				reader.ReadBits(&v, 22);
				break;
			default:
				return false;
			}

			Int32 ilon;
			Int32 ilat;
			Int64 ts = recvTime->ToUnixTimestamp();
			Int32 ispd;
			Int32 idir;
			Int32 altitude;

			record->valid = true;
			ilat = 0;
			ilon = 0;
			altitude = 0;

			reader.ReadBits(&v, 4);

			switch (v)
			{
			case 0: //No shape
				record->valid = false;
				break;
			case 1: //Location point
				reader.ReadBits(&ilon, 25);
				reader.ReadBits(&ilat, 24);
				break;
			case 2: //Location circle
				reader.ReadBits(&ilon, 25);
				reader.ReadBits(&ilat, 24);
				reader.ReadBits(&v, 6);
				break;
			case 3: //Location ellipse
				reader.ReadBits(&ilon, 25);
				reader.ReadBits(&ilat, 24);
				reader.ReadBits(&v, 6);
				reader.ReadBits(&v, 6);
				reader.ReadBits(&v, 8);
				reader.ReadBits(&v, 3);
				break;
			case 4: //Location point with altitude
				reader.ReadBits(&ilon, 25);
				reader.ReadBits(&ilat, 24);
				reader.ReadBits(&altitude, 12);
				break;
			case 5: //Location circle with altitude
				reader.ReadBits(&ilon, 25);
				reader.ReadBits(&ilat, 24);
				reader.ReadBits(&v, 6);
				reader.ReadBits(&altitude, 12);
				break;
			case 6: //Location ellipse with altitude
				reader.ReadBits(&ilon, 25);
				reader.ReadBits(&ilat, 24);
				reader.ReadBits(&v, 6);
				reader.ReadBits(&v, 6);
				reader.ReadBits(&v, 8);
				reader.ReadBits(&altitude, 12);
				reader.ReadBits(&v, 3);
				break;
			case 7: //Location circle with altitude and altitude uncertainty
				reader.ReadBits(&ilon, 25);
				reader.ReadBits(&ilat, 24);
				reader.ReadBits(&v, 6);
				reader.ReadBits(&altitude, 12);
				reader.ReadBits(&v, 3);
				break;
			case 8: //Location ellipse with altitude and altitude uncertainty
				reader.ReadBits(&ilon, 25);
				reader.ReadBits(&ilat, 24);
				reader.ReadBits(&v, 6);
				reader.ReadBits(&v, 6);
				reader.ReadBits(&v, 8);
				reader.ReadBits(&altitude, 12);
				reader.ReadBits(&v, 3);
				reader.ReadBits(&v, 3);
				break;
			case 9: //Location arc
				reader.ReadBits(&ilon, 25);
				reader.ReadBits(&ilat, 24);
				reader.ReadBits(&v, 16);
				reader.ReadBits(&v, 16);
				reader.ReadBits(&v, 8);
				reader.ReadBits(&v, 8);
				reader.ReadBits(&v, 3);
				break;
			case 10: //Location point and position error
				reader.ReadBits(&ilon, 25);
				reader.ReadBits(&ilat, 24);
				reader.ReadBits(&v, 3);
				if (v == 7)
				{
					record->valid = false;
				}
				break;
			default:
				return false;
			}
			if (ilon & 0x1000000)
			{
				ilon = ilon | 0xff000000;
			}
			if (ilat & 0x800000)
			{
				ilat = ilat | 0xff000000;
			}
			record->lat = ilat * 0.0000107288360595703125;
			record->lon = ilon * 0.0000107288360595703125;
			altitude = altitude & 2047;
			if (altitude <= 0)
				record->altitude = 0;
			else if (altitude <= 1200)
				record->altitude = altitude - 201;
			else if (altitude <= 1926)
				record->altitude = altitude * 2 - 1402;
			else
				record->altitude = altitude * 75 - 142000;
			record->utcTimeTS = (Int32)ts;

			reader.ReadBits(&v, 3);

			ispd = 0;
			idir = 0;
			switch (v)
			{
			case 0: //No velocity information
				break;
			case 1: //Horizontal velocity
				reader.ReadBits(&ispd, 7);
				break;
			case 2: //Horizontal velocity with uncertainty
				reader.ReadBits(&ispd, 7);
				reader.ReadBits(&v, 3);
				break;
			case 3: //Horizontal velocity and vertical velocity
				reader.ReadBits(&ispd, 7);
				reader.ReadBits(&v, 8);
				break;
			case 4: //Horizontal velocity and vertical velocity with uncertainty
				reader.ReadBits(&ispd, 7);
				reader.ReadBits(&v, 3);
				reader.ReadBits(&v, 8);
				reader.ReadBits(&v, 3);
				break;
			case 5: //Horizontal velocity with direction of travel extended
				reader.ReadBits(&ispd, 7);
				reader.ReadBits(&idir, 8);
				break;
			case 6: //Horizontal velocity with direction of travel extended and uncertainty
				reader.ReadBits(&ispd, 7);
				reader.ReadBits(&v, 3);
				reader.ReadBits(&idir, 8);
				reader.ReadBits(&v, 3);
				break;
			case 7: //Horizontal velocity and vertical velocity with direction of travel extended and uncertainty
				reader.ReadBits(&ispd, 7);
				reader.ReadBits(&v, 3);
				reader.ReadBits(&v, 8);
				reader.ReadBits(&v, 3);
				reader.ReadBits(&idir, 8);
				reader.ReadBits(&v, 3);
				break;
			}
			ispd = buff[7] >> 1;
			if (ispd <= 28)
			{
				record->speed = ispd / 1.852;
			}
			else if (ispd == 127)
			{
				record->speed = 0;
			}
			else
			{
				record->speed = 16 * Math_Pow(1.038, ispd - 13) / 1.852;
			}
			record->heading = idir * 360.0 / 256.0;
			record->nSateUsed = -1;
			record->nSateView = -1;
			reader.ReadBits(&v, 1);
			reader.ReadBits(&v, 1);
			if (v == 0)
			{
				v = reader.ReadBits(&v, 8);
				*reason = v;
			}
			else
			{
				*reason = 0;
			}
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

OSInt Net::TETRALIP::GenLocReq(UInt8 *buff)
{
	/*
	0	PDU type							1
	2	PDU type extension					1
	6	Request/response					0
	7	Report type							3 (Short Loc)
	9	Location information destination	0
	13	Terminal or location identification	0
	17	Direction of travel and direction of travel accuracy	0
	20	Horizontal position and horizontal position accuracy	0
	23	Horizontal velocity and horizontal velocity accuracy	0
	26	Location altitude and location altitude accuracy		0
	30	Maximum information age				30s
	37	Maximum response time				now
	44	Vertical velocity and vertical velocity accuracy		0
	*/

	buff[0] = 0x45;
	buff[1] = 0x80;
	buff[2] = 0x00;
	buff[3] = 0x00;
	buff[4] = 0xe8;
	buff[5] = 0x00;
	return 6;
	///////////////////////////
}
