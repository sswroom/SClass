#include "Stdafx.h"
#include "MyMemory.h"
#include "SIMD.h"
#include "Map/ScaledMapView.h"
#include "Math/Math.h"

extern "C"
{
	Bool ScaledMapView_IMapXYToScnXY(const Int32 *srcArr, Int32 *destArr, UOSInt nPoints, Double rRate, Double dleft, Double dbottom, Double xmul, Double ymul, Int32 ofstX, Int32 ofstY, UOSInt scnWidth, UOSInt scnHeight);
}

Map::ScaledMapView::ScaledMapView(Double scnWidth, Double scnHeight, Double centLat, Double centLon, Double scale) : Map::MapView(scnWidth, scnHeight)
{
	this->hdpi = 96.0;
	this->ddpi = 96.0;
	ChangeViewXY(scnWidth, scnHeight, Math::Coord2D<Double>(centLon, centLat), scale);
}

Map::ScaledMapView::~ScaledMapView()
{
}

void Map::ScaledMapView::ChangeViewXY(Double scnWidth, Double scnHeight, Math::Coord2D<Double> centMap, Double scale)
{
	this->centMap = centMap;
	if (scale < 400)
		scale = 400;
	if (scale > 100000000)
		scale = 100000000;
	this->scale = scale;

	Double diffx = scnWidth * 0.00025 * scale / (this->hdpi * 72.0 / 96.0) * 2.54 / 10000.0;
	Double diffy = scnHeight * 0.00025 * scale / (this->hdpi * 72.0 / 96.0) * 2.54 / 10000.0;

	this->rightX = centMap.x + diffx;
	this->leftX = centMap.x - diffx;
	this->bottomY = centMap.y + diffy;
	this->topY = centMap.y - diffy;

	this->scnWidth = scnWidth;
	this->scnHeight = scnHeight;
}

void Map::ScaledMapView::SetCenterXY(Math::Coord2D<Double> mapPos)
{
	ChangeViewXY(scnWidth, scnHeight, mapPos, scale);
}

void Map::ScaledMapView::SetMapScale(Double scale)
{
	ChangeViewXY(scnWidth, scnHeight, this->centMap, scale);
}

void Map::ScaledMapView::UpdateSize(Double width, Double height)
{
	ChangeViewXY(width, height, this->centMap, scale);
}

void Map::ScaledMapView::SetDPI(Double hdpi, Double ddpi)
{
	if (hdpi > 0 && ddpi > 0 && (this->hdpi != hdpi || this->ddpi != ddpi))
	{
		this->hdpi = hdpi;
		this->ddpi = ddpi;
		ChangeViewXY(this->scnWidth, this->scnHeight, this->centMap, this->scale);
	}
}

Double Map::ScaledMapView::GetLeftX()
{
	return this->leftX;
}

Double Map::ScaledMapView::GetTopY()
{
	return this->topY;
}

Double Map::ScaledMapView::GetRightX()
{
	return this->rightX;
}

Double Map::ScaledMapView::GetBottomY()
{
	return this->bottomY;
}

Double Map::ScaledMapView::GetMapScale()
{
	return this->scale;
}

Double Map::ScaledMapView::GetViewScale()
{
	return this->scale;
}

Double Map::ScaledMapView::GetCenterX()
{
	return this->centMap.x;
}

Double Map::ScaledMapView::GetCenterY()
{
	return this->centMap.y;
}

Double Map::ScaledMapView::GetHDPI()
{
	return this->hdpi;
}

Double Map::ScaledMapView::GetDDPI()
{
	return this->ddpi;
}

Bool Map::ScaledMapView::InViewXY(Double mapX, Double mapY)
{
	return mapY >= topY && mapY < bottomY && mapX >= leftX && mapX < rightX;
}

Bool Map::ScaledMapView::MapXYToScnXY(const Double *srcArr, Int32 *destArr, UOSInt nPoints, Int32 ofstX, Int32 ofstY)
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

