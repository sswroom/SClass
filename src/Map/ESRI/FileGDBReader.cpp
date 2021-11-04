#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Data/UUID.h"
#include "DB/ColDef.h"
#include "Map/ESRI/FileGDBReader.h"
#include "Math/Math.h"
#include "Math/Point3D.h"
#include "Math/WKTWriter.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"

Map::ESRI::FileGDBReader::FileGDBReader(IO::IStreamData *fd, UInt64 ofst, FileGDBTableInfo *tableInfo)
{
	this->fd = fd->GetPartialData(ofst, fd->GetDataSize() - ofst);
	this->currOfst = 0;
	this->tableInfo = Map::ESRI::FileGDBUtil::TableInfoClone(tableInfo);
	this->rowSize = 0;
	this->rowData = 0;
	this->objectId = 0;
	UOSInt fieldCnt = tableInfo->fields->GetCount();
	this->fieldNull = MemAlloc(UInt8, fieldCnt);
	this->fieldOfst = MemAlloc(UInt32, fieldCnt);
	MemClear(this->fieldNull, fieldCnt);
	MemClear(this->fieldOfst, fieldCnt * 4);
}

Map::ESRI::FileGDBReader::~FileGDBReader()
{
	DEL_CLASS(this->fd);
	Map::ESRI::FileGDBUtil::FreeTableInfo(this->tableInfo);
	if (this->rowData)
	{
		MemFree(this->rowData);
	}
	MemFree(this->fieldNull);
	MemFree(this->fieldOfst);
}

Bool Map::ESRI::FileGDBReader::ReadNext()
{
	UInt8 sizeBuff[4];
	if (this->rowData)
	{
		MemFree(this->rowData);
		this->rowData = 0;
	}
	while (true)
	{
		if (this->fd->GetRealData(this->currOfst, 4, sizeBuff) != 4)
		{
			return false;
		}
		Int32 size = ReadInt32(sizeBuff);
		if (size < 0)
		{
			this->currOfst += 4 + (UInt32)(-size);
		}
		else
		{
			this->rowSize = (UInt32)size;
			break;
		}
	}

	if (this->currOfst + 4 + this->rowSize > this->fd->GetDataSize())
	{
		return false;
	}
	this->rowData = MemAlloc(UInt8, this->rowSize);
	if (this->fd->GetRealData(this->currOfst + 4, this->rowSize, this->rowData) != this->rowSize)
	{
		MemFree(this->rowData);
		this->rowData = 0;
		return false;
	}
	this->objectId++;
	this->currOfst += 4 + this->rowSize;
	UOSInt rowOfst = (UOSInt)(this->tableInfo->nullableCnt + 7) >> 3;
	UOSInt nullIndex = 0;
	FileGDBFieldInfo *field;
	UInt64 v;
	UOSInt i = 0;
	UOSInt j = this->tableInfo->fields->GetCount();
	while (i < j)
	{
		field = this->tableInfo->fields->GetItem(i);
		this->fieldNull[i] = false;
		if (field->flags & 1)
		{
			this->fieldNull[i] = ((this->rowData[(nullIndex >> 3)] & (1 << (nullIndex & 7))) != 0);
			nullIndex++;
		}
		this->fieldOfst[i] = (UInt32)rowOfst;
		if (!this->fieldNull[i])
		{
			switch (field->fieldType)
			{
			case 0:
				rowOfst += 2;
				break;
			case 1:
				rowOfst += 4;
				break;
			case 2:
				rowOfst += 4;
				break;
			case 3:
				rowOfst += 8;
				break;
			case 4:
				rowOfst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, rowOfst, &v);
				rowOfst += (UOSInt)v;
				break;
			case 5:
				rowOfst += 8;
				break;
			case 6:
				break;
			case 7:
				rowOfst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, rowOfst, &v);
				rowOfst += (UOSInt)v;
				break;
			case 8:
				rowOfst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, rowOfst, &v);
				rowOfst += (UOSInt)v;
				break;
			case 9:
				//////////////////////////////
				break;
			case 10:
			case 11:
				rowOfst += 16;
				break;
			case 12:
				rowOfst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, rowOfst, &v);
				rowOfst += (UOSInt)v;
				break;
			}
		}
		i++;
	}
	return true;
}

UOSInt Map::ESRI::FileGDBReader::ColCount()
{
	return this->tableInfo->fields->GetCount();
}

