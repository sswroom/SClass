#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Data/UUID.h"
#include "DB/ColDef.h"
#include "Map/ESRI/ESRICurve.h"
#include "Map/ESRI/FileGDBReader.h"
#include "Math/Math.h"
#include "Math/WKTWriter.h"
#include "Math/Geometry/MultiPolygon.h"
#include "Math/Geometry/PointM.h"
#include "Math/Geometry/PointZM.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"
#include "Text/XLSUtil.h"

#include <stdio.h>

UOSInt Map::ESRI::FileGDBReader::GetFieldIndex(UOSInt colIndex)
{
	if (this->columnIndices)
	{
		if (colIndex >= this->columnIndices->GetCount())
		{
			return INVALID_INDEX;
		}
		else
		{
			return this->columnIndices->GetItem(colIndex);
		}
	}
	else
	{
		return colIndex;
	}
}

Map::ESRI::FileGDBReader::FileGDBReader(NN<IO::StreamData> fd, UInt64 ofst, NN<FileGDBTableInfo> tableInfo, Optional<Data::ArrayListStringNN> columnNames, UOSInt dataOfst, UOSInt maxCnt, Optional<Data::QueryConditions> conditions, UInt32 maxRowSize)
{
	this->indexCnt = 0;
	this->indexNext = 0;
	this->fd = fd->GetPartialData(0, fd->GetDataSize());
	this->currOfst = ofst;
	this->rowOfst = ofst;
	this->tableInfo = Map::ESRI::FileGDBUtil::TableInfoClone(tableInfo);
	this->rowSize = 0;
	this->objectId = 0;
	UOSInt fieldCnt = tableInfo->fields->GetCount();
	this->fieldNull = MemAlloc(UInt8, fieldCnt);
	this->fieldOfst = MemAlloc(UInt32, fieldCnt);
	MemClear(this->fieldNull, fieldCnt);
	MemClear(this->fieldOfst, fieldCnt * 4);
	this->dataOfst = dataOfst;
	this->maxRowSize = maxRowSize;
	this->maxCnt = maxCnt;
	if (this->maxCnt == 0)
	{
		this->maxCnt = INVALID_INDEX;
	}
	this->columnIndices = 0;
	this->conditions = conditions;
	NN<Data::ArrayListStringNN> nncolumnNames;
	if (columnNames.SetTo(nncolumnNames))
	{
		NEW_CLASS(this->columnIndices, Data::ArrayList<UOSInt>());
		UOSInt i = 0;
		UOSInt j = nncolumnNames->GetCount();
		UOSInt k = 0;
		while (i < j)
		{
			Bool found = false;
			NN<Text::String> name;
			if (nncolumnNames->GetItem(i).SetTo(name))
			{
				k = this->tableInfo->fields->GetCount();
				while (k-- > 0)
				{
					if (this->tableInfo->fields->GetItem(k)->name->Equals(name))
					{
						found = true;
						break;
					}
				}
			}
			if (!found)
			{
				this->columnIndices->Clear();
				break;
			}
			this->columnIndices->Add(k);
			i++;
		}
	}
}

Map::ESRI::FileGDBReader::~FileGDBReader()
{
	this->fd.Delete();
	Map::ESRI::FileGDBUtil::FreeTableInfo(this->tableInfo);
	MemFree(this->fieldNull);
	MemFree(this->fieldOfst);
	SDEL_CLASS(this->columnIndices);
}

Bool Map::ESRI::FileGDBReader::ReadNext()
{
	NN<Data::QueryConditions> nncondition;
	UInt8 sizeBuff[4];
	while (true)
	{
		this->rowData.Delete();
		if (this->indexBuff.GetSize() > 0)
		{
			while (true)
			{
				if (this->indexNext >= this->indexCnt)
				{
					return false;
				}

				this->currOfst = ReadUInt32(&this->indexBuff[this->indexNext * 5 + 1]);
				this->currOfst = (this->currOfst << 8) + this->indexBuff[this->indexNext * 5];
				this->indexNext++;
				this->objectId = (Int32)this->indexNext;
				if (this->currOfst != 0)
				{
					if (this->fd->GetRealData(this->currOfst, 4, BYTEARR(sizeBuff)) != 4)
					{
						return false;
					}
					this->rowOfst = this->currOfst;
					Int32 size = ReadInt32(sizeBuff);
					if (size < 0)
					{
						return false;
					}
					else
					{
						this->rowSize = (UInt32)size;
					}
					if (this->currOfst + 4 + this->rowSize > this->fd->GetDataSize())
					{
						return false;
					}
					break;
				}
			}
		}
		else
		{
			Bool lastIsFree = false;
			while (true)
			{
				if (this->fd->GetRealData(this->currOfst, 4, BYTEARR(sizeBuff)) != 4)
				{
					return false;
				}
				Int32 size = ReadInt32(sizeBuff);
				if (size < 0)
				{
					this->currOfst += 4 + (UInt32)(-size);
					lastIsFree = true;
				}
				else if ((UInt32)size > this->maxRowSize && lastIsFree)
				{
					this->currOfst += 4;
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
			this->objectId++;
		}
		this->rowData.ChangeSizeAndClear(this->rowSize);
		if (this->fd->GetRealData(this->currOfst + 4, this->rowSize, this->rowData) != this->rowSize)
		{
			this->rowData.Delete();
			return false;
		}
		this->rowOfst = this->currOfst;
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
					rowOfst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, rowOfst, v);
					if (rowOfst + v > this->rowSize)
					{
						return false;
					}
					rowOfst += (UOSInt)v;
					break;
				case 5:
					rowOfst += 8;
					break;
				case 6:
					break;
				case 7:
					rowOfst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, rowOfst, v);
					rowOfst += (UOSInt)v;
					break;
				case 8:
					rowOfst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, rowOfst, v);
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
					rowOfst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, rowOfst, v);
					rowOfst += (UOSInt)v;
					break;
				}
			}
			i++;
		}
		Bool valid = true;
		if (conditions.SetTo(nncondition))
		{
			if (!nncondition->IsValid(*this, valid))
			{
				valid = true;
			}
		}
		if (valid)
		{
			if (this->dataOfst == 0)
			{
				if (this->maxCnt == 0)
				{
					return false;
				}
				this->maxCnt--;
				break;
			}
			this->dataOfst--;
		}
	}
	return true;
}

UOSInt Map::ESRI::FileGDBReader::ColCount()
{
	if (this->columnIndices)
	{
		return this->columnIndices->GetCount();
	}
	else
	{
		return this->tableInfo->fields->GetCount();
	}
}

OSInt Map::ESRI::FileGDBReader::GetRowChanged()
{
	return 0;
}

