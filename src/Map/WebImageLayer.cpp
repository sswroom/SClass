#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "DB/ColDef.h"
#include "IO/StmData/BufferedStreamData.h"
#include "Map/WebImageLayer.h"
#include "Math/Geometry/VectorImage.h"
#include "Media/ImageList.h"
#include "Media/StaticImage.h"
#include "Sync/MutexUsage.h"
#include "Sync/RWMutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

Map::WebImageLayer::ImageStat::~ImageStat()
{

}

OSInt Map::WebImageLayer::ImageStat::CompareTo(Data::IComparable *obj) const
{
	Map::WebImageLayer::ImageStat *stat = (Map::WebImageLayer::ImageStat*)obj;
	if (this->zIndex > stat->zIndex)
	{
		return 1;
	}
	else if (this->zIndex < stat->zIndex)
	{
		return -1;
	}
	else if (this->id > stat->id)
	{
		return 1;
	}
	else if (this->id < stat->id)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

OSInt Map::WebImageLayer::GetImageStatIndex(Int32 id)
{
	OSInt i;
	OSInt j;
	OSInt k;
	Map::WebImageLayer::ImageStat *stat;
	i = 0;
	j = (OSInt)this->loadedList.GetCount() - 1;
	while (i <= j)
	{
		k = (i + j) >> 1;
		stat = this->loadedList.GetItem((UOSInt)k);
		if (stat->id > id)
		{
			j = k - 1;
		}
		else if (stat->id < id)
		{
			i = k + 1;
		}
		else
		{
			return k;
		}
	}
	return ~i;
}

Map::WebImageLayer::ImageStat *Map::WebImageLayer::GetImageStat(Int32 id)
{
	OSInt ind;
	Map::WebImageLayer::ImageStat *stat = 0;
	Sync::RWMutexUsage mutUsage(this->loadedMut, false);
	ind = this->GetImageStatIndex(id);
	if (ind >= 0)
	{
		stat = this->loadedList.GetItem((UOSInt)ind);
	}
	return stat;
}

void Map::WebImageLayer::LoadImage(Map::WebImageLayer::ImageStat *stat)
{
#if defined(VERBOSE)
	printf("WebImageLayer: loading %s\r\n", stat->url->v);
#endif
	stat->data = this->browser->GetData(stat->url->ToCString(), false, 0);
	NotNullPtr<IO::StreamData> fd;
	if (!fd.Set(stat->data))
	{
		SDEL_STRING(stat->name);
		stat->url->Release();
		DEL_CLASS(stat);
	}
	else if (fd->IsLoading())
	{
		this->loadingList.Add(stat);
		this->loadEvt.Set();
	}
	else
	{
		IO::ParsedObject *pobj;
		{
			IO::StmData::BufferedStreamData buffFd(fd);
			pobj = this->parsers->ParseFile(buffFd);
		}
		if (pobj == 0 || pobj->GetParserType() != IO::ParserType::ImageList)
		{
			SDEL_STRING(stat->name);
			stat->url->Release();
			DEL_CLASS(stat);
			if (pobj)
			{
				DEL_CLASS(pobj);
			}
		}
		else
		{
			Media::ImageList *imgList = (Media::ImageList*)pobj;
			imgList->ToStaticImage(0);
			Media::StaticImage *simg = (Media::StaticImage*)imgList->GetImage(0, 0);
			if (stat->alpha > 0 && stat->alpha != 1)
			{
				simg->MultiplyAlpha(stat->alpha);
			}
			NEW_CLASS(stat->simg, Media::SharedImage(imgList, true));
			Sync::RWMutexUsage loadedMutUsage(this->loadedMut, true);
			this->loadedList.Insert((UOSInt)~this->GetImageStatIndex(stat->id), stat);
			loadedMutUsage.EndUse();

			Sync::MutexUsage mutUsage(this->updMut);
			UOSInt i = this->updHdlrs.GetCount();
			while (i-- > 0)
			{
				this->updHdlrs.GetItem(i)(this->updObjs.GetItem(i));
			}
			mutUsage.EndUse();
		}
	}
}

UInt32 __stdcall Map::WebImageLayer::LoadThread(void *userObj)
{
	Map::WebImageLayer *me = (Map::WebImageLayer*)userObj;
	ImageStat *stat;
	UOSInt i;
	IO::ParsedObject *pobj;
	NotNullPtr<IO::StreamData> fd;

	me->threadRunning = true;
	while (!me->threadToStop)
	{
		Sync::MutexUsage mutUsage(me->loadingMut);
		i = me->loadingList.GetCount();
		while (i-- > 0)
		{
			stat = me->loadingList.GetItem(i);
			if (fd.Set(stat->data) && !stat->data->IsLoading())
			{
				me->loadingList.RemoveAt(i);
				pobj = me->parsers->ParseFile(fd);
				fd.Delete();
				if (pobj == 0 || pobj->GetParserType() != IO::ParserType::ImageList)
				{
					SDEL_STRING(stat->name);
					stat->url->Release();
					DEL_CLASS(stat);
					if (pobj)
					{
						DEL_CLASS(pobj);
					}
				}
				else
				{
					Media::ImageList *imgList = (Media::ImageList*)pobj;
					imgList->ToStaticImage(0);
					Media::StaticImage *simg = (Media::StaticImage*)imgList->GetImage(0, 0);
					if (stat->alpha > 0 && stat->alpha != 1)
					{
						simg->MultiplyAlpha(stat->alpha);
					}
					NEW_CLASS(stat->simg, Media::SharedImage(imgList, true));
					Sync::RWMutexUsage loadedMutUsage(me->loadedMut, true);
					me->loadedList.Insert((UOSInt)~me->GetImageStatIndex(stat->id), stat);
					loadedMutUsage.EndUse();

					Sync::MutexUsage mutUsage(me->updMut);
					UOSInt j = me->updHdlrs.GetCount();
					while (j-- > 0)
					{
						me->updHdlrs.GetItem(j)(me->updObjs.GetItem(j));
					}
					mutUsage.EndUse();
				}
			}
		}

		while (me->loadingList.GetCount() < 2)
		{
			if (me->pendingList.GetCount() == 0)
				break;
			me->LoadImage(me->pendingList.RemoveAt(0));
		}
		mutUsage.EndUse();
		me->loadEvt.Wait(100);
	}

	me->threadRunning = false;
	return 0;
}

Map::WebImageLayer::WebImageLayer(Net::WebBrowser *browser, Parser::ParserList *parsers, Text::CStringNN sourceName, NotNullPtr<Math::CoordinateSystem> csys, Text::CString layerName) : Map::MapDrawLayer(sourceName, 0, layerName, csys)
{
	this->browser = browser;
	this->parsers = parsers;
	this->nextId = 0;
	this->boundsExists = false;
	this->min = Math::Coord2DDbl(0, 0);
	this->max = Math::Coord2DDbl(0, 0);
	this->minTime = 0;
	this->maxTime = 0;
	this->currTime = 0;
	this->threadRunning = false;
	this->threadToStop = false;
	Sync::ThreadUtil::Create(LoadThread, this);
}

Map::WebImageLayer::~WebImageLayer()
{
	UOSInt i;
	ImageStat *stat;
	Sync::MutexUsage mutUsage(this->updMut);
	this->updHdlrs.Clear();
	this->updObjs.Clear();
	mutUsage.EndUse();

	this->threadToStop = true;
	this->loadEvt.Set();
	while (this->threadRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}

	i = this->pendingList.GetCount();
	while (i-- > 0)
	{
		stat = this->pendingList.RemoveAt(i);
		SDEL_STRING(stat->name);
		stat->url->Release();
		DEL_CLASS(stat);
	}

	i = this->loadingList.GetCount();
	while (i-- > 0)
	{
		stat = this->loadingList.RemoveAt(i);
		SDEL_STRING(stat->name);
		stat->url->Release();
		if (stat->data)
		{
			DEL_CLASS(stat->data);
		}
		DEL_CLASS(stat);
	}

	i = this->loadedList.GetCount();
	while (i-- > 0)
	{
		stat = this->loadedList.RemoveAt(i);
		SDEL_STRING(stat->name);
		stat->url->Release();
		if (stat->simg)
		{
			DEL_CLASS(stat->simg);
		}
		DEL_CLASS(stat);
	}
}

void Map::WebImageLayer::SetCurrTimeTS(Int64 timeStamp)
{
	Int64 oldTime;
	Bool oldValid;
	Bool currValid;
	Bool changed = false;
	UOSInt i;
	ImageStat *stat;
	oldTime = this->currTime;
	this->currTime = timeStamp;

	Sync::RWMutexUsage loadedMutUsage(this->loadedMut, false);
	i = this->loadedList.GetCount();
	while (i-- > 0)
	{
		stat = this->loadedList.GetItem(i);
		oldValid = true;
		currValid = true;
		if (stat->timeStart != 0)
		{
			if (oldTime < stat->timeStart)
				oldValid = false;
			if (timeStamp < stat->timeStart)
				currValid = false;
		}
		if (stat->timeEnd != 0)
		{
			if (oldTime > stat->timeEnd)
				oldValid = false;
			if (timeStamp > stat->timeEnd)
				currValid = false;
		}
		if (oldValid != currValid)
		{
			changed = true;
			break;
		}
	}
	loadedMutUsage.EndUse();

	if (changed)
	{
		Sync::MutexUsage mutUsage(this->updMut);
		i = this->updHdlrs.GetCount();
		while (i-- > 0)
		{
			this->updHdlrs.GetItem(i)(this->updObjs.GetItem(i));
		}
		mutUsage.EndUse();
	}
}

Int64 Map::WebImageLayer::GetTimeStartTS() const
{
	return this->minTime;
}

Int64 Map::WebImageLayer::GetTimeEndTS() const
{
	return this->maxTime;
}

Map::DrawLayerType Map::WebImageLayer::GetLayerType() const
{
	return Map::DRAW_LAYER_IMAGE;
}

UOSInt Map::WebImageLayer::GetAllObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr)
{
	UOSInt retCnt = 0;
	UOSInt i;
	ImageStat *stat;
	ImageStat **imgArr;
	Data::ArrayList<ImageStat *> imgList;
	Sync::RWMutexUsage loadedMutUsage(this->loadedMut, false);
	imgList.AddAll(this->loadedList);
	loadedMutUsage.EndUse();

	imgArr = imgList.GetPtr(retCnt);
	ArtificialQuickSort_SortCmpO((Data::IComparable**)imgArr, 0, (OSInt)retCnt - 1);

	i = 0;
	while (i < retCnt)
	{
		stat = imgList.GetItem(i);
		outArr->Add(stat->id);
		i++;
	}
	return retCnt;
}

