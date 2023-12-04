#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/ByteBuffer.h"
#include "IO/StreamData.h"
#include "Media/IImgResizer.h"
#include "Media/FrameInfo.h"
#include "Media/Image.h"
#include "Parser/ParserList.h"

Parser::ParserList::ParserList()
{
}

Parser::ParserList::~ParserList()
{
	IO::ParserBase *parser;
	UOSInt i = this->filePArr.GetCount();
	while (i-- > 0)
	{
		parser = this->filePArr.GetItem(i);
		DEL_CLASS(parser);
	}
	i = this->objPArr.GetCount();
	while (i-- > 0)
	{
		parser = this->objPArr.GetItem(i);
		DEL_CLASS(parser);
	}
}

void Parser::ParserList::AddFileParser(IO::FileParser *parser)
{
	this->filePArr.Add(parser);
	parser->SetParserList(this);
}

void Parser::ParserList::AddObjectParser(IO::ObjectParser *parser)
{
	this->objPArr.Add(parser);
	parser->SetParserList(this);
}

void Parser::ParserList::SetCodePage(UInt32 codePage)
{
	IO::ParserBase *parser;
	UOSInt i = this->filePArr.GetCount();
	while (i-- > 0)
	{
		parser = this->filePArr.GetItem(i);
		parser->SetCodePage(codePage);
	}
	i = this->objPArr.GetCount();
	while (i-- > 0)
	{
		parser = this->objPArr.GetItem(i);
		parser->SetCodePage(codePage);
	}
}

void Parser::ParserList::SetMapManager(Map::MapManager *mapMgr)
{
	IO::ParserBase *parser;
	UOSInt i = this->filePArr.GetCount();
	while (i-- > 0)
	{
		parser = this->filePArr.GetItem(i);
		parser->SetMapManager(mapMgr);
	}
	i = this->objPArr.GetCount();
	while (i-- > 0)
	{
		parser = this->objPArr.GetItem(i);
		parser->SetMapManager(mapMgr);
	}
}

void Parser::ParserList::SetEncFactory(Text::EncodingFactory *encFact)
{
	IO::ParserBase *parser;
	UOSInt i = this->filePArr.GetCount();
	while (i-- > 0)
	{
		parser = this->filePArr.GetItem(i);
		parser->SetEncFactory(encFact);
	}
	i = this->objPArr.GetCount();
	while (i-- > 0)
	{
		parser = this->objPArr.GetItem(i);
		parser->SetEncFactory(encFact);
	}
}

void Parser::ParserList::SetWebBrowser(Net::WebBrowser *browser)
{
	IO::ParserBase *parser;
	UOSInt i = this->filePArr.GetCount();
	while (i-- > 0)
	{
		parser = this->filePArr.GetItem(i);
		parser->SetWebBrowser(browser);
	}
	i = this->objPArr.GetCount();
	while (i-- > 0)
	{
		parser = this->objPArr.GetItem(i);
		parser->SetWebBrowser(browser);
	}
}

void Parser::ParserList::SetSocketFactory(NotNullPtr<Net::SocketFactory> sockf)
{
	IO::ParserBase *parser;
	UOSInt i = this->filePArr.GetCount();
	while (i-- > 0)
	{
		parser = this->filePArr.GetItem(i);
		parser->SetSocketFactory(sockf);
	}
	i = this->objPArr.GetCount();
	while (i-- > 0)
	{
		parser = this->objPArr.GetItem(i);
		parser->SetSocketFactory(sockf);
	}
}

void Parser::ParserList::SetSSLEngine(Optional<Net::SSLEngine> ssl)
{
	IO::ParserBase *parser;
	UOSInt i = this->filePArr.GetCount();
	while (i-- > 0)
	{
		parser = this->filePArr.GetItem(i);
		parser->SetSSLEngine(ssl);
	}
	i = this->objPArr.GetCount();
	while (i-- > 0)
	{
		parser = this->objPArr.GetItem(i);
		parser->SetSSLEngine(ssl);
	}
}

void Parser::ParserList::SetArcGISPRJParser(Math::ArcGISPRJParser *prjParser)
{
	IO::ParserBase *parser;
	UOSInt i;
	UOSInt j = this->filePArr.GetCount();
	i = 0;
	while (i < j)
	{
		parser = this->filePArr.GetItem(i);
		parser->SetArcGISPRJParser(prjParser);
		i++;
	}
	i = 0;
	j = this->objPArr.GetCount();
	while (i < j)
	{
		parser = this->objPArr.GetItem(i);
		parser->SetArcGISPRJParser(prjParser);
		i++;
	}
}

