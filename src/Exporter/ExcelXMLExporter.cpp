#include "Stdafx.h"
#include "MyMemory.h"
#include "Exporter/ExcelXMLExporter.h"
#include "IO/StreamWriter.h"
#include "Math/Math.h"
#include "Media/DrawEngine.h"
#include "Text/MyString.h"
#include "Text/Encoding.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XML.h"
#include "Text/SpreadSheet/Workbook.h"

Exporter::ExcelXMLExporter::ExcelXMLExporter()
{
	this->codePage = 0;
}

Exporter::ExcelXMLExporter::~ExcelXMLExporter()
{
}

Int32 Exporter::ExcelXMLExporter::GetName()
{
	return *(Int32*)"EXML";
}

IO::FileExporter::SupportType Exporter::ExcelXMLExporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	if (pobj->GetParserType() != IO::ParserType::Workbook)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	return IO::FileExporter::SupportType::NormalStream;
}

Bool Exporter::ExcelXMLExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcat(nameBuff, (const UTF8Char*)"Excel XML File");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.xml");
		return true;
	}
	return false;
}

void Exporter::ExcelXMLExporter::SetCodePage(UInt32 codePage)
{
	this->codePage = codePage;
}

Bool Exporter::ExcelXMLExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
{
	UTF8Char sbuff[256];
	UTF8Char sbuff2[256];
	UInt32 pal[56];
	UInt32 defPal[56];
	Bool found;
	IO::StreamWriter *writer;
	if (pobj->GetParserType() != IO::ParserType::Workbook)
	{
		return false;
	}
	Text::SpreadSheet::Workbook *wb;
	Text::SpreadSheet::Worksheet *ws;
	Text::SpreadSheet::CellStyle *style;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	UOSInt m;
	UOSInt n;
	UInt32 v;
	const UTF8Char *text;
	Text::String *s;
	Data::DateTime *dt;
	Double ver;
	Text::StringBuilderUTF8 sb;
	Text::Encoding enc(this->codePage);

	NEW_CLASS(writer, IO::StreamWriter(stm, &enc));
	wb = (Text::SpreadSheet::Workbook*)pobj;
	writer->WriteSignature();

	sb.AppendC(UTF8STRC("<?xml version=\"1.0\" encoding="));
	Text::EncodingFactory::GetInternetName(sbuff, enc.GetEncCodePage());
	Text::XML::ToAttrText(sbuff2, sbuff);
	sb.Append(sbuff2);
	sb.AppendC(UTF8STRC("?>"));
	writer->WriteLine(sb.ToString());

	writer->WriteLine((const UTF8Char*)"<?mso-application progid=\"Excel.Sheet\"?>");
	writer->WriteLine((const UTF8Char*)"<Workbook xmlns=\"urn:schemas-microsoft-com:office:spreadsheet\"");
	writer->WriteLine((const UTF8Char*)" xmlns:o=\"urn:schemas-microsoft-com:office:office\"");
	writer->WriteLine((const UTF8Char*)" xmlns:x=\"urn:schemas-microsoft-com:office:excel\"");
	writer->WriteLine((const UTF8Char*)" xmlns:ss=\"urn:schemas-microsoft-com:office:spreadsheet\"");
	writer->WriteLine((const UTF8Char*)" xmlns:html=\"http://www.w3.org/TR/REC-html40\">");

	if (wb->HasInfo())
	{
		writer->WriteLine((const UTF8Char*)" <DocumentProperties xmlns=\"urn:schemas-microsoft-com:office:office\">");
		text = wb->GetAuthor();
		if (text)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("  <Author>"));
			s = Text::XML::ToNewXMLText(text);
			sb.Append(s);
			sb.AppendC(UTF8STRC("</Author>"));
			s->Release();
			writer->WriteLine(sb.ToString());
		}
		text = wb->GetLastAuthor();
		if (text)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("  <LastAuthor>"));
			s = Text::XML::ToNewXMLText(text);
			sb.Append(s);
			sb.AppendC(UTF8STRC("</LastAuthor>"));
			s->Release();
			writer->WriteLine(sb.ToString());
		}
		dt = wb->GetCreateTime();
		if (dt)
		{
			dt->ToString(sbuff, "yyyy-MM-ddTHH:mm:ssZ");
			sb.ClearStr();
			sb.AppendC(UTF8STRC("  <Created>"));
			sb.Append(sbuff);
			sb.AppendC(UTF8STRC("</Created>"));
			writer->WriteLine(sb.ToString());
		}
		dt = wb->GetModifyTime();
		if (dt)
		{
			dt->ToString(sbuff, "yyyy-MM-ddTHH:mm:ssZ");
			sb.ClearStr();
			sb.AppendC(UTF8STRC("  <LastSaved>"));
			sb.Append(sbuff);
			sb.AppendC(UTF8STRC("</LastSaved>"));
			writer->WriteLine(sb.ToString());
		}
		text = wb->GetCompany();
		if (text)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("  <Company>"));
			s = Text::XML::ToNewXMLText(text);
			sb.Append(s);
			sb.AppendC(UTF8STRC("</Company>"));
			s->Release();
			writer->WriteLine(sb.ToString());
		}
		ver = wb->GetVersion();
		if (ver)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("  <Version>"));
			Text::SBAppendF64(&sb, ver);
			sb.AppendC(UTF8STRC("</Version>"));
			writer->WriteLine(sb.ToString());
		}
		writer->WriteLine((const UTF8Char*)" </DocumentProperties>");
	}

	if (wb->HasWindowInfo() != 0)
	{
		writer->WriteLine((const UTF8Char*)" <ExcelWorkbook xmlns=\"urn:schemas-microsoft-com:office:excel\">");
		if (wb->GetWindowHeight() != 0)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("  <WindowHeight>"));
			sb.AppendI32(wb->GetWindowHeight());
			sb.AppendC(UTF8STRC("</WindowHeight>"));
			writer->WriteLine(sb.ToString());
		}
		if (wb->GetWindowWidth() != 0)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("  <WindowWidth>"));
			sb.AppendI32(wb->GetWindowWidth());
			sb.AppendC(UTF8STRC("</WindowWidth>"));
			writer->WriteLine(sb.ToString());
		}
		if (wb->GetWindowTopX() != 0)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("  <WindowTopX>"));
			sb.AppendI32(wb->GetWindowTopX());
			sb.AppendC(UTF8STRC("</WindowTopX>"));
			writer->WriteLine(sb.ToString());
		}
		if (wb->GetWindowTopY() != 0)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("  <WindowTopY>"));
			sb.AppendI32(wb->GetWindowTopY());
			sb.AppendC(UTF8STRC("</WindowTopY>"));
			writer->WriteLine(sb.ToString());
		}
		if (wb->GetActiveSheet() != 0)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("  <ActiveSheet>"));
			sb.AppendUOSInt(wb->GetActiveSheet());
			sb.AppendC(UTF8STRC("</ActiveSheet>"));
			writer->WriteLine(sb.ToString());
		}
		writer->WriteLine((const UTF8Char*)" </ExcelWorkbook>");
	}

	wb->GetPalette(pal);
	Text::SpreadSheet::Workbook::GetDefPalette(defPal);
	found = false;
	i = 0;
	j = 56;
	while (i < j)
	{
		if (pal[i] != defPal[i])
		{
			if (!found)
			{
				found = true;
				writer->WriteLine((const UTF8Char*)" <OfficeDocumentSettings xmlns=\"urn:schemas-microsoft-com:office:office\">");
				writer->WriteLine((const UTF8Char*)"  <Colors>");
			}
			writer->WriteLine((const UTF8Char*)"   <Color>");
			sb.ClearStr();
			sb.AppendC(UTF8STRC("    <Index>"));
			sb.AppendUOSInt(i);
			sb.AppendC(UTF8STRC("</Index>"));
			writer->WriteLine(sb.ToString());
			sb.ClearStr();
			sb.AppendC(UTF8STRC("    <RGB>#"));
			sb.AppendHex24(pal[i]);
			sb.AppendC(UTF8STRC("</RGB>"));
			writer->WriteLine(sb.ToString());
			writer->WriteLine((const UTF8Char*)"   </Color>");
		}
		i++;
	}
	if (found)
	{
		writer->WriteLine((const UTF8Char*)"  </Colors>");
		writer->WriteLine((const UTF8Char*)" </OfficeDocumentSettings>");
	}

	if (wb->HasCellStyle())
	{
		writer->WriteLine((const UTF8Char*)" <Styles>");
		i = 0;
		j = wb->GetStyleCount();
		while (i < j)
		{
			style = wb->GetStyle(i);
			if (style)
			{
				if (i == 0)
				{
					style->SetID((const UTF8Char*)"Default");
				}
				else
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("s"));
					sb.AppendUOSInt((20 + i));
					style->SetID(sb.ToString());
				}
				sb.ClearStr();
				sb.AppendC(UTF8STRC("  <Style ss:ID="));
				s = Text::XML::ToNewAttrText(style->GetID());
				sb.Append(s);
				s->Release();
				sb.AppendC(UTF8STRC(">"));
				writer->WriteLine(sb.ToString());

				if (style->GetHAlign() != Text::SpreadSheet::HAlignment::Unknown || (style->GetVAlign() != Text::SpreadSheet::VAlignment::Center && style->GetVAlign() != Text::SpreadSheet::VAlignment::Unknown) || style->GetWordWrap())
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("   <Alignment"));
					switch (style->GetHAlign())
					{
					case Text::SpreadSheet::HAlignment::Left:
						sb.AppendC(UTF8STRC(" ss:Horizontal=\"Left\""));
						break;
					case Text::SpreadSheet::HAlignment::Center:
						sb.AppendC(UTF8STRC(" ss:Horizontal=\"Center\""));
						break;
					case Text::SpreadSheet::HAlignment::Right:
						sb.AppendC(UTF8STRC(" ss:Horizontal=\"Right\""));
						break;
					case Text::SpreadSheet::HAlignment::Fill:
						sb.AppendC(UTF8STRC(" ss:Horizontal=\"Fill\""));
						break;
					case Text::SpreadSheet::HAlignment::Justify:
						sb.AppendC(UTF8STRC(" ss:Horizontal=\"Justify\""));
						break;
					case Text::SpreadSheet::HAlignment::Unknown:
					default:
						break;
					}
					switch (style->GetVAlign())
					{
					case Text::SpreadSheet::VAlignment::Top:
						sb.AppendC(UTF8STRC(" ss:Vertical=\"Top\""));
						break;
					case Text::SpreadSheet::VAlignment::Center:
						//sb.AppendC(UTF8STRC(" ss:Vertical=\"Center\""));
						break;
					case Text::SpreadSheet::VAlignment::Bottom:
						sb.AppendC(UTF8STRC(" ss:Vertical=\"Bottom\""));
						break;
					case Text::SpreadSheet::VAlignment::Justify:
						sb.AppendC(UTF8STRC(" ss:Vertical=\"Justify\""));
						break;
					case Text::SpreadSheet::VAlignment::Unknown:
					default:
						break;
					}
					if (style->GetWordWrap())
					{
						sb.AppendC(UTF8STRC(" ss:WrapText=\"1\""));
					}
					sb.AppendC(UTF8STRC("/>"));
					writer->WriteLine(sb.ToString());
				}
				if (style->GetBorderLeft()->borderType != Text::SpreadSheet::BorderType::None ||
					style->GetBorderTop()->borderType != Text::SpreadSheet::BorderType::None ||
					style->GetBorderRight()->borderType != Text::SpreadSheet::BorderType::None ||
					style->GetBorderBottom()->borderType != Text::SpreadSheet::BorderType::None)
				{
					Text::SpreadSheet::CellStyle::BorderStyle *border;
					writer->WriteLine((const UTF8Char*)"   <Borders>");
					border = style->GetBorderBottom();
					if (border->borderType != Text::SpreadSheet::BorderType::None)
					{
						WriteBorderStyle(writer, (const UTF8Char*)"Bottom", border);
					}
					border = style->GetBorderLeft();
					if (border->borderType != Text::SpreadSheet::BorderType::None)
					{
						WriteBorderStyle(writer, (const UTF8Char*)"Left", border);
					}
					border = style->GetBorderRight();
					if (border->borderType != Text::SpreadSheet::BorderType::None)
					{
						WriteBorderStyle(writer, (const UTF8Char*)"Right", border);
					}
					border = style->GetBorderTop();
					if (border->borderType != Text::SpreadSheet::BorderType::None)
					{
						WriteBorderStyle(writer, (const UTF8Char*)"Top", border);
					}
					writer->WriteLine((const UTF8Char*)"   </Borders>");
				}

				if ((style->GetFillColor() & 0xffffff) != 0xffffff)
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("   <Interior ss:Color=\"#"));
					Text::StrHexVal32(sbuff, style->GetFillColor());
					sb.Append(&sbuff[2]);
					sb.AppendC(UTF8STRC("\" ss:Pattern=\"Solid\"/>"));
					writer->WriteLine(sb.ToString());
				}

				if (style->GetFont() != 0)
				{
					Text::SpreadSheet::WorkbookFont *font = style->GetFont();
					sb.ClearStr();
					sb.AppendC(UTF8STRC("   <Font ss:FontName="));
					s = Text::XML::ToNewAttrText(font->GetName());
					sb.Append(s);
					s->Release();
					if (font->GetSize() > 0)
					{
						sb.AppendC(UTF8STRC(" ss:Size=\""));
						Text::SBAppendF64(&sb, font->GetSize());
						sb.AppendC(UTF8STRC("\""));
					}
					if ((font->GetColor() & 0xffffff) != 0)
					{
						Text::StrHexVal32(sbuff, font->GetColor());
						sb.AppendC(UTF8STRC(" ss:Color=\"#"));
						sb.Append(&sbuff[2]);
						sb.AppendC(UTF8STRC("\""));
					}
					if (font->IsBold())
					{
						sb.AppendC(UTF8STRC(" ss:Bold=\"1\""));
					}
					sb.AppendC(UTF8STRC("/>"));
					writer->WriteLine(sb.ToString());
				}

				if (style->GetDataFormat() != 0)
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("   <NumberFormat ss:Format="));
					s = Text::XML::ToNewAttrText(style->GetDataFormat());
					sb.Append(s);
					s->Release();
					sb.AppendC(UTF8STRC("/>"));
					writer->WriteLine(sb.ToString());
				}

				writer->WriteLine((const UTF8Char*)"  </Style>");
			}
			i++;
		}
		writer->WriteLine((const UTF8Char*)" </Styles>");
	}

	i = 0;
	j = wb->GetCount();
	while (i < j)
	{
		ws = wb->GetItem(i);
		sb.ClearStr();
		sb.AppendC(UTF8STRC(" <Worksheet ss:Name="));
		s = Text::XML::ToNewAttrText(ws->GetName()->v);
		sb.Append(s);
		sb.AppendC(UTF8STRC(">"));
		s->Release();
		writer->WriteLine(sb.ToString());

		if (ws->GetCount() > 0)
		{
			Bool rowSkipped = false;
			UOSInt lastDispCol;
			Text::SpreadSheet::Worksheet::RowData *row;
			Text::SpreadSheet::Worksheet::CellData *cell;
			writer->WriteLine((const UTF8Char*)"  <Table>");

			Double lastColWidth = -1;
			UOSInt lastColIndex = INVALID_INDEX;
			Bool needIndex = true;

			k = 0;
			l = ws->GetColWidthCount();
			while (k < l)
			{
				if (ws->GetColWidthPt(k) != lastColWidth)
				{
					if (lastColWidth >= 0)
					{
						sb.ClearStr();
						sb.AppendC(UTF8STRC("   <Column"));
						if (needIndex)
						{
							sb.AppendC(UTF8STRC(" ss:Index=\""));
							sb.AppendUOSInt(lastColIndex + 1);
							sb.AppendC(UTF8STRC("\""));
						}
						sb.AppendC(UTF8STRC(" ss:AutoFitWidth=\"0\" ss:Width=\""));
						Text::SBAppendF64(&sb, Math::Double2Int32(lastColWidth * 4) * 0.25);
						sb.AppendC(UTF8STRC("\""));
						if (lastColIndex + 1 != k)
						{
							sb.AppendC(UTF8STRC(" ss:Span=\""));
							sb.AppendUOSInt((k - lastColIndex - 1));
							sb.AppendC(UTF8STRC("\""));
							needIndex = true;
						}
						else
						{
							needIndex = false;
						}
						sb.AppendC(UTF8STRC("/>"));
						writer->WriteLine(sb.ToString());
					}
					else if (k == 1)
					{
						needIndex = false;
					}
					lastColWidth = ws->GetColWidth(k, Math::Unit::Distance::DU_POINT);
					lastColIndex = k;
				}
				k++;
			}
			if (lastColWidth >= 0)
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("   <Column"));
				if (needIndex)
				{
					sb.AppendC(UTF8STRC(" ss:Index=\""));
					sb.AppendUOSInt(lastColIndex);
					sb.AppendC(UTF8STRC("\""));
				}
				sb.AppendC(UTF8STRC(" ss:AutoFitWidth=\"0\" ss:Width=\""));
				Text::SBAppendF64(&sb, Math::Double2Int32(lastColWidth * 4) * 0.25);
				sb.AppendC(UTF8STRC("\""));
				if (lastColIndex + 1 != l)
				{
					sb.AppendC(UTF8STRC(" ss:Span=\""));
					sb.AppendUOSInt((l - lastColIndex - 1));
					sb.AppendC(UTF8STRC("\""));
				}
				sb.AppendC(UTF8STRC("/>"));
				writer->WriteLine(sb.ToString());
			}

			k = 0;
			l = ws->GetCount();
			while (k < l)
			{
				row = ws->GetItem(k);
				if (row == 0)
				{
					rowSkipped = true;
				}
				else
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("   <Row"));
					if (rowSkipped)
					{
						sb.AppendC(UTF8STRC(" ss:Index=\""));
						sb.AppendUOSInt(k + 1);
						sb.AppendC(UTF8STRC("\""));
					}
					rowSkipped = false;

					cell = row->cells->GetItem(0);
					if (cell)
					{
						if (cell->hidden)
						{
							sb.AppendC(UTF8STRC(" ss:Hidden=\"1\""));
						}
					}
					if (row->height >= 0)
					{
						sb.AppendC(UTF8STRC(" ss:AutoFitHeight=\"0\" ss:Height=\""));
						if (row->height == 0)
						{
							Text::SBAppendF64(&sb, row->height);
						}
						else
						{
							Text::SBAppendF64(&sb, row->height + 0.05);
						}
						sb.AppendC(UTF8STRC("\""));
					}
					if (row->cells->GetCount() == 0)
					{
						sb.AppendC(UTF8STRC("/>"));
						writer->WriteLine(sb.ToString());
					}
					else
					{
						sb.AppendC(UTF8STRC(">"));
						writer->WriteLine(sb.ToString());

						lastDispCol = INVALID_INDEX;
						m = 0;
						n = row->cells->GetCount();
						while (m < n)
						{
							cell = row->cells->GetItem(m);
							if (cell == 0)
							{
							}
							else if (cell->cdt == Text::SpreadSheet::CellDataType::MergedLeft)
							{
							}
							else if (cell->cdt == Text::SpreadSheet::CellDataType::MergedTop)
							{
							}
							else
							{
								sb.ClearStr();
								sb.AppendC(UTF8STRC("    <Cell"));
								if (m != lastDispCol + 1)
								{
									sb.AppendC(UTF8STRC(" ss:Index=\""));
									sb.AppendUOSInt(m + 1);
									sb.AppendC(UTF8STRC("\""));
								}
								lastDispCol = m;
								if (cell->mergeVert > 1)
								{
									sb.AppendC(UTF8STRC(" ss:MergeDown=\""));
									sb.AppendU32((cell->mergeVert - 1));
									sb.AppendC(UTF8STRC("\""));
								}
								if (cell->mergeHori > 1)
								{
									sb.AppendC(UTF8STRC(" ss:MergeAcross=\""));
									sb.AppendU32((cell->mergeHori - 1));
									sb.AppendC(UTF8STRC("\""));
									lastDispCol += cell->mergeHori - 1;
								}
								if (cell->style)
								{
									text = cell->style->GetID();
									if (text)
									{
										sb.AppendC(UTF8STRC(" ss:StyleID="));
										s = Text::XML::ToNewAttrText(text);
										sb.Append(s);
										s->Release();
									}
								}
								if (cell->cellURL)
								{
									 sb.AppendC(UTF8STRC(" ss:HRef="));
									 s = Text::XML::ToNewAttrText(cell->cellURL->v);
									 sb.Append(s);
									 s->Release();
								}
								sb.AppendC(UTF8STRC(">"));
								if (cell->cellValue)
								{
									switch (cell->cdt)
									{
									case Text::SpreadSheet::CellDataType::Number:
										sb.AppendC(UTF8STRC("<Data ss:Type=\"Number\">"));
										s = Text::XML::ToNewXMLText(cell->cellValue->v);
										sb.Append(s);
										s->Release();
										sb.AppendC(UTF8STRC("</Data>"));
										break;
									case Text::SpreadSheet::CellDataType::DateTime:
										sb.AppendC(UTF8STRC("<Data ss:Type=\"DateTime\">"));
										s = Text::XML::ToNewXMLText(cell->cellValue->v);
										sb.Append(s);
										s->Release();
										sb.AppendC(UTF8STRC("</Data>"));
										break;
									case Text::SpreadSheet::CellDataType::MergedLeft:
									case Text::SpreadSheet::CellDataType::MergedTop:
									case Text::SpreadSheet::CellDataType::String:
									default:
										sb.AppendC(UTF8STRC("<Data ss:Type=\"String\">"));
										s = Text::XML::ToNewXMLText(cell->cellValue->v);
										sb.Append(s);
										s->Release();
										sb.AppendC(UTF8STRC("</Data>"));
										break;
									}
								}
								sb.AppendC(UTF8STRC("</Cell>"));
								writer->WriteLine(sb.ToString());
							}
							m++;
						}

						writer->WriteLine((const UTF8Char*)"   </Row>");
					}
				}
				k++;
			}
			writer->WriteLine((const UTF8Char*)"  </Table>");
		}
		else
		{
			writer->WriteLine((const UTF8Char*)"  <Table x:FullColumns=\"1\" x:FullRows=\"1\"/>");
		}

		found = false;
		if (!ws->IsDefaultPageSetup())
		{
			if (!found)
			{
				writer->WriteLine((const UTF8Char*)"  <WorksheetOptions xmlns=\"urn:schemas-microsoft-com:office:excel\">");
				found = true;
			}
			writer->WriteLine((const UTF8Char*)"   <PageSetup>");
			if (ws->GetMarginHeader() != 1.3)
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("    <Header x:Margin=\""));
				Text::SBAppendF64(&sb, ws->GetMarginHeader());
				sb.AppendC(UTF8STRC("\"/>"));
				writer->WriteLine(sb.ToString());
			}
			if (ws->GetMarginFooter() != 1.3)
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("    <Footer x:Margin=\""));
				Text::SBAppendF64(&sb, ws->GetMarginFooter());
				sb.AppendC(UTF8STRC("\"/>"));
				writer->WriteLine(sb.ToString());
			}
			if (ws->GetMarginTop() != 2.5 || ws->GetMarginBottom() != 2.5 || ws->GetMarginLeft() != 2.0 || ws->GetMarginBottom() != 2.0)
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("    <PageMargins x:Bottom=\""));
				Text::SBAppendF64(&sb, ws->GetMarginBottom());
				sb.AppendC(UTF8STRC("\" x:Left=\""));
				Text::SBAppendF64(&sb, ws->GetMarginLeft());
				sb.AppendC(UTF8STRC("\" x:Right=\""));
				Text::SBAppendF64(&sb, ws->GetMarginRight());
				sb.AppendC(UTF8STRC("\" x:Top=\""));
				Text::SBAppendF64(&sb, ws->GetMarginTop());
				sb.AppendC(UTF8STRC("\"/>"));
				writer->WriteLine(sb.ToString());

			}
			writer->WriteLine((const UTF8Char*)"   </PageSetup>");
		}
		if ((v = ws->GetZoom()) != 0)
		{
			if (!found)
			{
				writer->WriteLine((const UTF8Char*)"  <WorksheetOptions xmlns=\"urn:schemas-microsoft-com:office:excel\">");
				found = true;
			}
			sb.ClearStr();
			sb.AppendC(UTF8STRC("   <Zoom>"));
			sb.AppendUOSInt(v);
			sb.AppendC(UTF8STRC("</Zoom>"));
			writer->WriteLine(sb.ToString());
		}
		if ((v = ws->GetFreezeHori()) != 0)
		{
			if (!found)
			{
				writer->WriteLine((const UTF8Char*)"  <WorksheetOptions xmlns=\"urn:schemas-microsoft-com:office:excel\">");
				found = true;
			}
			sb.ClearStr();
			sb.AppendC(UTF8STRC("   <SplitVertical>"));
			sb.AppendU32(v);
			sb.AppendC(UTF8STRC("</SplitVertical>"));
			writer->WriteLine(sb.ToString());

			sb.ClearStr();
			sb.AppendC(UTF8STRC("   <LeftColumnRightPane>"));
			sb.AppendU32(v);
			sb.AppendC(UTF8STRC("</LeftColumnRightPane>"));
			writer->WriteLine(sb.ToString());
		}
		if ((v = ws->GetFreezeVert()) != 0)
		{
			if (!found)
			{
				writer->WriteLine((const UTF8Char*)"  <WorksheetOptions xmlns=\"urn:schemas-microsoft-com:office:excel\">");
				found = true;
			}
			sb.ClearStr();
			sb.AppendC(UTF8STRC("   <SplitHorizontal>"));
			sb.AppendU32(v);
			sb.AppendC(UTF8STRC("</SplitHorizontal>"));
			writer->WriteLine(sb.ToString());

			sb.ClearStr();
			sb.AppendC(UTF8STRC("   <TopRowBottomPane>"));
			sb.AppendU32(v);
			sb.AppendC(UTF8STRC("</TopRowBottomPane>"));
			writer->WriteLine(sb.ToString());
		}
		v = ws->GetOptions();
		if (v & 0x8)
		{
			if (!found)
			{
				writer->WriteLine((const UTF8Char*)"  <WorksheetOptions xmlns=\"urn:schemas-microsoft-com:office:excel\">");
				found = true;
			}
			writer->WriteLine((const UTF8Char*)"   <FreezePanes/>");
		}
		if (v & 0x100)
		{
			if (!found)
			{
				writer->WriteLine((const UTF8Char*)"  <WorksheetOptions xmlns=\"urn:schemas-microsoft-com:office:excel\">");
				found = true;
			}
			writer->WriteLine((const UTF8Char*)"   <FrozenNoSplit/>");
		}
		if (v & 0x200)
		{
			if (!found)
			{
				writer->WriteLine((const UTF8Char*)"  <WorksheetOptions xmlns=\"urn:schemas-microsoft-com:office:excel\">");
				found = true;
			}
			writer->WriteLine((const UTF8Char*)"   <Selected/>");
		}
		if ((v & 0x2) == 0)
		{
			if (!found)
			{
				writer->WriteLine((const UTF8Char*)"  <WorksheetOptions xmlns=\"urn:schemas-microsoft-com:office:excel\">");
				found = true;
			}
			writer->WriteLine((const UTF8Char*)"   <DoNotDisplayGridlines/>");
		}
		if ((v & 0x80) == 0)
		{
			if (!found)
			{
				writer->WriteLine((const UTF8Char*)"  <WorksheetOptions xmlns=\"urn:schemas-microsoft-com:office:excel\">");
				found = true;
			}
			writer->WriteLine((const UTF8Char*)"   <DoNotDisplayOutline/>");
		}
		if (found)
		{
			writer->WriteLine((const UTF8Char*)"  </WorksheetOptions>");
		}
		writer->WriteLine((const UTF8Char*)" </Worksheet>");
		i++;
	}
	writer->WriteLine((const UTF8Char*)"</Workbook>");
	DEL_CLASS(writer);
	return true;
}

