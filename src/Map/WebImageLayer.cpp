#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "DB/ColDef.h"
#include "IO/StmData/BufferedStreamData.h"
#include "Map/WebImageLayer.h"
#include "Math/Geometry/VectorImage.h"
#include "Media/ImageList.h"
#include "Media/ImagePreviewTool.h"
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

OSInt Map::WebImageLayer::ImageStat::CompareTo(NN<Data::Comparable> obj) const
{
	NN<Map::WebImageLayer::ImageStat> stat = NN<Map::WebImageLayer::ImageStat>::ConvertFrom(obj);
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
	NN<Map::WebImageLayer::ImageStat> stat;
	i = 0;
	j = (OSInt)this->loadedList.GetCount() - 1;
	while (i <= j)
	{
		k = (i + j) >> 1;
		stat = this->loadedList.GetItemNoCheck((UOSInt)k);
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

Optional<Map::WebImageLayer::ImageStat> Map::WebImageLayer::GetImageStat(Int32 id)
{
	OSInt ind;
	Optional<Map::WebImageLayer::ImageStat> stat = 0;
	Sync::RWMutexUsage mutUsage(this->loadedMut, false);
	ind = this->GetImageStatIndex(id);
	if (ind >= 0)
	{
		stat = this->loadedList.GetItem((UOSInt)ind);
	}
	return stat;
}

void Map::WebImageLayer::LoadImage(NN<Map::WebImageLayer::ImageStat> stat)
{
#if defined(VERBOSE)
	printf("WebImageLayer: loading %s\r\n", stat->url->v);
#endif
	stat->data = this->browser->GetData(stat->url->ToCString(), false, 0);
	NN<IO::StreamData> fd;
	if (!stat->data.SetTo(fd))
	{
		OPTSTR_DEL(stat->name);
		stat->url->Release();
		stat.Delete();
	}
	else if (fd->IsLoading())
	{
		this->loadingList.Add(stat);
		this->loadEvt.Set();
	}
	else
	{
		Optional<IO::ParsedObject> pobj;
		NN<IO::ParsedObject> nnpobj;
		{
			IO::StmData::BufferedStreamData buffFd(fd);
			pobj = this->parsers->ParseFile(buffFd);
		}
		if (!pobj.SetTo(nnpobj) || nnpobj->GetParserType() != IO::ParserType::ImageList)
		{
			OPTSTR_DEL(stat->name);
			stat->url->Release();
			stat.Delete();
			pobj.Delete();
		}
		else
		{
			NN<Media::ImageList> imgList = NN<Media::ImageList>::ConvertFrom(nnpobj);
			imgList->ToStaticImage(0);
			NN<Media::StaticImage> simg;
			if (Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(0, 0)).SetTo(simg))
			{
				if (stat->alpha > 0 && stat->alpha != 1)
				{
					simg->MultiplyAlpha(stat->alpha);
				}
				Data::ArrayListNN<Media::StaticImage> prevList;
				Media::ImagePreviewTool::CreatePreviews(imgList, prevList, 640);
				NEW_CLASS(stat->simg, Media::SharedImage(imgList, prevList));
				Sync::RWMutexUsage loadedMutUsage(this->loadedMut, true);
				this->loadedList.Insert((UOSInt)~this->GetImageStatIndex(stat->id), stat);
				loadedMutUsage.EndUse();

				Sync::MutexUsage mutUsage(this->updMut);
				UOSInt i = this->updHdlrs.GetCount();
				while (i-- > 0)
				{
					Data::CallbackStorage<UpdatedHandler> cb = this->updHdlrs.GetItem(i);
					cb.func(cb.userObj);
				}
				mutUsage.EndUse();
			}
		}
	}
}

UInt32 __stdcall Map::WebImageLayer::LoadThread(AnyType userObj)
{
	NN<Map::WebImageLayer> me = userObj.GetNN<Map::WebImageLayer>();
	NN<ImageStat> stat;
	UOSInt i;
	Optional<IO::ParsedObject> pobj;
	NN<IO::ParsedObject> nnpobj;
	NN<IO::StreamData> fd;

	me->threadRunning = true;
	while (!me->threadToStop)
	{
		Sync::MutexUsage mutUsage(me->loadingMut);
		i = me->loadingList.GetCount();
		while (i-- > 0)
		{
			stat = me->loadingList.GetItemNoCheck(i);
			if (stat->data.SetTo(fd) && !fd->IsLoading())
			{
				me->loadingList.RemoveAt(i);
				pobj = me->parsers->ParseFile(fd);
				fd.Delete();
				if (!pobj.SetTo(nnpobj) || nnpobj->GetParserType() != IO::ParserType::ImageList)
				{
					OPTSTR_DEL(stat->name);
					stat->url->Release();
					stat.Delete();
					pobj.Delete();
				}
				else
				{
					NN<Media::ImageList> imgList = NN<Media::ImageList>::ConvertFrom(nnpobj);
					imgList->ToStaticImage(0);
					NN<Media::StaticImage> simg;
					if (Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(0, 0)).SetTo(simg))
					{
						if (stat->alpha > 0 && stat->alpha != 1)
						{
							simg->MultiplyAlpha(stat->alpha);
						}
						Data::ArrayListNN<Media::StaticImage> prevList;
						Media::ImagePreviewTool::CreatePreviews(imgList, prevList, 640);
						NEW_CLASS(stat->simg, Media::SharedImage(imgList, prevList));
						Sync::RWMutexUsage loadedMutUsage(me->loadedMut, true);
						me->loadedList.Insert((UOSInt)~me->GetImageStatIndex(stat->id), stat);
						loadedMutUsage.EndUse();

						Sync::MutexUsage mutUsage(me->updMut);
						UOSInt j = me->updHdlrs.GetCount();
						while (j-- > 0)
						{
							Data::CallbackStorage<UpdatedHandler> cb = me->updHdlrs.GetItem(j);
							cb.func(cb.userObj);
						}
						mutUsage.EndUse();
					}
				}
			}
		}

		while (me->loadingList.GetCount() < 2)
		{
			if (me->pendingList.GetCount() == 0)
				break;
			me->LoadImage(me->pendingList.GetItemNoCheck(0));
			me->pendingList.RemoveAt(0);
		}
		mutUsage.EndUse();
		me->loadEvt.Wait(100);
	}

	me->threadRunning = false;
	return 0;
}

