#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/Doc/TextDocument.h"

Text::Doc::TextDocument::TextDocument() : IO::ParsedObject((const UTF8Char*)"Untitled")
{
	this->docName = 0;
	this->pflags = (Text::Doc::TextDocument::PropertiesFlags)0;
	NEW_CLASS(items, Data::ArrayList<DocSection*>());
	SetDocumentName((const UTF8Char*)"Untitled");
}

Text::Doc::TextDocument::TextDocument(const UTF8Char *name) : IO::ParsedObject(name)
{
	this->docName = 0;
	this->pflags = (Text::Doc::TextDocument::PropertiesFlags)0;
	NEW_CLASS(items, Data::ArrayList<DocSection*>());
	SetDocumentName(name);
}

Text::Doc::TextDocument::~TextDocument()
{
	DocSection *item;
	UOSInt i = this->items->GetCount();
	while (i-- > 0)
	{
		item = this->items->GetItem(i);
		DEL_CLASS(item);
	}
	DEL_CLASS(this->items);
	if (this->docName)
	{
		Text::StrDelNew(this->docName);
		this->docName = 0;
	}
}

IO::ParserType Text::Doc::TextDocument::GetParserType()
{
	return IO::ParserType::TextDocument;
}

void Text::Doc::TextDocument::SetDocumentName(const UTF8Char *docName)
{
	if (this->docName)
	{
		Text::StrDelNew(this->docName);
	}
	this->docName = Text::StrCopyNew(docName);
}

UTF8Char *Text::Doc::TextDocument::GetDocumentName(UTF8Char *docName)
{
	if (this->docName == 0)
		return 0;
	return Text::StrConcat(docName, this->docName);
}

void Text::Doc::TextDocument::SetTextColor(UInt32 textColor)
{
	this->textColor = textColor;
	this->pflags = (PropertiesFlags)(this->pflags | PF_TEXTCOLOR);
}

Bool Text::Doc::TextDocument::GetTextColor(UInt32 *textColor)
{
	if (this->pflags & PF_TEXTCOLOR)
	{
		*textColor = this->textColor;
		return true;
	}
	else
	{
		return false;
	}
}

void Text::Doc::TextDocument::SetBGColor(UInt32 bgColor)
{
	this->bgColor = bgColor;
	this->pflags = (PropertiesFlags)(this->pflags | PF_BGCOLOR);
}

Bool Text::Doc::TextDocument::GetBGColor(UInt32 *bgColor)
{
	if (this->pflags & PF_BGCOLOR)
	{
		*bgColor = this->bgColor;
		return true;
	}
	else
	{
		return false;
	}
}

void Text::Doc::TextDocument::SetLinkColor(UInt32 linkColor)
{
	this->linkColor = linkColor;
	this->pflags = (PropertiesFlags)(this->pflags | PF_LINKCOLOR);
}

Bool Text::Doc::TextDocument::GetLinkColor(UInt32 *linkColor)
{
	if (this->pflags & PF_LINKCOLOR)
	{
		*linkColor = this->linkColor;
		return true;
	}
	else
	{
		return false;
	}
}

void Text::Doc::TextDocument::SetActiveLinkColor(UInt32 activeLinkColor)
{
	this->activeLinkColor = activeLinkColor;
	this->pflags = (PropertiesFlags)(this->pflags | PF_ACTIVELINKCOLOR);
}

Bool Text::Doc::TextDocument::GetActiveLinkColor(UInt32 *activeLinkColor)
{
	if (this->pflags & PF_ACTIVELINKCOLOR)
	{
		*activeLinkColor = this->activeLinkColor;
		return true;
	}
	else
	{
		return false;
	}
}

void Text::Doc::TextDocument::SetVisitedLinkColor(UInt32 visitedLinkColor)
{
	this->visitedLinkColor = visitedLinkColor;
	this->pflags = (PropertiesFlags)(this->pflags | PF_VISITEDLINKCOLOR);
}

Bool Text::Doc::TextDocument::GetVisitedLinkColor(UInt32 *visitedLinkColor)
{
	if (this->pflags & PF_VISITEDLINKCOLOR)
	{
		*visitedLinkColor = this->visitedLinkColor;
		return true;
	}
	else
	{
		return false;
	}
}

UOSInt Text::Doc::TextDocument::Add(Text::Doc::DocSection *val)
{
	return this->items->Add(val);
}

UOSInt Text::Doc::TextDocument::GetCount()
{
	return this->items->GetCount();
}

Text::Doc::DocSection *Text::Doc::TextDocument::GetItem(UOSInt index)
{
	return this->items->GetItem(index);
}

Bool Text::Doc::TextDocument::BeginPrint(Media::IPrintDocument *doc)
{
	Text::Doc::DocSection *section;
	Media::PaperSize *paper;
	if (this->items->GetCount() == 0)
		return false;
	pStatus.currSection = 0;
	doc->SetDocName(this->docName);
	section = this->items->GetItem(0);
	NEW_CLASS(paper, Media::PaperSize(section->GetPaperType()));
	doc->SetNextPagePaperSizeMM(paper->GetWidthMM(), paper->GetHeightMM());
	doc->SetNextPageOrientation(section->IsLandscape()?Media::IPrintDocument::PageOrientation::Landscape:Media::IPrintDocument::PageOrientation::Portrait);
	DEL_CLASS(paper);
	return true;
}

Bool Text::Doc::TextDocument::PrintPage(Media::DrawImage *printPage)
{
	/////////////////////////
	return false;
}

Bool Text::Doc::TextDocument::EndPrint(Media::IPrintDocument *doc)
{
	/////////////////////////
	return true;
}
