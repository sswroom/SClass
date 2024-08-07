#include "Stdafx.h"
#include "MyMemory.h"
#include "Exporter/ExcelXMLExporter.h"
#include "IO/BufferedOutputStream.h"
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

IO::FileExporter::SupportType Exporter::ExcelXMLExporter::IsObjectSupported(NN<IO::ParsedObject> pobj)
{
	if (pobj->GetParserType() != IO::ParserType::Workbook)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	return IO::FileExporter::SupportType::NormalStream;
}

Bool Exporter::ExcelXMLExporter::GetOutputName(UOSInt index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("Excel XML File"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.xml"));
		return true;
	}
	return false;
}

void Exporter::ExcelXMLExporter::SetCodePage(UInt32 codePage)
{
	this->codePage = codePage;
}

Bool Exporter::ExcelXMLExporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
{
	UTF8Char sbuff[256];
	UTF8Char sbuff2[256];
	UnsafeArray<UTF8Char> sptr;
	UInt32 pal[56];
	UInt32 defPal[56];
	Bool found;
	if (pobj->GetParserType() != IO::ParserType::Workbook)
	{
		return false;
	}
	NN<Text::SpreadSheet::Workbook> wb;
	NN<Text::SpreadSheet::Worksheet> ws;
	NN<Text::SpreadSheet::CellStyle> style;
	UOSInt k;
	UOSInt l;
	UOSInt m;
	UOSInt n;
	UInt32 v;
	UnsafeArray<const UTF8Char> text;
	NN<Text::String> s;
	Data::DateTime *dt;
	Double ver;
	Text::StringBuilderUTF8 sb;

	IO::BufferedOutputStream cstm(stm, 65536);
	IO::StreamWriter writer(cstm, this->codePage);
	wb = NN<Text::SpreadSheet::Workbook>::ConvertFrom(pobj);
	writer.WriteSignature();

	sb.AppendC(UTF8STRC("<?xml version=\"1.0\" encoding="));
	Text::EncodingFactory::GetInternetName(sbuff, this->codePage);
	sptr = Text::XML::ToAttrText(sbuff2, sbuff);
	sb.AppendC(sbuff2, (UOSInt)(sptr - sbuff2));
	sb.AppendC(UTF8STRC("?>"));
	writer.WriteLine(sb.ToCString());

	writer.WriteLine(CSTR("<?mso-application progid=\"Excel.Sheet\"?>"));
	writer.WriteLine(CSTR("<Workbook xmlns=\"urn:schemas-microsoft-com:office:spreadsheet\""));
	writer.WriteLine(CSTR(" xmlns:o=\"urn:schemas-microsoft-com:office:office\""));
	writer.WriteLine(CSTR(" xmlns:x=\"urn:schemas-microsoft-com:office:excel\""));
	writer.WriteLine(CSTR(" xmlns:ss=\"urn:schemas-microsoft-com:office:spreadsheet\""));
	writer.WriteLine(CSTR(" xmlns:html=\"http://www.w3.org/TR/REC-html40\">"));

	if (wb->HasInfo())
	{
		writer.WriteLine(CSTR(" <DocumentProperties xmlns=\"urn:schemas-microsoft-com:office:office\">"));
		if (wb->GetAuthor().SetTo(text))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("  <Author>"));
			s = Text::XML::ToNewXMLText(text);
			sb.Append(s);
			sb.AppendC(UTF8STRC("</Author>"));
			s->Release();
			writer.WriteLine(sb.ToCString());
		}
		if (wb->GetLastAuthor().SetTo(text))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("  <LastAuthor>"));
			s = Text::XML::ToNewXMLText(text);
			sb.Append(s);
			sb.AppendC(UTF8STRC("</LastAuthor>"));
			s->Release();
			writer.WriteLine(sb.ToCString());
		}
		dt = wb->GetCreateTime();
		if (dt)
		{
			sptr = dt->ToString(sbuff, "yyyy-MM-ddTHH:mm:ssZ");
			sb.ClearStr();
			sb.AppendC(UTF8STRC("  <Created>"));
			sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
			sb.AppendC(UTF8STRC("</Created>"));
			writer.WriteLine(sb.ToCString());
		}
		dt = wb->GetModifyTime();
		if (dt)
		{
			sptr = dt->ToString(sbuff, "yyyy-MM-ddTHH:mm:ssZ");
			sb.ClearStr();
			sb.AppendC(UTF8STRC("  <LastSaved>"));
			sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
			sb.AppendC(UTF8STRC("</LastSaved>"));
			writer.WriteLine(sb.ToCString());
		}
		if (wb->GetCompany().SetTo(text))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("  <Company>"));
			s = Text::XML::ToNewXMLText(text);
			sb.Append(s);
			sb.AppendC(UTF8STRC("</Company>"));
			s->Release();
			writer.WriteLine(sb.ToCString());
		}
		ver = wb->GetVersion();
		if (ver)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("  <Version>"));
			sb.AppendDouble(ver);
			sb.AppendC(UTF8STRC("</Version>"));
			writer.WriteLine(sb.ToCString());
		}
		writer.WriteLine(CSTR(" </DocumentProperties>"));
	}

	if (wb->HasWindowInfo() != 0)
	{
		writer.WriteLine(CSTR(" <ExcelWorkbook xmlns=\"urn:schemas-microsoft-com:office:excel\">"));
		if (wb->GetWindowHeight() != 0)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("  <WindowHeight>"));
			sb.AppendI32(wb->GetWindowHeight());
			sb.AppendC(UTF8STRC("</WindowHeight>"));
			writer.WriteLine(sb.ToCString());
		}
		if (wb->GetWindowWidth() != 0)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("  <WindowWidth>"));
			sb.AppendI32(wb->GetWindowWidth());
			sb.AppendC(UTF8STRC("</WindowWidth>"));
			writer.WriteLine(sb.ToCString());
		}
		if (wb->GetWindowTopX() != 0)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("  <WindowTopX>"));
			sb.AppendI32(wb->GetWindowTopX());
			sb.AppendC(UTF8STRC("</WindowTopX>"));
			writer.WriteLine(sb.ToCString());
		}
		if (wb->GetWindowTopY() != 0)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("  <WindowTopY>"));
			sb.AppendI32(wb->GetWindowTopY());
			sb.AppendC(UTF8STRC("</WindowTopY>"));
			writer.WriteLine(sb.ToCString());
		}
		if (wb->GetActiveSheet() != 0)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("  <ActiveSheet>"));
			sb.AppendUOSInt(wb->GetActiveSheet());
			sb.AppendC(UTF8STRC("</ActiveSheet>"));
			writer.WriteLine(sb.ToCString());
		}
		writer.WriteLine(CSTR(" </ExcelWorkbook>"));
	}

	wb->GetPalette(pal);
	Text::SpreadSheet::Workbook::GetDefPalette(defPal);
	found = false;
	k = 0;
	l = 56;
	while (k < l)
	{
		if (pal[k] != defPal[k])
		{
			if (!found)
			{
				found = true;
				writer.WriteLine(CSTR(" <OfficeDocumentSettings xmlns=\"urn:schemas-microsoft-com:office:office\">"));
				writer.WriteLine(CSTR("  <Colors>"));
			}
			writer.WriteLine(CSTR("   <Color>"));
			sb.ClearStr();
			sb.AppendC(UTF8STRC("    <Index>"));
			sb.AppendUOSInt(k);
			sb.AppendC(UTF8STRC("</Index>"));
			writer.WriteLine(sb.ToCString());
			sb.ClearStr();
			sb.AppendC(UTF8STRC("    <RGB>#"));
			sb.AppendHex24(pal[k]);
			sb.AppendC(UTF8STRC("</RGB>"));
			writer.WriteLine(sb.ToCString());
			writer.WriteLine(CSTR("   </Color>"));
		}
		k++;
	}
	if (found)
	{
		writer.WriteLine(CSTR("  </Colors>"));
		writer.WriteLine(CSTR(" </OfficeDocumentSettings>"));
	}

	if (wb->HasCellStyle())
	{
		writer.WriteLine(CSTR(" <Styles>"));
		k = 0;
		l = wb->GetStyleCount();
		while (k < l)
		{
			if (wb->GetStyle(k).SetTo(style))
			{
				if (k == 0)
				{
					style->SetID((const UTF8Char*)"Default");
				}
				else
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("s"));
					sb.AppendUOSInt((20 + k));
					style->SetID(sb.ToString());
				}
				sb.ClearStr();
				sb.AppendC(UTF8STRC("  <Style ss:ID="));
				s = Text::XML::ToNewAttrText(style->GetID());
				sb.Append(s);
				s->Release();
				sb.AppendC(UTF8STRC(">"));
				writer.WriteLine(sb.ToCString());

				if (style->GetHAlign() != Text::HAlignment::Unknown || (style->GetVAlign() != Text::VAlignment::Center && style->GetVAlign() != Text::VAlignment::Unknown) || style->GetWordWrap())
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("   <Alignment"));
					switch (style->GetHAlign())
					{
					case Text::HAlignment::Left:
						sb.AppendC(UTF8STRC(" ss:Horizontal=\"Left\""));
						break;
					case Text::HAlignment::Center:
						sb.AppendC(UTF8STRC(" ss:Horizontal=\"Center\""));
						break;
					case Text::HAlignment::Right:
						sb.AppendC(UTF8STRC(" ss:Horizontal=\"Right\""));
						break;
					case Text::HAlignment::Fill:
						sb.AppendC(UTF8STRC(" ss:Horizontal=\"Fill\""));
						break;
					case Text::HAlignment::Justify:
						sb.AppendC(UTF8STRC(" ss:Horizontal=\"Justify\""));
						break;
					case Text::HAlignment::Unknown:
					default:
						break;
					}
					switch (style->GetVAlign())
					{
					case Text::VAlignment::Top:
						sb.AppendC(UTF8STRC(" ss:Vertical=\"Top\""));
						break;
					case Text::VAlignment::Center:
						//sb.AppendC(UTF8STRC(" ss:Vertical=\"Center\""));
						break;
					case Text::VAlignment::Bottom:
						sb.AppendC(UTF8STRC(" ss:Vertical=\"Bottom\""));
						break;
					case Text::VAlignment::Justify:
						sb.AppendC(UTF8STRC(" ss:Vertical=\"Justify\""));
						break;
					case Text::VAlignment::Unknown:
					default:
						break;
					}
					if (style->GetWordWrap())
					{
						sb.AppendC(UTF8STRC(" ss:WrapText=\"1\""));
					}
					sb.AppendC(UTF8STRC("/>"));
					writer.WriteLine(sb.ToCString());
				}
				if (style->GetBorderLeft().borderType != Text::SpreadSheet::BorderType::None ||
					style->GetBorderTop().borderType != Text::SpreadSheet::BorderType::None ||
					style->GetBorderRight().borderType != Text::SpreadSheet::BorderType::None ||
					style->GetBorderBottom().borderType != Text::SpreadSheet::BorderType::None)
				{
					Text::SpreadSheet::CellStyle::BorderStyle border;
					writer.WriteLine(CSTR("   <Borders>"));
					border = style->GetBorderBottom();
					if (border.borderType != Text::SpreadSheet::BorderType::None)
					{
						WriteBorderStyle(writer, (const UTF8Char*)"Bottom", border);
					}
					border = style->GetBorderLeft();
					if (border.borderType != Text::SpreadSheet::BorderType::None)
					{
						WriteBorderStyle(writer, (const UTF8Char*)"Left", border);
					}
					border = style->GetBorderRight();
					if (border.borderType != Text::SpreadSheet::BorderType::None)
					{
						WriteBorderStyle(writer, (const UTF8Char*)"Right", border);
					}
					border = style->GetBorderTop();
					if (border.borderType != Text::SpreadSheet::BorderType::None)
					{
						WriteBorderStyle(writer, (const UTF8Char*)"Top", border);
					}
					writer.WriteLine(CSTR("   </Borders>"));
				}

				if ((style->GetFillColor() & 0xffffff) != 0xffffff)
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("   <Interior ss:Color=\"#"));
					sptr = Text::StrHexVal32(sbuff, style->GetFillColor());
					sb.AppendC(&sbuff[2], (UOSInt)(sptr - &sbuff[2]));
					sb.AppendC(UTF8STRC("\" ss:Pattern=\"Solid\"/>"));
					writer.WriteLine(sb.ToCString());
				}

				NN<Text::SpreadSheet::WorkbookFont> font;
				if (style->GetFont().SetTo(font))
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("   <Font ss:FontName="));
					s = Text::XML::ToNewAttrText(OPTSTR_CSTR(font->GetName()).v);
					sb.Append(s);
					s->Release();
					if (font->GetSize() > 0)
					{
						sb.AppendC(UTF8STRC(" ss:Size=\""));
						sb.AppendDouble(font->GetSize());
						sb.AppendC(UTF8STRC("\""));
					}
					if ((font->GetColor() & 0xffffff) != 0)
					{
						sptr = Text::StrHexVal32(sbuff, font->GetColor());
						sb.AppendC(UTF8STRC(" ss:Color=\"#"));
						sb.AppendC(&sbuff[2], (UOSInt)(sptr - &sbuff[2]));
						sb.AppendC(UTF8STRC("\""));
					}
					if (font->IsBold())
					{
						sb.AppendC(UTF8STRC(" ss:Bold=\"1\""));
					}
					sb.AppendC(UTF8STRC("/>"));
					writer.WriteLine(sb.ToCString());
				}

				if (style->GetDataFormat().SetTo(s))
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("   <NumberFormat ss:Format="));
					s = Text::XML::ToNewAttrText(s->v);
					sb.Append(s);
					s->Release();
					sb.AppendC(UTF8STRC("/>"));
					writer.WriteLine(sb.ToCString());
				}

				writer.WriteLine(CSTR("  </Style>"));
			}
			k++;
		}
		writer.WriteLine(CSTR(" </Styles>"));
	}

	Data::ArrayIterator<NN<Text::SpreadSheet::Worksheet>> it = wb->Iterator();
	while (it.HasNext())
	{
		ws = it.Next();
		sb.ClearStr();
		sb.AppendC(UTF8STRC(" <Worksheet ss:Name="));
		s = Text::XML::ToNewAttrText(ws->GetName()->v);
		sb.Append(s);
		sb.AppendC(UTF8STRC(">"));
		s->Release();
		writer.WriteLine(sb.ToCString());

		if (ws->GetCount() > 0)
		{
			Bool rowSkipped = false;
			UOSInt lastDispCol;
			NN<Text::SpreadSheet::Worksheet::RowData> row;
			NN<Text::SpreadSheet::Worksheet::CellData> cell;
			writer.WriteLine(CSTR("  <Table>"));

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
						sb.AppendDouble(Double2Int32(lastColWidth * 4) * 0.25);
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
						writer.WriteLine(sb.ToCString());
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
				sb.AppendDouble(Double2Int32(lastColWidth * 4) * 0.25);
				sb.AppendC(UTF8STRC("\""));
				if (lastColIndex + 1 != l)
				{
					sb.AppendC(UTF8STRC(" ss:Span=\""));
					sb.AppendUOSInt((l - lastColIndex - 1));
					sb.AppendC(UTF8STRC("\""));
				}
				sb.AppendC(UTF8STRC("/>"));
				writer.WriteLine(sb.ToCString());
			}

			k = 0;
			l = ws->GetCount();
			while (k < l)
			{
				if (!ws->GetItem(k).SetTo(row))
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

					if (row->cells.GetItem(0).SetTo(cell))
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
							sb.AppendDouble(row->height);
						}
						else
						{
							sb.AppendDouble(row->height + 0.05);
						}
						sb.AppendC(UTF8STRC("\""));
					}
					if (row->cells.GetCount() == 0)
					{
						sb.AppendC(UTF8STRC("/>"));
						writer.WriteLine(sb.ToCString());
					}
					else
					{
						sb.AppendC(UTF8STRC(">"));
						writer.WriteLine(sb.ToCString());

						lastDispCol = INVALID_INDEX;
						m = 0;
						n = row->cells.GetCount();
						while (m < n)
						{
							if (!row->cells.GetItem(m).SetTo(cell))
							{
							}
							else if (cell->cdt == Text::SpreadSheet::CellDataType::MergedLeft)
							{
							}
							else if (cell->cdt == Text::SpreadSheet::CellDataType::MergedUp)
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
								NN<Text::SpreadSheet::CellStyle> tmpStyle;
								if (cell->style.SetTo(tmpStyle))
								{
									if (tmpStyle->GetID().SetTo(text))
									{
										sb.AppendC(UTF8STRC(" ss:StyleID="));
										s = Text::XML::ToNewAttrText(text);
										sb.Append(s);
										s->Release();
									}
								}
								NN<Text::String> s;
								if (cell->cellURL.SetTo(s))
								{
									 sb.AppendC(UTF8STRC(" ss:HRef="));
									 s = Text::XML::ToNewAttrText(s->v);
									 sb.Append(s);
									 s->Release();
								}
								sb.AppendC(UTF8STRC(">"));
								if (cell->cellValue.SetTo(s))
								{
									switch (cell->cdt)
									{
									case Text::SpreadSheet::CellDataType::Number:
										sb.AppendC(UTF8STRC("<Data ss:Type=\"Number\">"));
										s = Text::XML::ToNewXMLText(s->v);
										sb.Append(s);
										s->Release();
										sb.AppendC(UTF8STRC("</Data>"));
										break;
									case Text::SpreadSheet::CellDataType::DateTime:
										sb.AppendC(UTF8STRC("<Data ss:Type=\"DateTime\">"));
										sptr = Data::Timestamp::FromStr(s->ToCString(), Data::DateTimeUtil::GetLocalTzQhr()).ToString(sbuff, "yyyy-MM-ddTHH:mm:ss.fff");
										sb.AppendP(sbuff, sptr);
										sb.AppendC(UTF8STRC("</Data>"));
										break;
									case Text::SpreadSheet::CellDataType::MergedLeft:
									case Text::SpreadSheet::CellDataType::MergedUp:
									case Text::SpreadSheet::CellDataType::String:
									default:
										sb.AppendC(UTF8STRC("<Data ss:Type=\"String\">"));
										s = Text::XML::ToNewXMLText(s->v);
										sb.Append(s);
										s->Release();
										sb.AppendC(UTF8STRC("</Data>"));
										break;
									}
								}
								sb.AppendC(UTF8STRC("</Cell>"));
								writer.WriteLine(sb.ToCString());
							}
							m++;
						}

						writer.WriteLine(CSTR("   </Row>"));
					}
				}
				k++;
			}
			writer.WriteLine(CSTR("  </Table>"));
		}
		else
		{
			writer.WriteLine(CSTR("  <Table x:FullColumns=\"1\" x:FullRows=\"1\"/>"));
		}

		found = false;
		if (!ws->IsDefaultPageSetup())
		{
			if (!found)
			{
				writer.WriteLine(CSTR("  <WorksheetOptions xmlns=\"urn:schemas-microsoft-com:office:excel\">"));
				found = true;
			}
			writer.WriteLine(CSTR("   <PageSetup>"));
			if (ws->GetMarginHeader() != 1.3)
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("    <Header x:Margin=\""));
				sb.AppendDouble(ws->GetMarginHeader());
				sb.AppendC(UTF8STRC("\"/>"));
				writer.WriteLine(sb.ToCString());
			}
			if (ws->GetMarginFooter() != 1.3)
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("    <Footer x:Margin=\""));
				sb.AppendDouble(ws->GetMarginFooter());
				sb.AppendC(UTF8STRC("\"/>"));
				writer.WriteLine(sb.ToCString());
			}
			if (ws->GetMarginTop() != 2.5 || ws->GetMarginBottom() != 2.5 || ws->GetMarginLeft() != 2.0 || ws->GetMarginBottom() != 2.0)
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("    <PageMargins x:Bottom=\""));
				sb.AppendDouble(ws->GetMarginBottom());
				sb.AppendC(UTF8STRC("\" x:Left=\""));
				sb.AppendDouble(ws->GetMarginLeft());
				sb.AppendC(UTF8STRC("\" x:Right=\""));
				sb.AppendDouble(ws->GetMarginRight());
				sb.AppendC(UTF8STRC("\" x:Top=\""));
				sb.AppendDouble(ws->GetMarginTop());
				sb.AppendC(UTF8STRC("\"/>"));
				writer.WriteLine(sb.ToCString());

			}
			writer.WriteLine(CSTR("   </PageSetup>"));
		}
		if ((v = ws->GetZoom()) != 0)
		{
			if (!found)
			{
				writer.WriteLine(CSTR("  <WorksheetOptions xmlns=\"urn:schemas-microsoft-com:office:excel\">"));
				found = true;
			}
			sb.ClearStr();
			sb.AppendC(UTF8STRC("   <Zoom>"));
			sb.AppendUOSInt(v);
			sb.AppendC(UTF8STRC("</Zoom>"));
			writer.WriteLine(sb.ToCString());
		}
		if ((v = ws->GetFreezeHori()) != 0)
		{
			if (!found)
			{
				writer.WriteLine(CSTR("  <WorksheetOptions xmlns=\"urn:schemas-microsoft-com:office:excel\">"));
				found = true;
			}
			sb.ClearStr();
			sb.AppendC(UTF8STRC("   <SplitVertical>"));
			sb.AppendU32(v);
			sb.AppendC(UTF8STRC("</SplitVertical>"));
			writer.WriteLine(sb.ToCString());

			sb.ClearStr();
			sb.AppendC(UTF8STRC("   <LeftColumnRightPane>"));
			sb.AppendU32(v);
			sb.AppendC(UTF8STRC("</LeftColumnRightPane>"));
			writer.WriteLine(sb.ToCString());
		}
		if ((v = ws->GetFreezeVert()) != 0)
		{
			if (!found)
			{
				writer.WriteLine(CSTR("  <WorksheetOptions xmlns=\"urn:schemas-microsoft-com:office:excel\">"));
				found = true;
			}
			sb.ClearStr();
			sb.AppendC(UTF8STRC("   <SplitHorizontal>"));
			sb.AppendU32(v);
			sb.AppendC(UTF8STRC("</SplitHorizontal>"));
			writer.WriteLine(sb.ToCString());

			sb.ClearStr();
			sb.AppendC(UTF8STRC("   <TopRowBottomPane>"));
			sb.AppendU32(v);
			sb.AppendC(UTF8STRC("</TopRowBottomPane>"));
			writer.WriteLine(sb.ToCString());
		}
		v = ws->GetOptions();
		if (v & 0x8)
		{
			if (!found)
			{
				writer.WriteLine(CSTR("  <WorksheetOptions xmlns=\"urn:schemas-microsoft-com:office:excel\">"));
				found = true;
			}
			writer.WriteLine(CSTR("   <FreezePanes/>"));
		}
		if (v & 0x100)
		{
			if (!found)
			{
				writer.WriteLine(CSTR("  <WorksheetOptions xmlns=\"urn:schemas-microsoft-com:office:excel\">"));
				found = true;
			}
			writer.WriteLine(CSTR("   <FrozenNoSplit/>"));
		}
		if (v & 0x200)
		{
			if (!found)
			{
				writer.WriteLine(CSTR("  <WorksheetOptions xmlns=\"urn:schemas-microsoft-com:office:excel\">"));
				found = true;
			}
			writer.WriteLine(CSTR("   <Selected/>"));
		}
		if ((v & 0x2) == 0)
		{
			if (!found)
			{
				writer.WriteLine(CSTR("  <WorksheetOptions xmlns=\"urn:schemas-microsoft-com:office:excel\">"));
				found = true;
			}
			writer.WriteLine(CSTR("   <DoNotDisplayGridlines/>"));
		}
		if ((v & 0x80) == 0)
		{
			if (!found)
			{
				writer.WriteLine(CSTR("  <WorksheetOptions xmlns=\"urn:schemas-microsoft-com:office:excel\">"));
				found = true;
			}
			writer.WriteLine(CSTR("   <DoNotDisplayOutline/>"));
		}
		if (found)
		{
			writer.WriteLine(CSTR("  </WorksheetOptions>"));
		}
		writer.WriteLine(CSTR(" </Worksheet>"));
	}
	writer.WriteLine(CSTR("</Workbook>"));
	return true;
}

