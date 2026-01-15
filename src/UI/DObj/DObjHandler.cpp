#include "Stdafx.h"
#include "Sync/MutexUsage.h"
#include "UI/DObj/DirectObject.h"
#include "UI/DObj/DObjHandler.h"

void UI::DObj::DObjHandler::OnObjectClear()
{
}

UI::DObj::DObjHandler::DObjHandler(NN<Media::DrawEngine> deng)
{
	this->shown = false;
	this->deng = deng;
	this->moveObj = 0;
	this->downObj = 0;
}

UI::DObj::DObjHandler::~DObjHandler()
{
	this->objList.DeleteAll();
}

void UI::DObj::DObjHandler::ClearObjects()
{
	Sync::MutexUsage mutUsage(this->objMut);
	this->objList.DeleteAll();
	this->shown = false;
	this->downObj = 0;
	this->moveObj = 0;
	mutUsage.EndUse();
	this->OnObjectClear();
}

void UI::DObj::DObjHandler::AddObject(NN<DirectObject> obj)
{
	Sync::MutexUsage mutUsage(this->objMut);
	this->objList.Add(obj);
	this->shown = false;
}

Bool UI::DObj::DObjHandler::Check(NN<Media::DrawImage> dimg)
{
	Bool isChanged = !this->shown;
	Sync::MutexUsage mutUsage(this->objMut);
	Data::ArrayIterator<NN<DirectObject>> it = this->objList.Iterator();
	while (it.HasNext())
	{
		if (it.Next()->IsChanged())
		{
			isChanged = true;
		}
	}
	mutUsage.EndUse();
	if (isChanged)
	{
		this->DrawAll(dimg);
	}
	mutUsage.BeginUse();
	it = this->objList.Iterator();
	while (it.HasNext())
	{
		if (it.Next()->DoEvents())
		{
			return isChanged;
		}
	}
	return isChanged;
}

void UI::DObj::DObjHandler::DrawAll(NN<Media::DrawImage> dimg)
{
	this->shown = true;
	Sync::MutexUsage updMutUsage(this->updMut);
	this->DrawBkg(dimg);

	Sync::MutexUsage mutUsage(this->objMut);
	Data::ArrayIterator<NN<DirectObject>> it = this->objList.Iterator();
	while (it.HasNext())
	{
		it.Next()->DrawObject(dimg);
	}
}

void UI::DObj::DObjHandler::BeginUpdate()
{
	this->updMut.Lock();
}

void UI::DObj::DObjHandler::EndUpdate()
{
	this->updMut.Unlock();
}

void UI::DObj::DObjHandler::OnMouseDown(Math::Coord2D<IntOS> scnPos, UI::GUIControl::MouseButton button)
{
	if (button == UI::GUIControl::MBTN_LEFT)
	{
		UIntOS i;
		NN<DirectObject> obj;
		Sync::MutexUsage mutUsage(this->objMut);
		i = this->objList.GetCount();
		while (i-- > 0)
		{
			if (this->objList.GetItem(i).SetTo(obj) && obj->IsObject(scnPos))
			{
				this->downObj = obj.Ptr();
				this->downObj->OnMouseDown();
				break;
			}
		}
	}
}

void UI::DObj::DObjHandler::OnMouseUp(Math::Coord2D<IntOS> scnPos, UI::GUIControl::MouseButton button)
{
	if (button == UI::GUIControl::MBTN_LEFT)
	{
		Sync::MutexUsage mutUsage(this->objMut);
		if (this->downObj != 0)
		{
			this->downObj->OnMouseUp();
			if (this->downObj->IsObject(scnPos))
			{
				this->downObj->OnMouseClick();
			}
			this->downObj = 0;
		}
	}
}

void UI::DObj::DObjHandler::OnMouseMove(Math::Coord2D<IntOS> scnPos)
{
	DirectObject *mouseObj = 0;
	
	UIntOS i;
	NN<DirectObject> obj;
	Sync::MutexUsage mutUsage(this->objMut);
	i = this->objList.GetCount();
	while (i-- > 0)
	{
		if (this->objList.GetItem(i).SetTo(obj) && obj->IsObject(scnPos))
		{
			mouseObj = obj.Ptr();
			break;
		}
	}
	if (this->moveObj != mouseObj)
	{
		this->moveObj = mouseObj;
		if (this->moveObj != 0)
		{
//			this->pb->Cursor = this->moveObj->GetCursor();
		}
		else
		{
//			this->pb->Cursor = System::Windows::Forms::Cursors::Arrow;
		}
	}
}

void UI::DObj::DObjHandler::SizeChanged(Math::Size2D<UIntOS> size)
{

}
