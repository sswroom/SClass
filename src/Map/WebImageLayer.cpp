#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "DB/ColDef.h"
#include "IO/StmData/BufferedStreamData.h"
#include "Map/WebImageLayer.h"
#include "Math/VectorImage.h"
#include "Media/ImageList.h"
#include "Media/StaticImage.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"

Map::WebImageLayer::ImageStat::~ImageStat()
{

}

OSInt Map::WebImageLayer::ImageStat::CompareTo(Data::IComparable *obj)
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
	j = (OSInt)this->loadedList->GetCount() - 1;
	while (i <= j)
	{
		k = (i + j) >> 1;
		stat = this->loadedList->GetItem((UOSInt)k);
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
	this->loadedMut->LockRead();
	ind = this->GetImageStatIndex(id);
	if (ind >= 0)
	{
		stat = this->loadedList->GetItem((UOSInt)ind);
	}
	this->loadedMut->UnlockRead();
	return stat;
}

void Map::WebImageLayer::LoadImage(Map::WebImageLayer::ImageStat *stat)
{
	stat->data = this->browser->GetData(stat->url->ToCString(), false, 0);
	if (stat->data == 0)
	{
		if (stat->name)
		{
			Text::StrDelNew(stat->name);
		}
		SDEL_STRING(stat->url);
		DEL_CLASS(stat);
	}
	else if (stat->data->IsLoading())
	{
		this->loadingList->Add(stat);
		this->loadEvt->Set();
	}
	else
	{
		IO::ParsedObject *pobj;
		IO::ParserType pt;
		IO::StmData::BufferedStreamData *buffFd;
		NEW_CLASS(buffFd, IO::StmData::BufferedStreamData(stat->data));
		pobj = this->parsers->ParseFile(buffFd, &pt);
		DEL_CLASS(buffFd);
		if (pobj == 0 || pt != IO::ParserType::ImageList)
		{
			if (stat->name)
			{
				Text::StrDelNew(stat->name);
			}
			SDEL_STRING(stat->url);
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
			this->loadedMut->LockWrite();
			this->loadedList->Insert((UOSInt)~this->GetImageStatIndex(stat->id), stat);
			this->loadedMut->UnlockWrite();

			Sync::MutexUsage mutUsage(this->updMut);
			UOSInt i = this->updHdlrs->GetCount();
			while (i-- > 0)
			{
				this->updHdlrs->GetItem(i)(this->updObjs->GetItem(i));
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
	IO::ParserType pt;
	IO::ParsedObject *pobj;

	me->threadRunning = true;
	while (!me->threadToStop)
	{
		Sync::MutexUsage mutUsage(me->loadingMut);
		i = me->loadingList->GetCount();
		while (i-- > 0)
		{
			stat = me->loadingList->GetItem(i);
			if (!stat->data->IsLoading())
			{
				me->loadingList->RemoveAt(i);
				pobj = me->parsers->ParseFile(stat->data, &pt);
				DEL_CLASS(stat->data);
				if (pobj == 0 || pt != IO::ParserType::ImageList)
				{
					if (stat->name)
					{
						Text::StrDelNew(stat->name);
					}
					SDEL_STRING(stat->url);
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
					me->loadedMut->LockWrite();
					me->loadedList->Insert((UOSInt)~me->GetImageStatIndex(stat->id), stat);
					me->loadedMut->UnlockWrite();

					Sync::MutexUsage mutUsage(me->updMut);
					UOSInt j = me->updHdlrs->GetCount();
					while (j-- > 0)
					{
						me->updHdlrs->GetItem(j)(me->updObjs->GetItem(j));
					}
					mutUsage.EndUse();
				}
			}
		}

		while (me->loadingList->GetCount() < 2)
		{
			if (me->pendingList->GetCount() == 0)
				break;
			me->LoadImage(me->pendingList->RemoveAt(0));
		}
		mutUsage.EndUse();
		me->loadEvt->Wait(100);
	}

	me->threadRunning = false;
	return 0;
}

Map::WebImageLayer::WebImageLayer(Net::WebBrowser *browser, Parser::ParserList *parsers, Text::CString sourceName, Math::CoordinateSystem *csys, Text::CString layerName) : Map::IMapDrawLayer(sourceName, 0, layerName)
{
	this->browser = browser;
	this->parsers = parsers;
	this->csys = csys;
	this->nextId = 0;
	this->boundsExists = false;
	this->minX = 0;
	this->minY = 0;
	this->maxX = 0;
	this->maxY = 0;
	this->minTime = 0;
	this->maxTime = 0;
	this->currTime = 0;
	this->threadRunning = false;
	this->threadToStop = false;
	NEW_CLASS(this->updMut, Sync::Mutex());
	NEW_CLASS(this->updHdlrs, Data::ArrayList<Map::IMapDrawLayer::UpdatedHandler>());
	NEW_CLASS(this->updObjs, Data::ArrayList<void *>());

	NEW_CLASS(this->pendingList, Data::ArrayList<ImageStat *>());
	NEW_CLASS(this->loadingList, Data::ArrayList<ImageStat *>());
	NEW_CLASS(this->loadedList, Data::ArrayList<ImageStat *>());
	NEW_CLASS(loadedMut, Sync::RWMutex());
	NEW_CLASS(loadingMut, Sync::Mutex());
	NEW_CLASS(this->loadEvt, Sync::Event(true));

	Sync::Thread::Create(LoadThread, this);
}

Map::WebImageLayer::~WebImageLayer()
{
	UOSInt i;
	ImageStat *stat;
	Sync::MutexUsage mutUsage(this->updMut);
	this->updHdlrs->Clear();
	this->updObjs->Clear();
	mutUsage.EndUse();

	this->threadToStop = true;
	this->loadEvt->Set();
	while (this->threadRunning)
	{
		Sync::Thread::Sleep(10);
	}

	i = this->pendingList->GetCount();
	while (i-- > 0)
	{
		stat = this->pendingList->RemoveAt(i);
		if (stat->name)
		{
			Text::StrDelNew(stat->name);
		}
		SDEL_STRING(stat->url);
		DEL_CLASS(stat);
	}
	DEL_CLASS(this->pendingList);

	i = this->loadingList->GetCount();
	while (i-- > 0)
	{
		stat = this->loadingList->RemoveAt(i);
		if (stat->name)
		{
			Text::StrDelNew(stat->name);
		}
		SDEL_STRING(stat->url);
		if (stat->data)
		{
			DEL_CLASS(stat->data);
		}
		DEL_CLASS(stat);
	}
	DEL_CLASS(this->loadingList);

	i = this->loadedList->GetCount();
	while (i-- > 0)
	{
		stat = this->loadedList->RemoveAt(i);
		if (stat->name)
		{
			Text::StrDelNew(stat->name);
		}
		SDEL_STRING(stat->url);
		if (stat->simg)
		{
			DEL_CLASS(stat->simg);
		}
		DEL_CLASS(stat);
	}
	DEL_CLASS(this->loadedList);

	DEL_CLASS(this->loadEvt);
	DEL_CLASS(this->loadedMut);
	DEL_CLASS(this->loadingMut);
	DEL_CLASS(this->updMut);
	DEL_CLASS(this->updHdlrs);
	DEL_CLASS(this->updObjs);
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

	this->loadedMut->LockRead();
	i = this->loadedList->GetCount();
	while (i-- > 0)
	{
		stat = this->loadedList->GetItem(i);
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
	this->loadedMut->UnlockRead();

	if (changed)
	{
		Sync::MutexUsage mutUsage(this->updMut);
		i = this->updHdlrs->GetCount();
		while (i-- > 0)
		{
			this->updHdlrs->GetItem(i)(this->updObjs->GetItem(i));
		}
		mutUsage.EndUse();
	}
}

Int64 Map::WebImageLayer::GetTimeStartTS()
{
	return this->minTime;
}

Int64 Map::WebImageLayer::GetTimeEndTS()
{
	return this->maxTime;
}

Map::DrawLayerType Map::WebImageLayer::GetLayerType()
{
	return Map::DRAW_LAYER_IMAGE;
}

UOSInt Map::WebImageLayer::GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr)
{
	UOSInt retCnt = 0;
	UOSInt i;
	ImageStat *stat;
	ImageStat **imgArr;
	Data::ArrayList<ImageStat *> *imgList;
	NEW_CLASS(imgList, Data::ArrayList<ImageStat*>());
	this->loadedMut->LockRead();
	imgList->AddAll(this->loadedList);
	this->loadedMut->UnlockRead();

	imgArr = imgList->GetArray(&retCnt);
	ArtificialQuickSort_SortCmpO((Data::IComparable**)imgArr, 0, (OSInt)retCnt - 1);

	i = 0;
	while (i < retCnt)
	{
		stat = imgList->GetItem(i);
		outArr->Add(stat->id);
		i++;
	}
	DEL_CLASS(imgList);
	return retCnt;
}

UOSInt Map::WebImageLayer::GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Int32 x1, Int32 y1, Int32 x2, Int32 y2, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, x1 / mapRate, y1 / mapRate, x2 / mapRate, y2 / mapRate, keepEmpty);
}

UOSInt Map::WebImageLayer::GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Double x1, Double y1, Double x2, Double y2, Bool keepEmpty)
{
	UOSInt retCnt = 0;
	ImageStat *stat;
	ImageStat **imgArr;
	Data::ArrayList<ImageStat *> *imgList;
	UOSInt i;
	Bool valid;
	NEW_CLASS(imgList, Data::ArrayList<ImageStat*>());

	Double minX;
	Double minY;
	Double maxX;
	Double maxY;
	if (x1 > x2)
	{
		minX = x2;
		maxX = x1;
	}
	else
	{
		minX = x1;
		maxX = x2;
	}
	if (y1 > y2)
	{
		minY = y2;
		maxY = y1;
	}
	else
	{
		minY = y1;
		maxY = y2;
	}

	this->loadedMut->LockRead();
	i = 0;
	retCnt = this->loadedList->GetCount();
	while (i < retCnt)
	{
		stat = this->loadedList->GetItem(i);
		valid = true;
		if (stat->isScreen)
		{
		}
		else if (stat->x1 < maxX && stat->x2 > minX && stat->y1 < maxY && stat->y2 > minY)
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
			imgList->Add(stat);
		}

		i++;
	}
	this->loadedMut->UnlockRead();

	imgArr = imgList->GetArray(&retCnt);
	ArtificialQuickSort_SortCmpO((Data::IComparable**)imgArr, 0, (OSInt)retCnt - 1);

	i = 0;
	while (i < retCnt)
	{
		stat = imgList->GetItem(i);
		outArr->Add(stat->id);
		i++;
	}
	DEL_CLASS(imgList);
	return retCnt;
}

