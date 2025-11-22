#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.hpp"
#include "Data/ByteBuffer.h"
#include "IO/StreamData.h"
#include "Media/ImageResizer.h"
#include "Media/FrameInfo.h"
#include "Media/RasterImage.h"
#include "Parser/ParserList.h"

Parser::ParserList::ParserList()
{
}

Parser::ParserList::~ParserList()
{
	this->filePArr.DeleteAll();
	this->objPArr.DeleteAll();
}

void Parser::ParserList::AddFileParser(NN<IO::FileParser> parser)
{
	this->filePArr.Add(parser);
	parser->SetParserList(this);
}

void Parser::ParserList::AddObjectParser(NN<IO::ObjectParser> parser)
{
	this->objPArr.Add(parser);
	parser->SetParserList(this);
}

void Parser::ParserList::SetCodePage(UInt32 codePage)
{
	NN<IO::ParserBase> parser;
	UOSInt i = this->filePArr.GetCount();
	while (i-- > 0)
	{
		parser = this->filePArr.GetItemNoCheck(i);
		parser->SetCodePage(codePage);
	}
	i = this->objPArr.GetCount();
	while (i-- > 0)
	{
		parser = this->objPArr.GetItemNoCheck(i);
		parser->SetCodePage(codePage);
	}
}

void Parser::ParserList::SetMapManager(Optional<Map::MapManager> mapMgr)
{
	NN<IO::ParserBase> parser;
	UOSInt i = this->filePArr.GetCount();
	while (i-- > 0)
	{
		parser = this->filePArr.GetItemNoCheck(i);
		parser->SetMapManager(mapMgr);
	}
	i = this->objPArr.GetCount();
	while (i-- > 0)
	{
		parser = this->objPArr.GetItemNoCheck(i);
		parser->SetMapManager(mapMgr);
	}
}

void Parser::ParserList::SetEncFactory(Optional<Text::EncodingFactory> encFact)
{
	NN<IO::ParserBase> parser;
	UOSInt i = this->filePArr.GetCount();
	while (i-- > 0)
	{
		parser = this->filePArr.GetItemNoCheck(i);
		parser->SetEncFactory(encFact);
	}
	i = this->objPArr.GetCount();
	while (i-- > 0)
	{
		parser = this->objPArr.GetItemNoCheck(i);
		parser->SetEncFactory(encFact);
	}
}

void Parser::ParserList::SetWebBrowser(Optional<Net::WebBrowser> browser)
{
	NN<IO::ParserBase> parser;
	UOSInt i = this->filePArr.GetCount();
	while (i-- > 0)
	{
		parser = this->filePArr.GetItemNoCheck(i);
		parser->SetWebBrowser(browser);
	}
	i = this->objPArr.GetCount();
	while (i-- > 0)
	{
		parser = this->objPArr.GetItemNoCheck(i);
		parser->SetWebBrowser(browser);
	}
}

void Parser::ParserList::SetTCPClientFactory(NN<Net::TCPClientFactory> clif)
{
	NN<IO::ParserBase> parser;
	UOSInt i = this->filePArr.GetCount();
	while (i-- > 0)
	{
		parser = this->filePArr.GetItemNoCheck(i);
		parser->SetTCPClientFactory(clif);
	}
	i = this->objPArr.GetCount();
	while (i-- > 0)
	{
		parser = this->objPArr.GetItemNoCheck(i);
		parser->SetTCPClientFactory(clif);
	}
}

void Parser::ParserList::SetSSLEngine(Optional<Net::SSLEngine> ssl)
{
	NN<IO::ParserBase> parser;
	UOSInt i = this->filePArr.GetCount();
	while (i-- > 0)
	{
		parser = this->filePArr.GetItemNoCheck(i);
		parser->SetSSLEngine(ssl);
	}
	i = this->objPArr.GetCount();
	while (i-- > 0)
	{
		parser = this->objPArr.GetItemNoCheck(i);
		parser->SetSSLEngine(ssl);
	}
}