Int32 Map::ESRI::FileGDBReader::GetInt32(UOSInt colIndex)
{
	UOSInt fieldIndex = this->GetFieldIndex(colIndex);
	if (this->rowData.GetSize() == 0)
	{
		return 0;
	}
	Map::ESRI::FileGDBFieldInfo *field = this->tableInfo->fields->GetItem(fieldIndex);
	if (field == 0 || this->fieldNull[fieldIndex])
	{
		return 0;
	}
	switch (field->fieldType)
	{
	case 0:
		return ReadInt16(&this->rowData[this->fieldOfst[fieldIndex]]);
	case 1:
		return ReadInt32(&this->rowData[this->fieldOfst[fieldIndex]]);
	case 2:
		return Double2Int32(ReadFloat(&this->rowData[this->fieldOfst[fieldIndex]]));
	case 3:
		return Double2Int32(ReadDouble(&this->rowData[this->fieldOfst[fieldIndex]]));
	case 5:
		return (Int32)ReadDouble(&this->rowData[this->fieldOfst[fieldIndex]]);
	case 6:
		return this->objectId;
	case 4:
	case 12:
		{
			UInt64 v;
			Text::StringBuilderUTF8 sb;
			UOSInt ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, this->fieldOfst[fieldIndex], v);
			sb.AppendC(&this->rowData[ofst], (UOSInt)v);
			return sb.ToInt32();
		}
	}
	return 0;
}

Int64 Map::ESRI::FileGDBReader::GetInt64(UOSInt colIndex)
{
	UOSInt fieldIndex = this->GetFieldIndex(colIndex);
	if (this->rowData.GetSize() == 0)
	{
		return 0;
	}
	Map::ESRI::FileGDBFieldInfo *field = this->tableInfo->fields->GetItem(fieldIndex);
	if (field == 0 || this->fieldNull[fieldIndex])
	{
		return 0;
	}
	switch (field->fieldType)
	{
	case 0:
		return ReadInt16(&this->rowData[this->fieldOfst[fieldIndex]]);
	case 1:
		return ReadInt32(&this->rowData[this->fieldOfst[fieldIndex]]);
	case 2:
		return Double2Int64(ReadFloat(&this->rowData[this->fieldOfst[fieldIndex]]));
	case 3:
		return Double2Int64(ReadDouble(&this->rowData[this->fieldOfst[fieldIndex]]));
	case 5:
		return (Int64)ReadDouble(&this->rowData[this->fieldOfst[fieldIndex]]);
	case 6:
		return this->objectId;
	case 4:
	case 12:
		{
			UInt64 v;
			Text::StringBuilderUTF8 sb;
			UOSInt ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, this->fieldOfst[fieldIndex], v);
			sb.AppendC(&this->rowData[ofst], (UOSInt)v);
			return sb.ToInt64();
		}
	}
	return 0;
}

UnsafeArrayOpt<WChar> Map::ESRI::FileGDBReader::GetStr(UOSInt colIndex, UnsafeArray<WChar> buff)
{
	Text::StringBuilderUTF8 sb;
	if (this->GetStr(colIndex, sb))
	{
		return Text::StrUTF8_WChar(buff, sb.ToString(), 0);
	}
	return 0;
}

Bool Map::ESRI::FileGDBReader::GetStr(UOSInt colIndex, NN<Text::StringBuilderUTF8> sb)
{
	UOSInt fieldIndex = this->GetFieldIndex(colIndex);
	if (this->rowData.GetSize() == 0)
	{
		return false;
	}
	Map::ESRI::FileGDBFieldInfo *field = this->tableInfo->fields->GetItem(fieldIndex);
	if (field == 0 || this->fieldNull[fieldIndex])
	{
		return false;
	}
	UInt64 v;
	UOSInt ofst;
	switch (field->fieldType)
	{
	case 0:
		sb->AppendI16(ReadInt16(&this->rowData[this->fieldOfst[fieldIndex]]));
		return true;
	case 1:
		sb->AppendI32(ReadInt32(&this->rowData[this->fieldOfst[fieldIndex]]));
		return true;
	case 2:
		Text::SBAppendF32(sb, ReadFloat(&this->rowData[this->fieldOfst[fieldIndex]]));
		return true;
	case 3:
		Text::SBAppendF64(sb, ReadDouble(&this->rowData[this->fieldOfst[fieldIndex]]));
		return true;
	case 12:
	case 4:
		ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, this->fieldOfst[fieldIndex], v);
		sb->AppendC(&this->rowData[ofst], (UOSInt)v);
		return true;
	case 5:
		{
			sb->AppendTSNoZone(Text::XLSUtil::Number2Timestamp(ReadDouble(&this->rowData[this->fieldOfst[fieldIndex]])).RoundToS());
		}
		return true;
	case 6:
		sb->AppendI32(this->objectId);
		return true;
	case 7:
		{
			NN<Math::Geometry::Vector2D> vec;;
			if (this->GetVector(colIndex).SetTo(vec))
			{
				Math::WKTWriter writer;
				Bool succ = writer.ToText(sb, vec);
				vec.Delete();
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
			Data::UUID uuid(&this->rowData[this->fieldOfst[fieldIndex]]);
			uuid.ToString(sb);
			return true;
		}
	}
	return false;
}

Optional<Text::String> Map::ESRI::FileGDBReader::GetNewStr(UOSInt colIndex)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UOSInt fieldIndex = this->GetFieldIndex(colIndex);
	if (this->rowData.GetSize() == 0)
	{
		return 0;
	}
	Map::ESRI::FileGDBFieldInfo *field = this->tableInfo->fields->GetItem(fieldIndex);
	if (field == 0 || this->fieldNull[fieldIndex])
	{
		return 0;
	}
	UInt64 v;
	UOSInt ofst;
	switch (field->fieldType)
	{
	case 0:
		sptr = Text::StrInt16(sbuff, ReadInt16(&this->rowData[this->fieldOfst[fieldIndex]]));
		return Text::String::NewP(sbuff, sptr);
	case 1:
		sptr = Text::StrInt32(sbuff, ReadInt32(&this->rowData[this->fieldOfst[fieldIndex]]));
		return Text::String::NewP(sbuff, sptr);
	case 2:
		sptr = Text::StrDouble(sbuff, ReadFloat(&this->rowData[this->fieldOfst[fieldIndex]]));
		return Text::String::NewP(sbuff, sptr);
	case 3:
		sptr = Text::StrDouble(sbuff, ReadDouble(&this->rowData[this->fieldOfst[fieldIndex]]));
		return Text::String::NewP(sbuff, sptr);
	case 12:
	case 4:
		ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, this->fieldOfst[fieldIndex], v);
		return Text::String::New(&this->rowData[ofst], (UOSInt)v);
	case 5:
		{
			sptr = Text::XLSUtil::Number2Timestamp(ReadDouble(&this->rowData[this->fieldOfst[fieldIndex]])).RoundToS().ToString(sbuff);
			return Text::String::NewP(sbuff, sptr);
		}
	case 6:
		sptr = Text::StrInt32(sbuff, this->objectId);
		return Text::String::NewP(sbuff, sptr);
	case 7:
		{
			NN<Math::Geometry::Vector2D> vec;
			if (this->GetVector(colIndex).SetTo(vec))
			{
				Text::StringBuilderUTF8 sb;
				Math::WKTWriter writer;
				writer.ToText(sb, vec);
				vec.Delete();
				return Text::String::New(sb.ToCString());
			}
		}
		return 0;
	case 8:
		{
			Text::StringBuilderUTF8 sb;
			UOSInt size = this->GetBinarySize(colIndex);
			UInt8 *binBuff = MemAlloc(UInt8, size);
			this->GetBinary(colIndex, binBuff);
			sb.AppendHexBuff(binBuff, size, 0, Text::LineBreakType::None);
			MemFree(binBuff);
			return Text::String::New(sb.ToCString());
		}
	case 10:
	case 11:
		{
			Text::StringBuilderUTF8 sb;
			Data::UUID uuid(&this->rowData[this->fieldOfst[fieldIndex]]);
			uuid.ToString(sb);
			return Text::String::New(sb.ToCString());
		}
	}
	return 0;
}