Bool Map::ScaledMapView::MapXYToScnXY(const Math::Coord2D<Double> *srcArr, Math::Coord2D<Double> *destArr, UOSInt nPoints, Math::Coord2D<Double> ofst)
{
	if (nPoints == 0)
	{
		return false;
	}

/*	Double xmul = this->scnWidth / (rightX - leftX);
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
		*destArr++ = thisX = ((*srcArr++  - dleft) * xmul + ofstX);
		*destArr++ = thisY = ((dbottom - *srcArr++) * ymul + ofstY);
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
	return (imaxX >= 0) && (iminX < scnWidth) && (imaxY >= 0) && (iminY < scnHeight);*/

	Double dleft = leftX;
	Double dbottom = bottomY;
	Doublex2 ptMul = PDoublex2Set(this->scnWidth / (rightX - leftX), this->scnHeight / (bottomY - topY));
	Doublex2 ptOfst = PDoublex2Set(ofst.x, ofst.y);
	Doublex2 thisVal;
	Doublex2 minVal;
	Doublex2 maxVal;
	thisVal = PDoublex2Set((srcArr->x  - dleft), (dbottom - srcArr->y));
	minVal = maxVal = PADDPD(PMULPD(thisVal, ptMul), ptOfst);
	PStoreDoublex2((Double*)destArr, minVal);
	srcArr++;
	destArr++;
	nPoints--;
	while (nPoints-- > 0)
	{
		thisVal = PDoublex2Set((srcArr->x  - dleft), (dbottom - srcArr->y));
		thisVal = PADDPD(PMULPD(thisVal, ptMul), ptOfst);
		PStoreDoublex2((Double*)destArr, thisVal);
		srcArr++;
		destArr++;
		minVal = PMINPD(minVal, thisVal);
		maxVal = PMAXPD(maxVal, thisVal);
	}
	return (Doublex2GetLo(maxVal) >= 0) && (Doublex2GetLo(minVal) < scnWidth) && (Doublex2GetHi(maxVal) >= 0) && (Doublex2GetHi(minVal) < scnHeight);
}

Bool Map::ScaledMapView::IMapXYToScnXY(Double mapRate, const Int32 *srcArr, Int32 *destArr, UOSInt nPoints, Int32 ofstX, Int32 ofstY)
{
	if (nPoints == 0)
	{
		return false;
	}
	Double xmul = this->scnWidth / (this->rightX - this->leftX);
	Double ymul = this->scnHeight / (this->bottomY - this->topY);
	Double dleft = this->leftX;
	Double dbottom = this->bottomY;
	Double rRate = 1 / mapRate;

	return ScaledMapView_IMapXYToScnXY(srcArr, destArr, nPoints, rRate, dleft, dbottom, xmul, ymul, ofstX, ofstY, (UOSInt)this->scnWidth, (UOSInt)this->scnHeight);
}

Math::Coord2D<Double> Map::ScaledMapView::MapXYToScnXY(Math::Coord2D<Double> mapPos)
{
	return Math::Coord2D<Double>((mapPos.x - this->leftX) * scnWidth / (this->rightX - this->leftX),
		(this->bottomY - mapPos.y) * scnHeight / (this->bottomY - this->topY));
}

Math::Coord2D<Double> Map::ScaledMapView::ScnXYToMapXY(Math::Coord2D<Double> scnPos)
{
	return Math::Coord2D<Double>((this->leftX + (scnPos.x * (this->rightX - this->leftX) / scnWidth)), 
		(this->bottomY - (scnPos.y * (this->bottomY - this->topY) / scnHeight)));
}

Map::MapView *Map::ScaledMapView::Clone()
{
	Map::ScaledMapView *view;
	NEW_CLASS(view, Map::ScaledMapView(this->scnWidth, this->scnHeight, this->centMap.y, this->centMap.x, this->scale));
	return view;
}
