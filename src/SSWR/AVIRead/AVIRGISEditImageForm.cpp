#include "Stdafx.h"
#include "Math/Math.h"
#include "Math/Geometry/VectorImage.h"
#include "SSWR/AVIRead/AVIRGISEditImageForm.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRGISEditImageForm::OnImageChg(void *userObj)
{
	SSWR::AVIRead::AVIRGISEditImageForm *me = (SSWR::AVIRead::AVIRGISEditImageForm*)userObj;
	me->currImage = (OSInt)me->lbImages->GetSelectedItem();
	me->UpdateImgStat();
	if (me->chkAutoPan->IsChecked())
	{
		Math::Coord2DDbl pt = (me->imgMin + me->imgMax) * 0.5;
		me->navi->PanToMap(pt);
	}
}

void SSWR::AVIRead::AVIRGISEditImageForm::UpdateImgStat()
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Math::RectAreaDbl bounds;
	Map::GetObjectSess *sess = this->lyr->BeginGetObject();
	Math::Geometry::Vector2D *vec = this->lyr->GetNewVectorById(sess, this->currImage);
	if (vec)
	{
		bounds = vec->GetBounds();
		sptr = Text::StrDouble(sbuff,  bounds.tl.x);
		this->txtLeft->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, bounds.tl.y);
		this->txtBottom->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, bounds.br.x);
		this->txtRight->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, bounds.br.y);
		this->txtTop->SetText(CSTRP(sbuff, sptr));
		this->imgMin = bounds.tl;
		this->imgMax = bounds.br;
		DEL_CLASS(vec);
	}
	else
	{
		this->currImage = -1;
	}
	this->lyr->EndGetObject(sess);
}

