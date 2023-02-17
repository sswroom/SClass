#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListInt32.h"
#include "Data/Sort/ArtificialQuickSortC.h"
#include "DB/ColDef.h"
#include "IO/BufferedInputStream.h"
#include "IO/FileStream.h"
#include "IO/Stream.h"
#include "Map/CIPLayer2.h"
#include "Map/IMapDrawLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math.h"
#include "Math/Geometry/Point.h"
#include "Math/Geometry/Polygon.h"
#include "Math/Geometry/Polyline.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

Map::CIPLayer2::CIPLayer2(Text::CString layerName) : Map::IMapDrawLayer(layerName, 0, CSTR_NULL)
{
	UTF8Char fname[256];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	IO::FileStream *file;
	IO::BufferedInputStream *bstm;
	sptr = layerName.ConcatTo(fname);
	if (layerName.EndsWithICase(UTF8STRC(".CIP")))
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
	this->layerName = Text::String::New(fname, (UOSInt)(sptr - fname));

	sptr2 = Text::StrConcatC(sptr, UTF8STRC(".blk"));
	NEW_CLASS(file, IO::FileStream({fname, (UOSInt)(sptr2 - fname)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
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

	sptr2 = Text::StrConcatC(sptr, UTF8STRC(".cix"));
	NEW_CLASS(file, IO::FileStream({fname, (UOSInt)(sptr2 - fname)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential));
	if (!file->IsError())
	{
		i = (UOSInt)file->GetLength();
		this->ofsts = (UInt32*)MAlloc(i);
		file->Read((UInt8*)this->ofsts, i);
		this->maxId = (OSInt)((i - 4) / 8) - 1;
	}
	DEL_CLASS(file);

	this->csys = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84);
	missFile = false;
	if (!IsError())
	{
		sptr2 = Text::StrConcatC(sptr, UTF8STRC(".cip"));
		NEW_CLASS(file, IO::FileStream({fname, (UOSInt)(sptr2 - fname)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential));
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

		sptr2 = Text::StrConcatC(sptr, UTF8STRC(".ciu"));
		NEW_CLASS(file, IO::FileStream({fname, (UOSInt)(sptr2 - fname)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential));
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
				if ((Int32)tmpBuff[i * 4] != this->blks[i].blk.x || (Int32)tmpBuff[i * 4 + 1] != this->blks[i].blk.y)
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
		this->layerName->Release();
		this->layerName = 0;
	}
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

UOSInt Map::CIPLayer2::GetAllObjectIds(Data::ArrayListInt64 *outArr, NameArray **nameArr)
{
	UOSInt textSize;
	UOSInt i;
	UOSInt l = 0;
	UOSInt k;
	
	if (nameArr)
	{
		Data::FastMap<Int32, UTF16Char*> *tmpArr;
		NEW_CLASS(tmpArr, Data::Int32FastMap<UTF16Char*>());
		*nameArr = (NameArray*)tmpArr;
		UTF8Char fileName[256];
		UTF8Char *sptr;
		sptr = this->layerName->ConcatTo(fileName);
		sptr = Text::StrConcatC(sptr, UTF8STRC(".ciu"));
		IO::FileStream cis({fileName, (UOSInt)(sptr - fileName)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		
		k = 0;
		while (k < this->nblks)
		{
			UTF16Char *strTmp;
			UInt8 buff[5];
			cis.SeekFromBeginning(this->blks[k].sofst);
			i = this->blks[k].objCnt;
			while (i-- > 0)
			{
				cis.Read(buff, 5);
				if (tmpArr->Get(*(Int32*)buff) == 0)
				{
					if (buff[4])
					{
						strTmp = MemAlloc(UTF16Char, (UOSInt)(buff[4] >> 1) + 1);
						cis.Read((UInt8*)strTmp, buff[4]);
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
						cis.SeekFromCurrent(buff[4]);
					}
				}
			}
			k++;
		}
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

UOSInt Map::CIPLayer2::GetObjectIds(Data::ArrayListInt64 *outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	rect.tl.x = Double2Int32(rect.tl.x * 200000.0 / mapRate);
	rect.tl.y = Double2Int32(rect.tl.y * 200000.0 / mapRate);
	rect.br.x = Double2Int32(rect.br.x * 200000.0 / mapRate);
	rect.br.y = Double2Int32(rect.br.y * 200000.0 / mapRate);
	Int32 leftBlk;
	Int32 rightBlk;
	Int32 topBlk;
	Int32 bottomBlk;
	rect = rect.Reorder();
	leftBlk = rect.tl.x / blkScale;
	rightBlk = rect.br.x / blkScale;
	topBlk = rect.tl.y / blkScale;
	bottomBlk = rect.br.y / blkScale;

	UOSInt textSize;
	OSInt i;
	OSInt j;
	OSInt k;
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
		Data::FastMap<Int32, UTF16Char*> *tmpArr;
		NEW_CLASS(tmpArr, Data::Int32FastMap<UTF16Char*>());
		*nameArr = (NameArray*)tmpArr;
		UTF8Char fileName[256];
		UTF8Char *sptr;
		IO::FileStream *cis;
		sptr = this->layerName->ConcatTo(fileName);
		sptr = Text::StrConcatC(sptr, UTF8STRC(".ciu"));
		NEW_CLASS(cis, IO::FileStream({fileName, (UOSInt)(sptr - fileName)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		
		while ((UOSInt)k < this->nblks)
		{
			if (this->blks[k].blk.x > rightBlk)
				break;


			if ((this->blks[k].blk.y >= topBlk) && (this->blks[k].blk.y <= bottomBlk) && (this->blks[k].blk.x >= leftBlk))
			{
				UTF16Char *strTmp;
				UInt8 buff[5];
				cis->SeekFromBeginning(this->blks[k].sofst);
				i = (Int32)this->blks[k].objCnt;
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
			if (this->blks[k].blk.x > rightBlk)
				break;

			if ((this->blks[k].blk.y >= topBlk) && (this->blks[k].blk.y <= bottomBlk) && (this->blks[k].blk.x >= leftBlk))
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

UOSInt Map::CIPLayer2::GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	rect = rect * 200000;
	return GetObjectIds(outArr, nameArr, 200000.0, Math::RectArea<Int32>(Math::Coord2D<Int32>(Double2Int32(rect.tl.x), Double2Int32(rect.tl.y)),
		Math::Coord2D<Int32>(Double2Int32(rect.br.x), Double2Int32(rect.br.y))), keepEmpty);
}

Int64 Map::CIPLayer2::GetObjectIdMax()
{
	return this->maxId;
}

void Map::CIPLayer2::ReleaseNameArr(NameArray *nameArr)
{
	Data::FastMap<Int32, UTF16Char*> *tmpMap = (Data::FastMap<Int32, UTF16Char*>*)nameArr;
	UOSInt i = tmpMap->GetCount();
	while (i-- > 0)
	{
		MemFree(tmpMap->GetItem(i));
	}
	DEL_CLASS(tmpMap);
}

UTF8Char *Map::CIPLayer2::GetString(UTF8Char *buff, UOSInt buffSize, NameArray *nameArr, Int64 id, UOSInt strIndex)
{
	Data::FastMap<Int32, UTF16Char*> *tmpMap = (Data::FastMap<Int32, UTF16Char*>*)nameArr;
	if (strIndex != 0)
	{
		*buff = 0;
		return 0;
	}
	UTF16Char *s = tmpMap->Get((Int32)id);
	if (s)
	{
		return Text::StrUTF16_UTF8(buff, s);
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
		return Text::StrConcatC(buff, UTF8STRC("NAME"));
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

Bool Map::CIPLayer2::GetColumnDef(UOSInt colIndex, DB::ColDef *colDef)
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
	colDef->SetAutoInc(false);
	colDef->SetAttr(CSTR_NULL);
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

Bool Map::CIPLayer2::GetBounds(Math::RectAreaDbl *bounds)
{
	if (this->nblks == 0)
	{
		*bounds = Math::RectAreaDbl(0, 0, 0, 0);
		return false;
	}
	else
	{
		Math::Coord2D<Int32> maxBlk;
		Math::Coord2D<Int32> minBlk;
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
		*bounds = Math::RectAreaDbl(minBlk.ToDouble(), maxBlk.ToDouble() + 1) * (this->blkScale / 200000.0);
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
	if (id > this->maxId)
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

void Map::CIPLayer2::ReleaseFileObjs(Data::FastMap<Int32, Map::CIPLayer2::CIPFileObject*> *objs)
{
	Map::CIPLayer2::CIPFileObject *obj;
	UOSInt i = objs->GetCount();
	while (i-- > 0)
	{
		obj = objs->GetItem(i);
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

Map::GetObjectSess *Map::CIPLayer2::BeginGetObject()
{
	UTF8Char fileName[256];
	UTF8Char *sptr;
	IO::FileStream *cip;
	sptr = this->layerName->ConcatTo(fileName);
	sptr = Text::StrConcatC(sptr, UTF8STRC(".cip"));
	this->mut.Lock();
	if (this->currObjs == 0)
	{
		NEW_CLASS(this->currObjs, Data::Int32FastMap<Map::CIPLayer2::CIPFileObject*>());
	}
	NEW_CLASS(cip, IO::FileStream({fileName, (UOSInt)(sptr - fileName)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	return (GetObjectSess*)cip;
}

void Map::CIPLayer2::EndGetObject(GetObjectSess *session)
{
	IO::FileStream *cip = (IO::FileStream*)session;
	DEL_CLASS(cip);
	Data::FastMap<Int32, Map::CIPLayer2::CIPFileObject*> *tmpObjs;
	if (this->lastObjs)
	{
		this->ReleaseFileObjs(this->lastObjs);
	}
	tmpObjs = this->lastObjs;
	this->lastObjs = this->currObjs;
	this->currObjs = tmpObjs;
	this->mut.Unlock();
}

Math::Geometry::Vector2D *Map::CIPLayer2::GetNewVectorById(GetObjectSess *session, Int64 id)
{
	Map::CIPLayer2::CIPFileObject *fobj = this->GetFileObject(session, (Int32)id);
	if (fobj == 0)
	{
		return 0;
	}
	if (this->lyrType == Map::DRAW_LAYER_POINT)
	{
		Math::Geometry::Point *pt;
		NEW_CLASS(pt, Math::Geometry::Point(4326, fobj->pointArr[0] / 200000.0, fobj->pointArr[1] / 200000.0));
		return pt;
	}
	else if (fobj->ptOfstArr == 0)
	{
		return 0;
	}
	else if (this->lyrType == Map::DRAW_LAYER_POLYLINE || this->lyrType == Map::DRAW_LAYER_POLYGON)
	{
		Math::Geometry::PointOfstCollection *ptColl = 0;
		UInt32 *tmpPtOfsts;
		Math::Coord2DDbl *tmpPoints;
		UOSInt i;
		if (this->lyrType == Map::DRAW_LAYER_POLYLINE)
		{
			NEW_CLASS(ptColl, Math::Geometry::Polyline(4326, fobj->nPtOfst, fobj->nPoint, false, false));
		}
		else
		{
			NEW_CLASS(ptColl, Math::Geometry::Polygon(4326, fobj->nPtOfst, fobj->nPoint, false, false));
		}
		tmpPtOfsts = ptColl->GetPtOfstList(&i);
		MemCopyNO(tmpPtOfsts, fobj->ptOfstArr, fobj->nPtOfst * sizeof(UInt32));
		
		tmpPoints = ptColl->GetPointList(&i);
		while (i--)
		{
			tmpPoints[i].x = fobj->pointArr[(i << 1)] / 200000.0;
			tmpPoints[i].y = fobj->pointArr[(i << 1) + 1] / 200000.0;
		}
		return ptColl;
	}
	return 0;
}

Map::IMapDrawLayer::ObjectClass Map::CIPLayer2::GetObjectClass()
{
	return Map::IMapDrawLayer::OC_CIP_LAYER;
}
