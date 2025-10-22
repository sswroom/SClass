#include "Stdafx.h"
#include "MyMemory.h"
#include "Exporter/ExcelXMLExporter.h"
#include "Map/Leica/LeicaLevelFile.h"
#include "Math/Math.h"
#include "Media/DrawEngine.h"
#include "Text/SpreadSheet/Workbook.h"

Map::Leica::LeicaLevelFile::LeicaLevelFile()
{
	this->startLevel = 0;
	this->endLevel = 0;
}

Map::Leica::LeicaLevelFile::~LeicaLevelFile()
{
	OSInt i;
	NN<LevelPoint> point;
	i = this->pointList.GetCount();
	while (i-- > 0)
	{
		point = this->pointList.GetItemNoCheck(i);
		point->pointName->Release();
		MemFreeNN(point);
	}
}

void Map::Leica::LeicaLevelFile::AddPointHeight(Text::CStringNN pointName, NN<HeightMeasure> height)
{
	NN<LevelPoint> point;
	if (!this->pointMap.GetC(pointName).SetTo(point))
	{
		point = MemAllocNN(LevelPoint);
		point->pointName = Text::String::New(pointName);
		ClearMeasurement(point->interPoint);
		ClearMeasurement(point->backPoint);
		ClearMeasurement(point->forePoint);
		this->pointMap.PutNN(point->pointName, point);
		this->pointList.Add(point);
	}
	MemCopyNO(&point->height, height.Ptr(), sizeof(HeightMeasure));
	if (point->height.elevation != -1)
	{
		if (this->startLevel == 0)
		{
			this->startLevel = point->height.elevation * 0.00001;
		}
		this->endLevel = point->height.elevation * 0.00001;
	}
}

void Map::Leica::LeicaLevelFile::AddMeasurement(Text::CStringNN pointName, NN<Measurement> point, UTF8Char pointId)
{
	NN<LevelPoint> lpoint;
	if (!this->pointMap.GetC(pointName).SetTo(lpoint))
	{
		lpoint = MemAllocNN(LevelPoint);
		lpoint->pointName = Text::String::New(pointName);
		ClearMeasurement(lpoint->interPoint);
		ClearMeasurement(lpoint->backPoint);
		ClearMeasurement(lpoint->forePoint);
		ClearHeight(lpoint->height);
		this->pointMap.PutNN(lpoint->pointName, lpoint);
		this->pointList.Add(lpoint);
	}
	if (pointId == '1')
	{
		MemCopyNO(&lpoint->backPoint, point.Ptr(), sizeof(Measurement));
	}
	else if (pointId == '2')
	{
		MemCopyNO(&lpoint->forePoint, point.Ptr(), sizeof(Measurement));
	}
	else if (pointId == '0')
	{
		MemCopyNO(&lpoint->interPoint, point.Ptr(), sizeof(Measurement));
	}
}

void Map::Leica::LeicaLevelFile::SetStartLevel(Double startLevel)
{
	this->startLevel = startLevel;
}

void Map::Leica::LeicaLevelFile::SetEndLevel(Double endLevel)
{
	this->endLevel = endLevel;
}

Map::Leica::LeicaGSIFile::FileType Map::Leica::LeicaLevelFile::GetFileType()
{
	return Map::Leica::LeicaGSIFile::FT_LEVEL;
}

