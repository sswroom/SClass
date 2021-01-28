#include "Stdafx.h"
#include "Map/MultiMapRenderer.h"

void __stdcall Map::MultiMapRenderer::OnUpdated(void *userObj)
{
	Map::MultiMapRenderer *me = (Map::MultiMapRenderer*)userObj;
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
	NEW_CLASS(renderers, Data::ArrayList<Map::MapRenderer*>());
}

Map::MultiMapRenderer::~MultiMapRenderer()
{
	this->updating = true;
	Clear();
	DEL_CLASS(renderers);
}

void Map::MultiMapRenderer::DrawMap(Media::DrawImage *img, Map::MapView *view)
{
	OSInt i;
	OSInt j;
	Media::DrawBrush *b;
	b = img->NewBrushARGB(this->bgColor);
	img->DrawRect(0, 0, img->GetWidth(), img->GetHeight(), 0, b);
	img->DelBrush(b);
	i = 0;
	j = this->renderers->GetCount();
	while (i < j)
	{
		this->renderers->GetItem(i)->DrawMap(img, view);
		i++;
	}
}

void Map::MultiMapRenderer::SetUpdatedHandler(UpdatedHandler updHdlr, void *userObj)
{
	this->updHdlr = updHdlr;
	this->updObj = userObj;
}

void Map::MultiMapRenderer::Add(Map::MapRenderer *renderer)
{
	this->renderers->Add(renderer);
	renderer->SetUpdatedHandler(OnUpdated, this);
	if (!this->updating && this->updHdlr)
	{
		this->updHdlr(updObj);
	}
}

void Map::MultiMapRenderer::Clear()
{
	Map::MapRenderer *renderer;
	OSInt i;
	i = this->renderers->GetCount();
	if (i <= 0)
		return;

	while (i-- > 0)
	{
		renderer = this->renderers->RemoveAt(i);
		renderer->SetUpdatedHandler(0, 0);
	}

	if (!this->updating && this->updHdlr)
	{
		this->updHdlr(updObj);
	}
}

OSInt Map::MultiMapRenderer::GetCount()
{
	return this->renderers->GetCount();
}

Map::MapRenderer *Map::MultiMapRenderer::GetItem(OSInt index)
{
	return this->renderers->GetItem(index);
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
