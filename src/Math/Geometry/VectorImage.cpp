#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/CoordinateSystem.h"
#include "Math/Math.h"
#include "Math/Geometry/VectorImage.h"
#include "Media/StaticImage.h"

Math::Geometry::VectorImage::VectorImage(UInt32 srid, Media::SharedImage *img, Math::Coord2DDbl tl, Math::Coord2DDbl br, Bool scnCoord, Text::String *srcAddr, Int64 timeStart, Int64 timeEnd) : Math::Geometry::Vector2D(srid)
{
	this->img = img->Clone();
	if (scnCoord)
	{
		this->tl = tl;
		this->br = Math::Coord2DDbl(0, 0);
		this->size = br;
	}
	else
	{
		this->size = Math::Coord2DDbl(0, 0);
		this->tl = tl.Min(br);
		this->br = tl.Max(br);
	}
	this->scnCoord = scnCoord;
	this->hasHeight = false;
	this->height = 0;
	this->srcAddr = SCOPY_STRING(srcAddr);
	this->timeStart = timeStart;
	this->timeEnd = timeEnd;
	this->srcAlpha = -1;
	this->hasZIndex = false;
	this->zIndex = 0;
}

Math::Geometry::VectorImage::VectorImage(UInt32 srid, Media::SharedImage *img, Math::Coord2DDbl tl, Math::Coord2DDbl br, Bool scnCoord, Text::CString srcAddr, Int64 timeStart, Int64 timeEnd) : Math::Geometry::Vector2D(srid)
{
	this->img = img->Clone();
	if (scnCoord)
	{
		this->tl = tl;
		this->br = Math::Coord2DDbl(0, 0);
		this->size = br;
	}
	else
	{
		this->size = Math::Coord2DDbl(0, 0);
		this->tl = tl.Min(br);
		this->br = tl.Max(br);
	}
	this->scnCoord = scnCoord;
	this->hasHeight = false;
	this->height = 0;
	this->srcAddr = Text::String::New(srcAddr).Ptr();
	this->timeStart = timeStart;
	this->timeEnd = timeEnd;
	this->srcAlpha = -1;
	this->hasZIndex = false;
	this->zIndex = 0;
}

Math::Geometry::VectorImage::VectorImage(UInt32 srid, Media::SharedImage *img, Math::Coord2DDbl tl, Math::Coord2DDbl br, Math::Coord2DDbl size, Bool scnCoord, Text::String *srcAddr, Int64 timeStart, Int64 timeEnd) : Math::Geometry::Vector2D(srid)
{
	this->img = img->Clone();
	if (scnCoord)
	{
		this->tl = tl;
		this->br = br;
		this->size = size;
	}
	else
	{
		this->size = Math::Coord2DDbl(0, 0);
		this->tl = tl.Min(br);
		this->br = tl.Max(br);
	}
	this->scnCoord = scnCoord;
	this->hasHeight = false;
	this->height = 0;
	this->srcAddr = SCOPY_STRING(srcAddr);
	this->timeStart = timeStart;
	this->timeEnd = timeEnd;
	this->srcAlpha = -1;
	this->hasZIndex = false;
	this->zIndex = 0;
}

Math::Geometry::VectorImage::VectorImage(UInt32 srid, Media::SharedImage *img, Math::Coord2DDbl tl, Math::Coord2DDbl br, Math::Coord2DDbl size, Bool scnCoord, Text::CString srcAddr, Int64 timeStart, Int64 timeEnd) : Math::Geometry::Vector2D(srid)
{
	this->img = img->Clone();
	if (scnCoord)
	{
		this->tl = tl;
		this->br = br;
		this->size = size;
	}
	else
	{
		this->size = Math::Coord2DDbl(0, 0);
		this->tl = tl.Min(br);
		this->br = tl.Max(br);
	}
	this->scnCoord = scnCoord;
	this->hasHeight = false;
	this->height = 0;
	this->srcAddr = Text::String::New(srcAddr).Ptr();
	this->timeStart = timeStart;
	this->timeEnd = timeEnd;
	this->srcAlpha = -1;
	this->hasZIndex = false;
	this->zIndex = 0;
}

