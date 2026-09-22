#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/Engine3DFactory.h"
#include "UI/GUIMap3DControl.h"

UI::GUIMap3DControl::GUIMap3DControl(NN<GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, NN<Map::MapEnv> env, NN<Media::ColorManagerSess> colorSess) : GUICustomDraw(ui, parent, eng, colorSess)
{
	NN<Media::Engine3D> eng3d;
	NN<Map::Map3DView> view3d;
	this->colorSess = colorSess;
	this->mouseDown = false;
	this->mouseBtn = UI::GUIControl::MBTN_LEFT;
	this->lastMouse = Math::Coord2D<IntOS>(0, 0);

	eng3d = Media::Engine3DFactory::CreateEngine3D();
	this->scene3d = eng3d->CreateScene(this->GetDisplayHandle());
	NEW_CLASSNN(view3d, Map::Map3DView(env, eng3d, Math::Size2DDbl(640, 480)));
	this->eng3d = eng3d;
	this->view3d = view3d;
}

UI::GUIMap3DControl::~GUIMap3DControl()
{
	NN<Media::Engine3DScene> scene3d;
	if (this->scene3d.SetTo(scene3d))
	{
		this->eng3d->DeleteScene(scene3d);
		this->scene3d = nullptr;
	}
	this->view3d.Delete();
	this->eng3d.Delete();
}

void UI::GUIMap3DControl::HandleMapUpdated(MapUpdatedHandler hdlr, AnyType userObj)
{
	this->mapUpdHdlrs.Add({hdlr, userObj});
}

void UI::GUIMap3DControl::SetVAngle(Double vAngle)
{
	this->view3d->SetVAngle(vAngle);
	this->Redraw();
}

void UI::GUIMap3DControl::SetHAngle(Double hAngle)
{
	this->view3d->SetHAngle(hAngle);
	this->Redraw();
}

Double UI::GUIMap3DControl::GetVAngle() const
{
	return this->view3d->GetVAngle();
}

Double UI::GUIMap3DControl::GetHAngle() const
{
	return this->view3d->GetHAngle();
}

Math::Coord2DDbl UI::GUIMap3DControl::GetMapCenter() const
{
	return this->view3d->GetCenter();
}

UI::EventState UI::GUIMap3DControl::OnMouseDown(Math::Coord2D<IntOS> scnPos, MouseButton btn)
{
	this->SetCapture();
	this->mouseDown = true;
	this->mouseBtn = btn;
	this->lastMouse = scnPos;
	this->Focus();
	return UI::EventState::StopEvent;
}

UI::EventState UI::GUIMap3DControl::OnMouseUp(Math::Coord2D<IntOS> scnPos, MouseButton btn)
{
	this->ReleaseCapture();
	this->mouseDown = false;
	return UI::EventState::StopEvent;
}

void UI::GUIMap3DControl::OnMouseMove(Math::Coord2D<IntOS> scnPos)
{
	if (this->mouseDown)
	{
		Math::Coord2D<IntOS> diff = scnPos - this->lastMouse;
		if (this->mouseBtn == UI::GUIControl::MBTN_LEFT)
		{
			this->view3d->Orbit(-IntOS2Double(diff.x) * 0.01, -IntOS2Double(diff.y) * 0.01);
		}
		else
		{
			this->view3d->PanScreen(diff);
		}
		this->lastMouse = scnPos;
		this->Redraw();
	}
}

UI::EventState UI::GUIMap3DControl::OnMouseWheel(Math::Coord2D<IntOS> scnPos, Int32 delta)
{
	if (delta < 0)
	{
		this->view3d->Zoom(1.2);
	}
	else
	{
		this->view3d->Zoom(0.8);
	}
	this->Redraw();
	return UI::EventState::StopEvent;
}

void UI::GUIMap3DControl::OnDraw(NN<Media::DrawImage> img)
{
	this->view3d->SetScreenSize(img->GetSize());
	NN<Media::Engine3DScene> scene3d;
	if (this->scene3d.SetTo(scene3d))
	{
		this->view3d->Render(scene3d, Math::Size2D<UInt32>((UInt32)img->GetWidth(), (UInt32)img->GetHeight()));
	}

	Math::Coord2DDbl center = this->view3d->GetCenter();
	UIntOS i = this->mapUpdHdlrs.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<MapUpdatedHandler> cb = this->mapUpdHdlrs.GetItem(i);
		cb.func(cb.userObj, center);
	}
}
