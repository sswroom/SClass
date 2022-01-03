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

	sbContTypes.AppendC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"));
	sbContTypes.AppendC(UTF8STRC("<Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\">"));
	sbContTypes.AppendC(UTF8STRC("<Default Extension=\"xml\" ContentType=\"application/xml\"/>"));
	sbContTypes.AppendC(UTF8STRC("<Default Extension=\"rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>"));
	sbContTypes.AppendC(UTF8STRC("<Default Extension=\"png\" ContentType=\"image/png\"/>"));
	sbContTypes.AppendC(UTF8STRC("<Default Extension=\"jpeg\" ContentType=\"image/jpeg\"/>"));

	i = 0;
	j = workbook->GetCount();
	while (i < j)
	{
		sheet = workbook->GetItem(i);

		Data::ArrayList<LinkInfo*> links;
		LinkInfo *link;

		sb.ClearStr();
		sb.AppendC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"));
		sb.AppendC(UTF8STRC("<worksheet xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\">"));
		sb.AppendC(UTF8STRC("<sheetPr filterMode=\"false\">"));
		sb.AppendC(UTF8STRC("<pageSetUpPr fitToPage=\"false\"/>"));
		sb.AppendC(UTF8STRC("</sheetPr>"));
		sb.AppendC(UTF8STRC("<dimension ref=\"A1\"/>"));
		sb.AppendC(UTF8STRC("<sheetViews>"));
		sb.AppendC(UTF8STRC("<sheetView showFormulas=\"false\" showGridLines=\"true\" showRowColHeaders=\"true\" showZeros=\"true\" rightToLeft=\"false\" tabSelected=\"true\" showOutlineSymbols=\"true\" defaultGridColor=\"true\" view=\"normal\" topLeftCell=\"A1\" colorId=\"64\" zoomScale=\""));
		sb.AppendU32(sheet->GetZoom());
		sb.AppendC(UTF8STRC("\" zoomScaleNormal=\""));
		sb.AppendU32(sheet->GetZoom());
		sb.AppendC(UTF8STRC("\" zoomScalePageLayoutView=\""));
		sb.AppendU32(sheet->GetZoom());
		sb.AppendC(UTF8STRC("\" workbookViewId=\"0\">"));
		sb.AppendC(UTF8STRC("<selection pane=\"topLeft\" activeCell=\"A1\" activeCellId=\"0\" sqref=\"A1\"/>"));
		sb.AppendC(UTF8STRC("</sheetView>"));
		sb.AppendC(UTF8STRC("</sheetViews>"));
		sb.AppendC(UTF8STRC("<sheetFormatPr defaultColWidth=\""));
		Text::SBAppendF64(&sb, sheet->GetDefColWidthPt() / 5.25);
		sb.AppendC(UTF8STRC("\" defaultRowHeight=\""));
		Text::SBAppendF64(&sb, sheet->GetDefRowHeightPt());
		sb.AppendC(UTF8STRC("\" zeroHeight=\"false\" outlineLevelRow=\"0\" outlineLevelCol=\"0\"></sheetFormatPr>"));
		sb.AppendC(UTF8STRC("<cols>"));

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
					sb.AppendC(UTF8STRC("<col min=\""));
					sb.AppendUOSInt(lastColIndex + 1);
					sb.AppendC(UTF8STRC("\" max=\""));
					sb.AppendUOSInt(k);
					sb.AppendC(UTF8STRC("\" width=\""));
					if (lastColWidth >= 0)
					{
						Text::SBAppendF64(&sb, lastColWidth / 5.25);
						sb.AppendC(UTF8STRC("\" customWidth=\"1\""));
					}
					else
					{
						Text::SBAppendF64(&sb, sheet->GetDefColWidthPt() / 5.25);
						sb.AppendC(UTF8STRC("\" customWidth=\"false\""));
					}
					sb.AppendC(UTF8STRC("/>"));
				}
				lastColWidth = sheet->GetColWidthPt(k);
				lastColIndex = k;
			}
			k++;
		}
		if (lastColWidth >= 0)
		{
			sb.AppendC(UTF8STRC("<col min=\""));
			sb.AppendUOSInt(lastColIndex + 1);
			sb.AppendC(UTF8STRC("\" max=\""));
			sb.AppendUOSInt(l);
			sb.AppendC(UTF8STRC("\" width=\""));
			if (lastColWidth >= 0)
			{
				Text::SBAppendF64(&sb, lastColWidth / 5.25);
				sb.AppendC(UTF8STRC("\" customWidth=\"1\""));
			}
			else
			{
				Text::SBAppendF64(&sb, sheet->GetDefColWidthPt() / 5.25);
				sb.AppendC(UTF8STRC("\" customWidth=\"false\""));
			}
			sb.AppendC(UTF8STRC("/>"));
		}
		if (l < sheet->GetMaxCol())
		{
			sb.AppendC(UTF8STRC("<col min=\""));
			sb.AppendUOSInt(l + 1);
			sb.AppendC(UTF8STRC("\" max=\""));
			sb.AppendUOSInt(sheet->GetMaxCol());
			sb.AppendC(UTF8STRC("\" width=\""));
			Text::SBAppendF64(&sb, sheet->GetDefColWidthPt() / 5.25);
			sb.AppendC(UTF8STRC("\" customWidth=\"false\" collapsed=\"false\" hidden=\"false\" outlineLevel=\"0\" style=\"0\"/>"));
		}

		sb.AppendC(UTF8STRC("</cols>"));
		k = 0;
		l = sheet->GetCount();
		if (l > 0)
		{
			sb.AppendC(UTF8STRC("<sheetData>"));
			while (k < l)
			{
				Text::SpreadSheet::Worksheet::RowData *row = sheet->GetItem(k);
				if (row)
				{
					sb.AppendC(UTF8STRC("<row r=\""));
					sb.AppendUOSInt(k + 1);
					sb.AppendC(UTF8STRC("\" customFormat=\"false\" ht=\"12.8\" hidden=\"false\" customHeight=\"false\" outlineLevel=\"0\" collapsed=\"false\">"));

					m = 0;
					n = row->cells->GetCount();
					while (m < n)
					{
						Text::SpreadSheet::Worksheet::CellData *cell = row->cells->GetItem(m);
						if (cell && cell->cellValue && cell->cdt != Text::SpreadSheet::CellDataType::MergedLeft && cell->cdt != Text::SpreadSheet::CellDataType::MergedTop)
						{
							sb.AppendC(UTF8STRC("<c r=\""));
							Text::StrUOSInt(Text::SpreadSheet::Workbook::ColCode(sbuff, m), k + 1);
							sb.Append(sbuff);
							sb.AppendChar('"', 1);
							if (cell->style)
							{
								sb.AppendC(UTF8STRC(" s=\""));
								sb.AppendUOSInt(cell->style->GetIndex());
								sb.AppendChar('"', 1);
							}
							switch (cell->cdt)
							{
							case Text::SpreadSheet::CellDataType::String:
								sb.AppendC(UTF8STRC(" t=\"s\""));
								break;
							case Text::SpreadSheet::CellDataType::Number:
							case Text::SpreadSheet::CellDataType::DateTime:
								sb.AppendC(UTF8STRC(" t=\"n\""));
								break;
							case Text::SpreadSheet::CellDataType::MergedLeft:
							case Text::SpreadSheet::CellDataType::MergedTop:
								break;
							}
							sb.AppendC(UTF8STRC("><v>"));
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
							
							sb.AppendC(UTF8STRC("</v></c>"));
						}
						m++;
					}
					sb.AppendC(UTF8STRC("</row>"));
				}

				k++;
			}
			sb.AppendC(UTF8STRC("</sheetData>"));

			if (links.GetCount() > 0)
			{
				UOSInt idBase = sheet->GetDrawingCount() + 1;
				sb.AppendC(UTF8STRC("<hyperlinks>"));
				m = 0;
				n = links.GetCount();
				while (m < n)
				{
					link = links.GetItem(m);
					sb.AppendC(UTF8STRC("<hyperlink ref=\""));
					Text::SpreadSheet::Workbook::ColCode(sbuff, link->col);
					sb.Append(sbuff);
					sb.AppendUOSInt(link->row + 1);
					sb.AppendC(UTF8STRC("\" r:id=\"rId"));
					sb.AppendUOSInt(idBase + m);
					sb.AppendC(UTF8STRC("\" display="));
					s = Text::XML::ToNewAttrText(link->cell->cellValue->v);
					sb.Append(s);
					s->Release();
					sb.AppendC(UTF8STRC("/>"));
					m++;
				}
				sb.AppendC(UTF8STRC("</hyperlinks>"));
			}
		}
		else
		{
			sb.AppendC(UTF8STRC("<sheetData/>"));
		}
		//<sheetProtection sheet="true" password="cc1a" objects="true" scenarios="true"/><printOptions headings="false" gridLines="false" gridLinesSet="true" horizontalCentered="false" verticalCentered="false"/>
		sb.AppendC(UTF8STRC("<pageMargins left=\""));
		Text::SBAppendF64(&sb, sheet->GetMarginLeft());
		sb.AppendC(UTF8STRC("\" right=\""));
		Text::SBAppendF64(&sb, sheet->GetMarginRight());
		sb.AppendC(UTF8STRC("\" top=\""));
		Text::SBAppendF64(&sb, sheet->GetMarginTop());
		sb.AppendC(UTF8STRC("\" bottom=\""));
		Text::SBAppendF64(&sb, sheet->GetMarginBottom());
		sb.AppendC(UTF8STRC("\" header=\""));
		Text::SBAppendF64(&sb, sheet->GetMarginHeader());
		sb.AppendC(UTF8STRC("\" footer=\""));
		Text::SBAppendF64(&sb, sheet->GetMarginFooter());
		sb.AppendC(UTF8STRC("\"/>"));
		sb.AppendC(UTF8STRC("<pageSetup paperSize=\"9\" scale=\"100\" firstPageNumber=\"1\" fitToWidth=\"1\" fitToHeight=\"1\" pageOrder=\"downThenOver\" orientation=\"portrait\" blackAndWhite=\"false\" draft=\"false\" cellComments=\"none\" useFirstPageNumber=\"true\" horizontalDpi=\"300\" verticalDpi=\"300\" copies=\"1\"/>"));
		sb.AppendC(UTF8STRC("<headerFooter differentFirst=\"false\" differentOddEven=\"false\">"));
		sb.AppendC(UTF8STRC("<oddHeader>&amp;C&amp;&quot;Times New Roman,Regular&quot;&amp;12&amp;A</oddHeader>"));
		sb.AppendC(UTF8STRC("<oddFooter>&amp;C&amp;&quot;Times New Roman,Regular&quot;&amp;12Page &amp;P</oddFooter>"));
		sb.AppendC(UTF8STRC("</headerFooter>"));
		k = 0;
		l = sheet->GetDrawingCount();
		while (k < l)
		{
			sb.AppendC(UTF8STRC("<drawing r:id=\"rId"));
			sb.AppendUOSInt(k + 1);
			sb.AppendC(UTF8STRC("\"/>"));
			k++;
		}
		sb.AppendC(UTF8STRC("</worksheet>"));
		Text::StrConcat(Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"xl/worksheets/sheet"), i + 1), (const UTF8Char*)".xml");
		zip->AddFile(sbuff, sb.ToString(), sb.GetLength(), dt.ToTicks(), false);
		sbContTypes.AppendC(UTF8STRC("<Override PartName=\"/"));
		sbContTypes.Append(sbuff);
		sbContTypes.AppendC(UTF8STRC("\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.worksheet+xml\"/>"));

		if (sheet->GetDrawingCount() > 0 || links.GetCount() > 0)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"));
			sb.AppendC(UTF8STRC("<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"));
			k = 0;
			l = sheet->GetDrawingCount();
			while (k < l)
			{
				sb.AppendC(UTF8STRC("<Relationship Id=\"rId"));
				sb.AppendUOSInt(k + 1);
				sb.AppendC(UTF8STRC("\" Target=\"../drawings/drawing"));
				sb.AppendUOSInt(k + 1 + drawingCnt);
				sb.AppendC(UTF8STRC(".xml\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/drawing\"/>"));
				k++;
			}
			m = 0;
			n = links.GetCount();
			while (m < n)
			{
				link = links.GetItem(m);
				sb.AppendC(UTF8STRC("<Relationship Id=\"rId"));
				sb.AppendUOSInt(l + m + 1);
				sb.AppendC(UTF8STRC("\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/hyperlink\" Target="));
				s = Text::XML::ToNewAttrText(link->cell->cellURL->v);
				sb.Append(s);
				s->Release();
				sb.AppendC(UTF8STRC(" TargetMode=\"External\"/>"));
				MemFree(link);
				m++;
			}
			sb.AppendC(UTF8STRC("</Relationships>"));

			Text::StrConcat(Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"xl/worksheets/_rels/sheet"), i + 1), (const UTF8Char*)".xml.rels");
			zip->AddFile(sbuff, sb.ToString(), sb.GetLength(), dt.ToTicks(), false);
			sbContTypes.AppendC(UTF8STRC("<Override PartName=\"/"));
			sbContTypes.Append(sbuff);
			sbContTypes.AppendC(UTF8STRC("\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>"));

			k = 0;
			l = sheet->GetDrawingCount();
			while (k < l)
			{
				Text::SpreadSheet::WorksheetDrawing *drawing = sheet->GetDrawing(k);
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"));
				sb.AppendC(UTF8STRC("<xdr:wsDr xmlns:xdr=\"http://schemas.openxmlformats.org/drawingml/2006/spreadsheetDrawing\" xmlns:a=\"http://schemas.openxmlformats.org/drawingml/2006/main\" xmlns:c=\"http://schemas.openxmlformats.org/drawingml/2006/chart\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\">"));
				switch (drawing->anchorType)
				{
				case Text::SpreadSheet::AnchorType::Absolute:
					sb.AppendC(UTF8STRC("<xdr:absoluteAnchor>"));
					sb.AppendC(UTF8STRC("<xdr:pos x=\""));
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->posXInch)));
					sb.AppendC(UTF8STRC("\" y=\""));
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->posYInch)));
					sb.AppendC(UTF8STRC("\"/>"));
					sb.AppendC(UTF8STRC("<xdr:ext cx=\""));
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->widthInch)));
					sb.AppendC(UTF8STRC("\" cy=\""));
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->heightInch)));
					sb.AppendC(UTF8STRC("\"/>"));
					break;
				case Text::SpreadSheet::AnchorType::OneCell:
					sb.AppendC(UTF8STRC("<xdr:oneCellAnchor>"));
					sb.AppendC(UTF8STRC("<xdr:from>"));
					sb.AppendC(UTF8STRC("<xdr:col>"));
					sb.AppendUOSInt(drawing->col1 + 1);
					sb.AppendC(UTF8STRC("</xdr:col>"));
					sb.AppendC(UTF8STRC("<xdr:colOff>"));
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->posXInch)));
					sb.AppendC(UTF8STRC("</xdr:colOff>"));
					sb.AppendC(UTF8STRC("<xdr:row>"));
					sb.AppendUOSInt(drawing->row1 + 1);
					sb.AppendC(UTF8STRC("</xdr:row>"));
					sb.AppendC(UTF8STRC("<xdr:rowOff>"));
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->posYInch)));
					sb.AppendC(UTF8STRC("</xdr:rowOff>"));
					sb.AppendC(UTF8STRC("</xdr:from>"));
					sb.AppendC(UTF8STRC("<xdr:ext cx=\""));
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->widthInch)));
					sb.AppendC(UTF8STRC("\" cy=\""));
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->heightInch)));
					sb.AppendC(UTF8STRC("\"/>"));
					break;
				case Text::SpreadSheet::AnchorType::TwoCell:
					sb.AppendC(UTF8STRC("<xdr:twoCellAnchor editAs=\"twoCell\">"));
					sb.AppendC(UTF8STRC("<xdr:from>"));
					sb.AppendC(UTF8STRC("<xdr:col>"));
					sb.AppendUOSInt(drawing->col1 + 1);
					sb.AppendC(UTF8STRC("</xdr:col>"));
					sb.AppendC(UTF8STRC("<xdr:colOff>"));
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->posXInch)));
					sb.AppendC(UTF8STRC("</xdr:colOff>"));
					sb.AppendC(UTF8STRC("<xdr:row>"));
					sb.AppendUOSInt(drawing->row1 + 1);
					sb.AppendC(UTF8STRC("</xdr:row>"));
					sb.AppendC(UTF8STRC("<xdr:rowOff>"));
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->posYInch)));
					sb.AppendC(UTF8STRC("</xdr:rowOff>"));
					sb.AppendC(UTF8STRC("</xdr:from>"));
					sb.AppendC(UTF8STRC("<xdr:to>"));
					sb.AppendC(UTF8STRC("<xdr:col>"));
					sb.AppendUOSInt(drawing->col2 + 1);
					sb.AppendC(UTF8STRC("</xdr:col>"));
					sb.AppendC(UTF8STRC("<xdr:colOff>"));
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->widthInch)));
					sb.AppendC(UTF8STRC("</xdr:colOff>"));
					sb.AppendC(UTF8STRC("<xdr:row>"));
					sb.AppendUOSInt(drawing->row2 + 1);
					sb.AppendC(UTF8STRC("</xdr:row>"));
					sb.AppendC(UTF8STRC("<xdr:rowOff>"));
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->heightInch)));
					sb.AppendC(UTF8STRC("</xdr:rowOff>"));
					sb.AppendC(UTF8STRC("</xdr:to>"));
					break;
				}
				if (drawing->chart)
				{
					sb.AppendC(UTF8STRC("<xdr:graphicFrame>"));
					sb.AppendC(UTF8STRC("<xdr:nvGraphicFramePr>"));
					sb.AppendC(UTF8STRC("<xdr:cNvPr id=\""));
					sb.AppendUOSInt(chartCnt);
					sb.AppendC(UTF8STRC("\" name=\"Diagramm"));
					sb.AppendUOSInt(chartCnt);
					sb.AppendC(UTF8STRC("\"/>"));
					sb.AppendC(UTF8STRC("<xdr:cNvGraphicFramePr/>"));
					sb.AppendC(UTF8STRC("</xdr:nvGraphicFramePr>"));
					sb.AppendC(UTF8STRC("<xdr:xfrm>"));
					sb.AppendC(UTF8STRC("<a:off x=\""));
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->chart->GetXInch())));
					sb.AppendC(UTF8STRC("\" y=\""));
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->chart->GetYInch())));
					sb.AppendC(UTF8STRC("\"/>"));
					sb.AppendC(UTF8STRC("<a:ext cx=\""));
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->chart->GetWInch())));
					sb.AppendC(UTF8STRC("\" cy=\""));
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->chart->GetHInch())));
					sb.AppendC(UTF8STRC("\"/>"));
					sb.AppendC(UTF8STRC("</xdr:xfrm>"));
					sb.AppendC(UTF8STRC("<a:graphic>"));
					sb.AppendC(UTF8STRC("<a:graphicData uri=\"http://schemas.openxmlformats.org/drawingml/2006/chart\">"));
					sb.AppendC(UTF8STRC("<c:chart r:id=\"rId"));
					sb.AppendUOSInt(chartCnt + 1);
					sb.AppendC(UTF8STRC("\"/>"));
					sb.AppendC(UTF8STRC("</a:graphicData>"));
					sb.AppendC(UTF8STRC("</a:graphic>"));
					sb.AppendC(UTF8STRC("</xdr:graphicFrame>"));	
				}
				else
				{
					///////////////////////////////////////
				}
				sb.AppendC(UTF8STRC("<xdr:clientData/>"));
				switch (drawing->anchorType)
				{
				case Text::SpreadSheet::AnchorType::Absolute:
					sb.AppendC(UTF8STRC("</xdr:absoluteAnchor>"));
					break;
				case Text::SpreadSheet::AnchorType::OneCell:
					sb.AppendC(UTF8STRC("</xdr:oneCellAnchor>"));
					break;
				case Text::SpreadSheet::AnchorType::TwoCell:
					sb.AppendC(UTF8STRC("</xdr:twoCellAnchor>"));
					break;
				}
				sb.AppendC(UTF8STRC("</xdr:wsDr>"));
				drawingCnt++;
				Text::StrConcat(Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"xl/drawings/drawing"), drawingCnt), (const UTF8Char*)".xml");
				zip->AddFile(sbuff, sb.ToString(), sb.GetLength(), dt.ToTicks(), false);
				sbContTypes.AppendC(UTF8STRC("<Override PartName=\"/"));
				sbContTypes.Append(sbuff);
				sbContTypes.AppendC(UTF8STRC("\" ContentType=\"application/vnd.openxmlformats-officedocument.drawing+xml\"/>"));

				if (drawing->chart)
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"));
					sb.AppendC(UTF8STRC("<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"));
					sb.AppendC(UTF8STRC("<Relationship Id=\"rId1\" Target=\"../charts/chart"));
					sb.AppendUOSInt(chartCnt + 1);
					sb.AppendC(UTF8STRC(".xml\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/chart\"/>"));
					sb.AppendC(UTF8STRC("</Relationships>"));

					Text::StrConcat(Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"xl/drawings/_rels/drawing"), drawingCnt), (const UTF8Char*)".xml.rels");
					zip->AddFile(sbuff, sb.ToString(), sb.GetLength(), dt.ToTicks(), false);
					sbContTypes.AppendC(UTF8STRC("<Override PartName=\"/"));
					sbContTypes.Append(sbuff);
					sbContTypes.AppendC(UTF8STRC("\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>"));

					chartCnt++;
					sb.ClearStr();
					sb.AppendC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"));
					sb.AppendC(UTF8STRC("<c:chartSpace xmlns:c=\"http://schemas.openxmlformats.org/drawingml/2006/chart\" xmlns:a=\"http://schemas.openxmlformats.org/drawingml/2006/main\">"));
					sb.AppendC(UTF8STRC("<c:chart>"));
					if (drawing->chart->GetTitleText())
					{
						AppendTitle(&sb, drawing->chart->GetTitleText());
					}
					sb.AppendC(UTF8STRC("<c:plotArea>"));
					sb.AppendC(UTF8STRC("<c:layout/>"));
					if (drawing->chart->GetChartType() != ChartType::Unknown)
					{
						switch (drawing->chart->GetChartType())
						{
						case ChartType::LineChart:
							sb.AppendC(UTF8STRC("<c:lineChart>"));
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
							sb.AppendC(UTF8STRC("<c:axId val=\""));
							sb.AppendUOSInt(drawing->chart->GetAxisIndex(drawing->chart->GetCategoryAxis()));
							sb.AppendC(UTF8STRC("\"/>"));
						}
						if (drawing->chart->GetValueAxis())
						{
							sb.AppendC(UTF8STRC("<c:axId val=\""));
							sb.AppendUOSInt(drawing->chart->GetAxisIndex(drawing->chart->GetValueAxis()));
							sb.AppendC(UTF8STRC("\"/>"));
						}
						switch (drawing->chart->GetChartType())
						{
						case ChartType::LineChart:
							sb.AppendC(UTF8STRC("</c:lineChart>"));
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
					sb.AppendC(UTF8STRC("</c:plotArea>"));
					if (drawing->chart->HasLegend())
					{
						sb.AppendC(UTF8STRC("<c:legend>"));
						sb.AppendC(UTF8STRC("<c:legendPos val=\""));
						switch (drawing->chart->GetLegendPos())
						{
						case LegendPos::Bottom:
							sb.AppendC(UTF8STRC("b"));
							break;
						}
						sb.AppendC(UTF8STRC("\"/>"));
						sb.AppendC(UTF8STRC("<c:overlay val=\""));
						if (drawing->chart->IsLegendOverlay())
						{
							sb.AppendC(UTF8STRC("true"));
						}
						else
						{
							sb.AppendC(UTF8STRC("false"));
						}
						sb.AppendC(UTF8STRC("\"/>"));
						sb.AppendC(UTF8STRC("</c:legend>"));
					}
					sb.AppendC(UTF8STRC("<c:plotVisOnly val=\"true\"/>"));
					switch (drawing->chart->GetDisplayBlankAs())
					{
					case BlankAs::Default:
						break;
					case BlankAs::Gap:
						sb.AppendC(UTF8STRC("<c:dispBlanksAs val=\"gap\"/>"));
						break;
					case BlankAs::Zero:
						sb.AppendC(UTF8STRC("<c:dispBlanksAs val=\"zero\"/>"));
						break;
					}
					sb.AppendC(UTF8STRC("</c:chart>"));
					//////////////////////////////////////
					sb.AppendC(UTF8STRC("</c:chartSpace>"));

					Text::StrConcat(Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"xl/charts/chart"), chartCnt), (const UTF8Char*)".xml");
					zip->AddFile(sbuff, sb.ToString(), sb.GetLength(), dt.ToTicks(), false);
					sbContTypes.AppendC(UTF8STRC("<Override PartName=\"/"));
					sbContTypes.Append(sbuff);
					sbContTypes.AppendC(UTF8STRC("\" ContentType=\"application/vnd.openxmlformats-officedocument.drawingml.chart+xml\"/>"));
				}
				k++;
			}
		}
		i++;
	}

	sb.ClearStr();
	sb.AppendC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"));
	sb.AppendC(UTF8STRC("<workbook xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\">"));
	sb.AppendC(UTF8STRC("<fileVersion appName=\"AVIRead\"/>"));
	sb.AppendC(UTF8STRC("<workbookPr backupFile=\"false\" showObjects=\"all\" date1904=\"false\"/>"));
	sb.AppendC(UTF8STRC("<workbookProtection/>"));
	sb.AppendC(UTF8STRC("<bookViews>"));
	sb.AppendC(UTF8STRC("<workbookView showHorizontalScroll=\"true\" showVerticalScroll=\"true\" showSheetTabs=\"true\" xWindow=\"0\" yWindow=\"0\" windowWidth=\"16384\" windowHeight=\"8192\" tabRatio=\"500\" firstSheet=\"0\" activeTab=\"0\"/>"));
	sb.AppendC(UTF8STRC("</bookViews>"));
	sb.AppendC(UTF8STRC("<sheets>"));
	i = 0;
	j = workbook->GetCount();
	while (i < j)
	{
		Text::SpreadSheet::Worksheet *sheet = workbook->GetItem(i);
		sb.AppendC(UTF8STRC("<sheet name="));
		s = sheet->GetName();
		s2 = Text::XML::ToNewAttrText(s->v);
		sb.Append(s2);
		s2->Release();
		sb.AppendC(UTF8STRC(" sheetId=\""));
		sb.AppendUOSInt(i + 1);
		sb.AppendC(UTF8STRC("\" state=\"visible\" r:id=\"rId"));
		sb.AppendUOSInt(i + 2);
		sb.AppendC(UTF8STRC("\"/>"));
		i++;
	}
	sb.AppendC(UTF8STRC("</sheets>"));
	sb.AppendC(UTF8STRC("<calcPr iterateCount=\"100\" refMode=\"A1\" iterate=\"false\" iterateDelta=\"0.001\"/>"));
	sb.AppendC(UTF8STRC("</workbook>"));
	zip->AddFile((const UTF8Char*)"xl/workbook.xml", sb.ToString(), sb.GetLength(), dt.ToTicks(), false);
	sbContTypes.AppendC(UTF8STRC("<Override PartName=\"/xl/workbook.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet.main+xml\"/>"));

	sb.ClearStr();
	sb.AppendC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"));
	sb.AppendC(UTF8STRC("<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"));
	sb.AppendC(UTF8STRC("<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument\" Target=\"xl/workbook.xml\"/>"));
	sb.AppendC(UTF8STRC("<Relationship Id=\"rId2\" Type=\"http://schemas.openxmlformats.org/package/2006/relationships/metadata/core-properties\" Target=\"docProps/core.xml\"/>"));
	sb.AppendC(UTF8STRC("<Relationship Id=\"rId3\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/extended-properties\" Target=\"docProps/app.xml\"/>"));
	sb.AppendC(UTF8STRC("\n</Relationships>"));
	zip->AddFile((const UTF8Char*)"_rels/.rels", sb.ToString(), sb.GetLength(), dt.ToTicks(), false);
	sbContTypes.AppendC(UTF8STRC("<Override PartName=\"/_rels/.rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>"));

	sb.ClearStr();
	sb.AppendC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\r\n"));
	sb.AppendC(UTF8STRC("<styleSheet xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\">"));
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
			sb.AppendC(UTF8STRC("<numFmts count=\""));
			sb.AppendUOSInt(numFmts.GetCount());
			sb.AppendC(UTF8STRC("\">"));
			i = 0;
			j = numFmts.GetCount();
			while (i < j)
			{
				sb.AppendC(UTF8STRC("<numFmt numFmtId=\""));
				sb.AppendUOSInt(i + 164);
				sb.AppendC(UTF8STRC("\" formatCode="));
				ToFormatCode(sbuff, numFmts.GetItem(i));
				s = Text::XML::ToNewAttrText(sbuff);
				sb.Append(s);
				s->Release();
				sb.AppendC(UTF8STRC("/>"));
				i++;
			}
			sb.AppendC(UTF8STRC("</numFmts>"));
		}
		if (workbook->GetFontCount() > 0)
		{
			sb.AppendC(UTF8STRC("<fonts count=\""));
			sb.AppendUOSInt(workbook->GetFontCount());
			sb.AppendC(UTF8STRC("\">"));
			i = 0;
			j = workbook->GetFontCount();
			while (i < j)
			{
				Text::SpreadSheet::WorkbookFont *font = workbook->GetFont(i);
				sb.AppendC(UTF8STRC("<font>"));
				if (font->GetSize() != 0)
				{
					sb.AppendC(UTF8STRC("<sz val=\""));
					Text::SBAppendF64(&sb, font->GetSize());
					sb.AppendC(UTF8STRC("\"/>"));
				}
				if (font->GetName())
				{
					sb.AppendC(UTF8STRC("<name val="));
					s = Text::XML::ToNewAttrText(font->GetName());
					sb.Append(s);
					s->Release();
					sb.AppendC(UTF8STRC("/>"));
				}
				switch (font->GetFamily())
				{
				case FontFamily::NA:
					sb.AppendC(UTF8STRC("<family val=\"0\"/>"));
					break;
				case FontFamily::Roman:
					sb.AppendC(UTF8STRC("<family val=\"1\"/>"));
					break;
				case FontFamily::Swiss:
					sb.AppendC(UTF8STRC("<family val=\"2\"/>"));
					break;
				case FontFamily::Modern:
					sb.AppendC(UTF8STRC("<family val=\"3\"/>"));
					break;
				case FontFamily::Script:
					sb.AppendC(UTF8STRC("<family val=\"4\"/>"));
					break;
				case FontFamily::Decorative:
					sb.AppendC(UTF8STRC("<family val=\"5\"/>"));
					break;
				}
				sb.AppendC(UTF8STRC("</font>"));
				i++;
			}
			sb.AppendC(UTF8STRC("</fonts>"));
		}

		sb.AppendC(UTF8STRC("<fills count=\"2\">"));
		sb.AppendC(UTF8STRC("<fill>"));
		sb.AppendC(UTF8STRC("<patternFill patternType=\"none\"/>"));
		sb.AppendC(UTF8STRC("</fill>"));
		sb.AppendC(UTF8STRC("<fill>"));
		sb.AppendC(UTF8STRC("<patternFill patternType=\"gray125\"/>"));
		sb.AppendC(UTF8STRC("</fill>"));
		sb.AppendC(UTF8STRC("</fills>"));

		i = 0;
		j = borders.GetCount();
		sb.AppendC(UTF8STRC("<borders count=\""));
		sb.AppendUOSInt(j);
		sb.AppendC(UTF8STRC("\">"));
		while (i < j)
		{
			border = borders.GetItem(i);
			sb.AppendC(UTF8STRC("<border diagonalUp=\"false\" diagonalDown=\"false\">"));
			AppendBorder(&sb, border->left, (const UTF8Char*)"left");
			AppendBorder(&sb, border->right, (const UTF8Char*)"right");
			AppendBorder(&sb, border->top, (const UTF8Char*)"top");
			AppendBorder(&sb, border->bottom, (const UTF8Char*)"bottom");
			sb.AppendC(UTF8STRC("<diagonal/>"));
			sb.AppendC(UTF8STRC("</border>"));
			i++;
		}
		sb.AppendC(UTF8STRC("</borders>"));

		sb.AppendC(UTF8STRC("<cellStyleXfs count=\"20\">"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"164\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"true\" applyAlignment=\"true\" applyProtection=\"true\">"));
		sb.AppendC(UTF8STRC("<alignment horizontal=\"general\" vertical=\"bottom\" textRotation=\"0\" wrapText=\"false\" indent=\"0\" shrinkToFit=\"false\"/>"));
		sb.AppendC(UTF8STRC("<protection locked=\"true\" hidden=\"false\"/>"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"1\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"1\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"2\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"2\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"43\" fontId=\"1\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"41\" fontId=\"1\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"44\" fontId=\"1\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"42\" fontId=\"1\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"9\" fontId=\"1\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("</cellStyleXfs>"));

		if (workbook->GetStyleCount() > 0)
		{
			sb.AppendC(UTF8STRC("<cellXfs count=\""));
			sb.AppendUOSInt(workbook->GetStyleCount());
			sb.AppendC(UTF8STRC("\">"));
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
				sb.AppendC(UTF8STRC("<xf numFmtId=\""));
				sb.AppendUOSInt(numFmtMap.Get(csptr) + 164);
				sb.AppendC(UTF8STRC("\" fontId=\""));
				if (font == 0)
				{
					sb.AppendChar('0', 1);
				}
				else
				{
					sb.AppendUOSInt(workbook->GetFontIndex(font));
				}
				sb.AppendC(UTF8STRC("\" fillId=\"0\" borderId=\""));
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
				sb.AppendC(UTF8STRC("\" xfId=\"0\" applyFont=\""));
				if (font)
				{
					sb.AppendC(UTF8STRC("true"));
				}
				else
				{
					sb.AppendC(UTF8STRC("false"));
				}
				sb.AppendC(UTF8STRC("\" applyBorder=\"false\" applyAlignment=\"true\" applyProtection=\"false\">"));
				sb.AppendC(UTF8STRC("<alignment horizontal=\""));
				switch (style->GetHAlign())
				{
				case Text::SpreadSheet::HAlignment::Left:
					sb.AppendC(UTF8STRC("left"));
					break;
				case Text::SpreadSheet::HAlignment::Center:
					sb.AppendC(UTF8STRC("center"));
					break;
				case Text::SpreadSheet::HAlignment::Right:
					sb.AppendC(UTF8STRC("right"));
					break;
				case Text::SpreadSheet::HAlignment::Fill:
					sb.AppendC(UTF8STRC("fill"));
					break;
				case Text::SpreadSheet::HAlignment::Justify:
					sb.AppendC(UTF8STRC("justify"));
					break;
				case Text::SpreadSheet::HAlignment::Unknown:
				default:
					sb.AppendC(UTF8STRC("general"));
					break;
				}
				sb.AppendC(UTF8STRC("\" vertical=\""));
				switch (style->GetVAlign())
				{
				case Text::SpreadSheet::VAlignment::Top:
					sb.AppendC(UTF8STRC("top"));
					break;
				case Text::SpreadSheet::VAlignment::Center:
					sb.AppendC(UTF8STRC("center"));
					break;
				case Text::SpreadSheet::VAlignment::Bottom:
					sb.AppendC(UTF8STRC("bottom"));
					break;
				case Text::SpreadSheet::VAlignment::Justify:
					sb.AppendC(UTF8STRC("justify"));
					break;
				case Text::SpreadSheet::VAlignment::Unknown:
				default:
					sb.AppendC(UTF8STRC("general"));
					break;
				}
				sb.AppendC(UTF8STRC("\" textRotation=\"0\" wrapText=\""));
				sb.Append(style->GetWordWrap()?(const UTF8Char*)"true":(const UTF8Char*)"false");
				sb.AppendC(UTF8STRC("\" indent=\"0\" shrinkToFit=\"false\"/>"));
				sb.AppendC(UTF8STRC("<protection locked=\"true\" hidden=\"false\"/>"));
				sb.AppendC(UTF8STRC("</xf>"));
				i++;
			}
			sb.AppendC(UTF8STRC("</cellXfs>"));
		}

		sb.AppendC(UTF8STRC("<cellStyles count=\"6\">"));
		sb.AppendC(UTF8STRC("<cellStyle name=\"Normal\" xfId=\"0\" builtinId=\"0\"/>"));
		sb.AppendC(UTF8STRC("<cellStyle name=\"Comma\" xfId=\"15\" builtinId=\"3\"/>"));
		sb.AppendC(UTF8STRC("<cellStyle name=\"Comma [0]\" xfId=\"16\" builtinId=\"6\"/>"));
		sb.AppendC(UTF8STRC("<cellStyle name=\"Currency\" xfId=\"17\" builtinId=\"4\"/>"));
		sb.AppendC(UTF8STRC("<cellStyle name=\"Currency [0]\" xfId=\"18\" builtinId=\"7\"/>"));
		sb.AppendC(UTF8STRC("<cellStyle name=\"Percent\" xfId=\"19\" builtinId=\"5\"/>"));
		sb.AppendC(UTF8STRC("</cellStyles>"));

		i = 0;
		j = borders.GetCount();
		while (i < j)
		{
			MemFree(borders.GetItem(i));
			i++;
		}
	}
	sb.AppendC(UTF8STRC("</styleSheet>"));
	zip->AddFile((const UTF8Char*)"xl/styles.xml", sb.ToString(), sb.GetLength(), dt.ToTicks(), false);
	sbContTypes.AppendC(UTF8STRC("<Override PartName=\"/xl/styles.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.styles+xml\"/>"));

	if (sharedStrings.GetCount() > 0)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\r\n"));
		sb.AppendC(UTF8STRC("<sst xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\" count=\""));
		sb.AppendUOSInt(sharedStrings.GetCount());
		sb.AppendC(UTF8STRC("\" uniqueCount=\""));
		sb.AppendUOSInt(sharedStrings.GetCount());
		sb.AppendC(UTF8STRC("\">"));
		i = 0;
		j = sharedStrings.GetCount();
		while (i < j)
		{
			sb.AppendC(UTF8STRC("<si><t xml:space=\"preserve\">"));
			s = Text::XML::ToNewXMLText(sharedStrings.GetItem(i)->v);
			sb.Append(s);
			s->Release();
			sb.AppendC(UTF8STRC("</t></si>"));
			i++;
		}
		sb.AppendC(UTF8STRC("</sst>"));
		zip->AddFile((const UTF8Char*)"xl/sharedStrings.xml", sb.ToString(), sb.GetLength(), dt.ToTicks(), false);
		sbContTypes.AppendC(UTF8STRC("<Override PartName=\"/xl/sharedStrings.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.sharedStrings+xml\"/>"));
	}

	sb.ClearStr();
	sb.AppendC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"));
	sb.AppendC(UTF8STRC("<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"));
	sb.AppendC(UTF8STRC("<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles\" Target=\"styles.xml\"/>"));
	i = 0;
	j = workbook->GetCount();
	while (i < j)
	{
		sb.AppendC(UTF8STRC("<Relationship Id=\"rId"));
		sb.AppendUOSInt(i + 2);
		sb.AppendC(UTF8STRC("\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/worksheet\" Target=\"worksheets/sheet"));
		sb.AppendUOSInt(i + 1);
		sb.AppendC(UTF8STRC(".xml\"/>"));
		i++;
	}
	if (sharedStrings.GetCount() > 0)
	{
		sb.AppendC(UTF8STRC("<Relationship Id=\"rId"));
		sb.AppendUOSInt(workbook->GetCount() + 2);
		sb.AppendC(UTF8STRC("\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/sharedStrings\" Target=\"sharedStrings.xml\"/>"));
	}
	sb.AppendC(UTF8STRC("\n</Relationships>"));
	zip->AddFile((const UTF8Char*)"xl/_rels/workbook.xml.rels", sb.ToString(), sb.GetLength(), dt.ToTicks(), false);
	sbContTypes.AppendC(UTF8STRC("<Override PartName=\"/xl/_rels/workbook.xml.rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>"));

	sb.ClearStr();
	sb.AppendC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"));
	sb.AppendC(UTF8STRC("<cp:coreProperties xmlns:cp=\"http://schemas.openxmlformats.org/package/2006/metadata/core-properties\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:dcterms=\"http://purl.org/dc/terms/\" xmlns:dcmitype=\"http://purl.org/dc/dcmitype/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"));
	sb.AppendC(UTF8STRC("<dcterms:created xsi:type=\"dcterms:W3CDTF\">"));
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
	sb.AppendC(UTF8STRC("</dcterms:created>"));
	sb.AppendC(UTF8STRC("<dc:creator>"));
	csptr = workbook->GetAuthor();
	if (csptr)
	{
		s = Text::XML::ToNewXMLText(csptr);
		sb.Append(s);
		s->Release();
	}
	sb.AppendC(UTF8STRC("</dc:creator>"));
	sb.AppendC(UTF8STRC("<dc:description>"));
	csptr = 0;
	if (csptr)
	{
		s = Text::XML::ToNewXMLText(csptr);
		sb.Append(s);
		s->Release();
	}
	sb.AppendC(UTF8STRC("</dc:description>"));
	sb.AppendC(UTF8STRC("<dc:language>"));
	UInt32 lcid = Text::EncodingFactory::GetSystemLCID();
	Text::Locale::LocaleEntry *loc = Text::Locale::GetLocaleEntry(lcid);
	if (loc)
	{
		s = Text::XML::ToNewXMLText(loc->shortName);
		sb.Append(s);
		s->Release();
	}
	sb.AppendC(UTF8STRC("</dc:language>"));
	sb.AppendC(UTF8STRC("<cp:lastModifiedBy>"));
	csptr = workbook->GetLastAuthor();
	if (csptr)
	{
		s = Text::XML::ToNewXMLText(csptr);
		sb.Append(s);
		s->Release();
	}
	sb.AppendC(UTF8STRC("</cp:lastModifiedBy>"));
	sb.AppendC(UTF8STRC("<dcterms:modified xsi:type=\"dcterms:W3CDTF\">"));
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
	sb.AppendC(UTF8STRC("</dcterms:modified>"));
	sb.AppendC(UTF8STRC("<cp:revision>"));
	sb.AppendU32(1);
	sb.AppendC(UTF8STRC("</cp:revision>"));
	sb.AppendC(UTF8STRC("<dc:subject>"));
	csptr = 0;
	if (csptr)
	{
		s = Text::XML::ToNewXMLText(csptr);
		sb.Append(s);
		s->Release();
	}
	sb.AppendC(UTF8STRC("</dc:subject>"));
	sb.AppendC(UTF8STRC("<dc:title>"));
	csptr = 0;
	if (csptr)
	{
		s = Text::XML::ToNewXMLText(csptr);
		sb.Append(s);
		s->Release();
	}
	sb.AppendC(UTF8STRC("</dc:title>"));
	sb.AppendC(UTF8STRC("</cp:coreProperties>"));
	zip->AddFile((const UTF8Char*)"docProps/core.xml", sb.ToString(), sb.GetLength(), dt.ToTicks(), false);
	sbContTypes.AppendC(UTF8STRC("<Override PartName=\"/docProps/core.xml\" ContentType=\"application/vnd.openxmlformats-package.core-properties+xml\"/>"));

	sb.ClearStr();
	sb.AppendC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"));
	sb.AppendC(UTF8STRC("<Properties xmlns=\"http://schemas.openxmlformats.org/officeDocument/2006/extended-properties\" xmlns:vt=\"http://schemas.openxmlformats.org/officeDocument/2006/docPropsVTypes\">"));
	sb.AppendC(UTF8STRC("<Template></Template>"));
	sb.AppendC(UTF8STRC("<TotalTime>1</TotalTime>"));
	sb.AppendC(UTF8STRC("<Application>"));
	sb.AppendC(UTF8STRC("AVIRead/"));
	IO::BuildTime::GetBuildTime(&dt2);
	dt2.ToString(sbuff, "yyyyMMdd_HHmmss");
	sb.Append(sbuff);
	sb.AppendC(UTF8STRC("</Application>"));
	sb.AppendC(UTF8STRC("</Properties>"));
	zip->AddFile((const UTF8Char*)"docProps/app.xml", sb.ToString(), sb.GetLength(), dt.ToTicks(), false);
	sbContTypes.AppendC(UTF8STRC("<Override PartName=\"/docProps/app.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.extended-properties+xml\"/>"));

	sbContTypes.AppendC(UTF8STRC("\n</Types>"));
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
			sb->AppendC(UTF8STRC("<a:solidFill/>"));
		}
		else
		{
			sb->AppendC(UTF8STRC("<a:solidFill>"));
			OfficeColor *color = fill->GetColor();
			if (color->GetColorType() == ColorType::Preset)
			{
				sb->AppendC(UTF8STRC("<a:prstClr val=\""));
				sb->Append((const UTF8Char*)PresetColorCode(color->GetPresetColor()));
				sb->AppendC(UTF8STRC("\"/>"));
			}
			sb->AppendC(UTF8STRC("</a:solidFill>"));
		}
		break;
	}
}