void Exporter::ExcelXMLExporter::WriteBorderStyle(NN<IO::Writer> writer, UnsafeArray<const UTF8Char> position, Text::SpreadSheet::CellStyle::BorderStyle border)
{
	UTF8Char sbuff[10];
	UnsafeArray<UTF8Char> sptr;
	Text::StringBuilderUTF8 sb;
	NN<Text::String> s;
	sb.AppendC(UTF8STRC("    <Border ss:Position="));
	s = Text::XML::ToNewAttrText(position);
	sb.Append(s);
	s->Release();
	if (border.borderType == Text::SpreadSheet::BorderType::Thin)
	{
		sb.AppendC(UTF8STRC(" ss:LineStyle=\"Continuous\" ss:Weight=\"1\""));
	}
	else if (border.borderType == Text::SpreadSheet::BorderType::Hair)
	{
		sb.AppendC(UTF8STRC(" ss:LineStyle=\"Continuous\""));
	}
	else if (border.borderType == Text::SpreadSheet::BorderType::Dotted)
	{
		sb.AppendC(UTF8STRC(" ss:LineStyle=\"Dot\" ss:Weight=\"1\""));
	}
	else if (border.borderType == Text::SpreadSheet::BorderType::Dashed)
	{
		sb.AppendC(UTF8STRC(" ss:LineStyle=\"Dash\" ss:Weight=\"1\""));
	}
	else if (border.borderType == Text::SpreadSheet::BorderType::DashDot)
	{
		sb.AppendC(UTF8STRC(" ss:LineStyle=\"DashDot\" ss:Weight=\"1\""));
	}
	else if (border.borderType == Text::SpreadSheet::BorderType::DashDotDot)
	{
		sb.AppendC(UTF8STRC(" ss:LineStyle=\"DashDotDot\" ss:Weight=\"1\""));
	}
	else if (border.borderType == Text::SpreadSheet::BorderType::DOUBLE)
	{
		sb.AppendC(UTF8STRC(" ss:LineStyle=\"Double\" ss:Weight=\"3\""));
	}
	else if (border.borderType == Text::SpreadSheet::BorderType::Medium)
	{
		sb.AppendC(UTF8STRC(" ss:LineStyle=\"Continuous\" ss:Weight=\"2\""));
	}
	else if (border.borderType == Text::SpreadSheet::BorderType::MediumDashed)
	{
		sb.AppendC(UTF8STRC(" ss:LineStyle=\"Dash\" ss:Weight=\"2\""));
	}
	else if (border.borderType == Text::SpreadSheet::BorderType::MediumDashDot)
	{
		sb.AppendC(UTF8STRC(" ss:LineStyle=\"DashDot\" ss:Weight=\"2\""));
	}
	else if (border.borderType == Text::SpreadSheet::BorderType::MediumDashDotDot)
	{
		sb.AppendC(UTF8STRC(" ss:LineStyle=\"DashDotDot\" ss:Weight=\"2\""));
	}
	else if (border.borderType == Text::SpreadSheet::BorderType::SlantedDashDot)
	{
		sb.AppendC(UTF8STRC(" ss:LineStyle=\"SlantDashDot\" ss:Weight=\"2\""));
	}
	else if (border.borderType == Text::SpreadSheet::BorderType::Thick)
	{
		sb.AppendC(UTF8STRC(" ss:LineStyle=\"Continuous\" ss:Weight=\"3\""));
	}
	else
	{
		sb.AppendC(UTF8STRC(" ss:LineStyle=\"Continuous\" ss:Weight=\""));
		sb.AppendI32((Int32)border.borderType);
		sb.AppendC(UTF8STRC("\""));
	}

	if (border.borderColor & 0xffffff)
	{
		writer->WriteLine(sb.ToCString());
		sb.ClearStr();
		sb.AppendC(UTF8STRC("     ss:Color=\"#"));
		sptr = Text::StrHexVal32(sbuff, border.borderColor);
		sb.AppendC(&sbuff[2], (UOSInt)(sptr - &sbuff[2]));
		sb.AppendC(UTF8STRC("\""));
	}
	sb.AppendC(UTF8STRC("/>"));
	writer->WriteLine(sb.ToCString());
}
