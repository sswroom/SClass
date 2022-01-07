#include "Stdafx.h"
#include "Text/MyString.h"
#include "Text/SpreadSheet/Workbook.h"

const UInt32 Text::SpreadSheet::Workbook::defPalette[56] = {
	0xff000000, 0xffffffff, 0xffff0000, 0xff00ff00, 0xff0000ff, 0xffffff00, 0xffff00ff, 0xff00ffff,
	0xff800000, 0xff008000, 0xff000080, 0xff808000, 0xff800080, 0xff008080, 0xffc0c0c0, 0xff808080,
	0xff9999ff, 0xff993366, 0xffffffcc, 0xffccffff, 0xff660066, 0xffff8080, 0xff0066cc, 0xffccccff,
	0xff000080, 0xffff00ff, 0xffffff00, 0xff00ffff, 0xff800080, 0xff800000, 0xff008080, 0xff0000ff,
	0xff00ccff, 0xffccffff, 0xffccffcc, 0xffffff99, 0xff99ccff, 0xffff99cc, 0xffcc99ff, 0xffffcc99,
	0xff3366ff, 0xff33cccc, 0xff99cc00, 0xffffcc00, 0xffff9900, 0xffff6600, 0xff666699, 0xff969696,
	0xff003366, 0xff339966, 0xff003300, 0xff333300, 0xff993300, 0xff993366, 0xff333399, 0xff333333
};

Text::SpreadSheet::Workbook::Workbook() : IO::ParsedObject((const UTF8Char*)"Untitled")
{
	this->author = 0;
	this->lastAuthor = 0;
	this->company = 0;
	this->createTime = 0;
	this->modifyTime = 0;
	this->version = 0;
	this->windowTopX = 0;
	this->windowTopY = 0;
	this->windowWidth = 0;
	this->windowHeight = 0;
	this->activeSheet = 0;

	NEW_CLASS(this->sheets, Data::ArrayList<Text::SpreadSheet::Worksheet*>());
	NEW_CLASS(this->styles, Data::ArrayList<Text::SpreadSheet::CellStyle*>());
	NEW_CLASS(this->fonts, Data::ArrayList<Text::SpreadSheet::WorkbookFont*>());
	MemCopyNO(this->palette, defPalette, sizeof(defPalette));

	this->NewCellStyle(0, HAlignment::Unknown, VAlignment::Bottom, (const UTF8Char*)"general");
	this->NewFont((const UTF8Char*)"Arial", 10.0, false)->SetFamily(FontFamily::Swiss);
	this->NewFont((const UTF8Char*)"Arial", 10.0, false);
	this->NewFont((const UTF8Char*)"Arial", 10.0, false);
	this->NewFont((const UTF8Char*)"Arial", 10.0, false);
}

Text::SpreadSheet::Workbook::~Workbook()
{
	Text::SpreadSheet::Worksheet *ws;
	Text::SpreadSheet::CellStyle *style;
	Text::SpreadSheet::WorkbookFont *font;
	UOSInt i = this->sheets->GetCount();
	while (i-- > 0)
	{
		ws = this->sheets->GetItem(i);
		DEL_CLASS(ws);
	}
	DEL_CLASS(this->sheets);
	i = this->styles->GetCount();
	while (i-- > 0)
	{
		style = this->styles->GetItem(i);
		SDEL_CLASS(style);
	}
	DEL_CLASS(this->styles);
	i = this->fonts->GetCount();
	while (i-- > 0)
	{
		font = this->fonts->GetItem(i);
		DEL_CLASS(font);
	}
	DEL_CLASS(this->fonts);

	SDEL_TEXT(this->author);
	SDEL_TEXT(this->lastAuthor);
	SDEL_TEXT(this->company);
	SDEL_CLASS(this->createTime);
	SDEL_CLASS(this->modifyTime);
}

IO::ParserType Text::SpreadSheet::Workbook::GetParserType()
{
	return IO::ParserType::Workbook;
}

