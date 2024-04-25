#include "Stdafx.h"
#include "Data/RandomOS.h"
#include "Math/Math.h"
#include "Media/DrawEngine.h"
#include "UI/DObj/DynamicOverlayDObj.h"

UI::DObj::DynamicOverlayDObj::DynamicOverlayDObj(NN<Media::DrawEngine> deng, Text::CString fileName1, Text::CString fileName2, Math::Coord2D<OSInt> tl) : DirectObject(tl)
{
	this->deng = deng;
	if (fileName1.leng == 0)
	{
		this->bmp1 = 0;
	}
	else
	{
		this->bmp1 = this->deng->LoadImage(fileName1.OrEmpty());
	}

	if (fileName2.leng == 0)
	{
		this->bmp2 = 0;
	}
	else
	{
		this->bmp2 = this->deng->LoadImage(fileName2.OrEmpty());
	}
	NEW_CLASS(this->rnd, Data::RandomOS());
	this->alpha = this->rnd->NextDouble();
	this->a = 0;
}

UI::DObj::DynamicOverlayDObj::~DynamicOverlayDObj()
{
	NN<Media::DrawImage> img;
	if (img.Set(this->bmp1))
	{
		this->deng->DeleteImage(img);
		this->bmp1 = 0;
	}
	if (img.Set(this->bmp2))
	{
		this->deng->DeleteImage(img);
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
	NN<Media::DrawImage> bmp1;
	NN<Media::DrawImage> bmp2;
	if (bmp1.Set(this->bmp1) && bmp2.Set(this->bmp2))
	{
		if (bmp1->GetWidth() == bmp2->GetWidth() && bmp1->GetHeight() == bmp2->GetHeight())
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
			NN<Media::DrawImage> bmpTmp;
			if (bmpTmp.Set(this->deng->CreateImage32(bmp1->GetSize(), Media::AT_NO_ALPHA)))
			{
				bmpTmp->SetAlphaType(bmp1->GetAlphaType());
				Bool revOrder;
				UInt8 *ptrS1 = bmp1->GetImgBits(revOrder);
				UInt8 *ptrS2 = bmp2->GetImgBits(revOrder);
				UInt8 *ptrD = bmpTmp->GetImgBits(revOrder);
				UOSInt lineBytes = bmp1->GetWidth() * 4;
				UOSInt i;
				UOSInt j = bmp1->GetHeight();
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

				dimg->DrawImagePt(bmpTmp, tl);
				this->deng->DeleteImage(bmpTmp);
			}
		}
		else
		{
			dimg->DrawImagePt(bmp1, tl);
		}
	}
	else if (bmp1.Set(this->bmp1))
	{
		dimg->DrawImagePt(bmp1, tl);
	}
	else if (bmp2.Set(this->bmp2))
	{
		dimg->DrawImagePt(bmp2, tl);
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
