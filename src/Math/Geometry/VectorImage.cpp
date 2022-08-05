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
	this->srcAddr = Text::String::New(srcAddr.v, srcAddr.leng);
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
	this->srcAddr = Text::String::New(srcAddr.v, srcAddr.leng);
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

Math::Geometry::Vector2D *Math::Geometry::VectorImage::Clone() const
{
	Math::Geometry::VectorImage *vimg;
	if (this->scnCoord)
	{
		NEW_CLASS(vimg, Math::Geometry::VectorImage(this->srid, this->img, this->tl, this->br, this->size, this->scnCoord, this->srcAddr, this->timeStart, this->timeEnd));
	}
	else
	{
		NEW_CLASS(vimg, Math::Geometry::VectorImage(this->srid, this->img, this->tl, this->br, this->scnCoord, this->srcAddr, this->timeStart, this->timeEnd));
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

void Math::Geometry::VectorImage::GetBounds(Math::RectAreaDbl *bounds) const
{
	*bounds = Math::RectAreaDbl(this->tl, this->br);
}

Double Math::Geometry::VectorImage::CalSqrDistance(Math::Coord2DDbl pt, Math::Coord2DDbl *nearPt) const
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
	if (nearPt)
	{
		*nearPt = near;
	}
	near = pt - near;
	near = near * near;
	return near.x + near.y;
}

Bool Math::Geometry::VectorImage::JoinVector(Math::Geometry::Vector2D *vec)
{
	return false;
}

Bool Math::Geometry::VectorImage::HasZ() const
{
	return this->hasHeight;
}

void Math::Geometry::VectorImage::ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys)
{
	if (this->scnCoord)
	{
	}
	else
	{
		Math::CoordinateSystem::ConvertXYZ(srcCSys, destCSys, tl.x, tl.y, 0, &tl.x, &tl.y, 0);
		Math::CoordinateSystem::ConvertXYZ(srcCSys, destCSys, br.x, br.y, 0, &br.x, &br.y, 0);
	}
}

Bool Math::Geometry::VectorImage::Equals(Vector2D *vec) const
{
	if (vec == 0 || vec->GetVectorType() != VectorType::Image)
	{
		return false;
	}
//	Math::VectorImage *img = (Math::VectorImage*)vec;
	return false;
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
		sizeX = UOSInt2Double(simg->info.dispWidth) * hdpi / simg->info.hdpi;
		sizeY = UOSInt2Double(simg->info.dispHeight) * vdpi / simg->info.vdpi;
	}
	else if (this->size.x == 0)
	{
		sizeY = UOSInt2Double(scnHeight) * this->size.y;
		sizeX = UOSInt2Double(scnHeight) * this->size.y * UOSInt2Double(simg->info.dispWidth) / UOSInt2Double(simg->info.dispHeight);
	}
	else if (this->size.y == 0)
	{
		sizeX = UOSInt2Double(scnWidth) * this->size.x;
		sizeY = UOSInt2Double(scnWidth) * this->size.x * UOSInt2Double(simg->info.dispHeight) / UOSInt2Double(simg->info.dispWidth);
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

Media::StaticImage *Math::Geometry::VectorImage::GetImage(UInt32 *imgTimeMS) const
{
	return this->img->GetImage(imgTimeMS);
}

Media::StaticImage *Math::Geometry::VectorImage::GetImage(Double width, Double height, UInt32 *imgTimeMS) const
{
	if (width < 0)
		width = -width;
	if (height < 0)
		height = -height;
	Media::StaticImage *retImg = this->img->GetPrevImage(width, height, imgTimeMS);
	return retImg;
}