void Exporter::XLSXExporter::AppendLineStyle(Text::StringBuilderUTF *sb, Text::SpreadSheet::OfficeLineStyle *lineStyle)
{
	if (lineStyle == 0)
		return;
	sb->AppendC(UTF8STRC("<a:ln>"));
	AppendFill(sb, lineStyle->GetFillStyle());
	sb->AppendC(UTF8STRC("</a:ln>"));
}

void Exporter::XLSXExporter::AppendTitle(Text::StringBuilderUTF *sb, const UTF8Char *title)
{
	sb->AppendC(UTF8STRC("<c:title>"));
	sb->AppendC(UTF8STRC("<c:tx>"));
	sb->AppendC(UTF8STRC("<c:rich>"));
	sb->AppendC(UTF8STRC("<a:bodyPr anchor=\"t\" rtlCol=\"false\"/>"));
	sb->AppendC(UTF8STRC("<a:lstStyle/>"));
	sb->AppendC(UTF8STRC("<a:p>"));
	sb->AppendC(UTF8STRC("<a:pPr algn=\"l\">"));
	sb->AppendC(UTF8STRC("<a:defRPr/>"));
	sb->AppendC(UTF8STRC("</a:pPr>"));
	sb->AppendC(UTF8STRC("<a:r>"));
	sb->AppendC(UTF8STRC("<a:rPr lang=\"en-HK\"/>"));
	sb->AppendC(UTF8STRC("<a:t>"));
	Text::String *s = Text::XML::ToNewXMLText(title);
	sb->Append(s);
	s->Release();
	sb->AppendC(UTF8STRC("</a:t>"));
	sb->AppendC(UTF8STRC("</a:r>"));
	sb->AppendC(UTF8STRC("<a:endParaRPr lang=\"en-US\" sz=\"1100\"/>"));
	sb->AppendC(UTF8STRC("</a:p>"));
	sb->AppendC(UTF8STRC("</c:rich>"));
	sb->AppendC(UTF8STRC("</c:tx>"));
	sb->AppendC(UTF8STRC("<c:layout/>"));
	sb->AppendC(UTF8STRC("</c:title>"));
}