Math::Geometry::VectorImage::~VectorImage()
{
	DEL_CLASS(this->img);
	SDEL_STRING(this->srcAddr);
}

Math::Geometry::Vector2D::VectorType Math::Geometry::VectorImage::GetVectorType() const
{
	return Math::Geometry::Vector2D::VectorType::Image;
}

Math::Coord2DDbl Math::Geometry::VectorImage::GetCenter() const
{
	return (this->tl + this->br) * 0.5;
}

NotNullPtr<Math::Geometry::Vector2D> Math::Geometry::VectorImage::Clone() const
{
	NotNullPtr<Math::Geometry::VectorImage> vimg;
	if (this->scnCoord)
	{
		NEW_CLASSNN(vimg, Math::Geometry::VectorImage(this->srid, this->img, this->tl, this->br, this->size, this->scnCoord, this->srcAddr, this->timeStart, this->timeEnd));
	}
	else
	{
		NEW_CLASSNN(vimg, Math::Geometry::VectorImage(this->srid, this->img, this->tl, this->br, this->scnCoord, this->srcAddr, this->timeStart, this->timeEnd));
	}
	if (this->hasHeight)
	{
		vimg->SetHeight(this->height);
	}
	if (this->hasZIndex)
	{
		vimg->SetZIndex(this->zIndex);
	}
	return vimg;
}

Math::RectAreaDbl Math::Geometry::VectorImage::GetBounds() const
{
	return Math::RectAreaDbl(this->tl, this->br);
}

Double Math::Geometry::VectorImage::CalBoundarySqrDistance(Math::Coord2DDbl pt, OutParam<Math::Coord2DDbl> nearPt) const
{
	Bool hCenter = false;
	Bool vCenter = false;
	Math::Coord2DDbl near;
	if (pt.x > br.x)
	{
		near.x = br.x;
	}
	else if (pt.x < tl.x)
	{
		near.x = tl.x;
	}
	else
	{
		near.x = pt.x;
		hCenter = true;
	}
	if (pt.y > br.y)
	{
		near.y = br.y;
	}
	else if (pt.y < tl.y)
	{
		near.y = tl.y;
	}
	else
	{
		near.y = pt.y;
		vCenter = true;
	}
	if (hCenter && vCenter)
	{
		Double d[4];
		d[0] = pt.x - tl.x;
		d[1] = pt.y - tl.y;
		d[2] = br.x - pt.x;
		d[3] = br.y - pt.y;
		UOSInt minIndex = 0;
		if (d[minIndex] > d[1])
		{
			minIndex = 1;
		}
		if (d[minIndex] > d[2])
		{
			minIndex = 2;
		}
		if (d[minIndex] > d[3])
		{
			minIndex = 3;
		}
		switch (minIndex)
		{
		case 0:
			near = Math::Coord2DDbl(tl.x, pt.y);
			break;
		case 1:
			near = Math::Coord2DDbl(pt.x, tl.y);
			break;
		case 2:
			near = Math::Coord2DDbl(br.x, pt.y);
			break;
		case 3:
		default:
			near = Math::Coord2DDbl(pt.x, br.y);
			break;
		}
	}
	nearPt.Set(near);
	near = pt - near;
	near = near * near;
	return near.x + near.y;
}

Double Math::Geometry::VectorImage::CalSqrDistance(Math::Coord2DDbl pt, OutParam<Math::Coord2DDbl> nearPt) const
{
	Math::Coord2DDbl near;
	if (pt.x > br.x)
	{
		near.x = br.x;
	}
	else if (pt.x < tl.x)
	{
		near.x = tl.x;
	}
	else
	{
		near.x = pt.x;
	}
	if (pt.y > br.y)
	{
		near.y = br.y;
	}
	else if (pt.y < tl.y)
	{
		near.y = tl.y;
	}
	else
	{
		near.y = pt.y;
	}
	nearPt.Set(near);
	near = pt - near;
	near = near * near;
	return near.x + near.y;
}

