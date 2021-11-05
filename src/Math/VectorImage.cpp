#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/CoordinateSystem.h"
#include "Math/Math.h"
#include "Math/VectorImage.h"
#include "Media/StaticImage.h"

Math::VectorImage::VectorImage(UInt32 srid, Media::SharedImage *img, Double x1, Double y1, Double x2, Double y2, Bool scnCoord, const UTF8Char *srcAddr, Int64 timeStart, Int64 timeEnd) : Math::Vector2D(srid)
{
	this->img = img->Clone();
	if (scnCoord)
	{
		this->x1 = x1;
		this->y1 = y1;
		this->x2 = 0;
		this->y2 = 0;
		this->sizeX = x2;
		this->sizeY = y2;
	}
	else
	{
		this->sizeX = 0;
		this->sizeY = 0;
		if (x1 > x2)
		{
			this->x2 = x1;
			this->x1 = x2;
		}
		else
		{
			this->x1 = x1;
			this->x2 = x2;
		}
		if (y1 > y2)
		{
			this->y2 = y1;
			this->y1 = y2;
		}
		else
		{
			this->y1 = y1;
			this->y2 = y2;
		}
	}
	this->scnCoord = scnCoord;
	this->hasHeight = false;
	this->height = 0;
	if (srcAddr)
	{
		this->srcAddr = Text::StrCopyNew(srcAddr);
	}
	else
	{
		this->srcAddr = 0;
	}
	this->timeStart = timeStart;
	this->timeEnd = timeEnd;
	this->srcAlpha = -1;
	this->hasZIndex = false;
	this->zIndex = 0;
}

Math::VectorImage::VectorImage(UInt32 srid, Media::SharedImage *img, Double x1, Double y1, Double x2, Double y2, Double sizeX, Double sizeY, Bool scnCoord, const UTF8Char *srcAddr, Int64 timeStart, Int64 timeEnd) : Math::Vector2D(srid)
{
	this->img = img->Clone();
	if (scnCoord)
	{
		this->x1 = x1;
		this->y1 = y1;
		this->x2 = x2;
		this->y2 = y2;
		this->sizeX = sizeX;
		this->sizeY = sizeY;
	}
	else
	{
		this->sizeX = 0;
		this->sizeY = 0;
		if (x1 > x2)
		{
			this->x2 = x1;
			this->x1 = x2;
		}
		else
		{
			this->x1 = x1;
			this->x2 = x2;
		}
		if (y1 > y2)
		{
			this->y2 = y1;
			this->y1 = y2;
		}
		else
		{
			this->y1 = y1;
			this->y2 = y2;
		}
	}
	this->scnCoord = scnCoord;
	this->hasHeight = false;
	this->height = 0;
	this->srcAddr = Text::StrCopyNew(srcAddr);
	this->timeStart = timeStart;
	this->timeEnd = timeEnd;
	this->srcAlpha = -1;
	this->hasZIndex = false;
	this->zIndex = 0;
}

Math::VectorImage::~VectorImage()
{
	DEL_CLASS(this->img);
	SDEL_TEXT(this->srcAddr);
}

Math::Vector2D::VectorType Math::VectorImage::GetVectorType()
{
	return Math::Vector2D::VectorType::Image;
}

void Math::VectorImage::GetCenter(Double *x, Double *y)
{
	*x = (x1 + x2) * 0.5;
	*y = (y1 + y2) * 0.5;
}