void Exporter::XLSXExporter::AppendShapeProp(Text::StringBuilderUTF *sb, Text::SpreadSheet::OfficeShapeProp *shapeProp)
{
	if (shapeProp == 0)
		return;
	sb->AppendC(UTF8STRC("<c:spPr>"));
	AppendFill(sb, shapeProp->GetFill());
	AppendLineStyle(sb, shapeProp->GetLineStyle());
	sb->AppendC(UTF8STRC("</c:spPr>"));
}

void Exporter::XLSXExporter::AppendAxis(Text::StringBuilderUTF *sb, Text::SpreadSheet::OfficeChartAxis *axis, UOSInt index)
{
	if (axis == 0)
		return;
	
	switch (axis->GetAxisType())
	{
	case AxisType::Category:
		sb->AppendC(UTF8STRC("<c:catAx>"));
		break;
	case AxisType::Date:
		sb->AppendC(UTF8STRC("<c:dateAx>"));
		break;
	case AxisType::Numeric:
		sb->AppendC(UTF8STRC("<c:valAx>"));
		break;
	case AxisType::Series:
		sb->AppendC(UTF8STRC("<c:serAx>"));
		break;
	}
	sb->AppendC(UTF8STRC("<c:axId val=\""));
	sb->AppendUOSInt(index);
	sb->AppendC(UTF8STRC("\"/>"));
	sb->AppendC(UTF8STRC("<c:scaling>"));
	sb->AppendC(UTF8STRC("<c:orientation val=\"minMax\"/>"));
	sb->AppendC(UTF8STRC("</c:scaling>"));
	sb->AppendC(UTF8STRC("<c:delete val=\"false\"/>"));
	switch (axis->GetAxisPos())
	{
	case AxisPosition::Left:
		sb->AppendC(UTF8STRC("<c:axPos val=\"l\"/>"));
		break;
	case AxisPosition::Top:
		sb->AppendC(UTF8STRC("<c:axPos val=\"t\"/>"));
		break;
	case AxisPosition::Right:
		sb->AppendC(UTF8STRC("<c:axPos val=\"r\"/>"));
		break;
	case AxisPosition::Bottom:
		sb->AppendC(UTF8STRC("<c:axPos val=\"b\"/>"));
		break;
	}
	if (axis->GetMajorGridProp())
	{
		sb->AppendC(UTF8STRC("<c:majorGridlines>"));
		AppendShapeProp(sb, axis->GetMajorGridProp());
		sb->AppendC(UTF8STRC("</c:majorGridlines>"));
	}
	if (axis->GetTitle())
	{
		AppendTitle(sb, axis->GetTitle());
	}
	sb->AppendC(UTF8STRC("<c:majorTickMark val=\"cross\"/>"));
	sb->AppendC(UTF8STRC("<c:minorTickMark val=\"none\"/>"));
	switch (axis->GetTickLblPos())
	{
	case TickLabelPosition::High:
		sb->AppendC(UTF8STRC("<c:tickLblPos val=\"high\"/>"));
		break;
	case TickLabelPosition::Low:
		sb->AppendC(UTF8STRC("<c:tickLblPos val=\"low\"/>"));
		break;
	case TickLabelPosition::NextTo:
		sb->AppendC(UTF8STRC("<c:tickLblPos val=\"nextTo\"/>"));
		break;
	case TickLabelPosition::None:
		sb->AppendC(UTF8STRC("<c:tickLblPos val=\"none\"/>"));
		break;
	}
	AppendShapeProp(sb, axis->GetShapeProp());
// 	sb->AppendC(UTF8STRC("<c:crossAx val=\"1\"/>");
	sb->AppendC(UTF8STRC("<c:crosses val=\"autoZero\"/>"));
	sb->AppendC(UTF8STRC("<c:crossBetween val=\"midCat\"/>"));
	switch (axis->GetAxisType())
	{
	case AxisType::Category:
		sb->AppendC(UTF8STRC("</c:catAx>"));
		break;
	case AxisType::Date:
		sb->AppendC(UTF8STRC("</c:dateAx>"));
		break;
	case AxisType::Numeric:
		sb->AppendC(UTF8STRC("</c:valAx>"));
		break;
	case AxisType::Series:
		sb->AppendC(UTF8STRC("</c:serAx>"));
		break;
	}
}

