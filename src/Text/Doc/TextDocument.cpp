#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/Doc/TextDocument.h"

Text::Doc::TextDocument::TextDocument() : IO::ParsedObject(CSTR("Untitled"))
{
	this->docName = 0;
	this->pflags = (Text::Doc::TextDocument::PropertiesFlags)0;
	SetDocumentName(CSTR("Untitled"));
}

Text::Doc::TextDocument::TextDocument(Text::CStringNN name) : IO::ParsedObject(name)
{
	this->docName = 0;
	this->pflags = (Text::Doc::TextDocument::PropertiesFlags)0;
	SetDocumentName(name);
}

Text::Doc::TextDocument::~TextDocument()
{
	this->items.DeleteAll();
	SDEL_STRING(this->docName);
}

IO::ParserType Text::Doc::TextDocument::GetParserType() const
{
	return IO::ParserType::TextDocument;
}

void Text::Doc::TextDocument::SetDocumentName(Text::CString docName)
{
	SDEL_STRING(this->docName);
	this->docName = Text::String::New(docName).Ptr();
}

UTF8Char *Text::Doc::TextDocument::GetDocumentName(UTF8Char *docName) const
{
	if (this->docName == 0)
		return 0;
	return this->docName->ConcatTo(docName);
}

void Text::Doc::TextDocument::SetTextColor(UInt32 textColor)
{
	this->textColor = textColor;
	this->pflags = (PropertiesFlags)(this->pflags | PF_TEXTCOLOR);
}

Bool Text::Doc::TextDocument::GetTextColor(UInt32 *textColor) const
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

Bool Text::Doc::TextDocument::GetBGColor(UInt32 *bgColor) const
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

Bool Text::Doc::TextDocument::GetLinkColor(UInt32 *linkColor) const
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

Bool Text::Doc::TextDocument::GetActiveLinkColor(UInt32 *activeLinkColor) const
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

Bool Text::Doc::TextDocument::GetVisitedLinkColor(UInt32 *visitedLinkColor) const
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

UOSInt Text::Doc::TextDocument::Add(NN<Text::Doc::DocSection> val)
{
	return this->items.Add(val);
}

UOSInt Text::Doc::TextDocument::GetCount() const
{
	return this->items.GetCount();
}

NN<Text::Doc::DocSection> Text::Doc::TextDocument::GetItemNoCheck(UOSInt index) const
{
	return this->items.GetItemNoCheck(index);
}

Optional<Text::Doc::DocSection> Text::Doc::TextDocument::GetItem(UOSInt index) const
{
	return this->items.GetItem(index);
}

Bool Text::Doc::TextDocument::BeginPrint(NN<Media::IPrintDocument> doc)
{
	NN<Text::Doc::DocSection> section;
	Media::PaperSize *paper;
	if (this->items.GetCount() == 0)
		return false;
	pStatus.currSection = 0;
	doc->SetDocName(this->docName);
	section = this->items.GetItemNoCheck(0);
	NEW_CLASS(paper, Media::PaperSize(section->GetPaperType()));
	doc->SetNextPagePaperSizeMM(paper->GetWidthMM(), paper->GetHeightMM());
	doc->SetNextPageOrientation(section->IsLandscape()?Media::IPrintDocument::PageOrientation::Landscape:Media::IPrintDocument::PageOrientation::Portrait);
	DEL_CLASS(paper);
	return true;
}

Bool Text::Doc::TextDocument::PrintPage(NN<Media::DrawImage> printPage)
{
	/////////////////////////
	return false;
}

Bool Text::Doc::TextDocument::EndPrint(NN<Media::IPrintDocument> doc)
{
	/////////////////////////
	return true;
}