UOSInt Map::WebImageLayer::GetObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, rect.ToDouble() / mapRate, keepEmpty);
}

UOSInt Map::WebImageLayer::GetObjectIdsMapXY(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	UOSInt retCnt = 0;
	ImageStat *stat;
	ImageStat **imgArr;
	Data::ArrayList<ImageStat *> imgList;
	UOSInt i;
	Bool valid;

	rect = rect.Reorder();

	Sync::RWMutexUsage loadedMutUsage(this->loadedMut, false);
	i = 0;
	retCnt = this->loadedList.GetCount();
	while (i < retCnt)
	{
		stat = this->loadedList.GetItem(i);
		valid = true;
		if (stat->isScreen)
		{
		}
		else if (stat->x1 < rect.br.x && stat->x2 > rect.tl.x && stat->y1 < rect.br.y && stat->y2 > rect.tl.y)
		{
		}
		else
		{
			valid = false;
		}
		if (valid)
		{
			if (currTime == 0)
			{
				if (stat->timeStart != 0 || stat->timeEnd != 0)
				{
					valid = false;
				}
			}
			else
			{
				if (stat->timeStart != 0)
				{
					if (currTime < stat->timeStart)
					{
						valid = false;
					}
				}
				if (stat->timeEnd != 0)
				{
					if (currTime > stat->timeEnd)
					{
						valid = false;
					}
				}
			}
		}
		if (valid)
		{
			imgList.Add(stat);
		}

		i++;
	}
	loadedMutUsage.EndUse();

	imgArr = imgList.GetPtr(retCnt);
	ArtificialQuickSort_SortCmpO((Data::IComparable**)imgArr, 0, (OSInt)retCnt - 1);

	i = 0;
	while (i < retCnt)
	{
		stat = imgList.GetItem(i);
		outArr->Add(stat->id);
		i++;
	}
	return retCnt;
}

