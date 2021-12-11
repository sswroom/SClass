#include "Stdafx.h"
#include "MyMemory.h"
#include "Exporter/XLSXExporter.h"
#include "IO/BuildTime.h"
#include "IO/ZIPBuilder.h"
#include "Math/Math.h"
#include "Text/CharUtil.h"
#include "Text/Locale.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XLSUtil.h"
#include "Text/XML.h"
#include "Text/SpreadSheet/Workbook.h"

using namespace Text::SpreadSheet;

Exporter::XLSXExporter::XLSXExporter()
{
}

Exporter::XLSXExporter::~XLSXExporter()
{
}

Int32 Exporter::XLSXExporter::GetName()
{
	return *(Int32*)"XLSX";
}

IO::FileExporter::SupportType Exporter::XLSXExporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	if (pobj->GetParserType() != IO::ParserType::Workbook)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	return IO::FileExporter::SupportType::NormalStream;
}

Bool Exporter::XLSXExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcat(nameBuff, (const UTF8Char*)"Excel XLSX File");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.xlsx");
		return true;
	}
	return false;
}

Bool Exporter::XLSXExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
{
	if (pobj->GetParserType() != IO::ParserType::Workbook)
	{
		return false;
	}
	Text::SpreadSheet::Workbook *workbook = (Text::SpreadSheet::Workbook*)pobj;
	Text::SpreadSheet::Worksheet *sheet;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sbContTypes;
	UTF8Char sbuff[256];
	Data::DateTime dt;
	Data::DateTime dt2;
	Data::DateTime *t;
	Text::String *s;
	Text::String *s2;
	const UTF8Char *csptr;
	const UTF8Char *csptr2;
	IO::ZIPBuilder *zip;
	UOSInt i;
	UOSInt j = workbook->GetCount();
	UOSInt k;
	UOSInt l;
	UOSInt m;
	UOSInt n;
	UOSInt drawingCnt = 0;
	UOSInt chartCnt = 0;
	Data::ArrayList<Text::String*> sharedStrings;
	Data::StringUTF8Map<UOSInt> stringMap;
	dt.SetCurrTimeUTC();
	NEW_CLASS(zip, IO::ZIPBuilder(stm));

	sbContTypes.Append((const UTF8Char*)"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	sbContTypes.Append((const UTF8Char*)"<Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\">");
	sbContTypes.Append((const UTF8Char*)"<Default Extension=\"xml\" ContentType=\"application/xml\"/>");
	sbContTypes.Append((const UTF8Char*)"<Default Extension=\"rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>");
	sbContTypes.Append((const UTF8Char*)"<Default Extension=\"png\" ContentType=\"image/png\"/>");
	sbContTypes.Append((const UTF8Char*)"<Default Extension=\"jpeg\" ContentType=\"image/jpeg\"/>");

	i = 0;
	j = workbook->GetCount();
	while (i < j)
	{
		sheet = workbook->GetItem(i);

		Data::ArrayList<LinkInfo*> links;
		LinkInfo *link;

		sb.ClearStr();
		sb.Append((const UTF8Char*)"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n");
		sb.Append((const UTF8Char*)"<worksheet xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\">");
		sb.Append((const UTF8Char*)"<sheetPr filterMode=\"false\">");
		sb.Append((const UTF8Char*)"<pageSetUpPr fitToPage=\"false\"/>");
		sb.Append((const UTF8Char*)"</sheetPr>");
		sb.Append((const UTF8Char*)"<dimension ref=\"A1\"/>");
		sb.Append((const UTF8Char*)"<sheetViews>");
		sb.Append((const UTF8Char*)"<sheetView showFormulas=\"false\" showGridLines=\"true\" showRowColHeaders=\"true\" showZeros=\"true\" rightToLeft=\"false\" tabSelected=\"true\" showOutlineSymbols=\"true\" defaultGridColor=\"true\" view=\"normal\" topLeftCell=\"A1\" colorId=\"64\" zoomScale=\"");
		sb.AppendU32(sheet->GetZoom());
		sb.Append((const UTF8Char*)"\" zoomScaleNormal=\"");
		sb.AppendU32(sheet->GetZoom());
		sb.Append((const UTF8Char*)"\" zoomScalePageLayoutView=\"");
		sb.AppendU32(sheet->GetZoom());
		sb.Append((const UTF8Char*)"\" workbookViewId=\"0\">");
		sb.Append((const UTF8Char*)"<selection pane=\"topLeft\" activeCell=\"A1\" activeCellId=\"0\" sqref=\"A1\"/>");
		sb.Append((const UTF8Char*)"</sheetView>");
		sb.Append((const UTF8Char*)"</sheetViews>");
		sb.Append((const UTF8Char*)"<sheetFormatPr defaultColWidth=\"");
		Text::SBAppendF64(&sb, sheet->GetDefColWidthPt() / 5.25);
		sb.Append((const UTF8Char*)"\" defaultRowHeight=\"");
		Text::SBAppendF64(&sb, sheet->GetDefRowHeightPt());
		sb.Append((const UTF8Char*)"\" zeroHeight=\"false\" outlineLevelRow=\"0\" outlineLevelCol=\"0\"></sheetFormatPr>");
		sb.Append((const UTF8Char*)"<cols>");

		Double lastColWidth = -1;
		UOSInt lastColIndex = INVALID_INDEX;

		k = 0;
		l = sheet->GetColWidthCount();
		while (k < l)
		{
			if (sheet->GetColWidthPt(k) != lastColWidth)
			{
				if (lastColIndex != INVALID_INDEX)
				{
					sb.Append((const UTF8Char*)"<col min=\"");
					sb.AppendUOSInt(lastColIndex + 1);
					sb.Append((const UTF8Char*)"\" max=\"");
					sb.AppendUOSInt(k);
					sb.Append((const UTF8Char*)"\" width=\"");
					if (lastColWidth >= 0)
					{
						Text::SBAppendF64(&sb, lastColWidth / 5.25);
						sb.Append((const UTF8Char*)"\" customWidth=\"1\"");
					}
					else
					{
						Text::SBAppendF64(&sb, sheet->GetDefColWidthPt() / 5.25);
						sb.Append((const UTF8Char*)"\" customWidth=\"false\"");
					}
					sb.Append((const UTF8Char*)"/>");
				}
				lastColWidth = sheet->GetColWidthPt(k);
				lastColIndex = k;
			}
			k++;
		}
		if (lastColWidth >= 0)
		{
			sb.Append((const UTF8Char*)"<col min=\"");
			sb.AppendUOSInt(lastColIndex + 1);
			sb.Append((const UTF8Char*)"\" max=\"");
			sb.AppendUOSInt(l);
			sb.Append((const UTF8Char*)"\" width=\"");
			if (lastColWidth >= 0)
			{
				Text::SBAppendF64(&sb, lastColWidth / 5.25);
				sb.Append((const UTF8Char*)"\" customWidth=\"1\"");
			}
			else
			{
				Text::SBAppendF64(&sb, sheet->GetDefColWidthPt() / 5.25);
				sb.Append((const UTF8Char*)"\" customWidth=\"false\"");
			}
			sb.Append((const UTF8Char*)"/>");
		}
		if (l < sheet->GetMaxCol())
		{
			sb.Append((const UTF8Char*)"<col min=\"");
			sb.AppendUOSInt(l + 1);
			sb.Append((const UTF8Char*)"\" max=\"");
			sb.AppendUOSInt(sheet->GetMaxCol());
			sb.Append((const UTF8Char*)"\" width=\"");
			Text::SBAppendF64(&sb, sheet->GetDefColWidthPt() / 5.25);
			sb.Append((const UTF8Char*)"\" customWidth=\"false\" collapsed=\"false\" hidden=\"false\" outlineLevel=\"0\" style=\"0\"/>");
		}

		sb.Append((const UTF8Char*)"</cols>");
		k = 0;
		l = sheet->GetCount();
		if (l > 0)
		{
			sb.Append((const UTF8Char*)"<sheetData>");
			while (k < l)
			{
				Text::SpreadSheet::Worksheet::RowData *row = sheet->GetItem(k);
				if (row)
				{
					sb.Append((const UTF8Char*)"<row r=\"");
					sb.AppendUOSInt(k + 1);
					sb.Append((const UTF8Char*)"\" customFormat=\"false\" ht=\"12.8\" hidden=\"false\" customHeight=\"false\" outlineLevel=\"0\" collapsed=\"false\">");

					m = 0;
					n = row->cells->GetCount();
					while (m < n)
					{
						Text::SpreadSheet::Worksheet::CellData *cell = row->cells->GetItem(m);
						if (cell && cell->cellValue && cell->cdt != Text::SpreadSheet::CellDataType::MergedLeft && cell->cdt != Text::SpreadSheet::CellDataType::MergedTop)
						{
							sb.Append((const UTF8Char*)"<c r=\"");
							Text::StrUOSInt(Text::SpreadSheet::Workbook::ColCode(sbuff, m), k + 1);
							sb.Append(sbuff);
							sb.AppendChar('"', 1);
							if (cell->style)
							{
								sb.Append((const UTF8Char*)" s=\"");
								sb.AppendUOSInt(cell->style->GetIndex());
								sb.AppendChar('"', 1);
							}
							switch (cell->cdt)
							{
							case Text::SpreadSheet::CellDataType::String:
								sb.Append((const UTF8Char*)" t=\"s\"");
								break;
							case Text::SpreadSheet::CellDataType::Number:
							case Text::SpreadSheet::CellDataType::DateTime:
								sb.Append((const UTF8Char*)" t=\"n\"");
								break;
							case Text::SpreadSheet::CellDataType::MergedLeft:
							case Text::SpreadSheet::CellDataType::MergedTop:
								break;
							}
							sb.Append((const UTF8Char*)"><v>");
							switch (cell->cdt)
							{
							case Text::SpreadSheet::CellDataType::String:
								{
									UOSInt sIndex = stringMap.Get(cell->cellValue->v);
									if (sIndex == 0 && !stringMap.ContainsKey(cell->cellValue->v))
									{
										sIndex = sharedStrings.Add(cell->cellValue);
										stringMap.Put(cell->cellValue->v, sIndex);
									}
									sb.AppendUOSInt(sIndex);
								}
								break;
							case Text::SpreadSheet::CellDataType::Number:
								sb.AppendC(cell->cellValue->v, cell->cellValue->leng);
								break;
							case Text::SpreadSheet::CellDataType::DateTime:
								{
									Data::DateTime dt;
									dt.ToLocalTime();
									dt.SetValue(cell->cellValue->v);
									Text::SBAppendF64(&sb, Text::XLSUtil::Date2Number(&dt));
								}
								break;
							case Text::SpreadSheet::CellDataType::MergedLeft:
							case Text::SpreadSheet::CellDataType::MergedTop:
								break;
							}
							
							if (cell->cellURL)
							{
								link = MemAlloc(LinkInfo, 1);
								link->row = k;
								link->col = m;
								link->cell = cell;
								links.Add(link);
							}
							
							sb.Append((const UTF8Char*)"</v></c>");
						}
						m++;
					}
					sb.Append((const UTF8Char*)"</row>");
				}

				k++;
			}
			sb.Append((const UTF8Char*)"</sheetData>");

			if (links.GetCount() > 0)
			{
				UOSInt idBase = sheet->GetDrawingCount() + 1;
				sb.Append((const UTF8Char*)"<hyperlinks>");
				m = 0;
				n = links.GetCount();
				while (m < n)
				{
					link = links.GetItem(m);
					sb.Append((const UTF8Char*)"<hyperlink ref=\"");
					Text::SpreadSheet::Workbook::ColCode(sbuff, link->col);
					sb.Append(sbuff);
					sb.AppendUOSInt(link->row + 1);
					sb.Append((const UTF8Char*)"\" r:id=\"rId");
					sb.AppendUOSInt(idBase + m);
					sb.Append((const UTF8Char*)"\" display=");
					s = Text::XML::ToNewAttrText(link->cell->cellValue->v);
					sb.Append(s);
					s->Release();
					sb.Append((const UTF8Char*)"/>");
					m++;
				}
				sb.Append((const UTF8Char*)"</hyperlinks>");
			}
		}
		else
		{
			sb.Append((const UTF8Char*)"<sheetData/>");
		}
		//<sheetProtection sheet="true" password="cc1a" objects="true" scenarios="true"/><printOptions headings="false" gridLines="false" gridLinesSet="true" horizontalCentered="false" verticalCentered="false"/>
		sb.Append((const UTF8Char*)"<pageMargins left=\"");
		Text::SBAppendF64(&sb, sheet->GetMarginLeft());
		sb.Append((const UTF8Char*)"\" right=\"");
		Text::SBAppendF64(&sb, sheet->GetMarginRight());
		sb.Append((const UTF8Char*)"\" top=\"");
		Text::SBAppendF64(&sb, sheet->GetMarginTop());
		sb.Append((const UTF8Char*)"\" bottom=\"");
		Text::SBAppendF64(&sb, sheet->GetMarginBottom());
		sb.Append((const UTF8Char*)"\" header=\"");
		Text::SBAppendF64(&sb, sheet->GetMarginHeader());
		sb.Append((const UTF8Char*)"\" footer=\"");
		Text::SBAppendF64(&sb, sheet->GetMarginFooter());
		sb.Append((const UTF8Char*)"\"/>");
		sb.Append((const UTF8Char*)"<pageSetup paperSize=\"9\" scale=\"100\" firstPageNumber=\"1\" fitToWidth=\"1\" fitToHeight=\"1\" pageOrder=\"downThenOver\" orientation=\"portrait\" blackAndWhite=\"false\" draft=\"false\" cellComments=\"none\" useFirstPageNumber=\"true\" horizontalDpi=\"300\" verticalDpi=\"300\" copies=\"1\"/>");
		sb.Append((const UTF8Char*)"<headerFooter differentFirst=\"false\" differentOddEven=\"false\">");
		sb.Append((const UTF8Char*)"<oddHeader>&amp;C&amp;&quot;Times New Roman,Regular&quot;&amp;12&amp;A</oddHeader>");
		sb.Append((const UTF8Char*)"<oddFooter>&amp;C&amp;&quot;Times New Roman,Regular&quot;&amp;12Page &amp;P</oddFooter>");
		sb.Append((const UTF8Char*)"</headerFooter>");
		k = 0;
		l = sheet->GetDrawingCount();
		while (k < l)
		{
			sb.Append((const UTF8Char*)"<drawing r:id=\"rId");
			sb.AppendUOSInt(k + 1);
			sb.Append((const UTF8Char*)"\"/>");
			k++;
		}
		sb.Append((const UTF8Char*)"</worksheet>");
		Text::StrConcat(Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"xl/worksheets/sheet"), i + 1), (const UTF8Char*)".xml");
		zip->AddFile(sbuff, sb.ToString(), sb.GetLength(), dt.ToTicks(), false);
		sbContTypes.Append((const UTF8Char*)"<Override PartName=\"/");
		sbContTypes.Append(sbuff);
		sbContTypes.Append((const UTF8Char*)"\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.worksheet+xml\"/>");

		if (sheet->GetDrawingCount() > 0 || links.GetCount() > 0)
		{
			sb.ClearStr();
			sb.Append((const UTF8Char*)"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>");
			sb.Append((const UTF8Char*)"<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">");
			k = 0;
			l = sheet->GetDrawingCount();
			while (k < l)
			{
				sb.Append((const UTF8Char*)"<Relationship Id=\"rId");
				sb.AppendUOSInt(k + 1);
				sb.Append((const UTF8Char*)"\" Target=\"../drawings/drawing");
				sb.AppendUOSInt(k + 1 + drawingCnt);
				sb.Append((const UTF8Char*)".xml\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/drawing\"/>");
				k++;
			}
			m = 0;
			n = links.GetCount();
			while (m < n)
			{
				link = links.GetItem(m);
				sb.Append((const UTF8Char*)"<Relationship Id=\"rId");
				sb.AppendUOSInt(l + m + 1);
				sb.Append((const UTF8Char*)"\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/hyperlink\" Target=");
				s = Text::XML::ToNewAttrText(link->cell->cellURL->v);
				sb.Append(s);
				s->Release();
				sb.Append((const UTF8Char*)" TargetMode=\"External\"/>");
				MemFree(link);
				m++;
			}
			sb.Append((const UTF8Char*)"</Relationships>");

			Text::StrConcat(Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"xl/worksheets/_rels/sheet"), i + 1), (const UTF8Char*)".xml.rels");
			zip->AddFile(sbuff, sb.ToString(), sb.GetLength(), dt.ToTicks(), false);
			sbContTypes.Append((const UTF8Char*)"<Override PartName=\"/");
			sbContTypes.Append(sbuff);
			sbContTypes.Append((const UTF8Char*)"\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>");

			k = 0;
			l = sheet->GetDrawingCount();
			while (k < l)
			{
				Text::SpreadSheet::WorksheetDrawing *drawing = sheet->GetDrawing(k);
				sb.ClearStr();
				sb.Append((const UTF8Char*)"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");
				sb.Append((const UTF8Char*)"<xdr:wsDr xmlns:xdr=\"http://schemas.openxmlformats.org/drawingml/2006/spreadsheetDrawing\" xmlns:a=\"http://schemas.openxmlformats.org/drawingml/2006/main\" xmlns:c=\"http://schemas.openxmlformats.org/drawingml/2006/chart\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\">");
				switch (drawing->anchorType)
				{
				case Text::SpreadSheet::AnchorType::Absolute:
					sb.Append((const UTF8Char*)"<xdr:absoluteAnchor>");
					sb.Append((const UTF8Char*)"<xdr:pos x=\"");
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->posXInch)));
					sb.Append((const UTF8Char*)"\" y=\"");
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->posYInch)));
					sb.Append((const UTF8Char*)"\"/>");
					sb.Append((const UTF8Char*)"<xdr:ext cx=\"");
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->widthInch)));
					sb.Append((const UTF8Char*)"\" cy=\"");
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->heightInch)));
					sb.Append((const UTF8Char*)"\"/>");
					break;
				case Text::SpreadSheet::AnchorType::OneCell:
					sb.Append((const UTF8Char*)"<xdr:oneCellAnchor>");
					sb.Append((const UTF8Char*)"<xdr:from>");
					sb.Append((const UTF8Char*)"<xdr:col>");
					sb.AppendUOSInt(drawing->col1 + 1);
					sb.Append((const UTF8Char*)"</xdr:col>");
					sb.Append((const UTF8Char*)"<xdr:colOff>");
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->posXInch)));
					sb.Append((const UTF8Char*)"</xdr:colOff>");
					sb.Append((const UTF8Char*)"<xdr:row>");
					sb.AppendUOSInt(drawing->row1 + 1);
					sb.Append((const UTF8Char*)"</xdr:row>");
					sb.Append((const UTF8Char*)"<xdr:rowOff>");
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->posYInch)));
					sb.Append((const UTF8Char*)"</xdr:rowOff>");
					sb.Append((const UTF8Char*)"</xdr:from>");
					sb.Append((const UTF8Char*)"<xdr:ext cx=\"");
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->widthInch)));
					sb.Append((const UTF8Char*)"\" cy=\"");
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->heightInch)));
					sb.Append((const UTF8Char*)"\"/>");
					break;
				case Text::SpreadSheet::AnchorType::TwoCell:
					sb.Append((const UTF8Char*)"<xdr:twoCellAnchor editAs=\"twoCell\">");
					sb.Append((const UTF8Char*)"<xdr:from>");
					sb.Append((const UTF8Char*)"<xdr:col>");
					sb.AppendUOSInt(drawing->col1 + 1);
					sb.Append((const UTF8Char*)"</xdr:col>");
					sb.Append((const UTF8Char*)"<xdr:colOff>");
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->posXInch)));
					sb.Append((const UTF8Char*)"</xdr:colOff>");
					sb.Append((const UTF8Char*)"<xdr:row>");
					sb.AppendUOSInt(drawing->row1 + 1);
					sb.Append((const UTF8Char*)"</xdr:row>");
					sb.Append((const UTF8Char*)"<xdr:rowOff>");
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->posYInch)));
					sb.Append((const UTF8Char*)"</xdr:rowOff>");
					sb.Append((const UTF8Char*)"</xdr:from>");
					sb.Append((const UTF8Char*)"<xdr:to>");
					sb.Append((const UTF8Char*)"<xdr:col>");
					sb.AppendUOSInt(drawing->col2 + 1);
					sb.Append((const UTF8Char*)"</xdr:col>");
					sb.Append((const UTF8Char*)"<xdr:colOff>");
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->widthInch)));
					sb.Append((const UTF8Char*)"</xdr:colOff>");
					sb.Append((const UTF8Char*)"<xdr:row>");
					sb.AppendUOSInt(drawing->row2 + 1);
					sb.Append((const UTF8Char*)"</xdr:row>");
					sb.Append((const UTF8Char*)"<xdr:rowOff>");
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->heightInch)));
					sb.Append((const UTF8Char*)"</xdr:rowOff>");
					sb.Append((const UTF8Char*)"</xdr:to>");
					break;
				}
				if (drawing->chart)
				{
					sb.Append((const UTF8Char*)"<xdr:graphicFrame>");
					sb.Append((const UTF8Char*)"<xdr:nvGraphicFramePr>");
					sb.Append((const UTF8Char*)"<xdr:cNvPr id=\"");
					sb.AppendUOSInt(chartCnt);
					sb.Append((const UTF8Char*)"\" name=\"Diagramm");
					sb.AppendUOSInt(chartCnt);
					sb.Append((const UTF8Char*)"\"/>");
					sb.Append((const UTF8Char*)"<xdr:cNvGraphicFramePr/>");
					sb.Append((const UTF8Char*)"</xdr:nvGraphicFramePr>");
					sb.Append((const UTF8Char*)"<xdr:xfrm>");
					sb.Append((const UTF8Char*)"<a:off x=\"");
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->chart->GetXInch())));
					sb.Append((const UTF8Char*)"\" y=\"");
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->chart->GetYInch())));
					sb.Append((const UTF8Char*)"\"/>");
					sb.Append((const UTF8Char*)"<a:ext cx=\"");
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->chart->GetWInch())));
					sb.Append((const UTF8Char*)"\" cy=\"");
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->chart->GetHInch())));
					sb.Append((const UTF8Char*)"\"/>");
					sb.Append((const UTF8Char*)"</xdr:xfrm>");
					sb.Append((const UTF8Char*)"<a:graphic>");
					sb.Append((const UTF8Char*)"<a:graphicData uri=\"http://schemas.openxmlformats.org/drawingml/2006/chart\">");
					sb.Append((const UTF8Char*)"<c:chart r:id=\"rId");
					sb.AppendUOSInt(chartCnt + 1);
					sb.Append((const UTF8Char*)"\"/>");
					sb.Append((const UTF8Char*)"</a:graphicData>");
					sb.Append((const UTF8Char*)"</a:graphic>");
					sb.Append((const UTF8Char*)"</xdr:graphicFrame>");	
				}
				else
				{
					///////////////////////////////////////
				}
				sb.Append((const UTF8Char*)"<xdr:clientData/>");
				switch (drawing->anchorType)
				{
				case Text::SpreadSheet::AnchorType::Absolute:
					sb.Append((const UTF8Char*)"</xdr:absoluteAnchor>");
					break;
				case Text::SpreadSheet::AnchorType::OneCell:
					sb.Append((const UTF8Char*)"</xdr:oneCellAnchor>");
					break;
				case Text::SpreadSheet::AnchorType::TwoCell:
					sb.Append((const UTF8Char*)"</xdr:twoCellAnchor>");
					break;
				}
				sb.Append((const UTF8Char*)"</xdr:wsDr>");
				drawingCnt++;
				Text::StrConcat(Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"xl/drawings/drawing"), drawingCnt), (const UTF8Char*)".xml");
				zip->AddFile(sbuff, sb.ToString(), sb.GetLength(), dt.ToTicks(), false);
				sbContTypes.Append((const UTF8Char*)"<Override PartName=\"/");
				sbContTypes.Append(sbuff);
				sbContTypes.Append((const UTF8Char*)"\" ContentType=\"application/vnd.openxmlformats-officedocument.drawing+xml\"/>");

				if (drawing->chart)
				{
					sb.ClearStr();
					sb.Append((const UTF8Char*)"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>");
					sb.Append((const UTF8Char*)"<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">");
					sb.Append((const UTF8Char*)"<Relationship Id=\"rId1\" Target=\"../charts/chart");
					sb.AppendUOSInt(chartCnt + 1);
					sb.Append((const UTF8Char*)".xml\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/chart\"/>");
					sb.Append((const UTF8Char*)"</Relationships>");

					Text::StrConcat(Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"xl/drawings/_rels/drawing"), drawingCnt), (const UTF8Char*)".xml.rels");
					zip->AddFile(sbuff, sb.ToString(), sb.GetLength(), dt.ToTicks(), false);
					sbContTypes.Append((const UTF8Char*)"<Override PartName=\"/");
					sbContTypes.Append(sbuff);
					sbContTypes.Append((const UTF8Char*)"\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>");

					chartCnt++;
					sb.ClearStr();
					sb.Append((const UTF8Char*)"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");
					sb.Append((const UTF8Char*)"<c:chartSpace xmlns:c=\"http://schemas.openxmlformats.org/drawingml/2006/chart\" xmlns:a=\"http://schemas.openxmlformats.org/drawingml/2006/main\">");
					sb.Append((const UTF8Char*)"<c:chart>");
					if (drawing->chart->GetTitleText())
					{
						AppendTitle(&sb, drawing->chart->GetTitleText());
					}
					sb.Append((const UTF8Char*)"<c:plotArea>");
					sb.Append((const UTF8Char*)"<c:layout/>");
					if (drawing->chart->GetChartType() != ChartType::Unknown)
					{
						switch (drawing->chart->GetChartType())
						{
						case ChartType::LineChart:
							sb.Append((const UTF8Char*)"<c:lineChart>");
							break;
						case ChartType::Unknown:
							break;
						}
						m = 0;
						n = drawing->chart->GetSeriesCount();
						while (m < n)
						{
							AppendSeries(&sb, drawing->chart->GetSeries(m), m);
							m++;
						}
						if (drawing->chart->GetCategoryAxis())
						{
							sb.Append((const UTF8Char*)"<c:axId val=\"");
							sb.AppendUOSInt(drawing->chart->GetAxisIndex(drawing->chart->GetCategoryAxis()));
							sb.Append((const UTF8Char*)"\"/>");
						}
						if (drawing->chart->GetValueAxis())
						{
							sb.Append((const UTF8Char*)"<c:axId val=\"");
							sb.AppendUOSInt(drawing->chart->GetAxisIndex(drawing->chart->GetValueAxis()));
							sb.Append((const UTF8Char*)"\"/>");
						}
						switch (drawing->chart->GetChartType())
						{
						case ChartType::LineChart:
							sb.Append((const UTF8Char*)"</c:lineChart>");
							break;
						case ChartType::Unknown:
							break;
						}
					}
					m = 0;
					n = drawing->chart->GetAxisCount();
					while (m < n)
					{
						AppendAxis(&sb, drawing->chart->GetAxis(m), m);
						m++;
					}
					AppendShapeProp(&sb, drawing->chart->GetShapeProp());
					sb.Append((const UTF8Char*)"</c:plotArea>");
					if (drawing->chart->HasLegend())
					{
						sb.Append((const UTF8Char*)"<c:legend>");
						sb.Append((const UTF8Char*)"<c:legendPos val=\"");
						switch (drawing->chart->GetLegendPos())
						{
						case LegendPos::Bottom:
							sb.Append((const UTF8Char*)"b");
							break;
						}
						sb.Append((const UTF8Char*)"\"/>");
						sb.Append((const UTF8Char*)"<c:overlay val=\"");
						if (drawing->chart->IsLegendOverlay())
						{
							sb.Append((const UTF8Char*)"true");
						}
						else
						{
							sb.Append((const UTF8Char*)"false");
						}
						sb.Append((const UTF8Char*)"\"/>");
						sb.Append((const UTF8Char*)"</c:legend>");
					}
					sb.Append((const UTF8Char*)"<c:plotVisOnly val=\"true\"/>");
					switch (drawing->chart->GetDisplayBlankAs())
					{
					case BlankAs::Default:
						break;
					case BlankAs::Gap:
						sb.Append((const UTF8Char*)"<c:dispBlanksAs val=\"gap\"/>");
						break;
					case BlankAs::Zero:
						sb.Append((const UTF8Char*)"<c:dispBlanksAs val=\"zero\"/>");
						break;
					}
					sb.Append((const UTF8Char*)"</c:chart>");
					//////////////////////////////////////
					sb.Append((const UTF8Char*)"</c:chartSpace>");

					Text::StrConcat(Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"xl/charts/chart"), chartCnt), (const UTF8Char*)".xml");
					zip->AddFile(sbuff, sb.ToString(), sb.GetLength(), dt.ToTicks(), false);
					sbContTypes.Append((const UTF8Char*)"<Override PartName=\"/");
					sbContTypes.Append(sbuff);
					sbContTypes.Append((const UTF8Char*)"\" ContentType=\"application/vnd.openxmlformats-officedocument.drawingml.chart+xml\"/>");
				}
				k++;
			}
		}
		i++;
	}

	sb.ClearStr();
	sb.Append((const UTF8Char*)"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n");
	sb.Append((const UTF8Char*)"<workbook xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\">");
	sb.Append((const UTF8Char*)"<fileVersion appName=\"AVIRead\"/>");
	sb.Append((const UTF8Char*)"<workbookPr backupFile=\"false\" showObjects=\"all\" date1904=\"false\"/>");
	sb.Append((const UTF8Char*)"<workbookProtection/>");
	sb.Append((const UTF8Char*)"<bookViews>");
	sb.Append((const UTF8Char*)"<workbookView showHorizontalScroll=\"true\" showVerticalScroll=\"true\" showSheetTabs=\"true\" xWindow=\"0\" yWindow=\"0\" windowWidth=\"16384\" windowHeight=\"8192\" tabRatio=\"500\" firstSheet=\"0\" activeTab=\"0\"/>");
	sb.Append((const UTF8Char*)"</bookViews>");
	sb.Append((const UTF8Char*)"<sheets>");
	i = 0;
	j = workbook->GetCount();
	while (i < j)
	{
		Text::SpreadSheet::Worksheet *sheet = workbook->GetItem(i);
		sb.Append((const UTF8Char*)"<sheet name=");
		s = sheet->GetName();
		s2 = Text::XML::ToNewAttrText(s->v);
		sb.Append(s2);
		s2->Release();
		sb.Append((const UTF8Char*)" sheetId=\"");
		sb.AppendUOSInt(i + 1);
		sb.Append((const UTF8Char*)"\" state=\"visible\" r:id=\"rId");
		sb.AppendUOSInt(i + 2);
		sb.Append((const UTF8Char*)"\"/>");
		i++;
	}
	sb.Append((const UTF8Char*)"</sheets>");
	sb.Append((const UTF8Char*)"<calcPr iterateCount=\"100\" refMode=\"A1\" iterate=\"false\" iterateDelta=\"0.001\"/>");
	sb.Append((const UTF8Char*)"</workbook>");
	zip->AddFile((const UTF8Char*)"xl/workbook.xml", sb.ToString(), sb.GetLength(), dt.ToTicks(), false);
	sbContTypes.Append((const UTF8Char*)"<Override PartName=\"/xl/workbook.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet.main+xml\"/>");

	sb.ClearStr();
	sb.Append((const UTF8Char*)"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	sb.Append((const UTF8Char*)"<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">");
	sb.Append((const UTF8Char*)"<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument\" Target=\"xl/workbook.xml\"/>");
	sb.Append((const UTF8Char*)"<Relationship Id=\"rId2\" Type=\"http://schemas.openxmlformats.org/package/2006/relationships/metadata/core-properties\" Target=\"docProps/core.xml\"/>");
	sb.Append((const UTF8Char*)"<Relationship Id=\"rId3\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/extended-properties\" Target=\"docProps/app.xml\"/>");
	sb.Append((const UTF8Char*)"\n</Relationships>");
	zip->AddFile((const UTF8Char*)"_rels/.rels", sb.ToString(), sb.GetLength(), dt.ToTicks(), false);
	sbContTypes.Append((const UTF8Char*)"<Override PartName=\"/_rels/.rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>");

	sb.ClearStr();
	sb.Append((const UTF8Char*)"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\r\n");
	sb.Append((const UTF8Char*)"<styleSheet xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\">");
	{
		Data::StringUTF8Map<UOSInt> numFmtMap;
		Data::ArrayList<const UTF8Char*> numFmts;
		Data::ArrayList<BorderInfo*> borders;
		Text::SpreadSheet::CellStyle::BorderStyle borderNone;
		borderNone.borderType = BorderType::None;
		borderNone.borderColor = 0;
		BorderInfo *border = MemAlloc(BorderInfo, 1);
		border->left = &borderNone;
		border->top = &borderNone;
		border->right = &borderNone;
		border->bottom = &borderNone;
		borders.Add(border);

		csptr = (const UTF8Char*)"general";
		numFmtMap.Put(csptr, numFmts.GetCount());
		numFmts.Add(csptr);

		i = 0;
		j = workbook->GetStyleCount();
		while (i < j)
		{
			Text::SpreadSheet::CellStyle *style = workbook->GetStyle(i);
			csptr = style->GetDataFormat();
			if (csptr == 0)
			{
				csptr = (const UTF8Char*)"general";
			}
			if (!numFmtMap.ContainsKey(csptr))
			{
				numFmtMap.Put(csptr, numFmts.GetCount());
				numFmts.Add(csptr);
			}
			Bool borderFound = false;
			k = borders.GetCount();
			while (k-- > 0)
			{
				border = borders.GetItem(k);
				if (border->left->Equals(style->GetBorderLeft()) &&
					border->top->Equals(style->GetBorderTop()) &&
					border->right->Equals(style->GetBorderRight()) &&
					border->bottom->Equals(style->GetBorderBottom()))
				{
					borderFound = true;
					break;
				}
			}
			if (!borderFound)
			{
				border = MemAlloc(BorderInfo, 1);
				border->left = style->GetBorderLeft();
				border->top = style->GetBorderTop();
				border->right = style->GetBorderRight();
				border->bottom = style->GetBorderBottom();
				borders.Add(border);
			}
			i++;
		}
		if (numFmts.GetCount() > 0)
		{
			sb.Append((const UTF8Char*)"<numFmts count=\"");
			sb.AppendUOSInt(numFmts.GetCount());
			sb.Append((const UTF8Char*)"\">");
			i = 0;
			j = numFmts.GetCount();
			while (i < j)
			{
				sb.Append((const UTF8Char*)"<numFmt numFmtId=\"");
				sb.AppendUOSInt(i + 164);
				sb.Append((const UTF8Char*)"\" formatCode=");
				ToFormatCode(sbuff, numFmts.GetItem(i));
				s = Text::XML::ToNewAttrText(sbuff);
				sb.Append(s);
				s->Release();
				sb.Append((const UTF8Char*)"/>");
				i++;
			}
			sb.Append((const UTF8Char*)"</numFmts>");
		}
		if (workbook->GetFontCount() > 0)
		{
			sb.Append((const UTF8Char*)"<fonts count=\"");
			sb.AppendUOSInt(workbook->GetFontCount());
			sb.Append((const UTF8Char*)"\">");
			i = 0;
			j = workbook->GetFontCount();
			while (i < j)
			{
				Text::SpreadSheet::WorkbookFont *font = workbook->GetFont(i);
				sb.Append((const UTF8Char*)"<font>");
				if (font->GetSize() != 0)
				{
					sb.Append((const UTF8Char*)"<sz val=\"");
					Text::SBAppendF64(&sb, font->GetSize());
					sb.Append((const UTF8Char*)"\"/>");
				}
				if (font->GetName())
				{
					sb.Append((const UTF8Char*)"<name val=");
					s = Text::XML::ToNewAttrText(font->GetName());
					sb.Append(s);
					s->Release();
					sb.Append((const UTF8Char*)"/>");
				}
				switch (font->GetFamily())
				{
				case FontFamily::NA:
					sb.Append((const UTF8Char*)"<family val=\"0\"/>");
					break;
				case FontFamily::Roman:
					sb.Append((const UTF8Char*)"<family val=\"1\"/>");
					break;
				case FontFamily::Swiss:
					sb.Append((const UTF8Char*)"<family val=\"2\"/>");
					break;
				case FontFamily::Modern:
					sb.Append((const UTF8Char*)"<family val=\"3\"/>");
					break;
				case FontFamily::Script:
					sb.Append((const UTF8Char*)"<family val=\"4\"/>");
					break;
				case FontFamily::Decorative:
					sb.Append((const UTF8Char*)"<family val=\"5\"/>");
					break;
				}
				sb.Append((const UTF8Char*)"</font>");
				i++;
			}
			sb.Append((const UTF8Char*)"</fonts>");
		}

		sb.Append((const UTF8Char*)"<fills count=\"2\">");
		sb.Append((const UTF8Char*)"<fill>");
		sb.Append((const UTF8Char*)"<patternFill patternType=\"none\"/>");
		sb.Append((const UTF8Char*)"</fill>");
		sb.Append((const UTF8Char*)"<fill>");
		sb.Append((const UTF8Char*)"<patternFill patternType=\"gray125\"/>");
		sb.Append((const UTF8Char*)"</fill>");
		sb.Append((const UTF8Char*)"</fills>");

		i = 0;
		j = borders.GetCount();
		sb.Append((const UTF8Char*)"<borders count=\"");
		sb.AppendUOSInt(j);
		sb.Append((const UTF8Char*)"\">");
		while (i < j)
		{
			border = borders.GetItem(i);
			sb.Append((const UTF8Char*)"<border diagonalUp=\"false\" diagonalDown=\"false\">");
			AppendBorder(&sb, border->left, (const UTF8Char*)"left");
			AppendBorder(&sb, border->right, (const UTF8Char*)"right");
			AppendBorder(&sb, border->top, (const UTF8Char*)"top");
			AppendBorder(&sb, border->bottom, (const UTF8Char*)"bottom");
			sb.Append((const UTF8Char*)"<diagonal/>");
			sb.Append((const UTF8Char*)"</border>");
			i++;
		}
		sb.Append((const UTF8Char*)"</borders>");

		sb.Append((const UTF8Char*)"<cellStyleXfs count=\"20\">");
		sb.Append((const UTF8Char*)"<xf numFmtId=\"164\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"true\" applyAlignment=\"true\" applyProtection=\"true\">");
		sb.Append((const UTF8Char*)"<alignment horizontal=\"general\" vertical=\"bottom\" textRotation=\"0\" wrapText=\"false\" indent=\"0\" shrinkToFit=\"false\"/>");
		sb.Append((const UTF8Char*)"<protection locked=\"true\" hidden=\"false\"/>");
		sb.Append((const UTF8Char*)"</xf>");
		sb.Append((const UTF8Char*)"<xf numFmtId=\"1\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
		sb.Append((const UTF8Char*)"</xf>");
		sb.Append((const UTF8Char*)"<xf numFmtId=\"1\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
		sb.Append((const UTF8Char*)"</xf>");
		sb.Append((const UTF8Char*)"<xf numFmtId=\"2\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
		sb.Append((const UTF8Char*)"</xf>");
		sb.Append((const UTF8Char*)"<xf numFmtId=\"2\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
		sb.Append((const UTF8Char*)"</xf>");
		sb.Append((const UTF8Char*)"<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
		sb.Append((const UTF8Char*)"</xf>");
		sb.Append((const UTF8Char*)"<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
		sb.Append((const UTF8Char*)"</xf>");
		sb.Append((const UTF8Char*)"<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
		sb.Append((const UTF8Char*)"</xf>");
		sb.Append((const UTF8Char*)"<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
		sb.Append((const UTF8Char*)"</xf>");
		sb.Append((const UTF8Char*)"<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
		sb.Append((const UTF8Char*)"</xf>");
		sb.Append((const UTF8Char*)"<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
		sb.Append((const UTF8Char*)"</xf>");
		sb.Append((const UTF8Char*)"<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
		sb.Append((const UTF8Char*)"</xf>");
		sb.Append((const UTF8Char*)"<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
		sb.Append((const UTF8Char*)"</xf>");
		sb.Append((const UTF8Char*)"<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
		sb.Append((const UTF8Char*)"</xf>");
		sb.Append((const UTF8Char*)"<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
		sb.Append((const UTF8Char*)"</xf>");
		sb.Append((const UTF8Char*)"<xf numFmtId=\"43\" fontId=\"1\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
		sb.Append((const UTF8Char*)"</xf>");
		sb.Append((const UTF8Char*)"<xf numFmtId=\"41\" fontId=\"1\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
		sb.Append((const UTF8Char*)"</xf>");
		sb.Append((const UTF8Char*)"<xf numFmtId=\"44\" fontId=\"1\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
		sb.Append((const UTF8Char*)"</xf>");
		sb.Append((const UTF8Char*)"<xf numFmtId=\"42\" fontId=\"1\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
		sb.Append((const UTF8Char*)"</xf>");
		sb.Append((const UTF8Char*)"<xf numFmtId=\"9\" fontId=\"1\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
		sb.Append((const UTF8Char*)"</xf>");
		sb.Append((const UTF8Char*)"</cellStyleXfs>");

		if (workbook->GetStyleCount() > 0)
		{
			sb.Append((const UTF8Char*)"<cellXfs count=\"");
			sb.AppendUOSInt(workbook->GetStyleCount());
			sb.Append((const UTF8Char*)"\">");
			i = 0;
			j = workbook->GetStyleCount();
			while (i < j)
			{
				Text::SpreadSheet::CellStyle *style = workbook->GetStyle(i);
				Text::SpreadSheet::WorkbookFont *font = style->GetFont();
				csptr = style->GetDataFormat();
				if (csptr == 0)
				{
					csptr = (const UTF8Char*)"general";
				}
				sb.Append((const UTF8Char*)"<xf numFmtId=\"");
				sb.AppendUOSInt(numFmtMap.Get(csptr) + 164);
				sb.Append((const UTF8Char*)"\" fontId=\"");
				if (font == 0)
				{
					sb.AppendChar('0', 1);
				}
				else
				{
					sb.AppendUOSInt(workbook->GetFontIndex(font));
				}
				sb.Append((const UTF8Char*)"\" fillId=\"0\" borderId=\"");
				k = borders.GetCount();
				while (k-- > 0)
				{
					border = borders.GetItem(k);
					if (border->left->Equals(style->GetBorderLeft()) &&
						border->top->Equals(style->GetBorderTop()) &&
						border->right->Equals(style->GetBorderRight()) &&
						border->bottom->Equals(style->GetBorderBottom()))
					{
						break;
					}
				}
				if (k == INVALID_INDEX)
				{
					k = 0;
				}
				sb.AppendUOSInt(k);
				sb.Append((const UTF8Char*)"\" xfId=\"0\" applyFont=\"");
				if (font)
				{
					sb.Append((const UTF8Char*)"true");
				}
				else
				{
					sb.Append((const UTF8Char*)"false");
				}
				sb.Append((const UTF8Char*)"\" applyBorder=\"false\" applyAlignment=\"true\" applyProtection=\"false\">");
				sb.Append((const UTF8Char*)"<alignment horizontal=\"");
				switch (style->GetHAlign())
				{
				case Text::SpreadSheet::HAlignment::Left:
					sb.Append((const UTF8Char*)"left");
					break;
				case Text::SpreadSheet::HAlignment::Center:
					sb.Append((const UTF8Char*)"center");
					break;
				case Text::SpreadSheet::HAlignment::Right:
					sb.Append((const UTF8Char*)"right");
					break;
				case Text::SpreadSheet::HAlignment::Fill:
					sb.Append((const UTF8Char*)"fill");
					break;
				case Text::SpreadSheet::HAlignment::Justify:
					sb.Append((const UTF8Char*)"justify");
					break;
				case Text::SpreadSheet::HAlignment::Unknown:
				default:
					sb.Append((const UTF8Char*)"general");
					break;
				}
				sb.Append((const UTF8Char*)"\" vertical=\"");
				switch (style->GetVAlign())
				{
				case Text::SpreadSheet::VAlignment::Top:
					sb.Append((const UTF8Char*)"top");
					break;
				case Text::SpreadSheet::VAlignment::Center:
					sb.Append((const UTF8Char*)"center");
					break;
				case Text::SpreadSheet::VAlignment::Bottom:
					sb.Append((const UTF8Char*)"bottom");
					break;
				case Text::SpreadSheet::VAlignment::Justify:
					sb.Append((const UTF8Char*)"justify");
					break;
				case Text::SpreadSheet::VAlignment::Unknown:
				default:
					sb.Append((const UTF8Char*)"general");
					break;
				}
				sb.Append((const UTF8Char*)"\" textRotation=\"0\" wrapText=\"");
				sb.Append(style->GetWordWrap()?(const UTF8Char*)"true":(const UTF8Char*)"false");
				sb.Append((const UTF8Char*)"\" indent=\"0\" shrinkToFit=\"false\"/>");
				sb.Append((const UTF8Char*)"<protection locked=\"true\" hidden=\"false\"/>");
				sb.Append((const UTF8Char*)"</xf>");
				i++;
			}
			sb.Append((const UTF8Char*)"</cellXfs>");
		}

		sb.Append((const UTF8Char*)"<cellStyles count=\"6\">");
		sb.Append((const UTF8Char*)"<cellStyle name=\"Normal\" xfId=\"0\" builtinId=\"0\"/>");
		sb.Append((const UTF8Char*)"<cellStyle name=\"Comma\" xfId=\"15\" builtinId=\"3\"/>");
		sb.Append((const UTF8Char*)"<cellStyle name=\"Comma [0]\" xfId=\"16\" builtinId=\"6\"/>");
		sb.Append((const UTF8Char*)"<cellStyle name=\"Currency\" xfId=\"17\" builtinId=\"4\"/>");
		sb.Append((const UTF8Char*)"<cellStyle name=\"Currency [0]\" xfId=\"18\" builtinId=\"7\"/>");
		sb.Append((const UTF8Char*)"<cellStyle name=\"Percent\" xfId=\"19\" builtinId=\"5\"/>");
		sb.Append((const UTF8Char*)"</cellStyles>");

		i = 0;
		j = borders.GetCount();
		while (i < j)
		{
			MemFree(borders.GetItem(i));
			i++;
		}
	}
	sb.Append((const UTF8Char*)"</styleSheet>");
	zip->AddFile((const UTF8Char*)"xl/styles.xml", sb.ToString(), sb.GetLength(), dt.ToTicks(), false);
	sbContTypes.Append((const UTF8Char*)"<Override PartName=\"/xl/styles.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.styles+xml\"/>");

	if (sharedStrings.GetCount() > 0)
	{
		sb.ClearStr();
		sb.Append((const UTF8Char*)"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\r\n");
		sb.Append((const UTF8Char*)"<sst xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\" count=\"");
		sb.AppendUOSInt(sharedStrings.GetCount());
		sb.Append((const UTF8Char*)"\" uniqueCount=\"");
		sb.AppendUOSInt(sharedStrings.GetCount());
		sb.Append((const UTF8Char*)"\">");
		i = 0;
		j = sharedStrings.GetCount();
		while (i < j)
		{
			sb.Append((const UTF8Char*)"<si><t xml:space=\"preserve\">");
			csptr = Text::XML::ToNewXMLText(sharedStrings.GetItem(i)->v);
			sb.Append(csptr);
			Text::XML::FreeNewText(csptr);
			sb.Append((const UTF8Char*)"</t></si>");
			i++;
		}
		sb.Append((const UTF8Char*)"</sst>");
		zip->AddFile((const UTF8Char*)"xl/sharedStrings.xml", sb.ToString(), sb.GetLength(), dt.ToTicks(), false);
		sbContTypes.Append((const UTF8Char*)"<Override PartName=\"/xl/sharedStrings.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.sharedStrings+xml\"/>");
	}

	sb.ClearStr();
	sb.Append((const UTF8Char*)"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	sb.Append((const UTF8Char*)"<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">");
	sb.Append((const UTF8Char*)"<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles\" Target=\"styles.xml\"/>");
	i = 0;
	j = workbook->GetCount();
	while (i < j)
	{
		sb.Append((const UTF8Char*)"<Relationship Id=\"rId");
		sb.AppendUOSInt(i + 2);
		sb.Append((const UTF8Char*)"\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/worksheet\" Target=\"worksheets/sheet");
		sb.AppendUOSInt(i + 1);
		sb.Append((const UTF8Char*)".xml\"/>");
		i++;
	}
	if (sharedStrings.GetCount() > 0)
	{
		sb.Append((const UTF8Char*)"<Relationship Id=\"rId");
		sb.AppendUOSInt(workbook->GetCount() + 2);
		sb.Append((const UTF8Char*)"\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/sharedStrings\" Target=\"sharedStrings.xml\"/>");
	}
	sb.Append((const UTF8Char*)"\n</Relationships>");
	zip->AddFile((const UTF8Char*)"xl/_rels/workbook.xml.rels", sb.ToString(), sb.GetLength(), dt.ToTicks(), false);
	sbContTypes.Append((const UTF8Char*)"<Override PartName=\"/xl/_rels/workbook.xml.rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>");

	sb.ClearStr();
	sb.Append((const UTF8Char*)"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n");
	sb.Append((const UTF8Char*)"<cp:coreProperties xmlns:cp=\"http://schemas.openxmlformats.org/package/2006/metadata/core-properties\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:dcterms=\"http://purl.org/dc/terms/\" xmlns:dcmitype=\"http://purl.org/dc/dcmitype/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">");
	sb.Append((const UTF8Char*)"<dcterms:created xsi:type=\"dcterms:W3CDTF\">");
	t = workbook->GetCreateTime();
	if (t)
	{
		t->ToString(sbuff, "yyyy-MM-dd");
		sb.Append(sbuff);
		sb.AppendChar('T', 1);
		t->ToString(sbuff, "HH:mm:ss");
		sb.Append(sbuff);
		sb.AppendChar('Z', 1);
	}
	sb.Append((const UTF8Char*)"</dcterms:created>");
	sb.Append((const UTF8Char*)"<dc:creator>");
	csptr = workbook->GetAuthor();
	if (csptr)
	{
		csptr2 = Text::XML::ToNewXMLText(csptr);
		sb.Append(csptr2);
		Text::XML::FreeNewText(csptr2);
	}
	sb.Append((const UTF8Char*)"</dc:creator>");
	sb.Append((const UTF8Char*)"<dc:description>");
	csptr = 0;
	if (csptr)
	{
		csptr2 = Text::XML::ToNewXMLText(csptr);
		sb.Append(csptr2);
		Text::XML::FreeNewText(csptr2);
	}
	sb.Append((const UTF8Char*)"</dc:description>");
	sb.Append((const UTF8Char*)"<dc:language>");
	UInt32 lcid = Text::EncodingFactory::GetSystemLCID();
	Text::Locale::LocaleEntry *loc = Text::Locale::GetLocaleEntry(lcid);
	if (loc)
	{
		csptr2 = Text::XML::ToNewXMLText(loc->shortName);
		sb.Append(csptr2);
		Text::XML::FreeNewText(csptr2);
	}
	sb.Append((const UTF8Char*)"</dc:language>");
	sb.Append((const UTF8Char*)"<cp:lastModifiedBy>");
	csptr = workbook->GetLastAuthor();
	if (csptr)
	{
		csptr2 = Text::XML::ToNewXMLText(csptr);
		sb.Append(csptr2);
		Text::XML::FreeNewText(csptr2);
	}
	sb.Append((const UTF8Char*)"</cp:lastModifiedBy>");
	sb.Append((const UTF8Char*)"<dcterms:modified xsi:type=\"dcterms:W3CDTF\">");
	t = workbook->GetModifyTime();
	if (t)
	{
		t->ToString(sbuff, "yyyy-MM-dd");
		sb.Append(sbuff);
		sb.AppendChar('T', 1);
		t->ToString(sbuff, "HH:mm:ss");
		sb.Append(sbuff);
		sb.AppendChar('Z', 1);
	}
	sb.Append((const UTF8Char*)"</dcterms:modified>");
	sb.Append((const UTF8Char*)"<cp:revision>");
	sb.AppendU32(1);
	sb.Append((const UTF8Char*)"</cp:revision>");
	sb.Append((const UTF8Char*)"<dc:subject>");
	csptr = 0;
	if (csptr)
	{
		csptr2 = Text::XML::ToNewXMLText(csptr);
		sb.Append(csptr2);
		Text::XML::FreeNewText(csptr2);
	}
	sb.Append((const UTF8Char*)"</dc:subject>");
	sb.Append((const UTF8Char*)"<dc:title>");
	csptr = 0;
	if (csptr)
	{
		csptr2 = Text::XML::ToNewXMLText(csptr);
		sb.Append(csptr2);
		Text::XML::FreeNewText(csptr2);
	}
	sb.Append((const UTF8Char*)"</dc:title>");
	sb.Append((const UTF8Char*)"</cp:coreProperties>");
	zip->AddFile((const UTF8Char*)"docProps/core.xml", sb.ToString(), sb.GetLength(), dt.ToTicks(), false);
	sbContTypes.Append((const UTF8Char*)"<Override PartName=\"/docProps/core.xml\" ContentType=\"application/vnd.openxmlformats-package.core-properties+xml\"/>");

	sb.ClearStr();
	sb.Append((const UTF8Char*)"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n");
	sb.Append((const UTF8Char*)"<Properties xmlns=\"http://schemas.openxmlformats.org/officeDocument/2006/extended-properties\" xmlns:vt=\"http://schemas.openxmlformats.org/officeDocument/2006/docPropsVTypes\">");
	sb.Append((const UTF8Char*)"<Template></Template>");
	sb.Append((const UTF8Char*)"<TotalTime>1</TotalTime>");
	sb.Append((const UTF8Char*)"<Application>");
	sb.Append((const UTF8Char*)"AVIRead/");
	IO::BuildTime::GetBuildTime(&dt2);
	dt2.ToString(sbuff, "yyyyMMdd_HHmmss");
	sb.Append(sbuff);
	sb.Append((const UTF8Char*)"</Application>");
	sb.Append((const UTF8Char*)"</Properties>");
	zip->AddFile((const UTF8Char*)"docProps/app.xml", sb.ToString(), sb.GetLength(), dt.ToTicks(), false);
	sbContTypes.Append((const UTF8Char*)"<Override PartName=\"/docProps/app.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.extended-properties+xml\"/>");

	sbContTypes.Append((const UTF8Char*)"\n</Types>");
	zip->AddFile((const UTF8Char*)"[Content_Types].xml", sbContTypes.ToString(), sbContTypes.GetLength(), dt.ToTicks(), false);

	DEL_CLASS(zip);
	return true;
}

void Exporter::XLSXExporter::AppendFill(Text::StringBuilderUTF *sb, OfficeFill *fill)
{
	if (fill == 0)
		return;
	switch (fill->GetFillType())
	{
	case FillType::SolidFill:
		if (fill->GetColor() == 0)
		{
			sb->Append((const UTF8Char*)"<a:solidFill/>");
		}
		else
		{
			sb->Append((const UTF8Char*)"<a:solidFill>");
			OfficeColor *color = fill->GetColor();
			if (color->GetColorType() == ColorType::Preset)
			{
				sb->Append((const UTF8Char*)"<a:prstClr val=\"");
				sb->Append((const UTF8Char*)PresetColorCode(color->GetPresetColor()));
				sb->Append((const UTF8Char*)"\"/>");
			}
			sb->Append((const UTF8Char*)"</a:solidFill>");
		}
		break;
	}
}

void Exporter::XLSXExporter::AppendLineStyle(Text::StringBuilderUTF *sb, Text::SpreadSheet::OfficeLineStyle *lineStyle)
{
	if (lineStyle == 0)
		return;
	sb->Append((const UTF8Char*)"<a:ln>");
	AppendFill(sb, lineStyle->GetFillStyle());
	sb->Append((const UTF8Char*)"</a:ln>");
}

void Exporter::XLSXExporter::AppendTitle(Text::StringBuilderUTF *sb, const UTF8Char *title)
{
	sb->Append((const UTF8Char*)"<c:title>");
	sb->Append((const UTF8Char*)"<c:tx>");
	sb->Append((const UTF8Char*)"<c:rich>");
	sb->Append((const UTF8Char*)"<a:bodyPr anchor=\"t\" rtlCol=\"false\"/>");
	sb->Append((const UTF8Char*)"<a:lstStyle/>");
	sb->Append((const UTF8Char*)"<a:p>");
	sb->Append((const UTF8Char*)"<a:pPr algn=\"l\">");
	sb->Append((const UTF8Char*)"<a:defRPr/>");
	sb->Append((const UTF8Char*)"</a:pPr>");
	sb->Append((const UTF8Char*)"<a:r>");
	sb->Append((const UTF8Char*)"<a:rPr lang=\"en-HK\"/>");
	sb->Append((const UTF8Char*)"<a:t>");
	const UTF8Char *csptr = Text::XML::ToNewXMLText(title);
	sb->Append(csptr);
	Text::XML::FreeNewText(csptr);
	sb->Append((const UTF8Char*)"</a:t>");
	sb->Append((const UTF8Char*)"</a:r>");
	sb->Append((const UTF8Char*)"<a:endParaRPr lang=\"en-US\" sz=\"1100\"/>");
	sb->Append((const UTF8Char*)"</a:p>");
	sb->Append((const UTF8Char*)"</c:rich>");
	sb->Append((const UTF8Char*)"</c:tx>");
	sb->Append((const UTF8Char*)"<c:layout/>");
	sb->Append((const UTF8Char*)"</c:title>");
}

void Exporter::XLSXExporter::AppendShapeProp(Text::StringBuilderUTF *sb, Text::SpreadSheet::OfficeShapeProp *shapeProp)
{
	if (shapeProp == 0)
		return;
	sb->Append((const UTF8Char*)"<c:spPr>");
	AppendFill(sb, shapeProp->GetFill());
	AppendLineStyle(sb, shapeProp->GetLineStyle());
	sb->Append((const UTF8Char*)"</c:spPr>");
}

void Exporter::XLSXExporter::AppendAxis(Text::StringBuilderUTF *sb, Text::SpreadSheet::OfficeChartAxis *axis, UOSInt index)
{
	if (axis == 0)
		return;
	
	switch (axis->GetAxisType())
	{
	case AxisType::Category:
		sb->Append((const UTF8Char*)"<c:catAx>");
		break;
	case AxisType::Date:
		sb->Append((const UTF8Char*)"<c:dateAx>");
		break;
	case AxisType::Numeric:
		sb->Append((const UTF8Char*)"<c:valAx>");
		break;
	case AxisType::Series:
		sb->Append((const UTF8Char*)"<c:serAx>");
		break;
	}
	sb->Append((const UTF8Char*)"<c:axId val=\"");
	sb->AppendUOSInt(index);
	sb->Append((const UTF8Char*)"\"/>");
	sb->Append((const UTF8Char*)"<c:scaling>");
	sb->Append((const UTF8Char*)"<c:orientation val=\"minMax\"/>");
	sb->Append((const UTF8Char*)"</c:scaling>");
	sb->Append((const UTF8Char*)"<c:delete val=\"false\"/>");
	switch (axis->GetAxisPos())
	{
	case AxisPosition::Left:
		sb->Append((const UTF8Char*)"<c:axPos val=\"l\"/>");
		break;
	case AxisPosition::Top:
		sb->Append((const UTF8Char*)"<c:axPos val=\"t\"/>");
		break;
	case AxisPosition::Right:
		sb->Append((const UTF8Char*)"<c:axPos val=\"r\"/>");
		break;
	case AxisPosition::Bottom:
		sb->Append((const UTF8Char*)"<c:axPos val=\"b\"/>");
		break;
	}
	if (axis->GetMajorGridProp())
	{
		sb->Append((const UTF8Char*)"<c:majorGridlines>");
		AppendShapeProp(sb, axis->GetMajorGridProp());
		sb->Append((const UTF8Char*)"</c:majorGridlines>");
	}
	if (axis->GetTitle())
	{
		AppendTitle(sb, axis->GetTitle());
	}
	sb->Append((const UTF8Char*)"<c:majorTickMark val=\"cross\"/>");
	sb->Append((const UTF8Char*)"<c:minorTickMark val=\"none\"/>");
	switch (axis->GetTickLblPos())
	{
	case TickLabelPosition::High:
		sb->Append((const UTF8Char*)"<c:tickLblPos val=\"high\"/>");
		break;
	case TickLabelPosition::Low:
		sb->Append((const UTF8Char*)"<c:tickLblPos val=\"low\"/>");
		break;
	case TickLabelPosition::NextTo:
		sb->Append((const UTF8Char*)"<c:tickLblPos val=\"nextTo\"/>");
		break;
	case TickLabelPosition::None:
		sb->Append((const UTF8Char*)"<c:tickLblPos val=\"none\"/>");
		break;
	}
	AppendShapeProp(sb, axis->GetShapeProp());
// 	sb->Append((const UTF8Char*)"<c:crossAx val=\"1\"/>");
	sb->Append((const UTF8Char*)"<c:crosses val=\"autoZero\"/>");
	sb->Append((const UTF8Char*)"<c:crossBetween val=\"midCat\"/>");
	switch (axis->GetAxisType())
	{
	case AxisType::Category:
		sb->Append((const UTF8Char*)"</c:catAx>");
		break;
	case AxisType::Date:
		sb->Append((const UTF8Char*)"</c:dateAx>");
		break;
	case AxisType::Numeric:
		sb->Append((const UTF8Char*)"</c:valAx>");
		break;
	case AxisType::Series:
		sb->Append((const UTF8Char*)"</c:serAx>");
		break;
	}
}

void Exporter::XLSXExporter::AppendSeries(Text::StringBuilderUTF *sb, Text::SpreadSheet::OfficeChartSeries *series, UOSInt index)
{
	const UTF8Char *csptr;
	sb->Append((const UTF8Char*)"<c:ser>");
	sb->Append((const UTF8Char*)"<c:idx val=\"");
	sb->AppendUOSInt(index);
	sb->Append((const UTF8Char*)"\"/>");
	sb->Append((const UTF8Char*)"<c:order val=\"");
	sb->AppendUOSInt(index);
	sb->Append((const UTF8Char*)"\"/>");
	if (series->GetTitle())
	{
		sb->Append((const UTF8Char*)"<c:tx>");
		sb->Append((const UTF8Char*)"<c:v>");
		csptr = Text::XML::ToNewXMLText(series->GetTitle()->v);
		sb->Append(csptr);
		Text::XML::FreeNewText(csptr);
		sb->Append((const UTF8Char*)"</c:v>");
		sb->Append((const UTF8Char*)"</c:tx>");
	}
	AppendShapeProp(sb, series->GetShapeProp());
	sb->Append((const UTF8Char*)"<c:marker>");
	switch (series->GetMarkerStyle())
	{
	case MarkerStyle::Circle:
		sb->Append((const UTF8Char*)"<c:symbol val=\"circle\"/>");
		break;
	case MarkerStyle::Dash:
		sb->Append((const UTF8Char*)"<c:symbol val=\"dash\"/>");
		break;
	case MarkerStyle::Diamond:
		sb->Append((const UTF8Char*)"<c:symbol val=\"diamond\"/>");
		break;
	case MarkerStyle::Dot:
		sb->Append((const UTF8Char*)"<c:symbol val=\"dot\"/>");
		break;
	case MarkerStyle::None:
		sb->Append((const UTF8Char*)"<c:symbol val=\"none\"/>");
		break;
	case MarkerStyle::Picture:
		sb->Append((const UTF8Char*)"<c:symbol val=\"picture\"/>");
		break;
	case MarkerStyle::Plus:
		sb->Append((const UTF8Char*)"<c:symbol val=\"plus\"/>");
		break;
	case MarkerStyle::Square:
		sb->Append((const UTF8Char*)"<c:symbol val=\"square\"/>");
		break;
	case MarkerStyle::Star:
		sb->Append((const UTF8Char*)"<c:symbol val=\"star\"/>");
		break;
	case MarkerStyle::Triangle:
		sb->Append((const UTF8Char*)"<c:symbol val=\"triangle\"/>");
		break;
	case MarkerStyle::X:
		sb->Append((const UTF8Char*)"<c:symbol val=\"x\"/>");
		break;
	}
	if (series->GetMarkerSize() != 0)
	{
		sb->Append((const UTF8Char*)"<c:size val=\"");
		sb->AppendU32(series->GetMarkerSize());
		sb->Append((const UTF8Char*)"\"/>");
	}
	sb->Append((const UTF8Char*)"</c:marker>");

	UTF8Char sbuff[128];
	WorkbookDataSource *catData = series->GetCategoryData();
	sb->Append((const UTF8Char*)"<c:cat>");
	sb->Append((const UTF8Char*)"<c:strRef>");
	sb->Append((const UTF8Char*)"<c:f>");
	catData->ToCodeRange(sbuff);
	csptr = Text::XML::ToNewXMLText(sbuff);
	sb->Append(csptr);
	Text::XML::FreeNewText(csptr);
	sb->Append((const UTF8Char*)"</c:f>");
	sb->Append((const UTF8Char*)"<c:strCache/>");
	sb->Append((const UTF8Char*)"</c:strRef>");
	sb->Append((const UTF8Char*)"</c:cat>");

	WorkbookDataSource *valData = series->GetValueData();
	sb->Append((const UTF8Char*)"<c:val>");
	sb->Append((const UTF8Char*)"<c:numRef>");
	sb->Append((const UTF8Char*)"<c:f>");
	valData->ToCodeRange(sbuff);
	csptr = Text::XML::ToNewXMLText(sbuff);
	sb->Append(csptr);
	Text::XML::FreeNewText(csptr);
	sb->Append((const UTF8Char*)"</c:f>");
	sb->Append((const UTF8Char*)"<c:numCache/>");
	sb->Append((const UTF8Char*)"</c:numRef>");
	sb->Append((const UTF8Char*)"</c:val>");

	sb->Append((const UTF8Char*)"<c:smooth val=\"");
	sb->Append((const UTF8Char*)(series->IsSmooth()?"true":"false"));
	sb->Append((const UTF8Char*)"\"/>");
	sb->Append((const UTF8Char*)"</c:ser>");
}

void Exporter::XLSXExporter::AppendBorder(Text::StringBuilderUTF *sb, Text::SpreadSheet::CellStyle::BorderStyle *border, const UTF8Char *name)
{
	sb->AppendChar('<', 1);
	sb->Append(name);
	if (border->borderType == BorderType::None)
	{
		sb->Append((const UTF8Char*)"/>");
	}
	else
	{
		switch (border->borderType)
		{
			case BorderType::Thin:
				sb->Append((const UTF8Char*)" style=\"thin\">");
				break;
			case BorderType::Medium:
				sb->Append((const UTF8Char*)" style=\"medium\">");
				break;
			case BorderType::Dashed:
				sb->Append((const UTF8Char*)" style=\"dashed\">");
				break;
			case BorderType::Dotted:
				sb->Append((const UTF8Char*)" style=\"dotted\">");
				break;
			case BorderType::Thick:
				sb->Append((const UTF8Char*)" style=\"thick\">");
				break;
			case BorderType::DOUBLE:
				sb->Append((const UTF8Char*)" style=\"double\">");
				break;
			case BorderType::Hair:
				sb->Append((const UTF8Char*)" style=\"hair\">");
				break;
			case BorderType::MediumDashed:
				sb->Append((const UTF8Char*)" style=\"mediumDashed\">");
				break;
			case BorderType::DashDot:
				sb->Append((const UTF8Char*)" style=\"dashDot\">");
				break;
			case BorderType::MediumDashDot:
				sb->Append((const UTF8Char*)" style=\"mediumDashDot\">");
				break;
			case BorderType::DashDotDot:
				sb->Append((const UTF8Char*)" style=\"dashDotDot\">");
				break;
			case BorderType::MediumDashDotDot:
				sb->Append((const UTF8Char*)" style=\"mediumDashDotDot\">");
				break;
			case BorderType::SlantedDashDot:
				sb->Append((const UTF8Char*)" style=\"slantDashDot\">");
				break;
			case BorderType::None:
				break;
		}
		sb->Append((const UTF8Char*)"<color rgb=\"");
		sb->AppendHex32(border->borderColor);
		sb->Append((const UTF8Char*)"\"/>");
		sb->Append((const UTF8Char*)"</");
		sb->Append(name);
		sb->AppendChar('>', 1);
	}
}

const Char *Exporter::XLSXExporter::PresetColorCode(PresetColor color)
{
	switch (color)
	{
	case PresetColor::AliceBlue:
		return "aliceBlue";		
	case PresetColor::AntiqueWhite:
		return "antiqueWhite";		
	case PresetColor::Aqua:
		return "aqua";		
	case PresetColor::Aquamarine:
		return "aquamarine";		
	case PresetColor::Azure:
		return "azure";		
	case PresetColor::Beige:
		return "beige";		
	case PresetColor::Bisque:
		return "bisque";		
	case PresetColor::Black:
		return "black";		
	case PresetColor::BlanchedAlmond:
		return "blanchedAlmond";		
	case PresetColor::Blue:
		return "blue";		
	case PresetColor::BlueViolet:
		return "blueViolet";		
	case PresetColor::Brown:
		return "brown";		
	case PresetColor::BurlyWood:
		return "burlyWood";		
	case PresetColor::CadetBlue:
		return "cadetBlue";		
	case PresetColor::Chartreuse:
		return "chartreuse";		
	case PresetColor::Chocolate:
		return "chocolate";		
	case PresetColor::Coral:
		return "coral";		
	case PresetColor::CornflowerBlue:
		return "cornflowerBlue";		
	case PresetColor::Cornsilk:
		return "cornsilk";		
	case PresetColor::Crimson:
		return "crimson";		
	case PresetColor::Cyan:
		return "cyan";		
	case PresetColor::DeepPink:
		return "deepPink";		
	case PresetColor::DeepSkyBlue:
		return "deepSkyBlue";		
	case PresetColor::DimGray:
		return "dimGray";		
	case PresetColor::DarkBlue:
		return "dkBlue";		
	case PresetColor::DarkCyan:
		return "dkCyan";		
	case PresetColor::DarkGoldenrod:
		return "dkGoldenrod";		
	case PresetColor::DarkGray:
		return "dkGray";		
	case PresetColor::DarkGreen:
		return "dkGreen";		
	case PresetColor::DarkKhaki:
		return "dkKhaki";		
	case PresetColor::DarkMagenta:
		return "dkMagenta";		
	case PresetColor::DarkOliveGreen:
		return "dkOliveGreen";		
	case PresetColor::DarkOrange:
		return "dkOrange";		
	case PresetColor::DarkOrchid:
		return "dkOrchid";		
	case PresetColor::DarkRed:
		return "dkRed";		
	case PresetColor::DarkSalmon:
		return "dkSalmon";		
	case PresetColor::DarkSeaGreen:
		return "dkSeaGreen";		
	case PresetColor::DarkSlateBlue:
		return "dkSlateBlue";		
	case PresetColor::DarkSlateGray:
		return "dkSlateGray";		
	case PresetColor::DarkTurquoise:
		return "dkTurquoise";		
	case PresetColor::DarkViolet:
		return "dkViolet";		
	case PresetColor::DodgerBlue:
		return "dodgerBlue";		
	case PresetColor::Firebrick:
		return "firebrick";		
	case PresetColor::FloralWhite:
		return "floralWhite";		
	case PresetColor::ForestGreen:
		return "forestGreen";		
	case PresetColor::Fuchsia:
		return "fuchsia";		
	case PresetColor::Gainsboro:
		return "gainsboro";		
	case PresetColor::GhostWhite:
		return "ghostWhite";		
	case PresetColor::Gold:
		return "gold";		
	case PresetColor::Goldenrod:
		return "goldenrod";		
	case PresetColor::Gray:
		return "gray";		
	case PresetColor::Green:
		return "green";		
	case PresetColor::GreenYellow:
		return "greenYellow";		
	case PresetColor::Honeydew:
		return "honeydew";		
	case PresetColor::HotPink:
		return "hotPink";		
	case PresetColor::IndianRed:
		return "indianRed";		
	case PresetColor::Indigo:
		return "indigo";		
	case PresetColor::Ivory:
		return "ivory";		
	case PresetColor::Khaki:
		return "khaki";		
	case PresetColor::Lavender:
		return "lavender";		
	case PresetColor::LavenderBlush:
		return "lavenderBlush";		
	case PresetColor::LawnGreen:
		return "lawnGreen";		
	case PresetColor::LemonChiffon:
		return "lemonChiffon";		
	case PresetColor::Lime:
		return "lime";		
	case PresetColor::LimeGreen:
		return "limeGreen";		
	case PresetColor::Linen:
		return "linen";		
	case PresetColor::LightBlue:
		return "ltBlue";		
	case PresetColor::LightCoral:
		return "ltCoral";		
	case PresetColor::LightCyan:
		return "ltCyan";		
	case PresetColor::LightGoldenrodYellow:
		return "ltGoldenrodYellow";		
	case PresetColor::LightGray:
		return "ltGray";		
	case PresetColor::LightGreen:
		return "ltGreen";		
	case PresetColor::LightPink:
		return "ltPink";		
	case PresetColor::LightSalmon:
		return "ltSalmon";		
	case PresetColor::LightSeaGreen:
		return "ltSeaGreen";		
	case PresetColor::LightSkyBlue:
		return "ltSkyBlue";		
	case PresetColor::LightSlateGray:
		return "ltSlateGray";		
	case PresetColor::LightSteelBlue:
		return "ltSteelBlue";		
	case PresetColor::LightYellow:
		return "ltYellow";		
	case PresetColor::Magenta:
		return "magenta";		
	case PresetColor::Maroon:
		return "maroon";		
	case PresetColor::MediumAquamarine:
		return "medAquamarine";		
	case PresetColor::MediumBlue:
		return "medBlue";		
	case PresetColor::MediumOrchid:
		return "medOrchid";		
	case PresetColor::MediumPurple:
		return "medPurple";		
	case PresetColor::MediumSeaGreen:
		return "medSeaGreen";		
	case PresetColor::MediumSlateBlue:
		return "medSlateBlue";		
	case PresetColor::MediumSpringGreen:
		return "medSpringGreen";		
	case PresetColor::MediumTurquoise:
		return "medTurquoise";		
	case PresetColor::MediumVioletRed:
		return "medVioletRed";		
	case PresetColor::MidnightBlue:
		return "midnightBlue";		
	case PresetColor::MintCream:
		return "mintCream";		
	case PresetColor::MistyRose:
		return "mistyRose";		
	case PresetColor::Moccasin:
		return "moccasin";		
	case PresetColor::NavajoWhite:
		return "navajoWhite";		
	case PresetColor::Navy:
		return "navy";		
	case PresetColor::OldLace:
		return "oldLace";		
	case PresetColor::Olive:
		return "olive";		
	case PresetColor::OliveDrab:
		return "oliveDrab";		
	case PresetColor::Orange:
		return "orange";		
	case PresetColor::OrangeRed:
		return "orangeRed";		
	case PresetColor::Orchid:
		return "orchid";		
	case PresetColor::PaleGoldenrod:
		return "paleGoldenrod";		
	case PresetColor::PaleGreen:
		return "paleGreen";		
	case PresetColor::PaleTurquoise:
		return "paleTurquoise";		
	case PresetColor::PaleVioletRed:
		return "paleVioletRed";		
	case PresetColor::PapayaWhip:
		return "papayaWhip";		
	case PresetColor::PeachPuff:
		return "peachPuff";		
	case PresetColor::Peru:
		return "peru";		
	case PresetColor::Pink:
		return "pink";		
	case PresetColor::Plum:
		return "plum";		
	case PresetColor::PowderBlue:
		return "powderBlue";		
	case PresetColor::Purple:
		return "purple";		
	case PresetColor::Red:
		return "red";		
	case PresetColor::RosyBrown:
		return "rosyBrown";		
	case PresetColor::RoyalBlue:
		return "royalBlue";		
	case PresetColor::SaddleBrown:
		return "saddleBrown";		
	case PresetColor::Salmon:
		return "salmon";		
	case PresetColor::SandyBrown:
		return "sandyBrown";		
	case PresetColor::SeaGreen:
		return "seaGreen";		
	case PresetColor::SeaShell:
		return "seaShell";		
	case PresetColor::Sienna:
		return "sienna";		
	case PresetColor::Silver:
		return "silver";		
	case PresetColor::SkyBlue:
		return "skyBlue";		
	case PresetColor::SlateBlue:
		return "slateBlue";		
	case PresetColor::SlateGray:
		return "slateGray";		
	case PresetColor::Snow:
		return "snow";		
	case PresetColor::SpringGreen:
		return "springGreen";		
	case PresetColor::SteelBlue:
		return "steelBlue";		
	case PresetColor::Tan:
		return "tan";		
	case PresetColor::Teal:
		return "teal";		
	case PresetColor::Thistle:
		return "thistle";		
	case PresetColor::Tomato:
		return "tomato";		
	case PresetColor::Turquoise:
		return "turquoise";		
	case PresetColor::Violet:
		return "violet";		
	case PresetColor::Wheat:
		return "wheat";		
	case PresetColor::White:
		return "white";		
	case PresetColor::WhiteSmoke:
		return "whiteSmoke";		
	case PresetColor::Yellow:
		return "yellow";		
	case PresetColor::YellowGreen:
		return "yellowGreen";		
	default:
		return "Unknown";		
	}
}
UTF8Char *Exporter::XLSXExporter::ToFormatCode(UTF8Char *sbuff, const UTF8Char *dataFormat)
{
	UTF8Char c;
	while (true)
	{
		c = *dataFormat++;
		if (c == 0)
		{
			*sbuff = 0;
			break;
		}
		else if (c == '-')
		{
			*sbuff++ = '\\';
			*sbuff++ = c;
		}
		else
		{
			*sbuff++ = Text::CharUtil::ToLower(c);
		}
	}
	return sbuff;
}