Text::SpreadSheet::Workbook *Text::SpreadSheet::Workbook::Clone()
{
	UOSInt i;
	UOSInt j;
	Text::SpreadSheet::Workbook *newWB;
	NEW_CLASS(newWB, Text::SpreadSheet::Workbook());
	newWB->author = SCOPY_TEXT(this->author);
	newWB->lastAuthor = SCOPY_TEXT(this->lastAuthor);
	newWB->company = SCOPY_TEXT(this->company);
	if (this->createTime)
	{
		NEW_CLASS(newWB->createTime, Data::DateTime(this->createTime));
	}
	if (this->modifyTime)
	{
		NEW_CLASS(newWB->modifyTime, Data::DateTime(this->modifyTime));
	}
	newWB->version = this->version;
	newWB->windowTopX = this->windowTopX;
	newWB->windowTopY = this->windowTopY;
	newWB->windowWidth = this->windowWidth;
	newWB->windowHeight = this->windowHeight;
	newWB->activeSheet = this->activeSheet;
	MemCopyNO(newWB->palette, this->palette, sizeof(defPalette));

	i = 0;
	j = this->styles->GetCount();
	while (i < j)
	{
		newWB->styles->Add(this->styles->GetItem(i)->Clone());
		i++;
	}
	i = 0;
	j = this->sheets->GetCount();
	while (i < j)
	{
		newWB->sheets->Add(this->sheets->GetItem(i)->Clone(this, newWB));
		i++;
	}
	i = 0;
	j = this->fonts->GetCount();
	while (i < j)
	{
		newWB->fonts->Add(this->fonts->GetItem(i)->Clone());
		i++;
	}
	return newWB;
}

void Text::SpreadSheet::Workbook::AddDefaultStyles()
{
	Text::SpreadSheet::CellStyle *style;
	while (this->styles->GetCount() < 21)
	{
		NEW_CLASS(style, Text::SpreadSheet::CellStyle(this->styles->GetCount()));
		this->styles->Add(style);
	}
}

void Text::SpreadSheet::Workbook::SetAuthor(const UTF8Char *author)
{
	if (this->author)
	{
		Text::StrDelNew(this->author);
	}
	if (author)
	{
		this->author = Text::StrCopyNew(author);
	}
	else
	{
		this->author = 0;
	}
}

void Text::SpreadSheet::Workbook::SetLastAuthor(const UTF8Char *lastAuthor)
{
	if (this->lastAuthor)
	{
		Text::StrDelNew(this->lastAuthor);
	}
	if (lastAuthor)
	{
		this->lastAuthor = Text::StrCopyNew(lastAuthor);
	}
	else
	{
		this->lastAuthor = 0;
	}
}

void Text::SpreadSheet::Workbook::SetCompany(const UTF8Char *company)
{
	if (this->company)
	{
		Text::StrDelNew(this->company);
	}
	if (company)
	{
		this->company = Text::StrCopyNew(company);
	}
	else
	{
		this->company = 0;
	}
}

void Text::SpreadSheet::Workbook::SetCreateTime(Data::DateTime *createTime)
{
	SDEL_CLASS(this->createTime);
	if (createTime)
	{
		NEW_CLASS(this->createTime, Data::DateTime(createTime));
	}
}

void Text::SpreadSheet::Workbook::SetModifyTime(Data::DateTime *modifyTime)
{
	SDEL_CLASS(this->modifyTime);
	if (modifyTime)
	{
		NEW_CLASS(this->modifyTime, Data::DateTime(modifyTime));
	}
}

void Text::SpreadSheet::Workbook::SetVersion(Double version)
{
	this->version = version;
}

const UTF8Char *Text::SpreadSheet::Workbook::GetAuthor()
{
	return this->author;
}

const UTF8Char *Text::SpreadSheet::Workbook::GetLastAuthor()
{
	return this->lastAuthor;
}

const UTF8Char *Text::SpreadSheet::Workbook::GetCompany()
{
	return this->company;
}

Data::DateTime *Text::SpreadSheet::Workbook::GetCreateTime()
{
	return this->createTime;
}

Data::DateTime *Text::SpreadSheet::Workbook::GetModifyTime()
{
	return this->modifyTime;
}