Int64 Map::WebImageLayer::GetObjectIdMax() const
{
	Int64 maxId = -1;
	Sync::RWMutexUsage loadedMutUsage(this->loadedMut, false);
	Map::WebImageLayer::ImageStat *stat = this->loadedList.GetItem(this->loadedList.GetCount() - 1);
	if (stat)
	{
		maxId = stat->id;
	}
	return maxId;
}

void Map::WebImageLayer::ReleaseNameArr(NameArray *nameArr)
{
}

Bool Map::WebImageLayer::GetString(NotNullPtr<Text::StringBuilderUTF8> sb, NameArray *nameArr, Int64 id, UOSInt colIndex)
{
	if (colIndex != 0)
		return false;
	Map::WebImageLayer::ImageStat *stat = GetImageStat((Int32)id);
	if (stat)
	{
		sb->Append(stat->name);
		return true;
	}
	else
	{
		return false;
	}
}

UOSInt Map::WebImageLayer::GetColumnCnt() const
{
	return 1;
}

UTF8Char *Map::WebImageLayer::GetColumnName(UTF8Char *buff, UOSInt colIndex)
{
	if (colIndex > 0)
		return 0;
	return Text::StrConcatC(buff, UTF8STRC("Name"));
}

DB::DBUtil::ColType Map::WebImageLayer::GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize)
{
	if (colIndex == 0)
	{
		colSize.Set(256);
		return DB::DBUtil::CT_VarUTF8Char;
	}
	return DB::DBUtil::CT_Unknown;
}

