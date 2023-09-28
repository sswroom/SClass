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

Map::SPDLayer::SPDLayer(Text::CStringNN layerName) : Map::MapDrawLayer(layerName, 0, CSTR_NULL, Math::CoordinateSystemManager::CreateDefaultCsys())
{
	UTF8Char fname[256];
	UTF8Char *sptr;
	UTF8Char *sptr2;
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

UOSInt Map::SPDLayer::GetAllObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr)
{
	UOSInt textSize;
	UOSInt i;
	UOSInt k;
	UOSInt l;

	k = 0;
	l = 0;
	if (nameArr)
	{
		Data::ArrayList<WChar *> *tmpArr;
		NEW_CLASS(tmpArr, Data::ArrayList<WChar *>());
		*nameArr = (NameArray*)tmpArr;
		UTF8Char fileName[256];
		UTF8Char *sptr;
		IO::FileStream *cis;
		sptr = Text::StrConcat(fileName, this->layerName);
		sptr = Text::StrConcatC(sptr, UTF8STRC(".sps"));
		NEW_CLASS(cis, IO::FileStream({fileName, (UOSInt)(sptr - fileName)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		
		while (k < this->nblks)
		{
			WChar *strTmp;
			UInt8 buff[13];
			cis->SeekFromBeginning(this->blks[k].sofst);
			i = this->blks[k].objCnt;
			while (i-- > 0)
			{
				cis->Read(Data::ByteArray(buff, 13));
				if (buff[4])
				{
					strTmp = MemAlloc(WChar, (UOSInt)(buff[12] >> 1) + 1);
					cis->Read(Data::ByteArray((UInt8*)strTmp, buff[12]));
					strTmp[buff[12] >> 1] = 0;
					outArr->Add(*(Int32*)buff);
					tmpArr->Add(strTmp);
					cis->SeekFromCurrent(ReadInt32(&buff[4]) - 13 - buff[12]);
					textSize = Text::StrWChar_UTF8Cnt(strTmp) - 1;
					if (textSize > this->maxTextSize)
						maxTextSize = textSize;
				}
				else
				{
					strTmp = MemAlloc(WChar, 1);
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

UOSInt Map::SPDLayer::GetObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	rect.tl.x = Double2Int32(rect.tl.x * 200000.0 / mapRate);
	rect.tl.y = Double2Int32(rect.tl.y * 200000.0 / mapRate);
	rect.br.x = Double2Int32(rect.br.x * 200000.0 / mapRate);
	rect.br.y = Double2Int32(rect.br.y * 200000.0 / mapRate);
	rect = rect.Reorder();
	Int32 leftBlk;
	Int32 rightBlk;
	Int32 topBlk;
	Int32 bottomBlk;
//	this->mut->Lock();
	leftBlk = rect.tl.x / blkScale;
	rightBlk = rect.br.x / blkScale;
	topBlk = rect.tl.y / blkScale;
	bottomBlk = rect.br.y / blkScale;

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
	if (nameArr)
	{
		Data::ArrayList<WChar *> *tmpArr;
		NEW_CLASS(tmpArr, Data::ArrayList<WChar *>());
		*nameArr = (NameArray*)tmpArr;
		UTF8Char fileName[256];
		UTF8Char *sptr;
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
				WChar *strTmp;
				UInt8 buff[13];
				cis->SeekFromBeginning(this->blks[k].sofst);
				i = (Int32)this->blks[k].objCnt;
				while (i-- > 0)
				{
					cis->Read(Data::ByteArray(buff, 13));
					if (buff[4])
					{
						strTmp = MemAlloc(WChar, (UOSInt)(buff[12] >> 1) + 1);
						cis->Read(Data::ByteArray((UInt8*)strTmp, buff[12]));
						strTmp[buff[12] >> 1] = 0;
						outArr->Add(*(Int32*)buff);
						tmpArr->Add(strTmp);
						cis->SeekFromCurrent(ReadInt32(&buff[4]) - 13 - buff[12]);
						textSize = Text::StrWChar_UTF8Cnt(strTmp);
						if (textSize > this->maxTextSize)
						{
							this->maxTextSize = textSize;
						}
					}
					else if (keepEmpty)
					{
						strTmp = MemAlloc(WChar, 1);
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

UOSInt Map::SPDLayer::GetObjectIdsMapXY(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	rect = rect * 200000;
	return GetObjectIds(outArr, nameArr, 200000.0, Math::RectArea<Int32>(Math::Coord2D<Int32>(Double2Int32(rect.tl.x), Double2Int32(rect.tl.y)),
		 Math::Coord2D<Int32>(Double2Int32(rect.br.x), Double2Int32(rect.br.y))), keepEmpty);
}

Int64 Map::SPDLayer::GetObjectIdMax() const
{
	return this->maxId;
}

void Map::SPDLayer::ReleaseNameArr(NameArray *nameArr)
{
	Data::ArrayList<WChar *>*tmpArr = (Data::ArrayList<WChar*>*)nameArr;
	UOSInt i = tmpArr->GetCount();
	while (i-- > 0)
	{
		MemFree(tmpArr->RemoveAt(i));
	}
	DEL_CLASS(tmpArr);
}

UTF8Char *Map::SPDLayer::GetString(UTF8Char *buff, UOSInt buffSize, NameArray *nameArr, Int64 id, UOSInt strIndex)
{
	Data::ArrayList<WChar*> *tmpArr = (Data::ArrayList<WChar*>*)nameArr;
	if (strIndex != 0)
	{
		*buff = 0;
		return 0;
	}
	WChar *s = tmpArr->GetItem((UOSInt)id);
	if (s)
	{
		return Text::StrWChar_UTF8(buff, s);
	}
	else
	{
		*buff = 0;
		return 0;
	}
}

UOSInt Map::SPDLayer::GetColumnCnt() const
{
	return 1;
}

UTF8Char *Map::SPDLayer::GetColumnName(UTF8Char *buff, UOSInt colIndex)
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

DB::DBUtil::ColType Map::SPDLayer::GetColumnType(UOSInt colIndex, UOSInt *colSize)
{
	if (colIndex == 0)
	{
		if (colSize)
		{
			*colSize = this->maxTextSize;
		}
		return DB::DBUtil::CT_VarUTF8Char;
	}
	else
	{
		if (colSize)
		{
			*colSize = 0;
		}
		return DB::DBUtil::CT_Unknown;
	}
}

Bool Map::SPDLayer::GetColumnDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef)
{
	if (colIndex != 0)
		return false;
	colDef->SetColName(CSTR("NAME"));
	colDef->SetColSize(this->maxTextSize);
	colDef->SetColDP(0);
	colDef->SetColType(DB::DBUtil::CT_VarUTF8Char);
	colDef->SetDefVal(CSTR_NULL);
	colDef->SetNotNull(false);
	colDef->SetPK(false);
	colDef->SetAutoInc(DB::ColDef::AutoIncType::None, 1, 1);
	colDef->SetAttr(CSTR_NULL);
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

Map::GetObjectSess *Map::SPDLayer::BeginGetObject()
{
	UTF8Char fileName[256];
	UTF8Char *sptr;
	IO::FileStream *cip;
//	this->mut->Lock();
	sptr = Text::StrConcat(fileName, this->layerName);
	sptr = Text::StrConcatC(sptr, UTF8STRC(".spd"));
	NEW_CLASS(cip, IO::FileStream({fileName, (UOSInt)(sptr - fileName)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	return (Map::GetObjectSess*)cip;
}

void Map::SPDLayer::EndGetObject(Map::GetObjectSess *session)
{
	IO::FileStream *cip = (IO::FileStream*)session;
	DEL_CLASS(cip);
//	this->mut->Unlock();
}

Math::Geometry::Vector2D *Map::SPDLayer::GetNewVectorById(Map::GetObjectSess *session, Int64 id)
{
	Int32 buff[3];

	IO::FileStream *cip = (IO::FileStream*)session;
	UInt32 ofst = this->ofsts[2 + (id << 1)];
	Math::Geometry::PointOfstCollection *ptColl = 0;
	UInt32 *ptOfsts;
	Int32 *points;
	UOSInt i;
	UInt32 *tmpPtOfsts;
	Math::Coord2DDbl *tmpPoints;

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
		return ptColl;
	}
	else if (ptOfsts == 0)
	{
	}
	else if (this->lyrType == Map::DRAW_LAYER_POLYLINE)
	{
		NEW_CLASS(ptColl, Math::Geometry::Polyline(4326, (UInt32)buff[1], (UInt32)buff[2], false, false));
		tmpPtOfsts = ptColl->GetPtOfstList(i);
		MemCopyNO(tmpPtOfsts, ptOfsts, (UInt32)buff[1] << 2);
		
		tmpPoints = ptColl->GetPointList(i);
		while (i--)
		{
			tmpPoints[i].x = points[(i << 1)] / 200000.0;
			tmpPoints[i].y = points[(i << 1) + 1] / 200000.0;
		}
	}
	else if (this->lyrType == Map::DRAW_LAYER_POLYGON)
	{
		NEW_CLASS(ptColl, Math::Geometry::Polygon(4326, (UInt32)buff[1], (UInt32)buff[2], false, false));
		tmpPtOfsts = ptColl->GetPtOfstList(i);
		MemCopyNO(tmpPtOfsts, ptOfsts, (UInt32)buff[1] << 2);
		
		tmpPoints = ptColl->GetPointList(i);
		while (i--)
		{
			tmpPoints[i].x = points[(i << 1)] / 200000.0;
			tmpPoints[i].y = points[(i << 1) + 1] / 200000.0;
		}
	}

	if (ptOfsts)
	{
		MemFree(ptOfsts);
	}
	MemFree(points);
	return ptColl;
}

Map::MapDrawLayer::ObjectClass Map::SPDLayer::GetObjectClass() const
{
	return Map::MapDrawLayer::OC_SPD_LAYER;
}