Double Text::SpreadSheet::Workbook::GetVersion()
{
	return this->version;
}

Bool Text::SpreadSheet::Workbook::HasInfo()
{
	if (this->author)
		return true;
	if (this->lastAuthor)
		return true;
	if (this->company)
		return true;
	if (this->createTime)
		return true;
	if (this->version != 0)
		return true;
	return false;
}

void Text::SpreadSheet::Workbook::SetWindowTopX(Int32 windowTopX)
{
	this->windowTopX = windowTopX;
}

void Text::SpreadSheet::Workbook::SetWindowTopY(Int32 windowTopY)
{
	this->windowTopY = windowTopY;
}

void Text::SpreadSheet::Workbook::SetWindowWidth(Int32 windowWidth)
{
	this->windowWidth = windowWidth;
}

void Text::SpreadSheet::Workbook::SetWindowHeight(Int32 windowHeight)
{
	this->windowHeight = windowHeight;
}

void Text::SpreadSheet::Workbook::SetActiveSheet(UOSInt index)
{
	this->activeSheet = index;
}

Int32 Text::SpreadSheet::Workbook::GetWindowTopX()
{
	return this->windowTopX;
}

Int32 Text::SpreadSheet::Workbook::GetWindowTopY()
{
	return this->windowTopY;
}

Int32 Text::SpreadSheet::Workbook::GetWindowWidth()
{
	return this->windowWidth;
}

Int32 Text::SpreadSheet::Workbook::GetWindowHeight()
{
	return this->windowHeight;
}

UOSInt Text::SpreadSheet::Workbook::GetActiveSheet()
{
	return this->activeSheet;
}

Bool Text::SpreadSheet::Workbook::HasWindowInfo()
{
	if (this->windowTopX != 0 || this->windowTopY != 0 || this->windowWidth != 0 || this->windowHeight != 0 || this->activeSheet != 0)
		return true;
	return false;
}

Bool Text::SpreadSheet::Workbook::HasCellStyle()
{
	if (this->styles->GetCount() > 1)
		return true;
	if (this->styles->GetItem(0))
		return true;
	return false;
}

Text::SpreadSheet::CellStyle *Text::SpreadSheet::Workbook::NewCellStyle()
{
	CellStyle *style;
	NEW_CLASS(style, CellStyle(this->styles->GetCount()));
	this->styles->Add(style);
	return style;
}

Text::SpreadSheet::CellStyle *Text::SpreadSheet::Workbook::NewCellStyle(WorkbookFont *font, HAlignment halign, VAlignment valign, const UTF8Char *dataFormat)
{
	CellStyle *style;
	NEW_CLASS(style, CellStyle(this->styles->GetCount()));
	style->SetFont(font);
	style->SetHAlign(halign);
	style->SetVAlign(valign);
	style->SetDataFormat(dataFormat);
	this->styles->Add(style);
	return style;
}

UOSInt Text::SpreadSheet::Workbook::GetStyleCount()
{
	return this->styles->GetCount();
}

OSInt Text::SpreadSheet::Workbook::GetStyleIndex(CellStyle *style)
{
	UOSInt i = this->styles->GetCount();
	while (i-- > 0)
	{
		if (this->styles->GetItem(i) == style)
			return (OSInt)i;
	}
	return -1;
}

Text::SpreadSheet::CellStyle *Text::SpreadSheet::Workbook::GetStyle(UOSInt Index)
{
	return this->styles->GetItem(Index);
}

Text::SpreadSheet::CellStyle *Text::SpreadSheet::Workbook::GetDefaultStyle()
{
	Text::SpreadSheet::CellStyle *style = this->styles->GetItem(0);
	return style;
}

void Text::SpreadSheet::Workbook::GetPalette(UInt32 *palette)
{
	MemCopyNO(palette, this->palette, sizeof(UInt32) * 56);
}

void Text::SpreadSheet::Workbook::SetPalette(UInt32 *palette)
{
	MemCopyNO(this->palette, palette, sizeof(UInt32) * 56);
}

