#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "IO/IStreamData.h"
#include "Media/IImgResizer.h"
#include "Media/FrameInfo.h"
#include "Media/Image.h"
#include "Parser/ParserList.h"

Parser::ParserList::ParserList()
{
	NEW_CLASS(this->filePArr, Data::ArrayList<IO::IFileParser*>());
	NEW_CLASS(this->objPArr, Data::ArrayList<IO::IObjectParser*>());
}

Parser::ParserList::~ParserList()
{
	IO::IParser *parser;
	UOSInt i = this->filePArr->GetCount();
	while (i-- > 0)
	{
		parser = this->filePArr->GetItem(i);
		DEL_CLASS(parser);
	}
	DEL_CLASS(this->filePArr);
	i = this->objPArr->GetCount();
	while (i-- > 0)
	{
		parser = this->objPArr->GetItem(i);
		DEL_CLASS(parser);
	}
	DEL_CLASS(this->objPArr);
}

void Parser::ParserList::AddFileParser(IO::IFileParser *parser)
{
	this->filePArr->Add(parser);
	parser->SetParserList(this);
}

void Parser::ParserList::AddObjectParser(IO::IObjectParser *parser)
{
	this->objPArr->Add(parser);
	parser->SetParserList(this);
}

void Parser::ParserList::SetCodePage(UInt32 codePage)
{
	IO::IParser *parser;
	UOSInt i = this->filePArr->GetCount();
	while (i-- > 0)
	{
		parser = this->filePArr->GetItem(i);
		parser->SetCodePage(codePage);
	}
	i = this->objPArr->GetCount();
	while (i-- > 0)
	{
		parser = this->objPArr->GetItem(i);
		parser->SetCodePage(codePage);
	}
}

void Parser::ParserList::SetMapManager(Map::MapManager *mapMgr)
{
	IO::IParser *parser;
	UOSInt i = this->filePArr->GetCount();
	while (i-- > 0)
	{
		parser = this->filePArr->GetItem(i);
		parser->SetMapManager(mapMgr);
	}
	i = this->objPArr->GetCount();
	while (i-- > 0)
	{
		parser = this->objPArr->GetItem(i);
		parser->SetMapManager(mapMgr);
	}
}

void Parser::ParserList::SetEncFactory(Text::EncodingFactory *encFact)
{
	IO::IParser *parser;
	UOSInt i = this->filePArr->GetCount();
	while (i-- > 0)
	{
		parser = this->filePArr->GetItem(i);
		parser->SetEncFactory(encFact);
	}
	i = this->objPArr->GetCount();
	while (i-- > 0)
	{
		parser = this->objPArr->GetItem(i);
		parser->SetEncFactory(encFact);
	}
}

void Parser::ParserList::SetWebBrowser(Net::WebBrowser *browser)
{
	IO::IParser *parser;
	UOSInt i = this->filePArr->GetCount();
	while (i-- > 0)
	{
		parser = this->filePArr->GetItem(i);
		parser->SetWebBrowser(browser);
	}
	i = this->objPArr->GetCount();
	while (i-- > 0)
	{
		parser = this->objPArr->GetItem(i);
		parser->SetWebBrowser(browser);
	}
}

void Parser::ParserList::SetSocketFactory(Net::SocketFactory *sockf)
{
	IO::IParser *parser;
	UOSInt i = this->filePArr->GetCount();
	while (i-- > 0)
	{
		parser = this->filePArr->GetItem(i);
		parser->SetSocketFactory(sockf);
	}
	i = this->objPArr->GetCount();
	while (i-- > 0)
	{
		parser = this->objPArr->GetItem(i);
		parser->SetSocketFactory(sockf);
	}
}

void Parser::ParserList::SetSSLEngine(Net::SSLEngine *ssl)
{
	IO::IParser *parser;
	UOSInt i = this->filePArr->GetCount();
	while (i-- > 0)
	{
		parser = this->filePArr->GetItem(i);
		parser->SetSSLEngine(ssl);
	}
	i = this->objPArr->GetCount();
	while (i-- > 0)
	{
		parser = this->objPArr->GetItem(i);
		parser->SetSSLEngine(ssl);
	}
}

void Parser::ParserList::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	IO::IParser *parser;
	UOSInt i;
	UOSInt j = this->filePArr->GetCount();
	i = 0;
	while (i < j)
	{
		parser = this->filePArr->GetItem(i);
		parser->PrepareSelector(selector, t);
		i++;
	}
	i = 0;
	j = this->objPArr->GetCount();
	while (i < j)
	{
		parser = this->objPArr->GetItem(i);
		parser->PrepareSelector(selector, t);
		i++;
	}
}

IO::ParsedObject *Parser::ParserList::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType *t, IO::ParsedObject::ParserType targetType)
{
	UOSInt i = 0;
	UOSInt j = this->filePArr->GetCount();
	IO::IFileParser *parser;
	IO::ParsedObject *result;
	if (fd->GetDataSize() <= 0)
		return 0;
	while (i < j)
	{
		parser = this->filePArr->GetItem(i);
		if ((result = parser->ParseFile(fd, pkgFile, targetType)) != 0)
		{
			if (t)
			{
				*t = result->GetParserType();
			}
			return result;
		}
		i++;
	}
	return 0;
}

IO::ParsedObject *Parser::ParserList::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType *t)
{
	return ParseFile(fd, pkgFile, t, IO::ParsedObject::PT_UNKNOWN);
}

IO::ParsedObject *Parser::ParserList::ParseFile(IO::IStreamData *fd, IO::ParsedObject::ParserType *t)
{
	return ParseFile(fd, 0, t, IO::ParsedObject::PT_UNKNOWN);
}

IO::ParsedObject *Parser::ParserList::ParseFileType(IO::IStreamData *fd, IO::ParsedObject::ParserType t)
{
	IO::ParsedObject::ParserType pt;
	IO::ParsedObject *pobj = this->ParseFile(fd, 0, &pt, t);
	IO::ParsedObject *pobj2;
	while (pobj)
	{
		if (pt == t)
			return pobj;
		pobj2 = this->ParseObject(pobj, &pt);
		DEL_CLASS(pobj);
		pobj = pobj2;
	}
	return 0;
}

IO::ParsedObject *Parser::ParserList::ParseObject(IO::ParsedObject *pobj, IO::ParsedObject::ParserType *t)
{
	UOSInt i = 0;
	UOSInt j = this->objPArr->GetCount();
	IO::IObjectParser *parser;
	IO::ParsedObject *result;
	while (i < j)
	{
		parser = this->objPArr->GetItem(i);
		if ((result = parser->ParseObject(pobj, 0, IO::ParsedObject::PT_UNKNOWN)) != 0)
		{
			if (t)
			{
				*t = result->GetParserType();
			}
			return result;
		}
		i++;
	}
	return 0;
}
