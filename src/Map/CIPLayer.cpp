#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListInt32.h"
#include "Data/FastMapObj.hpp"
#include "Data/Sort/ArtificialQuickSort.h"
#include "DB/ColDef.h"
#include "IO/BufferedInputStream.h"
#include "IO/Console.h"
#include "IO/FileStream.h"
#include "IO/FileViewStream.h"
#include "Manage/HiResClock.h"
#include "Map/CIPLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math_C.h"
#include "Math/Geometry/Point.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderW.h"

Map::CIPLayer::CIPLayer(Text::CStringNN layerName) : Map::MapDrawLayer(layerName, 0, layerName, Math::CoordinateSystemManager::CreateWGS84Csys())
{
	UTF8Char fname[256];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	NN<IO::FileStream> file;
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
	NEW_CLASSNN(this->lastObjs, Data::Int32FastMapObj<Optional<CIPFileObject>>());
	NEW_CLASSNN(this->currObjs, Data::Int32FastMapObj<Optional<CIPFileObject>>());

	sptr2 = Text::StrConcatC(sptr, UTF8STRC(".blk"));
	NEW_CLASSNN(file, IO::FileStream(CSTRP(fname, sptr2), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASS(bstm, IO::BufferedInputStream(file, 65536));
	if (!file->IsError())
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
		i = file->GetErrCode();
	}
	DEL_CLASS(bstm);
	file.Delete();

	sptr2 = Text::StrConcatC(sptr, UTF8STRC(".cix"));
	NEW_CLASSNN(file, IO::FileStream(CSTRP(fname, sptr2), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (!file->IsError())
	{
		i = (Int32)file->GetLength();
		this->ofsts = (Int32*)MAlloc(i);
		file->Read(Data::ByteArray((UInt8*)this->ofsts, i));
		this->maxId = (i / 8) - 2;
	}
	file.Delete();

	missFile = false;
	if (!IsError())
	{
		sptr2 = Text::StrConcatC(sptr, UTF8STRC(".cip"));
		NEW_CLASSNN(file, IO::FileStream(CSTRP(fname, sptr2), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		if (!file->IsError())
		{
			file->Read(Data::ByteArray((UInt8*)&i, 4));
			file->Read(Data::ByteArray((UInt8*)&this->lyrType, 4));
		}
		else
		{
			missFile = true;
		}
		file.Delete();

		sptr2 = Text::StrConcatC(sptr, UTF8STRC(".ciu"));
		NEW_CLASSNN(file, IO::FileStream(CSTRP(fname, sptr2), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		NEW_CLASS(bstm, IO::BufferedInputStream(file, 65536));
		if (!file->IsError())
		{
			Int32 buff[4];
			bstm->Read(Data::ByteArray((UInt8*)buff, 8));
			if (buff[0] != this->nblks)
			{
#ifdef HAS_ASM32
				_asm int 3;
#endif
			}
			i = 0;
			while (i < this->nblks)
			{
				bstm->Read(Data::ByteArray((UInt8*)buff, 16));
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
		file.Delete();
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
	this->layerName->Release();
	this->ReleaseFileObjs(this->lastObjs);
	this->lastObjs.Delete();
	this->currObjs.Delete();
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

NN<Text::String> Map::CIPLayer::GetName()
{
	return this->layerName;
}

Map::DrawLayerType Map::CIPLayer::GetLayerType()
{
	return lyrType;
}

UIntOS Map::CIPLayer::GetAllObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr)
{
	IntOS textSize;
	UIntOS i;
	UIntOS j;
	UIntOS l = 0;
	UIntOS k;
	
	if (nameArr.IsNotNull())
	{
		Text::Encoding enc(65001);
		Data::Int64FastMapObj<UTF16Char*> *tmpArr;
		NEW_CLASS(tmpArr, Data::Int64FastMapObj<UTF16Char*>());
		nameArr.SetNoCheck((NameArray*)tmpArr);
		UTF8Char fileName[256];
		UnsafeArray<UTF8Char> sptr;
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
				cis->Read(Data::ByteArray(buff, 5));
				if (tmpArr->Get(ReadInt32(buff)) == 0)
				{
					if (buff[4])
					{
						strTmp = MemAlloc(UTF16Char, (buff[4] >> 1) + 1);
						cis->Read(Data::ByteArray((UInt8*)strTmp, buff[4]));
						strTmp[buff[4] >> 1] = 0;
						tmpArr->Put(ReadInt32(buff), strTmp);
						textSize = Text::StrUTF16_UTF8Cnt(strTmp);
						if (textSize > this->maxTextSize)
							maxTextSize = (Int32)textSize;
					}
					else
					{
						strTmp = MemAlloc(UTF16Char, 1);
						strTmp[0] = 0;
						tmpArr->Put(ReadInt32(buff), strTmp);
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
		tmpArr->AddKeysTo(outArr);
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

UIntOS Map::CIPLayer::GetObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Double mapRate, Int32 x1, Int32 y1, Int32 x2, Int32 y2, Bool keepEmpty)
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

	IntOS textSize;
	IntOS i;
	IntOS j;
	IntOS k;
	UIntOS l;
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
	if (nameArr.IsNotNull())
	{
		Data::Int64FastMapObj<WChar*> *tmpArr;
		NEW_CLASS(tmpArr, Data::Int64FastMapObj<WChar*>());
		nameArr.SetNoCheck((NameArray*)tmpArr);
		UTF8Char fileName[256];
		UnsafeArray<UTF8Char> sptr;
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
		
		while (k < (IntOS)this->nblks)
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
					cis->Read(Data::ByteArray(buff, 5));
					if (tmpArr->Get(*(Int32*)buff) == 0)
					{
						if (buff[4])
						{
							strTmp = MemAlloc(WChar, (buff[4] >> 1) + 1);
							cis->Read(Data::ByteArray((UInt8*)strTmp, buff[4]));
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
		tmpArr->AddKeysTo(outArr);
	}
	else
	{
		Data::ArrayListInt32 *tmpArr;
		NEW_CLASS(tmpArr, Data::ArrayListInt32());
		while (k < (IntOS)this->nblks)
		{
			if (this->blks[k].xblk > rightBlk)
				break;

			if ((this->blks[k].yblk >= topBlk) && (this->blks[k].yblk <= bottomBlk) && (this->blks[k].xblk >= leftBlk))
			{
				tmpArr->AddRange(this->blks[k].ids, this->blks[k].objCnt);
			}
			k++;
		}
		UIntOS arrSize = tmpArr->GetCount();
		Int64 lastId;
		UnsafeArray<Int32> arr = tmpArr->Arr();
		ArtificialQuickSort_SortInt32(arr.Ptr(), 0, arrSize - 1);
		if (tmpArr->GetCount() > 0)
		{
			lastId = -1;
			outArr->EnsureCapacity(arrSize);
			k = 0;
			while (k < (IntOS)arrSize)
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
		if (nameArr.IsNotNull())
		{
			sb.AppendC(UTF8STRC(" (has nameArr)"));
		}
		sb.AppendC(UTF8STRC("\r\n"));
		IO::Console::PrintStrO(sb.ToString());
	}
	return l;
}

UIntOS Map::CIPLayer::GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Double x1, Double y1, Double x2, Double y2, Bool keepEmpty)
{
	return GetObjectIds(outArr, nameArr, 200000.0, Double2Int32(x1 * 200000), Double2Int32(y1 * 200000), Double2Int32(x2 * 200000), Double2Int32(y2 * 200000), keepEmpty);
}

Int64 Map::CIPLayer::GetObjectIdMax()
{
	return this->maxId;
}

void Map::CIPLayer::ReleaseNameArr(Optional<NameArray> nameArr)
{
	if (nameArr.NotNull())
	{
		Data::Int32FastMapObj<WChar*> *tmpMap = (Data::Int32FastMapObj<WChar*>*)nameArr.OrNull();
		UIntOS i = tmpMap->GetCount();
		while (i-- > 0)
		{
			MemFree(tmpMap->GetItem(i));
		}
		DEL_CLASS(tmpMap);
	}
}

Bool Map::CIPLayer::GetString(NN<Text::StringBuilderUTF8> sb, Optional<NameArray> nameArr, Int64 id, UIntOS strIndex)
{
	Data::Int32FastMapObj<WChar*> *tmpMap = (Data::Int32FastMapObj<WChar*>*)nameArr.OrNull();
	if (strIndex != 0)
	{
		return false;
	}
	WChar *s = tmpMap->Get((Int32)id);
	if (s)
	{
		sb->AppendW(s);
		return true;
	}
	else
	{
		return false;
	}
}

UIntOS Map::CIPLayer::GetColumnCnt()
{
	return 1;
}

UnsafeArrayOpt<UTF8Char> Map::CIPLayer::GetColumnName(UnsafeArray<UTF8Char> buff, UIntOS colIndex)
{
	if (colIndex == 0)
	{
		return Text::StrConcatC(buff, UTF8STRC("NAME"));
	}
	else
	{
		return nullptr;
	}
}


DB::DBUtil::ColType Map::CIPLayer::GetColumnType(UIntOS colIndex, OptOut<UIntOS> colSize)
{
	if (colIndex == 0)
	{
		colSize.Set(this->maxTextSize);
		return DB::DBUtil::CT_VarUTF16Char;
	}
	else
	{
		colSize.Set(0);
		return DB::DBUtil::CT_Unknown;
	}
}

Bool Map::CIPLayer::GetColumnDef(UIntOS colIndex, DB::ColDef *colDef)
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
	colDef->SetAutoInc(DB::ColDef::AutoIncType::None, 0, 1);
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
		IntOS i = this->nblks;
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

Optional<Map::CIPLayer::CIPFileObject> Map::CIPLayer::GetFileObject(NN<GetObjectSess> session, Int32 id)
{
	NN<IO::SeekableStream> cip = NN<IO::SeekableStream>::ConvertFrom(session);
	Optional<Map::CIPLayer::CIPFileObject> obj;
	NN<Map::CIPLayer::CIPFileObject> nnobj;
	Int32 buff[2];

	obj = this->lastObjs->Get(id);
	if (obj.NotNull())
	{
		this->lastObjs->Put(id, nullptr);
		this->currObjs->Put(id, obj);
		return obj;
	}
	obj = this->currObjs->Get(id);
	if (obj.NotNull())
		return obj;

	Int32 ofst = this->ofsts[2 + (id << 1)];
	cip->SeekFromBeginning(ofst);
	if (cip->Read(Data::ByteArray((UInt8*)buff, 8)) != 8)
	{
		return nullptr;
	}
	nnobj = MemAllocNN(Map::CIPLayer::CIPFileObject);
	nnobj->id = buff[0];
	nnobj->nParts = buff[1];
	if (buff[1] > 0)
	{
		UnsafeArray<UInt32> parts;
		nnobj->parts = parts = MemAlloc(UInt32, buff[1]);
		cip->Read(Data::ByteArray(UnsafeArray<UInt8>::ConvertFrom(parts), sizeof(Int32) * buff[1]));
	}
	else
	{
		nnobj->parts = nullptr;
	}
	cip->Read(Data::ByteArray((UInt8*)&nnobj->nPoints, 4));
	nnobj->points = MemAlloc(Int32, nnobj->nPoints * 2);
	cip->Read(Data::ByteArray(UnsafeArray<UInt8>::ConvertFrom(nnobj->points), nnobj->nPoints * 8));
	this->currObjs->Put(id, nnobj);
	return nnobj;
}

void Map::CIPLayer::ReleaseFileObjs(NN<Data::Int32FastMapObj<Optional<Map::CIPLayer::CIPFileObject>>> objs)
{
	Optional<Map::CIPLayer::CIPFileObject> obj;
	NN<Map::CIPLayer::CIPFileObject> nnobj;
	UnsafeArray<UInt32> parts;
	UIntOS i = objs->GetCount();
	while (i-- > 0)
	{
		obj = objs->GetItem(i);
		if (obj.SetTo(nnobj))
		{
			if (nnobj->parts.SetTo(parts))
			{
				MemFreeArr(parts);
			}
			MemFreeArr(nnobj->points);
			MemFreeNN(nnobj);
		}
	}
	objs->Clear();
}

NN<Map::GetObjectSess> Map::CIPLayer::BeginGetObject()
{
	UTF8Char fileName[256];
	UnsafeArray<UTF8Char> sptr;
	NN<IO::SeekableStream> cip;
	sptr = this->layerName->ConcatTo(fileName);
	sptr = Text::StrConcatC(sptr, UTF8STRC(".cip"));
	this->mut.Lock();
	NEW_CLASSNN(cip, IO::FileStream(CSTRP(fileName, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	//NEW_CLASS(cip, IO::FileViewStream(fileName));
	return NN<Map::GetObjectSess>::ConvertFrom(cip);
}

void Map::CIPLayer::EndGetObject(NN<GetObjectSess> session)
{
	NN<IO::SeekableStream> cip = NN<IO::SeekableStream>::ConvertFrom(session);
	cip.Delete();
	NN<Data::Int32FastMapObj<Optional<CIPFileObject>>> tmpObjs;
	this->ReleaseFileObjs(this->lastObjs);
	tmpObjs = this->lastObjs;
	this->lastObjs = this->currObjs;
	this->currObjs = tmpObjs;
	this->mut.Unlock();
}

Optional<Math::Geometry::Vector2D> Map::CIPLayer::GetNewVectorById(NN<GetObjectSess> session, Int64 id)
{
	NN<Map::CIPLayer::CIPFileObject> fobj;
	if (!this->GetFileObject(session, (Int32)id).SetTo(fobj))
	{
		return nullptr;
	}
	NN<Math::Geometry::Vector2D> vec;
	Double r = 1 / 200000.0;
	if (this->lyrType == Map::DRAW_LAYER_POINT || this->lyrType == Map::DRAW_LAYER_POINT3D)
	{
		Math::Geometry::Point *pt;
		NEW_CLASS(pt, Math::Geometry::Point(4326, fobj->points[0] * r, fobj->points[1] * r));
		return pt;
	}
	else if (this->lyrType == Map::DRAW_LAYER_POLYLINE3D || this->lyrType == Map::DRAW_LAYER_POLYLINE)
	{
		NEW_CLASSNN(vec, Math::Geometry::Polyline(4326));//, (UIntOS)fobj->nParts, (UIntOS)fobj->nPoints, false, false));
		NN<Math::Geometry::Polyline>::ConvertFrom(vec)->AddFrom0PtOfst(fobj->parts, fobj->nParts, fobj->points, fobj->nPoints, nullptr, nullptr);
	}
	else if (this->lyrType == Map::DRAW_LAYER_POLYGON)
	{
		NEW_CLASSNN(vec, Math::Geometry::Polygon(4326));//, (UIntOS)fobj->nParts, (UIntOS)fobj->nPoints, false, false));
	}
	else
	{
		return nullptr;
	}
	if (fobj->parts)
	{
		UIntOS nPtOfst;
		UInt32 *ptOfstArr = vec->GetPtOfstList(&nPtOfst);
		MemCopyNO(ptOfstArr, fobj->parts, sizeof(UInt32) * fobj->nParts);
	}
	UIntOS nPoint;
	Math::Coord2DDbl *pointArr = vec->GetPointList(&nPoint);
	UIntOS i = nPoint;
	while (i-- > 0)
	{
		pointArr[i] = Math::Coord2DDbl(fobj->points[i * 2] * r, fobj->points[i * 2 + 1] * r);
	}
	return vec;
}

Map::MapDrawLayer::ObjectClass Map::CIPLayer::GetObjectClass()
{
	return Map::MapDrawLayer::OC_CIP_LAYER;
}
