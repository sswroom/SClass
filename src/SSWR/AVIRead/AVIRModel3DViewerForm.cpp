#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "Math/Math_C.h"
#include "Math/Vector3D.h"
#include "Media/Engine3DFactory.h"
#include "SSWR/AVIRead/AVIRModel3DViewerForm.h"

class SSWR::AVIRead::AVIRModel3DViewerForm::ModelControl : public UI::GUICustomDraw
{
	struct ViewTriangle
	{
		Math::Coord2DDbl points[3];
		Double depth;
		Double light;
	};

	class ViewTriangleComparator : public Data::Comparator<NN<ViewTriangle>>
	{
	public:
		virtual IntOS Compare(NN<ViewTriangle> a, NN<ViewTriangle> b) const
		{
			if (a->depth < b->depth) return -1;
			if (a->depth > b->depth) return 1;
			return 0;
		}
	};

	NN<Math::Model3D> model;
	Bool mouseDown;
	Math::Coord2D<IntOS> lastMouse;
	Double yaw;
	Double pitch;
	Double zoom;
	NN<Media::Engine3D> engine3d;
	Optional<Media::Engine3DScene> scene;

	Bool RenderHardware(NN<Media::DrawImage> dimg)
	{
		NN<Media::Engine3DScene> scene;
		if (!this->scene.SetTo(scene))
			return false;
		if (this->model->GetCount() == 0)
			return false;

		Math::Vector3D minPt;
		Math::Vector3D maxPt;
		this->model->GetBounds(minPt, maxPt);

		Math::Vector3D centerPt = (minPt + maxPt) * 0.5;
		Double extent = Math_Max(maxPt.GetX() - minPt.GetX(), Math_Max(maxPt.GetY() - minPt.GetY(), maxPt.GetZ() - minPt.GetZ()));
		if (extent <= 1e-20)
			extent = 1;

		Media::Engine3D::CameraState camera;
		camera.fovDeg = 45;
		camera.hAngle = this->yaw;
		camera.vAngle = this->pitch;
		camera.targetPos = centerPt;
		Math::Size2D<UInt32> frameSize((UInt32)dimg->GetWidth(), (UInt32)dimg->GetHeight());
		if (frameSize.x == 0 || frameSize.y == 0)
			return false;
		Double aspect = (Double)frameSize.x / frameSize.y;
		Double fitRatio = Math_Min(1.0, aspect);
		Double camDist = extent * 0.5 / (Math_Tan(camera.fovDeg * Math::PI / 360.0) * 0.76 * fitRatio * this->zoom);
		Double cosPitch = Math_Cos(this->pitch);
		camera.camPos = centerPt + Math::Vector3D(camDist * cosPitch * Math_Cos(this->yaw), camDist * cosPitch * Math_Sin(this->yaw), camDist * Math_Sin(this->pitch));

		if (!this->engine3d->BeginFrame(scene, frameSize))
			return false;
		Bool ret = this->engine3d->RenderModel3D(camera, this->model);
		this->engine3d->EndFrame();
		return ret;
	}

protected:
	virtual UI::EventState OnMouseDown(Math::Coord2D<IntOS> scnPos, UI::GUIControl::MouseButton btn)
	{
		this->SetCapture();
		this->mouseDown = true;
		this->lastMouse = scnPos;
		this->Focus();
		return UI::EventState::StopEvent;
	}

	virtual UI::EventState OnMouseUp(Math::Coord2D<IntOS> scnPos, UI::GUIControl::MouseButton btn)
	{
		this->ReleaseCapture();
		this->mouseDown = false;
		return UI::EventState::StopEvent;
	}

	virtual void OnMouseMove(Math::Coord2D<IntOS> scnPos)
	{
		if (this->mouseDown)
		{
			Math::Coord2D<IntOS> diff = scnPos - this->lastMouse;
			this->yaw += IntOS2Double(diff.x) * 0.01;
			this->pitch += IntOS2Double(diff.y) * 0.01;
			if (this->pitch < -1.5) this->pitch = -1.5;
			if (this->pitch > 1.5) this->pitch = 1.5;
			this->lastMouse = scnPos;
			this->Redraw();
		}
	}

