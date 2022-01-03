#include "Stdafx.h"
#include "Math/Math.h"
#include "Math/VectorImage.h"
#include "SSWR/AVIRead/AVIRGISEditImageForm.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRGISEditImageForm::OnImageChg(void *userObj)
{
	SSWR::AVIRead::AVIRGISEditImageForm *me = (SSWR::AVIRead::AVIRGISEditImageForm*)userObj;
	me->currImage = (OSInt)me->lbImages->GetSelectedItem();
	me->UpdateImgStat();
	if (me->chkAutoPan->IsChecked())
	{
		me->navi->PanToMap((me->imgMinY + me->imgMaxY) * 0.5, (me->imgMinX + me->imgMaxX) * 0.5);
	}
}

void SSWR::AVIRead::AVIRGISEditImageForm::UpdateImgStat()
{
	UTF8Char sbuff[256];
	Double x1;
	Double y1;
	Double x2;
	Double y2;
	void *sess = this->lyr->BeginGetObject();
	Math::Vector2D *vec = this->lyr->GetVectorById(sess, this->currImage);
	if (vec)
	{
		vec->GetBounds(&x1, &y1, &x2, &y2);
		Text::StrDouble(sbuff, x1);
		this->txtLeft->SetText(sbuff);
		Text::StrDouble(sbuff, y1);
		this->txtBottom->SetText(sbuff);
		Text::StrDouble(sbuff, x2);
		this->txtRight->SetText(sbuff);
		Text::StrDouble(sbuff, y2);
		this->txtTop->SetText(sbuff);
		this->imgMinX = x1;
		this->imgMinY = y1;
		this->imgMaxX = x2;
		this->imgMaxY = y2;
		DEL_CLASS(vec);
	}
	else
	{
		this->currImage = -1;
	}
	this->lyr->EndGetObject(sess);
}