Bool Map::WebImageLayer::GetColumnDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef)
{
	if (colIndex == 0)
	{
		colDef->SetColName(CSTR("Name"));
		colDef->SetColType(DB::DBUtil::CT_VarUTF8Char);
		colDef->SetColSize(256);
		colDef->SetDefVal(CSTR_NULL);
		colDef->SetNotNull(false);
		return true;
	}
	return false;
}

UInt32 Map::WebImageLayer::GetCodePage() const
{
	return 0;
}

Bool Map::WebImageLayer::GetBounds(OutParam<Math::RectAreaDbl> bounds) const
{
	bounds.Set(Math::RectAreaDbl(this->min, this->max));
	return this->min.x != 0 || this->min.y != 0 || this->max.x != 0 || this->max.y != 0;
}

Map::GetObjectSess *Map::WebImageLayer::BeginGetObject()
{
	return 0;
}

void Map::WebImageLayer::EndGetObject(GetObjectSess *session)
{
}

Math::Geometry::Vector2D *Map::WebImageLayer::GetNewVectorById(GetObjectSess *session, Int64 id)
{
	ImageStat *stat = this->GetImageStat((Int32)id);
	if (stat)
	{
		Math::Geometry::VectorImage *img;
		NEW_CLASS(img, Math::Geometry::VectorImage(this->csys->GetSRID(), stat->simg, Math::Coord2DDbl(stat->x1, stat->y1), Math::Coord2DDbl(stat->x2, stat->y2), Math::Coord2DDbl(stat->sizeX, stat->sizeY), stat->isScreen, stat->url.Ptr(), stat->timeStart, stat->timeEnd));
		if (stat->hasAltitude)
		{
			img->SetHeight(stat->altitude);
		}
		if (stat->alpha > 0)
		{
			img->SetSrcAlpha(stat->alpha);
		}
		img->SetZIndex(stat->zIndex);
		return img;
	}
	else
	{
		return 0;
	}
}

