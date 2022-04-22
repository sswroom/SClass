#include "Stdafx.h"
#include "MyMemory.h"
#include "Map/ProjectedMapView.h"
#include "Math/Math.h"
#include "Math/Unit/Distance.h"

Map::ProjectedMapView::ProjectedMapView(Double scnWidth, Double scnHeight, Double centX, Double centY, Double scale) : Map::MapView(scnWidth, scnHeight)
{
	this->hdpi = 96.0;
	this->ddpi = 96.0;
	ChangeViewXY(scnWidth, scnHeight, centX, centY, scale);
}

Map::ProjectedMapView::~ProjectedMapView()
{
}

void Map::ProjectedMapView::ChangeViewXY(Double scnWidth, Double scnHeight, Double centX, Double centY, Double scale)
{
	this->centX = centX;
	this->centY = centY;
	if (scale < 400)
		scale = 400;
	if (scale > 100000000)
		scale = 100000000;
	this->scale = scale;

	Double diffx = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, scnWidth * 0.5 / this->hdpi * this->scale);
	Double diffy = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, scnHeight * 0.5 / this->hdpi * this->scale);

	this->rightX = centX + diffx;
	this->leftX = centX - diffx;
	this->bottomY = centY + diffy;
	this->topY = centY - diffy;

	this->scnWidth = scnWidth;
	this->scnHeight = scnHeight;
}

void Map::ProjectedMapView::SetCenterXY(Double mapX, Double mapY)
{
	ChangeViewXY(scnWidth, scnHeight, mapX, mapY, scale);
}

void Map::ProjectedMapView::SetMapScale(Double scale)
{
	ChangeViewXY(scnWidth, scnHeight, this->centX, this->centY, scale);
}

void Map::ProjectedMapView::UpdateSize(Double width, Double height)
{
	ChangeViewXY(width, height, this->centX, this->centY, this->scale);
}

void Map::ProjectedMapView::SetDPI(Double hdpi, Double ddpi)
{
	if (hdpi > 0 && ddpi > 0 && (this->hdpi != hdpi || this->ddpi != ddpi))
	{
		this->hdpi = hdpi;
		this->ddpi = ddpi;
		ChangeViewXY(this->scnWidth, this->scnHeight, this->centX, this->centY, this->scale);
	}
}

Double Map::ProjectedMapView::GetLeftX()
{
	return this->leftX;
}

Double Map::ProjectedMapView::GetTopY()
{
	return this->topY;
}

Double Map::ProjectedMapView::GetRightX()
{
	return this->rightX;
}

Double Map::ProjectedMapView::GetBottomY()
{
	return this->bottomY;
}

Double Map::ProjectedMapView::GetMapScale()
{
	return this->scale;
}

Double Map::ProjectedMapView::GetViewScale()
{
	return this->scale;
}

Double Map::ProjectedMapView::GetCenterX()
{
	return this->centX;
}

Double Map::ProjectedMapView::GetCenterY()
{
	return this->centY;
}

Double Map::ProjectedMapView::GetHDPI()
{
	return this->hdpi;
}

Double Map::ProjectedMapView::GetDDPI()
{
	return this->ddpi;
}

Bool Map::ProjectedMapView::InViewXY(Double mapX, Double mapY)
{
	return mapY >= topY && mapY < bottomY && mapX >= leftX && mapX < rightX;
}

