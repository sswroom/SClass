#include "Stdafx.h"
#include "Sync/MutexUsage.h"
#include "UI/DObj/DirectObject.h"
#include "UI/DObj/DObjHandler.h"

void UI::DObj::DObjHandler::OnObjectClear()
{
}

UI::DObj::DObjHandler::DObjHandler(NotNullPtr<Media::DrawEngine> deng)
{
	this->shown = false;
	this->deng = deng;
	this->colorSess = 0;
	this->moveObj = 0;
	this->downObj = 0;
}

UI::DObj::DObjHandler::~DObjHandler()
{
	UOSInt i;
	DirectObject *obj;
	i = this->objList.GetCount();
	while (i-- > 0)
	{
		obj = this->objList.GetItem(i);
		DEL_CLASS(obj);
	}
	this->objList.Clear();
}

void UI::DObj::DObjHandler::SetColorSess(Media::ColorManagerSess *colorSess)
{
	this->colorSess = colorSess;
}

void UI::DObj::DObjHandler::ClearObjects()
{
	UOSInt i;
	DirectObject *obj;
	Sync::MutexUsage mutUsage(&this->objMut);
	i = this->objList.GetCount();
	while (i-- > 0)
	{
		obj = this->objList.GetItem(i);
		DEL_CLASS(obj);
	}
	this->objList.Clear();
	this->shown = false;
	this->downObj = 0;
	this->moveObj = 0;
	mutUsage.EndUse();
	this->OnObjectClear();
}

void UI::DObj::DObjHandler::AddObject(DirectObject *obj)
{
	Sync::MutexUsage mutUsage(&this->objMut);
	this->objList.Add(obj);
	this->shown = false;
}

Bool UI::DObj::DObjHandler::Check(Media::DrawImage *dimg)
{
	Bool isChanged = !this->shown;
	UOSInt i;
	DirectObject *obj;
	Sync::MutexUsage mutUsage(&this->objMut);
	i = this->objList.GetCount();
	while (i-- > 0)
	{
		obj = this->objList.GetItem(i);
		if (obj->IsChanged())
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
	i = this->objList.GetCount();
	while (i-- > 0)
	{
		obj = this->objList.GetItem(i);
		if (obj->DoEvents())
		{
			return isChanged;
		}
	}
	return isChanged;
}

void UI::DObj::DObjHandler::DrawAll(Media::DrawImage *dimg)
{
	this->shown = true;
	Sync::MutexUsage updMutUsage(&this->updMut);
	this->DrawBkg(dimg);

	UOSInt i;
	UOSInt j;
	DirectObject *obj;
	Sync::MutexUsage mutUsage(&this->objMut);
	i = 0;
	j = this->objList.GetCount();
	while (i < j)
	{
		obj = this->objList.GetItem(i);
		obj->DrawObject(dimg);
		i++;
	}
	mutUsage.EndUse();
}

void UI::DObj::DObjHandler::BeginUpdate()
{
	this->updMut.Lock();
}

void UI::DObj::DObjHandler::EndUpdate()
{
	this->updMut.Unlock();
}

void UI::DObj::DObjHandler::OnMouseDown(Math::Coord2D<OSInt> scnPos, UI::GUIControl::MouseButton button)
{
	if (button == UI::GUIControl::MBTN_LEFT)
	{
		UOSInt i;
		DirectObject *obj;
		Sync::MutexUsage mutUsage(&this->objMut);
		i = this->objList.GetCount();
		while (i-- > 0)
		{
			obj = this->objList.GetItem(i);
			if (obj->IsObject(scnPos))
			{
				this->downObj = obj;
				this->downObj->OnMouseDown();
				break;
			}
		}
	}
}

void UI::DObj::DObjHandler::OnMouseUp(Math::Coord2D<OSInt> scnPos, UI::GUIControl::MouseButton button)
{
	if (button == UI::GUIControl::MBTN_LEFT)
	{
		Sync::MutexUsage mutUsage(&this->objMut);
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

void UI::DObj::DObjHandler::OnMouseMove(Math::Coord2D<OSInt> scnPos)
{
	DirectObject *mouseObj = 0;
	
	UOSInt i;
	DirectObject *obj;
	Sync::MutexUsage mutUsage(&this->objMut);
	i = this->objList.GetCount();
	while (i-- > 0)
	{
		obj = this->objList.GetItem(i);
		if (obj->IsObject(scnPos))
		{
			mouseObj = obj;
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

void UI::DObj::DObjHandler::SizeChanged(Math::Size2D<UOSInt> size)
{

}
