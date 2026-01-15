#include "Stdafx.h"
#include "Math/Math_C.h"
#include "Math/Geometry/Polygon.h"
#include "Math/Geometry/VectorImage.h"
#include "SSWR/AVIRead/AVIRGISEditImageForm.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRGISEditImageForm::OnImageChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISEditImageForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISEditImageForm>();
	me->currImage = (OSInt)me->lbImages->GetSelectedItem().p;
	me->UpdateImgStat();
	if (me->chkAutoPan->IsChecked())
	{
		Math::Coord2DDbl pt = (me->imgMin + me->imgMax) * 0.5;
		NN<Math::CoordinateSystem> lyrCsys = me->lyr->GetCoordinateSystem();
		NN<Math::CoordinateSystem> envCsys = me->navi->GetCoordinateSystem();
		if (!lyrCsys->Equals(envCsys))
		{
			pt = Math::CoordinateSystem::Convert(lyrCsys, envCsys, pt);
		}
		me->navi->PanToMap(pt);
	}
	NN<Map::GetObjectSess> sess = me->lyr->BeginGetObject();
	NN<Math::Geometry::VectorImage> img;
	if (Optional<Math::Geometry::VectorImage>::ConvertFrom(me->lyr->GetNewVectorById(sess, me->currImage)).SetTo(img))
	{
		if (img->HasSrcAlpha())
		{
			UTF8Char sbuff[64];
			UnsafeArray<UTF8Char> sptr;
			UOSInt scrollPos = (UOSInt)Double2OSInt(img->GetSrcAlpha() * 200);
			me->trkAlpha->SetPos(scrollPos);
			sptr = Text::StrDouble(sbuff, img->GetSrcAlpha());
			me->txtAlpha->SetText(CSTRP(sbuff, sptr));
		}
		else
		{
			me->trkAlpha->SetPos(200);
			me->txtAlpha->SetText(CSTR("1.0"));
		}
		img.Delete();
	}
	me->lyr->EndGetObject(sess);
}

void SSWR::AVIRead::AVIRGISEditImageForm::UpdateImgStat()
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	Math::RectAreaDbl bounds;
	NN<Map::GetObjectSess> sess = this->lyr->BeginGetObject();
	NN<Math::Geometry::Vector2D> vec;
	if (this->lyr->GetNewVectorById(sess, this->currImage).SetTo(vec))
	{
		bounds = vec->GetBounds();
		sptr = Text::StrDouble(sbuff,  bounds.min.x);
		this->txtLeft->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, bounds.min.y);
		this->txtBottom->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, bounds.max.x);
		this->txtRight->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, bounds.max.y);
		this->txtTop->SetText(CSTRP(sbuff, sptr));
		this->imgMin = bounds.min;
		this->imgMax = bounds.max;
		vec.Delete();
	}
	else
	{
		this->currImage = -1;
	}
	this->lyr->EndGetObject(sess);
}

UI::EventState __stdcall SSWR::AVIRead::AVIRGISEditImageForm::OnMouseDown(AnyType userObj, Math::Coord2D<OSInt> scnPos)
{
	NN<SSWR::AVIRead::AVIRGISEditImageForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISEditImageForm>();
	if (me->currImage == -1)
		return UI::EventState::ContinueEvent;
	if (!me->chkEdit->IsChecked())
		return UI::EventState::ContinueEvent;
	me->downType = me->CalcDownType(scnPos);
	if (me->downType != 0)
	{
		Math::Geometry::Polygon *pg;
		NN<Math::Geometry::LinearRing> lr;
		UnsafeArray<Math::Coord2DDbl> ptList;
		UOSInt nPoints;
		NEW_CLASS(pg, Math::Geometry::Polygon(me->navi->GetSRID()));
		NEW_CLASSNN(lr, Math::Geometry::LinearRing(me->navi->GetSRID(), 5, false, false))
		ptList = lr->GetPointList(nPoints);
		NN<Math::CoordinateSystem> lyrCsys = me->lyr->GetCoordinateSystem();
		NN<Math::CoordinateSystem> envCsys = me->navi->GetCoordinateSystem();
		if (!lyrCsys->Equals(envCsys))
		{
			ptList[0] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(me->imgMin.x, me->imgMax.y));
			ptList[1] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(me->imgMax.x, me->imgMax.y));
			ptList[2] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(me->imgMax.x, me->imgMin.y));
			ptList[3] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(me->imgMin.x, me->imgMin.y));
			ptList[4] = ptList[0];
		}
		else
		{
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
		}
		pg->AddGeometry(lr);

		me->navi->SetSelectedVector(pg);
		me->downPos = scnPos;
		return UI::EventState::StopEvent;
	}
	else
	{
		return UI::EventState::ContinueEvent;
	}
}