UnsafeArrayOpt<UTF8Char> Map::ESRI::FileGDBReader::GetStr(UOSInt colIndex, UnsafeArray<UTF8Char> buff, UOSInt buffSize)
{
	Text::StringBuilderUTF8 sb;
	if (this->GetStr(colIndex, sb))
	{
		return Text::StrConcatS(buff, sb.ToString(), buffSize);
	}
	return 0;
}

Data::Timestamp Map::ESRI::FileGDBReader::GetTimestamp(UOSInt colIndex)
{
	UOSInt fieldIndex = this->GetFieldIndex(colIndex);
	if (this->rowData.GetSize() == 0)
	{
		return Data::Timestamp(0);
	}
	Map::ESRI::FileGDBFieldInfo *field = this->tableInfo->fields->GetItem(fieldIndex);
	if (field == 0)
	{
		return Data::Timestamp(0);
	}
	else if (this->fieldNull[fieldIndex])
	{
		return Data::Timestamp(0);
	}
	switch (field->fieldType)
	{
	case 5:
		return Text::XLSUtil::Number2Timestamp(ReadDouble(&this->rowData[this->fieldOfst[fieldIndex]])).RoundToS();
	case 4:
	case 12:
		{
			UInt64 v;
			Text::StringBuilderUTF8 sb;
			UOSInt ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, this->fieldOfst[fieldIndex], v);
			sb.AppendC(&this->rowData[ofst], (UOSInt)v);
			return Data::Timestamp::FromStr(sb.ToCString(), 0);
		}
	}
	return Data::Timestamp(0);
}

Double Map::ESRI::FileGDBReader::GetDblOrNAN(UOSInt colIndex)
{
	UOSInt fieldIndex = this->GetFieldIndex(colIndex);
	if (this->rowData.GetSize() == 0)
	{
		return NAN;
	}
	Map::ESRI::FileGDBFieldInfo *field = this->tableInfo->fields->GetItem(fieldIndex);
	if (field == 0 || this->fieldNull[fieldIndex])
	{
		return NAN;
	}
	switch (field->fieldType)
	{
	case 0:
		return ReadInt16(&this->rowData[this->fieldOfst[fieldIndex]]);
	case 1:
		return ReadInt32(&this->rowData[this->fieldOfst[fieldIndex]]);
	case 2:
		return ReadFloat(&this->rowData[this->fieldOfst[fieldIndex]]);
	case 3:
		return ReadDouble(&this->rowData[this->fieldOfst[fieldIndex]]);
	case 5:
		return ReadDouble(&this->rowData[this->fieldOfst[fieldIndex]]);
	case 6:
		return this->objectId;
	case 4:
	case 12:
		{
			UInt64 v;
			Text::StringBuilderUTF8 sb;
			UOSInt ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, this->fieldOfst[fieldIndex], v);
			sb.AppendC(&this->rowData[ofst], (UOSInt)v);
			return sb.ToDoubleOrNAN();
		}
	}
	return NAN;
}

Bool Map::ESRI::FileGDBReader::GetBool(UOSInt colIndex)
{
	return this->GetInt32(colIndex) != 0;
}

UOSInt Map::ESRI::FileGDBReader::GetBinarySize(UOSInt colIndex)
{
	UOSInt fieldIndex = this->GetFieldIndex(colIndex);
	if (this->rowData.GetSize() == 0)
	{
		return 0;
	}
	Map::ESRI::FileGDBFieldInfo *field = this->tableInfo->fields->GetItem(fieldIndex);
	if (field == 0 || this->fieldNull[fieldIndex])
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
		Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, this->fieldOfst[fieldIndex], v);
		return (UOSInt)v;
	case 5:
		return 8;
	case 6:
		return 4;
	case 7:
		Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, this->fieldOfst[fieldIndex], v);
		return (UOSInt)v;
	case 8:
		Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, this->fieldOfst[fieldIndex], v);
		return (UOSInt)v;
	case 10:
	case 11:
		return 16;
	}
	return 0;
}

UOSInt Map::ESRI::FileGDBReader::GetBinary(UOSInt colIndex, UnsafeArray<UInt8> buff)
{
	UOSInt fieldIndex = this->GetFieldIndex(colIndex);
	if (this->rowData.GetSize() == 0)
	{
		return 0;
	}
	Map::ESRI::FileGDBFieldInfo *field = this->tableInfo->fields->GetItem(fieldIndex);
	if (field == 0 || this->fieldNull[fieldIndex])
	{
		return 0;
	}
	UInt64 v;
	UOSInt ofst;
	switch (field->fieldType)
	{
	case 0:
		MemCopyNO(buff.Ptr(), &this->rowData[this->fieldOfst[fieldIndex]], 2);
		return 2;
	case 1:
		MemCopyNO(buff.Ptr(), &this->rowData[this->fieldOfst[fieldIndex]], 4);
		return 4;
	case 2:
		MemCopyNO(buff.Ptr(), &this->rowData[this->fieldOfst[fieldIndex]], 4);
		return 4;
	case 3:
		MemCopyNO(buff.Ptr(), &this->rowData[this->fieldOfst[fieldIndex]], 8);
		return 8;
	case 12:
	case 4:
		ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, this->fieldOfst[fieldIndex], v);
		MemCopyNO(buff.Ptr(), &this->rowData[ofst], (UOSInt)v);
		return (UOSInt)v;
	case 5:
		MemCopyNO(buff.Ptr(), &this->rowData[this->fieldOfst[fieldIndex]], 8);
		return 8;
	case 6:
		MemCopyNO(buff.Ptr(), &this->objectId, 4);
		return 4;
	case 7:
		ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, this->fieldOfst[fieldIndex], v);
		MemCopyNO(buff.Ptr(), &this->rowData[ofst], (UOSInt)v);
		return (UOSInt)v;
	case 8:
		ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, this->fieldOfst[fieldIndex], v);
		MemCopyNO(buff.Ptr(), &this->rowData[ofst], (UOSInt)v);
		return (UOSInt)v;
	case 10:
	case 11:
		MemCopyNO(buff.Ptr(), &this->rowData[this->fieldOfst[fieldIndex]], 16);
		return 16;
	}
	return 0;
}