Int64 Map::WebImageLayer::GetObjectIdMax()
{
	Int64 maxId = -1;
	this->loadedMut->LockRead();
	Map::WebImageLayer::ImageStat *stat = this->loadedList->GetItem(this->loadedList->GetCount() - 1);
	if (stat)
	{
		maxId = stat->id;
	}
	this->loadedMut->UnlockRead();
	return maxId;
}

void Map::WebImageLayer::ReleaseNameArr(void *nameArr)
{
}

UTF8Char *Map::WebImageLayer::GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt colIndex)
{
	if (colIndex != 0)
		return 0;
	Map::WebImageLayer::ImageStat *stat = GetImageStat((Int32)id);
	if (stat)
	{
		return Text::StrConcatS(buff, stat->name, buffSize);
	}
	else
	{
		return 0;
	}
}

UOSInt Map::WebImageLayer::GetColumnCnt()
{
	return 1;
}

UTF8Char *Map::WebImageLayer::GetColumnName(UTF8Char *buff, UOSInt colIndex)
{
	if (colIndex > 0)
		return 0;
	return Text::StrConcatC(buff, UTF8STRC("Name"));
}

DB::DBUtil::ColType Map::WebImageLayer::GetColumnType(UOSInt colIndex, UOSInt *colSize)
{
	if (colIndex == 0)
	{
		if (colSize)
		{
			*colSize = 256;
		}
		return DB::DBUtil::CT_VarChar;
	}
	return DB::DBUtil::CT_Unknown;
}