void Exporter::ExcelXMLExporter::WriteBorderStyle(IO::Writer *writer, const UTF8Char *position, Text::SpreadSheet::CellStyle::BorderStyle *border)
{
	UTF8Char sbuff[10];
	Text::StringBuilderUTF8 sb;
	Text::String *s;
	sb.AppendC(UTF8STRC("    <Border ss:Position="));
	s = Text::XML::ToNewAttrText(position);
	sb.Append(s);
	s->Release();
	if (border->borderType == Text::SpreadSheet::BorderType::Thin)
	{
		sb.AppendC(UTF8STRC(" ss:LineStyle=\"Continuous\" ss:Weight=\"1\""));
	}
	else if (border->borderType == Text::SpreadSheet::BorderType::Hair)
	{
		sb.AppendC(UTF8STRC(" ss:LineStyle=\"Continuous\""));
	}
	else if (border->borderType == Text::SpreadSheet::BorderType::Dotted)
	{
		sb.AppendC(UTF8STRC(" ss:LineStyle=\"Dot\" ss:Weight=\"1\""));
	}
	else if (border->borderType == Text::SpreadSheet::BorderType::Dashed)
	{
		sb.AppendC(UTF8STRC(" ss:LineStyle=\"Dash\" ss:Weight=\"1\""));
	}
	else if (border->borderType == Text::SpreadSheet::BorderType::DashDot)
	{
		sb.AppendC(UTF8STRC(" ss:LineStyle=\"DashDot\" ss:Weight=\"1\""));
	}
	else if (border->borderType == Text::SpreadSheet::BorderType::DashDotDot)
	{
		sb.AppendC(UTF8STRC(" ss:LineStyle=\"DashDotDot\" ss:Weight=\"1\""));
	}
	else if (border->borderType == Text::SpreadSheet::BorderType::DOUBLE)
	{
		sb.AppendC(UTF8STRC(" ss:LineStyle=\"Double\" ss:Weight=\"3\""));
	}
	else if (border->borderType == Text::SpreadSheet::BorderType::Medium)
	{
		sb.AppendC(UTF8STRC(" ss:LineStyle=\"Continuous\" ss:Weight=\"2\""));
	}
	else if (border->borderType == Text::SpreadSheet::BorderType::MediumDashed)
	{
		sb.AppendC(UTF8STRC(" ss:LineStyle=\"Dash\" ss:Weight=\"2\""));
	}
	else if (border->borderType == Text::SpreadSheet::BorderType::MediumDashDot)
	{
		sb.AppendC(UTF8STRC(" ss:LineStyle=\"DashDot\" ss:Weight=\"2\""));
	}
	else if (border->borderType == Text::SpreadSheet::BorderType::MediumDashDotDot)
	{
		sb.AppendC(UTF8STRC(" ss:LineStyle=\"DashDotDot\" ss:Weight=\"2\""));
	}
	else if (border->borderType == Text::SpreadSheet::BorderType::SlantedDashDot)
	{
		sb.AppendC(UTF8STRC(" ss:LineStyle=\"SlantDashDot\" ss:Weight=\"2\""));
	}
	else if (border->borderType == Text::SpreadSheet::BorderType::Thick)
	{
		sb.AppendC(UTF8STRC(" ss:LineStyle=\"Continuous\" ss:Weight=\"3\""));
	}
	else
	{
		sb.AppendC(UTF8STRC(" ss:LineStyle=\"Continuous\" ss:Weight=\""));
		sb.AppendI32((Int32)border->borderType);
		sb.AppendC(UTF8STRC("\""));
	}

	if (border->borderColor & 0xffffff)
	{
		writer->WriteLine(sb.ToString());
		sb.ClearStr();
		sb.AppendC(UTF8STRC("     ss:Color=\"#"));
		Text::StrHexVal32(sbuff, border->borderColor);
		sb.Append(&sbuff[2]);
		sb.AppendC(UTF8STRC("\""));
	}
	sb.AppendC(UTF8STRC("/>"));
	writer->WriteLine(sb.ToString());
}