Optional<Math::Geometry::Vector2D> Map::ESRI::FileGDBReader::GetVector(UOSInt colIndex)
{
	UOSInt fieldIndex = this->GetFieldIndex(colIndex);
	if (this->rowData.GetSize() == 0)
	{
		return 0;
	}
	Map::ESRI::FileGDBFieldInfo *field = this->tableInfo->fields->GetItem(fieldIndex);
	if (field == 0 || this->fieldNull[fieldIndex])
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
	ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, this->fieldOfst[fieldIndex], geometryLen);
	ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, geometryType);
	Double x;
	Double y;
	Double z = 0;
	Double m = 0;
	UInt64 v;
	UInt32 srid;
	NN<Math::CoordinateSystem> csys;
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
		ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, v);
		x = UInt64_Double(v - 1) / this->tableInfo->xyScale + this->tableInfo->xOrigin;
		ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, v);
		y = UInt64_Double(v - 1) / this->tableInfo->xyScale + this->tableInfo->yOrigin;
		if (this->tableInfo->geometryFlags & 0x80)
		{
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, v);
			z = UInt64_Double(v - 1) / this->tableInfo->zScale + this->tableInfo->zOrigin;
		}
		if (this->tableInfo->geometryFlags & 0x40)
		{
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, v);
			m = UInt64_Double(v - 1) / this->tableInfo->mScale + this->tableInfo->mOrigin;
		}
		srid = 0;
		if (this->tableInfo->csys.SetTo(csys))
		{
			srid = csys->GetSRID();
		}
		switch (this->tableInfo->geometryType & 0xC0)
		{
		case 0:
			{
				Math::Geometry::Point *pt;
				NEW_CLASS(pt, Math::Geometry::Point(srid, x, y));
				return pt;
			}
		case 0x40:
			{
				Math::Geometry::PointM *pt;
				NEW_CLASS(pt, Math::Geometry::PointM(srid, x, y, m));
				return pt;
			}
		case 0x80:
			{
				Math::Geometry::PointZ *pt;
				NEW_CLASS(pt, Math::Geometry::PointZ(srid, x, y, z));
				return pt;
			}
		case 0xC0:
			{
				Math::Geometry::PointZM *pt;
				NEW_CLASS(pt, Math::Geometry::PointZM(srid, x, y, z, m));
				return pt;
			}
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
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, nPoints);
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, nParts);
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, v); //xmin
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, v); //ymin
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, v); //xmax
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, v); //ymax
			Math::Geometry::Polyline *pl;
			srid = 0;
			if (this->tableInfo->csys.SetTo(csys))
			{
				srid = csys->GetSRID();
			}
			UOSInt i;
			UOSInt j;
			UOSInt k;
			NN<Math::Geometry::LineString> lineString;
			UInt32 *ptOfstList;
			UnsafeArray<Math::Coord2DDbl> points;
			UnsafeArray<Double> zArr;
			UnsafeArray<Double> mArr;
			NEW_CLASS(pl, Math::Geometry::Polyline(srid));
			//, (UOSInt)nParts, (UOSInt)nPoints, (this->tableInfo->geometryFlags & 0x80) != 0, (this->tableInfo->geometryFlags & 0x40) != 0
			ptOfstList = MemAlloc(UInt32, (UOSInt)nParts);
			ptOfstList[0] = 0;
			UInt32 ptOfst = 0;
			i = 1;
			while (i < nParts)
			{
				ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, v);
				ptOfst += (UInt32)v;
				ptOfstList[i] = ptOfst;
				i++;
			}
			Int64 iv;
			OSInt dx = 0;
			OSInt dy = 0;
			OSInt dz = 0;
			OSInt dm = 0;
			i = 0;
			while (i < nParts)
			{
				if (i + 1 == nParts)
				{
					k = nPoints - ptOfstList[i];
				}
				else
				{
					k = ptOfstList[i + 1] - ptOfstList[i];
				}
				NEW_CLASSNN(lineString, Math::Geometry::LineString(srid, k, (this->tableInfo->geometryFlags & 0x80) != 0, (this->tableInfo->geometryFlags & 0x40) != 0));
				points = lineString->GetPointList(j);

				j = 0;
				while (j < k)
				{
					ofst = Map::ESRI::FileGDBUtil::ReadVarInt(this->rowData, ofst, iv);
					dx += (OSInt)iv;
					x = Int64_Double(dx) / this->tableInfo->xyScale + this->tableInfo->xOrigin;
					ofst = Map::ESRI::FileGDBUtil::ReadVarInt(this->rowData, ofst, iv);
					dy += (OSInt)iv;
					y = Int64_Double(dy) / this->tableInfo->xyScale + this->tableInfo->yOrigin;
					points[j].x = x;
					points[j].y = y;
					j++;
				}
				if ((this->tableInfo->geometryFlags & 0x80) && lineString->GetZList(j).SetTo(zArr))
				{
					j = 0;
					while (j < k)
					{
						ofst = Map::ESRI::FileGDBUtil::ReadVarInt(this->rowData, ofst, iv);
						dz += (OSInt)iv;
						z = Int64_Double(dz) / this->tableInfo->zScale + this->tableInfo->zOrigin;
						zArr[j] = z;
						j++;
					}
				}
				if ((this->tableInfo->geometryFlags & 0x40) && lineString->GetMList(j).SetTo(mArr))
				{
					j = 0;
					while (j < k)
					{
						ofst = Map::ESRI::FileGDBUtil::ReadVarInt(this->rowData, ofst, iv);
						dm += (OSInt)iv;
						m = Int64_Double(dm) / this->tableInfo->mScale + this->tableInfo->mOrigin;
						mArr[j] = m;
						j++;
					}
				}
				pl->AddGeometry(lineString);

				i++;
			}
			MemFree(ptOfstList);
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
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, nPoints);
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, nParts);
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, v); //xmin
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, v); //ymin
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, v); //xmax
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, v); //ymax
			NN<Math::Geometry::Polygon> pg;
			srid = 0;
			if (this->tableInfo->csys.SetTo(csys))
			{
				srid = csys->GetSRID();
			}
			NEW_CLASSNN(pg, Math::Geometry::Polygon(srid));
			UOSInt i;
			UInt32 *parts = MemAlloc(UInt32, (UOSInt)nParts);
			Math::Coord2DDbl *points = MemAllocA(Math::Coord2DDbl, (UOSInt)nPoints);
			Double *zArr = 0;
			Double *mArr = 0;
			if ((this->tableInfo->geometryFlags & 0x80) != 0)
			{
				zArr = MemAlloc(Double, (UOSInt)nPoints);
			}
			if ((this->tableInfo->geometryFlags & 0x40) != 0)
			{
				mArr = MemAlloc(Double, (UOSInt)nPoints);
			}
			parts[0] = 0;
			UInt32 ptOfst = 0;
			i = 1;
			while (i < nParts)
			{
				ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, v);
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
				ofst = Map::ESRI::FileGDBUtil::ReadVarInt(this->rowData, ofst, iv);
				dx += (OSInt)iv;
				x = Int64_Double(dx) / this->tableInfo->xyScale + this->tableInfo->xOrigin;
				ofst = Map::ESRI::FileGDBUtil::ReadVarInt(this->rowData, ofst, iv);
				dy += (OSInt)iv;
				y = Int64_Double(dy) / this->tableInfo->xyScale + this->tableInfo->yOrigin;
				points[i].x = x;
				points[i].y = y;
				i++;
			}
			if (this->tableInfo->geometryFlags & 0x80)
			{
				dx = 0;
				i = 0;
				while (i < nPoints)
				{
					ofst = Map::ESRI::FileGDBUtil::ReadVarInt(this->rowData, ofst, iv);
					dx += (OSInt)iv;
					z = Int64_Double(dx) / this->tableInfo->zScale + this->tableInfo->zOrigin;
					zArr[i] = z;
					i++;
				}
			}
			if (this->tableInfo->geometryFlags & 0x40)
			{
				dx = 0;
				i = 0;
				while (i < nPoints)
				{
					ofst = Map::ESRI::FileGDBUtil::ReadVarInt(this->rowData, ofst, iv);
					dx += (OSInt)iv;
					m = Int64_Double(dx) / this->tableInfo->mScale + this->tableInfo->mOrigin;
					mArr[i] = m;
					i++;
				}
			}
			pg->AddFromPtOfst(parts, nParts, points, nPoints, zArr, mArr);
			if (mArr)
				MemFree(mArr);
			if (zArr)
				MemFree(zArr);
			MemFreeA(points);
			MemFree(parts);
			NN<Math::Geometry::MultiPolygon> mpg;
			NEW_CLASSNN(mpg, Math::Geometry::MultiPolygon(pg->GetSRID()));
			mpg->AddGeometry(pg);
			return mpg.Ptr();
		}
		break;
	case 50: //SHPT_GENERALPOLYLINE
		if (this->rowData[ofst] == 0)
		{
			return 0;
		}
		else
		{
			UInt64 nPoints;
			UInt64 nParts;
			UInt64 nCurves = 0;
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, nPoints);
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, nParts);
			if (geometryType & 0x20000000)
			{
				ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, nCurves);
			}
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, v); //xmin
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, v); //ymin
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, v); //xmax
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, v); //ymax
			srid = 0;
			if (this->tableInfo->csys.SetTo(csys))
			{
				srid = csys->GetSRID();
			}
			UOSInt i;
			UOSInt j;
			UnsafeArray<UInt32> ptOfstList;
			UnsafeArray<Math::Coord2DDbl> points;
			UnsafeArrayOpt<Double> zArr = 0;
			UnsafeArrayOpt<Double> mArr = 0;

			UnsafeArray<Double> nnArr;
			//, (UOSInt)nParts, (UOSInt)nPoints, (this->tableInfo->geometryFlags & 0x80) != 0, (this->tableInfo->geometryFlags & 0x40) != 0
			ptOfstList = MemAllocArr(UInt32, (UOSInt)nParts);
			ptOfstList[0] = 0;
			UInt32 ptOfst = 0;
			i = 1;
			while (i < nParts)
			{
				ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, v);
				ptOfst += (UInt32)v;
				ptOfstList[i] = ptOfst;
				i++;
			}
			points = MemAllocAArr(Math::Coord2DDbl, (UOSInt)nPoints);
			Int64 iv;
			OSInt dx = 0;
			OSInt dy = 0;
			OSInt dz = 0;
			OSInt dm = 0;
			j = 0;
			while (j < nPoints)
			{
				ofst = Map::ESRI::FileGDBUtil::ReadVarInt(this->rowData, ofst, iv);
				dx += (OSInt)iv;
				x = Int64_Double(dx) / this->tableInfo->xyScale + this->tableInfo->xOrigin;
				ofst = Map::ESRI::FileGDBUtil::ReadVarInt(this->rowData, ofst, iv);
				dy += (OSInt)iv;
				y = Int64_Double(dy) / this->tableInfo->xyScale + this->tableInfo->yOrigin;
				points[j].x = x;
				points[j].y = y;
				j++;
			}
			if ((this->tableInfo->geometryFlags & 0x80))
			{
				nnArr = MemAllocArr(Double, (UOSInt)nPoints);
				zArr = nnArr;
				j = 0;
				while (j < nPoints)
				{
					ofst = Map::ESRI::FileGDBUtil::ReadVarInt(this->rowData, ofst, iv);
					dz += (OSInt)iv;
					z = Int64_Double(dz) / this->tableInfo->zScale + this->tableInfo->zOrigin;
					nnArr[j] = z;
					j++;
				}
			}
			if ((this->tableInfo->geometryFlags & 0x40))
			{
				nnArr = MemAllocArr(Double, (UOSInt)nPoints);
				mArr = nnArr;
				j = 0;
				while (j < nPoints)
				{
					ofst = Map::ESRI::FileGDBUtil::ReadVarInt(this->rowData, ofst, iv);
					dm += (OSInt)iv;
					m = Int64_Double(dm) / this->tableInfo->mScale + this->tableInfo->mOrigin;
					nnArr[j] = m;
					j++;
				}
			}
			NN<Math::Geometry::Vector2D> vec;
			Map::ESRI::ESRICurve curve(srid, ptOfstList, nParts, points, nPoints, zArr, mArr);
			if (nCurves > 0)
			{
				UOSInt type;
				UInt64 uv;
				i = 0;
				while (i < nCurves)
				{
					ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, uv);
					type = this->rowData[ofst];
					if (type == 1) //esriSegmentArc
					{
						curve.AddArc((UOSInt)uv, Math::Coord2DDbl(ReadDouble(&this->rowData[ofst + 1]), ReadDouble(&this->rowData[ofst + 9])), ReadUInt32(&this->rowData[ofst + 17]));
						ofst += 21;
					}
					else if (type == 4) //esriSegmentBezier3Curve
					{
						curve.AddBezier3Curve((UOSInt)uv, Math::Coord2DDbl(ReadDouble(&this->rowData[ofst + 1]), ReadDouble(&this->rowData[ofst + 9])), Math::Coord2DDbl(ReadDouble(&this->rowData[ofst + 17]), ReadDouble(&this->rowData[ofst + 25])));
						ofst += 33;
					}
					else if (type == 5) //esriSegmentEllipticArc
					{
						curve.AddEllipticArc((UOSInt)uv, Math::Coord2DDbl(ReadDouble(&this->rowData[ofst + 1]), ReadDouble(&this->rowData[ofst + 9])), ReadDouble(&this->rowData[ofst + 17]), ReadDouble(&this->rowData[ofst + 25]), ReadDouble(&this->rowData[ofst + 33]), ReadUInt32(&this->rowData[ofst + 41]));
						ofst += 45;
					}
					else
					{
						printf("FGDB: Unsupported curve type: %d\r\n", (UInt32)type);
						break;
					}
					i++;
				}
			}
			vec = curve.CreatePolyline();
			if (zArr.SetTo(nnArr))
			{
				MemFreeArr(nnArr);
			}
			if (mArr.SetTo(nnArr))
			{
				MemFreeArr(nnArr);
			}
			MemFreeAArr(points);
			MemFreeArr(ptOfstList);
			return vec;
		}
		break;
	case 51: //SHPT_GENERALPOLYGON
		if (this->rowData[ofst] == 0)
		{
			return 0;
		}
		else
		{
			UInt64 nPoints;
			UInt64 nParts;
			UInt64 nCurves = 0;
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, nPoints);
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, nParts);
			if (geometryType & 0x20000000)
			{
				ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, nCurves);
			}
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, v); //xmin
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, v); //ymin
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, v); //xmax
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, v); //ymax
			srid = 0;
			UOSInt i;
			if (this->tableInfo->csys.SetTo(csys))
			{
				srid = csys->GetSRID();
			}
			UnsafeArray<UInt32> parts;
			UnsafeArray<Math::Coord2DDbl> points;
			UnsafeArrayOpt<Double> zArr = 0;
			UnsafeArrayOpt<Double> mArr = 0;
			UnsafeArray<Double> nnArr;
			parts = MemAllocArr(UInt32, (UOSInt)nParts);
			points = MemAllocAArr(Math::Coord2DDbl, (UOSInt)nPoints);
			parts[0] = 0;
			UInt32 ptOfst = 0;
			i = 1;
			while (i < nParts)
			{
				ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, v);
				ptOfst += (UInt32)v;
				parts[i] = ptOfst;
				i++;
			}
			UOSInt j;
			OSInt dx = 0;
			OSInt dy = 0;
			OSInt dz = 0;
			OSInt dm = 0;
			Int64 iv;
			j = 0;
			while (j < nPoints)
			{
				ofst = Map::ESRI::FileGDBUtil::ReadVarInt(this->rowData, ofst, iv);
				dx += (OSInt)iv;
				x = OSInt2Double(dx) / this->tableInfo->xyScale + this->tableInfo->xOrigin;
				ofst = Map::ESRI::FileGDBUtil::ReadVarInt(this->rowData, ofst, iv);
				dy += (OSInt)iv;
				y = OSInt2Double(dy) / this->tableInfo->xyScale + this->tableInfo->yOrigin;
				points[j].x = x;
				points[j].y = y;
				j++;
			}
			if (geometryType & 0x80000000)
			{
				nnArr = MemAllocArr(Double, (UOSInt)nPoints);
				zArr = nnArr;
				j = 0;
				while (j < nPoints)
				{
					ofst = Map::ESRI::FileGDBUtil::ReadVarInt(this->rowData, ofst, iv);
					dz += (OSInt)iv;
					z = OSInt2Double(dz) / this->tableInfo->zScale + this->tableInfo->zOrigin;
					nnArr[j] = z;
					j++;
				}
			}
			if (geometryType & 0x40000000)
			{
				nnArr = MemAllocArr(Double, (UOSInt)nPoints);
				mArr = nnArr;
				j = 0;
				while (j < nPoints)
				{
					ofst = Map::ESRI::FileGDBUtil::ReadVarInt(this->rowData, ofst, iv);
					dm += (OSInt)iv;
					m = OSInt2Double(dm) / this->tableInfo->mScale + this->tableInfo->mOrigin;
					nnArr[j] = m;
					j++;
				}
			}
			NN<Math::Geometry::Vector2D> vec;
			Map::ESRI::ESRICurve curve(srid, parts, nParts, points, nPoints, zArr, mArr);
			if (nCurves > 0)
			{
				UOSInt type;
				UInt64 startIndex;
				i = 0;
				while (i < nCurves)
				{
					ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, ofst, startIndex);
					type = this->rowData[ofst];
					if (type == 1) //esriSegmentArc
					{
						curve.AddArc((UOSInt)startIndex, Math::Coord2DDbl(ReadDouble(&this->rowData[ofst + 1]), ReadDouble(&this->rowData[ofst + 9])), ReadUInt32(&this->rowData[ofst + 17]));
						ofst += 21;
					}
					else if (type == 4) //esriSegmentBezier3Curve
					{
						curve.AddBezier3Curve((UOSInt)startIndex, Math::Coord2DDbl(ReadDouble(&this->rowData[ofst + 1]), ReadDouble(&this->rowData[ofst + 9])), Math::Coord2DDbl(ReadDouble(&this->rowData[ofst + 17]), ReadDouble(&this->rowData[ofst + 25])));
						ofst += 33;
					}
					else if (type == 5) //esriSegmentEllipticArc
					{
						curve.AddEllipticArc((UOSInt)startIndex, Math::Coord2DDbl(ReadDouble(&this->rowData[ofst + 1]), ReadDouble(&this->rowData[ofst + 9])), ReadDouble(&this->rowData[ofst + 17]), ReadDouble(&this->rowData[ofst + 25]), ReadDouble(&this->rowData[ofst + 33]), ReadUInt32(&this->rowData[ofst + 41]));
						ofst += 45;
					}
					else
					{
						printf("FGDB: Unsupported curve type: %d\r\n", (UInt32)type);
						break;
					}
					i++;
				}
			}
			vec = curve.CreatePolygon();
			if (mArr.SetTo(nnArr))
			{
				MemFreeArr(nnArr);
			}
			if (zArr.SetTo(nnArr))
			{
				MemFreeArr(nnArr);
			}
			MemFreeAArr(points);
			MemFreeArr(parts);
			return vec;
		}
		break;
	}
	return 0;
}

