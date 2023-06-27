#include "Stdafx.h"
#include "Data/RandomOS.h"
#include "Math/Math.h"
#include "Media/DrawEngine.h"
#include "UI/DObj/DynamicOverlayDObj.h"

UI::DObj::DynamicOverlayDObj::DynamicOverlayDObj(Media::DrawEngine *deng, Text::CString fileName1, Text::CString fileName2, Math::Coord2D<OSInt> tl) : DirectObject(tl)
{
	this->deng = deng;
	if (fileName1.leng == 0)
	{
		this->bmp1 = 0;
	}
	else
	{
		this->bmp1 = this->deng->LoadImage(fileName1);
	}

	if (fileName2.leng == 0)
	{
		this->bmp2 = 0;
	}
	else
	{
		this->bmp2 = this->deng->LoadImage(fileName2);
	}
	NEW_CLASS(this->rnd, Data::RandomOS());
	this->alpha = this->rnd->NextDouble();
	this->a = 0;
}

UI::DObj::DynamicOverlayDObj::~DynamicOverlayDObj()
{
	if (this->bmp1)
	{
		this->deng->DeleteImage(this->bmp1);
		this->bmp1 = 0;
	}
	if (this->bmp2)
	{
		this->deng->DeleteImage(this->bmp2);
		this->bmp2 = 0;
	}
}

Bool UI::DObj::DynamicOverlayDObj::IsChanged()
{
	if (this->bmp1 != 0 && this->bmp2 != 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

Bool UI::DObj::DynamicOverlayDObj::DoEvents()
{
	return false;
}

void UI::DObj::DynamicOverlayDObj::DrawObject(Media::DrawImage *dimg)
{
	Math::Coord2DDbl tl = this->GetCurrPos().ToDouble();
	if (this->bmp1 && this->bmp2)
	{
		if (this->bmp1->GetWidth() == this->bmp2->GetWidth() && this->bmp1->GetHeight() == this->bmp2->GetHeight())
		{
			this->a += (this->rnd->NextDouble() - 0.5) * 0.016;
			this->alpha += this->a;
			if (this->alpha > 1)
			{
				this->alpha = 1;
				this->a = 0;
			}
			else if (this->alpha < 0)
			{
				this->alpha = 0;
				this->a = 0;
			}
			Media::DrawImage *bmpS1 = this->bmp1;
			Media::DrawImage *bmpS2 = this->bmp2;
			Media::DrawImage *bmpTmp = this->deng->CreateImage32(bmpS1->GetWidth(), bmpS1->GetHeight(), Media::AT_NO_ALPHA);
			bmpTmp->SetAlphaType(bmpS1->GetAlphaType());
			Bool revOrder;
			UInt8 *ptrS1 = bmpS1->GetImgBits(&revOrder);
			UInt8 *ptrS2 = bmpS2->GetImgBits(&revOrder);
			UInt8 *ptrD = bmpTmp->GetImgBits(&revOrder);
			UOSInt lineBytes = bmpS1->GetWidth() * 4;
			UOSInt i;
			UOSInt j = bmpS1->GetHeight();
			Double a1 = this->alpha;
			Double a2 = 1 - this->alpha; 
			while (j-- > 0)
			{
				i = lineBytes;
				while (i-- > 0)
				{
					*ptrD++ = (UInt8)Double2Int32(a1 * (*ptrS1++) + a2 * (*ptrS2++));
				}
			}

			dimg->DrawImagePt(bmpTmp, tl.x, tl.y);
			this->deng->DeleteImage(bmpTmp);
		}
		else
		{
			dimg->DrawImagePt(this->bmp1, tl.x, tl.y);
		}
	}
	else if (this->bmp1)
	{
		dimg->DrawImagePt(this->bmp1, tl.x, tl.y);
	}
	else if (this->bmp2)
	{
		dimg->DrawImagePt(this->bmp2, tl.x, tl.y);
	}
}

Bool UI::DObj::DynamicOverlayDObj::IsObject(Int32 x, Int32 y)
{
	return false;
}

/*System::Windows::Forms::Cursor ^UI::DObj::DynamicOverlayDObj::GetCursor()
{
	return System::Windows::Forms::Cursors::Arrow;
}*/

void UI::DObj::DynamicOverlayDObj::OnMouseDown()
{
}

void UI::DObj::DynamicOverlayDObj::OnMouseUp()
{
}

void UI::DObj::DynamicOverlayDObj::OnMouseClick()
{
}
