#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListInt32.h"
#include "Data/Int32Map.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "DB/ColDef.h"
#include "IO/BufferedInputStream.h"
#include "IO/Console.h"
#include "IO/FileStream.h"
#include "IO/FileViewStream.h"
#include "Manage/HiResClock.h"
#include "Map/CIPLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math.h"
#include "Math/Geometry/Point.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderW.h"

Map::CIPLayer::CIPLayer(Text::CString layerName) : Map::MapDrawLayer(layerName, 0, layerName)
{
	UTF8Char fname[256];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	IO::FileStream *file;
	IO::BufferedInputStream *bstm;
	sptr = layerName.ConcatTo(fname);
	UInt32 i;
	Int32 *currIds;
	Int32 totalCnt = 0;

	this->maxId = -1;
	this->ofsts = 0;
	this->nblks = 0;
	this->blkScale = 0;
	this->blks = 0;
	this->ids = 0;
	this->maxTextSize = 0;
	this->lyrType = (Map::DrawLayerType)0;
	this->layerName = Text::String::New(layerName);
	this->csys = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84);
	NEW_CLASS(this->currObjs, Data::Int32Map<CIPFileObject*>());
	NEW_CLASS(this->lastObjs, Data::Int32Map<CIPFileObject*>());

	sptr2 = Text::StrConcatC(sptr, UTF8STRC(".blk"));
	NEW_CLASS(file, IO::FileStream(CSTRP(fname, sptr2), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
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
		i = file->GetErrCode();
	}
	DEL_CLASS(bstm);
	DEL_CLASS(file);

	sptr2 = Text::StrConcatC(sptr, UTF8STRC(".cix"));
	NEW_CLASS(file, IO::FileStream(CSTRP(fname, sptr2), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (!file->IsError())
	{
		i = (Int32)file->GetLength();
		this->ofsts = (Int32*)MAlloc(i);
		file->Read((UInt8*)this->ofsts, i);
		this->maxId = (i / 8) - 2;
	}
	DEL_CLASS(file);

	missFile = false;
	if (!IsError())
	{
		sptr2 = Text::StrConcatC(sptr, UTF8STRC(".cip"));
		NEW_CLASS(file, IO::FileStream(CSTRP(fname, sptr2), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
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
		NEW_CLASS(file, IO::FileStream(CSTRP(fname, sptr2), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		NEW_CLASS(bstm, IO::BufferedInputStream(file, 65536));
		if (!file->IsError())
		{
			Int32 buff[4];
			bstm->Read((UInt8*)buff, 8);
			if (buff[0] != this->nblks)
			{
#ifdef HAS_ASM32
				_asm int 3;
#endif
			}
			i = 0;
			while (i < this->nblks)
			{
				bstm->Read((UInt8*)buff, 16);
				if (buff[0] != this->blks[i].xblk || buff[1] != this->blks[i].yblk)
				{
#ifdef HAS_ASM32
					_asm int 3;
#endif
				}
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

Map::CIPLayer::~CIPLayer()
{
	Int32 i;
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
	SDEL_STRING(this->layerName);
	this->ReleaseFileObjs(this->lastObjs);
	DEL_CLASS(this->lastObjs);
	DEL_CLASS(this->currObjs);
}

Bool Map::CIPLayer::IsError()
{
	if (ofsts == 0)
		return true;
	if (blks == 0)
		return true;
	if (missFile)
		return true;
	return false;
}

Text::String *Map::CIPLayer::GetName()
{
	return this->layerName;
}

Map::DrawLayerType Map::CIPLayer::GetLayerType()
{
	return lyrType;
}

UOSInt Map::CIPLayer::GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr)
{
	OSInt textSize;
	UOSInt i;
	UOSInt j;
	UOSInt l = 0;
	UOSInt k;
	
	if (nameArr)
	{
		Text::Encoding enc(65001);
		Data::Int32Map<UTF16Char*> *tmpArr;
		NEW_CLASS(tmpArr, Data::Int32Map<UTF16Char*>());
		*nameArr = tmpArr;
		UTF8Char fileName[256];
		UTF8Char *sptr;
		IO::FileStream *cis;
		sptr = this->layerName->ConcatTo(fileName);
		sptr = Text::StrConcatC(sptr, UTF8STRC(".ciu"));
		NEW_CLASS(cis, IO::FileStream(CSTRP(fileName, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		
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
						strTmp = MemAlloc(UTF16Char, (buff[4] >> 1) + 1);
						cis->Read((UInt8*)strTmp, buff[4]);
						strTmp[buff[4] >> 1] = 0;
						tmpArr->Put(*(Int32*)buff, strTmp);
						textSize = Text::StrUTF16_UTF8Cnt(strTmp);
						if (textSize > this->maxTextSize)
							maxTextSize = (Int32)textSize;
					}
					else
					{
						strTmp = MemAlloc(UTF16Char, 1);
						strTmp[0] = 0;
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
		outArr->AddRangeI32(tmpArr->GetKeys());
	}
	else
	{
		k = 0;
		while (k < this->nblks)
		{
//			outArr->AddRange(this->blks[k].ids, this->blks[k].objCnt);
			i = this->blks[k].objCnt;
			while (i-- > 0)
			{
				if ((j = outArr->SortedIndexOf(this->blks[k].ids[i])) < 0)
				{
					outArr->Insert(~j, this->blks[k].ids[i]);
				}
			}
			l += this->blks[k].objCnt;

			k++;
		}
	}
	return l;
}

UOSInt Map::CIPLayer::GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Int32 x1, Int32 y1, Int32 x2, Int32 y2, Bool keepEmpty)
{
	Int32 leftBlk;
	Int32 rightBlk;
	Int32 topBlk;
	Int32 bottomBlk;
	Manage::HiResClock clk;
	x1 = Double2Int32(x1 * 200000.0 / mapRate);
	y1 = Double2Int32(y1 * 200000.0 / mapRate);
	x2 = Double2Int32(x2 * 200000.0 / mapRate);
	y2 = Double2Int32(y2 * 200000.0 / mapRate);
	clk.Start();
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

	OSInt textSize;
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
		Data::Int32Map<WChar*> *tmpArr;
		NEW_CLASS(tmpArr, Data::Int32Map<WChar*>());
		*nameArr = tmpArr;
		UTF8Char fileName[256];
		UTF8Char *sptr;
		IO::FileStream *fs = 0;
		IO::SeekableStream *cis;
		sptr = this->layerName->ConcatTo(fileName);
		sptr = Text::StrConcatC(sptr, UTF8STRC(".ciu"));
		if (this->nblks > 500)
		{
//			NEW_CLASS(fs, IO::FileStream(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
//			NEW_CLASS(cis, IO::BufferedInputStream(fs, 8192));

			NEW_CLASS(cis, IO::FileViewStream(CSTRP(fileName, sptr)));
		}
		else
		{
			NEW_CLASS(cis, IO::FileStream(CSTRP(fileName, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		}
		
		while (k < (OSInt)this->nblks)
		{
			if (this->blks[k].xblk > rightBlk)
				break;


			if ((this->blks[k].yblk >= topBlk) && (this->blks[k].yblk <= bottomBlk) && (this->blks[k].xblk >= leftBlk))
			{
				WChar *strTmp;
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
							strTmp = MemAlloc(WChar, (buff[4] >> 1) + 1);
							cis->Read((UInt8*)strTmp, buff[4]);
							strTmp[buff[4] >> 1] = 0;
							tmpArr->Put(*(Int32*)buff, strTmp);
							textSize = Text::StrWChar_UTF8Cnt(strTmp);
							if (textSize > this->maxTextSize)
							{
								this->maxTextSize = (Int32)textSize;
							}
						}
						else if (keepEmpty)
						{
							strTmp = MemAlloc(WChar, 1);
							strTmp[0] = 0;
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
		if (fs)
		{
			DEL_CLASS(fs);
		}
		outArr->AddRangeI32(tmpArr->GetKeys());
	}
	else
	{
		Data::ArrayListInt32 *tmpArr;
		NEW_CLASS(tmpArr, Data::ArrayListInt32());
		while (k < (OSInt)this->nblks)
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
		Int64 lastId;
		Int32 *arr = tmpArr->GetArray(&arrSize);
		ArtificialQuickSort_SortInt32(arr, 0, arrSize - 1);
		if (tmpArr->GetCount() > 0)
		{
			lastId = -1;
			outArr->EnsureCapacity(arrSize);
			k = 0;
			while (k < (OSInt)arrSize)
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
	Double t = clk.GetTimeDiff();
	if (t > .1)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("GetObjectIds too slow: time = "));
		sb.AppendI32(Double2Int32(t * 1000));
		sb.AppendC(UTF8STRC("ms, layer = "));
		sb.Append(this->layerName);
		if (nameArr)
		{
			sb.AppendC(UTF8STRC(" (has nameArr)"));
		}
		sb.AppendC(UTF8STRC("\r\n"));
		IO::Console::PrintStrO(sb.ToString());
	}
	return l;
}

UOSInt Map::CIPLayer::GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Double x1, Double y1, Double x2, Double y2, Bool keepEmpty)
{
	return GetObjectIds(outArr, nameArr, 200000.0, Double2Int32(x1 * 200000), Double2Int32(y1 * 200000), Double2Int32(x2 * 200000), Double2Int32(y2 * 200000), keepEmpty);
}

Int64 Map::CIPLayer::GetObjectIdMax()
{
	return this->maxId;
}

void Map::CIPLayer::ReleaseNameArr(void *nameArr)
{
	Data::Int32Map<WChar*> *tmpMap = (Data::Int32Map<WChar*>*)nameArr;
	const Data::ArrayList<WChar*> *tmpArr = tmpMap->GetValues();
	UOSInt i = tmpArr->GetCount();
	while (i-- > 0)
	{
		MemFree(tmpArr->GetItem(i));
	}
	DEL_CLASS(tmpMap);
}

WChar *Map::CIPLayer::GetString(WChar *buff, void *nameArr, Int64 id, UOSInt strIndex)
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
		return Text::StrConcat(buff, s);
	}
	else
	{
		*buff = 0;
		return 0;
	}
}

UOSInt Map::CIPLayer::GetColumnCnt()
{
	return 1;
}

UTF8Char *Map::CIPLayer::GetColumnName(UTF8Char *buff, UOSInt colIndex)
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


DB::DBUtil::ColType Map::CIPLayer::GetColumnType(UOSInt colIndex, UOSInt *colSize)
{
	if (colIndex == 0)
	{
		if (colSize)
		{
			*colSize = this->maxTextSize;
		}
		return DB::DBUtil::CT_VarUTF16Char;
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

Bool Map::CIPLayer::GetColumnDef(UOSInt colIndex, DB::ColDef *colDef)
{
	if (colIndex != 0)
		return false;
	colDef->SetColName(CSTR("NAME"));
	colDef->SetColSize(this->maxTextSize);
	colDef->SetColDP(0);
	colDef->SetColType(DB::DBUtil::CT_VarUTF16Char);
	colDef->SetDefVal((Text::String*)0);
	colDef->SetNotNull(false);
	colDef->SetPK(false);
	colDef->SetAutoInc(false);
	colDef->SetAttr((Text::String*)0);
	return true;
}

Int32 Map::CIPLayer::GetBlockSize()
{
	return this->blkScale;
}

UInt32 Map::CIPLayer::GetCodePage()
{
	return 0;
}

Bool Map::CIPLayer::GetBoundsDbl(Double *minX, Double *minY, Double *maxX, Double *maxY)
{
	if (this->nblks == 0)
	{
		*minX = 0;
		*minY = 0;
		*maxX = 0;
		*maxY = 0;
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
	}
	return true;
}

Map::CIPLayer::CIPFileObject *Map::CIPLayer::GetFileObject(void *session, Int32 id)
{
	IO::SeekableStream *cip = (IO::SeekableStream*)session;
	Map::CIPLayer::CIPFileObject *obj;
	Int32 buff[2];

	obj = this->lastObjs->Get(id);
	if (obj)
	{
		this->lastObjs->Put(id, 0);
		this->currObjs->Put(id, obj);
		return obj;
	}
	obj = this->currObjs->Get(id);
	if (obj)
		return obj;

	Int32 ofst = this->ofsts[2 + (id << 1)];
	cip->SeekFromBeginning(ofst);
	if (cip->Read((UInt8*)buff, 8) != 8)
	{
		return 0;
	}
	obj = MemAlloc(Map::CIPLayer::CIPFileObject, 1);
	obj->id = buff[0];
	obj->nParts = buff[1];
	if (buff[1] > 0)
	{
		obj->parts = MemAlloc(Int32, buff[1]);
		cip->Read((UInt8*)obj->parts, sizeof(Int32) * buff[1]);
	}
	else
	{
		obj->parts = 0;
	}
	cip->Read((UInt8*)&obj->nPoints, 4);
	obj->points = MemAlloc(Int32, obj->nPoints * 2);
	cip->Read((UInt8*)obj->points, obj->nPoints * 8);
	this->currObjs->Put(id, obj);
	return obj;
}

void Map::CIPLayer::ReleaseFileObjs(Data::Int32Map<Map::CIPLayer::CIPFileObject*> *objs)
{
	const Data::ArrayList<Map::CIPLayer::CIPFileObject*> *objArr = objs->GetValues();
	Map::CIPLayer::CIPFileObject *obj;
	UOSInt i = objArr->GetCount();
	while (i-- > 0)
	{
		obj = objArr->GetItem(i);
		if (obj)
		{
			if (obj->parts)
			{
				MemFree(obj->parts);
			}
			if (obj->points)
			{
				MemFree(obj->points);
			}
			MemFree(obj);
		}
	}
	objs->Clear();
}

void *Map::CIPLayer::BeginGetObject()
{
	UTF8Char fileName[256];
	UTF8Char *sptr;
	IO::SeekableStream *cip;
	sptr = this->layerName->ConcatTo(fileName);
	sptr = Text::StrConcatC(sptr, UTF8STRC(".cip"));
	this->mut.Lock();
	NEW_CLASS(cip, IO::FileStream(CSTRP(fileName, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	//NEW_CLASS(cip, IO::FileViewStream(fileName));
	return cip;
}

void Map::CIPLayer::EndGetObject(void *session)
{
	IO::SeekableStream *cip = (IO::SeekableStream*)session;
	DEL_CLASS(cip);
	Data::Int32Map<CIPFileObject*> *tmpObjs;
	this->ReleaseFileObjs(this->lastObjs);
	tmpObjs = this->lastObjs;
	this->lastObjs = this->currObjs;
	this->currObjs = tmpObjs;
	this->mut.Unlock();
}

Math::Geometry::Vector2D *Map::CIPLayer::GetNewVectorById(void *session, Int64 id)
{
	Map::CIPLayer::CIPFileObject *fobj = this->GetFileObject(session, (Int32)id);
	if (fobj == 0)
	{
		return 0;
	}
	Math::Geometry::PointOfstCollection *ptOfst;
	Double r = 1 / 200000.0;
	if (this->lyrType == Map::DRAW_LAYER_POINT || this->lyrType == Map::DRAW_LAYER_POINT3D)
	{
		Math::Geometry::Point *pt;
		NEW_CLASS(pt, Math::Geometry::Point(4326, fobj->points[0] * r, fobj->points[1] * r));
		return pt;
	}
	else if (this->lyrType == Map::DRAW_LAYER_POLYLINE3D || this->lyrType == Map::DRAW_LAYER_POLYLINE)
	{
		NEW_CLASS(ptOfst, Math::Geometry::Polyline(4326, (UOSInt)fobj->nParts, (UOSInt)fobj->nPoints, false, false));
	}
	else if (this->lyrType == Map::DRAW_LAYER_POLYGON)
	{
		NEW_CLASS(ptOfst, Math::Geometry::Polygon(4326, (UOSInt)fobj->nParts, (UOSInt)fobj->nPoints, false, false));
	}
	else
	{
		return 0;
	}
	if (fobj->parts)
	{
		UOSInt nPtOfst;
		UInt32 *ptOfstArr = ptOfst->GetPtOfstList(&nPtOfst);
		MemCopyNO(ptOfstArr, fobj->parts, sizeof(UInt32) * fobj->nParts);
	}
	UOSInt nPoint;
	Math::Coord2DDbl *pointArr = ptOfst->GetPointList(&nPoint);
	UOSInt i = nPoint;
	while (i-- > 0)
	{
		pointArr[i] = Math::Coord2DDbl(fobj->points[i * 2] * r, fobj->points[i * 2 + 1] * r);
	}
	return ptOfst;
}

Map::MapDrawLayer::ObjectClass Map::CIPLayer::GetObjectClass()
{
	return Map::MapDrawLayer::OC_CIP_LAYER;
}