	virtual UI::EventState OnMouseWheel(Math::Coord2D<IntOS> scnPos, Int32 delta)
	{
		if (delta == 0)
			return UI::EventState::StopEvent;
		this->zoom *= delta > 0 ? 1.15 : 0.87;
		if (this->zoom < 0.05) this->zoom = 0.05;
		if (this->zoom > 100) this->zoom = 100;
		this->Redraw();
		return UI::EventState::StopEvent;
	}

	virtual void OnDraw(NN<Media::DrawImage> dimg)
	{
		if (RenderHardware(dimg))
			return;

		NN<Media::DrawBrush> background = dimg->NewBrushARGB(0xfff2f4f3);
		dimg->DrawRect(Math::Coord2DDbl(0, 0), dimg->GetSize(), nullptr, background);
		dimg->DelBrush(background);

		UIntOS triCount = this->model->GetCount();
		if (triCount == 0) return;
		Math::Vector3D minPt = nullptr;
		Math::Vector3D maxPt = nullptr;
		UIntOS i = 0;
		while (i < triCount)
		{
			NN<Math::Model3D::Triangle> tri = NN<Math::Model3D::Triangle>::ConvertFrom(this->model->GetItemNoCheck(i));
			if (i == 0)
			{
				minPt = maxPt = tri->pt1;
			}
			minPt = minPt.Min(tri->pt1).Min(tri->pt2).Min(tri->pt3);
			maxPt = maxPt.Max(tri->pt1).Max(tri->pt2).Max(tri->pt3);
			i++;
		}
		Math::Vector3D centerPt = (minPt + maxPt) * 0.5;
		Double extent = Math_Max(maxPt.GetX() - minPt.GetX(), Math_Max(maxPt.GetY() - minPt.GetY(), maxPt.GetZ() - minPt.GetZ()));
		if (extent <= 1e-20) extent = 1;
		Double scale = Math_Min(dimg->GetWidth(), dimg->GetHeight()) * 0.76 / extent * this->zoom;
		Double sinYaw = Math_Sin(this->yaw);
		Double cosYaw = Math_Cos(this->yaw);
		Double sinPitch = Math_Sin(this->pitch);
		Double cosPitch = Math_Cos(this->pitch);
		Double screenCX = dimg->GetWidth() * 0.5;
		Double screenCY = dimg->GetHeight() * 0.5;
		Data::ArrayListNN<ViewTriangle> viewTris(triCount);
		i = 0;
		while (i < triCount)
		{
			NN<ViewTriangle> viewTri;
			NEW_CLASSNN(viewTri, ViewTriangle());
			NN<Math::Model3D::Triangle> tri = NN<Math::Model3D::Triangle>::ConvertFrom(this->model->GetItemNoCheck(i));
			Math::Vector3D pts[3];
			pts[0] = tri->pt1 - centerPt;
			pts[1] = tri->pt2 - centerPt;
			pts[2] = tri->pt3 - centerPt;
			Double vx[3];
			Double vy[3];
			Double vz[3];
			UIntOS j = 0;
			while (j < 3)
			{
				Double rx = pts[j].GetX() * cosYaw - pts[j].GetY() * sinYaw;
				Double ry = pts[j].GetX() * sinYaw + pts[j].GetY() * cosYaw;
				vx[j] = rx;
				vy[j] = ry * cosPitch - pts[j].GetZ() * sinPitch;
				vz[j] = ry * sinPitch + pts[j].GetZ() * cosPitch;
				viewTri->points[j] = Math::Coord2DDbl(screenCX + rx * scale, screenCY - vy[j] * scale);
				j++;
			}
			viewTri->depth = (vz[0] + vz[1] + vz[2]) / 3.0;
			Double ax = vx[1] - vx[0];
			Double ay = vy[1] - vy[0];
			Double az = vz[1] - vz[0];
			Double bx = vx[2] - vx[0];
			Double by = vy[2] - vy[0];
			Double bz = vz[2] - vz[0];
			Double nx = ay * bz - az * by;
			Double ny = az * bx - ax * bz;
			Double nz = ax * by - ay * bx;
			Double nLen = std::sqrt(nx * nx + ny * ny + nz * nz);
			Double dot = nLen > 1e-20 ? std::abs((nx * 0.25 - ny * 0.35 + nz * 0.9) / nLen) : 0.3;
			viewTri->light = 0.30 + 0.65 * dot;
			viewTris.Add(viewTri);
			i++;
		}
		ViewTriangleComparator comp;
		Data::Sort::ArtificialQuickSort::Sort<NN<ViewTriangle>>(viewTris, comp);

		NN<Media::DrawBrush> brushes[16];
		i = 0;
		while (i < 16)
		{
			UInt32 level = 96 + (UInt32)i * 10;
			UInt32 color = 0xff000000 | (level << 16) | ((level + 18) << 8) | (level + 6);
			brushes[i] = dimg->NewBrushARGB(color);
			i++;
		}
		i = 0;
		UIntOS j = viewTris.GetCount();
		while (i < j)
		{
			NN<ViewTriangle> tri = viewTris.GetItemNoCheck(i);
			UIntOS level = (UIntOS)((tri->light - 0.30) / 0.65 * 15.0);
			if (level > 15) level = 15;
			dimg->DrawPolygon(tri->points, 3, nullptr, brushes[level]);
			i++;
		}
		i = 0;
		while (i < 16)
		{
			dimg->DelBrush(brushes[i]);
			i++;
		}
		viewTris.DeleteAll();
	}

public:
	ModelControl(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, NN<Media::ColorManagerSess> colorSess, NN<Media::Engine3D> engine3d, NN<Math::Model3D> model) : UI::GUICustomDraw(ui, parent, eng, colorSess)
	{
		this->model = model;
		this->engine3d = engine3d;
		this->scene = this->engine3d->CreateScene(this->GetHandle());
		this->mouseDown = false;
		this->lastMouse = Math::Coord2D<IntOS>(0, 0);
		this->yaw = 0.65;
		this->pitch = 0.45;
		this->zoom = 1;
	}

