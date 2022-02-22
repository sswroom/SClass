#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/FastStringMap.h"
#include "Data/StringUTF8Map.h"
#include "Math/Math.h"
#include "Media/PaperSize.h"
#include "Text/MyStringFloat.h"
#include "Text/ReportBuilder.h"
#include "Text/StringBuilderUTF8.h"

Text::SpreadSheet::AxisType Text::ReportBuilder::FromChartDataType(Data::IChart::DataType dataType)
{
	switch (dataType)
	{
	case Data::IChart::DataType::DateTicks:
		return Text::SpreadSheet::AxisType::Date;
	case Data::IChart::DataType::None:
	case Data::IChart::DataType::DOUBLE:
	case Data::IChart::DataType::Integer:
		return Text::SpreadSheet::AxisType::Numeric;
	default:
		return Text::SpreadSheet::AxisType::Numeric;
	}
}

Text::ReportBuilder::ReportBuilder(const UTF8Char *name, UOSInt colCount, const UTF8Char **columns)
{
	Text::String **cols;
	UOSInt i;
	this->name = Text::String::NewNotNull(name);
	this->fontName = Text::String::New(UTF8STRC("Arial"));
	this->colCount = colCount;
	this->colWidthPts = MemAlloc(Double, this->colCount);
	this->chart = 0;
	this->paperHori = false;
	this->colTypes = MemAlloc(ColType, this->colCount);
	cols = MemAlloc(Text::String *, this->colCount);
	i = 0;
	while (i < this->colCount)
	{
		colWidthPts[i] = 0;
		if (columns[i])
		{
			cols[i] = Text::String::NewNotNull(columns[i]);
		}
		else
		{
			cols[i] = 0;
		}
		this->colTypes[i] = CT_STRING;
		i++;
	}
	NEW_CLASS(this->preheaders, Data::ArrayList<Text::String **>());
	NEW_CLASS(this->headers, Data::ArrayList<Text::String **>());
	NEW_CLASS(this->tableContent, Data::ArrayList<Text::String **>());
	NEW_CLASS(this->tableRowType, Data::ArrayList<RowType>());
	NEW_CLASS(this->urlList, Data::ArrayList<ColURLLatLon*>());
	NEW_CLASS(this->icons, Data::ArrayList<Data::ArrayList<Text::ReportBuilder::ColIcon*>*>());
	this->tableContent->Add(cols);
	this->tableRowType->Add(RT_HEADER);
}

Text::ReportBuilder::~ReportBuilder()
{
	ColURLLatLon *url;
	Text::ReportBuilder::ColIcon *icon;
	Text::String **cols;
	Data::ArrayList<Text::ReportBuilder::ColIcon*> *iconList;
	UOSInt i;
	UOSInt j;
	j = this->headers->GetCount();
	while (j-- > 0)
	{
		cols = this->headers->GetItem(j);
		cols[0]->Release();
		cols[1]->Release();
		MemFree(cols);
	}
	j = this->preheaders->GetCount();
	while (j-- > 0)
	{
		cols = this->preheaders->GetItem(j);
		cols[0]->Release();
		cols[1]->Release();
		MemFree(cols);
	}
	j = this->tableContent->GetCount();
	while (j-- > 0)
	{
		cols = this->tableContent->GetItem(j);
		i = this->colCount;
		while (i-- > 0)
		{
			SDEL_STRING(cols[i]);
		}
		MemFree(cols);
	}
	i = this->urlList->GetCount();
	while (i-- > 0)
	{
		url = this->urlList->GetItem(i);
		MemFree(url);
	}
	i = this->icons->GetCount();
	while (i-- > 0)
	{
		iconList = this->icons->GetItem(i);
		if (iconList)
		{
			j = iconList->GetCount();
			while (j-- > 0)
			{
				icon = iconList->GetItem(j);
				SDEL_STRING(icon->fileName);
				SDEL_STRING(icon->name);
				MemFree(icon);
			}
			DEL_CLASS(iconList);
		}
	}
	DEL_CLASS(this->headers);
	DEL_CLASS(this->preheaders);
	DEL_CLASS(this->tableContent);
	DEL_CLASS(this->tableRowType);
	DEL_CLASS(this->urlList);
	SDEL_CLASS(this->chart);
	DEL_CLASS(this->icons);
	MemFree(this->colWidthPts);
	MemFree(this->colTypes);
	this->fontName->Release();
	this->name->Release();
}