Double Math::Geometry::VectorImage::CalArea() const
{
	return 0;
}

Bool Math::Geometry::VectorImage::JoinVector(NotNullPtr<const Math::Geometry::Vector2D> vec)
{
	return false;
}

Bool Math::Geometry::VectorImage::HasZ() const
{
	return this->hasHeight;
}

Bool Math::Geometry::VectorImage::GetZBounds(OutParam<Double> min, OutParam<Double> max) const
{
	if (hasHeight)
	{
		min.Set(this->height);
		max.Set(this->height);
		return true;
	}
	return false;
}

Bool Math::Geometry::VectorImage::GetMBounds(OutParam<Double> min, OutParam<Double> max) const
{
	return false;
}

void Math::Geometry::VectorImage::Convert(NotNullPtr<Math::CoordinateConverter> converter)
{
	if (this->scnCoord)
	{
/*		if (converter->ConvertScreen())
		{
			this->srid = destCSys->GetSRID();
		}*/
	}
	else
	{
		tl = converter->Convert2D(tl);
		br = converter->Convert2D(br);
		this->srid = converter->GetOutputSRID();
	}
}

Bool Math::Geometry::VectorImage::Equals(NotNullPtr<const Vector2D> vec, Bool sameTypeOnly, Bool nearlyVal) const
{
	if (vec->GetVectorType() != VectorType::Image)
	{
		return false;
	}
//	Math::VectorImage *img = (Math::VectorImage*)vec;
	return false;
}

UOSInt Math::Geometry::VectorImage::GetCoordinates(NotNullPtr<Data::ArrayListA<Math::Coord2DDbl>> coordList) const
{
	if (this->scnCoord)
	{
		coordList->Add(this->tl);
		coordList->Add(Math::Coord2DDbl(this->tl.x + this->size.x, this->tl.y));
		coordList->Add(this->tl + this->size);
		coordList->Add(Math::Coord2DDbl(this->tl.x, this->tl.y + this->size.y));
	}
	else
	{
		coordList->Add(this->tl);
		coordList->Add(Math::Coord2DDbl(this->br.x, this->tl.y));
		coordList->Add(this->br);
		coordList->Add(Math::Coord2DDbl(this->tl.x, this->br.y));
	}
	return 4;
}

Bool Math::Geometry::VectorImage::InsideOrTouch(Math::Coord2DDbl coord) const
{
	return this->tl.x <= coord.x && this->tl.y <= coord.y && this->br.x >= coord.x && this->br.y >= coord.y;
}

void Math::Geometry::VectorImage::SwapXY()
{
	this->tl = this->tl.SwapXY();
	this->br = this->br.SwapXY();
	this->size = this->size.SwapXY();
}

void Math::Geometry::VectorImage::MultiplyCoordinatesXY(Double v)
{
	this->tl = this->tl * v;
	this->br = this->br * v;
	this->size = this->size * v;
}

UOSInt Math::Geometry::VectorImage::GetPointCount() const
{
	return 4;
}

Text::String *Math::Geometry::VectorImage::GetSourceAddr() const
{
	return this->srcAddr;
}

void Math::Geometry::VectorImage::SetHeight(Double height)
{
	this->hasHeight = true;
	this->height = height;
}

Double Math::Geometry::VectorImage::GetHeight() const
{
	return this->height;
}

Int64 Math::Geometry::VectorImage::GetTimeStart() const
{
	return this->timeStart;
}

Int64 Math::Geometry::VectorImage::GetTimeEnd() const
{
	return this->timeEnd;
}

void Math::Geometry::VectorImage::SetSrcAlpha(Double srcAlpha)
{
	this->srcAlpha = srcAlpha;
}

