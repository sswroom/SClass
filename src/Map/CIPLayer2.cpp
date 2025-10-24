#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListInt32.h"
#include "Data/Sort/ArtificialQuickSort_C.h"
#include "DB/ColDef.h"
#include "IO/BufferedInputStream.h"
#include "IO/FileStream.h"
#include "IO/Stream.h"
#include "Map/CIPLayer2.h"
#include "Map/MapDrawLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math_C.h"
#include "Math/Geometry/Point.h"
#include "Math/Geometry/Polygon.h"
#include "Math/Geometry/Polyline.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

Map::CIPLayer2::CIPLayer2(Text::CStringNN layerName) : Map::MapDrawLayer(layerName, 0, nullptr, Math::CoordinateSystemManager::CreateWGS84Csys())
{
	UTF8Char fname[256];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
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
	this->layerName = Text::String::NewP(fname, sptr);

	sptr2 = Text::StrConcatC(sptr, UTF8STRC(".blk"));
	{
		IO::FileStream file({fname, (UOSInt)(sptr2 - fname)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		NEW_CLASS(bstm, IO::BufferedInputStream(file, 65536));
		if (!file.IsError())
		{
			bstm->Read(Data::ByteArray((UInt8*)&this->nblks, 4));
			bstm->Read(Data::ByteArray((UInt8*)&this->blkScale, 4));
			this->blks = MemAlloc(CIPBlock, this->nblks);
			i = 0;
			while (i < this->nblks)
			{
				bstm->Read(Data::ByteArray((UInt8*)&this->blks[i], 12));
				totalCnt += this->blks[i].objCnt;
				this->blks[i].ids = MemAlloc(Int32, this->blks[i].objCnt);
				bstm->Read(Data::ByteArray((UInt8*)this->blks[i].ids, this->blks[i].objCnt << 2));
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
	}

	sptr2 = Text::StrConcatC(sptr, UTF8STRC(".cix"));
	{
		IO::FileStream file({fname, (UOSInt)(sptr2 - fname)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential);
		if (!file.IsError())
		{
			i = (UOSInt)file.GetLength();
			this->ofsts = (UInt32*)MAlloc(i);
			file.Read(Data::ByteArray((UInt8*)this->ofsts, i));
			this->maxId = (OSInt)((i - 4) / 8) - 1;
		}
	}

	missFile = false;
	if (!IsError())
	{
		sptr2 = Text::StrConcatC(sptr, UTF8STRC(".cip"));
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

		sptr2 = Text::StrConcatC(sptr, UTF8STRC(".ciu"));
		{
			IO::FileStream file({fname, (UOSInt)(sptr2 - fname)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential);
			if (!file.IsError())
			{
				UInt32 buff[4];
				file.Read(Data::ByteArray((UInt8*)buff, 8));
				if (buff[0] != this->nblks)
				{
					//DebugBreak();
					missFile = true;
				}
				UInt32 *tmpBuff = MemAlloc(UInt32, this->nblks * 4);
				file.Read(Data::ByteArray((UInt8*)tmpBuff, this->nblks * 16));
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
		}
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
	this->layerName->Release();
	NN<Data::FastMapNN<Int32, CIPFileObject>> objs;
	if (this->lastObjs.SetTo(objs))
	{
		this->ReleaseFileObjs(objs);
		this->lastObjs.Delete();
	}
	this->currObjs.Delete();
}

Bool Map::CIPLayer2::IsError() const
{
	if (ofsts == 0)
		return true;
	if (blks == 0)
		return true;
	if (missFile)
		return true;
	return false;
}

Map::DrawLayerType Map::CIPLayer2::GetLayerType() const
{
	return lyrType;
}

UOSInt Map::CIPLayer2::GetAllObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr)
{
	UOSInt textSize;
	UOSInt i;
	UOSInt l = 0;
	UOSInt k;
	
	if (nameArr.IsNotNull())
	{
		Data::FastMap<Int32, UTF16Char*> *tmpArr;
		NEW_CLASS(tmpArr, Data::Int32FastMap<UTF16Char*>());
		nameArr.SetNoCheck((NameArray*)tmpArr);
		UTF8Char fileName[256];
		UnsafeArray<UTF8Char> sptr;
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
				cis.Read(Data::ByteArray(buff, 5));
				if (tmpArr->Get(*(Int32*)buff) == 0)
				{
					if (buff[4])
					{
						strTmp = MemAlloc(UTF16Char, (UOSInt)(buff[4] >> 1) + 1);
						cis.Read(Data::ByteArray((UInt8*)strTmp, buff[4]));
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
		UnsafeArray<Int64> arr = outArr->GetArr(arrSize);
		ArtificialQuickSort_SortInt64(arr.Ptr(), 0, (OSInt)arrSize - 1);
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

UOSInt Map::CIPLayer2::GetObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	rect.min.x = Double2Int32(rect.min.x * 200000.0 / mapRate);
	rect.min.y = Double2Int32(rect.min.y * 200000.0 / mapRate);
	rect.max.x = Double2Int32(rect.max.x * 200000.0 / mapRate);
	rect.max.y = Double2Int32(rect.max.y * 200000.0 / mapRate);
	Int32 leftBlk;
	Int32 rightBlk;
	Int32 topBlk;
	Int32 bottomBlk;
	rect = rect.Reorder();
	leftBlk = rect.min.x / blkScale;
	rightBlk = rect.max.x / blkScale;
	topBlk = rect.min.y / blkScale;
	bottomBlk = rect.max.y / blkScale;

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
	if (nameArr.IsNotNull())
	{
		Data::FastMap<Int32, UTF16Char*> *tmpArr;
		NEW_CLASS(tmpArr, Data::Int32FastMap<UTF16Char*>());
		nameArr.SetNoCheck((NameArray*)tmpArr);
		UTF8Char fileName[256];
		UnsafeArray<UTF8Char> sptr;
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
					cis->Read(Data::ByteArray(buff, 5));
					if (tmpArr->Get(*(Int32*)buff) == 0)
					{
						if (buff[4])
						{
							strTmp = MemAlloc(UTF16Char, (UOSInt)(buff[4] >> 1) + 1);
							cis->Read(Data::ByteArray((UInt8*)strTmp, buff[4]));
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
		UnsafeArray<Int32> arr = tmpArr->GetArr(arrSize);
		ArtificialQuickSort_SortInt32(arr.Ptr(), 0, (OSInt)arrSize - 1);
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

UOSInt Map::CIPLayer2::GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	rect = rect * 200000;
	return GetObjectIds(outArr, nameArr, 200000.0, Math::RectArea<Int32>(Math::Coord2D<Int32>(Double2Int32(rect.min.x), Double2Int32(rect.min.y)),
		Math::Coord2D<Int32>(Double2Int32(rect.max.x), Double2Int32(rect.max.y))), keepEmpty);
}

Int64 Map::CIPLayer2::GetObjectIdMax() const
{
	return this->maxId;
}

void Map::CIPLayer2::ReleaseNameArr(Optional<NameArray> nameArr)
{
	NN<Data::FastMap<Int32, UTF16Char*>> tmpMap;
	if (Optional<Data::FastMap<Int32, UTF16Char*>>::ConvertFrom(nameArr).SetTo(tmpMap))
	{
		UOSInt i = tmpMap->GetCount();
		while (i-- > 0)
		{
			MemFree(tmpMap->GetItem(i));
		}
		tmpMap.Delete();
	}
}

Bool Map::CIPLayer2::GetString(NN<Text::StringBuilderUTF8> sb, Optional<NameArray> nameArr, Int64 id, UOSInt strIndex)
{
	NN<Data::FastMap<Int32, UTF16Char*>> tmpMap;;
	if (!Optional<Data::FastMap<Int32, UTF16Char*>>::ConvertFrom(nameArr).SetTo(tmpMap) || strIndex != 0)
	{
		return false;
	}
	UTF16Char *s = tmpMap->Get((Int32)id);
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

UOSInt Map::CIPLayer2::GetColumnCnt() const
{
	return 1;
}

UnsafeArrayOpt<UTF8Char> Map::CIPLayer2::GetColumnName(UnsafeArray<UTF8Char> buff, UOSInt colIndex)
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


DB::DBUtil::ColType Map::CIPLayer2::GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize)
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

Bool Map::CIPLayer2::GetColumnDef(UOSInt colIndex, NN<DB::ColDef> colDef)
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
	colDef->SetAutoIncNone();
	colDef->SetAttr(Text::CString(nullptr));
	return true;
}

Int32 Map::CIPLayer2::GetBlockSize() const
{
	return this->blkScale;
}

UInt32 Map::CIPLayer2::GetCodePage() const
{
	return 0;
}

Bool Map::CIPLayer2::GetBounds(OutParam<Math::RectAreaDbl> bounds) const
{
	if (this->nblks == 0)
	{
		bounds.Set(Math::RectAreaDbl(0, 0, 0, 0));
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
		bounds.Set(Math::RectAreaDbl(minBlk.ToDouble(), maxBlk.ToDouble() + 1) * (this->blkScale / 200000.0));
		return true;
	}
}

Optional<Map::CIPLayer2::CIPFileObject> Map::CIPLayer2::GetFileObject(NN<GetObjectSess> session, Int32 id)
{
	NN<IO::FileStream> cip = NN<IO::FileStream>::ConvertFrom(session);
	NN<Map::CIPLayer2::CIPFileObject> obj;
	Int32 buff[2];
	NN<Data::FastMapNN<Int32, CIPFileObject>> objs;

	if (this->lastObjs.SetTo(objs))
	{
		if (objs->Get(id).SetTo(obj))
		{
			objs->Remove(id);
			if (this->currObjs.SetTo(objs))
				objs->Put(id, obj);
			return obj;
		}
	}
	if (!this->currObjs.SetTo(objs))
		return 0;
	if (objs->Get(id).SetTo(obj))
		return obj;
	if (id > this->maxId)
		return 0;

	UInt32 ofst = this->ofsts[2 + (id << 1)];
	cip->SeekFromBeginning(ofst);
	if (cip->Read(Data::ByteArray((UInt8*)buff, 8)) != 8)
	{
		return 0;
	}
	obj = MemAllocNN(Map::CIPLayer2::CIPFileObject);
	obj->id = buff[0];
	obj->nPtOfst = (UInt32)buff[1];
	if (buff[1] > 0)
	{
		obj->ptOfstArr = MemAlloc(UInt32, (UInt32)buff[1]);
		cip->Read(Data::ByteArray((UInt8*)obj->ptOfstArr, sizeof(UInt32) * (UInt32)buff[1]));
	}
	else
	{
		obj->ptOfstArr = 0;
	}
	cip->Read(Data::ByteArray((UInt8*)&obj->nPoint, 4));
	obj->pointArr = MemAlloc(Int32, obj->nPoint * 2);
	cip->Read(Data::ByteArray((UInt8*)obj->pointArr, obj->nPoint * 8));
	objs->Put(id, obj);
	return obj;
}

void Map::CIPLayer2::ReleaseFileObjs(NN<Data::FastMapNN<Int32, Map::CIPLayer2::CIPFileObject>> objs)
{
	NN<Map::CIPLayer2::CIPFileObject> obj;
	UOSInt i = objs->GetCount();
	while (i-- > 0)
	{
		obj = objs->GetItemNoCheck(i);
		if (obj->ptOfstArr)
		{
			MemFree(obj->ptOfstArr);
		}
		if (obj->pointArr)
		{
			MemFree(obj->pointArr);
		}
		MemFreeNN(obj);
	}
	objs->Clear();
}

NN<Map::GetObjectSess> Map::CIPLayer2::BeginGetObject()
{
	UTF8Char fileName[256];
	UnsafeArray<UTF8Char> sptr;
	NN<IO::FileStream> cip;
	sptr = this->layerName->ConcatTo(fileName);
	sptr = Text::StrConcatC(sptr, UTF8STRC(".cip"));
	this->mut.Lock();
	if (this->currObjs.IsNull())
	{
		NN<Data::FastMapNN<Int32, CIPFileObject>> objs;
		NEW_CLASSNN(objs, Data::Int32FastMapNN<Map::CIPLayer2::CIPFileObject>());
		this->currObjs = objs;
	}
	NEW_CLASSNN(cip, IO::FileStream({fileName, (UOSInt)(sptr - fileName)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	return NN<GetObjectSess>::ConvertFrom(cip);
}

void Map::CIPLayer2::EndGetObject(NN<GetObjectSess> session)
{
	NN<IO::FileStream> cip = NN<IO::FileStream>::ConvertFrom(session);
	cip.Delete();
	Optional<Data::FastMapNN<Int32, Map::CIPLayer2::CIPFileObject>> tmpObjs;
	NN<Data::FastMapNN<Int32, Map::CIPLayer2::CIPFileObject>> objs;
	if (this->lastObjs.SetTo(objs))
	{
		this->ReleaseFileObjs(objs);
	}
	tmpObjs = this->lastObjs;
	this->lastObjs = this->currObjs;
	this->currObjs = tmpObjs;
	this->mut.Unlock();
}

Optional<Math::Geometry::Vector2D> Map::CIPLayer2::GetNewVectorById(NN<GetObjectSess> session, Int64 id)
{
	NN<Map::CIPLayer2::CIPFileObject> fobj;
	if (!this->GetFileObject(session, (Int32)id).SetTo(fobj))
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
	else if (this->lyrType == Map::DRAW_LAYER_POLYLINE)
	{
		Math::Geometry::Polyline *pl;
		NN<Math::Geometry::LineString> lineString;
		NEW_CLASS(pl, Math::Geometry::Polyline(4326));
		UOSInt i = 0;
		UOSInt j;
		UOSInt k;
		UOSInt l;
		UnsafeArray<Math::Coord2DDbl> tmpPoints;
		while (i < fobj->nPtOfst)
		{
			j = fobj->ptOfstArr[i];
			if (i + 1 >= fobj->nPtOfst)
				k = fobj->nPoint;
			else
				k = fobj->ptOfstArr[i + 1];
			NEW_CLASSNN(lineString, Math::Geometry::LineString(4326, (k - j), false, false));
			tmpPoints = lineString->GetPointList(l);
			l = 0;
			while (j < k)
			{
				tmpPoints[l].x = fobj->pointArr[(j << 1)] / 200000.0;
				tmpPoints[l].y = fobj->pointArr[(j << 1) + 1] / 200000.0;
				j++;
				l++;
			}
			pl->AddGeometry(lineString);
			i++;
		}
		return pl;
	}
	else if (this->lyrType == Map::DRAW_LAYER_POLYGON)
	{
		Math::Geometry::Polygon *pg = 0;
		NN<Math::Geometry::LinearRing> lr;
		UnsafeArray<Math::Coord2DDbl> tmpPoints;
		UOSInt i;
		UOSInt j;
		UOSInt k;
		UOSInt l;
		NEW_CLASS(pg, Math::Geometry::Polygon(4326));
		j = 0;
		i = 0;
		while (i < fobj->nPtOfst)
		{
			if (i + 1 == fobj->nPtOfst)
				k = fobj->nPoint;
			else
				k = fobj->ptOfstArr[i + 1];
			NEW_CLASSNN(lr, Math::Geometry::LinearRing(4326, (k - j), false, false));
			tmpPoints = lr->GetPointList(l);
			l = 0;
			while (j < k)
			{
				tmpPoints[l].x = fobj->pointArr[(j << 1)] / 200000.0;
				tmpPoints[l].y = fobj->pointArr[(j << 1) + 1] / 200000.0;
				j++;
			}
			pg->AddGeometry(lr);
			i++;
		}
		return pg;
	}
	return 0;
}

UOSInt Map::CIPLayer2::GetGeomCol() const
{
	return INVALID_INDEX;
}

Map::MapDrawLayer::ObjectClass Map::CIPLayer2::GetObjectClass() const
{
	return Map::MapDrawLayer::OC_CIP_LAYER;
}