OSInt Map::ESRI::FileGDBReader::GetRowChanged()
{
	return 0;
}

Int32 Map::ESRI::FileGDBReader::GetInt32(UOSInt colIndex)
{
	if (this->rowData == 0)
	{
		return 0;
	}
	Map::ESRI::FileGDBFieldInfo *field = this->tableInfo->fields->GetItem(colIndex);
	if (field == 0 || this->fieldNull[colIndex])
	{
		return 0;
	}
	switch (field->fieldType)
	{
	case 0:
		return ReadInt16(&this->rowData[this->fieldOfst[colIndex]]);
	case 1:
		return ReadInt32(&this->rowData[this->fieldOfst[colIndex]]);
	case 2:
		return Math::Double2Int32(ReadFloat(&this->rowData[this->fieldOfst[colIndex]]));
	case 3:
		return Math::Double2Int32(ReadDouble(&this->rowData[this->fieldOfst[colIndex]]));
	case 5:
		return (Int32)ReadDouble(&this->rowData[this->fieldOfst[colIndex]]);
	case 6:
		return this->objectId;
	}
	return 0;
}

Int64 Map::ESRI::FileGDBReader::GetInt64(UOSInt colIndex)
{
	if (this->rowData == 0)
	{
		return 0;
	}
	Map::ESRI::FileGDBFieldInfo *field = this->tableInfo->fields->GetItem(colIndex);
	if (field == 0 || this->fieldNull[colIndex])
	{
		return 0;
	}
	switch (field->fieldType)
	{
	case 0:
		return ReadInt16(&this->rowData[this->fieldOfst[colIndex]]);
	case 1:
		return ReadInt32(&this->rowData[this->fieldOfst[colIndex]]);
	case 2:
		return Math::Double2Int64(ReadFloat(&this->rowData[this->fieldOfst[colIndex]]));
	case 3:
		return Math::Double2Int64(ReadDouble(&this->rowData[this->fieldOfst[colIndex]]));
	case 5:
		return (Int64)ReadDouble(&this->rowData[this->fieldOfst[colIndex]]);
	case 6:
		return this->objectId;
	}
	return 0;
}

WChar *Map::ESRI::FileGDBReader::GetStr(UOSInt colIndex, WChar *buff)
{
	Text::StringBuilderUTF8 sb;
	if (this->GetStr(colIndex, &sb))
	{
		return Text::StrUTF8_WChar(buff, sb.ToString(), 0);
	}
	return 0;
}

Bool Map::ESRI::FileGDBReader::GetStr(UOSInt colIndex, Text::StringBuilderUTF *sb)
{
	if (this->rowData == 0)
	{
		return false;
	}
	Map::ESRI::FileGDBFieldInfo *field = this->tableInfo->fields->GetItem(colIndex);
	if (field == 0 || this->fieldNull[colIndex])
	{
		return false;
	}
	UInt64 v;
	UOSInt ofst;
	switch (field->fieldType)
	{
	case 0:
		sb->AppendI16(ReadInt16(&this->rowData[this->fieldOfst[colIndex]]));
		return true;
	case 1:
		sb->AppendI32(ReadInt32(&this->rowData[this->fieldOfst[colIndex]]));
		return true;
	case 2:
		Text::SBAppendF32(sb, ReadFloat(&this->rowData[this->fieldOfst[colIndex]]));
		return true;
	case 3:
		Text::SBAppendF64(sb, ReadDouble(&this->rowData[this->fieldOfst[colIndex]]));
		return true;
	case 12:
	case 4:
		ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, this->fieldOfst[colIndex], &v);
		sb->AppendC(&this->rowData[ofst], (UOSInt)v);
		return true;
	case 5:
		{
			Data::DateTime dt;
			Map::ESRI::FileGDBUtil::ToDateTime(&dt, ReadDouble(&this->rowData[this->fieldOfst[colIndex]]));
			sb->AppendDate(&dt);
		}
		return true;
	case 6:
		sb->AppendI32(this->objectId);
		return true;
	case 7:
		{
			Math::Vector2D *vec = this->GetVector(colIndex);
			if (vec)
			{
				Math::WKTWriter writer;
				Bool succ = writer.GenerateWKT(sb, vec);
				DEL_CLASS(vec);
				return succ;
			}
		}
		return false;
	case 8:
		{
			UOSInt size = this->GetBinarySize(colIndex);
			UInt8 *binBuff = MemAlloc(UInt8, size);
			this->GetBinary(colIndex, binBuff);
			sb->AppendHexBuff(binBuff, size, 0, Text::LineBreakType::None);
			MemFree(binBuff);
			return true;
		}
	case 10:
	case 11:
		{
			Data::UUID uuid(&this->rowData[this->fieldOfst[colIndex]]);
			uuid.ToString(sb);
			return true;
		}
	}
	return false;
}