UI::EventState __stdcall SSWR::AVIRead::AVIRGISEditImageForm::OnMouseUp(AnyType userObj, Math::Coord2D<OSInt> scnPos)
{
	NN<SSWR::AVIRead::AVIRGISEditImageForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISEditImageForm>();
	if (me->currImage == -1)
	{
		return UI::EventState::ContinueEvent;
	}
	NN<Math::CoordinateSystem> lyrCsys = me->lyr->GetCoordinateSystem();
	NN<Math::CoordinateSystem> envCsys = me->navi->GetCoordinateSystem();
	if (me->downType == 1)
	{
		Bool changed = false;
		Double x2;
		Double y2;
		Math::Coord2DDbl pt1 = me->navi->ScnXY2MapXY(scnPos);
		if (!lyrCsys->Equals(envCsys))
		{
			pt1 = Math::CoordinateSystem::Convert(envCsys, lyrCsys, pt1);
		}

		NN<Map::GetObjectSess> sess = me->lyr->BeginGetObject();
		NN<Math::Geometry::VectorImage> img;
		if (Optional<Math::Geometry::VectorImage>::ConvertFrom(me->lyr->GetNewVectorById(sess, me->currImage)).SetTo(img))
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
		if (!lyrCsys->Equals(envCsys))
		{
			pt1 = Math::CoordinateSystem::Convert(envCsys, lyrCsys, pt1);
		}

		NN<Map::GetObjectSess> sess = me->lyr->BeginGetObject();
		NN<Math::Geometry::VectorImage> img;
		if (Optional<Math::Geometry::VectorImage>::ConvertFrom(me->lyr->GetNewVectorById(sess, me->currImage)).SetTo(img))
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
		if (!lyrCsys->Equals(envCsys))
		{
			pt1 = Math::CoordinateSystem::Convert(envCsys, lyrCsys, pt1);
		}

		NN<Map::GetObjectSess> sess = me->lyr->BeginGetObject();
		NN<Math::Geometry::VectorImage> img;
		if (Optional<Math::Geometry::VectorImage>::ConvertFrom(me->lyr->GetNewVectorById(sess, me->currImage)).SetTo(img))
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
		if (!lyrCsys->Equals(envCsys))
		{
			pt1 = Math::CoordinateSystem::Convert(envCsys, lyrCsys, pt1);
		}

		NN<Map::GetObjectSess> sess = me->lyr->BeginGetObject();
		NN<Math::Geometry::VectorImage> img;
		if (Optional<Math::Geometry::VectorImage>::ConvertFrom(me->lyr->GetNewVectorById(sess, me->currImage)).SetTo(img))
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
		if (!lyrCsys->Equals(envCsys))
		{
			pt1 = Math::CoordinateSystem::Convert(envCsys, lyrCsys, pt1);
		}

		NN<Map::GetObjectSess> sess = me->lyr->BeginGetObject();
		NN<Math::Geometry::VectorImage> img;
		if (Optional<Math::Geometry::VectorImage>::ConvertFrom(me->lyr->GetNewVectorById(sess, me->currImage)).SetTo(img))
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
		if (!lyrCsys->Equals(envCsys))
		{
			pt1 = Math::CoordinateSystem::Convert(envCsys, lyrCsys, pt1);
		}

		NN<Map::GetObjectSess> sess = me->lyr->BeginGetObject();
		NN<Math::Geometry::VectorImage> img;
		if (Optional<Math::Geometry::VectorImage>::ConvertFrom(me->lyr->GetNewVectorById(sess, me->currImage)).SetTo(img))
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
		if (!lyrCsys->Equals(envCsys))
		{
			pt1 = Math::CoordinateSystem::Convert(envCsys, lyrCsys, pt1);
		}

		NN<Map::GetObjectSess> sess = me->lyr->BeginGetObject();
		NN<Math::Geometry::VectorImage> img;
		if (Optional<Math::Geometry::VectorImage>::ConvertFrom(me->lyr->GetNewVectorById(sess, me->currImage)).SetTo(img))
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
		if (!lyrCsys->Equals(envCsys))
		{
			pt1 = Math::CoordinateSystem::Convert(envCsys, lyrCsys, pt1);
		}

		NN<Map::GetObjectSess> sess = me->lyr->BeginGetObject();
		NN<Math::Geometry::VectorImage> img;
		if (Optional<Math::Geometry::VectorImage>::ConvertFrom(me->lyr->GetNewVectorById(sess, me->currImage)).SetTo(img))
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
		if (!lyrCsys->Equals(envCsys))
		{
			pt1 = Math::CoordinateSystem::Convert(envCsys, lyrCsys, pt1);
			pt2 = Math::CoordinateSystem::Convert(envCsys, lyrCsys, pt2);
		}

		NN<Map::GetObjectSess> sess = me->lyr->BeginGetObject();
		NN<Math::Geometry::VectorImage> img;
		if (Optional<Math::Geometry::VectorImage>::ConvertFrom(me->lyr->GetNewVectorById(sess, me->currImage)).SetTo(img))
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
	me->navi->SetSelectedVector(nullptr);
	me->downType = 0;
	return UI::EventState::ContinueEvent;
}

UI::EventState __stdcall SSWR::AVIRead::AVIRGISEditImageForm::OnMouseMove(AnyType userObj, Math::Coord2D<OSInt> scnPos)
{
	NN<SSWR::AVIRead::AVIRGISEditImageForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISEditImageForm>();
	if (me->currImage == -1)
		return UI::EventState::ContinueEvent;
	NN<Math::CoordinateSystem> lyrCsys = me->lyr->GetCoordinateSystem();
	NN<Math::CoordinateSystem> envCsys = me->navi->GetCoordinateSystem();
	if (me->downType == 1)
	{
		Math::Coord2DDbl mapPos = me->navi->ScnXY2MapXY(scnPos);
		if (!lyrCsys->Equals(envCsys))
		{
			mapPos = Math::CoordinateSystem::Convert(envCsys, lyrCsys, mapPos);
		}

		Math::Geometry::Polygon *pg;
		NN<Math::Geometry::LinearRing> lr;
		UnsafeArray<Math::Coord2DDbl> ptList;
		UOSInt nPoints;
		NEW_CLASS(pg, Math::Geometry::Polygon(me->navi->GetSRID()));
		NEW_CLASSNN(lr, Math::Geometry::LinearRing(me->navi->GetSRID(), 5, false, false))
		ptList = lr->GetPointList(nPoints);
		if (!lyrCsys->Equals(envCsys))
		{
			ptList[0] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(me->imgMin.x, mapPos.y));
			ptList[1] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(me->imgMax.x, mapPos.y));
			ptList[2] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(me->imgMax.x, me->imgMin.y));
			ptList[3] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(me->imgMin.x, me->imgMin.y));
			ptList[4] = ptList[0];
		}
		else
		{
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
		}
		pg->AddGeometry(lr);

		me->navi->SetSelectedVector(pg);
	}
	else if (me->downType == 2)
	{
		Math::Coord2DDbl mapPos = me->navi->ScnXY2MapXY(scnPos);
		if (!lyrCsys->Equals(envCsys))
		{
			mapPos = Math::CoordinateSystem::Convert(envCsys, lyrCsys, mapPos);
		}

		Math::Geometry::Polygon *pg;
		NN<Math::Geometry::LinearRing> lr;
		UnsafeArray<Math::Coord2DDbl> ptList;
		UOSInt nPoints;
		NEW_CLASS(pg, Math::Geometry::Polygon(me->navi->GetSRID()));
		NEW_CLASSNN(lr, Math::Geometry::LinearRing(me->navi->GetSRID(), 5, false, false))
		ptList = lr->GetPointList(nPoints);
		if (!lyrCsys->Equals(envCsys))
		{
			ptList[0] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(me->imgMin.x, mapPos.y));
			ptList[1] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, mapPos);
			ptList[2] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(mapPos.x, me->imgMin.y));
			ptList[3] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(me->imgMin.x, me->imgMin.y));
			ptList[4] = ptList[0];
		}
		else
		{
			ptList[0].x = me->imgMin.x;
			ptList[0].y = mapPos.y;
			ptList[1] = mapPos;
			ptList[2].x = mapPos.x;
			ptList[2].y = me->imgMin.y;
			ptList[3].x = me->imgMin.x;
			ptList[3].y = me->imgMin.y;
			ptList[4].x = me->imgMin.x;
			ptList[4].y = mapPos.y;
		}
		pg->AddGeometry(lr);

		me->navi->SetSelectedVector(pg);
	}
	else if (me->downType == 3)
	{
		Math::Coord2DDbl mapPos = me->navi->ScnXY2MapXY(scnPos);
		if (!lyrCsys->Equals(envCsys))
		{
			mapPos = Math::CoordinateSystem::Convert(envCsys, lyrCsys, mapPos);
		}

		Math::Geometry::Polygon *pg;
		NN<Math::Geometry::LinearRing> lr;
		UnsafeArray<Math::Coord2DDbl> ptList;
		UOSInt nPoints;
		NEW_CLASS(pg, Math::Geometry::Polygon(me->navi->GetSRID()));
		NEW_CLASSNN(lr, Math::Geometry::LinearRing(me->navi->GetSRID(), 5, false, false))
		ptList = lr->GetPointList(nPoints);
		if (!lyrCsys->Equals(envCsys))
		{
			ptList[0] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(me->imgMin.x, me->imgMax.y));
			ptList[1] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(mapPos.x, me->imgMax.y));
			ptList[2] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(mapPos.x, me->imgMin.y));
			ptList[3] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(me->imgMin.x, me->imgMin.y));
			ptList[4] = ptList[0];
		}
		else
		{
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
		}
		pg->AddGeometry(lr);

		me->navi->SetSelectedVector(pg);
	}
	else if (me->downType == 4)
	{
		Math::Coord2DDbl mapPos = me->navi->ScnXY2MapXY(scnPos);
		if (!lyrCsys->Equals(envCsys))
		{
			mapPos = Math::CoordinateSystem::Convert(envCsys, lyrCsys, mapPos);
		}

		Math::Geometry::Polygon *pg;
		NN<Math::Geometry::LinearRing> lr;
		UnsafeArray<Math::Coord2DDbl> ptList;
		UOSInt nPoints;
		NEW_CLASS(pg, Math::Geometry::Polygon(me->navi->GetSRID()));
		NEW_CLASSNN(lr, Math::Geometry::LinearRing(me->navi->GetSRID(), 5, false, false))
		ptList = lr->GetPointList(nPoints);
		if (!lyrCsys->Equals(envCsys))
		{
			ptList[0] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(me->imgMin.x, me->imgMax.y));
			ptList[1] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(mapPos.x, me->imgMax.y));
			ptList[2] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, mapPos);
			ptList[3] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(me->imgMin.x, mapPos.y));
			ptList[4] = ptList[0];
		}
		else
		{
			ptList[0].x = me->imgMin.x;
			ptList[0].y = me->imgMax.y;
			ptList[1].x = mapPos.x;
			ptList[1].y = me->imgMax.y;
			ptList[2] = mapPos;
			ptList[3].x = me->imgMin.x;
			ptList[3].y = mapPos.y;
			ptList[4].x = me->imgMin.x;
			ptList[4].y = me->imgMax.y;
		}
		pg->AddGeometry(lr);

		me->navi->SetSelectedVector(pg);
	}
	else if (me->downType == 5)
	{
		Math::Coord2DDbl mapPos = me->navi->ScnXY2MapXY(scnPos);
		if (!lyrCsys->Equals(envCsys))
		{
			mapPos = Math::CoordinateSystem::Convert(envCsys, lyrCsys, mapPos);
		}

		Math::Geometry::Polygon *pg;
		NN<Math::Geometry::LinearRing> lr;
		UnsafeArray<Math::Coord2DDbl> ptList;
		UOSInt nPoints;
		NEW_CLASS(pg, Math::Geometry::Polygon(me->navi->GetSRID()));
		NEW_CLASSNN(lr, Math::Geometry::LinearRing(me->navi->GetSRID(), 5, false, false))
		ptList = lr->GetPointList(nPoints);
		if (!lyrCsys->Equals(envCsys))
		{
			ptList[0] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(me->imgMin.x, me->imgMax.y));
			ptList[1] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(me->imgMax.x, me->imgMax.y));
			ptList[2] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(me->imgMax.x, mapPos.y));
			ptList[3] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(me->imgMin.x, mapPos.y));
			ptList[4] = ptList[0];
		}
		else
		{
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
		}
		pg->AddGeometry(lr);

		me->navi->SetSelectedVector(pg);
	}
	else if (me->downType == 6)
	{
		Math::Coord2DDbl mapPos = me->navi->ScnXY2MapXY(scnPos);
		if (!lyrCsys->Equals(envCsys))
		{
			mapPos = Math::CoordinateSystem::Convert(envCsys, lyrCsys, mapPos);
		}

		Math::Geometry::Polygon *pg;
		NN<Math::Geometry::LinearRing> lr;
		UnsafeArray<Math::Coord2DDbl> ptList;
		UOSInt nPoints;
		NEW_CLASS(pg, Math::Geometry::Polygon(me->navi->GetSRID()));
		NEW_CLASSNN(lr, Math::Geometry::LinearRing(me->navi->GetSRID(), 5, false, false))
		ptList = lr->GetPointList(nPoints);
		if (!lyrCsys->Equals(envCsys))
		{
			ptList[0] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(mapPos.x, me->imgMax.y));
			ptList[1] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(me->imgMax.x, me->imgMax.y));
			ptList[2] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(me->imgMax.x, mapPos.y));
			ptList[3] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, mapPos);
			ptList[4] = ptList[0];
		}
		else
		{
			ptList[0].x = mapPos.x;
			ptList[0].y = me->imgMax.y;
			ptList[1].x = me->imgMax.x;
			ptList[1].y = me->imgMax.y;
			ptList[2].x = me->imgMax.x;
			ptList[2].y = mapPos.y;
			ptList[3] = mapPos;
			ptList[4].x = mapPos.x;
			ptList[4].y = me->imgMax.y;
		}
		pg->AddGeometry(lr);

		me->navi->SetSelectedVector(pg);
	}
	else if (me->downType == 7)
	{
		Math::Coord2DDbl mapPos = me->navi->ScnXY2MapXY(scnPos);
		if (!lyrCsys->Equals(envCsys))
		{
			mapPos = Math::CoordinateSystem::Convert(envCsys, lyrCsys, mapPos);
		}

		Math::Geometry::Polygon *pg;
		NN<Math::Geometry::LinearRing> lr;
		UnsafeArray<Math::Coord2DDbl> ptList;
		UOSInt nPoints;
		NEW_CLASS(pg, Math::Geometry::Polygon(me->navi->GetSRID()));
		NEW_CLASSNN(lr, Math::Geometry::LinearRing(me->navi->GetSRID(), 5, false, false))
		ptList = lr->GetPointList(nPoints);
		if (!lyrCsys->Equals(envCsys))
		{
			ptList[0] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(mapPos.x, me->imgMax.y));
			ptList[1] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(me->imgMax.x, me->imgMax.y));
			ptList[2] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(me->imgMax.x, me->imgMin.y));
			ptList[3] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(mapPos.x, me->imgMin.y));
			ptList[4] = ptList[0];
		}
		else
		{
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
		}
		pg->AddGeometry(lr);

		me->navi->SetSelectedVector(pg);
	}
	else if (me->downType == 8)
	{
		Math::Coord2DDbl mapPos = me->navi->ScnXY2MapXY(scnPos);
		if (!lyrCsys->Equals(envCsys))
		{
			mapPos = Math::CoordinateSystem::Convert(envCsys, lyrCsys, mapPos);
		}

		Math::Geometry::Polygon *pg;
		NN<Math::Geometry::LinearRing> lr;
		UnsafeArray<Math::Coord2DDbl> ptList;
		UOSInt nPoints;
		NEW_CLASS(pg, Math::Geometry::Polygon(me->navi->GetSRID()));
		NEW_CLASSNN(lr, Math::Geometry::LinearRing(me->navi->GetSRID(), 5, false, false))
		ptList = lr->GetPointList(nPoints);
		if (!lyrCsys->Equals(envCsys))
		{
			ptList[0] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, mapPos);
			ptList[1] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(me->imgMax.x, mapPos.y));
			ptList[2] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(me->imgMax.x, me->imgMin.y));
			ptList[3] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(mapPos.x, me->imgMin.y));
			ptList[4] = ptList[0];
		}
		else
		{
			ptList[0] = mapPos;
			ptList[1].x = me->imgMax.x;
			ptList[1].y = mapPos.y;
			ptList[2].x = me->imgMax.x;
			ptList[2].y = me->imgMin.y;
			ptList[3].x = mapPos.x;
			ptList[3].y = me->imgMin.y;
			ptList[4] = mapPos;
		}
		pg->AddGeometry(lr);

		me->navi->SetSelectedVector(pg);
	}
	else if (me->downType == 9)
	{
		Math::Coord2DDbl pt1 = me->navi->ScnXY2MapXY(Math::Coord2D<OSInt>(0, 0));
		Math::Coord2DDbl pt2 = me->navi->ScnXY2MapXY(scnPos - me->downPos);
		if (!lyrCsys->Equals(envCsys))
		{
			pt1 = Math::CoordinateSystem::Convert(envCsys, lyrCsys, pt1);
			pt2 = Math::CoordinateSystem::Convert(envCsys, lyrCsys, pt2);
		}

		Math::Geometry::Polygon *pg;
		NN<Math::Geometry::LinearRing> lr;
		UnsafeArray<Math::Coord2DDbl> ptList;
		UOSInt nPoints;
		NEW_CLASS(pg, Math::Geometry::Polygon(me->navi->GetSRID()));
		NEW_CLASSNN(lr, Math::Geometry::LinearRing(me->navi->GetSRID(), 5, false, false))
		ptList = lr->GetPointList(nPoints);
		if (!lyrCsys->Equals(envCsys))
		{
			ptList[0] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(me->imgMin.x + pt2.x - pt1.x, me->imgMax.y + pt2.y - pt1.y));
			ptList[1] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(me->imgMax.x + pt2.x - pt1.x, me->imgMax.y + pt2.y - pt1.y));
			ptList[2] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(me->imgMax.x + pt2.x - pt1.x, me->imgMin.y + pt2.y - pt1.y));
			ptList[3] = Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(me->imgMin.x + pt2.x - pt1.x, me->imgMin.y + pt2.y - pt1.y));
			ptList[4] = ptList[0];
		}
		else
		{
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
		}
		pg->AddGeometry(lr);

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
	return UI::EventState::ContinueEvent;
}

