#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListInt32.h"
#include "Data/ByteTool.h"
#include "DB/ColDef.h"
#include "IO/BufferedInputStream.h"
#include "IO/FileStream.h"
#include "Map/MapDrawLayer.h"
#include "Map/SPDLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math.h"
#include "Math/Geometry/Point.h"
#include "Math/Geometry/Polygon.h"
#include "Math/Geometry/Polyline.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

Map::SPDLayer::SPDLayer(Text::CStringNN layerName) : Map::MapDrawLayer(layerName, 0, nullptr, Math::CoordinateSystemManager::CreateWGS84Csys())
{
	UTF8Char fname[256];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	IO::BufferedInputStream *bstm;
	sptr = layerName.ConcatTo(fname);
	if (Text::StrEqualsICaseC(&sptr[-4], 4, UTF8STRC(".SPD")))
	{
		sptr = &sptr[-4];
		*sptr = 0;
	}
	UOSInt i;

	this->maxId = -1;
	this->ofsts = 0;
	this->nblks = 0;
	this->blkScale = 0;
	this->blks = 0;
	this->maxTextSize = 0;
	this->lyrType = (Map::DrawLayerType)0;
	this->layerName = Text::StrCopyNew(fname).Ptr();

	sptr2 = Text::StrConcatC(sptr, UTF8STRC(".spb"));
	{
		IO::FileStream file({fname, (UOSInt)(sptr2 - fname)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		NEW_CLASS(bstm, IO::BufferedInputStream(file, 65536));
		if (!file.IsError())
		{
			bstm->Read(Data::ByteArray((UInt8*)&this->nblks, 4));
			bstm->Read(Data::ByteArray((UInt8*)&this->blkScale, 4));
			this->blks = MemAlloc(SPDBlock, this->nblks);
			i = 0;
			while (i < this->nblks)
			{
				bstm->Read(Data::ByteArray((UInt8*)&this->blks[i], 12));
				this->blks[i].ids = MemAlloc(Int32, this->blks[i].objCnt);
				bstm->Read(Data::ByteArray((UInt8*)this->blks[i].ids, this->blks[i].objCnt << 2));
				i++;
			}
		}
		else
		{
			i = (UOSInt)file.GetErrCode();
		}
		DEL_CLASS(bstm);
	}

	sptr2 = Text::StrConcatC(sptr, UTF8STRC(".spi"));
	{
		IO::FileStream file({fname, (UOSInt)(sptr2 - fname)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential);
		if (!file.IsError())
		{
			i = (UOSInt)file.GetLength();
			this->ofsts = (UInt32*)MAlloc(i);
			file.Read(Data::ByteArray((UInt8*)this->ofsts, i));
			this->maxId = (OSInt)(i / 8) - 2;
		}
	}

	missFile = false;
	if (!IsError())
	{
		sptr2 = Text::StrConcatC(sptr, UTF8STRC(".spd"));
		{
			IO::FileStream file({fname, (UOSInt)(sptr2 - fname)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential);
			if (!file.IsError())
			{
				file.Read(Data::ByteArray((UInt8*)&i, 4));
				file.Read(Data::ByteArray((UInt8*)&this->lyrType, 4));
			}
			else
			{
				missFile = true;
			}
		}

		sptr2 = Text::StrConcatC(sptr, UTF8STRC(".sps"));
		{
			IO::FileStream file({fname, (UOSInt)(sptr2 - fname)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential);
			NEW_CLASS(bstm, IO::BufferedInputStream(file, 65536));
			if (!file.IsError())
			{
				UInt32 buff[4];
				bstm->Read(Data::ByteArray((UInt8*)buff, 8));
				i = 0;
				while (i < this->nblks)
				{
					bstm->Read(Data::ByteArray((UInt8*)buff, 16));
					this->blks[i].sofst = buff[3];
					i++;
				}
			}
			else
			{
				missFile = true;
			}
			DEL_CLASS(bstm);
		}
	}
}

Map::SPDLayer::~SPDLayer()
{
	UInt32 i;
/*	if (cip)
	{
		DEL_CLASS(cip);
		cip = 0;
	}*/
	if (ofsts)
	{
		MemFree(ofsts);
		ofsts = 0;
	}
	if (blks)
	{
		i = this->nblks;
		while (i-- > 0)
		{
			MemFree(this->blks[i].ids);
		}
		MemFree(blks);
		blks = 0;
	}
	if (this->layerName)
	{
		Text::StrDelNew(this->layerName);
		this->layerName = 0;
	}
}

Bool Map::SPDLayer::IsError() const
{
	if (ofsts == 0)
		return true;
	if (blks == 0)
		return true;
	if (missFile)
		return true;
	return false;
}

Map::DrawLayerType Map::SPDLayer::GetLayerType() const
{
	return lyrType;
}

UOSInt Map::SPDLayer::GetAllObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr)
{
	UOSInt textSize;
	UOSInt i;
	UOSInt k;
	UOSInt l;

	k = 0;
	l = 0;
	if (nameArr.IsNotNull())
	{
		Data::ArrayList<UTF16Char *> *tmpArr;
		NEW_CLASS(tmpArr, Data::ArrayList<UTF16Char *>());
		nameArr.SetNoCheck((NameArray*)tmpArr);
		UTF8Char fileName[256];
		UnsafeArray<UTF8Char> sptr;
		IO::FileStream *cis;
		sptr = Text::StrConcat(fileName, this->layerName);
		sptr = Text::StrConcatC(sptr, UTF8STRC(".sps"));
		NEW_CLASS(cis, IO::FileStream({fileName, (UOSInt)(sptr - fileName)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		
		while (k < this->nblks)
		{
			UTF16Char *strTmp;
			UInt8 buff[13];
			cis->SeekFromBeginning(this->blks[k].sofst);
			i = this->blks[k].objCnt;
			while (i-- > 0)
			{
				cis->Read(Data::ByteArray(buff, 13));
				if (buff[4])
				{
					strTmp = MemAlloc(UTF16Char, (UOSInt)(buff[12] >> 1) + 1);
					cis->Read(Data::ByteArray((UInt8*)strTmp, buff[12]));
					strTmp[buff[12] >> 1] = 0;
					outArr->Add(*(Int32*)buff);
					tmpArr->Add(strTmp);
					cis->SeekFromCurrent(ReadInt32(&buff[4]) - 13 - buff[12]);
					textSize = Text::StrUTF16_UTF8Cnt(strTmp) - 1;
					if (textSize > this->maxTextSize)
						maxTextSize = textSize;
				}
				else
				{
					strTmp = MemAlloc(UTF16Char, 1);
					strTmp[0] = 0;
					outArr->Add(*(Int32*)buff);
					tmpArr->Add(strTmp);
				}
			}
			k++;
		}
		DEL_CLASS(cis);
	}
	else
	{
		while (k < this->nblks)
		{
			outArr->AddRangeI32(this->blks[k].ids, this->blks[k].objCnt);
			l += this->blks[k].objCnt;

			k++;
		}
	}
	return l;
}

UOSInt Map::SPDLayer::GetObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	rect.min.x = Double2Int32(rect.min.x * 200000.0 / mapRate);
	rect.min.y = Double2Int32(rect.min.y * 200000.0 / mapRate);
	rect.max.x = Double2Int32(rect.max.x * 200000.0 / mapRate);
	rect.max.y = Double2Int32(rect.max.y * 200000.0 / mapRate);
	rect = rect.Reorder();
	Int32 leftBlk;
	Int32 rightBlk;
	Int32 topBlk;
	Int32 bottomBlk;
//	this->mut->Lock();
	leftBlk = rect.min.x / blkScale;
	rightBlk = rect.max.x / blkScale;
	topBlk = rect.min.y / blkScale;
	bottomBlk = rect.max.y / blkScale;

	UOSInt textSize;
	Int32 i;
	Int32 j;
	Int32 k;
	UOSInt l;
	if (this->nblks > 10)
	{
		i = 0;
		j = (Int32)this->nblks - 1;
		while (i <= j)
		{
			k = (i + j) >> 1;
			if (this->blks[k].blk.x < leftBlk)
			{
				i = k + 1;
			}
			else if (this->blks[k].blk.x > leftBlk)
			{
				j = k - 1;
			}
			else if (this->blks[k].blk.y < topBlk)
			{
				i = k + 1;
			}
			else if (this->blks[k].blk.y > topBlk)
			{
				j = k - 1;
			}
			else
			{
				j = k;
				break;
			}
		}
	}
	else
	{
		j = 0;
	}

	if (j < 0)
		j = 0;

	k = j;
	l = 0;
	if (nameArr.IsNotNull())
	{
		Data::ArrayList<UTF16Char *> *tmpArr;
		NEW_CLASS(tmpArr, Data::ArrayList<UTF16Char *>());
		nameArr.SetNoCheck((NameArray*)tmpArr);
		UTF8Char fileName[256];
		UnsafeArray<UTF8Char> sptr;
		IO::FileStream *cis;
		sptr = Text::StrConcat(fileName, this->layerName);
		sptr = Text::StrConcatC(sptr, UTF8STRC(".sps"));
		NEW_CLASS(cis, IO::FileStream({fileName, (UOSInt)(sptr - fileName)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		
		while ((UOSInt)k < this->nblks)
		{
			if (this->blks[k].blk.x > rightBlk)
				break;


			if ((this->blks[k].blk.y >= topBlk) && (this->blks[k].blk.y <= bottomBlk) && (this->blks[k].blk.x >= leftBlk))
			{
				UTF16Char *strTmp;
				UInt8 buff[13];
				cis->SeekFromBeginning(this->blks[k].sofst);
				i = (Int32)this->blks[k].objCnt;
				while (i-- > 0)
				{
					cis->Read(Data::ByteArray(buff, 13));
					if (buff[4])
					{
						strTmp = MemAlloc(UTF16Char, (UOSInt)(buff[12] >> 1) + 1);
						cis->Read(Data::ByteArray((UInt8*)strTmp, buff[12]));
						strTmp[buff[12] >> 1] = 0;
						outArr->Add(*(Int32*)buff);
						tmpArr->Add(strTmp);
						cis->SeekFromCurrent(ReadInt32(&buff[4]) - 13 - buff[12]);
						textSize = Text::StrUTF16_UTF8Cnt(strTmp);
						if (textSize > this->maxTextSize)
						{
							this->maxTextSize = textSize;
						}
					}
					else if (keepEmpty)
					{
						strTmp = MemAlloc(UTF16Char, 1);
						strTmp[0] = 0;
						outArr->Add(*(Int32*)buff);
						tmpArr->Add(strTmp);
					}
				}
			}
			k++;
		}
		DEL_CLASS(cis);
	}
	else
	{
		while ((UOSInt)k < this->nblks)
		{
			if (this->blks[k].blk.x > rightBlk)
				break;


			if ((this->blks[k].blk.y >= topBlk) && (this->blks[k].blk.y <= bottomBlk) && (this->blks[k].blk.x >= leftBlk))
			{
				outArr->AddRangeI32(this->blks[k].ids, this->blks[k].objCnt);
				l += this->blks[k].objCnt;
			}
			k++;
		}
	}
//	this->mut->Unlock();
	return l;
}

UOSInt Map::SPDLayer::GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	rect = rect * 200000;
	return GetObjectIds(outArr, nameArr, 200000.0, Math::RectArea<Int32>(Math::Coord2D<Int32>(Double2Int32(rect.min.x), Double2Int32(rect.min.y)),
		 Math::Coord2D<Int32>(Double2Int32(rect.max.x), Double2Int32(rect.max.y))), keepEmpty);
}

Int64 Map::SPDLayer::GetObjectIdMax() const
{
	return this->maxId;
}

void Map::SPDLayer::ReleaseNameArr(Optional<NameArray> nameArr)
{
	NN<Data::ArrayList<UTF16Char *>> tmpArr;
	if (Optional<Data::ArrayList<UTF16Char*>>::ConvertFrom(nameArr).SetTo(tmpArr))
	{
		UOSInt i = tmpArr->GetCount();
		while (i-- > 0)
		{
			MemFree(tmpArr->RemoveAt(i));
		}
		tmpArr.Delete();
	}
}

Bool Map::SPDLayer::GetString(NN<Text::StringBuilderUTF8> sb, Optional<NameArray> nameArr, Int64 id, UOSInt strIndex)
{
	NN<Data::ArrayList<UTF16Char*>> tmpArr;
	if (!Optional<Data::ArrayList<UTF16Char*>>::ConvertFrom(nameArr).SetTo(tmpArr) || strIndex != 0)
	{
		return false;
	}
	UTF16Char *s = tmpArr->GetItem((UOSInt)id);
	if (s)
	{
		sb->AppendUTF16(s);
		return true;
	}
	else
	{
		return false;
	}
}

UOSInt Map::SPDLayer::GetColumnCnt() const
{
	return 1;
}

UnsafeArrayOpt<UTF8Char> Map::SPDLayer::GetColumnName(UnsafeArray<UTF8Char> buff, UOSInt colIndex)
{
	if (colIndex == 0)
	{
		return Text::StrConcatC(buff, UTF8STRC("NAME"));
	}
	else
	{
		return 0;
	}
}

DB::DBUtil::ColType Map::SPDLayer::GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize)
{
	if (colIndex == 0)
	{
		colSize.Set(this->maxTextSize);
		return DB::DBUtil::CT_VarUTF8Char;
	}
	else
	{
		colSize.Set(0);
		return DB::DBUtil::CT_Unknown;
	}
}

Bool Map::SPDLayer::GetColumnDef(UOSInt colIndex, NN<DB::ColDef> colDef)
{
	if (colIndex != 0)
		return false;
	colDef->SetColName(CSTR("NAME"));
	colDef->SetColSize(this->maxTextSize);
	colDef->SetColDP(0);
	colDef->SetColType(DB::DBUtil::CT_VarUTF8Char);
	colDef->SetDefVal(Text::CString(nullptr));
	colDef->SetNotNull(false);
	colDef->SetPK(false);
	colDef->SetAutoInc(DB::ColDef::AutoIncType::None, 1, 1);
	colDef->SetAttr(Text::CString(nullptr));
	return true;
}

Int32 Map::SPDLayer::GetBlockSize() const
{
	return this->blkScale;
}

UInt32 Map::SPDLayer::GetCodePage() const
{
	return 0;
}

Bool Map::SPDLayer::GetBounds(OutParam<Math::RectAreaDbl> bounds) const
{
	if (this->nblks == 0)
	{
		bounds.Set(Math::RectAreaDbl(0, 0, 0, 0));
		return false;
	}
	else
	{
		Math::Coord2D<Int32> minBlk;
		Math::Coord2D<Int32> maxBlk;
		maxBlk = minBlk = this->blks[0].blk;
		UOSInt i = this->nblks;
		while (i-- > 0)
		{
			if (this->blks[i].blk.x > maxBlk.x)
			{
				maxBlk.x = this->blks[i].blk.x;
			}
			if (this->blks[i].blk.x < minBlk.x)
			{
				minBlk.x = this->blks[i].blk.x;
			}
			if (this->blks[i].blk.y > maxBlk.y)
			{
				maxBlk.y = this->blks[i].blk.y;
			}
			if (this->blks[i].blk.y < minBlk.y)
			{
				minBlk.y = this->blks[i].blk.y;
			}
		}
		bounds.Set(Math::RectAreaDbl(minBlk.ToDouble(), maxBlk.ToDouble() + 1) * (this->blkScale / 200000.0));
		return true;
	}
}

NN<Map::GetObjectSess> Map::SPDLayer::BeginGetObject()
{
	UTF8Char fileName[256];
	UnsafeArray<UTF8Char> sptr;
	NN<IO::FileStream> cip;
//	this->mut->Lock();
	sptr = Text::StrConcat(fileName, this->layerName);
	sptr = Text::StrConcatC(sptr, UTF8STRC(".spd"));
	NEW_CLASSNN(cip, IO::FileStream({fileName, (UOSInt)(sptr - fileName)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	return NN<Map::GetObjectSess>::ConvertFrom(cip);
}

void Map::SPDLayer::EndGetObject(NN<Map::GetObjectSess> session)
{
	NN<IO::FileStream> cip = NN<IO::FileStream>::ConvertFrom(session);
	cip.Delete();
//	this->mut->Unlock();
}

Optional<Math::Geometry::Vector2D> Map::SPDLayer::GetNewVectorById(NN<Map::GetObjectSess> session, Int64 id)
{
	Int32 buff[3];
	NN<IO::FileStream> cip = NN<IO::FileStream>::ConvertFrom(session);
	UInt32 ofst = this->ofsts[2 + (id << 1)];
	UInt32 *ptOfsts;
	Int32 *points;
	UnsafeArray<Math::Coord2DDbl> tmpPoints;

	cip->SeekFromBeginning(ofst);
	cip->Read(Data::ByteArray((UInt8*)buff, 8));
	
	if (buff[1] > 0)
	{
		ptOfsts = MemAlloc(UInt32, (UInt32)buff[1]);
		cip->Read(Data::ByteArray((UInt8*)ptOfsts, sizeof(UInt32) * (UInt32)buff[1]));
	}
	else
	{
		ptOfsts = 0;
	}
	cip->Read(Data::ByteArray((UInt8*)&buff[2], 4));
	points = MemAlloc(Int32, (UOSInt)buff[2] * 2);
	cip->Read(Data::ByteArray((UInt8*)points, (UOSInt)buff[2] * 8));

	if (this->lyrType == Map::DRAW_LAYER_POINT)
	{
		Math::Geometry::Point *pt;
		NEW_CLASS(pt, Math::Geometry::Point(4326, points[0] / 200000.0, points[1] / 200000.0));
		if (ptOfsts)
		{
			MemFree(ptOfsts);
		}
		MemFree(points);
		return pt;
	}
	else if (ptOfsts == 0)
	{
	}
	else if (this->lyrType == Map::DRAW_LAYER_POLYLINE)
	{
		Math::Geometry::Polyline *pl;
		NN<Math::Geometry::LineString> lineString;
		NEW_CLASS(pl, Math::Geometry::Polyline(4326));
		UOSInt i = 0;
		UOSInt j;
		UOSInt k;
		UOSInt l;
		while (i < (UInt32)buff[1])
		{
			j = ptOfsts[i];
			if (i + 1 >= (UInt32)buff[1])
				k = (UInt32)buff[2];
			else
				k = ptOfsts[i + 1];
			NEW_CLASSNN(lineString, Math::Geometry::LineString(4326, (k - j), false, false));
			tmpPoints = lineString->GetPointList(l);
			l = 0;
			while (j < k)
			{
				tmpPoints[l].x = points[(j << 1)] / 200000.0;
				tmpPoints[l].y = points[(j << 1) + 1] / 200000.0;
				j++;
				l++;
			}
			pl->AddGeometry(lineString);
			i++;
		}

		MemFree(ptOfsts);
		MemFree(points);
		return pl;
	}
	else if (this->lyrType == Map::DRAW_LAYER_POLYGON)
	{
		Math::Geometry::Polygon *pg;
		NN<Math::Geometry::LinearRing> lr;
		NEW_CLASS(pg, Math::Geometry::Polygon(4326));
		UOSInt i = 0;
		UOSInt j;
		UOSInt k;
		UOSInt l;
		while (i < (UInt32)buff[1])
		{
			j = ptOfsts[i];
			if (i + 1 >= (UInt32)buff[1])
				k = (UInt32)buff[2];
			else
				k = ptOfsts[i + 1];
			NEW_CLASSNN(lr, Math::Geometry::LinearRing(4326, (k - j), false, false));
			tmpPoints = lr->GetPointList(l);
			l = 0;
			while (j < k)
			{
				tmpPoints[l].x = points[(j << 1)] / 200000.0;
				tmpPoints[l].y = points[(j << 1) + 1] / 200000.0;
				j++;
				l++;
			}
			pg->AddGeometry(lr);
			i++;
		}

		MemFree(ptOfsts);
		MemFree(points);
		return pg;
	}

	if (ptOfsts)
	{
		MemFree(ptOfsts);
	}
	MemFree(points);
	return 0;
}

UOSInt Map::SPDLayer::GetGeomCol() const
{
	return INVALID_INDEX;
}

Map::MapDrawLayer::ObjectClass Map::SPDLayer::GetObjectClass() const
{
	return Map::MapDrawLayer::OC_SPD_LAYER;
}