const UTF8Char *Map::ESRI::FileGDBReader::GetNewStr(UOSInt colIndex)
{
	Text::StringBuilderUTF8 sb;
	if (this->GetStr(colIndex, &sb))
	{
		return Text::StrCopyNew(sb.ToString());
	}
	return 0;
}

UTF8Char *Map::ESRI::FileGDBReader::GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
{
	Text::StringBuilderUTF8 sb;
	if (this->GetStr(colIndex, &sb))
	{
		return Text::StrConcatS(buff, sb.ToString(), buffSize);
	}
	return 0;
}

DB::DBReader::DateErrType Map::ESRI::FileGDBReader::GetDate(UOSInt colIndex, Data::DateTime *outVal)
{
	if (this->rowData == 0)
	{
		return DET_ERROR;
	}
	Map::ESRI::FileGDBFieldInfo *field = this->tableInfo->fields->GetItem(colIndex);
	if (field == 0)
	{
		return DET_ERROR;
	}
	else if (this->fieldNull[colIndex])
	{
		return DET_NULL;
	}
	switch (field->fieldType)
	{
	case 5:
		Map::ESRI::FileGDBUtil::ToDateTime(outVal, ReadDouble(&this->rowData[this->fieldOfst[colIndex]]));
		return DET_OK;
	}
	return DET_ERROR;
}

Double Map::ESRI::FileGDBReader::GetDbl(UOSInt colIndex)
{
	if (this->rowData == 0)
	{
		return 0;
	}
	Map::ESRI::FileGDBFieldInfo *field = this->tableInfo->fields->GetItem(colIndex);
	if (field == 0 || this->fieldNull[colIndex])
	{
		return 0;
	}
	switch (field->fieldType)
	{
	case 0:
		return ReadInt16(&this->rowData[this->fieldOfst[colIndex]]);
	case 1:
		return ReadInt32(&this->rowData[this->fieldOfst[colIndex]]);
	case 2:
		return ReadFloat(&this->rowData[this->fieldOfst[colIndex]]);
	case 3:
		return ReadDouble(&this->rowData[this->fieldOfst[colIndex]]);
	case 5:
		return ReadDouble(&this->rowData[this->fieldOfst[colIndex]]);
	case 6:
		return this->objectId;
	}
	return 0;
}

Bool Map::ESRI::FileGDBReader::GetBool(UOSInt colIndex)
{
	return this->GetInt32(colIndex) != 0;
}

UOSInt Map::ESRI::FileGDBReader::GetBinarySize(UOSInt colIndex)
{
	if (this->rowData == 0)
	{
		return 0;
	}
	Map::ESRI::FileGDBFieldInfo *field = this->tableInfo->fields->GetItem(colIndex);
	if (field == 0 || this->fieldNull[colIndex])
	{
		return 0;
	}
	UInt64 v;
	switch (field->fieldType)
	{
	case 0:
		return 2;
	case 1:
		return 4;
	case 2:
		return 4;
	case 3:
		return 8;
	case 12:
	case 4:
		Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, this->fieldOfst[colIndex], &v);
		return (UOSInt)v;
	case 5:
		return 8;
	case 6:
		return 4;
	case 7:
		Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, this->fieldOfst[colIndex], &v);
		return (UOSInt)v;
	case 8:
		Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, this->fieldOfst[colIndex], &v);
		return (UOSInt)v;
	case 10:
	case 11:
		return 16;
	}
	return 0;
}