Bool Map::WebImageLayer::GetColumnDef(UOSInt colIndex, DB::ColDef *colDef)
{
	if (colIndex == 0)
	{
		colDef->SetColName(CSTR("Name"));
		colDef->SetColType(DB::DBUtil::CT_VarChar);
		colDef->SetColSize(256);
		colDef->SetDefVal(CSTR_NULL);
		colDef->SetNotNull(false);
		return true;
	}
	return false;
}

UInt32 Map::WebImageLayer::GetCodePage()
{
	return 0;
}

Bool Map::WebImageLayer::GetBoundsDbl(Double *minX, Double *minY, Double *maxX, Double *maxY)
{
	*minX = this->minX;
	*minY = this->minY;
	*maxX = this->maxX;
	*maxY = this->maxY;
	return this->minX != 0 || this->minY != 0 || this->maxX != 0 || this->maxY != 0;
}

void *Map::WebImageLayer::BeginGetObject()
{
	return 0;
}

void Map::WebImageLayer::EndGetObject(void *session)
{
}

Map::DrawObjectL *Map::WebImageLayer::GetNewObjectById(void *session, Int64 id)
{
	return 0;
}

Math::Vector2D *Map::WebImageLayer::GetNewVectorById(void *session, Int64 id)
{
	ImageStat *stat = this->GetImageStat((Int32)id);
	if (stat)
	{
		Math::VectorImage *img;
		NEW_CLASS(img, Math::VectorImage(this->csys->GetSRID(), stat->simg, stat->x1, stat->y1, stat->x2, stat->y2, stat->sizeX, stat->sizeY, stat->isScreen, stat->url, stat->timeStart, stat->timeEnd));
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

void Map::WebImageLayer::ReleaseObject(void *session, DrawObjectL *obj)
{
}

Map::IMapDrawLayer::ObjectClass Map::WebImageLayer::GetObjectClass()
{
	return Map::IMapDrawLayer::OC_WEB_IMAGE_LAYER;
}

void Map::WebImageLayer::AddUpdatedHandler(UpdatedHandler hdlr, void *obj)
{
	Sync::MutexUsage mutUsage(this->updMut);
	this->updHdlrs->Add(hdlr);
	this->updObjs->Add(obj);
	mutUsage.EndUse();
}

void Map::WebImageLayer::RemoveUpdatedHandler(UpdatedHandler hdlr, void *obj)
{
	UOSInt i;
	Sync::MutexUsage mutUsage(this->updMut);
	i = this->updHdlrs->GetCount();
	while (i-- > 0)
	{
		if (this->updHdlrs->GetItem(i) == hdlr && this->updObjs->GetItem(i) == obj)
		{
			this->updHdlrs->RemoveAt(i);
			this->updObjs->RemoveAt(i);
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
		stat->name = Text::StrCopyNewC(name.v, name.leng);
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
			if (this->minX > stat->x1)
			{
				this->minX = stat->x1;
			}
			if (this->minY > stat->y1)
			{
				this->minY = stat->y1;
			}
			if (this->maxX < stat->x2)
			{
				this->maxX = stat->x2;
			}
			if (this->maxY < stat->y2)
			{
				this->maxY = stat->y2;
			}
		}
		else
		{
			boundsExists = true;
			this->minX = stat->x1;
			this->minY = stat->y1;
			this->maxX = stat->x2;
			this->maxY = stat->y2;
		}
	}
	stat->isScreen = isScreen;
	stat->alpha = alpha;
	stat->hasAltitude = hasAltitude;
	stat->altitude = altitude;
	Sync::MutexUsage mutUsage(this->loadingMut);
	if (this->loadingList->GetCount() >= 2)
	{
		this->pendingList->Add(stat);
	}
	else
	{
		this->LoadImage(stat);
	}
	mutUsage.EndUse();
}