Map::WebImageLayer::WebImageLayer(NN<Net::WebBrowser> browser, NN<Parser::ParserList> parsers, Text::CStringNN sourceName, NN<Math::CoordinateSystem> csys, Text::CString layerName) : Map::MapDrawLayer(sourceName, 0, layerName, csys)
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
	NN<ImageStat> stat;
	Sync::MutexUsage mutUsage(this->updMut);
	this->updHdlrs.Clear();
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
		stat = this->pendingList.GetItemNoCheck(i);
		OPTSTR_DEL(stat->name);
		stat->url->Release();
		stat.Delete();
	}
	this->pendingList.Clear();

	i = this->loadingList.GetCount();
	while (i-- > 0)
	{
		stat = this->loadingList.GetItemNoCheck(i);
		OPTSTR_DEL(stat->name);
		stat->url->Release();
		stat->data.Delete();
		stat.Delete();
	}
	this->loadingList.Clear();

	i = this->loadedList.GetCount();
	while (i-- > 0)
	{
		stat = this->loadedList.GetItemNoCheck(i);
		OPTSTR_DEL(stat->name);
		stat->url->Release();
		if (stat->simg)
		{
			DEL_CLASS(stat->simg);
		}
		stat.Delete();
	}
	this->loadedList.Clear();
}

void Map::WebImageLayer::SetCurrTimeTS(Int64 timeStamp)
{
	Int64 oldTime;
	Bool oldValid;
	Bool currValid;
	Bool changed = false;
	UOSInt i;
	NN<ImageStat> stat;
	oldTime = this->currTime;
	this->currTime = timeStamp;

	Sync::RWMutexUsage loadedMutUsage(this->loadedMut, false);
	i = this->loadedList.GetCount();
	while (i-- > 0)
	{
		stat = this->loadedList.GetItemNoCheck(i);
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
			Data::CallbackStorage<UpdatedHandler> cb = this->updHdlrs.GetItem(i);
			cb.func(cb.userObj);
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

UOSInt Map::WebImageLayer::GetAllObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr)
{
	UOSInt retCnt = 0;
	UOSInt i;
	NN<ImageStat> stat;
	NN<ImageStat> *imgArr;
	Data::ArrayListNN<ImageStat> imgList;
	Sync::RWMutexUsage loadedMutUsage(this->loadedMut, false);
	imgList.AddAll(this->loadedList);
	loadedMutUsage.EndUse();

	imgArr = imgList.GetArr(retCnt).Ptr();
	Data::Sort::ArtificialQuickSort::SortCmpO((NN<Data::Comparable>*)imgArr, 0, (OSInt)retCnt - 1);

	i = 0;
	while (i < retCnt)
	{
		stat = imgList.GetItemNoCheck(i);
		outArr->Add(stat->id);
		i++;
	}
	return retCnt;
}

UOSInt Map::WebImageLayer::GetObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, rect.ToDouble() / mapRate, keepEmpty);
}

UOSInt Map::WebImageLayer::GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	UOSInt retCnt = 0;
	NN<ImageStat> stat;
	NN<ImageStat> *imgArr;
	Data::ArrayListNN<ImageStat> imgList;
	UOSInt i;
	Bool valid;

	rect = rect.Reorder();

	Sync::RWMutexUsage loadedMutUsage(this->loadedMut, false);
	i = 0;
	retCnt = this->loadedList.GetCount();
	while (i < retCnt)
	{
		stat = this->loadedList.GetItemNoCheck(i);
		valid = true;
		if (stat->isScreen)
		{
		}
		else if (stat->x1 < rect.max.x && stat->x2 > rect.min.x && stat->y1 < rect.max.y && stat->y2 > rect.min.y)
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

	imgArr = imgList.GetArr(retCnt).Ptr();
	Data::Sort::ArtificialQuickSort::SortCmpO((NN<Data::Comparable>*)imgArr, 0, (OSInt)retCnt - 1);

	i = 0;
	while (i < retCnt)
	{
		stat = imgList.GetItemNoCheck(i);
		outArr->Add(stat->id);
		i++;
	}
	return retCnt;
}