Bool Math::Geometry::VectorImage::HasSrcAlpha() const
{
	return this->srcAlpha >= 0 && this->srcAlpha <=	1;
}

Double Math::Geometry::VectorImage::GetSrcAlpha() const
{
	return this->srcAlpha;
}

void Math::Geometry::VectorImage::SetZIndex(Int32 zIndex)
{
	this->zIndex = zIndex;
	this->hasZIndex = true;
}

Bool Math::Geometry::VectorImage::HasZIndex() const
{
	return this->hasZIndex;
}

Int32 Math::Geometry::VectorImage::GetZIndex() const
{
	return this->zIndex;
}

void Math::Geometry::VectorImage::GetScreenBounds(UOSInt scnWidth, UOSInt scnHeight, Double hdpi, Double vdpi, Double *x1, Double *y1, Double *x2, Double *y2) const
{
	Media::StaticImage *simg = this->img->GetImage(0);
	Double scnX;
	Double scnY;
	Double sizeX;
	Double sizeY;

	if (this->size.x == 0 && this->size.y == 0)
	{
		sizeX = UOSInt2Double(simg->info.dispSize.x) * hdpi / simg->info.hdpi;
		sizeY = UOSInt2Double(simg->info.dispSize.y) * vdpi / simg->info.vdpi;
	}
	else if (this->size.x == 0)
	{
		sizeY = UOSInt2Double(scnHeight) * this->size.y;
		sizeX = UOSInt2Double(scnHeight) * this->size.y * UOSInt2Double(simg->info.dispSize.x) / UOSInt2Double(simg->info.dispSize.y);
	}
	else if (this->size.y == 0)
	{
		sizeX = UOSInt2Double(scnWidth) * this->size.x;
		sizeY = UOSInt2Double(scnWidth) * this->size.x * UOSInt2Double(simg->info.dispSize.y) / UOSInt2Double(simg->info.dispSize.x);
	}
	else
	{
		sizeX = UOSInt2Double(scnWidth) * this->size.x;
		sizeY = UOSInt2Double(scnHeight) * this->size.y;
	}

	scnX = UOSInt2Double(scnWidth) * this->tl.x - sizeX * this->br.x;
	scnY = UOSInt2Double(scnHeight) * (1 - this->tl.y) - sizeY * (1 - this->br.y);
	*x1 = scnX;
	*y1 = scnY;
	*x2 = scnX + sizeX;
	*y2 = scnY + sizeY;
}

void Math::Geometry::VectorImage::GetVectorSize(Double *sizeX, Double *sizeY) const
{
	*sizeX = this->size.x;
	*sizeY = this->size.y;
}

Bool Math::Geometry::VectorImage::IsScnCoord() const
{
	return this->scnCoord;
}

void Math::Geometry::VectorImage::SetBounds(Double minX, Double minY, Double maxX, Double maxY)
{
	this->tl.x = minX;
	this->tl.y = minY;
	this->br.x = maxX;
	this->br.y = maxY;
}

Media::StaticImage *Math::Geometry::VectorImage::GetImage(OptOut<UInt32> imgTimeMS) const
{
	return this->img->GetImage(imgTimeMS);
}

Media::StaticImage *Math::Geometry::VectorImage::GetImage(Double width, Double height, OptOut<UInt32> imgTimeMS) const
{
	if (width < 0)
		width = -width;
	if (height < 0)
		height = -height;
	Media::StaticImage *retImg = this->img->GetPrevImage(width, height, imgTimeMS);
	return retImg;
}

Math::Geometry::VectorImage *Math::Geometry::VectorImage::CreateScreenImage(UInt32 srid, Media::SharedImage *img, Math::Coord2DDbl tl, Math::Coord2DDbl size, Text::CString srcAddr)
{
	return NEW_CLASS_D(Math::Geometry::VectorImage(srid, img, Math::Coord2DDbl(tl.x, 1 - tl.y - size.y), Math::Coord2DDbl(0, 0), size, true, srcAddr, 0, 0));
}