Map::MapDrawLayer::ObjectClass Map::WebImageLayer::GetObjectClass() const
{
	return Map::MapDrawLayer::OC_WEB_IMAGE_LAYER;
}

void Map::WebImageLayer::AddUpdatedHandler(UpdatedHandler hdlr, void *obj)
{
	Sync::MutexUsage mutUsage(this->updMut);
	this->updHdlrs.Add(hdlr);
	this->updObjs.Add(obj);
}

void Map::WebImageLayer::RemoveUpdatedHandler(UpdatedHandler hdlr, void *obj)
{
	UOSInt i;
	Sync::MutexUsage mutUsage(this->updMut);
	i = this->updHdlrs.GetCount();
	while (i-- > 0)
	{
		if (this->updHdlrs.GetItem(i) == hdlr && this->updObjs.GetItem(i) == obj)
		{
			this->updHdlrs.RemoveAt(i);
			this->updObjs.RemoveAt(i);
		}
	}
	mutUsage.EndUse();
}

void Map::WebImageLayer::AddImage(Text::CString name, Text::CString url, Int32 zIndex, Double x1, Double y1, Double x2, Double y2, Double sizeX, Double sizeY, Bool isScreen, Int64 timeStart, Int64 timeEnd, Double alpha, Bool hasAltitude, Double altitude)
{
	ImageStat *stat;
	NEW_CLASS(stat, ImageStat());
	stat->id = this->nextId++;
	stat->url = Text::String::New(url);
	stat->simg = 0;
	stat->data = 0;
	if (name.leng > 0)
	{
		stat->name = Text::String::New(name).Ptr();
	}
	else
	{
		stat->name = 0;
	}
	stat->timeStart = timeStart;
	stat->timeEnd = timeEnd;
	if (timeStart != 0)
	{
		if (this->minTime == 0)
			this->minTime = timeStart;
		else if (this->minTime > timeStart)
		{
			this->minTime = timeStart;
		}
	}
	if (timeEnd != 0)
	{
		if (this->maxTime == 0)
			this->maxTime = timeEnd;
		else if (this->maxTime < timeEnd)
		{
			this->maxTime = timeEnd;
		}
	}
	stat->zIndex = zIndex;
	if (isScreen)
	{
		stat->x1 = x1;
		stat->y1 = y1;
		stat->x2 = x2;
		stat->y2 = y2;
		stat->sizeX = sizeX;
		stat->sizeY = sizeY;
	}
	else
	{
		if (x1 > x2)
		{
			stat->x1 = x2;
			stat->x2 = x1;
		}
		else
		{
			stat->x1 = x1;
			stat->x2 = x2;
		}
		if (y1 > y2)
		{
			stat->y1 = y2;
			stat->y2 = y1;
		}
		else
		{
			stat->y1 = y1;
			stat->y2 = y2;
		}
		stat->sizeX = sizeX;
		stat->sizeY = sizeY;

		if (boundsExists)
		{
			if (this->min.x > stat->x1)
			{
				this->min.x = stat->x1;
			}
			if (this->min.y > stat->y1)
			{
				this->min.y = stat->y1;
			}
			if (this->max.x < stat->x2)
			{
				this->max.x = stat->x2;
			}
			if (this->max.y < stat->y2)
			{
				this->max.y = stat->y2;
			}
		}
		else
		{
			boundsExists = true;
			this->min.x = stat->x1;
			this->min.y = stat->y1;
			this->max.x = stat->x2;
			this->max.y = stat->y2;
		}
	}
	stat->isScreen = isScreen;
	stat->alpha = alpha;
	stat->hasAltitude = hasAltitude;
	stat->altitude = altitude;
	Sync::MutexUsage mutUsage(this->loadingMut);
	if (this->loadingList.GetCount() >= 2)
	{
		this->pendingList.Add(stat);
	}
	else
	{
		this->LoadImage(stat);
	}
	mutUsage.EndUse();
}