void Parser::ParserList::SetLogTool(IO::LogTool *log)
{
	IO::ParserBase *parser;
	UOSInt i;
	UOSInt j = this->filePArr.GetCount();
	i = 0;
	while (i < j)
	{
		parser = this->filePArr.GetItem(i);
		parser->SetLogTool(log);
		i++;
	}
	i = 0;
	j = this->objPArr.GetCount();
	while (i < j)
	{
		parser = this->objPArr.GetItem(i);
		parser->SetLogTool(log);
		i++;
	}
}

void Parser::ParserList::PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t)
{
	IO::ParserBase *parser;
	UOSInt i;
	UOSInt j = this->filePArr.GetCount();
	i = 0;
	while (i < j)
	{
		parser = this->filePArr.GetItem(i);
		parser->PrepareSelector(selector, t);
		i++;
	}
	i = 0;
	j = this->objPArr.GetCount();
	while (i < j)
	{
		parser = this->objPArr.GetItem(i);
		parser->PrepareSelector(selector, t);
		i++;
	}
}

IO::ParsedObject *Parser::ParserList::ParseFile(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType *t, IO::ParserType targetType)
{
	UOSInt i = 0;
	UOSInt j = this->filePArr.GetCount();
	IO::FileParser *parser;
	IO::ParsedObject *result;
	if (fd->GetDataSize() <= 0)
		return 0;
	Data::ByteBuffer hdr(IO::FileParser::hdrSize);
	UOSInt readSize;
	readSize = fd->GetRealData(0, IO::FileParser::hdrSize, hdr);
	if (readSize != IO::FileParser::hdrSize)
	{
		if (readSize != fd->GetDataSize())
		{
			UOSInt hdrSize = readSize;
			while (true)
			{
				readSize = fd->GetRealData(hdrSize, IO::FileParser::hdrSize - hdrSize, hdr.SubArray(hdrSize));
				if (readSize == 0)
				{
					return 0;
				}
				hdrSize += readSize;
				if (hdrSize == IO::FileParser::hdrSize)
				{
					break;
				}
				else if (hdrSize == fd->GetDataSize())
				{
					break;
				}
			}	
		}
	}
	while (i < j)
	{
		parser = this->filePArr.GetItem(i);
		if ((result = parser->ParseFileHdr(fd, pkgFile, targetType, hdr.Ptr())) != 0)
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

IO::ParsedObject *Parser::ParserList::ParseFile(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType *t)
{
	return ParseFile(fd, pkgFile, t, IO::ParserType::Unknown);
}

IO::ParsedObject *Parser::ParserList::ParseFile(NotNullPtr<IO::StreamData> fd, IO::ParserType *t)
{
	return ParseFile(fd, 0, t, IO::ParserType::Unknown);
}

IO::ParsedObject *Parser::ParserList::ParseFileType(NotNullPtr<IO::StreamData> fd, IO::ParserType t)
{
	IO::ParserType pt;
	NotNullPtr<IO::ParsedObject> pobj;
	IO::ParsedObject *pobj2 = this->ParseFile(fd, 0, &pt, t);
	while (pobj.Set(pobj2))
	{
		if (pt == t)
			return pobj.Ptr();
		pobj2 = this->ParseObjectType(pobj, pt, t);
		pobj.Delete();
	}
	return 0;
}

IO::ParsedObject *Parser::ParserList::ParseObject(NotNullPtr<IO::ParsedObject> pobj, OptOut<IO::ParserType> t)
{
	return ParseObjectType(pobj, t, IO::ParserType::Unknown);
}

IO::ParsedObject *Parser::ParserList::ParseObjectType(NotNullPtr<IO::ParsedObject> pobj, OptOut<IO::ParserType> t, IO::ParserType targetType)
{
	UOSInt i = 0;
	UOSInt j = this->objPArr.GetCount();
	IO::ObjectParser *parser;
	IO::ParsedObject *result;
	while (i < j)
	{
		parser = this->objPArr.GetItem(i);
		if ((result = parser->ParseObject(pobj, 0, targetType)) != 0)
		{
			if (t.IsNotNull())
			{
				t.SetNoCheck(result->GetParserType());
			}
			return result;
		}
		i++;
	}
	return 0;
}