UOSInt Map::ESRI::FileGDBReader::GetBinary(UOSInt colIndex, UInt8 *buff)
{
	if (this->rowData == 0)
	{
		return 0;
	}
	Map::ESRI::FileGDBFieldInfo *field = this->tableInfo->fields->GetItem(colIndex);
	if (field == 0 || this->fieldNull[colIndex])
	{
		return 0;
	}
	UInt64 v;
	UOSInt ofst;
	switch (field->fieldType)
	{
	case 0:
		MemCopyNO(buff, &this->rowData[this->fieldOfst[colIndex]], 2);
		return 2;
	case 1:
		MemCopyNO(buff, &this->rowData[this->fieldOfst[colIndex]], 4);
		return 4;
	case 2:
		MemCopyNO(buff, &this->rowData[this->fieldOfst[colIndex]], 4);
		return 4;
	case 3:
		MemCopyNO(buff, &this->rowData[this->fieldOfst[colIndex]], 8);
		return 8;
	case 12:
	case 4:
		ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, this->fieldOfst[colIndex], &v);
		MemCopyNO(buff, &this->rowData[ofst], (UOSInt)v);
		return (UOSInt)v;
	case 5:
		MemCopyNO(buff, &this->rowData[this->fieldOfst[colIndex]], 8);
		return 8;
	case 6:
		MemCopyNO(buff, &this->objectId, 4);
		return 4;
	case 7:
		ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, this->fieldOfst[colIndex], &v);
		MemCopyNO(buff, &this->rowData[ofst], (UOSInt)v);
		return (UOSInt)v;
	case 8:
		ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, this->fieldOfst[colIndex], &v);
		MemCopyNO(buff, &this->rowData[ofst], (UOSInt)v);
		return (UOSInt)v;
	case 10:
	case 11:
		MemCopyNO(buff, &this->rowData[this->fieldOfst[colIndex]], 16);
		return 16;
	}
	return 0;
}

Math::Vector2D *Map::ESRI::FileGDBReader::GetVector(UOSInt colIndex)
{
	if (this->rowData == 0)
	{
		return 0;
	}
	Map::ESRI::FileGDBFieldInfo *field = this->tableInfo->fields->GetItem(colIndex);
	if (field == 0 || this->fieldNull[colIndex])
	{
		return 0;
	}
	if (field->fieldType != 7)
	{
		return 0;
	}
	UInt64 geometryLen;
	UInt64 geometryType;
	UOSInt ofst;
	ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, this->fieldOfst[colIndex], &geometryLen);
	ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, &geometryType);
	Double x;
	Double y;
	Double z;
	UInt64 v;
	UInt32 srid;