Bool Map::ProjectedMapView::MapXYToScnXY(const Double *srcArr, Int32 *destArr, UOSInt nPoints, Int32 ofstX, Int32 ofstY)
{
	if (nPoints == 0)
	{
		return false;
	}

	Double xmul = this->scnWidth / (rightX - leftX);
	Double ymul = this->scnHeight / (bottomY - topY);
	Double dleft = leftX;
	Double dbottom = bottomY;
#ifdef HAS_ASM32
	Int32 iminX;
	Int32 imaxX;
	Int32 iminY;
	Int32 imaxY;

	_asm
	{
		mov esi,srcArr
		mov edi,destArr
		mov ecx,nPoints
		mov ebx,0x7fffffff
		mov edx,0x80000000
mtslop:
		fld qword ptr [esi]
		fsub dleft
		fmul xmul
		fiadd ofstX
		fistp dword ptr [edi]

		fld dbottom
		fsub qword ptr [esi+8]
		fmul ymul
		fiadd ofstY
		fistp dword ptr [edi+4]

		mov eax,dword ptr [edi]
		cmp ebx,eax
		cmovg ebx,eax
		cmp edx,eax
		cmovl edx,eax

		add esi,16
		add edi,8
		dec ecx
		jnz mtslop

		mov iminX,ebx
		mov imaxX,edx

		mov esi,srcArr
		mov ecx,nPoints
		mov ebx,0x7fffffff
		mov edx,0x80000000
mtslop4:
		add esi,4
		lodsd
		cmp ebx,eax
		cmovg ebx,eax
		cmp edx,eax
		cmovl edx,eax

		dec ecx
		jnz mtslop4

		mov iminY,ebx
		mov imaxY,edx
	}
	return (imaxX >= 0) && (iminX < (OSInt)scnWidth) && (imaxY >= 0) && (iminY < (OSInt)scnHeight);
#else
	Int32 iminX = 0;
	Int32 iminY = 0;
	Int32 imaxX = 0;
	Int32 imaxY = 0;
	Int32 thisX;
	Int32 thisY;
	while (nPoints-- > 0)
	{
		*destArr++ = thisX = Double2Int32((*srcArr++  - dleft) * xmul + ofstX);
		*destArr++ = thisY = Double2Int32((dbottom - *srcArr++) * ymul + ofstY);
		if (iminX == 0 && imaxX == 0)
		{
			iminX = imaxX = thisX;
			iminY = imaxY = thisY;
		}
		else
		{
			if (thisX < iminX)
				iminX = thisX;
			if (thisX > imaxX)
				imaxX = thisX;
			if (thisY < iminY)
				iminY = thisY;
			if (thisY > imaxY)
				imaxY = thisY;
		}
	}
	return (imaxX >= 0) && (iminX < (OSInt)scnWidth) && (imaxY >= 0) && (iminY < (OSInt)scnHeight);
#endif
}

Bool Map::ProjectedMapView::MapXYToScnXY(const Math::Coord2D<Double> *srcArr, Math::Coord2D<Double> *destArr, UOSInt nPoints, Math::Coord2D<Double> ofst)
{
	if (nPoints == 0)
	{
		return false;
	}

	Double xmul = this->scnWidth / (rightX - leftX);
	Double ymul = this->scnHeight / (bottomY - topY);
	Double dleft = leftX;
	Double dbottom = bottomY;
	Double iminX = 0;
	Double iminY = 0;
	Double imaxX = 0;
	Double imaxY = 0;
	Double thisX;
	Double thisY;
	while (nPoints-- > 0)
	{
		destArr->x = thisX = ((srcArr->x  - dleft) * xmul + ofst.x);
		destArr->y = thisY = ((dbottom - srcArr->y) * ymul + ofst.y);
		destArr++;
		srcArr++;
		if (iminX == 0 && imaxX == 0)
		{
			iminX = imaxX = thisX;
			iminY = imaxY = thisY;
		}
		else
		{
			if (thisX < iminX)
				iminX = thisX;
			if (thisX > imaxX)
				imaxX = thisX;
			if (thisY < iminY)
				iminY = thisY;
			if (thisY > imaxY)
				imaxY = thisY;
		}
	}
	return (imaxX >= 0) && (iminX < scnWidth) && (imaxY >= 0) && (iminY < scnHeight);
}


