#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListInt32.h"
#include "Data/Int32Map.h"
#include "Data/Sort/ArtificialQuickSortC.h"
#include "DB/ColDef.h"
#include "IO/BufferedInputStream.h"
#include "IO/FileStream.h"
#include "IO/Stream.h"
#include "Map/CIPLayer2.h"
#include "Map/IMapDrawLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math.h"
#include "Math/Point.h"
#include "Math/Polygon.h"
#include "Math/Polyline.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

Map::CIPLayer2::CIPLayer2(const UTF8Char *layerName) : Map::IMapDrawLayer(layerName, 0, 0)
{
	UTF8Char fname[256];
	UTF8Char *sptr;
	IO::FileStream *file;
	IO::BufferedInputStream *bstm;
	sptr = Text::StrConcat(fname, layerName);
	if (Text::StrCompareICase(&sptr[-4], (const UTF8Char*)".CIP") == 0)
	{
		sptr = &sptr[-4];
		*sptr = 0;
	}
	UOSInt i;
	Int32 *currIds;
	UInt32 totalCnt = 0;

	this->maxId = -1;
	this->ofsts = 0;
	this->nblks = 0;
	this->blkScale = 0;
	this->blks = 0;
	this->ids = 0;
	this->maxTextSize = 0;
	this->lastObjs = 0;
	this->currObjs = 0;
	this->lyrType = (Map::DrawLayerType)0;
	this->layerName = Text::StrCopyNew(fname);
	NEW_CLASS(mut, Sync::Mutex());

	Text::StrConcat(sptr, (const UTF8Char*)".blk");
	NEW_CLASS(file, IO::FileStream(fname, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	NEW_CLASS(bstm, IO::BufferedInputStream(file, 65536));
	if (!file->IsError())
	{
		bstm->Read((UInt8*)&this->nblks, 4);
		bstm->Read((UInt8*)&this->blkScale, 4);
		this->blks = MemAlloc(CIPBlock, this->nblks);
		i = 0;
		while (i < this->nblks)
		{
			bstm->Read((UInt8*)&this->blks[i], 12);
			totalCnt += this->blks[i].objCnt;
			this->blks[i].ids = MemAlloc(Int32, this->blks[i].objCnt);
			bstm->Read((UInt8*)this->blks[i].ids, this->blks[i].objCnt << 2);
			i++;
		}

		if (true)
		{
			currIds = this->ids = MemAlloc(Int32, totalCnt);
			i = 0;
			while (i < this->nblks)
			{
				MemCopyNO(currIds, this->blks[i].ids, this->blks[i].objCnt << 2);
				MemFree(this->blks[i].ids);
				this->blks[i].ids = currIds;
				currIds += this->blks[i].objCnt;
				i++;
			}
		}
	}
	else
	{
	}
	DEL_CLASS(bstm);
	DEL_CLASS(file);

	Text::StrConcat(sptr, (const UTF8Char*)".cix");
	NEW_CLASS(file, IO::FileStream(fname, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_SEQUENTIAL));
	if (!file->IsError())
	{
		i = (UOSInt)file->GetLength();
		this->ofsts = (UInt32*)MAlloc(i);
		file->Read((UInt8*)this->ofsts, i);
		this->maxId = (OSInt)(i / 8) - 2;
	}
	DEL_CLASS(file);

	this->csys = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::GeographicCoordinateSystem::GCST_WGS84);
	missFile = false;
	if (!IsError())
	{
		Text::StrConcat(sptr, (const UTF8Char*)".cip");
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

		Text::StrConcat(sptr, (const UTF8Char*)".ciu");
		NEW_CLASS(file, IO::FileStream(fname, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_SEQUENTIAL));
		if (!file->IsError())
		{
			UInt32 buff[4];
			file->Read((UInt8*)buff, 8);
			if (buff[0] != this->nblks)
			{
				//DebugBreak();
				missFile = true;
			}
			UInt32 *tmpBuff = MemAlloc(UInt32, this->nblks * 4);
			file->Read((UInt8*)tmpBuff, this->nblks * 16);
			i = 0;
			while (i < this->nblks)
			{
				if ((Int32)tmpBuff[i * 4] != this->blks[i].xblk || (Int32)tmpBuff[i * 4 + 1] != this->blks[i].yblk)
				{
					//DebugBreak();
					missFile = true;
				}
				this->blks[i].sofst = tmpBuff[i * 4 + 3];
				i++;
			}
			MemFree(tmpBuff);
		}
		else
		{
			missFile = true;
		}
		DEL_CLASS(file);
	}
}

Map::CIPLayer2::~CIPLayer2()
{
	UOSInt i;
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
		if (this->ids)
		{
			MemFree(this->ids);
		}
		else
		{
			i = this->nblks;
			while (i-- > 0)
			{
				MemFree(this->blks[i].ids);
			}
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
	if (this->lastObjs)
	{
		this->ReleaseFileObjs(this->lastObjs);
		DEL_CLASS(this->lastObjs);
	}
	if (this->currObjs)
	{
		DEL_CLASS(this->currObjs);
	}
}

Bool Map::CIPLayer2::IsError()
{
	if (ofsts == 0)
		return true;
	if (blks == 0)
		return true;
	if (missFile)
		return true;
	return false;
}

Map::DrawLayerType Map::CIPLayer2::GetLayerType()
{
	return lyrType;
}

UOSInt Map::CIPLayer2::GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr)
{
	UOSInt textSize;
	UOSInt i;
	UOSInt l = 0;
	UOSInt k;
	
	if (nameArr)
	{
		Data::Int32Map<UTF16Char*> *tmpArr;
		NEW_CLASS(tmpArr, Data::Int32Map<UTF16Char*>());
		*nameArr = tmpArr;
		UTF8Char fileName[256];
		UTF8Char *sptr;
		IO::FileStream *cis;
		sptr = Text::StrConcat(fileName, this->layerName);
		sptr = Text::StrConcat(sptr, (const UTF8Char*)".ciu");
		NEW_CLASS(cis, IO::FileStream(fileName, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
		
		k = 0;
		while (k < this->nblks)
		{
			UTF16Char *strTmp;
			UInt8 buff[5];
			cis->SeekFromBeginning(this->blks[k].sofst);
			i = this->blks[k].objCnt;
			while (i-- > 0)
			{
				cis->Read(buff, 5);
				if (tmpArr->Get(*(Int32*)buff) == 0)
				{
					if (buff[4])
					{
						strTmp = MemAlloc(UTF16Char, (UOSInt)(buff[4] >> 1) + 1);
						cis->Read((UInt8*)strTmp, buff[4]);
						strTmp[buff[4] >> 1] = 0;
						outArr->Add(*(Int32*)buff);
						tmpArr->Put(*(Int32*)buff, strTmp);
						textSize = Text::StrUTF16_UTF8Cnt(strTmp) - 1;
						if (textSize > this->maxTextSize)
							maxTextSize = textSize;
					}
					else
					{
						strTmp = MemAlloc(UTF16Char, 1);
						strTmp[0] = 0;
						outArr->Add(*(Int32*)buff);
						tmpArr->Put(*(Int32*)buff, strTmp);
					}
				}
				else
				{
					if (buff[4])
					{
						cis->SeekFromCurrent(buff[4]);
					}
				}
			}
			k++;
		}
		DEL_CLASS(cis);
	}
	else
	{
		k = 0;
		while (k < this->nblks)
		{
			outArr->AddRangeI32(this->blks[k].ids, this->blks[k].objCnt);
			l += this->blks[k].objCnt;

			k++;
		}
		UOSInt arrSize;
		Int64 lastId;
		Int64 *arr = outArr->GetArray(&arrSize);
		ArtificialQuickSort_SortInt64(arr, 0, (OSInt)arrSize - 1);
		if (outArr->GetCount() > 0)
		{
			lastId = -1;
			k = outArr->GetCount();
			while (k-- > 0)
			{
				if (outArr->GetItem(k) == lastId)
				{
					outArr->RemoveAt(k + 1);
					l--;
				}
				else
				{
					lastId = outArr->GetItem(k);
				}
			}
		}
	}
	return l;
}

UOSInt Map::CIPLayer2::GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Int32 x1, Int32 y1, Int32 x2, Int32 y2, Bool keepEmpty)
{
	x1 = Math::Double2Int32(x1 * 200000.0 / mapRate);
	y1 = Math::Double2Int32(y1 * 200000.0 / mapRate);
	x2 = Math::Double2Int32(x2 * 200000.0 / mapRate);
	y2 = Math::Double2Int32(y2 * 200000.0 / mapRate);
	Int32 leftBlk;
	Int32 rightBlk;
	Int32 topBlk;
	Int32 bottomBlk;
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
	OSInt i;
	OSInt j;
	OSInt k;
	UOSInt l;
	if (this->nblks > 10)
	{
		i = 0;
		j = this->nblks - 1;
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

	if (j & 0x80000000)
		j = 0;

	k = j;
	l = 0;
	if (nameArr)
	{
		Data::Int32Map<UTF16Char*> *tmpArr;
		NEW_CLASS(tmpArr, Data::Int32Map<UTF16Char*>());
		*nameArr = tmpArr;
		UTF8Char fileName[256];
		UTF8Char *sptr;
		IO::FileStream *cis;
		sptr = Text::StrConcat(fileName, this->layerName);
		sptr = Text::StrConcat(sptr, (const UTF8Char*)".ciu");
		NEW_CLASS(cis, IO::FileStream(fileName, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
		
		while ((UOSInt)k < this->nblks)
		{
			if (this->blks[k].xblk > rightBlk)
				break;


			if ((this->blks[k].yblk >= topBlk) && (this->blks[k].yblk <= bottomBlk) && (this->blks[k].xblk >= leftBlk))
			{
				UTF16Char *strTmp;
				UInt8 buff[5];
				cis->SeekFromBeginning(this->blks[k].sofst);
				i = this->blks[k].objCnt;
				while (i-- > 0)
				{
					cis->Read(buff, 5);
					if (tmpArr->Get(*(Int32*)buff) == 0)
					{
						if (buff[4])
						{
							strTmp = MemAlloc(UTF16Char, (UOSInt)(buff[4] >> 1) + 1);
							cis->Read((UInt8*)strTmp, buff[4]);
							strTmp[buff[4] >> 1] = 0;
							outArr->Add(*(Int32*)buff);
							tmpArr->Put(*(Int32*)buff, strTmp);
							textSize = Text::StrUTF16_UTF8Cnt(strTmp) - 1;
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
							tmpArr->Put(*(Int32*)buff, strTmp);
						}
					}
					else
					{
						if (buff[4])
						{
							cis->SeekFromCurrent(buff[4]);
						}
					}
				}
			}
			k++;
		}
		DEL_CLASS(cis);
	}
	else
	{
		Data::ArrayListInt32 *tmpArr;
		NEW_CLASS(tmpArr, Data::ArrayListInt32());
		while ((UOSInt)k < this->nblks)
		{
			if (this->blks[k].xblk > rightBlk)
				break;

			if ((this->blks[k].yblk >= topBlk) && (this->blks[k].yblk <= bottomBlk) && (this->blks[k].xblk >= leftBlk))
			{
				tmpArr->AddRange(this->blks[k].ids, this->blks[k].objCnt);
			}
			k++;
		}
		UOSInt arrSize;
		UOSInt k;
		Int64 lastId;
		Int32 *arr = tmpArr->GetArray(&arrSize);
		ArtificialQuickSort_SortInt32(arr, 0, (OSInt)arrSize - 1);
		if (tmpArr->GetCount() > 0)
		{
			lastId = -1;
			outArr->EnsureCapacity(arrSize);
			k = 0;
			while (k < arrSize)
			{
				if (arr[k] == lastId)
				{
				}
				else
				{
					lastId = arr[k];
					outArr->Add(arr[k]);
					l++;
				}
				k++;
			}
		}
		DEL_CLASS(tmpArr);
	}
	return l;
}

UOSInt Map::CIPLayer2::GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Double x1, Double y1, Double x2, Double y2, Bool keepEmpty)
{
	return GetObjectIds(outArr, nameArr, 200000.0, Math::Double2Int32(x1 * 200000), Math::Double2Int32(y1 * 200000), Math::Double2Int32(x2 * 200000), Math::Double2Int32(y2 * 200000), keepEmpty);
}

Int64 Map::CIPLayer2::GetObjectIdMax()
{
	return this->maxId;
}

void Map::CIPLayer2::ReleaseNameArr(void *nameArr)
{
	Data::Int32Map<WChar*> *tmpMap = (Data::Int32Map<WChar*>*)nameArr;
	Data::ArrayList<WChar*> *tmpArr = tmpMap->GetValues();
	UOSInt i = tmpArr->GetCount();
	while (i-- > 0)
	{
		MemFree(tmpArr->RemoveAt(i));
	}
	DEL_CLASS(tmpMap);
}

UTF8Char *Map::CIPLayer2::GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt strIndex)
{
	Data::Int32Map<WChar*> *tmpMap = (Data::Int32Map<WChar*>*)nameArr;
	if (strIndex != 0)
	{
		*buff = 0;
		return 0;
	}
	WChar *s = tmpMap->Get((Int32)id);
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

UOSInt Map::CIPLayer2::GetColumnCnt()
{
	return 1;
}

UTF8Char *Map::CIPLayer2::GetColumnName(UTF8Char *buff, UOSInt colIndex)
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


DB::DBUtil::ColType Map::CIPLayer2::GetColumnType(UOSInt colIndex, UOSInt *colSize)
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

Bool Map::CIPLayer2::GetColumnDef(UOSInt colIndex, DB::ColDef *colDef)
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

Int32 Map::CIPLayer2::GetBlockSize()
{
	return this->blkScale;
}

UInt32 Map::CIPLayer2::GetCodePage()
{
	return 0;
}

Bool Map::CIPLayer2::GetBoundsDbl(Double *minX, Double *minY, Double *maxX, Double *maxY)
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

Map::CIPLayer2::CIPFileObject *Map::CIPLayer2::GetFileObject(void *session, Int32 id)
{
	IO::FileStream *cip = (IO::FileStream*)session;
	Map::CIPLayer2::CIPFileObject *obj;
	Int32 buff[2];

	if (this->lastObjs)
	{
		obj = this->lastObjs->Get(id);
		if (obj)
		{
			this->lastObjs->Put(id, 0);
			this->currObjs->Put(id, obj);
			return obj;
		}
	}
	obj = this->currObjs->Get(id);
	if (obj)
		return obj;
	if (id > maxId)
		return 0;

	UInt32 ofst = this->ofsts[2 + (id << 1)];
	cip->SeekFromBeginning(ofst);
	if (cip->Read((UInt8*)buff, 8) != 8)
	{
		return 0;
	}
	obj = MemAlloc(Map::CIPLayer2::CIPFileObject, 1);
	obj->id = buff[0];
	obj->nPtOfst = (UInt32)buff[1];
	if (buff[1] > 0)
	{
		obj->ptOfstArr = MemAlloc(UInt32, (UInt32)buff[1]);
		cip->Read((UInt8*)obj->ptOfstArr, sizeof(UInt32) * (UInt32)buff[1]);
	}
	else
	{
		obj->ptOfstArr = 0;
	}
	cip->Read((UInt8*)&obj->nPoint, 4);
	obj->pointArr = MemAlloc(Int32, obj->nPoint * 2);
	cip->Read((UInt8*)obj->pointArr, obj->nPoint * 8);
	this->currObjs->Put(id, obj);
	return obj;
}

void Map::CIPLayer2::ReleaseFileObjs(Data::Int32Map<Map::CIPLayer2::CIPFileObject*> *objs)
{
	Data::ArrayList<Map::CIPLayer2::CIPFileObject*> *objArr = objs->GetValues();
	Map::CIPLayer2::CIPFileObject *obj;
	UOSInt i = objArr->GetCount();
	while (i-- > 0)
	{
		obj = objArr->GetItem(i);
		if (obj)
		{
			if (obj->ptOfstArr)
			{
				MemFree(obj->ptOfstArr);
			}
			if (obj->pointArr)
			{
				MemFree(obj->pointArr);
			}
			MemFree(obj);
		}
	}
	objs->Clear();
}

void *Map::CIPLayer2::BeginGetObject()
{
	UTF8Char fileName[256];
	UTF8Char *sptr;
	IO::FileStream *cip;
	sptr = Text::StrConcat(fileName, this->layerName);
	sptr = Text::StrConcat(sptr, (const UTF8Char*)".cip");
	mut->Lock();
	if (this->currObjs == 0)
	{
		NEW_CLASS(this->currObjs, Data::Int32Map<Map::CIPLayer2::CIPFileObject*>());
	}
	NEW_CLASS(cip, IO::FileStream(fileName, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	return cip;
}

void Map::CIPLayer2::EndGetObject(void *session)
{
	IO::FileStream *cip = (IO::FileStream*)session;
	DEL_CLASS(cip);
	Data::Int32Map<Map::CIPLayer2::CIPFileObject*> *tmpObjs;
	if (this->lastObjs)
	{
		this->ReleaseFileObjs(this->lastObjs);
	}
	tmpObjs = this->lastObjs;
	this->lastObjs = this->currObjs;
	this->currObjs = tmpObjs;
	mut->Unlock();
}

Map::DrawObjectL *Map::CIPLayer2::GetObjectByIdD(void *session, Int64 id)
{
	Map::CIPLayer2::CIPFileObject *fobj = this->GetFileObject(session, (Int32)id);
	if (fobj == 0)
	{
		return 0;
	}
	Map::DrawObjectL *obj;
	obj = MemAlloc(Map::DrawObjectL, 1);
	obj->objId = fobj->id;
	obj->nPtOfst = fobj->nPtOfst;
	obj->nPoint = fobj->nPoint;
	if (fobj->ptOfstArr)
	{
		obj->ptOfstArr = MemAlloc(UInt32, fobj->nPtOfst);
		MemCopyNO(obj->ptOfstArr, fobj->ptOfstArr, sizeof(Int32) * fobj->nPtOfst);
	}
	obj->pointArr = MemAlloc(Double, fobj->nPoint << 1);
	Double r = 1 / 200000.0;
	UOSInt i = 0;
	UOSInt j = fobj->nPoint * 2;
	while (i < j)
	{
		obj->pointArr[i] = fobj->pointArr[i] * r;
		i++;
	}
	obj->flags = 0;
	obj->lineColor = 0;
	return obj;
}

Math::Vector2D *Map::CIPLayer2::GetVectorById(void *session, Int64 id)
{
	Map::CIPLayer2::CIPFileObject *fobj = this->GetFileObject(session, (Int32)id);
	if (fobj == 0)
	{
		return 0;
	}
	if (this->lyrType == Map::DRAW_LAYER_POINT)
	{
		Math::Point *pt;
		NEW_CLASS(pt, Math::Point(4326, fobj->pointArr[0] / 200000.0, fobj->pointArr[1] / 200000.0));
		return pt;
	}
	else if (fobj->ptOfstArr == 0)
	{
		return 0;
	}
	else if (this->lyrType == Map::DRAW_LAYER_POLYLINE || this->lyrType == Map::DRAW_LAYER_POLYGON)
	{
		Math::PointCollection *ptColl = 0;
		UInt32 *tmpPtOfsts;
		Double *tmpPoints;
		UOSInt i;
		if (this->lyrType == Map::DRAW_LAYER_POLYLINE)
		{
			NEW_CLASS(ptColl, Math::Polyline(4326, fobj->nPtOfst, fobj->nPoint));
		}
		else
		{
			NEW_CLASS(ptColl, Math::Polygon(4326, fobj->nPtOfst, fobj->nPoint));
		}
		tmpPtOfsts = ptColl->GetPtOfstList(&i);
		MemCopyNO(tmpPtOfsts, fobj->ptOfstArr, fobj->nPtOfst * sizeof(UInt32));
		
		tmpPoints = ptColl->GetPointList(&i);
		i = i << 1;
		while (i--)
		{
			tmpPoints[i] = fobj->pointArr[i] / 200000.0;
		}
		return ptColl;
	}
	return 0;
}

void Map::CIPLayer2::ReleaseObject(void *session, Map::DrawObjectL *obj)
{
	if (obj->ptOfstArr)
		MemFree(obj->ptOfstArr);
	if (obj->pointArr)
		MemFree(obj->pointArr);
	MemFree(obj);
}

Map::IMapDrawLayer::ObjectClass Map::CIPLayer2::GetObjectClass()
{
	return Map::IMapDrawLayer::OC_CIP_LAYER;
}