/*
#define SHPT_MULTIPOINT    8
#define SHPT_MULTIPOINTM  28
#define SHPT_MULTIPOINTZM 18
#define SHPT_MULTIPOINTZ  20

#define SHPT_MULTIPATCHM  31
#define SHPT_MULTIPATCH   32

#define SHPT_GENERALPOLYLINE    50
#define SHPT_GENERALPOLYGON     51
#define SHPT_GENERALPOINT       52
#define SHPT_GENERALMULTIPOINT  53
#define SHPT_GENERALMULTIPATCH  54*/

	switch (geometryType & 0xff)
	{
	case 1: //SHPT_POINT
	case 9: //SHPT_POINTZ
	case 11: //SHPT_POINTZM
	case 21: //SHPT_POINTM
		ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, &v);
		x = Math::UInt64_Double(v - 1) / this->tableInfo->xyScale + this->tableInfo->xOrigin;
		ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, &v);
		y = Math::UInt64_Double(v - 1) / this->tableInfo->xyScale + this->tableInfo->yOrigin;
		if (this->tableInfo->geometryFlags & 0x80)
		{
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, &v);
			z = Math::UInt64_Double(v - 1) / this->tableInfo->zScale + this->tableInfo->zOrigin;
		}
		if (this->tableInfo->geometryFlags & 0x40)
		{
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, &v);
			//m = Math::UInt64_Double(v - 1) / this->tableInfo->mScale + this->tableInfo->mOrigin;
		}
		srid = 0;
		if (this->tableInfo->csys)
		{
			srid = this->tableInfo->csys->GetSRID();
		}
		if (this->tableInfo->geometryType & 0x80)
		{
			Math::Point3D *pt;
			NEW_CLASS(pt, Math::Point3D(srid, x, y, z));
			return pt;
		}
		else
		{
			Math::Point *pt;
			NEW_CLASS(pt, Math::Point(srid, x, y));
			return pt;
		}
		break;
	case 3: //SHPT_ARC
	case 10: //SHPT_ARCZ
	case 13: //SHPT_ARCZM
	case 23: //SHPT_ARCM
		if (this->rowData[ofst] == 0)
		{
			return 0;
		}
		else
		{
			UInt64 nPoints;
			UInt64 nParts;
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, &nPoints);
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, &nParts);
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, &v); //xmin
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, &v); //ymin
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, &v); //xmax
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, &v); //ymax
			Math::Polyline *pl;
			srid = 0;
			if (this->tableInfo->csys)
			{
				srid = this->tableInfo->csys->GetSRID();
			}
			UOSInt i;
			UInt32 *parts;
			Double *points;
			Double *altitiudes = 0;
			if (this->tableInfo->geometryFlags & 0x80)
			{
				NEW_CLASS(pl, Math::Polyline3D(srid, (UOSInt)nParts, (UOSInt)nPoints));
				altitiudes = ((Math::Polyline3D*)pl)->GetAltitudeList(&i);
			}
			else
			{
				NEW_CLASS(pl, Math::Polyline(srid, (UOSInt)nParts, (UOSInt)nPoints));

			}
			parts = pl->GetPtOfstList(&i);
			points = pl->GetPointList(&i);
			parts[0] = 0;
			UInt32 ptOfst = 0;
			i = 1;
			while (i < nParts)
			{
				ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, &v);
				ptOfst += (UInt32)v;
				parts[i] = ptOfst;
				i++;
			}
			Int64 iv;
			OSInt dx = 0;
			OSInt dy = 0;
			i = 0;
			while (i < nPoints)
			{
				ofst = Map::ESRI::FileGDBUtil::ReadVarInt(this->rowData, ofst, &iv);
				dx -= (OSInt)iv;
				x = Math::Int64_Double(dx) / this->tableInfo->xyScale + this->tableInfo->xOrigin;
				ofst = Map::ESRI::FileGDBUtil::ReadVarInt(this->rowData, ofst, &iv);
				dy -= (OSInt)iv;
				y = Math::Int64_Double(dy) / this->tableInfo->xyScale + this->tableInfo->yOrigin;
				points[i * 2] = x;
				points[i * 2 + 1] = y;
				i++;
			}
			if (this->tableInfo->geometryFlags & 0x80)
			{
				dx = 0;
				i = 0;
				while (i < nPoints)
				{
					ofst = Map::ESRI::FileGDBUtil::ReadVarInt(this->rowData, ofst, &iv);
					dx -= iv;
					z = Math::Int64_Double(dx) / this->tableInfo->zScale + this->tableInfo->zOrigin;
					altitiudes[i] = z;
					i++;
				}
			}
			if (this->tableInfo->geometryFlags & 0x40)
			{
				dx = 0;
				i = 0;
				while (i < nPoints)
				{
					ofst = Map::ESRI::FileGDBUtil::ReadVarInt(this->rowData, ofst, &iv);
					dx -= iv;
					//m = Math::Int64_Double(dx) / this->tableInfo->mScale + this->tableInfo->mOrigin;
					i++;
				}
			}
			return pl;
		}
		break;
	case 5: //SHPT_POLYGON
	case 15: //SHPT_POLYGONZM
	case 19: //SHPT_POLYGONZ
	case 25: //SHPT_POLYGONM
		if (this->rowData[ofst] == 0)
		{
			return 0;
		}
		else
		{
			UInt64 nPoints;
			UInt64 nParts;
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, &nPoints);
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, &nParts);
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, &v); //xmin
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, &v); //ymin
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, &v); //xmax
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, &v); //ymax
			Math::Polygon *pg;
			srid = 0;
			if (this->tableInfo->csys)
			{
				srid = this->tableInfo->csys->GetSRID();
			}
			NEW_CLASS(pg, Math::Polygon(srid, (UOSInt)nParts, (UOSInt)nPoints));
			UOSInt i;
			UInt32 *parts = pg->GetPtOfstList(&i);
			Double *points = pg->GetPointList(&i);
			parts[0] = 0;
			UInt32 ptOfst = 0;
			i = 1;
			while (i < nParts)
			{
				ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, &v);
				ptOfst += (UInt32)v;
				parts[i] = ptOfst;
				i++;
			}
			Int64 iv;
			OSInt dx = 0;
			OSInt dy = 0;
			i = 0;
			while (i < nPoints)
			{
				ofst = Map::ESRI::FileGDBUtil::ReadVarInt(this->rowData, ofst, &iv);
				dx += (OSInt)iv;
				x = Math::Int64_Double(dx) / this->tableInfo->xyScale + this->tableInfo->xOrigin;
				ofst = Map::ESRI::FileGDBUtil::ReadVarInt(this->rowData, ofst, &iv);
				dy += (OSInt)iv;
				y = Math::Int64_Double(dy) / this->tableInfo->xyScale + this->tableInfo->yOrigin;
				points[i * 2] = x;
				points[i * 2 + 1] = y;
				i++;
			}
			if (this->tableInfo->geometryFlags & 0x80)
			{
				dx = 0;
				i = 0;
				while (i < nPoints)
				{
					ofst = Map::ESRI::FileGDBUtil::ReadVarInt(this->rowData, ofst, &iv);
					dx += iv;
					z = Math::Int64_Double(dx) / this->tableInfo->zScale + this->tableInfo->zOrigin;
					i++;
				}
			}
			if (this->tableInfo->geometryFlags & 0x40)
			{
				dx = 0;
				i = 0;
				while (i < nPoints)
				{
					ofst = Map::ESRI::FileGDBUtil::ReadVarInt(this->rowData, ofst, &iv);
					dx += iv;
					//m = Math::Int64_Double(dx) / this->tableInfo->mScale + this->tableInfo->mOrigin;
					i++;
				}
			}
			return pg;
		}
		break;
	case 50: //SHPT_GENERALPOLYLINE
		if (this->rowData[ofst] == 0)
		{
			return 0;
		}
		else
		{
			UOSInt nPoints;
			UOSInt nParts;
			UOSInt nCurves = 0;
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, &nPoints);
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, &nParts);
			if (geometryType & 0x20000000)
			{
				ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, &nCurves);
			}
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, &v); //xmin
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, &v); //ymin
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, &v); //xmax
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, &v); //ymax
			Math::Polyline *pl;
			srid = 0;
			UOSInt i;
			if (this->tableInfo->csys)
			{
				srid = this->tableInfo->csys->GetSRID();
			}
			UInt32 *parts;
			Double *points;
			Double *altitiudes = 0;
			if (geometryType & 0x80000000)
			{
				NEW_CLASS(pl, Math::Polyline3D(srid, (UOSInt)nParts, (UOSInt)nPoints));
				altitiudes = ((Math::Polyline3D*)pl)->GetAltitudeList(&i);
			}
			else
			{
				NEW_CLASS(pl, Math::Polyline(srid, (UOSInt)nParts, (UOSInt)nPoints));

			}
			parts = pl->GetPtOfstList(&i);
			points = pl->GetPointList(&i);
			parts[0] = 0;
			UInt32 ptOfst = 0;
			i = 1;
			while (i < nParts)
			{
				ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, &v);
				ptOfst += (UInt32)v;
				parts[i] = ptOfst;
				i++;
			}
			UOSInt j;
			UOSInt k;
			i = 0;
			while (i < nParts)
			{
				OSInt iv;
				OSInt dx = 0;
				OSInt dy = 0;
				j = parts[i];
				if (i + 1 < nParts)
				{
					k = parts[i + 1];
				}
				else
				{
					k = nPoints;
				}
				while (j < k)
				{
					ofst = Map::ESRI::FileGDBUtil::ReadVarInt(this->rowData, ofst, &iv);
					dx -= iv;
					x = Math::OSInt2Double(dx) / this->tableInfo->xyScale + this->tableInfo->xOrigin;
					ofst = Map::ESRI::FileGDBUtil::ReadVarInt(this->rowData, ofst, &iv);
					dy -= iv;
					y = Math::OSInt2Double(dy) / this->tableInfo->xyScale + this->tableInfo->yOrigin;
					points[j * 2] = x;
					points[j * 2 + 1] = y;
					j++;
				}
				if (geometryType & 0x80000000)
				{
					dx = 0;
					j = parts[i];
					while (j < k)
					{
						ofst = Map::ESRI::FileGDBUtil::ReadVarInt(this->rowData, ofst, &iv);
						dx -= iv;
						z = Math::OSInt2Double(dx) / this->tableInfo->zScale + this->tableInfo->zOrigin;
						altitiudes[j] = z;
						j++;
					}
				}
				if (geometryType & 0x40000000)
				{
					dx = 0;
					j = parts[i];
					while (j < k)
					{
						ofst = Map::ESRI::FileGDBUtil::ReadVarInt(this->rowData, ofst, &iv);
						dx -= iv;
						//m = Math::OSInt2Double(dx) / this->tableInfo->mScale + this->tableInfo->mOrigin;
					}
				}
				i++;
			}
			MemFree(parts);
		}
		break;
	}
	return 0;
}

