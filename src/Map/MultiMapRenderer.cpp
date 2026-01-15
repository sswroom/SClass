#include "Stdafx.h"
#include "Map/MultiMapRenderer.h"

void __stdcall Map::MultiMapRenderer::OnUpdated(AnyType userObj)
{
	NN<Map::MultiMapRenderer> me = userObj.GetNN<Map::MultiMapRenderer>();
	if (!me->updating && me->updHdlr)
	{
		me->updHdlr(me->updObj);
	}
}

Map::MultiMapRenderer::MultiMapRenderer(Int32 bgColor)
{
	this->bgColor = bgColor;
	this->updating = false;
	this->updHdlr = 0;
	this->updObj = 0;
}

Map::MultiMapRenderer::~MultiMapRenderer()
{
	this->updating = true;
	Clear();
}

void Map::MultiMapRenderer::DrawMap(NN<Media::DrawImage> img, NN<Map::MapView> view, OptOut<UInt32> imgDurMS)
{
	UIntOS i;
	UIntOS j;
	NN<Media::DrawBrush> b;
	b = img->NewBrushARGB(this->bgColor);
	img->DrawRect(Math::Coord2DDbl(0, 0), img->GetSize().ToDouble(), 0, b);
	img->DelBrush(b);
	UInt32 totalDur = 0;
	i = 0;
	j = this->renderers.GetCount();
	while (i < j)
	{
		UInt32 thisDur;
		this->renderers.GetItemNoCheck(i)->DrawMap(img, view, thisDur);
		totalDur += thisDur;
		i++;
	}
	imgDurMS.Set(totalDur);
}

void Map::MultiMapRenderer::SetUpdatedHandler(UpdatedHandler updHdlr, AnyType userObj)
{
	this->updHdlr = updHdlr;
	this->updObj = userObj;
}

UIntOS Map::MultiMapRenderer::Add(NN<Map::MapRenderer> renderer)
{
	UIntOS ret = this->renderers.Add(renderer);
	renderer->SetUpdatedHandler(OnUpdated, this);
	if (!this->updating && this->updHdlr)
	{
		this->updHdlr(updObj);
	}
}

void Map::MultiMapRenderer::Clear()
{
	NN<Map::MapRenderer> renderer;
	IntOS i;
	i = this->renderers.GetCount();
	if (i <= 0)
		return;

	while (i-- > 0)
	{
		renderer = this->renderers.GetItemNoCheck(i);
		renderer->SetUpdatedHandler(0, 0);
	}
	this->renderers.Clear();

	if (!this->updating && this->updHdlr)
	{
		this->updHdlr(updObj);
	}
}

UIntOS Map::MultiMapRenderer::GetCount()
{
	return this->renderers.GetCount();
}

NN<Map::MapRenderer> Map::MultiMapRenderer::GetItemNoCheck(UIntOS index)
{
	return this->renderers.GetItemNoCheck(index);
}

Optional<Map::MapRenderer> Map::MultiMapRenderer::GetItem(UIntOS index)
{
	return this->renderers.GetItem(index);
}

void Map::MultiMapRenderer::BeginUpdate()
{
	this->updating = true;
}

void Map::MultiMapRenderer::EndUpdate()
{
	this->updating = false;
	if (this->updHdlr)
	{
		this->updHdlr(updObj);
	}
}