Bool __stdcall SSWR::AVIRead::AVIRGISEditImageForm::OnMouseDown(void *userObj, OSInt x, OSInt y)
{
	SSWR::AVIRead::AVIRGISEditImageForm *me = (SSWR::AVIRead::AVIRGISEditImageForm*)userObj;
	if (me->currImage == -1)
		return false;
	if (!me->chkEdit->IsChecked())
		return false;
	me->downType = me->CalcDownType(x, y);
	if (me->downType != 0)
	{
		Math::Polygon *pg;
		Double *ptList;
		UOSInt nPoints;
		NEW_CLASS(pg, Math::Polygon(me->navi->GetSRID(), 1, 5));
		ptList = pg->GetPointList(&nPoints);
		ptList[0] = me->imgMinX;
		ptList[1] = me->imgMaxY;
		ptList[2] = me->imgMaxX;
		ptList[3] = me->imgMaxY;
		ptList[4] = me->imgMaxX;
		ptList[5] = me->imgMinY;
		ptList[6] = me->imgMinX;
		ptList[7] = me->imgMinY;
		ptList[8] = me->imgMinX;
		ptList[9] = me->imgMaxY;

		me->navi->SetSelectedVector(pg);
		me->downX = x;
		me->downY = y;
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall SSWR::AVIRead::AVIRGISEditImageForm::OnMouseUp(void *userObj, OSInt x, OSInt y)
{
	SSWR::AVIRead::AVIRGISEditImageForm *me = (SSWR::AVIRead::AVIRGISEditImageForm*)userObj;
	if (me->currImage == -1)
	{
		return false;
	}
	if (me->downType == 1)
	{
		Bool changed = false;
		Double x1;
		Double y1;
		Double x2;
		Double y2;
		me->navi->ScnXY2MapXY(x, y, &x1, &y1);

		void *sess = me->lyr->BeginGetObject();
		Math::VectorImage *img = (Math::VectorImage*)me->lyr->GetVectorById(sess, me->currImage);
		if (img)
		{
			if (y1 > me->imgMinY)
			{
				y2 = y1;
				y1 = me->imgMinY;
			}
			else
			{
				y2 = me->imgMinY;
			}
			x1 = me->imgMinX;
			x2 = me->imgMaxX;
			img->SetBounds(x1, y1, x2, y2);
			me->lyr->ReplaceVector(me->currImage, img);
			changed = true;
		}
		me->lyr->EndGetObject(sess);
		if (changed)
		{
			me->UpdateImgStat();
			me->navi->RedrawMap();
		}
	}
	else if (me->downType == 2)
	{
		Bool changed = false;
		Double x1;
		Double y1;
		Double x2;
		Double y2;
		me->navi->ScnXY2MapXY(x, y, &x1, &y1);

		void *sess = me->lyr->BeginGetObject();
		Math::VectorImage *img = (Math::VectorImage*)me->lyr->GetVectorById(sess, me->currImage);
		if (img)
		{
			if (y1 > me->imgMinY)
			{
				y2 = y1;
				y1 = me->imgMinY;
			}
			else
			{
				y2 = me->imgMinY;
			}
			if (x1 > me->imgMinX)
			{
				x2 = x1;
				x1 = me->imgMinX;
			}
			else
			{
				x2 = me->imgMinX;
			}
			img->SetBounds(x1, y1, x2, y2);
			me->lyr->ReplaceVector(me->currImage, img);
			changed = true;
		}
		me->lyr->EndGetObject(sess);
		if (changed)
		{
			me->UpdateImgStat();
			me->navi->RedrawMap();
		}
	}
	else if (me->downType == 3)
	{
		Bool changed = false;
		Double x1;
		Double y1;
		Double x2;
		Double y2;
		me->navi->ScnXY2MapXY(x, y, &x1, &y1);

		void *sess = me->lyr->BeginGetObject();
		Math::VectorImage *img = (Math::VectorImage*)me->lyr->GetVectorById(sess, me->currImage);
		if (img)
		{
			y1 = me->imgMinY;
			y2 = me->imgMaxY;
			if (x1 > me->imgMinX)
			{
				x2 = x1;
				x1 = me->imgMinX;
			}
			else
			{
				x2 = me->imgMinX;
			}
			img->SetBounds(x1, y1, x2, y2);
			me->lyr->ReplaceVector(me->currImage, img);
			changed = true;
		}
		me->lyr->EndGetObject(sess);
		if (changed)
		{
			me->UpdateImgStat();
			me->navi->RedrawMap();
		}
	}
	else if (me->downType == 4)
	{
		Bool changed = false;
		Double x1;
		Double y1;
		Double x2;
		Double y2;
		me->navi->ScnXY2MapXY(x, y, &x1, &y1);

		void *sess = me->lyr->BeginGetObject();
		Math::VectorImage *img = (Math::VectorImage*)me->lyr->GetVectorById(sess, me->currImage);
		if (img)
		{
			if (y1 < me->imgMaxY)
			{
				y2 = me->imgMaxY;
			}
			else
			{
				y2 = y1;
				y1 = me->imgMaxY;
			}
			if (x1 > me->imgMinX)
			{
				x2 = x1;
				x1 = me->imgMinX;
			}
			else
			{
				x2 = me->imgMinX;
			}
			img->SetBounds(x1, y1, x2, y2);
			me->lyr->ReplaceVector(me->currImage, img);
			changed = true;
		}
		me->lyr->EndGetObject(sess);
		if (changed)
		{
			me->UpdateImgStat();
			me->navi->RedrawMap();
		}
	}
	else if (me->downType == 5)
	{
		Bool changed = false;
		Double x1;
		Double y1;
		Double x2;
		Double y2;
		me->navi->ScnXY2MapXY(x, y, &x1, &y1);

		void *sess = me->lyr->BeginGetObject();
		Math::VectorImage *img = (Math::VectorImage*)me->lyr->GetVectorById(sess, me->currImage);
		if (img)
		{
			if (y1 < me->imgMaxY)
			{
				y2 = me->imgMaxY;
			}
			else
			{
				y2 = y1;
				y1 = me->imgMaxY;
			}
			x1 = me->imgMinX;
			x2 = me->imgMaxX;
			img->SetBounds(x1, y1, x2, y2);
			me->lyr->ReplaceVector(me->currImage, img);
			changed = true;
		}
		me->lyr->EndGetObject(sess);
		if (changed)
		{
			me->UpdateImgStat();
			me->navi->RedrawMap();
		}
	}
	else if (me->downType == 6)
	{
		Bool changed = false;
		Double x1;
		Double y1;
		Double x2;
		Double y2;
		me->navi->ScnXY2MapXY(x, y, &x1, &y1);

		void *sess = me->lyr->BeginGetObject();
		Math::VectorImage *img = (Math::VectorImage*)me->lyr->GetVectorById(sess, me->currImage);
		if (img)
		{
			if (y1 < me->imgMaxY)
			{
				y2 = me->imgMaxY;
			}
			else
			{
				y2 = y1;
				y1 = me->imgMaxY;
			}
			if (x1 < me->imgMaxX)
			{
				x2 = me->imgMaxX;
			}
			else
			{
				x2 = x1;
				x1 = me->imgMaxX;
			}
			img->SetBounds(x1, y1, x2, y2);
			me->lyr->ReplaceVector(me->currImage, img);
			changed = true;
		}
		me->lyr->EndGetObject(sess);
		if (changed)
		{
			me->UpdateImgStat();
			me->navi->RedrawMap();
		}
	}
	else if (me->downType == 7)
	{
		Bool changed = false;
		Double x1;
		Double y1;
		Double x2;
		Double y2;
		me->navi->ScnXY2MapXY(x, y, &x1, &y1);

		void *sess = me->lyr->BeginGetObject();
		Math::VectorImage *img = (Math::VectorImage*)me->lyr->GetVectorById(sess, me->currImage);
		if (img)
		{
			y1 = me->imgMinY;
			y2 = me->imgMaxY;
			if (x1 < me->imgMaxX)
			{
				x2 = me->imgMaxX;
			}
			else
			{
				x2 = x1;
				x1 = me->imgMaxX;
			}
			img->SetBounds(x1, y1, x2, y2);
			me->lyr->ReplaceVector(me->currImage, img);
			changed = true;
		}
		me->lyr->EndGetObject(sess);
		if (changed)
		{
			me->UpdateImgStat();
			me->navi->RedrawMap();
		}
	}
	else if (me->downType == 8)
	{
		Bool changed = false;
		Double x1;
		Double y1;
		Double x2;
		Double y2;
		me->navi->ScnXY2MapXY(x, y, &x1, &y1);

		void *sess = me->lyr->BeginGetObject();
		Math::VectorImage *img = (Math::VectorImage*)me->lyr->GetVectorById(sess, me->currImage);
		if (img)
		{
			if (y1 > me->imgMinY)
			{
				y2 = y1;
				y1 = me->imgMinY;
			}
			else
			{
				y2 = me->imgMinY;
			}
			if (x1 < me->imgMaxX)
			{
				x2 = me->imgMaxX;
			}
			else
			{
				x2 = x1;
				x1 = me->imgMaxX;
			}
			img->SetBounds(x1, y1, x2, y2);
			me->lyr->ReplaceVector(me->currImage, img);
			changed = true;
		}
		me->lyr->EndGetObject(sess);
		if (changed)
		{
			me->UpdateImgStat();
			me->navi->RedrawMap();
		}
	}
	else if (me->downType == 9)
	{
		Bool changed = false;
		Double x1;
		Double y1;
		Double x2;
		Double y2;
		me->navi->ScnXY2MapXY(0, 0, &x1, &y1);
		me->navi->ScnXY2MapXY(x - me->downX, y - me->downY, &x2, &y2);

		void *sess = me->lyr->BeginGetObject();
		Math::VectorImage *img = (Math::VectorImage*)me->lyr->GetVectorById(sess, me->currImage);
		if (img)
		{
			img->SetBounds(me->imgMinX + x2 - x1, me->imgMinY + y2 - y1, me->imgMaxX + x2 - x1, me->imgMaxY + y2 - y1);
			me->lyr->ReplaceVector(me->currImage, img);
			changed = true;
		}
		me->lyr->EndGetObject(sess);
		if (changed)
		{
			me->UpdateImgStat();
			me->navi->RedrawMap();
		}
	}
	me->navi->SetSelectedVector(0);
	me->downType = 0;
	return false;
}

Bool __stdcall SSWR::AVIRead::AVIRGISEditImageForm::OnMouseMove(void *userObj, OSInt x, OSInt y)
{
	SSWR::AVIRead::AVIRGISEditImageForm *me = (SSWR::AVIRead::AVIRGISEditImageForm*)userObj;
	if (me->currImage == -1)
		return false;
	if (me->downType == 1)
	{
		Double x1;
		Double y1;
		me->navi->ScnXY2MapXY(x, y, &x1, &y1);

		Math::Polygon *pg;
		Double *ptList;
		UOSInt nPoints;
		NEW_CLASS(pg, Math::Polygon(me->navi->GetSRID(), 1, 5));
		ptList = pg->GetPointList(&nPoints);
		ptList[0] = me->imgMinX;
		ptList[1] = y1;
		ptList[2] = me->imgMaxX;
		ptList[3] = y1;
		ptList[4] = me->imgMaxX;
		ptList[5] = me->imgMinY;
		ptList[6] = me->imgMinX;
		ptList[7] = me->imgMinY;
		ptList[8] = me->imgMinX;
		ptList[9] = y1;

		me->navi->SetSelectedVector(pg);
	}
	else if (me->downType == 2)
	{
		Double x1;
		Double y1;
		me->navi->ScnXY2MapXY(x, y, &x1, &y1);

		Math::Polygon *pg;
		Double *ptList;
		UOSInt nPoints;
		NEW_CLASS(pg, Math::Polygon(me->navi->GetSRID(), 1, 5));
		ptList = pg->GetPointList(&nPoints);
		ptList[0] = me->imgMinX;
		ptList[1] = y1;
		ptList[2] = x1;
		ptList[3] = y1;
		ptList[4] = x1;
		ptList[5] = me->imgMinY;
		ptList[6] = me->imgMinX;
		ptList[7] = me->imgMinY;
		ptList[8] = me->imgMinX;
		ptList[9] = y1;

		me->navi->SetSelectedVector(pg);
	}
	else if (me->downType == 3)
	{
		Double x1;
		Double y1;
		me->navi->ScnXY2MapXY(x, y, &x1, &y1);

		Math::Polygon *pg;
		Double *ptList;
		UOSInt nPoints;
		NEW_CLASS(pg, Math::Polygon(me->navi->GetSRID(), 1, 5));
		ptList = pg->GetPointList(&nPoints);
		ptList[0] = me->imgMinX;
		ptList[1] = me->imgMaxY;
		ptList[2] = x1;
		ptList[3] = me->imgMaxY;
		ptList[4] = x1;
		ptList[5] = me->imgMinY;
		ptList[6] = me->imgMinX;
		ptList[7] = me->imgMinY;
		ptList[8] = me->imgMinX;
		ptList[9] = me->imgMaxY;

		me->navi->SetSelectedVector(pg);
	}
	else if (me->downType == 4)
	{
		Double x1;
		Double y1;
		me->navi->ScnXY2MapXY(x, y, &x1, &y1);

		Math::Polygon *pg;
		Double *ptList;
		UOSInt nPoints;
		NEW_CLASS(pg, Math::Polygon(me->navi->GetSRID(), 1, 5));
		ptList = pg->GetPointList(&nPoints);
		ptList[0] = me->imgMinX;
		ptList[1] = me->imgMaxY;
		ptList[2] = x1;
		ptList[3] = me->imgMaxY;
		ptList[4] = x1;
		ptList[5] = y1;
		ptList[6] = me->imgMinX;
		ptList[7] = y1;
		ptList[8] = me->imgMinX;
		ptList[9] = me->imgMaxY;

		me->navi->SetSelectedVector(pg);
	}
	else if (me->downType == 5)
	{
		Double x1;
		Double y1;
		me->navi->ScnXY2MapXY(x, y, &x1, &y1);

		Math::Polygon *pg;
		Double *ptList;
		UOSInt nPoints;
		NEW_CLASS(pg, Math::Polygon(me->navi->GetSRID(), 1, 5));
		ptList = pg->GetPointList(&nPoints);
		ptList[0] = me->imgMinX;
		ptList[1] = me->imgMaxY;
		ptList[2] = me->imgMaxX;
		ptList[3] = me->imgMaxY;
		ptList[4] = me->imgMaxX;
		ptList[5] = y1;
		ptList[6] = me->imgMinX;
		ptList[7] = y1;
		ptList[8] = me->imgMinX;
		ptList[9] = me->imgMaxY;

		me->navi->SetSelectedVector(pg);
	}
	else if (me->downType == 6)
	{
		Double x1;
		Double y1;
		me->navi->ScnXY2MapXY(x, y, &x1, &y1);

		Math::Polygon *pg;
		Double *ptList;
		UOSInt nPoints;
		NEW_CLASS(pg, Math::Polygon(me->navi->GetSRID(), 1, 5));
		ptList = pg->GetPointList(&nPoints);
		ptList[0] = x1;
		ptList[1] = me->imgMaxY;
		ptList[2] = me->imgMaxX;
		ptList[3] = me->imgMaxY;
		ptList[4] = me->imgMaxX;
		ptList[5] = y1;
		ptList[6] = x1;
		ptList[7] = y1;
		ptList[8] = x1;
		ptList[9] = me->imgMaxY;

		me->navi->SetSelectedVector(pg);
	}
	else if (me->downType == 7)
	{
		Double x1;
		Double y1;
		me->navi->ScnXY2MapXY(x, y, &x1, &y1);

		Math::Polygon *pg;
		Double *ptList;
		UOSInt nPoints;
		NEW_CLASS(pg, Math::Polygon(me->navi->GetSRID(), 1, 5));
		ptList = pg->GetPointList(&nPoints);
		ptList[0] = x1;
		ptList[1] = me->imgMaxY;
		ptList[2] = me->imgMaxX;
		ptList[3] = me->imgMaxY;
		ptList[4] = me->imgMaxX;
		ptList[5] = me->imgMinY;
		ptList[6] = x1;
		ptList[7] = me->imgMinY;
		ptList[8] = x1;
		ptList[9] = me->imgMaxY;

		me->navi->SetSelectedVector(pg);
	}
	else if (me->downType == 8)
	{
		Double x1;
		Double y1;
		me->navi->ScnXY2MapXY(x, y, &x1, &y1);

		Math::Polygon *pg;
		Double *ptList;
		UOSInt nPoints;
		NEW_CLASS(pg, Math::Polygon(me->navi->GetSRID(), 1, 5));
		ptList = pg->GetPointList(&nPoints);
		ptList[0] = x1;
		ptList[1] = y1;
		ptList[2] = me->imgMaxX;
		ptList[3] = y1;
		ptList[4] = me->imgMaxX;
		ptList[5] = me->imgMinY;
		ptList[6] = x1;
		ptList[7] = me->imgMinY;
		ptList[8] = x1;
		ptList[9] = y1;

		me->navi->SetSelectedVector(pg);
	}
	else if (me->downType == 9)
	{
		Double x1;
		Double y1;
		Double x2;
		Double y2;
		me->navi->ScnXY2MapXY(0, 0, &x1, &y1);
		me->navi->ScnXY2MapXY(x - me->downX, y - me->downY, &x2, &y2);

		Math::Polygon *pg;
		Double *ptList;
		UOSInt nPoints;
		NEW_CLASS(pg, Math::Polygon(me->navi->GetSRID(), 1, 5));
		ptList = pg->GetPointList(&nPoints);
		ptList[0] = me->imgMinX + x2 - x1;
		ptList[1] = me->imgMaxY + y2 - y1;
		ptList[2] = me->imgMaxX + x2 - x1;
		ptList[3] = me->imgMaxY + y2 - y1;
		ptList[4] = me->imgMaxX + x2 - x1;
		ptList[5] = me->imgMinY + y2 - y1;
		ptList[6] = me->imgMinX + x2 - x1;
		ptList[7] = me->imgMinY + y2 - y1;
		ptList[8] = me->imgMinX + x2 - x1;
		ptList[9] = me->imgMaxY + y2 - y1;

		me->navi->SetSelectedVector(pg);
	}
	else
	{
		if (me->chkEdit->IsChecked())
		{
			Int32 downType = me->CalcDownType(x, y);
			switch (downType)
			{
			case 1:
			case 5:
				me->navi->SetMapCursor(UI::GUIControl::CT_SIZEV);
				break;
			case 3:
			case 7:
				me->navi->SetMapCursor(UI::GUIControl::CT_SIZEH);
				break;
			case 2:
			case 6:
				me->navi->SetMapCursor(UI::GUIControl::CT_SIZENE);
				break;
			case 4:
			case 8:
				me->navi->SetMapCursor(UI::GUIControl::CT_SIZENW);
				break;
			case 9:
				me->navi->SetMapCursor(UI::GUIControl::CT_HAND);
				break;
			default:
				me->navi->SetMapCursor(UI::GUIControl::CT_ARROW);
				break;
			}
		}
		else
		{
			me->navi->SetMapCursor(UI::GUIControl::CT_ARROW);
		}
	}
	return false;
}

Int32 SSWR::AVIRead::AVIRGISEditImageForm::CalcDownType(OSInt x, OSInt y)
{
	if (this->currImage == -1)
		return 0;
	Int32 hPos = 0;
	Int32 vPos = 0;
	OSInt left;
	OSInt top;
	OSInt right;
	OSInt bottom;
	this->navi->MapXY2ScnXY(this->imgMinX, this->imgMaxY, &left, &top);
	this->navi->MapXY2ScnXY(this->imgMaxX, this->imgMinY, &right, &bottom);
	if (x >= left - 5 && x <= left + 5)
	{
		hPos = 1;
	}
	else if (x >= right - 5 && x <= right + 5)
	{
		hPos = 2;
	}
	else if (x >= left && x <= right)
	{
		hPos = 3;
	}
	else
	{
		hPos = 0;
	}

	if (y >= top - 5 && y <= top + 5)
	{
		vPos = 1;
	}
	else if (y >= bottom - 5 && y <= bottom + 5)
	{
		vPos = 2;
	}
	else if (y >= top && y <= bottom)
	{
		vPos = 3;
	}
	else
	{
		vPos = 0;
	}
	if (hPos == 3 && vPos == 3)
	{
		return 9;
	}
	else if (hPos == 2 && vPos == 1)
	{
		return 2;
	}
	else if (hPos == 2 && vPos == 2)
	{
		return 4;
	}
	else if (hPos == 1 && vPos == 2)
	{
		return 6;
	}
	else if (hPos == 1 && vPos == 1)
	{
		return 8;
	}
	else if (vPos == 1)
	{
		return 1;
	}
	else if (hPos == 2)
	{
		return 3;
	}
	else if (vPos == 2)
	{
		return 5;
	}
	else if (hPos == 1)
	{
		return 7;
	}
	else
	{
		return 0;
	}
}

SSWR::AVIRead::AVIRGISEditImageForm::AVIRGISEditImageForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Map::VectorLayer *lyr, IMapNavigator *navi) : UI::GUIForm(parent, 416, 408, ui)
{
	Text::StringBuilderUTF8 sb;
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->lyr = lyr;
	this->navi = navi;
	sb.AppendC(UTF8STRC("Edit Image - "));
	sb.Append(lyr->GetSourceNameObj());
	SetText(sb.ToString());
	SetFont(0, 8.25, false);
	this->currImage = -1;
	this->downType = 0;

	NEW_CLASS(this->pnlCtrl, UI::GUIPanel(ui, this));
	this->pnlCtrl->SetRect(0, 0, 100, 23, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->chkAutoPan, UI::GUICheckBox(ui, this->pnlCtrl, (const UTF8Char*)"Auto Pan", true));
	this->chkAutoPan->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->chkEdit, UI::GUICheckBox(ui, this->pnlCtrl, (const UTF8Char*)"Edit Mode", true));
	this->chkEdit->SetRect(120, 0, 100, 23, false);
	NEW_CLASS(this->lbImages, UI::GUIListBox(ui, this, false));
	this->lbImages->SetRect(0, 0, 200, 10, false);
	this->lbImages->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbImages->HandleSelectionChange(OnImageChg, this);
	NEW_CLASS(this->hspMain, UI::GUIHSplitter(ui, this, 3, false));
	NEW_CLASS(this->pnlMain, UI::GUIPanel(ui, this));
	this->pnlMain->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->lblLeft, UI::GUILabel(ui, this->pnlMain, (const UTF8Char*)"Left"));
	this->lblLeft->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->txtLeft, UI::GUITextBox(ui, this->pnlMain, (const UTF8Char*)""));
	this->txtLeft->SetRect(100, 0, 100, 23, false);
	this->txtLeft->SetReadOnly(true);
	NEW_CLASS(this->lblTop, UI::GUILabel(ui, this->pnlMain, (const UTF8Char*)"Top"));
	this->lblTop->SetRect(0, 24, 100, 23, false);
	NEW_CLASS(this->txtTop, UI::GUITextBox(ui, this->pnlMain, (const UTF8Char*)""));
	this->txtTop->SetRect(100, 24, 100, 23, false);
	this->txtTop->SetReadOnly(true);
	NEW_CLASS(this->lblRight, UI::GUILabel(ui, this->pnlMain, (const UTF8Char*)"Right"));
	this->lblRight->SetRect(0, 48, 100, 23, false);
	NEW_CLASS(this->txtRight, UI::GUITextBox(ui, this->pnlMain, (const UTF8Char*)""));
	this->txtRight->SetRect(100, 48, 100, 23, false);
	this->txtRight->SetReadOnly(true);
	NEW_CLASS(this->lblBottom, UI::GUILabel(ui, this->pnlMain, (const UTF8Char*)"Bottom"));
	this->lblBottom->SetRect(0, 72, 100, 23, false);
	NEW_CLASS(this->txtBottom, UI::GUITextBox(ui, this->pnlMain, (const UTF8Char*)""));
	this->txtBottom->SetRect(100, 72, 100, 23, false);
	this->txtBottom->SetReadOnly(true);

	Data::ArrayListInt64 objIds;
	void *nameArr;
	UTF8Char sbuff[256];
	UOSInt cnt = lyr->GetColumnCnt();
	UOSInt i;
	UOSInt j;
	lyr->GetAllObjectIds(&objIds, &nameArr);
	if (cnt > 0)
	{
		i = 0;
		j = objIds.GetCount();
		while (i < j)
		{
			lyr->GetString(sbuff, sizeof(sbuff), nameArr, objIds.GetItem(i), 0);
			this->lbImages->AddItem(sbuff, (void*)(OSInt)objIds.GetItem(i));
			i++;
		}
	}
	else
	{
		i = 0;
		j = objIds.GetCount();
		while (i < j)
		{
			Text::StrInt32(Text::StrConcat(sbuff, (const UTF8Char*)"Image "), (Int32)i);
			this->lbImages->AddItem(sbuff, (void*)(OSInt)objIds.GetItem(i));
			i++;
		}
	}
	lyr->ReleaseNameArr(nameArr);
	this->navi->HandleMapMouseDown(OnMouseDown, this);
	this->navi->HandleMapMouseUp(OnMouseUp, this);
	this->navi->HandleMapMouseMove(OnMouseMove, this);
}

SSWR::AVIRead::AVIRGISEditImageForm::~AVIRGISEditImageForm()
{
	this->navi->HideMarker();
	this->navi->SetSelectedVector(0);
	this->navi->UnhandleMapMouse(this);
}

void SSWR::AVIRead::AVIRGISEditImageForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
