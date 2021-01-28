#include "Stdafx.h"
#include "UI/DObj/DirectObject.h"
#include "UI/DObj/DObjHandler.h"

void UI::DObj::DObjHandler::OnObjectClear()
{
}

UI::DObj::DObjHandler::DObjHandler(Media::DrawEngine *deng)
{
	this->shown = false;
	this->deng = deng;
	this->colorSess = 0;
	NEW_CLASS(this->objList, Data::ArrayList<DirectObject*>());
	NEW_CLASS(this->objMut, Sync::Mutex());
	NEW_CLASS(this->updMut, Sync::Mutex());
	this->moveObj = 0;
	this->downObj = 0;
}

UI::DObj::DObjHandler::~DObjHandler()
{
	OSInt i;
	DirectObject *obj;
	i = this->objList->GetCount();
	while (i-- > 0)
	{
		obj = this->objList->GetItem(i);
		DEL_CLASS(obj);
	}
	this->objList->Clear();
	DEL_CLASS(this->updMut);
	DEL_CLASS(this->objMut);
	DEL_CLASS(this->objList);
}

void UI::DObj::DObjHandler::SetColorSess(Media::ColorManagerSess *colorSess)
{
	this->colorSess = colorSess;
}

void UI::DObj::DObjHandler::ClearObjects()
{
	OSInt i;
	DirectObject *obj;
	this->objMut->Lock();
	i = this->objList->GetCount();
	while (i-- > 0)
	{
		obj = this->objList->GetItem(i);
		DEL_CLASS(obj);
	}
	this->objList->Clear();
	this->shown = false;
	this->downObj = 0;
	this->moveObj = 0;
	this->objMut->Unlock();
	this->OnObjectClear();
}

void UI::DObj::DObjHandler::AddObject(DirectObject *obj)
{
	this->objMut->Lock();
	this->objList->Add(obj);
	this->shown = false;
	this->objMut->Unlock();
}

Bool UI::DObj::DObjHandler::Check(Media::DrawImage *dimg)
{
	Bool isChanged = !this->shown;
	OSInt i;
	DirectObject *obj;
	this->objMut->Lock();
	i = this->objList->GetCount();
	while (i-- > 0)
	{
		obj = this->objList->GetItem(i);
		if (obj->IsChanged())
		{
			isChanged = true;
		}
	}
	this->objMut->Unlock();
	if (isChanged)
	{
		this->DrawAll(dimg);
	}
	this->objMut->Lock();
	i = this->objList->GetCount();
	while (i-- > 0)
	{
		obj = this->objList->GetItem(i);
		if (obj->DoEvents())
		{
			this->objMut->Unlock();
			return isChanged;
		}
	}
	this->objMut->Unlock();
	return isChanged;
}

void UI::DObj::DObjHandler::DrawAll(Media::DrawImage *dimg)
{
	this->shown = true;
	this->updMut->Lock();
	this->DrawBkg(dimg);

	OSInt i;
	OSInt j;
	DirectObject *obj;
	this->objMut->Lock();
	i = 0;
	j = this->objList->GetCount();
	while (i < j)
	{
		obj = this->objList->GetItem(i);
		obj->DrawObject(dimg);
		i++;
	}
	this->objMut->Unlock();
	this->updMut->Unlock();
}

void UI::DObj::DObjHandler::BeginUpdate()
{
	this->updMut->Lock();
}

void UI::DObj::DObjHandler::EndUpdate()
{
	this->updMut->Unlock();
}

void UI::DObj::DObjHandler::OnMouseDown(OSInt x, OSInt y, UI::GUIControl::MouseButton button)
{
	if (button == UI::GUIControl::MBTN_LEFT)
	{
		OSInt i;
		DirectObject *obj;
		this->objMut->Lock();
		i = this->objList->GetCount();
		while (i-- > 0)
		{
			obj = this->objList->GetItem(i);
			if (obj->IsObject(x, y))
			{
				this->downObj = obj;
				this->downObj->OnMouseDown();
				break;
			}
		}
		this->objMut->Unlock();
	}
}

void UI::DObj::DObjHandler::OnMouseUp(OSInt x, OSInt y, UI::GUIControl::MouseButton button)
{
	if (button == UI::GUIControl::MBTN_LEFT)
	{
		this->objMut->Lock();
		if (this->downObj != 0)
		{
			this->downObj->OnMouseUp();
			if (this->downObj->IsObject(x, y))
			{
				this->downObj->OnMouseClick();
			}
			this->downObj = 0;
		}
		this->objMut->Unlock();
	}
}

void UI::DObj::DObjHandler::OnMouseMove(OSInt x, OSInt y)
{
	DirectObject *mouseObj = 0;
	
	OSInt i;
	DirectObject *obj;
	this->objMut->Lock();
	i = this->objList->GetCount();
	while (i-- > 0)
	{
		obj = this->objList->GetItem(i);
		if (obj->IsObject(x, y))
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
	this->objMut->Unlock();
}