Bool Map::ESRI::FileGDBReader::GetUUID(UOSInt colIndex, NN<Data::UUID> uuid)
{
	UOSInt fieldIndex = this->GetFieldIndex(colIndex);
	if (this->rowData.GetSize() == 0)
	{
		return false;
	}
	Map::ESRI::FileGDBFieldInfo *field = this->tableInfo->fields->GetItem(fieldIndex);
	if (field == 0 || this->fieldNull[fieldIndex])
	{
		return false;
	}
	switch (field->fieldType)
	{
	case 10:
	case 11:
		uuid->SetValue(&this->rowData[this->fieldOfst[fieldIndex]]);
		return true;
	}
	return false;
}

Bool Map::ESRI::FileGDBReader::GetVariItem(UOSInt colIndex, NN<Data::VariItem> item)
{
	UOSInt fieldIndex = this->GetFieldIndex(colIndex);
	if (this->rowData.GetSize() == 0)
	{
		return false;
	}
	Map::ESRI::FileGDBFieldInfo *field = this->tableInfo->fields->GetItem(fieldIndex);
	if (field == 0)
	{
		return false;
	}
	if (this->fieldNull[fieldIndex])
	{
		item->SetNull();
		return true;
	}
	UOSInt ofst;
	UInt64 v;
	switch (field->fieldType)
	{
	case 0:
		item->SetI16(ReadInt16(&this->rowData[this->fieldOfst[fieldIndex]]));
		return true;
	case 1:
		item->SetI32(ReadInt32(&this->rowData[this->fieldOfst[fieldIndex]]));
		return true;
	case 2:
		item->SetF32(ReadFloat(&this->rowData[this->fieldOfst[fieldIndex]]));
		return true;
	case 3:
		item->SetF64(ReadDouble(&this->rowData[this->fieldOfst[fieldIndex]]));
		return true;
	case 12:
	case 4:
		ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, this->fieldOfst[fieldIndex], v);
		{
			NN<Text::String> s = Text::String::New(&this->rowData[ofst], (UOSInt)v);
			item->SetStr(s);
			s->Release();
			return true;
		}
	case 5:
		{
			item->SetDate(Text::XLSUtil::Number2Timestamp(ReadDouble(&this->rowData[this->fieldOfst[fieldIndex]])).RoundToS());
			return true;
		}
	case 6:
		item->SetI32(this->objectId);
		return true;
	case 7:
		{
			NN<Math::Geometry::Vector2D> vec;
			if (this->GetVector(colIndex).SetTo(vec))
			{
				item->SetVectorDirect(vec);
				return true;
			}
			item->SetNull();
			return true;
		}
	case 8:
		{
			ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, this->fieldOfst[fieldIndex], v);
			item->SetByteArr(&this->rowData[ofst], (UOSInt)v);
			return true;
		}
	case 10:
	case 11:
		{
			NN<Data::UUID> uuid;
			NEW_CLASSNN(uuid, Data::UUID(&this->rowData[this->fieldOfst[fieldIndex]]));
			item->SetUUIDDirect(uuid);
			return true;
		}
	}
	item->SetNull();
	return true;
}