void __stdcall SSWR::AVIRead::AVIRGISEditImageForm::OnAlphaScrolled(AnyType userObj, UOSInt scrollPos)
{
	NN<SSWR::AVIRead::AVIRGISEditImageForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISEditImageForm>();
	if (me->currImage == -1)
		return;
	Bool changed = false;
	NN<Map::GetObjectSess> sess = me->lyr->BeginGetObject();
	NN<Math::Geometry::VectorImage> img;
	if (Optional<Math::Geometry::VectorImage>::ConvertFrom(me->lyr->GetNewVectorById(sess, me->currImage)).SetTo(img))
	{
		Double alpha = (Double)scrollPos * 0.005;
		UTF8Char sbuff[64];
		UnsafeArray<UTF8Char> sptr = Text::StrDouble(sbuff, alpha);
		me->txtAlpha->SetText(CSTRP(sbuff, sptr));
		img->SetSrcAlpha(alpha);
		me->lyr->ReplaceVector(me->currImage, img);
		changed = true;
	}
	me->lyr->EndGetObject(sess);
	if (changed)
	{
		me->navi->RedrawMap();
	}
}

Int32 SSWR::AVIRead::AVIRGISEditImageForm::CalcDownType(Math::Coord2D<OSInt> scnPos)
{
	if (this->currImage == -1)
		return 0;
	Int32 hPos = 0;
	Int32 vPos = 0;
	Math::Coord2D<OSInt> tl;
	Math::Coord2D<OSInt> br;
	NN<Math::CoordinateSystem> lyrCsys = this->lyr->GetCoordinateSystem();
	NN<Math::CoordinateSystem> envCsys = this->navi->GetCoordinateSystem();
	if (!lyrCsys->Equals(envCsys))
	{
		tl = this->navi->MapXY2ScnXY(Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(this->imgMin.x, this->imgMax.y)));
		br = this->navi->MapXY2ScnXY(Math::CoordinateSystem::Convert(lyrCsys, envCsys, Math::Coord2DDbl(this->imgMax.x, this->imgMin.y)));
	}
	else
	{
		tl = this->navi->MapXY2ScnXY(Math::Coord2DDbl(this->imgMin.x, this->imgMax.y));
		br = this->navi->MapXY2ScnXY(Math::Coord2DDbl(this->imgMax.x, this->imgMin.y));
	}

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