Math::Vector2D *Math::VectorImage::Clone()
{
	Math::VectorImage *vimg;
	if (this->scnCoord)
	{
		NEW_CLASS(vimg, Math::VectorImage(this->srid, this->img, this->x1, this->y1, this->x2, this->y2, this->sizeX, this->sizeY, this->scnCoord, this->srcAddr, this->timeStart, this->timeEnd));
	}
	else
	{
		NEW_CLASS(vimg, Math::VectorImage(this->srid, this->img, this->x1, this->y1, this->x2, this->y2, this->scnCoord, this->srcAddr, this->timeStart, this->timeEnd));
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

void Math::VectorImage::GetBounds(Double *minX, Double *minY, Double *maxX, Double *maxY)
{
	*minX = this->x1;
	*minY = this->y1;
	*maxX = this->x2;
	*maxY = this->y2;
}

Double Math::VectorImage::CalSqrDistance(Double x, Double y, Double *nearPtX, Double *nearPtY)
{
	Double nearX;
	Double nearY;
	if (x > x2)
	{
		nearX = x2;
	}
	else if (x < x1)
	{
		nearX = x1;
	}
	else
	{
		nearX = x;
	}
	if (y > y2)
	{
		nearY = y2;
	}
	else if (y < y1)
	{
		nearY = y1;
	}
	else
	{
		nearY = y;
	}
	if (nearPtX)
	{
		*nearPtX = nearX;
	}
	if (nearPtY)
	{
		*nearPtY = nearY;
	}
	if (nearX == x && nearY == y)
		return 0;
	return (x - nearX) * (x - nearX) + (y - nearY) * (y - nearY);
}

Bool Math::VectorImage::JoinVector(Math::Vector2D *vec)
{
	return false;
}

Bool Math::VectorImage::Support3D()
{
	return this->hasHeight;
}

void Math::VectorImage::ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys)
{
	if (this->scnCoord)
	{
	}
	else
	{
		Math::CoordinateSystem::ConvertXYZ(srcCSys, destCSys, x1, y1, 0, &x1, &y1, 0);
		Math::CoordinateSystem::ConvertXYZ(srcCSys, destCSys, x2, y2, 0, &x2, &y2, 0);
	}
}

const UTF8Char *Math::VectorImage::GetSourceAddr()
{
	return this->srcAddr;
}

void Math::VectorImage::SetHeight(Double height)
{
	this->hasHeight = true;
	this->height = height;
}

Double Math::VectorImage::GetHeight()
{
	return this->height;
}

Int64 Math::VectorImage::GetTimeStart()
{
	return this->timeStart;
}

Int64 Math::VectorImage::GetTimeEnd()
{
	return this->timeEnd;
}

void Math::VectorImage::SetSrcAlpha(Double srcAlpha)
{
	this->srcAlpha = srcAlpha;
}

Bool Math::VectorImage::HasSrcAlpha()
{
	return this->srcAlpha >= 0 && this->srcAlpha <=	1;
}

Double Math::VectorImage::GetSrcAlpha()
{
	return this->srcAlpha;
}

void Math::VectorImage::SetZIndex(Int32 zIndex)
{
	this->zIndex = zIndex;
	this->hasZIndex = true;
}

Bool Math::VectorImage::HasZIndex()
{
	return this->hasZIndex;
}

Int32 Math::VectorImage::GetZIndex()
{
	return this->zIndex;
}

void Math::VectorImage::GetScreenBounds(UOSInt scnWidth, UOSInt scnHeight, Double hdpi, Double vdpi, Double *x1, Double *y1, Double *x2, Double *y2)
{
	Media::StaticImage *simg = this->img->GetImage(0);
	Double scnX;
	Double scnY;
	Double sizeX;
	Double sizeY;

	if (this->sizeX == 0 && this->sizeY == 0)
	{
		sizeX = Math::UOSInt2Double(simg->info->dispWidth) * hdpi / simg->info->hdpi;
		sizeY = Math::UOSInt2Double(simg->info->dispHeight) * vdpi / simg->info->vdpi;
	}
	else if (this->sizeX == 0)
	{
		sizeY = Math::UOSInt2Double(scnHeight) * this->sizeY;
		sizeX = Math::UOSInt2Double(scnHeight) * this->sizeY * Math::UOSInt2Double(simg->info->dispWidth) / Math::UOSInt2Double(simg->info->dispHeight);
	}
	else if (this->sizeY == 0)
	{
		sizeX = Math::UOSInt2Double(scnWidth) * this->sizeX;
		sizeY = Math::UOSInt2Double(scnWidth) * this->sizeX * Math::UOSInt2Double(simg->info->dispHeight) / Math::UOSInt2Double(simg->info->dispWidth);
	}
	else
	{
		sizeX = Math::UOSInt2Double(scnWidth) * this->sizeX;
		sizeY = Math::UOSInt2Double(scnHeight) * this->sizeY;
	}

	scnX = Math::UOSInt2Double(scnWidth) * this->x1 - sizeX * this->x2;
	scnY = Math::UOSInt2Double(scnHeight) * (1 - this->y1) - sizeY * (1 - this->y2);
	*x1 = scnX;
	*y1 = scnY;
	*x2 = scnX + sizeX;
	*y2 = scnY + sizeY;
}

void Math::VectorImage::GetVectorSize(Double *sizeX, Double *sizeY)
{
	*sizeX = this->sizeX;
	*sizeY = this->sizeY;
}

Bool Math::VectorImage::IsScnCoord()
{
	return this->scnCoord;
}

void Math::VectorImage::SetBounds(Double minX, Double minY, Double maxX, Double maxY)
{
	this->x1 = minX;
	this->y1 = minY;
	this->x2 = maxX;
	this->y2 = maxY;
}

Media::StaticImage *Math::VectorImage::GetImage(UInt32 *imgTimeMS)
{
	return this->img->GetImage(imgTimeMS);
}

Media::StaticImage *Math::VectorImage::GetImage(Double width, Double height, UInt32 *imgTimeMS)
{
	if (width < 0)
		width = -width;
	if (height < 0)
		height = -height;
	Media::StaticImage *retImg = this->img->GetPrevImage(width, height, imgTimeMS);
	return retImg;
}