	virtual ~ModelControl()
	{
		NN<Media::Engine3DScene> scene;
		if (this->scene.SetTo(scene))
			this->engine3d->DeleteScene(scene);
	}
};

SSWR::AVIRead::AVIRModel3DViewerForm::AVIRModel3DViewerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Math::Model3D> model) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->model = model;
	this->engine3d = Media::Engine3DFactory::CreateEngine3D();
	this->colorSess = this->core->GetColorManager()->CreateSess(this->GetHMonitor());
	this->SetText(model->GetSourceNameObj()->ToCString());
	this->SetFont(nullptr, 8.25, false);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->lblInfo = ui->NewLabel(*this, CSTR(""));
	this->lblInfo->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblInfo->SetRect(0, 0, 100, 26, false);
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr = Text::StrUIntOS(sbuff, model->GetCount());
	sptr = Text::StrConcatC(sptr, UTF8STRC(" triangles"));
	this->lblInfo->SetText(CSTRP(sbuff, sptr));
	NEW_CLASSNN(this->modelCtrl, ModelControl(ui, *this, this->core->GetDrawEngine(), this->colorSess, this->engine3d, model));
	this->modelCtrl->SetDockType(UI::GUIControl::DOCK_FILL);
}

SSWR::AVIRead::AVIRModel3DViewerForm::~AVIRModel3DViewerForm()
{
	this->ClearChildren();
	this->engine3d.Delete();
	this->model.Delete();
	this->core->GetDrawEngine()->EndColorSess(this->colorSess);
	this->core->GetColorManager()->DeleteSess(this->colorSess);
}

void SSWR::AVIRead::AVIRModel3DViewerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}