SSWR::AVIRead::AVIRGISEditImageForm::AVIRGISEditImageForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::VectorLayer> lyr, NN<AVIRMapNavigator> navi) : UI::GUIForm(parent, 416, 408, ui)
{
	Text::StringBuilderUTF8 sb;
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->lyr = lyr;
	this->navi = navi;
	sb.AppendC(UTF8STRC("Edit Image - "));
	sb.Append(lyr->GetSourceNameObj());
	this->SetText(sb.ToCString());
	this->SetFont(nullptr, 8.25, false);
	this->currImage = -1;
	this->downType = 0;

	this->pnlCtrl = ui->NewPanel(*this);
	this->pnlCtrl->SetRect(0, 0, 100, 23, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->chkAutoPan = ui->NewCheckBox(this->pnlCtrl, CSTR("Auto Pan"), true);
	this->chkAutoPan->SetRect(0, 0, 100, 23, false);
	this->chkEdit = ui->NewCheckBox(this->pnlCtrl, CSTR("Edit Mode"), true);
	this->chkEdit->SetRect(120, 0, 100, 23, false);
	this->lbImages = ui->NewListBox(*this, false);
	this->lbImages->SetRect(0, 0, 200, 10, false);
	this->lbImages->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbImages->HandleSelectionChange(OnImageChg, this);
	this->hspMain = ui->NewHSplitter(*this, 3, false);
	this->pnlMain = ui->NewPanel(*this);
	this->pnlMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lblLeft = ui->NewLabel(this->pnlMain, CSTR("Left"));
	this->lblLeft->SetRect(0, 0, 100, 23, false);
	this->txtLeft = ui->NewTextBox(this->pnlMain, CSTR(""));
	this->txtLeft->SetRect(100, 0, 100, 23, false);
	this->txtLeft->SetReadOnly(true);
	this->lblTop = ui->NewLabel(this->pnlMain, CSTR("Top"));
	this->lblTop->SetRect(0, 24, 100, 23, false);
	this->txtTop = ui->NewTextBox(this->pnlMain, CSTR(""));
	this->txtTop->SetRect(100, 24, 100, 23, false);
	this->txtTop->SetReadOnly(true);
	this->lblRight = ui->NewLabel(this->pnlMain, CSTR("Right"));
	this->lblRight->SetRect(0, 48, 100, 23, false);
	this->txtRight = ui->NewTextBox(this->pnlMain, CSTR(""));
	this->txtRight->SetRect(100, 48, 100, 23, false);
	this->txtRight->SetReadOnly(true);
	this->lblBottom = ui->NewLabel(this->pnlMain, CSTR("Bottom"));
	this->lblBottom->SetRect(0, 72, 100, 23, false);
	this->txtBottom = ui->NewTextBox(this->pnlMain, CSTR(""));
	this->txtBottom->SetRect(100, 72, 100, 23, false);
	this->txtBottom->SetReadOnly(true);
	this->lblAlpha = ui->NewLabel(this->pnlMain, CSTR("Transparency"));
	this->lblAlpha->SetRect(0, 120, 100, 23, false);
	this->txtAlpha = ui->NewTextBox(this->pnlMain, CSTR(""));
	this->txtAlpha->SetRect(100, 120, 100, 23, false);
	this->txtAlpha->SetReadOnly(true);
	this->trkAlpha = ui->NewTrackBar(this->pnlMain, 0, 200, 200);
	this->trkAlpha->SetRect(0, 144, 200, 23, false);
	this->trkAlpha->HandleScrolled(OnAlphaScrolled, this);

	Data::ArrayListInt64 objIds;
	Optional<Map::NameArray> nameArr;
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UOSInt cnt = lyr->GetColumnCnt();
	UOSInt i;
	UOSInt j;
	lyr->GetAllObjectIds(objIds, nameArr);
	if (cnt > 0)
	{
		i = 0;
		j = objIds.GetCount();
		while (i < j)
		{
			sb.ClearStr();
			lyr->GetString(sb, nameArr, objIds.GetItem(i), 0);
			this->lbImages->AddItem(sb.ToCString(), (void*)(OSInt)objIds.GetItem(i));
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
	this->navi->HandleMapMouseLDown(OnMouseDown, this);
	this->navi->HandleMapMouseLUp(OnMouseUp, this);
	this->navi->HandleMapMouseMove(OnMouseMove, this);
}

SSWR::AVIRead::AVIRGISEditImageForm::~AVIRGISEditImageForm()
{
	this->navi->HideMarker();
	this->navi->SetSelectedVector(nullptr);
	this->navi->UnhandleMapMouse(this);
}

void SSWR::AVIRead::AVIRGISEditImageForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
