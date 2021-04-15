#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListInt32.h"
#include "DB/ColDef.h"
#include "IO/BufferedInputStream.h"
#include "IO/FileStream.h"
#include "Map/IMapDrawLayer.h"
#include "Map/SPDLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math.h"
#include "Math/Point.h"
#include "Math/Polygon.h"
#include "Math/Polyline.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

Map::SPDLayer::SPDLayer(const UTF8Char *layerName) : Map::IMapDrawLayer(layerName, 0, 0)
{
	UTF8Char fname[256];
	UTF8Char *sptr;
	IO::FileStream *file;
	IO::BufferedInputStream *bstm;
	sptr = Text::StrConcat(fname, layerName);
	if (Text::StrCompareICase(&sptr[-4], (const UTF8Char*)".SPD") == 0)
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
	this->csys = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::GeographicCoordinateSystem::GCST_WGS84);
	this->lyrType = (Map::DrawLayerType)0;
	this->layerName = Text::StrCopyNew(fname);
	NEW_CLASS(this->mut, Sync::Mutex());

	Text::StrConcat(sptr, (const UTF8Char*)".spb");
	NEW_CLASS(file, IO::FileStream(fname, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	NEW_CLASS(bstm, IO::BufferedInputStream(file, 65536));
	if (!file->IsError())
	{
		bstm->Read((UInt8*)&this->nblks, 4);
		bstm->Read((UInt8*)&this->blkScale, 4);
		this->blks = MemAlloc(SPDBlock, this->nblks);
		i = 0;
		while (i < this->nblks)
		{
			bstm->Read((UInt8*)&this->blks[i], 12);
			this->blks[i].ids = MemAlloc(Int32, this->blks[i].objCnt);
			bstm->Read((UInt8*)this->blks[i].ids, this->blks[i].objCnt << 2);
			i++;
		}
	}
	else
	{
		i = (UOSInt)file->GetErrCode();
	}
	DEL_CLASS(bstm);
	DEL_CLASS(file);

	Text::StrConcat(sptr, (const UTF8Char*)".spi");
	NEW_CLASS(file, IO::FileStream(fname, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_SEQUENTIAL));
	if (!file->IsError())
	{
		i = (UOSInt)file->GetLength();
		this->ofsts = (Int32*)MAlloc(i);
		file->Read((UInt8*)this->ofsts, i);
		this->maxId = (OSInt)(i / 8) - 2;
	}
	DEL_CLASS(file);

	missFile = false;
	if (!IsError())
	{
		Text::StrConcat(sptr, (const UTF8Char*)".spd");
		NEW_CLASS(file, IO::FileStream(fname, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_SEQUENTIAL));
		if (!file->IsError())
		{
			file->Read((UInt8*)&i, 4);
			file->Read((UInt8*)&this->lyrType, 4);
		}
		else
		{
			missFile = true;
		}
		DEL_CLASS(file);

		Text::StrConcat(sptr, (const UTF8Char*)".sps");
		NEW_CLASS(file, IO::FileStream(fname, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_SEQUENTIAL));
		NEW_CLASS(bstm, IO::BufferedInputStream(file, 65536));
		if (!file->IsError())
		{
			Int32 buff[4];
			bstm->Read((UInt8*)buff, 8);
			i = 0;
			while (i < this->nblks)
			{
				bstm->Read((UInt8*)buff, 16);
				this->blks[i].sofst = buff[3];
				i++;
			}
		}
		else
		{
			missFile = true;
		}
		DEL_CLASS(bstm);
		DEL_CLASS(file);
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
	DEL_CLASS(mut);
}

Bool Map::SPDLayer::IsError()
{
	if (ofsts == 0)
		return true;
	if (blks == 0)
		return true;
	if (missFile)
		return true;
	return false;
}

Map::DrawLayerType Map::SPDLayer::GetLayerType()
{
	return lyrType;
}

UOSInt Map::SPDLayer::GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr)
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
		*nameArr = tmpArr;
		UTF8Char fileName[256];
		UTF8Char *sptr;
		IO::FileStream *cis;
		sptr = Text::StrConcat(fileName, this->layerName);
		sptr = Text::StrConcat(sptr, (const UTF8Char*)".sps");
		NEW_CLASS(cis, IO::FileStream(fileName, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
		
		while (k < this->nblks)
		{
			WChar *strTmp;
			UInt8 buff[13];
			cis->Seek(IO::SeekableStream::ST_BEGIN, this->blks[k].sofst);
			i = this->blks[k].objCnt;
			while (i-- > 0)
			{
				cis->Read(buff, 13);
				if (buff[4])
				{
					strTmp = MemAlloc(WChar, (UOSInt)(buff[12] >> 1) + 1);
					cis->Read((UInt8*)strTmp, buff[12]);
					strTmp[buff[12] >> 1] = 0;
					outArr->Add(*(Int32*)buff);
					tmpArr->Add(strTmp);
					cis->Seek(IO::SeekableStream::ST_CURRENT, *(Int32*)&buff[4] - 13 - buff[12]);
					textSize = Text::StrWChar_UTF8Cnt(strTmp, -1) - 1;
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

UOSInt Map::SPDLayer::GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Int32 x1, Int32 y1, Int32 x2, Int32 y2, Bool keepEmpty)
{
	x1 = Math::Double2Int32(x1 * 200000.0 / mapRate);
	y1 = Math::Double2Int32(y1 * 200000.0 / mapRate);
	x2 = Math::Double2Int32(x2 * 200000.0 / mapRate);
	y2 = Math::Double2Int32(y2 * 200000.0 / mapRate);
	Int32 leftBlk;
	Int32 rightBlk;
	Int32 topBlk;
	Int32 bottomBlk;
//	this->mut->Lock();
	if (x1 > x2)
	{
		leftBlk = x2 / blkScale;
		rightBlk = x1 / blkScale;
	}
	else
	{
		leftBlk = x1 / blkScale;
		rightBlk = x2 / blkScale;
	}
	if (y1 > y2)
	{
		topBlk = y2 / blkScale;
		bottomBlk = y1 / blkScale;
	}
	else
	{
		topBlk = y1 / blkScale;
		bottomBlk = y2 / blkScale;
	}

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
			if (this->blks[k].xblk < leftBlk)
			{
				i = k + 1;
			}
			else if (this->blks[k].xblk > leftBlk)
			{
				j = k - 1;
			}
			else if (this->blks[k].yblk < topBlk)
			{
				i = k + 1;
			}
			else if (this->blks[k].yblk > topBlk)
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
		*nameArr = tmpArr;
		UTF8Char fileName[256];
		UTF8Char *sptr;
		IO::FileStream *cis;
		sptr = Text::StrConcat(fileName, this->layerName);
		sptr = Text::StrConcat(sptr, (const UTF8Char*)".sps");
		NEW_CLASS(cis, IO::FileStream(fileName, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
		
		while ((UOSInt)k < this->nblks)
		{
			if (this->blks[k].xblk > rightBlk)
				break;


			if ((this->blks[k].yblk >= topBlk) && (this->blks[k].yblk <= bottomBlk) && (this->blks[k].xblk >= leftBlk))
			{
				WChar *strTmp;
				UInt8 buff[13];
				cis->Seek(IO::SeekableStream::ST_BEGIN, this->blks[k].sofst);
				i = (Int32)this->blks[k].objCnt;
				while (i-- > 0)
				{
					cis->Read(buff, 13);
					if (buff[4])
					{
						strTmp = MemAlloc(WChar, (UOSInt)(buff[12] >> 1) + 1);
						cis->Read((UInt8*)strTmp, buff[12]);
						strTmp[buff[12] >> 1] = 0;
						outArr->Add(*(Int32*)buff);
						tmpArr->Add(strTmp);
						cis->Seek(IO::SeekableStream::ST_CURRENT, *(Int32*)&buff[4] - 13 - buff[12]);
						textSize = Text::StrWChar_UTF8Cnt(strTmp, -1);
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
			if (this->blks[k].xblk > rightBlk)
				break;


			if ((this->blks[k].yblk >= topBlk) && (this->blks[k].yblk <= bottomBlk) && (this->blks[k].xblk >= leftBlk))
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

UOSInt Map::SPDLayer::GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Double x1, Double y1, Double x2, Double y2, Bool keepEmpty)
{
	return GetObjectIds(outArr, nameArr, 200000.0, Math::Double2Int32(x1 * 200000), Math::Double2Int32(y1 * 200000), Math::Double2Int32(x2 * 200000), Math::Double2Int32(y2 * 200000), keepEmpty);
}

Int64 Map::SPDLayer::GetObjectIdMax()
{
	return this->maxId;
}

void Map::SPDLayer::ReleaseNameArr(void *nameArr)
{
	Data::ArrayList<WChar *>*tmpArr = (Data::ArrayList<WChar*>*)nameArr;
	UOSInt i = tmpArr->GetCount();
	while (i-- > 0)
	{
		MemFree(tmpArr->RemoveAt(i));
	}
	DEL_CLASS(tmpArr);
}

UTF8Char *Map::SPDLayer::GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt strIndex)
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
		return Text::StrWChar_UTF8(buff, s, -1);
	}
	else
	{
		*buff = 0;
		return 0;
	}
}

UOSInt Map::SPDLayer::GetColumnCnt()
{
	return 1;
}

UTF8Char *Map::SPDLayer::GetColumnName(UTF8Char *buff, UOSInt colIndex)
{
	if (colIndex == 0)
	{
		return Text::StrConcat(buff, (const UTF8Char*)"NAME");
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
		return DB::DBUtil::CT_VarChar;
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

Bool Map::SPDLayer::GetColumnDef(UOSInt colIndex, DB::ColDef *colDef)
{
	if (colIndex != 0)
		return false;
	colDef->SetColName((const UTF8Char*)"NAME");
	colDef->SetColSize(this->maxTextSize);
	colDef->SetColDP(0);
	colDef->SetColType(DB::DBUtil::CT_VarChar);
	colDef->SetDefVal(0);
	colDef->SetNotNull(false);
	colDef->SetPK(false);
	colDef->SetAutoInc(false);
	colDef->SetAttr(0);
	return true;
}

Int32 Map::SPDLayer::GetBlockSize()
{
	return this->blkScale;
}

Int32 Map::SPDLayer::GetCodePage()
{
	return 0;
}

Bool Map::SPDLayer::GetBoundsDbl(Double *minX, Double *minY, Double *maxX, Double *maxY)
{
	if (this->nblks == 0)
	{
		*minX = 0;
		*minY = 0;
		*maxX = 0;
		*maxY = 0;
		return false;
	}
	else
	{
		Int32 maxXBlk;
		Int32 maxYBlk;
		Int32 minXBlk;
		Int32 minYBlk;
		maxXBlk = minXBlk = this->blks[0].xblk;
		maxYBlk = minYBlk = this->blks[0].yblk;
		OSInt i = this->nblks;
		while (i-- > 0)
		{
			if (this->blks[i].xblk > maxXBlk)
			{
				maxXBlk = this->blks[i].xblk;
			}
			if (this->blks[i].xblk < minXBlk)
			{
				minXBlk = this->blks[i].xblk;
			}
			if (this->blks[i].yblk > maxYBlk)
			{
				maxYBlk = this->blks[i].yblk;
			}
			if (this->blks[i].yblk < minYBlk)
			{
				minYBlk = this->blks[i].yblk;
			}
		}
		*minX = minXBlk / 200000.0 * this->blkScale;
		*minY = minYBlk / 200000.0 * this->blkScale;
		*maxX = (maxXBlk + 1) / 200000.0 * this->blkScale;
		*maxY = (maxYBlk + 1) / 200000.0 * this->blkScale;
		return true;
	}
}

void *Map::SPDLayer::BeginGetObject()
{
	UTF8Char fileName[256];
	UTF8Char *sptr;
	IO::FileStream *cip;
//	this->mut->Lock();
	sptr = Text::StrConcat(fileName, this->layerName);
	sptr = Text::StrConcat(sptr, (const UTF8Char*)".spd");
	NEW_CLASS(cip, IO::FileStream(fileName, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	return cip;
}

void Map::SPDLayer::EndGetObject(void *session)
{
	IO::FileStream *cip = (IO::FileStream*)session;
	DEL_CLASS(cip);
//	this->mut->Unlock();
}

Map::DrawObjectL *Map::SPDLayer::GetObjectByIdD(void *session, Int64 id)
{
	IO::FileStream *cip = (IO::FileStream*)session;

	Int32 objBuff[2];
	Int32 ofst = this->ofsts[2 + (id << 1)];
	Map::DrawObjectL *obj;
	cip->Seek(IO::SeekableStream::ST_BEGIN, ofst);
	cip->Read((UInt8*)objBuff, 8);
	obj = MemAlloc(Map::DrawObjectL, 1);
	obj->objId = objBuff[0];
	obj->nPtOfst = (UInt32)objBuff[1];
	if (obj->nPtOfst > 0)
	{
		obj->ptOfstArr = MemAlloc(UInt32, obj->nPtOfst);
		cip->Read((UInt8*)obj->ptOfstArr, sizeof(UInt32) * obj->nPtOfst);
	}
	else
	{
		obj->ptOfstArr = 0;
	}
	cip->Read((UInt8*)&obj->nPoint, 4);
	UOSInt j = obj->nPoint * 2;
	obj->pointArr = MemAlloc(Double, j);
	Int32 *tmpArr = MemAlloc(Int32, j);
	Double r = 1 / 200000.0;
	UOSInt i = 0;
	cip->Read((UInt8*)tmpArr, obj->nPoint * 8);
	while (i < j)
	{
		obj->pointArr[i] = tmpArr[i] * r;
		i++;
	}
	MemFree(tmpArr);
	obj->flags = 0;
	obj->lineColor = 0;
	return obj;
}

Math::Vector2D *Map::SPDLayer::GetVectorById(void *session, Int64 id)
{
	Int32 buff[3];

	IO::FileStream *cip = (IO::FileStream*)session;
	Int32 ofst = this->ofsts[2 + (id << 1)];
	Math::PointCollection *ptColl = 0;
	UInt32 *ptOfsts;
	Int32 *points;
	UOSInt i;
	UInt32 *tmpPtOfsts;
	Double *tmpPoints;

	cip->Seek(IO::SeekableStream::ST_BEGIN, ofst);
	cip->Read((UInt8*)buff, 8);
	
	if (buff[1] > 0)
	{
		ptOfsts = MemAlloc(UInt32, (UOSInt)buff[1]);
		cip->Read((UInt8*)ptOfsts, sizeof(UInt32) * (UOSInt)buff[1]);
	}
	else
	{
		ptOfsts = 0;
	}
	cip->Read((UInt8*)&buff[2], 4);
	points = MemAlloc(Int32, (UOSInt)buff[2] * 2);
	cip->Read((UInt8*)points, (UOSInt)buff[2] * 8);

	if (this->lyrType == Map::DRAW_LAYER_POINT)
	{
		Math::Point *pt;
		NEW_CLASS(pt, Math::Point(4326, points[0] / 200000.0, points[1] / 200000.0));
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
		NEW_CLASS(ptColl, Math::Polyline(4326, (UInt32)buff[1], (UInt32)buff[2]));
		tmpPtOfsts = ptColl->GetPtOfstList(&i);
		MemCopyNO(tmpPtOfsts, ptOfsts, (UInt32)buff[1] << 2);
		
		tmpPoints = ptColl->GetPointList(&i);
		i = i << 1;
		while (i--)
		{
			tmpPoints[i] = points[i] / 200000.0;
		}
	}
	else if (this->lyrType == Map::DRAW_LAYER_POLYGON)
	{
		NEW_CLASS(ptColl, Math::Polygon(4326, (UInt32)buff[1], (UInt32)buff[2]));
		tmpPtOfsts = ptColl->GetPtOfstList(&i);
		MemCopyNO(tmpPtOfsts, ptOfsts, (UInt32)buff[1] << 2);
		
		tmpPoints = ptColl->GetPointList(&i);
		i = i << 1;
		while (i--)
		{
			tmpPoints[i] = points[i] / 200000.0;
		}
	}

	if (ptOfsts)
	{
		MemFree(ptOfsts);
	}
	MemFree(points);
	return ptColl;
}

void Map::SPDLayer::ReleaseObject(void *session, Map::DrawObjectL *obj)
{
	if (obj->ptOfstArr)
		MemFree(obj->ptOfstArr);
	if (obj->pointArr)
		MemFree(obj->pointArr);
	MemFree(obj);
}

/*void Map::SPDLayer::ReleaseObject(void *session, DrawObjectDbl *obj)
{
	if (obj->parts)
		MemFree(obj->parts);
	if (obj->ipoints)
		MemFree(obj->ipoints);
	if (obj->points)
		MemFree(obj->points);
	MemFree(obj);
}*/

Map::IMapDrawLayer::ObjectClass Map::SPDLayer::GetObjectClass()
{
	return Map::IMapDrawLayer::OC_SPD_LAYER;
}