NN<Data::VariItem> Map::ESRI::FileGDBReader::GetNewItem(Text::CStringNN name)
{
	UOSInt colIndex = INVALID_INDEX;
	UOSInt fieldIndex = INVALID_INDEX;
	UOSInt i;
	UOSInt j;
	FileGDBFieldInfo *field;
	if (this->columnIndices)
	{
		i = this->columnIndices->GetCount();
		while (i-- > 0)
		{
			j = this->columnIndices->GetItem(i);
			field = this->tableInfo->fields->GetItem(j);
			if (field && field->name->EqualsICase(name))
			{
				fieldIndex = j;
				colIndex = i;
				break;
			}
		}
	}
	else
	{
		i = this->tableInfo->fields->GetCount();
		while (i-- > 0)
		{
			field = this->tableInfo->fields->GetItem(i);
			if (field && field->name->EqualsICase(name))
			{
				fieldIndex = i;
				colIndex = i;
				break;
			}
		}
	}
	if (fieldIndex == INVALID_INDEX)
	{
		return Data::VariItem::NewUnknown();
	}
	if (this->fieldNull[fieldIndex])
		return Data::VariItem::NewNull();
	UOSInt ofst;
	UInt64 v;
	switch (field->fieldType)
	{
	case 0:
		return Data::VariItem::NewI16(ReadInt16(&this->rowData[this->fieldOfst[fieldIndex]]));
	case 1:
		return Data::VariItem::NewI32(ReadInt32(&this->rowData[this->fieldOfst[fieldIndex]]));
	case 2:
		return Data::VariItem::NewF32(ReadFloat(&this->rowData[this->fieldOfst[fieldIndex]]));
	case 3:
		return Data::VariItem::NewF64(ReadDouble(&this->rowData[this->fieldOfst[fieldIndex]]));
	case 12:
	case 4:
		ofst = Map::ESRI::FileGDBUtil::ReadVarUInt(this->rowData, this->fieldOfst[fieldIndex], v);
		{
			return Data::VariItem::NewStr(Text::CStringNN(&this->rowData[ofst], (UOSInt)v));
		}
	case 5:
		{
			return Data::VariItem::NewTS(Text::XLSUtil::Number2Timestamp(ReadDouble(&this->rowData[this->fieldOfst[fieldIndex]])).RoundToS());
		}
	case 6:
		return Data::VariItem::NewI32(this->objectId);
	case 7:
		{
			NN<Math::Geometry::Vector2D> vec;
			if (this->GetVector(colIndex).SetTo(vec))
			{
				NN<Data::VariItem> item = Data::VariItem::NewVector(vec);
				vec.Delete();
				return item;
			}
			return Data::VariItem::NewNull();
		}
	case 8:
		{
			UOSInt size = this->GetBinarySize(colIndex);
			UInt8 *binBuff = MemAlloc(UInt8, size);
			this->GetBinary(colIndex, binBuff);
			NN<Data::VariItem> item = Data::VariItem::NewByteArr(binBuff, size);
			MemFree(binBuff);
			return item;
		}
	case 10:
	case 11:
		{
			Data::UUID uuid(&this->rowData[this->fieldOfst[fieldIndex]]);
			return Data::VariItem::NewUUID(&uuid);
		}
	}
	return Data::VariItem::NewUnknown();
}