Bool Map::Leica::LeicaLevelFile::ExportExcel(NN<IO::SeekableStream> stm, Text::CStringNN fileName)
{
	NN<Text::SpreadSheet::Workbook> wb;
	NN<Text::SpreadSheet::Worksheet> ws;
	NN<Text::SpreadSheet::CellStyle> style1;
	NN<Text::SpreadSheet::CellStyle> style2;
	NN<Text::SpreadSheet::CellStyle> style3;
	NN<Text::SpreadSheet::CellStyle> style4;
	NN<Text::SpreadSheet::CellStyle> style5;
	NN<Text::SpreadSheet::CellStyle> style6;
	Text::SpreadSheet::CellStyle::BorderStyle border;
	Text::PString pointName;
	Bool succ = false;
	UInt32 i;
	UInt32 j;
	NEW_CLASSNN(wb, Text::SpreadSheet::Workbook());
	wb->AddDefaultStyles();
	
	border.borderType = Text::SpreadSheet::BorderType::Medium;
	border.borderColor = 0xff0000ff;

	style1 = wb->NewCellStyle();
	style1->SetFont(wb->NewFont(CSTR("Arial"), 8, true));
	style1->SetFontColor(0xff0000ff);
	style1->SetHAlign(Text::HAlignment::Center);
	style1->SetBorderBottom(border);
	style1->SetBorderLeft(border);
	style1->SetBorderRight(border);
	style1->SetBorderTop(border);
	style2 = wb->NewCellStyle();
	style2->SetFont(wb->NewFont(CSTR("Arial"), 8, true));
	style2->SetFontColor(0xff0000ff);
	style2->SetHAlign(Text::HAlignment::Center);
	style2->SetBorderBottom(border);
	style2->SetBorderLeft(border);
	style2->SetBorderRight(border);
	style2->SetBorderTop(border);
	style2->SetFillColor(0xff00ff00, Text::SpreadSheet::CellStyle::FillPattern::FP_SOLID_FOREGROUND);
	style3 = wb->NewCellStyle();
	style3->SetFont(wb->NewFont(CSTR("Arial"), 8, true));
	style3->SetFontColor(0xff0000ff);
	style3->SetHAlign(Text::HAlignment::Center);
	style3->SetBorderBottom(border);
	style3->SetBorderLeft(border);
	style3->SetBorderRight(border);
	style3->SetBorderTop(border);
	style3->SetFillColor(0xff00ffff, Text::SpreadSheet::CellStyle::FillPattern::FP_SOLID_FOREGROUND);
	ws = wb->AddWorksheet(CSTR("LEICA"));
	ws->SetCellString(10, 1, CSTR("No."));
	ws->SetCellStyle(10, 1, style1);
	ws->SetCellString(10, 2, CSTR("Point"));
	ws->SetCellStyle(10, 2, style1);
	ws->SetCellString(10, 3, CSTR("Back"));
	ws->SetCellStyle(10, 3, style1);
	ws->SetCellString(10, 4, CSTR("Inter"));
	ws->SetCellStyle(10, 4, style1);
	ws->SetCellString(10, 5, CSTR("Fore"));
	ws->SetCellStyle(10, 5, style1);
	ws->SetCellString(10, 6, CSTR("dH"));
	ws->SetCellStyle(10, 6, style1);
	ws->SetCellString(10, 7, CSTR("Dist1"));
	ws->SetCellStyle(10, 7, style1);
	ws->SetCellString(10, 8, CSTR("Dist0"));
	ws->SetCellStyle(10, 8, style1);
	ws->SetCellString(10, 9, CSTR("Dist2"));
	ws->SetCellStyle(10, 9, style1);
	ws->SetCellString(10, 10, CSTR("Prov. R.L."));
	ws->SetCellStyle(10, 10, style1);
	ws->SetCellString(10, 11, CSTR("Corr"));
	ws->SetCellStyle(10, 11, style1);
	ws->SetCellString(10, 12, CSTR("Adj.R.L.(S)"));
	ws->SetCellStyle(10, 12, style2);
	ws->SetCellString(10, 13, CSTR("Corr"));
	ws->SetCellStyle(10, 13, style1);
	ws->SetCellString(10, 14, CSTR("Adj.R.L.(D)"));
	ws->SetCellStyle(10, 14, style3);

	ws->SetColWidth(1, 25.5, Math::Unit::Distance::DU_EMU);
	ws->SetColWidth(2, 63.75, Math::Unit::Distance::DU_EMU);
	ws->SetColWidth(3, 70.5, Math::Unit::Distance::DU_EMU);
	ws->SetColWidth(4, 63, Math::Unit::Distance::DU_EMU);
	ws->SetColWidth(5, 77.25, Math::Unit::Distance::DU_EMU);
	ws->SetColWidth(6, 66.75, Math::Unit::Distance::DU_EMU);
	ws->SetColWidth(7, 59.25, Math::Unit::Distance::DU_EMU);
	ws->SetColWidth(8, 59.25, Math::Unit::Distance::DU_EMU);
	ws->SetColWidth(9, 58.5, Math::Unit::Distance::DU_EMU);
	ws->SetColWidth(10, 56.25, Math::Unit::Distance::DU_EMU);
	ws->SetColWidth(11, 66, Math::Unit::Distance::DU_EMU);
	ws->SetColWidth(12, 56.25, Math::Unit::Distance::DU_EMU);
	ws->SetZoom(85);

	style1 = wb->NewCellStyle();
	style1->SetFont(wb->NewFont(CSTR("Arial"), 8, true));
	style1->SetHAlign(Text::HAlignment::Right);
	ws->SetCellString(1, 2, CSTR("Title:"));
	ws->SetCellStyle(1, 2, style1);
	ws->SetCellString(2, 2, CSTR("Job no."));
	ws->SetCellStyle(2, 2, style1);
	ws->SetCellString(3, 2, CSTR("Comp Folder"));
	ws->SetCellStyle(3, 2, style1);
	ws->SetCellString(4, 2, CSTR("File"));
	ws->SetCellStyle(4, 2, style1);
	ws->SetCellString(5, 2, CSTR("Observed by"));
	ws->SetCellStyle(5, 2, style1);
	ws->SetCellString(6, 2, CSTR("Checked by"));
	ws->SetCellStyle(6, 2, style1);
	ws->SetCellString(7, 2, CSTR("Survey Date"));
	ws->SetCellStyle(7, 2, style1);
	ws->SetCellString(8, 2, CSTR("Instrument"));
	ws->SetCellStyle(8, 2, style1);

	ws->SetCellString(2, 10, CSTR("Starting Stn."));
	ws->SetCellStyle(2, 10, style1);
	ws->SetCellString(3, 10, CSTR("Closing Stn."));
	ws->SetCellStyle(3, 10, style1);
	ws->SetCellString(4, 10, CSTR("Misclosure"));
	ws->SetCellStyle(4, 10, style1);
	ws->SetCellString(5, 10, CSTR("Total BS dist."));
	ws->SetCellStyle(5, 10, style1);
	ws->SetCellString(6, 10, CSTR("Total FS dist."));
	ws->SetCellStyle(6, 10, style1);
	ws->SetCellString(7, 10, CSTR("Total Chainage"));
	ws->SetCellStyle(7, 10, style1);
	ws->SetCellString(8, 10, CSTR("Adjustment per setup"));
	ws->SetCellStyle(8, 10, style1);
	ws->SetCellString(9, 10, CSTR("Total Setup"));
	ws->SetCellStyle(9, 10, style1);

	style1 = wb->NewCellStyle();
	style1->SetBorderLeft(border);
	style1->SetBorderTop(border);
	style1->SetBorderRight(border);
	style1->SetBorderBottom(border);
	style1->SetFont(wb->NewFont(CSTR("Arial"), 10, true));
	style1->SetHAlign(Text::HAlignment::Center);
	ws->SetCellString(1, 11, CSTR("Name"));
	ws->SetCellStyle(1, 11, style1);
	ws->SetCellString(1, 12, CSTR("RL"));
	ws->SetCellStyle(1, 12, style1);

	border.borderType = Text::SpreadSheet::BorderType::Thin;
	style1 = wb->NewCellStyle();
	style1->SetFont(wb->NewFont(CSTR("Arial"), 10, false));
	style1->SetFontColor(0xff0000ff);
	style1->SetHAlign(Text::HAlignment::Center);
	style1->SetBorderLeft(border);
	style1->SetBorderRight(border);
	style2 = wb->NewCellStyle();
	style2->SetFont(wb->NewFont(CSTR("Arial"), 10, true));
	style2->SetFontColor(0xff3366ff);
	style2->SetHAlign(Text::HAlignment::Center);
	style2->SetDataFormat(CSTR("0.00000"));
	style2->SetBorderLeft(border);
	style2->SetBorderRight(border);
	style3 = wb->NewCellStyle();
	style3->SetFont(wb->NewFont(CSTR("Arial"), 10, true));
	style3->SetHAlign(Text::HAlignment::Center);
	style3->SetDataFormat(CSTR("0.00000"));
	style3->SetBorderLeft(border);
	style3->SetBorderRight(border);
	style4 = wb->NewCellStyle();
	style4->SetFont(wb->NewFont(CSTR("Arial"), 10, true));
	style4->SetFontColor(0xffff0000);
	style4->SetHAlign(Text::HAlignment::Center);
	style4->SetDataFormat(CSTR("0.00000"));
	style4->SetBorderLeft(border);
	style4->SetBorderRight(border);
	style5 = wb->NewCellStyle();
	style5->SetFont(wb->NewFont(CSTR("Arial"), 10, true));
	style5->SetFontColor(0xff0000ff);
	style5->SetHAlign(Text::HAlignment::Center);
	style5->SetDataFormat(CSTR("0.00000"));
	style5->SetBorderLeft(border);
	style5->SetBorderRight(border);
	style6 = wb->NewCellStyle();
	style6->SetFont(wb->NewFont(CSTR("Arial"), 10, true));
	style6->SetHAlign(Text::HAlignment::Center);
	style6->SetDataFormat(CSTR("0.00"));
	style6->SetBorderLeft(border);
	style6->SetBorderRight(border);

	Int32 lastHeight = -1;
	Int32 lastElev = -1;
	Int32 dH;
	Int64 totalBSDist = 0;
	Int64 totalFSDist = 0;
	Int64 totalBSHeight = 0;
	Int64 totalFSHeight = 0;
	Int32 dHCount = 0;
	Bool dHFound;
	Data::ArrayListInt32 rlList;
	NN<LevelPoint> point;
	i = 0;
	j = (UInt32)this->pointList.GetCount();
	while (i < j)
	{
		point = this->pointList.GetItemNoCheck(i);
		ws->SetCellStyle(11 + i, 1, style1);
		ws->SetCellStyle(11 + i, 2, style2);
		ws->SetCellStyle(11 + i, 3, style3);
		ws->SetCellStyle(11 + i, 4, style3);
		ws->SetCellStyle(11 + i, 5, style3);
		ws->SetCellStyle(11 + i, 6, style2);
		ws->SetCellStyle(11 + i, 7, style6);
		ws->SetCellStyle(11 + i, 8, style6);
		ws->SetCellStyle(11 + i, 9, style6);
		ws->SetCellStyle(11 + i, 10, style2);
		ws->SetCellStyle(11 + i, 11, style4);
		ws->SetCellStyle(11 + i, 12, style4);
		ws->SetCellStyle(11 + i, 13, style5);
		ws->SetCellStyle(11 + i, 14, style5);

		ws->SetCellInt32(11 + i, 1, i + 1);
		pointName = point->pointName.Ptr()[0];
		while (pointName.v[0] == '0')
		{
			pointName = pointName.Substring(1);
		}
		ws->SetCellString(11 + i, 2, pointName.ToCString());
		if (point->backPoint.heightDiff != -1)
		{
			ws->SetCellDouble(11 + i, 3, point->backPoint.heightDiff * 0.00001);
			totalBSHeight += point->backPoint.heightDiff;
		}
		if (point->interPoint.heightDiff != -1)
		{
			ws->SetCellDouble(11 + i, 4, point->interPoint.heightDiff * 0.00001);
		}
		if (point->forePoint.heightDiff != -1)
		{
			ws->SetCellDouble(11 + i, 5, point->forePoint.heightDiff * 0.00001);
			totalFSHeight += point->forePoint.heightDiff;
		}
		if (lastHeight != -1 && point->forePoint.heightDiff != -1)
		{
			dH = lastHeight - point->forePoint.heightDiff;
			dHFound = true;

			ws->SetCellDouble(11 + i, 6, dH * 0.00001);
			dHCount++;
		}
		else
		{
			dH = 0;
			dHFound = false;
		}
		if (point->backPoint.horizonalDistance != -1)
		{
			ws->SetCellDouble(11 + i, 7, point->backPoint.horizonalDistance * 0.00001);
			totalBSDist += point->backPoint.horizonalDistance;
		}
		if (point->interPoint.horizonalDistance != -1)
		{
			ws->SetCellDouble(11 + i, 8, point->interPoint.horizonalDistance * 0.00001);
		}
		if (point->forePoint.horizonalDistance != -1)
		{
			ws->SetCellDouble(11 + i, 9, point->forePoint.horizonalDistance * 0.00001);
			totalFSDist += point->forePoint.horizonalDistance;
		}
		if (dHFound && lastElev != -1)
		{
			lastElev += dH;
			ws->SetCellDouble(11 + i, 10, lastElev * 0.00001);
			rlList.Add(lastElev);
		}
		else
		{
			lastElev = point->height.elevation;
			if (i == 0)
			{
				lastElev = Double2Int32(this->startLevel * 100000);
			}
			if (lastElev != -1)
			{
				ws->SetCellDouble(11 + i, 10, lastElev * 0.00001);
			}
			rlList.Add(lastElev);
		}

		lastHeight = point->backPoint.heightDiff;

		i++;
	}
	while (j < 500)
	{
		ws->SetCellStyle(11 + j, 1, style1);
		ws->SetCellStyle(11 + j, 2, style2);
		ws->SetCellStyle(11 + j, 3, style3);
		ws->SetCellStyle(11 + j, 4, style3);
		ws->SetCellStyle(11 + j, 5, style3);
		ws->SetCellStyle(11 + j, 6, style2);
		ws->SetCellStyle(11 + j, 7, style6);
		ws->SetCellStyle(11 + j, 8, style6);
		ws->SetCellStyle(11 + j, 9, style6);
		ws->SetCellStyle(11 + j, 10, style2);
		ws->SetCellStyle(11 + j, 11, style4);
		ws->SetCellStyle(11 + j, 12, style4);
		ws->SetCellStyle(11 + j, 13, style5);
		ws->SetCellStyle(11 + j, 14, style5);

		ws->SetCellInt32(11 + j, 1, j + 1);
		j++;
	}
	style1 = wb->NewCellStyle();
	style1->SetBorderTop(border);
	ws->SetCellStyle(511, 1, style1);
	ws->SetCellStyle(511, 2, style1);
	ws->SetCellStyle(511, 3, style1);
	ws->SetCellStyle(511, 4, style1);
	ws->SetCellStyle(511, 5, style1);
	ws->SetCellStyle(511, 6, style1);
	ws->SetCellStyle(511, 7, style1);
	ws->SetCellStyle(511, 8, style1);
	ws->SetCellStyle(511, 9, style1);
	ws->SetCellStyle(511, 10, style1);
	ws->SetCellStyle(511, 11, style1);
	ws->SetCellStyle(511, 12, style1);
	ws->SetCellStyle(511, 13, style1);
	ws->SetCellStyle(511, 14, style1);
	
	style1 = wb->NewCellStyle();
	style1->SetFillColor(0xffffff00, Text::SpreadSheet::CellStyle::FillPattern::FP_SOLID_FOREGROUND);
	border.borderType = Text::SpreadSheet::BorderType::Medium;
	style1->SetBorderLeft(border);
	style1->SetBorderTop(border);
	style1->SetBorderRight(border);
	style1->SetBorderBottom(border);
	style1->SetFont(wb->NewFont(CSTR("Arial"), 10, true));
	style1->SetFontColor(0xff0000ff);
	style1->SetHAlign(Text::HAlignment::Center);
	pointName = this->pointList.GetItemNoCheck(0)->pointName.Ptr()[0];
	while (pointName.v[0] == '0')
		pointName = pointName.Substring(1);
	ws->SetCellString(2, 11, pointName.ToCString());
	ws->SetCellStyle(2, 11, style1);
	pointName = this->pointList.GetItemNoCheck(this->pointList.GetCount() - 1)->pointName.Ptr()[0];
	while (pointName.v[0] == '0')
		pointName = pointName.Substring(1);
	ws->SetCellString(3, 11, pointName.ToCString());
	ws->SetCellStyle(3, 11, style1);

	style1 = wb->NewCellStyle();
	style1->SetFillColor(0xffffff00, Text::SpreadSheet::CellStyle::FillPattern::FP_SOLID_FOREGROUND);
	style1->SetBorderLeft(border);
	style1->SetBorderTop(border);
	style1->SetBorderRight(border);
	style1->SetBorderBottom(border);
	style1->SetFont(wb->NewFont(CSTR("Arial"), 10, true));
	style1->SetFontColor(0xff0000ff);
	style1->SetHAlign(Text::HAlignment::Right);
	style1->SetDataFormat(CSTR("0.00000"));
	style2 = wb->NewCellStyle();
	style2->SetFillColor(0xffccffcc, Text::SpreadSheet::CellStyle::FillPattern::FP_SOLID_FOREGROUND);
	style2->SetBorderLeft(border);
	style2->SetBorderTop(border);
	style2->SetBorderRight(border);
	style2->SetBorderBottom(border);
	style2->SetFont(wb->NewFont(CSTR("Arial"), 12, true));
	style2->SetFontColor(0xff0000ff);
	style2->SetHAlign(Text::HAlignment::Right);
	style2->SetDataFormat(CSTR("0.00 \"mm\""));
	ws->SetCellDouble(2, 12, this->startLevel);
	ws->SetCellStyle(2, 12, style1);
	ws->SetCellDouble(3, 12, this->endLevel);
	ws->SetCellStyle(3, 12, style1);

	Double misclosure = (this->startLevel * 100000 - this->endLevel * 100000) - (totalFSHeight - totalBSHeight);
	Double adjustment = -misclosure / dHCount;
	style1 = wb->NewCellStyle();
	style1->SetFont(wb->NewFont(CSTR("Arial"), 10, true));
	style1->SetFontColor(0xff0000ff);
	style1->SetDataFormat(CSTR("0.## \"m\""));
	style1->SetHAlign(Text::HAlignment::Right);
	style3 = wb->NewCellStyle();
	style3->SetFillColor(0xffccffcc, Text::SpreadSheet::CellStyle::FillPattern::FP_SOLID_FOREGROUND);
	style3->SetFont(wb->NewFont(CSTR("Arial"), 10, true));
	style3->SetFontColor(0xff0000ff);
	style3->SetDataFormat(CSTR("0.0000 \"mm\""));
	style3->SetHAlign(Text::HAlignment::Right);
	ws->SetCellDouble(4, 11, misclosure * 0.01);
	ws->SetCellStyle(4, 11, style2);
	ws->SetCellDouble(5, 11, totalBSDist * 0.00001);
	ws->SetCellStyle(5, 11, style1);
	ws->SetCellDouble(6, 11, totalFSDist * 0.00001);
	ws->SetCellStyle(6, 11, style1);
	ws->SetCellDouble(7, 11, (totalBSDist + totalFSDist) * 0.00001);
	ws->SetCellStyle(7, 11, style1);
	ws->SetCellDouble(8, 11, adjustment * 0.01);
	ws->SetCellStyle(8, 11, style3);

	style1 = wb->NewCellStyle();
	style1->SetFont(wb->NewFont(CSTR("Arial"), 10, true));
	style1->SetFontColor(0xff0000ff);
	style1->SetFillColor(0xffffff00, Text::SpreadSheet::CellStyle::FillPattern::FP_SOLID_FOREGROUND);
	style1->SetHAlign(Text::HAlignment::Center);
	ws->SetCellInt32(9, 11, dHCount);
	ws->SetCellStyle(9, 11, style1);

	Double corrV = 0;
	Double corrV2;
	Int64 currBSSum = 0;
	Int64 currFSSum = 0;
	i = 0;
	j = (UInt32)this->pointList.GetCount();
	while (i < j)
	{
		point = this->pointList.GetItemNoCheck(i);
		ws->SetCellDouble(11 + i, 11, corrV * 0.00001);
		ws->SetCellDouble(11 + i, 12, (rlList.GetItem(i) + corrV) * 0.00001);
		if (point->forePoint.horizonalDistance != -1)
		{
			currFSSum += point->forePoint.horizonalDistance;
		}
		corrV2 = (currFSSum + currBSSum) / (Double)(totalFSDist + totalBSDist) * -misclosure;
		ws->SetCellDouble(11 + i, 13, corrV2 * 0.00001);
		ws->SetCellDouble(11 + i, 14, (rlList.GetItem(i) + corrV2) * 0.00001);
		if (point->backPoint.horizonalDistance != -1)
		{
			currBSSum += point->backPoint.horizonalDistance;
		}

		corrV += adjustment;
		i++;
	}

	Exporter::ExcelXMLExporter exporter;
	succ = exporter.ExportFile(stm, fileName, wb, 0);
	wb.Delete();
	return succ;
}

