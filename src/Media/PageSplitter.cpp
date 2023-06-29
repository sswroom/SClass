#include "Stdafx.h"
#include "Media/PageSplitter.h"
#include "Media/PaperSize.h"

Media::PageSplitter::PageSplitter()
{
	this->drawSize = Media::PaperSize::PaperTypeGetSizeMM(Media::PaperSize::PT_A4) * Math::Unit::Distance::GetConvertRatio(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_METER);
}

void Media::PageSplitter::SetDrawSize(Math::Size2DDbl size, Math::Unit::Distance::DistanceUnit unit)
{
	this->drawSize = size * Math::Unit::Distance::GetConvertRatio(unit, Math::Unit::Distance::DU_METER);
}

UOSInt Media::PageSplitter::SplitDrawings(Data::ArrayListA<Math::RectAreaDbl> *pages, Math::RectAreaDbl objectArea, Double objectBuffer, Double pageOverlapBuffer, Double scale)
{
	Math::Size2DDbl drawMapSize =  this->drawSize * scale;
	if (pageOverlapBuffer >= drawMapSize.width || pageOverlapBuffer >= drawMapSize.height)
	{
		return 0;
	}
	objectArea = objectArea.Reorder().Expand(objectBuffer);
	Math::Coord2DDbl objAreaSize = objectArea.GetSize();
	UOSInt paperXCount = 1;
	UOSInt paperYCount = 1;
	Math::Coord2DDbl totalDrawSize = drawMapSize.ToCoord();
	while (totalDrawSize.x < objAreaSize.x)
	{
		paperXCount++;
		totalDrawSize.x += drawMapSize.width - pageOverlapBuffer;
	}
	while (totalDrawSize.y < objAreaSize.y)
	{
		paperYCount++;
		totalDrawSize.y += drawMapSize.height - pageOverlapBuffer;
	}
	Math::Coord2DDbl center = objectArea.GetCenter();
	Math::Coord2DDbl tl = center - totalDrawSize * 0.5;
	Math::Coord2DDbl currPt = tl;
	UOSInt i = 0;
	UOSInt j = 0;
	while (j < paperYCount)
	{
		currPt.x = tl.x;
		i = 0;
		while (i < paperXCount)
		{
			pages->Add(Math::RectAreaDbl(currPt, currPt + drawMapSize.ToCoord()));
			currPt.x += drawMapSize.width - pageOverlapBuffer;
			i++;
		}
		currPt.y += drawMapSize.height - pageOverlapBuffer;
		j++;
	}
	return pages->GetCount();
}	