Bool Map::ProjectedMapView::IMapXYToScnXY(Double mapRate, const Int32 *srcArr, Int32 *destArr, UOSInt nPoints, Int32 ofstX, Int32 ofstY)
{
	if (nPoints == 0)
	{
		return false;
	}
	Double xmul = this->scnWidth / (rightX - leftX);
	Double ymul = this->scnHeight / (bottomY - topY);
	Double dleft = leftX;
	Double dbottom = bottomY;
	Double rRate = 1 / mapRate;
#ifdef HAS_ASM32
	Int32 iminX;
	Int32 imaxX;
	Int32 iminY;
	Int32 imaxY;
	_asm
	{
		mov esi,srcArr
		mov edi,destArr
		mov ecx,nPoints
		mov ebx,0x7fffffff
		mov edx,0x80000000
mtslop:

		fild dword ptr [esi]
		fmul rRate
		fsub dleft
		fmul xmul
		fiadd ofstX
		fistp dword ptr [edi]

		fild dword ptr [esi+4]
		fmul rRate
		fsubr dbottom
		fmul ymul
		fiadd ofstY
		fistp dword ptr [edi+4]

		mov eax,dword ptr [edi]
		cmp ebx,eax
		cmovg ebx,eax
		cmp edx,eax
		cmovl edx,eax

		add esi,8
		add edi,8
		dec ecx
		jnz mtslop

		mov iminX,ebx
		mov imaxX,edx

		mov esi,srcArr
		mov ecx,nPoints
		mov ebx,0x7fffffff
		mov edx,0x80000000
mtslop4:
		add esi,4
		lodsd
		cmp ebx,eax
		cmovg ebx,eax
		cmp edx,eax
		cmovl edx,eax

		dec ecx
		jnz mtslop4

		mov iminY,ebx
		mov imaxY,edx
	}
	return (imaxX >= 0) && (iminX < (OSInt)scnWidth) && (imaxY >= 0) && (iminY < (OSInt)scnHeight);
#else

	Int32 iminX = 0;
	Int32 iminY = 0;
	Int32 imaxX = 0;
	Int32 imaxY = 0;
	Int32 thisX;
	Int32 thisY;
	while (nPoints-- > 0)
	{
		*destArr++ = thisX = Double2Int32((*srcArr++ * rRate - dleft) * xmul + ofstX);
		*destArr++ = thisY = Double2Int32((dbottom - *srcArr++ * rRate) * ymul + ofstY);
		if (iminX == 0 && imaxX == 0)
		{
			iminX = imaxX = thisX;
			iminY = imaxY = thisY;
		}
		else
		{
			if (thisX < iminX)
				iminX = thisX;
			if (thisX > imaxX)
				imaxX = thisX;
			if (thisY < iminY)
				iminY = thisY;
			if (thisY > imaxY)
				imaxY = thisY;
		}
	}
	return (imaxX >= 0) && (iminX < (OSInt)scnWidth) && (imaxY >= 0) && (iminY < (OSInt)scnHeight);
#endif
}

void Map::ProjectedMapView::MapXYToScnXY(Double mapX, Double mapY, Double *scnX, Double *scnY)
{
	*scnX = (mapX - this->leftX) * scnWidth / (this->rightX - this->leftX);
	*scnY = (this->bottomY - mapY) * scnHeight / (this->bottomY - this->topY);
}

void Map::ProjectedMapView::ScnXYToMapXY(Double scnX, Double scnY, Double *mapX, Double *mapY)
{
	*mapX = (this->leftX + (scnX * (this->rightX - this->leftX) / scnWidth));
	*mapY = (this->bottomY - (scnY * (this->bottomY - this->topY) / scnHeight));
}

Map::MapView *Map::ProjectedMapView::Clone()
{
	Map::ProjectedMapView *view;
	NEW_CLASS(view, Map::ProjectedMapView(this->scnWidth, this->scnHeight, this->centY, this->centX, this->scale));
	return view;
}