void Text::ReportBuilder::SetFontName(Text::String *fontName)
{
	if (fontName)
	{
		this->fontName->Release();
		this->fontName = fontName->Clone();
	}
}

void Text::ReportBuilder::SetFontName(const UTF8Char *fontName)
{
	if (fontName)
	{
		this->fontName->Release();
		this->fontName = Text::String::NewNotNull(fontName);
	}
}

void Text::ReportBuilder::SetPaperHori(Bool paperHori)
{
	this->paperHori = paperHori;
}

void Text::ReportBuilder::AddChart(Data::IChart *chart)
{
	SDEL_CLASS(this->chart);
	this->chart = chart;
}

void Text::ReportBuilder::AddPreHeader(Text::CString name, Text::CString val)
{
	Text::String **cols;
	cols = MemAlloc(Text::String *, 2);
	cols[0] = Text::String::New(name.v, name.leng);
	cols[1] = Text::String::New(val.v, val.leng);
	this->preheaders->Add(cols);
}

void Text::ReportBuilder::AddHeader(Text::CString name, Text::CString val)
{
	Text::String **cols;
	cols = MemAlloc(Text::String *, 2);
	cols[0] = Text::String::New(name.v, name.leng);
	cols[1] = Text::String::New(val.v, val.leng);
	this->headers->Add(cols);
}

void Text::ReportBuilder::AddTableContent(const UTF8Char **content)
{
	Text::String **cols;
	UOSInt i;
	cols = MemAlloc(Text::String *, this->colCount);
	i = 0;
	while (i < this->colCount)
	{
		if (content[i])
		{
			cols[i] = Text::String::NewNotNull(content[i]);
		}
		else
		{
			cols[i] = 0;
		}
		i++;
	}
	this->tableContent->Add(cols);
	this->tableRowType->Add(RT_CONTENT);
}

void Text::ReportBuilder::AddTableSummary(const UTF8Char **content)
{
	Text::String **cols;
	UOSInt i;
	cols = MemAlloc(Text::String *, this->colCount);
	i = 0;
	while (i < this->colCount)
	{
		if (content[i])
		{
			cols[i] = Text::String::NewNotNull(content[i]);
		}
		else
		{
			cols[i] = 0;
		}
		i++;
	}
	this->tableContent->Add(cols);
	this->tableRowType->Add(RT_SUMMARY);
}

void Text::ReportBuilder::AddIcon(UOSInt index, Text::CString fileName, Text::CString name)
{
	UOSInt cnt = this->tableContent->GetCount() - 1;
	Data::ArrayList<Text::ReportBuilder::ColIcon*> *iconList;
	Text::ReportBuilder::ColIcon *icon;
	while (this->icons->GetCount() < cnt)
	{
		this->icons->Add(0);
	}
	iconList = this->icons->GetItem(cnt);
	if (iconList == 0)
	{
		NEW_CLASS(iconList, Data::ArrayList<Text::ReportBuilder::ColIcon*>());
		this->icons->Add(iconList);
	}
	icon = MemAlloc(Text::ReportBuilder::ColIcon, 1);
	icon->col = index;
	if (fileName.v)
	{
		icon->fileName = Text::String::New(fileName.v, fileName.leng);
	}
	else
	{
		icon->fileName = 0;
	}
	if (name.v)
	{
		icon->name = Text::String::New(name.v, name.leng);
	}
	else
	{
		icon->name = 0;
	}
	iconList->Add(icon);
}

void Text::ReportBuilder::SetColumnWidthPts(UOSInt index, Double width)
{
	if (index >= this->colCount)
		return;
	this->colWidthPts[index] = width;
}

void Text::ReportBuilder::SetColumnType(UOSInt index, ColType colType)
{
	if (index >= this->colCount)
		return;
	this->colTypes[index] = colType;
}