Bool Map::ESRI::FileGDBReader::IsNull(UOSInt colIndex)
{
	if (colIndex >= this->tableInfo->fields->GetCount())
	{
		return true;
	}
	return this->fieldNull[colIndex] != 0;
}

UTF8Char *Map::ESRI::FileGDBReader::GetName(UOSInt colIndex, UTF8Char *buff)
{
	Map::ESRI::FileGDBFieldInfo *field = this->tableInfo->fields->GetItem(colIndex);
	if (field && field->name)
	{
		return Text::StrConcat(buff, field->name);
	}
	return 0;
}

DB::DBUtil::ColType Map::ESRI::FileGDBReader::GetColType(UOSInt colIndex, UOSInt *colSize)
{
	Map::ESRI::FileGDBFieldInfo *field = this->tableInfo->fields->GetItem(colIndex);
	if (field)
	{
		if (colSize)
			*colSize = field->fieldSize;
		switch (field->fieldType)
		{
		case 0:
			return DB::DBUtil::CT_Int16;
		case 1:
			return DB::DBUtil::CT_Int32;
		case 2:
			return DB::DBUtil::CT_Float;
		case 3:
			return DB::DBUtil::CT_Double;
		case 4:
			return DB::DBUtil::CT_VarChar;
		case 5:
			return DB::DBUtil::CT_DateTime;
		case 6:
			return DB::DBUtil::CT_Int32;
		case 7:
			return DB::DBUtil::CT_Vector;
		case 8:
			return DB::DBUtil::CT_Binary;
		case 9:
			return DB::DBUtil::CT_Binary;
		case 10:
			return DB::DBUtil::CT_UUID;
		case 11:
			return DB::DBUtil::CT_UUID;
		case 12:
			return DB::DBUtil::CT_VarChar;
		default:
			return DB::DBUtil::CT_Unknown;
		}
	}
	return DB::DBUtil::CT_Unknown;
}