UOSInt Map::Leica::LeicaLevelFile::GetPointCnt()
{
	return this->pointList.GetCount();
}

Text::CString Map::Leica::LeicaLevelFile::GetPointName(UOSInt index)
{
	NN<LevelPoint> point;
	Text::PString pointName;
	if (this->pointList.GetItem(index).SetTo(point))
	{
		pointName = point->pointName.Ptr()[0];
		while (pointName.v[0] == '0')
			pointName = pointName.Substring(1);
		return pointName.ToCString();
	}
	return nullptr;
}

Optional<const Map::Leica::LeicaLevelFile::Measurement> Map::Leica::LeicaLevelFile::GetPointBack(UOSInt index)
{
	NN<LevelPoint> point;
	if (this->pointList.GetItem(index).SetTo(point))
	{
		if (point->backPoint.heightDiff == -1)
			return 0;
		else
			return point->backPoint;
	}
	return 0;
}

Optional<const Map::Leica::LeicaLevelFile::Measurement> Map::Leica::LeicaLevelFile::GetPointInter(UOSInt index)
{
	NN<LevelPoint> point;
	if (this->pointList.GetItem(index).SetTo(point))
	{
		if (point->interPoint.heightDiff == -1)
			return 0;
		else
			return point->interPoint;
	}
	return 0;
}