Bool __stdcall SSWR::AVIRead::AVIRGISEditImageForm::OnMouseDown(void *userObj, Math::Coord2D<OSInt> scnPos)
{
	SSWR::AVIRead::AVIRGISEditImageForm *me = (SSWR::AVIRead::AVIRGISEditImageForm*)userObj;
	if (me->currImage == -1)
		return false;
	if (!me->chkEdit->IsChecked())
		return false;
	me->downType = me->CalcDownType(scnPos);
	if (me->downType != 0)
	{
		Math::Geometry::Polygon *pg;
		Math::Coord2DDbl *ptList;
		UOSInt nPoints;
		NEW_CLASS(pg, Math::Geometry::Polygon(me->navi->GetSRID(), 1, 5, false, false));
		ptList = pg->GetPointList(&nPoints);
		ptList[0].x = me->imgMin.x;
		ptList[0].y = me->imgMax.y;
		ptList[1].x = me->imgMax.x;
		ptList[1].y = me->imgMax.y;
		ptList[2].x = me->imgMax.x;
		ptList[2].y = me->imgMin.y;
		ptList[3].x = me->imgMin.x;
		ptList[3].y = me->imgMin.y;
		ptList[4].x = me->imgMin.x;
		ptList[4].y = me->imgMax.y;

		me->navi->SetSelectedVector(pg);
		me->downPos = scnPos;
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall SSWR::AVIRead::AVIRGISEditImageForm::OnMouseUp(void *userObj, Math::Coord2D<OSInt> scnPos)
{
	SSWR::AVIRead::AVIRGISEditImageForm *me = (SSWR::AVIRead::AVIRGISEditImageForm*)userObj;
	if (me->currImage == -1)
	{
		return false;
	}
	if (me->downType == 1)
	{
		Bool changed = false;
		Double x2;
		Double y2;
		Math::Coord2DDbl pt1 = me->navi->ScnXY2MapXY(scnPos);

		Map::GetObjectSess *sess = me->lyr->BeginGetObject();
		Math::Geometry::VectorImage *img = (Math::Geometry::VectorImage*)me->lyr->GetNewVectorById(sess, me->currImage);
		if (img)
		{
			if (pt1.y > me->imgMin.y)
			{
				y2 = pt1.y;
				pt1.y = me->imgMin.y;
			}
			else
			{
				y2 = me->imgMin.y;
			}
			pt1.x = me->imgMin.x;
			x2 = me->imgMax.x;
			img->SetBounds(pt1.x, pt1.y, x2, y2);
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
		Double x2;
		Double y2;
		Math::Coord2DDbl pt1 = me->navi->ScnXY2MapXY(scnPos);

		Map::GetObjectSess *sess = me->lyr->BeginGetObject();
		Math::Geometry::VectorImage *img = (Math::Geometry::VectorImage*)me->lyr->GetNewVectorById(sess, me->currImage);
		if (img)
		{
			if (pt1.y > me->imgMin.y)
			{
				y2 = pt1.y;
				pt1.y = me->imgMin.y;
			}
			else
			{
				y2 = me->imgMin.y;
			}
			if (pt1.x > me->imgMin.x)
			{
				x2 = pt1.x;
				pt1.x = me->imgMin.x;
			}
			else
			{
				x2 = me->imgMin.x;
			}
			img->SetBounds(pt1.x, pt1.y, x2, y2);
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
		Double x2;
		Double y2;
		Math::Coord2DDbl pt1 = me->navi->ScnXY2MapXY(scnPos);

		Map::GetObjectSess *sess = me->lyr->BeginGetObject();
		Math::Geometry::VectorImage *img = (Math::Geometry::VectorImage*)me->lyr->GetNewVectorById(sess, me->currImage);
		if (img)
		{
			pt1.y = me->imgMin.y;
			y2 = me->imgMax.y;
			if (pt1.x > me->imgMin.x)
			{
				x2 = pt1.x;
				pt1.x = me->imgMin.x;
			}
			else
			{
				x2 = me->imgMin.x;
			}
			img->SetBounds(pt1.x, pt1.y, x2, y2);
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
		Double x2;
		Double y2;
		Math::Coord2DDbl pt1 = me->navi->ScnXY2MapXY(scnPos);

		Map::GetObjectSess *sess = me->lyr->BeginGetObject();
		Math::Geometry::VectorImage *img = (Math::Geometry::VectorImage*)me->lyr->GetNewVectorById(sess, me->currImage);
		if (img)
		{
			if (pt1.y < me->imgMax.y)
			{
				y2 = me->imgMax.y;
			}
			else
			{
				y2 = pt1.y;
				pt1.y = me->imgMax.y;
			}
			if (pt1.x > me->imgMin.x)
			{
				x2 = pt1.x;
				pt1.x = me->imgMin.x;
			}
			else
			{
				x2 = me->imgMin.x;
			}
			img->SetBounds(pt1.x, pt1.y, x2, y2);
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
		Double x2;
		Double y2;
		Math::Coord2DDbl pt1 = me->navi->ScnXY2MapXY(scnPos);

		Map::GetObjectSess *sess = me->lyr->BeginGetObject();
		Math::Geometry::VectorImage *img = (Math::Geometry::VectorImage*)me->lyr->GetNewVectorById(sess, me->currImage);
		if (img)
		{
			if (pt1.y < me->imgMax.y)
			{
				y2 = me->imgMax.y;
			}
			else
			{
				y2 = pt1.y;
				pt1.y = me->imgMax.y;
			}
			pt1.x = me->imgMin.x;
			x2 = me->imgMax.x;
			img->SetBounds(pt1.x, pt1.y, x2, y2);
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
		Double x2;
		Double y2;
		Math::Coord2DDbl pt1 = me->navi->ScnXY2MapXY(scnPos);

		Map::GetObjectSess *sess = me->lyr->BeginGetObject();
		Math::Geometry::VectorImage *img = (Math::Geometry::VectorImage*)me->lyr->GetNewVectorById(sess, me->currImage);
		if (img)
		{
			if (pt1.y < me->imgMax.y)
			{
				y2 = me->imgMax.y;
			}
			else
			{
				y2 = pt1.y;
				pt1.y = me->imgMax.y;
			}
			if (pt1.x < me->imgMax.x)
			{
				x2 = me->imgMax.x;
			}
			else
			{
				x2 = pt1.x;
				pt1.x = me->imgMax.x;
			}
			img->SetBounds(pt1.x, pt1.y, x2, y2);
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
		Double x2;
		Double y2;
		Math::Coord2DDbl pt1 = me->navi->ScnXY2MapXY(scnPos);

		Map::GetObjectSess *sess = me->lyr->BeginGetObject();
		Math::Geometry::VectorImage *img = (Math::Geometry::VectorImage*)me->lyr->GetNewVectorById(sess, me->currImage);
		if (img)
		{
			pt1.y = me->imgMin.y;
			y2 = me->imgMax.y;
			if (pt1.x < me->imgMax.x)
			{
				x2 = me->imgMax.x;
			}
			else
			{
				x2 = pt1.x;
				pt1.x = me->imgMax.x;
			}
			img->SetBounds(pt1.x, pt1.y, x2, y2);
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
		Double x2;
		Double y2;
		Math::Coord2DDbl pt1 = me->navi->ScnXY2MapXY(scnPos);

		Map::GetObjectSess *sess = me->lyr->BeginGetObject();
		Math::Geometry::VectorImage *img = (Math::Geometry::VectorImage*)me->lyr->GetNewVectorById(sess, me->currImage);
		if (img)
		{
			if (pt1.y > me->imgMin.y)
			{
				y2 = pt1.y;
				pt1.y = me->imgMin.y;
			}
			else
			{
				y2 = me->imgMin.y;
			}
			if (pt1.x < me->imgMax.x)
			{
				x2 = me->imgMax.x;
			}
			else
			{
				x2 = pt1.x;
				pt1.x = me->imgMax.x;
			}
			img->SetBounds(pt1.x, pt1.y, x2, y2);
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
		Math::Coord2DDbl pt1 = me->navi->ScnXY2MapXY(Math::Coord2D<OSInt>(0, 0));
		Math::Coord2DDbl pt2 = me->navi->ScnXY2MapXY(scnPos - me->downPos);

		Map::GetObjectSess *sess = me->lyr->BeginGetObject();
		Math::Geometry::VectorImage *img = (Math::Geometry::VectorImage*)me->lyr->GetNewVectorById(sess, me->currImage);
		if (img)
		{
			img->SetBounds(me->imgMin.x + pt2.x - pt1.x, me->imgMin.y + pt2.y - pt1.y, me->imgMax.x + pt2.x - pt1.x, me->imgMax.y + pt2.y - pt1.y);
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

Bool __stdcall SSWR::AVIRead::AVIRGISEditImageForm::OnMouseMove(void *userObj, Math::Coord2D<OSInt> scnPos)
{
	SSWR::AVIRead::AVIRGISEditImageForm *me = (SSWR::AVIRead::AVIRGISEditImageForm*)userObj;
	if (me->currImage == -1)
		return false;
	if (me->downType == 1)
	{
		Math::Coord2DDbl mapPos = me->navi->ScnXY2MapXY(scnPos);

		Math::Geometry::Polygon *pg;
		Math::Coord2DDbl *ptList;
		UOSInt nPoints;
		NEW_CLASS(pg, Math::Geometry::Polygon(me->navi->GetSRID(), 1, 5, false, false));
		ptList = pg->GetPointList(&nPoints);
		ptList[0].x = me->imgMin.x;
		ptList[0].y = mapPos.y;
		ptList[1].x = me->imgMax.x;
		ptList[1].y = mapPos.y;
		ptList[2].x = me->imgMax.x;
		ptList[2].y = me->imgMin.y;
		ptList[3].x = me->imgMin.x;
		ptList[3].y = me->imgMin.y;
		ptList[4].x = me->imgMin.x;
		ptList[4].y = mapPos.y;

		me->navi->SetSelectedVector(pg);
	}
	else if (me->downType == 2)
	{
		Math::Coord2DDbl mapPos = me->navi->ScnXY2MapXY(scnPos);

		Math::Geometry::Polygon *pg;
		Math::Coord2DDbl *ptList;
		UOSInt nPoints;
		NEW_CLASS(pg, Math::Geometry::Polygon(me->navi->GetSRID(), 1, 5, false, false));
		ptList = pg->GetPointList(&nPoints);
		ptList[0].x = me->imgMin.x;
		ptList[0].y = mapPos.y;
		ptList[1] = mapPos;
		ptList[2].x = mapPos.x;
		ptList[2].y = me->imgMin.y;
		ptList[3].x = me->imgMin.x;
		ptList[3].y = me->imgMin.y;
		ptList[4].x = me->imgMin.x;
		ptList[4].y = mapPos.y;

		me->navi->SetSelectedVector(pg);
	}
	else if (me->downType == 3)
	{
		Math::Coord2DDbl mapPos = me->navi->ScnXY2MapXY(scnPos);

		Math::Geometry::Polygon *pg;
		Math::Coord2DDbl *ptList;
		UOSInt nPoints;
		NEW_CLASS(pg, Math::Geometry::Polygon(me->navi->GetSRID(), 1, 5, false, false));
		ptList = pg->GetPointList(&nPoints);
		ptList[0].x = me->imgMin.x;
		ptList[0].y = me->imgMax.y;
		ptList[1].x = mapPos.x;
		ptList[1].y = me->imgMax.y;
		ptList[2].x = mapPos.x;
		ptList[2].y = me->imgMin.y;
		ptList[3].x = me->imgMin.x;
		ptList[3].y = me->imgMin.y;
		ptList[4].x = me->imgMin.x;
		ptList[4].y = me->imgMax.y;

		me->navi->SetSelectedVector(pg);
	}
	else if (me->downType == 4)
	{
		Math::Coord2DDbl mapPos = me->navi->ScnXY2MapXY(scnPos);

		Math::Geometry::Polygon *pg;
		Math::Coord2DDbl *ptList;
		UOSInt nPoints;
		NEW_CLASS(pg, Math::Geometry::Polygon(me->navi->GetSRID(), 1, 5, false, false));
		ptList = pg->GetPointList(&nPoints);
		ptList[0].x = me->imgMin.x;
		ptList[0].y = me->imgMax.y;
		ptList[1].x = mapPos.x;
		ptList[1].y = me->imgMax.y;
		ptList[2] = mapPos;
		ptList[3].x = me->imgMin.x;
		ptList[3].y = mapPos.y;
		ptList[4].x = me->imgMin.x;
		ptList[4].y = me->imgMax.y;

		me->navi->SetSelectedVector(pg);
	}
	else if (me->downType == 5)
	{
		Math::Coord2DDbl mapPos = me->navi->ScnXY2MapXY(scnPos);

		Math::Geometry::Polygon *pg;
		Math::Coord2DDbl *ptList;
		UOSInt nPoints;
		NEW_CLASS(pg, Math::Geometry::Polygon(me->navi->GetSRID(), 1, 5, false, false));
		ptList = pg->GetPointList(&nPoints);
		ptList[0].x = me->imgMin.x;
		ptList[0].y = me->imgMax.y;
		ptList[1].x = me->imgMax.x;
		ptList[1].y = me->imgMax.y;
		ptList[2].x = me->imgMax.x;
		ptList[2].y = mapPos.y;
		ptList[3].x = me->imgMin.x;
		ptList[3].y = mapPos.y;
		ptList[4].x = me->imgMin.x;
		ptList[4].y = me->imgMax.y;

		me->navi->SetSelectedVector(pg);
	}
	else if (me->downType == 6)
	{
		Math::Coord2DDbl mapPos = me->navi->ScnXY2MapXY(scnPos);

		Math::Geometry::Polygon *pg;
		Math::Coord2DDbl *ptList;
		UOSInt nPoints;
		NEW_CLASS(pg, Math::Geometry::Polygon(me->navi->GetSRID(), 1, 5, false, false));
		ptList = pg->GetPointList(&nPoints);
		ptList[0].x = mapPos.x;
		ptList[0].y = me->imgMax.y;
		ptList[1].x = me->imgMax.x;
		ptList[1].y = me->imgMax.y;
		ptList[2].x = me->imgMax.x;
		ptList[2].y = mapPos.y;
		ptList[3] = mapPos;
		ptList[4].x = mapPos.x;
		ptList[4].y = me->imgMax.y;

		me->navi->SetSelectedVector(pg);
	}
	else if (me->downType == 7)
	{
		Math::Coord2DDbl mapPos = me->navi->ScnXY2MapXY(scnPos);

		Math::Geometry::Polygon *pg;
		Math::Coord2DDbl *ptList;
		UOSInt nPoints;
		NEW_CLASS(pg, Math::Geometry::Polygon(me->navi->GetSRID(), 1, 5, false, false));
		ptList = pg->GetPointList(&nPoints);
		ptList[0].x = mapPos.x;
		ptList[0].y = me->imgMax.y;
		ptList[1].x = me->imgMax.x;
		ptList[1].y = me->imgMax.y;
		ptList[2].x = me->imgMax.x;
		ptList[2].y = me->imgMin.y;
		ptList[3].x = mapPos.x;
		ptList[3].y = me->imgMin.y;
		ptList[4].x = mapPos.x;
		ptList[4].y = me->imgMax.y;

		me->navi->SetSelectedVector(pg);
	}
	else if (me->downType == 8)
	{
		Math::Coord2DDbl mapPos = me->navi->ScnXY2MapXY(scnPos);

		Math::Geometry::Polygon *pg;
		Math::Coord2DDbl *ptList;
		UOSInt nPoints;
		NEW_CLASS(pg, Math::Geometry::Polygon(me->navi->GetSRID(), 1, 5, false, false));
		ptList = pg->GetPointList(&nPoints);
		ptList[0] = mapPos;
		ptList[1].x = me->imgMax.x;
		ptList[1].y = mapPos.y;
		ptList[2].x = me->imgMax.x;
		ptList[2].y = me->imgMin.y;
		ptList[3].x = mapPos.x;
		ptList[3].y = me->imgMin.y;
		ptList[4] = mapPos;

		me->navi->SetSelectedVector(pg);
	}
	else if (me->downType == 9)
	{
		Math::Coord2DDbl pt1 = me->navi->ScnXY2MapXY(Math::Coord2D<OSInt>(0, 0));
		Math::Coord2DDbl pt2 = me->navi->ScnXY2MapXY(scnPos - me->downPos);

		Math::Geometry::Polygon *pg;
		Math::Coord2DDbl *ptList;
		UOSInt nPoints;
		NEW_CLASS(pg, Math::Geometry::Polygon(me->navi->GetSRID(), 1, 5, false, false));
		ptList = pg->GetPointList(&nPoints);
		ptList[0].x = me->imgMin.x + pt2.x - pt1.x;
		ptList[0].y = me->imgMax.y + pt2.y - pt1.y;
		ptList[1].x = me->imgMax.x + pt2.x - pt1.x;
		ptList[1].y = me->imgMax.y + pt2.y - pt1.y;
		ptList[2].x = me->imgMax.x + pt2.x - pt1.x;
		ptList[2].y = me->imgMin.y + pt2.y - pt1.y;
		ptList[3].x = me->imgMin.x + pt2.x - pt1.x;
		ptList[3].y = me->imgMin.y + pt2.y - pt1.y;
		ptList[4].x = me->imgMin.x + pt2.x - pt1.x;
		ptList[4].y = me->imgMax.y + pt2.y - pt1.y;

		me->navi->SetSelectedVector(pg);
	}
	else
	{
		if (me->chkEdit->IsChecked())
		{
			Int32 downType = me->CalcDownType(scnPos);
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

Int32 SSWR::AVIRead::AVIRGISEditImageForm::CalcDownType(Math::Coord2D<OSInt> scnPos)
{
	if (this->currImage == -1)
		return 0;
	Int32 hPos = 0;
	Int32 vPos = 0;
	Math::Coord2D<OSInt> tl;
	Math::Coord2D<OSInt> br;
	tl = this->navi->MapXY2ScnXY(Math::Coord2DDbl(this->imgMin.x, this->imgMax.y));
	br = this->navi->MapXY2ScnXY(Math::Coord2DDbl(this->imgMax.x, this->imgMin.y));
	if (scnPos.x >= tl.x - 5 && scnPos.x <= tl.x + 5)
	{
		hPos = 1;
	}
	else if (scnPos.x >= br.x - 5 && scnPos.x <= br.x + 5)
	{
		hPos = 2;
	}
	else if (scnPos.x >= tl.x && scnPos.x <= br.x)
	{
		hPos = 3;
	}
	else
	{
		hPos = 0;
	}

	if (scnPos.y >= tl.y - 5 && scnPos.y <= tl.y + 5)
	{
		vPos = 1;
	}
	else if (scnPos.y >= br.y - 5 && scnPos.y <= br.y + 5)
	{
		vPos = 2;
	}
	else if (scnPos.y >= tl.y && scnPos.y <= br.y)
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

SSWR::AVIRead::AVIRGISEditImageForm::AVIRGISEditImageForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Map::VectorLayer *lyr, IMapNavigator *navi) : UI::GUIForm(parent, 416, 408, ui)
{
	Text::StringBuilderUTF8 sb;
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->lyr = lyr;
	this->navi = navi;
	sb.AppendC(UTF8STRC("Edit Image - "));
	sb.Append(lyr->GetSourceNameObj());
	this->SetText(sb.ToCString());
	this->SetFont(0, 0, 8.25, false);
	this->currImage = -1;
	this->downType = 0;

	NEW_CLASS(this->pnlCtrl, UI::GUIPanel(ui, this));
	this->pnlCtrl->SetRect(0, 0, 100, 23, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->chkAutoPan, UI::GUICheckBox(ui, this->pnlCtrl, CSTR("Auto Pan"), true));
	this->chkAutoPan->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->chkEdit, UI::GUICheckBox(ui, this->pnlCtrl, CSTR("Edit Mode"), true));
	this->chkEdit->SetRect(120, 0, 100, 23, false);
	NEW_CLASS(this->lbImages, UI::GUIListBox(ui, this, false));
	this->lbImages->SetRect(0, 0, 200, 10, false);
	this->lbImages->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbImages->HandleSelectionChange(OnImageChg, this);
	NEW_CLASS(this->hspMain, UI::GUIHSplitter(ui, this, 3, false));
	NEW_CLASS(this->pnlMain, UI::GUIPanel(ui, this));
	this->pnlMain->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->lblLeft, UI::GUILabel(ui, this->pnlMain, CSTR("Left")));
	this->lblLeft->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->txtLeft, UI::GUITextBox(ui, this->pnlMain, CSTR("")));
	this->txtLeft->SetRect(100, 0, 100, 23, false);
	this->txtLeft->SetReadOnly(true);
	NEW_CLASS(this->lblTop, UI::GUILabel(ui, this->pnlMain, CSTR("Top")));
	this->lblTop->SetRect(0, 24, 100, 23, false);
	NEW_CLASS(this->txtTop, UI::GUITextBox(ui, this->pnlMain, CSTR("")));
	this->txtTop->SetRect(100, 24, 100, 23, false);
	this->txtTop->SetReadOnly(true);
	NEW_CLASS(this->lblRight, UI::GUILabel(ui, this->pnlMain, CSTR("Right")));
	this->lblRight->SetRect(0, 48, 100, 23, false);
	NEW_CLASS(this->txtRight, UI::GUITextBox(ui, this->pnlMain, CSTR("")));
	this->txtRight->SetRect(100, 48, 100, 23, false);
	this->txtRight->SetReadOnly(true);
	NEW_CLASS(this->lblBottom, UI::GUILabel(ui, this->pnlMain, CSTR("Bottom")));
	this->lblBottom->SetRect(0, 72, 100, 23, false);
	NEW_CLASS(this->txtBottom, UI::GUITextBox(ui, this->pnlMain, CSTR("")));
	this->txtBottom->SetRect(100, 72, 100, 23, false);
	this->txtBottom->SetReadOnly(true);

	Data::ArrayListInt64 objIds;
	Map::NameArray *nameArr;
	UTF8Char sbuff[256];
	UTF8Char *sptr;
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
			sptr = lyr->GetString(sbuff, sizeof(sbuff), nameArr, objIds.GetItem(i), 0);
			this->lbImages->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)objIds.GetItem(i));
			i++;
		}
	}
	else
	{
		i = 0;
		j = objIds.GetCount();
		while (i < j)
		{
			sptr = Text::StrInt32(Text::StrConcatC(sbuff, UTF8STRC("Image ")), (Int32)i);
			this->lbImages->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)objIds.GetItem(i));
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