Bool Map::ESRI::FileGDBReader::GetColDef(UOSInt colIndex, DB::ColDef *colDef)
{
	Map::ESRI::FileGDBFieldInfo *field = this->tableInfo->fields->GetItem(colIndex);
	if (field == 0)
	{
		return false;
	}
	UTF8Char sbuff[32];
	UOSInt colSize;
	colDef->SetColName(field->name);
	colDef->SetColSize(field->fieldSize);
	colDef->SetColType(this->GetColType(colIndex, &colSize));
	colDef->SetNotNull((field->flags & 1) == 0);
	colDef->SetPK(field->fieldType == 6);
	colDef->SetAutoInc(field->fieldType == 6);
	if (field->defValue)
	{
		if (field->fieldType == 0)
		{
			if (field->defSize == 2)
			{
				Text::StrInt16(sbuff, ReadInt16(field->defValue));
				colDef->SetDefVal(sbuff);
			}
		}
		else if (field->fieldType == 1)
		{
			if (field->defSize == 4)
			{
				Text::StrInt32(sbuff, ReadInt32(field->defValue));
				colDef->SetDefVal(sbuff);
			}
		}
		else if (field->fieldType == 2)
		{
			if (field->defSize == 4)
			{
				Text::StrDouble(sbuff, ReadFloat(field->defValue));
				colDef->SetDefVal(sbuff);
			}
		}
		else if (field->fieldType == 3)
		{
			if (field->defSize == 8)
			{
				Text::StrDouble(sbuff, ReadDouble(field->defValue));
				colDef->SetDefVal(sbuff);
			}
		}
		else if (field->fieldType == 4)
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(field->defValue, field->defSize);
			colDef->SetDefVal(sb.ToString());
		}
		else if (field->fieldType == 5)
		{
			if (field->defSize == 8)
			{
				Data::DateTime dt;
				Map::ESRI::FileGDBUtil::ToDateTime(&dt, ReadDouble(field->defValue));
				dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
				colDef->SetDefVal(sbuff);
			}
		}
	}
	return true;
}

void Map::ESRI::FileGDBReader::DelNewStr(const UTF8Char *s)
{
	Text::StrDelNew(s);
}