void Parser::ParserList::SetArcGISPRJParser(Optional<Math::ArcGISPRJParser> prjParser)
{
	NN<IO::ParserBase> parser;
	UOSInt i;
	UOSInt j = this->filePArr.GetCount();
	i = 0;
	while (i < j)
	{
		parser = this->filePArr.GetItemNoCheck(i);
		parser->SetArcGISPRJParser(prjParser);
		i++;
	}
	i = 0;
	j = this->objPArr.GetCount();
	while (i < j)
	{
		parser = this->objPArr.GetItemNoCheck(i);
		parser->SetArcGISPRJParser(prjParser);
		i++;
	}
}

void Parser::ParserList::SetLogTool(Optional<IO::LogTool> log)
{
	NN<IO::ParserBase> parser;
	UOSInt i;
	UOSInt j = this->filePArr.GetCount();
	i = 0;
	while (i < j)
	{
		parser = this->filePArr.GetItemNoCheck(i);
		parser->SetLogTool(log);
		i++;
	}
	i = 0;
	j = this->objPArr.GetCount();
	while (i < j)
	{
		parser = this->objPArr.GetItemNoCheck(i);
		parser->SetLogTool(log);
		i++;
	}
}

void Parser::ParserList::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	NN<IO::ParserBase> parser;
	UOSInt i;
	UOSInt j = this->filePArr.GetCount();
	i = 0;
	while (i < j)
	{
		parser = this->filePArr.GetItemNoCheck(i);
		parser->PrepareSelector(selector, t);
		i++;
	}
	i = 0;
	j = this->objPArr.GetCount();
	while (i < j)
	{
		parser = this->objPArr.GetItemNoCheck(i);
		parser->PrepareSelector(selector, t);
		i++;
	}
}

Optional<IO::ParsedObject> Parser::ParserList::ParseFile(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType)
{
	UOSInt i = 0;
	UOSInt j = this->filePArr.GetCount();
	NN<IO::FileParser> parser;
	NN<IO::ParsedObject> result;
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
		parser = this->filePArr.GetItemNoCheck(i);
		if (parser->ParseFileHdr(fd, pkgFile, targetType, hdr).SetTo(result))
		{
			return result;
		}
		i++;
	}
	return 0;
}

Optional<IO::ParsedObject> Parser::ParserList::ParseFile(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile)
{
	return ParseFile(fd, pkgFile, IO::ParserType::Unknown);
}

Optional<IO::ParsedObject> Parser::ParserList::ParseFile(NN<IO::StreamData> fd)
{
	return ParseFile(fd, 0, IO::ParserType::Unknown);
}

Optional<IO::ParsedObject> Parser::ParserList::ParseFileType(NN<IO::StreamData> fd, IO::ParserType t)
{
	NN<IO::ParsedObject> pobj;
	Optional<IO::ParsedObject> pobj2 = this->ParseFile(fd, 0, t);
	while (pobj2.SetTo(pobj))
	{
		if (pobj->GetParserType() == t || (t == IO::ParserType::ReadingDB && pobj->GetParserType() == IO::ParserType::MapLayer))
			return pobj;
		pobj2 = this->ParseObjectType(pobj, t);
		pobj.Delete();
	}
	return 0;
}

Optional<IO::ParsedObject> Parser::ParserList::ParseObject(NN<IO::ParsedObject> pobj)
{
	return ParseObjectType(pobj, IO::ParserType::Unknown);
}

Optional<IO::ParsedObject> Parser::ParserList::ParseObjectType(NN<IO::ParsedObject> pobj, IO::ParserType targetType)
{
	UOSInt i = 0;
	UOSInt j = this->objPArr.GetCount();
	NN<IO::ObjectParser> parser;
	NN<IO::ParsedObject> result;
	while (i < j)
	{
		parser = this->objPArr.GetItemNoCheck(i);
		if (parser->ParseObject(pobj, 0, targetType).SetTo(result))
		{
			return result;
		}
		i++;
	}
	return 0;
}