void Exporter::XLSXExporter::AppendSeries(Text::StringBuilderUTF *sb, Text::SpreadSheet::OfficeChartSeries *series, UOSInt index)
{
	Text::String *s;
	sb->AppendC(UTF8STRC("<c:ser>"));
	sb->AppendC(UTF8STRC("<c:idx val=\""));
	sb->AppendUOSInt(index);
	sb->AppendC(UTF8STRC("\"/>"));
	sb->AppendC(UTF8STRC("<c:order val=\""));
	sb->AppendUOSInt(index);
	sb->AppendC(UTF8STRC("\"/>"));
	if (series->GetTitle())
	{
		sb->AppendC(UTF8STRC("<c:tx>"));
		sb->AppendC(UTF8STRC("<c:v>"));
		s = Text::XML::ToNewXMLText(series->GetTitle()->v);
		sb->Append(s);
		s->Release();
		sb->AppendC(UTF8STRC("</c:v>"));
		sb->AppendC(UTF8STRC("</c:tx>"));
	}
	AppendShapeProp(sb, series->GetShapeProp());
	sb->AppendC(UTF8STRC("<c:marker>"));
	switch (series->GetMarkerStyle())
	{
	case MarkerStyle::Circle:
		sb->AppendC(UTF8STRC("<c:symbol val=\"circle\"/>"));
		break;
	case MarkerStyle::Dash:
		sb->AppendC(UTF8STRC("<c:symbol val=\"dash\"/>"));
		break;
	case MarkerStyle::Diamond:
		sb->AppendC(UTF8STRC("<c:symbol val=\"diamond\"/>"));
		break;
	case MarkerStyle::Dot:
		sb->AppendC(UTF8STRC("<c:symbol val=\"dot\"/>"));
		break;
	case MarkerStyle::None:
		sb->AppendC(UTF8STRC("<c:symbol val=\"none\"/>"));
		break;
	case MarkerStyle::Picture:
		sb->AppendC(UTF8STRC("<c:symbol val=\"picture\"/>"));
		break;
	case MarkerStyle::Plus:
		sb->AppendC(UTF8STRC("<c:symbol val=\"plus\"/>"));
		break;
	case MarkerStyle::Square:
		sb->AppendC(UTF8STRC("<c:symbol val=\"square\"/>"));
		break;
	case MarkerStyle::Star:
		sb->AppendC(UTF8STRC("<c:symbol val=\"star\"/>"));
		break;
	case MarkerStyle::Triangle:
		sb->AppendC(UTF8STRC("<c:symbol val=\"triangle\"/>"));
		break;
	case MarkerStyle::X:
		sb->AppendC(UTF8STRC("<c:symbol val=\"x\"/>"));
		break;
	}
	if (series->GetMarkerSize() != 0)
	{
		sb->AppendC(UTF8STRC("<c:size val=\""));
		sb->AppendU32(series->GetMarkerSize());
		sb->AppendC(UTF8STRC("\"/>"));
	}
	sb->AppendC(UTF8STRC("</c:marker>"));

	UTF8Char sbuff[128];
	WorkbookDataSource *catData = series->GetCategoryData();
	sb->AppendC(UTF8STRC("<c:cat>"));
	sb->AppendC(UTF8STRC("<c:strRef>"));
	sb->AppendC(UTF8STRC("<c:f>"));
	catData->ToCodeRange(sbuff);
	s = Text::XML::ToNewXMLText(sbuff);
	sb->Append(s);
	s->Release();
	sb->AppendC(UTF8STRC("</c:f>"));
	sb->AppendC(UTF8STRC("<c:strCache/>"));
	sb->AppendC(UTF8STRC("</c:strRef>"));
	sb->AppendC(UTF8STRC("</c:cat>"));

	WorkbookDataSource *valData = series->GetValueData();
	sb->AppendC(UTF8STRC("<c:val>"));
	sb->AppendC(UTF8STRC("<c:numRef>"));
	sb->AppendC(UTF8STRC("<c:f>"));
	valData->ToCodeRange(sbuff);
	s = Text::XML::ToNewXMLText(sbuff);
	sb->Append(s);
	s->Release();
	sb->AppendC(UTF8STRC("</c:f>"));
	sb->AppendC(UTF8STRC("<c:numCache/>"));
	sb->AppendC(UTF8STRC("</c:numRef>"));
	sb->AppendC(UTF8STRC("</c:val>"));

	sb->AppendC(UTF8STRC("<c:smooth val=\""));
	sb->Append((const UTF8Char*)(series->IsSmooth()?"true":"false"));
	sb->AppendC(UTF8STRC("\"/>"));
	sb->AppendC(UTF8STRC("</c:ser>"));
}