Bool Map::ESRI::FileGDBReader::IsNull(UOSInt colIndex)
{
	UOSInt fieldIndex = this->GetFieldIndex(colIndex);
	if (fieldIndex >= this->tableInfo->fields->GetCount())
	{
		return true;
	}
	return this->fieldNull[fieldIndex] != 0;
}

UnsafeArrayOpt<UTF8Char> Map::ESRI::FileGDBReader::GetName(UOSInt colIndex, UnsafeArray<UTF8Char> buff)
{
	UOSInt fieldIndex = this->GetFieldIndex(colIndex);
	Map::ESRI::FileGDBFieldInfo *field = this->tableInfo->fields->GetItem(fieldIndex);
	if (field)
	{
		return field->name->ConcatTo(buff);
	}
	return 0;
}

DB::DBUtil::ColType Map::ESRI::FileGDBReader::GetColType(UOSInt colIndex, OptOut<UOSInt> colSize)
{
	UOSInt fieldIndex = this->GetFieldIndex(colIndex);
	Map::ESRI::FileGDBFieldInfo *field = this->tableInfo->fields->GetItem(fieldIndex);
	if (field)
	{
		colSize.Set(field->fieldSize);
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
			return DB::DBUtil::CT_VarUTF8Char;
		case 5:
			colSize.Set(0);
			return DB::DBUtil::CT_DateTime;
		case 6:
			return DB::DBUtil::CT_Int32;
		case 7:
			return DB::DBUtil::CT_Vector;
		case 8:
			if (field->fieldSize == 0)
				colSize.Set(1048576);
			return DB::DBUtil::CT_Binary;
		case 9:
			if (field->fieldSize == 0)
				colSize.Set(1048576);
			return DB::DBUtil::CT_Binary;
		case 10:
			return DB::DBUtil::CT_UUID;
		case 11:
			return DB::DBUtil::CT_UUID;
		case 12:
			return DB::DBUtil::CT_VarUTF8Char;
		default:
			return DB::DBUtil::CT_Unknown;
		}
	}
	return DB::DBUtil::CT_Unknown;
}