Text::SpreadSheet::Worksheet *Text::SpreadSheet::Workbook::AddWorksheet()
{
	UTF8Char sbuff[32];
	Text::SpreadSheet::Worksheet *ws;
	Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Sheet")), this->sheets->GetCount());
	NEW_CLASS(ws, Text::SpreadSheet::Worksheet(sbuff));
	this->sheets->Add(ws);
	return ws;
}

Text::SpreadSheet::Worksheet *Text::SpreadSheet::Workbook::AddWorksheet(Text::String *name)
{
	Text::SpreadSheet::Worksheet *ws;
	NEW_CLASS(ws, Text::SpreadSheet::Worksheet(name));
	this->sheets->Add(ws);
	return ws;
}

Text::SpreadSheet::Worksheet *Text::SpreadSheet::Workbook::AddWorksheet(const UTF8Char *name)
{
	Text::SpreadSheet::Worksheet *ws;
	NEW_CLASS(ws, Text::SpreadSheet::Worksheet(name));
	this->sheets->Add(ws);
	return ws;
}

Text::SpreadSheet::Worksheet *Text::SpreadSheet::Workbook::InsertWorksheet(UOSInt index, const UTF8Char *name)
{
	Text::SpreadSheet::Worksheet *ws;
	NEW_CLASS(ws, Text::SpreadSheet::Worksheet(name));
	this->sheets->Insert(index, ws);
	return ws;
}

UOSInt Text::SpreadSheet::Workbook::GetCount()
{
	return this->sheets->GetCount();
}

Text::SpreadSheet::Worksheet *Text::SpreadSheet::Workbook::GetItem(UOSInt index)
{
	return this->sheets->GetItem(index);
}

void Text::SpreadSheet::Workbook::RemoveAt(UOSInt index)
{
	Text::SpreadSheet::Worksheet *ws = this->sheets->RemoveAt(index);
	if (ws)
	{
		DEL_CLASS(ws);
	}
}

UOSInt Text::SpreadSheet::Workbook::GetFontCount()
{
	return this->fonts->GetCount();
}

Text::SpreadSheet::WorkbookFont *Text::SpreadSheet::Workbook::GetFont(UOSInt index)
{
	return this->fonts->GetItem(index);
}

UOSInt Text::SpreadSheet::Workbook::GetFontIndex(WorkbookFont *font)
{
	UOSInt i = this->fonts->GetCount();
	while (i-- > 0)
	{
		if (this->fonts->GetItem(i) == font)
		{
			return i;
		}
	}
	return INVALID_INDEX;
}

Text::SpreadSheet::WorkbookFont *Text::SpreadSheet::Workbook::NewFont(const UTF8Char *name, Double size, Bool bold)
{
	Text::SpreadSheet::WorkbookFont *font;
	NEW_CLASS(font, Text::SpreadSheet::WorkbookFont());
	this->fonts->Add(font);
	return font->SetName(name)->SetSize(size)->SetBold(bold);
}

void Text::SpreadSheet::Workbook::GetDefPalette(UInt32 *palette)
{
	MemCopyNO(palette, Text::SpreadSheet::Workbook::defPalette, sizeof(Text::SpreadSheet::Workbook::defPalette));
}

UTF8Char *Text::SpreadSheet::Workbook::ColCode(UTF8Char *sbuff, UOSInt col)
{
	if (col < 26)
	{
		*sbuff++ = (UTF8Char)(65 + col);
		*sbuff = 0;
		return sbuff;
	}
	col -= 26;
	if (col < 26 * 26)
	{
		*sbuff++ = (UTF8Char)(65 + (col / 26));
		*sbuff++ = (UTF8Char)(65 + (col % 26));
		*sbuff = 0;
		return sbuff;
	}
	col -= 26 * 26;
	sbuff[2] = (UTF8Char)(65 + (col % 26));
	col = col / 26;
	sbuff[1] = (UTF8Char)(65 + (col % 26));
	sbuff[0] = (UTF8Char)(65 + (col / 26));
	sbuff += 3;
	*sbuff = 0;
	return sbuff;
}
