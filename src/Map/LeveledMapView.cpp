#include "Stdafx.h"
#include "MyMemory.h"
#include "Map/LeveledMapView.h"
#include "Math/Math.h"

void Map::LeveledMapView::UpdateVals()
{
	Double scale = this->scales->GetItem(this->level);
	Double diffx = Math::UOSInt2Double(this->scnWidth) * 0.00025 * scale / (this->hdpi * 72.0 / 96) * 2.54 / 10000.0;
	Double diffy = Math::UOSInt2Double(this->scnHeight) * 0.00025 * scale / (this->hdpi * 72.0 / 96) * 2.54 / 10000.0;

	this->rightX = this->centX + diffx;
	this->leftX = this->centX - diffx;
	this->bottomY = this->centY + diffy;
	this->topY = this->centY - diffy;
}

Map::LeveledMapView::LeveledMapView(UOSInt scnWidth, UOSInt scnHeight, Double centLat, Double centLon, Data::ArrayListDbl *scales) : Map::MapView(scnWidth, scnHeight)
{
	this->hdpi = 96.0;
	this->ddpi = 96.0;
	NEW_CLASS(this->scales, Data::ArrayListDbl());
	this->scales->AddRange(scales);
	ChangeViewXY(scnWidth, scnHeight, centLon, centLat, Math::Double2Int32(this->scales->GetItem(this->scales->GetCount() >> 1)));
}

Map::LeveledMapView::~LeveledMapView()
{
	DEL_CLASS(this->scales);
}

void Map::LeveledMapView::ChangeViewXY(UOSInt scnWidth, UOSInt scnHeight, Double centX, Double centY, Double scale)
{
	this->centX = centX;
	this->centY = centY;
	this->scnWidth = scnWidth;
	this->scnHeight = scnHeight;
	this->SetMapScale(scale);
}

void Map::LeveledMapView::SetCenterXY(Double x, Double y)
{
	this->centX = x;
	this->centY = y;
	this->UpdateVals();
}

void Map::LeveledMapView::SetMapScale(Double scale)
{
	Double ldiff;
	Double minDiff;
	UOSInt minInd;
	UOSInt i;
	Double logResol = Math::Log10(scale);
	minInd = 0;
	minDiff = 100000.0;
	i = this->scales->GetCount();
	while (i-- > 0)
	{
		ldiff = Math::Log10(this->scales->GetItem(i)) - logResol;
		if (ldiff < 0)
			ldiff = -ldiff;
		if (ldiff < minDiff)
		{
			minDiff = ldiff;
			minInd = i;
		}
	}
	this->level = (UInt32)minInd;
	this->UpdateVals();
}

void Map::LeveledMapView::UpdateSize(UOSInt width, UOSInt height)
{
	this->scnWidth = width;
	this->scnHeight = height;
	this->UpdateVals();
}

void Map::LeveledMapView::SetDPI(Double hdpi, Double ddpi)
{
	if (hdpi > 0 && ddpi > 0 && (this->hdpi != hdpi || this->ddpi != ddpi))
	{
		this->hdpi = hdpi;
		this->ddpi = ddpi;
		this->UpdateVals();
	}
}

Double Map::LeveledMapView::GetLeftX()
{
	return this->leftX;
}

Double Map::LeveledMapView::GetTopY()
{
	return this->topY;
}

Double Map::LeveledMapView::GetRightX()
{
	return this->rightX;
}

Double Map::LeveledMapView::GetBottomY()
{
	return this->bottomY;
}

Double Map::LeveledMapView::GetMapScale()
{
	return this->scales->GetItem(this->level);
}

Double Map::LeveledMapView::GetViewScale()
{
	return this->scales->GetItem(this->level);
}

Double Map::LeveledMapView::GetCenterX()
{
	return this->centX;
}

Double Map::LeveledMapView::GetCenterY()
{
	return this->centY;
}

Double Map::LeveledMapView::GetHDPI()
{
	return this->hdpi;
}

Double Map::LeveledMapView::GetDDPI()
{
	return this->ddpi;
}

Bool Map::LeveledMapView::InViewXY(Double x, Double y)
{
	return y >= topY && y < bottomY && x >= leftX && x < rightX;
}

Bool Map::LeveledMapView::MapXYToScnXY(const Double *srcArr, Int32 *destArr, UOSInt nPoints, Int32 ofstX, Int32 ofstY)
{
	if (nPoints == 0)
	{
		return false;
	}

	Double xmul = Math::UOSInt2Double(this->scnWidth) / (rightX - leftX);
	Double ymul = Math::UOSInt2Double(this->scnHeight) / (bottomY - topY);
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
		*destArr++ = thisX = Math::Double2Int32((*srcArr++  - dleft) * xmul + ofstX);
		*destArr++ = thisY = Math::Double2Int32((dbottom - *srcArr++) * ymul + ofstY);
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

Bool Map::LeveledMapView::MapXYToScnXY(const Double *srcArr, Double *destArr, UOSInt nPoints, Double ofstX, Double ofstY)
{
	if (nPoints == 0)
	{
		return false;
	}

	Double xmul = Math::UOSInt2Double(this->scnWidth) / (rightX - leftX);
	Double ymul = Math::UOSInt2Double(this->scnHeight) / (bottomY - topY);
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
	return (imaxX >= 0) && (iminX < Math::UOSInt2Double(scnWidth)) && (imaxY >= 0) && (iminY < Math::UOSInt2Double(scnHeight));
}

Bool Map::LeveledMapView::IMapXYToScnXY(Double mapRate, const Int32 *srcArr, Int32 *destArr, UOSInt nPoints, Int32 ofstX, Int32 ofstY)
{
	if (nPoints == 0)
	{
		return false;
	}
	Double xmul = Math::UOSInt2Double(this->scnWidth) / (rightX - leftX);
	Double ymul = Math::UOSInt2Double(this->scnHeight) / (bottomY - topY);
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
		*destArr++ = thisX = Math::Double2Int32((*srcArr++ * rRate - dleft) * xmul + ofstX);
		*destArr++ = thisY = Math::Double2Int32((dbottom - *srcArr++ * rRate) * ymul + ofstY);
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

void Map::LeveledMapView::MapXYToScnXY(Double mapX, Double mapY, Double *scnX, Double *scnY)
{
	*scnX = (mapX - this->leftX) * Math::UOSInt2Double(scnWidth) / (this->rightX - this->leftX);
	*scnY = (this->bottomY - mapY) * Math::UOSInt2Double(scnHeight) / (this->bottomY - this->topY);
}

void Map::LeveledMapView::ScnXYToMapXY(Double scnX, Double scnY, Double *mapX, Double *mapY)
{
	*mapX = (this->leftX + (scnX * (this->rightX - this->leftX) / Math::UOSInt2Double(scnWidth)));
	*mapY = (this->bottomY - (scnY * (this->bottomY - this->topY) / Math::UOSInt2Double(scnHeight)));
}

Map::MapView *Map::LeveledMapView::Clone()
{
	Map::LeveledMapView *view;
	NEW_CLASS(view, Map::LeveledMapView(this->scnWidth, this->scnHeight, this->centY, this->centX, this->scales));
	return view;
}
