#include "Stdafx.h"
#include "Math/Math_C.h"
#include "SSWR/AVIRead/AVIRModel3DViewerForm.h"
#include <algorithm>
#include <cmath>
#include <vector>

class SSWR::AVIRead::AVIRModel3DViewerForm::ModelControl : public UI::GUICustomDraw
{
	struct ViewTriangle
	{
		Math::Coord2DDbl points[3];
		Double depth;
		Double light;
	};

	NN<Model::Model3D> model;
	Bool mouseDown;
	Math::Coord2D<IntOS> lastMouse;
	Double yaw;
	Double pitch;
	Double zoom;

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
		this->zoom *= delta > 0 ? 1.15 : 0.87;
		if (this->zoom < 0.05) this->zoom = 0.05;
		if (this->zoom > 100) this->zoom = 100;
		this->Redraw();
		return UI::EventState::StopEvent;
	}

	virtual void OnDraw(NN<Media::DrawImage> dimg)
	{
		NN<Media::DrawBrush> background = dimg->NewBrushARGB(0xfff2f4f3);
		dimg->DrawRect(Math::Coord2DDbl(0, 0), dimg->GetSize(), nullptr, background);
		dimg->DelBrush(background);

		UIntOS triCount = this->model->GetTriangleCount();
		if (triCount == 0) return;
		Double minX = 0;
		Double minY = 0;
		Double minZ = 0;
		Double maxX = 0;
		Double maxY = 0;
		Double maxZ = 0;
		UIntOS i = 0;
		while (i < triCount)
		{
			const Model::Model3D::Triangle &tri = this->model->GetTriangle(i);
			Double xs[3] = {tri.x1, tri.x2, tri.x3};
			Double ys[3] = {tri.y1, tri.y2, tri.y3};
			Double zs[3] = {tri.z1, tri.z2, tri.z3};
			if (i == 0)
			{
				minX = maxX = xs[0]; minY = maxY = ys[0]; minZ = maxZ = zs[0];
			}
			UIntOS j = 0;
			while (j < 3)
			{
				if (xs[j] < minX) minX = xs[j];
				if (ys[j] < minY) minY = ys[j];
				if (zs[j] < minZ) minZ = zs[j];
				if (xs[j] > maxX) maxX = xs[j];
				if (ys[j] > maxY) maxY = ys[j];
				if (zs[j] > maxZ) maxZ = zs[j];
				j++;
			}
			i++;
		}
		Double centerX = (minX + maxX) * 0.5;
		Double centerY = (minY + maxY) * 0.5;
		Double centerZ = (minZ + maxZ) * 0.5;
		Double extent = std::max(maxX - minX, std::max(maxY - minY, maxZ - minZ));
		if (extent <= 1e-20) extent = 1;
		Double scale = std::min(dimg->GetWidth(), dimg->GetHeight()) * 0.76 / extent * this->zoom;
		Double sinYaw = std::sin(this->yaw);
		Double cosYaw = std::cos(this->yaw);
		Double sinPitch = std::sin(this->pitch);
		Double cosPitch = std::cos(this->pitch);
		Double screenCX = dimg->GetWidth() * 0.5;
		Double screenCY = dimg->GetHeight() * 0.5;
		std::vector<ViewTriangle> viewTris;
		viewTris.reserve((size_t)triCount);
		i = 0;
		while (i < triCount)
		{
			const Model::Model3D::Triangle &tri = this->model->GetTriangle(i);
			Double x[3] = {tri.x1 - centerX, tri.x2 - centerX, tri.x3 - centerX};
			Double y[3] = {tri.y1 - centerY, tri.y2 - centerY, tri.y3 - centerY};
			Double z[3] = {tri.z1 - centerZ, tri.z2 - centerZ, tri.z3 - centerZ};
			Double vx[3];
			Double vy[3];
			Double vz[3];
			ViewTriangle viewTri;
			UIntOS j = 0;
			while (j < 3)
			{
				Double rx = x[j] * cosYaw - y[j] * sinYaw;
				Double ry = x[j] * sinYaw + y[j] * cosYaw;
				vx[j] = rx;
				vy[j] = ry * cosPitch - z[j] * sinPitch;
				vz[j] = ry * sinPitch + z[j] * cosPitch;
				viewTri.points[j] = Math::Coord2DDbl(screenCX + rx * scale, screenCY - vy[j] * scale);
				j++;
			}
			viewTri.depth = (vz[0] + vz[1] + vz[2]) / 3.0;
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
			viewTri.light = 0.30 + 0.65 * dot;
			viewTris.push_back(viewTri);
			i++;
		}
		std::stable_sort(viewTris.begin(), viewTris.end(), [](const ViewTriangle &a, const ViewTriangle &b) { return a.depth < b.depth; });

		NN<Media::DrawBrush> brushes[16];
		i = 0;
		while (i < 16)
		{
			UInt32 level = 96 + (UInt32)i * 10;
			UInt32 color = 0xff000000 | (level << 16) | ((level + 18) << 8) | (level + 6);
			brushes[i] = dimg->NewBrushARGB(color);
			i++;
		}
		for (const ViewTriangle &tri : viewTris)
		{
			UIntOS level = (UIntOS)((tri.light - 0.30) / 0.65 * 15.0);
			if (level > 15) level = 15;
			dimg->DrawPolygon(tri.points, 3, nullptr, brushes[level]);
		}
		i = 0;
		while (i < 16)
		{
			dimg->DelBrush(brushes[i]);
			i++;
		}
	}

public:
	ModelControl(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, NN<Media::ColorManagerSess> colorSess, NN<Model::Model3D> model) : UI::GUICustomDraw(ui, parent, eng, colorSess)
	{
		this->model = model;
		this->mouseDown = false;
		this->lastMouse = Math::Coord2D<IntOS>(0, 0);
		this->yaw = 0.65;
		this->pitch = 0.45;
		this->zoom = 1;
	}
};

SSWR::AVIRead::AVIRModel3DViewerForm::AVIRModel3DViewerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Model::Model3D> model) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->model = model;
	this->colorSess = this->core->GetColorManager()->CreateSess(this->GetHMonitor());
	this->SetText(model->GetSourceNameObj()->ToCString());
	this->SetFont(nullptr, 8.25, false);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->lblInfo = ui->NewLabel(*this, CSTR(""));
	this->lblInfo->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblInfo->SetRect(0, 0, 100, 26, false);
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr = Text::StrUIntOS(sbuff, model->GetTriangleCount());
	sptr = Text::StrConcatC(sptr, UTF8STRC(" triangles"));
	this->lblInfo->SetText(CSTRP(sbuff, sptr));
	NEW_CLASSNN(this->modelCtrl, ModelControl(ui, *this, this->core->GetDrawEngine(), this->colorSess, model));
	this->modelCtrl->SetDockType(UI::GUIControl::DOCK_FILL);
}

SSWR::AVIRead::AVIRModel3DViewerForm::~AVIRModel3DViewerForm()
{
	this->ClearChildren();
	this->model.Delete();
	this->core->GetDrawEngine()->EndColorSess(this->colorSess);
	this->core->GetColorManager()->DeleteSess(this->colorSess);
}

void SSWR::AVIRead::AVIRModel3DViewerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}