void Exporter::XLSXExporter::AppendBorder(Text::StringBuilderUTF *sb, Text::SpreadSheet::CellStyle::BorderStyle *border, const UTF8Char *name)
{
	sb->AppendChar('<', 1);
	sb->Append(name);
	if (border->borderType == BorderType::None)
	{
		sb->AppendC(UTF8STRC("/>"));
	}
	else
	{
		switch (border->borderType)
		{
			case BorderType::Thin:
				sb->AppendC(UTF8STRC(" style=\"thin\">"));
				break;
			case BorderType::Medium:
				sb->AppendC(UTF8STRC(" style=\"medium\">"));
				break;
			case BorderType::Dashed:
				sb->AppendC(UTF8STRC(" style=\"dashed\">"));
				break;
			case BorderType::Dotted:
				sb->AppendC(UTF8STRC(" style=\"dotted\">"));
				break;
			case BorderType::Thick:
				sb->AppendC(UTF8STRC(" style=\"thick\">"));
				break;
			case BorderType::DOUBLE:
				sb->AppendC(UTF8STRC(" style=\"double\">"));
				break;
			case BorderType::Hair:
				sb->AppendC(UTF8STRC(" style=\"hair\">"));
				break;
			case BorderType::MediumDashed:
				sb->AppendC(UTF8STRC(" style=\"mediumDashed\">"));
				break;
			case BorderType::DashDot:
				sb->AppendC(UTF8STRC(" style=\"dashDot\">"));
				break;
			case BorderType::MediumDashDot:
				sb->AppendC(UTF8STRC(" style=\"mediumDashDot\">"));
				break;
			case BorderType::DashDotDot:
				sb->AppendC(UTF8STRC(" style=\"dashDotDot\">"));
				break;
			case BorderType::MediumDashDotDot:
				sb->AppendC(UTF8STRC(" style=\"mediumDashDotDot\">"));
				break;
			case BorderType::SlantedDashDot:
				sb->AppendC(UTF8STRC(" style=\"slantDashDot\">"));
				break;
			case BorderType::None:
				break;
		}
		sb->AppendC(UTF8STRC("<color rgb=\""));
		sb->AppendHex32(border->borderColor);
		sb->AppendC(UTF8STRC("\"/>"));
		sb->AppendC(UTF8STRC("</"));
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