Bool Map::ESRI::FileGDBReader::GetColDef(UOSInt colIndex, NN<DB::ColDef> colDef)
{
	UOSInt fieldIndex = this->GetFieldIndex(colIndex);
	Map::ESRI::FileGDBFieldInfo *field = this->tableInfo->fields->GetItem(fieldIndex);
	if (field == 0)
	{
		return false;
	}
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<Math::CoordinateSystem> csys;
	UOSInt colSize;
	colDef->SetColName(field->name);
	colDef->SetColSize(field->fieldSize);
	colDef->SetColType(this->GetColType(colIndex, colSize));
	if (colDef->GetColType() == DB::DBUtil::CT_DateTime)
	{
		colDef->SetColSize(0);
	}
	else if (colDef->GetColType() == DB::DBUtil::CT_Binary)
	{
		if (field->fieldSize == 0)
		{
			colDef->SetColSize(1048576);			
		}		
	}
	colDef->SetNotNull((field->flags & 1) == 0);
	colDef->SetPK(field->fieldType == 6);
	if (field->fieldType == 6)
	{
		Int64 startIndex = 1;
		if (this->indexBuff.GetCount() > 0)
		{
			startIndex = (Int64)this->indexCnt + 1;
		}
		colDef->SetAutoInc(DB::ColDef::AutoIncType::Default, startIndex, 1);
	}
	else
	{
		colDef->SetAutoIncNone();
	}
	if (field->srsValue)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendUTF16((const UTF16Char*)field->srsValue, field->srsSize >> 1);
		colDef->SetAttr(sb.ToCString());
	}
	if (colDef->GetColType() == DB::DBUtil::CT_Vector)
	{
		if (this->tableInfo->csys.SetTo(csys))
		{
			colDef->SetColDP(csys->GetSRID());
		}
		switch (this->tableInfo->geometryType)
		{
		default:
			printf("FileGDBReader: geometryType not supported: %d\r\n", this->tableInfo->geometryType);
		case 0:
		case 7:
			if ((this->tableInfo->geometryFlags & 0xC0) == 0x80)
				colDef->SetColSize((UOSInt)DB::ColDef::GeometryType::AnyZ);
			else if ((this->tableInfo->geometryFlags & 0xC0) == 0xC0)
				colDef->SetColSize((UOSInt)DB::ColDef::GeometryType::AnyZM);
			else if ((this->tableInfo->geometryFlags & 0xC0) == 0x40)
				colDef->SetColSize((UOSInt)DB::ColDef::GeometryType::AnyM);
			else
				colDef->SetColSize((UOSInt)DB::ColDef::GeometryType::Any);
			break;
		case 1:
			if ((this->tableInfo->geometryFlags & 0xC0) == 0x80)
				colDef->SetColSize((UOSInt)DB::ColDef::GeometryType::PointZ);
			else if ((this->tableInfo->geometryFlags & 0xC0) == 0xC0)
				colDef->SetColSize((UOSInt)DB::ColDef::GeometryType::PointZM);
			else if ((this->tableInfo->geometryFlags & 0xC0) == 0x40)
				colDef->SetColSize((UOSInt)DB::ColDef::GeometryType::PointM);
			else
				colDef->SetColSize((UOSInt)DB::ColDef::GeometryType::Point);
			break;
		case 2:
			if ((this->tableInfo->geometryFlags & 0xC0) == 0x80)
				colDef->SetColSize((UOSInt)DB::ColDef::GeometryType::MultipointZ);
			else if ((this->tableInfo->geometryFlags & 0xC0) == 0xC0)
				colDef->SetColSize((UOSInt)DB::ColDef::GeometryType::MultipointZM);
			else if ((this->tableInfo->geometryFlags & 0xC0) == 0x40)
				colDef->SetColSize((UOSInt)DB::ColDef::GeometryType::MultipointM);
			else
				colDef->SetColSize((UOSInt)DB::ColDef::GeometryType::Multipoint);
			break;
		case 3:
			if ((this->tableInfo->geometryFlags & 0xC0) == 0x80)
				colDef->SetColSize((UOSInt)DB::ColDef::GeometryType::PolylineZ);
			else if ((this->tableInfo->geometryFlags & 0xC0) == 0xC0)
				colDef->SetColSize((UOSInt)DB::ColDef::GeometryType::PolylineZM);
			else if ((this->tableInfo->geometryFlags & 0xC0) == 0x40)
				colDef->SetColSize((UOSInt)DB::ColDef::GeometryType::PolylineM);
			else
				colDef->SetColSize((UOSInt)DB::ColDef::GeometryType::Polyline);
			break;
		case 4:
			if ((this->tableInfo->geometryFlags & 0xC0) == 0x80)
				colDef->SetColSize((UOSInt)DB::ColDef::GeometryType::PolygonZ);
			else if ((this->tableInfo->geometryFlags & 0xC0) == 0xC0)
				colDef->SetColSize((UOSInt)DB::ColDef::GeometryType::PolygonZM);
			else if ((this->tableInfo->geometryFlags & 0xC0) == 0x40)
				colDef->SetColSize((UOSInt)DB::ColDef::GeometryType::PolygonM);
			else
				colDef->SetColSize((UOSInt)DB::ColDef::GeometryType::Polygon);
			break;
		case 5:
			if ((this->tableInfo->geometryFlags & 0xC0) == 0x80)
				colDef->SetColSize((UOSInt)DB::ColDef::GeometryType::RectangleZ);
			else if ((this->tableInfo->geometryFlags & 0xC0) == 0xC0)
				colDef->SetColSize((UOSInt)DB::ColDef::GeometryType::RectangleZM);
			else if ((this->tableInfo->geometryFlags & 0xC0) == 0x40)
				colDef->SetColSize((UOSInt)DB::ColDef::GeometryType::RectangleM);
			else
				colDef->SetColSize((UOSInt)DB::ColDef::GeometryType::Rectangle);
			break;
		case 6:
			if ((this->tableInfo->geometryFlags & 0xC0) == 0x80)
				colDef->SetColSize((UOSInt)DB::ColDef::GeometryType::PathZ);
			else if ((this->tableInfo->geometryFlags & 0xC0) == 0xC0)
				colDef->SetColSize((UOSInt)DB::ColDef::GeometryType::PathZM);
			else if ((this->tableInfo->geometryFlags & 0xC0) == 0x40)
				colDef->SetColSize((UOSInt)DB::ColDef::GeometryType::PathM);
			else
				colDef->SetColSize((UOSInt)DB::ColDef::GeometryType::Path);
			break;
		}
	}
	if (field->defValue)
	{
		if (field->fieldType == 0)
		{
			if (field->defSize == 2)
			{
				sptr = Text::StrInt16(sbuff, ReadInt16(field->defValue));
				colDef->SetDefVal(CSTRP(sbuff, sptr));
			}
		}
		else if (field->fieldType == 1)
		{
			if (field->defSize == 4)
			{
				sptr = Text::StrInt32(sbuff, ReadInt32(field->defValue));
				colDef->SetDefVal(CSTRP(sbuff, sptr));
			}
		}
		else if (field->fieldType == 2)
		{
			if (field->defSize == 4)
			{
				sptr = Text::StrDouble(sbuff, ReadFloat(field->defValue));
				colDef->SetDefVal(CSTRP(sbuff, sptr));
			}
		}
		else if (field->fieldType == 3)
		{
			if (field->defSize == 8)
			{
				sptr = Text::StrDouble(sbuff, ReadDouble(field->defValue));
				colDef->SetDefVal(CSTRP(sbuff, sptr));
			}
		}
		else if (field->fieldType == 4)
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(field->defValue, field->defSize);
			colDef->SetDefVal(sb.ToCString());
		}
		else if (field->fieldType == 5)
		{
			if (field->defSize == 8)
			{
				Data::DateTime dt;
				Text::XLSUtil::Number2Date(dt, ReadDouble(field->defValue));
				sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
				colDef->SetDefVal(CSTRP(sbuff, sptr));
			}
		}
	}
	return true;
}

UInt64 Map::ESRI::FileGDBReader::GetRowFileOfst() const
{
	return this->rowOfst;
}

void Map::ESRI::FileGDBReader::SetIndex(NN<IO::StreamData> fd, UOSInt indexCnt)
{
	if (this->indexBuff.GetSize() == 0)
	{
		this->indexCnt = indexCnt;
		UOSInt len = this->indexCnt * 5;
		this->indexBuff.ChangeSizeAndClear(len);
		if (fd->GetRealData(16, len, this->indexBuff) != len)
		{
			this->indexBuff.Delete();
		}
	}
}