Optional<const Map::Leica::LeicaLevelFile::Measurement> Map::Leica::LeicaLevelFile::GetPointFore(UOSInt index)
{
	NN<LevelPoint> point;
	if (this->pointList.GetItem(index).SetTo(point))
	{
		if (point->forePoint.heightDiff == -1)
			return 0;
		else
			return point->forePoint;
	}
	return 0;
}

Optional<const Map::Leica::LeicaLevelFile::HeightMeasure> Map::Leica::LeicaLevelFile::GetPointHeight(UOSInt index)
{
	NN<LevelPoint> point;;
	if (this->pointList.GetItem(index).SetTo(point))
	{
		return point->height;
	}
	return 0;
}

Bool Map::Leica::LeicaLevelFile::CalcPointLevs(NN<Data::ArrayList<Double>> calcLevs, NN<Data::ArrayList<Double>> adjLevS, NN<Data::ArrayList<Double>> adjLevD)
{
	NN<LevelPoint> point;
	UOSInt i;
	UOSInt j;
	Double corrV = 0;
	Double corrV2;
	Int32 lastHeight = -1;
	Double lastElev = -1;
	Int64 totalBSDist = 0;
	Int64 totalFSDist = 0;
	Int64 totalBSHeight = 0;
	Int64 totalFSHeight = 0;
	Int64 currBSSum = 0;
	Int64 currFSSum = 0;
	Int32 dH;
	Int32 dHCount = 0;
	Bool dHFound;
	Double levS;
	Double levD;
	calcLevs->Clear();

	i = 0;
	j = this->pointList.GetCount();
	while (i < j)
	{
		point = this->pointList.GetItemNoCheck(i);

		if (point->backPoint.heightDiff != -1)
		{
			totalBSHeight += point->backPoint.heightDiff;
		}
		if (point->forePoint.heightDiff != -1)
		{
			totalFSHeight += point->forePoint.heightDiff;
		}
		if (lastHeight != -1 && point->forePoint.heightDiff != -1)
		{
			dH = lastHeight - point->forePoint.heightDiff;
			dHFound = true;
			dHCount++;
		}
		else
		{
			dH = 0;
			dHFound = false;
		}
		if (point->backPoint.horizonalDistance != -1)
		{
			totalBSDist += point->backPoint.horizonalDistance;
		}
		if (point->forePoint.horizonalDistance != -1)
		{
			totalFSDist += point->forePoint.horizonalDistance;
		}
		if (dHFound && lastElev != -1)
		{
			lastElev += dH;
			calcLevs->Add(lastElev * 0.00001);
		}
		else
		{
			lastElev = point->height.elevation;
			if (i == 0)
			{
				lastElev = this->startLevel * 100000.0;
			}
			calcLevs->Add(lastElev * 0.00001);
		}

		lastHeight = point->backPoint.heightDiff;

		i++;
	}

	Double misclosure = (this->startLevel * 100000 - this->endLevel * 100000) - (totalFSHeight - totalBSHeight);
	Double adjustment = -misclosure / dHCount;

	i = 0;
	j = this->pointList.GetCount();
	while (i < j)
	{
		point = this->pointList.GetItemNoCheck(i);
		levS = calcLevs->GetItem(i) + (corrV * 0.00001);
		if (point->forePoint.horizonalDistance != -1)
		{
			currFSSum += point->forePoint.horizonalDistance;
		}
		corrV2 = (currFSSum + currBSSum) / (Double)(totalFSDist + totalBSDist) * -misclosure;
		levD = calcLevs->GetItem(i) + (corrV2 * 0.00001);
		if (point->backPoint.horizonalDistance != -1)
		{
			currBSSum += point->backPoint.horizonalDistance;
		}
		adjLevS->Add(levS);
		adjLevD->Add(levD);
		corrV += adjustment;
		i++;
	}

	return true;
}

void Map::Leica::LeicaLevelFile::ClearMeasurement(NN<Measurement> measure)
{
	measure->horizonalDistance = -1;
	measure->heightDiff = -1;
	measure->distExtra[0] = -1;
	measure->distExtra[1] = -1;
}

void Map::Leica::LeicaLevelFile::ClearHeight(NN<HeightMeasure> height)
{
	height->elevation = -1;
	height->distExtra[0] = -1;
	height->distExtra[1] = -1;
}