void Text::ReportBuilder::SetColURLLatLon(UOSInt index, Double lat, Double lon)
{
	ColURLLatLon *url;
	url = MemAlloc(ColURLLatLon, 1);
	url->col = index;
	url->row = this->tableContent->GetCount();
	url->lat = lat;
	url->lon = lon;
	this->urlList->Add(url);
}

Bool Text::ReportBuilder::HasChart()
{
	return this->chart != 0;
}

Text::SpreadSheet::Workbook *Text::ReportBuilder::CreateWorkbook()
{
	UOSInt i;
	UOSInt j;
	UInt32 k;
	UOSInt l;
	UOSInt m;
	UOSInt urlAdd;
	Text::SpreadSheet::Workbook *wb;
	Text::SpreadSheet::Worksheet *ws;
	Text::SpreadSheet::Worksheet *dataSheet;
	Text::String **csarr;
	Text::StringBuilderUTF8 sb;
	ColURLLatLon *url;
	Data::ArrayList<Text::ReportBuilder::ColIcon *> *iconList;
	Text::ReportBuilder::ColIcon *icon;
	RowType lastRowType;
	RowType currRowType;
	Text::SpreadSheet::CellStyle *styleSummary = 0;
	
	if (this->chart == 0)
	{
		NEW_CLASS(wb, Text::SpreadSheet::Workbook());
		wb->AddDefaultStyles();
		ws = wb->AddWorksheet(this->name);

		k = 0;
		i = 0;
		j = this->preheaders->GetCount();
		while (i < j)
		{
			csarr = this->preheaders->GetItem(i);
			ws->SetCellString(k, 0, csarr[0]);
			ws->SetCellString(k, 1, csarr[1]);
			k++;
			i++;
		}

		ws->SetCellString(k, 0, this->name);
		i = 0;
		j = this->colCount;
		while (i < j)
		{
			if (this->colWidthPts[i] != 0)
			{
				ws->SetColWidth(i, this->colWidthPts[i], Math::Unit::Distance::DU_POINT);
			}
			i++;
		}

		i = 0;
		k++;
		j = this->headers->GetCount();
		while (i < j)
		{
			csarr = this->headers->GetItem(i);
			ws->SetCellString(k, 0, csarr[0]);
			ws->SetCellString(k, 1, csarr[1]);
			k++;
			i++;
		}
		urlAdd = k;
		k++;
		lastRowType = RT_UNKNOWN;
		i = 0;
		j = this->tableContent->GetCount();
		while (i < j)
		{
			iconList = this->icons->GetItem(i);
			csarr = this->tableContent->GetItem(i);
			currRowType = this->tableRowType->GetItem(i);
			if ((lastRowType == RT_CONTENT && currRowType == RT_SUMMARY) || (lastRowType == RT_HEADER && currRowType != RT_HEADER))
			{
				if (styleSummary == 0)
				{
					Text::SpreadSheet::CellStyle::BorderStyle bs;
					bs.borderType = Text::SpreadSheet::BorderType::Medium;
					bs.borderColor = 0xff000000;
					styleSummary = wb->NewCellStyle();
					styleSummary->SetBorderTop(&bs);
				}
				l = 0;
				while (l < this->colCount)
				{
					ws->SetCellStyle(k, l, styleSummary);
					l++;
				}
			}
			if (iconList)
			{
				Text::StringBuilderUTF8 **sbList = MemAlloc(Text::StringBuilderUTF8*, this->colCount);
				l = 0;
				while (l < this->colCount)
				{
					NEW_CLASS(sbList[l], Text::StringBuilderUTF8());
					if (csarr[l])
					{
						sbList[l]->Append(csarr[l]);
					}
					l++;
				}
				m = iconList->GetCount();
				l = 0;
				while (l < m)
				{
					icon = iconList->GetItem(l);
					if (sbList[icon->col]->GetLength() > 0)
					{
						sbList[icon->col]->AppendC(UTF8STRC(", "));
					}
					sbList[icon->col]->Append(icon->name);
					l++;
				}

				l = 0;
				while (l < this->colCount)
				{
					if (sbList[l]->GetLength() > 0)
					{
						ws->SetCellString(k, l, sbList[l]->ToString());
					}
					DEL_CLASS(sbList[l]);
					l++;
				}
				MemFree(sbList);
			}
			else if (currRowType == RT_HEADER)
			{
				l = 0;
				while (l < this->colCount)
				{
					if (csarr[l])
					{
						ws->SetCellString(k, l, csarr[l]);
					}
					l++;
				}
			}
			else
			{
				l = 0;
				while (l < this->colCount)
				{
					if (csarr[l])
					{
						if (this->colTypes[l] == CT_DOUBLE)
						{
							ws->SetCellDouble(k, l, csarr[l]->ToDouble());
						}
						else if (this->colTypes[l] == CT_INT32)
						{
							ws->SetCellInt32(k, l, csarr[l]->ToInt32());
						}
						else
						{
							ws->SetCellString(k, l, csarr[l]);
						}
					}
					l++;
				}
			}
			lastRowType = currRowType;
			k++;
			i++;
		}
		i = 0;
		j = this->urlList->GetCount();
		while (i < j)
		{
			url = this->urlList->GetItem(i);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("https://www.google.com/maps/place/"));
			Text::SBAppendF64(&sb, url->lat);
			sb.AppendC(UTF8STRC(","));
			Text::SBAppendF64(&sb, url->lon);
			sb.AppendC(UTF8STRC("/@"));
			Text::SBAppendF64(&sb, url->lat);
			sb.AppendC(UTF8STRC(","));
			Text::SBAppendF64(&sb, url->lon);
			sb.AppendC(UTF8STRC(",19z"));
			ws->SetCellURL(url->row + urlAdd, url->col, sb.ToString());
			i++;
		}
		return wb;
	}
	else
	{
		NEW_CLASS(wb, Text::SpreadSheet::Workbook());
		Text::SpreadSheet::WorkbookFont *font10 = wb->NewFont(CSTR("Arial"), 10, false);
		Text::SpreadSheet::CellStyle *strStyle = wb->NewCellStyle(font10, Text::SpreadSheet::HAlignment::Left, Text::SpreadSheet::VAlignment::Center, CSTR("General"));
		ws = wb->AddWorksheet(this->name);
		dataSheet = wb->AddWorksheet(CSTR("ChartData"));

		k = 0;
		i = 0;
		j = this->preheaders->GetCount();
		while (i < j)
		{
			csarr = this->preheaders->GetItem(i);
			ws->SetCellString(k, 0, strStyle, csarr[0]);
			ws->SetCellString(k, 1, strStyle, csarr[1]);
			k++;
			i++;
		}

		ws->SetCellString(k, 0, this->name);
		i = 0;
		j = this->colCount;
		while (i < j)
		{
			if (this->colWidthPts[i] != 0)
			{
				ws->SetColWidth(i, this->colWidthPts[i], Math::Unit::Distance::DU_POINT);
			}
			i++;
		}

		i = 0;
		k++;
		j = this->headers->GetCount();
		while (i < j)
		{
			csarr = this->headers->GetItem(i);
			ws->SetCellString(k, 0, strStyle, csarr[0]);
			ws->SetCellString(k, 1, strStyle, csarr[1]);
			k++;
			i++;
		}

		Text::SpreadSheet::OfficeChart *shChart = ws->CreateChart(Math::Unit::Distance::DU_INCH, 0.64, 1.61, 13.10, 5.53, STR_CSTR(chart->GetTitle()));
		shChart->InitLineChart(chart->GetYAxisName()->ToCString(), chart->GetXAxisName()->ToCString(), FromChartDataType(chart->GetXAxisType()));
		shChart->SetDisplayBlankAs(Text::SpreadSheet::BlankAs::Gap);
		shChart->AddLegend(Text::SpreadSheet::LegendPos::Bottom);

		if (chart->GetXDataCount() == 1)
		{
			Text::SpreadSheet::CellStyle *dateStyle = 0;
			Text::SpreadSheet::CellStyle *intStyle = 0;
			Text::SpreadSheet::CellStyle *dblStyle = wb->NewCellStyle(font10, Text::SpreadSheet::HAlignment::Left, Text::SpreadSheet::VAlignment::Center, CSTR("General"));

			UOSInt i;
			UOSInt j;
			UOSInt colCount;
			switch (chart->GetXAxisType())
			{
			case Data::IChart::DataType::DateTicks:
			{
				if (dateStyle == 0)
				{
					if (chart->GetTimeFormat())
					{
						dateStyle = wb->NewCellStyle(font10, Text::SpreadSheet::HAlignment::Left, Text::SpreadSheet::VAlignment::Center, chart->GetTimeFormat()->ToCString());
					}
					else
					{
						dateStyle = wb->NewCellStyle(font10, Text::SpreadSheet::HAlignment::Left, Text::SpreadSheet::VAlignment::Center, CSTR("YYYY-MM-dd HH:mm"));
					}
				}
				Data::DateTime dt;
				dt.ToLocalTime();
				Int64 *dateTicks = chart->GetXDateTicks(0, &colCount);
				i = 0;
				while (i < colCount)
				{
					dt.SetTicks(dateTicks[i]);
					dataSheet->SetCellDate(0, i + 1, dateStyle, &dt);
					i++;
				}
				break;
			}
			case Data::IChart::DataType::DOUBLE:
			{
				if (dblStyle == 0)
				{
					if (chart->GetDblFormat())
					{
						dblStyle = wb->NewCellStyle(font10, Text::SpreadSheet::HAlignment::Left, Text::SpreadSheet::VAlignment::Center, chart->GetDblFormat()->ToCString());
					}
					else
					{
						dblStyle = wb->NewCellStyle(font10, Text::SpreadSheet::HAlignment::Left, Text::SpreadSheet::VAlignment::Center, CSTR("0.###"));
					}
				}
				Double *dblValues = chart->GetXDouble(0, &colCount);
				i = 0;
				while (i < colCount)
				{
					dataSheet->SetCellDouble(0, i + 1, dblStyle, dblValues[i]);
					i++;
				}
				break;
			}
			case Data::IChart::DataType::Integer:
			{
				if (intStyle == 0)
				{
					intStyle = wb->NewCellStyle(font10, Text::SpreadSheet::HAlignment::Left, Text::SpreadSheet::VAlignment::Center, CSTR("0"));
				}
				Int32 *intValues = chart->GetXInt32(0, &colCount);
				i = 0;
				while (i < colCount)
				{
					dataSheet->SetCellInt32(0, i + 1, intStyle, intValues[i]);
					i++;
				}
				break;
			}
			case Data::IChart::DataType::None:
				colCount = 0;
				break;
			}
			i = 0;
			j = chart->GetYDataCount();
			while (i < j)
			{
				dataSheet->SetCellString(1 + i, 0, strStyle, chart->GetYName(i));
				switch (chart->GetYType(i))
				{
				case Data::IChart::DataType::DateTicks:
				{
					if (dateStyle == 0)
					{
						if (chart->GetTimeFormat())
						{
							dateStyle = wb->NewCellStyle(font10, Text::SpreadSheet::HAlignment::Left, Text::SpreadSheet::VAlignment::Center, chart->GetTimeFormat()->ToCString());
						}
						else
						{
							dateStyle = wb->NewCellStyle(font10, Text::SpreadSheet::HAlignment::Left, Text::SpreadSheet::VAlignment::Center, CSTR("YYYY-MM-dd HH:mm"));
						}
					}
					Data::DateTime dt;
					dt.ToLocalTime();
					Int64 *dateTicks = chart->GetYDateTicks(i, &colCount);
					k = 0;
					while (k < colCount)
					{
						dt.SetTicks(dateTicks[k]);
						dataSheet->SetCellDate(i + 1, k + 1, dateStyle, &dt);
						k++;
					}
					break;
				}
				case Data::IChart::DataType::DOUBLE:
				{
					if (dblStyle == 0)
					{
						if (chart->GetDblFormat())
						{
							dblStyle = wb->NewCellStyle(font10, Text::SpreadSheet::HAlignment::Left, Text::SpreadSheet::VAlignment::Center, chart->GetDblFormat()->ToCString());
						}
						else
						{
							dblStyle = wb->NewCellStyle(font10, Text::SpreadSheet::HAlignment::Left, Text::SpreadSheet::VAlignment::Center, CSTR("0.###"));
						}
					}
					Double *dblValues = chart->GetYDouble(i, &colCount);
					k = 0;
					while (k < colCount)
					{
						dataSheet->SetCellDouble(i + 1, k + 1, dblStyle, dblValues[k]);
						k++;
					}
					break;
				}
				case Data::IChart::DataType::Integer:
				{
					if (intStyle == 0)
					{
						intStyle = wb->NewCellStyle(font10, Text::SpreadSheet::HAlignment::Left, Text::SpreadSheet::VAlignment::Center, CSTR("0"));
					}
					Int32 *intValues = chart->GetYInt32(i, &colCount);
					k = 0;
					while (k < colCount)
					{
						dataSheet->SetCellInt32(i + 1, k + 1, intStyle, intValues[k]);
						k++;
					}
					break;
				}
				case Data::IChart::DataType::None:
					colCount = 0;
					break;				
				}
				Text::SpreadSheet::WorkbookDataSource *catSource = NEW_CLASS_D(Text::SpreadSheet::WorkbookDataSource(dataSheet, 0, 0, 1, colCount));
				Text::SpreadSheet::WorkbookDataSource *valSource = NEW_CLASS_D(Text::SpreadSheet::WorkbookDataSource(dataSheet, i + 1, i + 1, 1, colCount));
				shChart->AddSeries(catSource, valSource, chart->GetYName(i), false);
				i++;
			}
		}
		else
		{

		}

		return wb;
	}
}