Int64 Map::WebImageLayer::GetObjectIdMax() const
{
	Int64 maxId = -1;
	Sync::RWMutexUsage loadedMutUsage(this->loadedMut, false);
	NN<Map::WebImageLayer::ImageStat> stat;
	if (this->loadedList.GetItem(this->loadedList.GetCount() - 1).SetTo(stat))
	{
		maxId = stat->id;
	}
	return maxId;
}

void Map::WebImageLayer::ReleaseNameArr(Optional<NameArray> nameArr)
{
}

Bool Map::WebImageLayer::GetString(NN<Text::StringBuilderUTF8> sb, Optional<NameArray> nameArr, Int64 id, UOSInt colIndex)
{
	if (colIndex != 0)
		return false;
	NN<Map::WebImageLayer::ImageStat> stat;
	if (GetImageStat((Int32)id).SetTo(stat))
	{
		sb->AppendOpt(stat->name);
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

UnsafeArrayOpt<UTF8Char> Map::WebImageLayer::GetColumnName(UnsafeArray<UTF8Char> buff, UOSInt colIndex)
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

Bool Map::WebImageLayer::GetColumnDef(UOSInt colIndex, NN<DB::ColDef> colDef)
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

NN<Map::GetObjectSess> Map::WebImageLayer::BeginGetObject()
{
	return NN<GetObjectSess>::ConvertFrom(NN<WebImageLayer>(*this));
}

void Map::WebImageLayer::EndGetObject(NN<GetObjectSess> session)
{
}

Optional<Math::Geometry::Vector2D> Map::WebImageLayer::GetNewVectorById(NN<GetObjectSess> session, Int64 id)
{
	NN<ImageStat> stat;
	NN<Media::SharedImage> shimg;
	if (this->GetImageStat((Int32)id).SetTo(stat) && shimg.Set(stat->simg))
	{
		Math::Geometry::VectorImage *img;
		NEW_CLASS(img, Math::Geometry::VectorImage(this->csys->GetSRID(), shimg, Math::Coord2DDbl(stat->x1, stat->y1), Math::Coord2DDbl(stat->x2, stat->y2), Math::Coord2DDbl(stat->sizeX, stat->sizeY), stat->isScreen, stat->url.Ptr(), stat->timeStart, stat->timeEnd));
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

UOSInt Map::WebImageLayer::GetGeomCol() const
{
	return INVALID_INDEX;
}

Map::MapDrawLayer::ObjectClass Map::WebImageLayer::GetObjectClass() const
{
	return Map::MapDrawLayer::OC_WEB_IMAGE_LAYER;
}

void Map::WebImageLayer::AddUpdatedHandler(UpdatedHandler hdlr, AnyType obj)
{
	Sync::MutexUsage mutUsage(this->updMut);
	this->updHdlrs.Add({hdlr, obj});
}

void Map::WebImageLayer::RemoveUpdatedHandler(UpdatedHandler hdlr, AnyType obj)
{
	UOSInt i;
	Sync::MutexUsage mutUsage(this->updMut);
	i = this->updHdlrs.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<UpdatedHandler> cb = this->updHdlrs.GetItem(i);
		if (cb.func == hdlr && cb.userObj == obj)
		{
			this->updHdlrs.RemoveAt(i);
		}
	}
}

void Map::WebImageLayer::AddImage(Text::CString name, Text::CStringNN url, Int32 zIndex, Double x1, Double y1, Double x2, Double y2, Double sizeX, Double sizeY, Bool isScreen, Int64 timeStartTS, Int64 timeEndTS, Double alpha, Bool hasAltitude, Double altitude)
{
	NN<ImageStat> stat;
	NEW_CLASSNN(stat, ImageStat());
	stat->id = this->nextId++;
	stat->url = Text::String::New(url);
	stat->simg = 0;
	stat->data = 0;
	stat->name = Text::String::NewOrNull(name);
	stat->timeStart = timeStartTS;
	stat->timeEnd = timeEndTS;
	if (timeStartTS != 0)
	{
		if (this->minTime == 0)
			this->minTime = timeStartTS;
		else if (this->minTime > timeStartTS)
		{
			this->minTime = timeStartTS;
		}
	}
	if (timeEndTS != 0)
	{
		if (this->maxTime == 0)
			this->maxTime = timeEndTS;
		else if (this->maxTime < timeEndTS)
		{
			this->maxTime = timeEndTS;
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
