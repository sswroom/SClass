#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/StringUTF8Map.h"
#include "Media/PaperSize.h"
#include "Text/MyStringFloat.h"
#include "Text/ReportBuilder.h"
#include "Text/StringBuilderUTF8.h"

Text::ReportBuilder::ReportBuilder(const UTF8Char *name, OSInt colCount, const UTF8Char **columns)
{
	const UTF8Char **cols;
	OSInt i;
	this->name = Text::StrCopyNew(name);
	this->colCount = colCount;
	this->colWidth = MemAlloc(Double, this->colCount);
	this->chart = 0;
	this->paperHori = false;
	this->colTypes = MemAlloc(ColType, this->colCount);
	cols = MemAlloc(const UTF8Char *, this->colCount);
	i = 0;
	while (i < this->colCount)
	{
		colWidth[i] = 0;
		if (columns[i])
		{
			cols[i] = Text::StrCopyNew(columns[i]);
		}
		else
		{
			cols[i] = 0;
		}
		this->colTypes[i] = CT_STRING;
		i++;
	}
	NEW_CLASS(this->preheaders, Data::ArrayList<const UTF8Char **>());
	NEW_CLASS(this->headers, Data::ArrayList<const UTF8Char **>());
	NEW_CLASS(this->tableContent, Data::ArrayList<const UTF8Char **>());
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
	const UTF8Char **cols;
	Data::ArrayList<Text::ReportBuilder::ColIcon*> *iconList;
	OSInt i;
	OSInt j;
	j = this->headers->GetCount();
	while (j-- > 0)
	{
		cols = this->headers->GetItem(j);
		Text::StrDelNew(cols[0]);
		Text::StrDelNew(cols[1]);
		MemFree(cols);
	}
	j = this->preheaders->GetCount();
	while (j-- > 0)
	{
		cols = this->preheaders->GetItem(j);
		Text::StrDelNew(cols[0]);
		Text::StrDelNew(cols[1]);
		MemFree(cols);
	}
	j = this->tableContent->GetCount();
	while (j-- > 0)
	{
		cols = this->tableContent->GetItem(j);
		i = this->colCount;
		while (i-- > 0)
		{
			SDEL_TEXT(cols[i]);
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
				SDEL_TEXT(icon->fileName);
				SDEL_TEXT(icon->name);
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
	MemFree(this->colWidth);
	MemFree(this->colTypes);
	Text::StrDelNew(this->name);
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

void Text::ReportBuilder::AddPreHeader(const UTF8Char *name, const UTF8Char *val)
{
	const UTF8Char **cols;
	cols = MemAlloc(const UTF8Char *, 2);
	cols[0] = Text::StrCopyNew(name);
	cols[1] = Text::StrCopyNew(val);
	this->preheaders->Add(cols);
}

void Text::ReportBuilder::AddHeader(const UTF8Char *name, const UTF8Char *val)
{
	const UTF8Char **cols;
	cols = MemAlloc(const UTF8Char *, 2);
	cols[0] = Text::StrCopyNew(name);
	cols[1] = Text::StrCopyNew(val);
	this->headers->Add(cols);
}

void Text::ReportBuilder::AddTableContent(const UTF8Char **content)
{
	const UTF8Char **cols;
	OSInt i;
	cols = MemAlloc(const UTF8Char *, this->colCount);
	i = 0;
	while (i < this->colCount)
	{
		if (content[i])
		{
			cols[i] = Text::StrCopyNew(content[i]);
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
	const UTF8Char **cols;
	OSInt i;
	cols = MemAlloc(const UTF8Char *, this->colCount);
	i = 0;
	while (i < this->colCount)
	{
		if (content[i])
		{
			cols[i] = Text::StrCopyNew(content[i]);
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

void Text::ReportBuilder::AddIcon(OSInt index, const UTF8Char *fileName, const UTF8Char *name)
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
	if (fileName)
	{
		icon->fileName = Text::StrCopyNew(fileName);
	}
	else
	{
		icon->fileName = 0;
	}
	if (name)
	{
		icon->name = Text::StrCopyNew(name);
	}
	else
	{
		icon->name = 0;
	}
	iconList->Add(icon);
}

void Text::ReportBuilder::SetColumnWidth(OSInt index, Double width)
{
	if (index < 0 || index >= this->colCount)
		return;
	this->colWidth[index] = width;
}

void Text::ReportBuilder::SetColumnType(OSInt index, ColType colType)
{
	if (index < 0 || index >= this->colCount)
		return;
	this->colTypes[index] = colType;
}

void Text::ReportBuilder::SetColURLLatLon(OSInt index, Double lat, Double lon)
{
	ColURLLatLon *url;
	url = MemAlloc(ColURLLatLon, 1);
	url->col = index;
	url->row = this->tableContent->GetCount();
	url->lat = lat;
	url->lon = lon;
	this->urlList->Add(url);
}

Text::SpreadSheet::Workbook *Text::ReportBuilder::CreateWorkbook()
{
	OSInt i;
	OSInt j;
	UInt32 k;
	OSInt l;
	OSInt m;
	OSInt urlAdd;
	Text::SpreadSheet::Workbook *wb;
	Text::SpreadSheet::Worksheet *ws;
	const UTF8Char **csarr;
	Text::StringBuilderUTF8 sb;
	ColURLLatLon *url;
	Data::ArrayList<Text::ReportBuilder::ColIcon *> *iconList;
	Text::ReportBuilder::ColIcon *icon;
	RowType lastRowType;
	RowType currRowType;
	Text::SpreadSheet::CellStyle *styleSummary = 0;
	
	if (this->chart != 0)
		return 0;

	NEW_CLASS(wb, Text::SpreadSheet::Workbook());
	wb->AddDefaultStyles();
	ws = wb->AddWorksheet(this->name);

	k = 1;
	i = 0;
	j = this->preheaders->GetCount();
	while (i < j)
	{
		csarr = this->preheaders->GetItem(i);
		ws->SetCellString(k, 1, csarr[0]);
		ws->SetCellString(k, 2, csarr[1]);
		k++;
		i++;
	}

	ws->SetCellString(k, 1, this->name);
	i = 0;
	j = this->colCount;
	while (i < j)
	{
		if (this->colWidth[i] != 0)
		{
			ws->SetColWidth(i + 1, this->colWidth[i]);
		}
		i++;
	}

	i = 0;
	k++;
	j = this->headers->GetCount();
	while (i < j)
	{
		csarr = this->headers->GetItem(i);
		ws->SetCellString(k, 1, csarr[0]);
		ws->SetCellString(k, 2, csarr[1]);
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
				bs.borderType = Text::SpreadSheet::CellStyle::BT_MEDIUM;
				bs.borderColor = 0xff000000;
				styleSummary = wb->NewCellStyle();
				styleSummary->SetBorderTop(&bs);
			}
			l = 1;
			while (l <= this->colCount)
			{
				ws->SetCellStyle(k, (UInt32)l, styleSummary);
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
					sbList[icon->col]->Append((const UTF8Char*)", ");
				}
				sbList[icon->col]->Append(icon->name);
				l++;
			}

			l = 0;
			while (l < this->colCount)
			{
				if (sbList[l]->GetLength() > 0)
				{
					ws->SetCellString(k, (UInt32)(l + 1), sbList[l]->ToString());
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
					ws->SetCellString(k, (UInt32)(l + 1), csarr[l]);
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
						ws->SetCellDouble(k, (UInt32)(l + 1), Text::StrToDouble(csarr[l]));
					}
					else if (this->colTypes[l] == CT_INT32)
					{
						ws->SetCellInt32(k, (UInt32)(l + 1), Text::StrToInt32(csarr[l]));
					}
					else
					{
						ws->SetCellString(k, (UInt32)(l + 1), csarr[l]);
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
		sb.Append((const UTF8Char*)"https://www.google.com/maps/place/");
		Text::SBAppendF64(&sb, url->lat);
		sb.Append((const UTF8Char*)",");
		Text::SBAppendF64(&sb, url->lon);
		sb.Append((const UTF8Char*)"/@");
		Text::SBAppendF64(&sb, url->lat);
		sb.Append((const UTF8Char*)",");
		Text::SBAppendF64(&sb, url->lon);
		sb.Append((const UTF8Char*)",19z");
		ws->SetCellURL((UInt32)(url->row + urlAdd), (UInt32)(url->col + 1), sb.ToString());
		i++;
	}
	return wb;
}

Media::VectorDocument *Text::ReportBuilder::CreateVDoc(Int32 id, Media::DrawEngine *deng)
{
	Media::VectorDocument *doc;
	Media::VectorGraph *g;
	UTF8Char sbuff[32];
	UTF8Char u8buff[32];
	Media::PaperSize paperSize(Media::PaperSize::PT_A4);
	Double border = 10.0;
	Double fontHeight = 3.0;
	Media::DrawFont *f;
	Media::DrawBrush *b;
	Media::DrawPen *p;
	Double headerW1;
	Double headerW2;
	Double sz[2];
	Double currY;
	const UTF8Char **strs;
	OSInt i;
	OSInt j;
	OSInt k;
	OSInt l;
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
	Data::StringUTF8Map<IconStatus *> iconStatus;
	Data::ArrayList<IconStatus *> *iconStList;
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
	f = g->NewFontH((const UTF8Char*)"Arial", fontHeight, Media::DrawEngine::DFS_NORMAL, 0);
	headerW1 = 0;
	headerW2 = 0;
	i = this->headers->GetCount();
	while (i-- > 0)
	{
		strs = this->headers->GetItem(i);
		g->GetTextSize(f, strs[0], -1, sz);
		if (sz[0] > headerW1)
			headerW1 = sz[0];
		g->GetTextSize(f, strs[1], -1, sz);
		if (sz[0] > headerW2)
			headerW2 = sz[0];
	}
	i = this->preheaders->GetCount();
	while (i-- > 0)
	{
		strs = this->preheaders->GetItem(i);
		g->GetTextSize(f, strs[0], -1, sz);
		if (sz[0] > headerW1)
			headerW1 = sz[0];
		g->GetTextSize(f, strs[1], -1, sz);
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
				g->GetTextSize(f, strs[j], -1, sz);
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
						iconSt->dimg = deng->LoadImage(icon->fileName);
						iconStatus.Put(icon->fileName, iconSt);
					}

					if (iconSt->dimg)
					{
						colCurrX[icon->col] += fontHeight / iconSt->dimg->GetHeight() * iconSt->dimg->GetWidth();
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
		endY = paperSize.GetWidthMM() - border - fontHeight;
		drawWidth = paperSize.GetHeightMM() - border * 2.0;
	}
	else
	{
		endY = paperSize.GetHeightMM() - border - fontHeight;
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
		f = g->NewFontH((const UTF8Char*)"Arial", fontHeight, Media::DrawEngine::DFS_NORMAL, 0);
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

			currY += fontHeight * 1.5;
			i++;
		}

		g->DrawString(border, currY, this->name, f, b);
		currY += fontHeight * 2;
		i = 0;
		j = this->headers->GetCount();
		while (i < j)
		{
			strs = this->headers->GetItem(i);
			g->DrawString(border, currY, strs[0], f, b);
			g->DrawString(border + headerW1 + 0.5, currY, strs[1], f, b);

			currY += fontHeight * 1.5;
			i++;
		}
		currY += fontHeight;

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
			currY += fontHeight * 1.5 + 0.2;
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
							g->GetTextSize(f, strs[i], -1, sz);
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
								Double w = fontHeight * iconSt->dimg->GetWidth() / iconSt->dimg->GetHeight();
								Double dpi = iconSt->dimg->GetHeight() / Math::Unit::Distance::Convert(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_INCH, fontHeight);
								iconSt->dimg->SetHDPI(dpi);
								iconSt->dimg->SetVDPI(dpi);
								g->DrawImagePt(iconSt->dimg, colCurrX[icon->col], currY);
								colCurrX[icon->col] += w;
							}
						}
						i++;
					}
				}

				currY += fontHeight * 1.5;
				lastRowType = currRowType;
				k++;

				if (currY >= endY)
				{
					break;
				}
			}
			pageId++;
			Text::StrInt32(sbuff, pageId);
			g->GetTextSize(f, sbuff, -1, sz);
			g->DrawString(border + (drawWidth - sz[0]) * 0.5, paperSize.GetHeightMM() - border, sbuff, f, b);
		}

		g->DelFont(f);
		g->DelBrush(b);
		g->DelPen(p);

		if (k >= l)
		{
			if (this->chart)
			{
				this->chart->Plot(g, border, currY, drawWidth, paperSize.GetHeightMM() - border - currY);
			}
			break;
		}
		if (this->paperHori)
		{
			g = doc->AddGraph(paperSize.GetHeightMM(), paperSize.GetWidthMM(), Math::Unit::Distance::DU_MILLIMETER);
		}
		else
		{
			g = doc->AddGraph(paperSize.GetWidthMM(), paperSize.GetHeightMM(), Math::Unit::Distance::DU_MILLIMETER);
		}
		lastRowType = RT_UNKNOWN;
	}
	MemFree(colMinWidth);
	MemFree(colTotalWidth);
	MemFree(colPos);
	MemFree(colSize);
	MemFree(colCurrX);

	iconStList = iconStatus.GetValues();
	i = iconStList->GetCount();
	while (i-- > 0)
	{
		iconSt = iconStList->GetItem(i);
		if (iconSt->dimg)
		{
			deng->DeleteImage(iconSt->dimg);
		}
		MemFree(iconSt);
	}
	return doc;
}