Media::VectorDocument *Text::ReportBuilder::CreateVDoc(Int32 id, Media::DrawEngine *deng)
{
	Media::VectorDocument *doc;
	Media::VectorGraph *g;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	UTF8Char u8buff[32];
	Media::PaperSize paperSize(Media::PaperSize::PT_A4);
	Double border = 10.0;
	Double fontHeightMM = 3.0;
	Double fontHeightPt = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_POINT, fontHeightMM);
	Media::DrawFont *f;
	Media::DrawBrush *b;
	Media::DrawPen *p;
	Double headerW1;
	Double headerW2;
	Double sz[2];
	Double currY;
	Text::String **strs;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	Double *colMinWidth;
	Double *colTotalWidth;
	Double *colPos;
	Double *colSize;
	Double *colCurrX;
	Int32 pageId = 0;
	RowType lastRowType;
	RowType currRowType;

	colMinWidth = MemAlloc(Double, this->colCount);
	colTotalWidth = MemAlloc(Double, this->colCount);
	colPos = MemAlloc(Double, this->colCount);
	colSize = MemAlloc(Double, this->colCount);
	colCurrX = MemAlloc(Double, this->colCount);

	Data::ArrayList<Text::ReportBuilder::ColIcon *> *iconList;
	Data::FastStringMap<IconStatus *> iconStatus;
	IconStatus *iconSt;
	Text::ReportBuilder::ColIcon *icon;

	lastRowType = RT_UNKNOWN;
	Text::StrInt32(u8buff, id);
	NEW_CLASS(doc, Media::VectorDocument(0, u8buff, deng));
	if (this->paperHori)
	{
		g = doc->AddGraph(paperSize.GetHeightMM(), paperSize.GetWidthMM(), Math::Unit::Distance::DU_MILLIMETER);
	}
	else
	{
		g = doc->AddGraph(paperSize.GetWidthMM(), paperSize.GetHeightMM(), Math::Unit::Distance::DU_MILLIMETER);
	}
	f = g->NewFontPt(this->fontName->ToCString(), fontHeightPt, Media::DrawEngine::DFS_NORMAL, 0);
	headerW1 = 0;
	headerW2 = 0;
	i = this->headers->GetCount();
	while (i-- > 0)
	{
		strs = this->headers->GetItem(i);
		g->GetTextSize(f, strs[0]->ToCString(), sz);
		if (sz[0] > headerW1)
			headerW1 = sz[0];
		g->GetTextSize(f, strs[1]->ToCString(), sz);
		if (sz[0] > headerW2)
			headerW2 = sz[0];
	}
	i = this->preheaders->GetCount();
	while (i-- > 0)
	{
		strs = this->preheaders->GetItem(i);
		g->GetTextSize(f, strs[0]->ToCString(), sz);
		if (sz[0] > headerW1)
			headerW1 = sz[0];
		g->GetTextSize(f, strs[1]->ToCString(), sz);
		if (sz[0] > headerW2)
			headerW2 = sz[0];
	}

	i = this->colCount;
	while (i-- > 0)
	{
		colMinWidth[i] = 0;
		colTotalWidth[i] = 0;
		colPos[i] = 0;
		colSize[i] = 0;
	}

	i = this->tableContent->GetCount();
	while (i-- > 0)
	{
		strs = this->tableContent->GetItem(i);
		iconList = this->icons->GetItem(i);
		j = this->colCount;
		while (j-- > 0)
		{
			colCurrX[j] = 0;
			if (strs[j])
			{
				g->GetTextSize(f, strs[j]->ToCString(), sz);
				colCurrX[j] = sz[0];
			}
		}

		if (iconList)
		{
			j = iconList->GetCount();
			while (j-- > 0)
			{
				icon = iconList->GetItem(j);
				if (icon->fileName)
				{
					iconSt = iconStatus.Get(icon->fileName);
					if (iconSt == 0)
					{
						iconSt = MemAlloc(IconStatus, 1);
						iconSt->dimg = deng->LoadImage(icon->fileName->ToCString());
						iconStatus.Put(icon->fileName, iconSt);
					}

					if (iconSt->dimg)
					{
						colCurrX[icon->col] += fontHeightPt / UOSInt2Double(iconSt->dimg->GetHeight()) * UOSInt2Double(iconSt->dimg->GetWidth());
					}
				}
			}
		}

		j = this->colCount;
		while (j-- > 0)
		{
			if (colTotalWidth[j] < colCurrX[j])
				colTotalWidth[j] = colCurrX[j];
		}
	}

	Double endY;
	Double totalColWidth;
	Double drawWidth;
	totalColWidth = 0;
	if (this->paperHori)
	{
		endY = paperSize.GetWidthMM() - border - fontHeightMM;
		drawWidth = paperSize.GetHeightMM() - border * 2.0;
	}
	else
	{
		endY = paperSize.GetHeightMM() - border - fontHeightMM;
		drawWidth = paperSize.GetWidthMM() - border * 2.0;
	}

	i = this->colCount;
	while (i-- > 0)
	{
		totalColWidth += colTotalWidth[i];
	}
	i = this->colCount;
	while (i-- > 0)
	{
		///////////////////////////
		colSize[i] = colTotalWidth[i] * drawWidth / totalColWidth;
	}

	colPos[0] = border;
	i = 1;
	j = this->colCount;
	while (i < j)
	{
		colPos[i] = colPos[i - 1] + colSize[i - 1];
		i++;
	}
	g->DelFont(f);

	k = 1;
	l = this->tableContent->GetCount();
	while (true)
	{
		f = g->NewFontPt(this->fontName->ToCString(), fontHeightPt, Media::DrawEngine::DFS_NORMAL, 0);
		b = g->NewBrushARGB(0xff000000);
		p = g->NewPenARGB(0xff000000, 0.2, 0, 0);

		currY = border;
		i = 0;
		j = this->preheaders->GetCount();
		while (i < j)
		{
			strs = this->preheaders->GetItem(i);
			g->DrawString(border, currY, strs[0], f, b);
			g->DrawString(border + headerW1 + 0.5, currY, strs[1], f, b);

			currY += fontHeightMM * 1.5;
			i++;
		}

		g->DrawString(border, currY, this->name, f, b);
		currY += fontHeightMM * 2;
		i = 0;
		j = this->headers->GetCount();
		while (i < j)
		{
			strs = this->headers->GetItem(i);
			g->DrawString(border, currY, strs[0], f, b);
			g->DrawString(border + headerW1 + 0.5, currY, strs[1], f, b);

			currY += fontHeightMM * 1.5;
			i++;
		}
		currY += fontHeightMM;

		if (l == 1 && this->chart)
		{

		}
		else
		{
			strs = this->tableContent->GetItem(0);
			i = 0;
			j = this->colCount;
			while (i < j)
			{
				g->DrawString(colPos[i], currY, strs[i], f, b);
				i++;
			}
			currY += fontHeightMM * 1.5 + 0.2;
			g->DrawLine(border, currY, border + drawWidth, currY, p);
			if (currY > endY)
			{
				k = l;
			}
			while (k < l)
			{
				strs = this->tableContent->GetItem(k);
				currRowType = this->tableRowType->GetItem(k);
				if (lastRowType == RT_CONTENT && currRowType == RT_SUMMARY)
				{
					g->DrawLine(border, currY, border + drawWidth, currY, p);
				}

				i = 0;
				j = this->colCount;
				while (i < j)
				{
					if (strs[i])
					{
						g->DrawString(colPos[i], currY, strs[i], f, b);
					}
					i++;
				}

				iconList = this->icons->GetItem(k);
				if (iconList)
				{
					i = 0;
					j = this->colCount;
					while (i < j)
					{
						if (strs[i])
						{
							g->GetTextSize(f, strs[i]->ToCString(), sz);
							colCurrX[i] = colPos[i] + sz[0];
						}
						else
						{
							colCurrX[i] = colPos[i];
						}
						i++;
					}

					i = 0;
					j = iconList->GetCount();
					while (i < j)
					{
						icon = iconList->GetItem(i);
						if (icon->fileName)
						{
							iconSt = iconStatus.Get(icon->fileName);
							if (iconSt && iconSt->dimg)
							{
								Double w = fontHeightMM * UOSInt2Double(iconSt->dimg->GetWidth()) / UOSInt2Double(iconSt->dimg->GetHeight());
								Double dpi = UOSInt2Double(iconSt->dimg->GetHeight()) / Math::Unit::Distance::Convert(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_INCH, fontHeightMM);
								iconSt->dimg->SetHDPI(dpi);
								iconSt->dimg->SetVDPI(dpi);
								g->DrawImagePt(iconSt->dimg, colCurrX[icon->col], currY);
								colCurrX[icon->col] += w;
							}
						}
						i++;
					}
				}

				currY += fontHeightMM * 1.5;
				lastRowType = currRowType;
				k++;

				if (currY >= endY)
				{
					break;
				}
			}
			pageId++;
			sptr = Text::StrInt32(sbuff, pageId);
			g->GetTextSize(f, CSTRP(sbuff, sptr), sz);
			g->DrawString(border + (drawWidth - sz[0]) * 0.5, paperSize.GetHeightMM() - border, CSTRP(sbuff, sptr), f, b);
		}

		g->DelFont(f);
		g->DelBrush(b);
		g->DelPen(p);

		Double paperWidthMM;
		Double paperHeightMM;
		if (this->paperHori)
		{
			paperWidthMM = paperSize.GetHeightMM();
			paperHeightMM = paperSize.GetWidthMM();
		}
		else
		{
			paperWidthMM = paperSize.GetWidthMM();
			paperHeightMM = paperSize.GetHeightMM();
		}
		if (k >= l)
		{
			if (this->chart)
			{
				this->chart->Plot(g, border, currY, drawWidth, paperHeightMM - border - currY);
			}
			break;
		}
		g = doc->AddGraph(paperWidthMM, paperHeightMM, Math::Unit::Distance::DU_MILLIMETER);
		lastRowType = RT_UNKNOWN;
	}
	MemFree(colMinWidth);
	MemFree(colTotalWidth);
	MemFree(colPos);
	MemFree(colSize);
	MemFree(colCurrX);

	i = iconStatus.GetCount();
	while (i-- > 0)
	{
		iconSt = iconStatus.GetItem(i);
		if (iconSt->dimg)
		{
			deng->DeleteImage(iconSt->dimg);
		}
		MemFree(iconSt);
	}
	return doc;
}
