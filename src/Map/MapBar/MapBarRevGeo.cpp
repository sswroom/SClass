#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/MemoryStream.h"
#include "IO/FileStream.h"
#include "Map/MapBar/MapBarRevGeo.h"
#include "Net/HTTPClient.h"
#include "Net/TCPClient.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XMLDOM.h"

Map::MapBar::MapBarRevGeo::MapBarRevGeo(NotNullPtr<Net::SocketFactory> sockf, IO::Writer *errWriter, Map::MapBar::MapBarAdjuster *adjuster, Int32 imgWidth, Int32 imgHeight)
{
	this->sockf = sockf;
	this->errWriter = errWriter;
	this->adjuster = adjuster;
	this->imgWidth = imgWidth;
	this->imgHeight = imgHeight;
}

Map::MapBar::MapBarRevGeo::~MapBarRevGeo()
{
}

UTF8Char *Map::MapBar::MapBarRevGeo::SearchName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, Int32 lcid)
{
	Double srcPt[2];
	Double destPt[2];
	srcPt[0] = lat;
	srcPt[1] = lon;
	this->adjuster->AdjustPoints(srcPt, destPt, this->imgWidth, this->imgHeight, 1);
	return SearchNameAdjusted(buff, buffSize, destPt[0], destPt[1], lcid);
}

UTF8Char *Map::MapBar::MapBarRevGeo::CacheName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, Int32 lcid)
{
	return SearchName(buff, buffSize, lat, lon, lcid);
}

UTF8Char *Map::MapBar::MapBarRevGeo::SearchNameAdjusted(UTF8Char *buff, UOSInt buffSize, Double adjLat, Double adjLon, Int32 lcid)
{
	Text::StringBuilderUTF8 sb;
	NotNullPtr<Net::HTTPClient> cli;
	IO::MemoryStream mstm(UTF8STRC("Map.MapBar.MapBarRevGeo"));
	OSInt readSize;
	UInt8 *dataBbuff;
	UInt8 *xmlBuff;
	UOSInt buffSize;
	IO::FileStream *fs;
	Text::XMLDocument *xmlDoc;
	Text::XMLNode *resultNode;
	Text::XMLNode *node;
	Text::EncodingFactory encFact;

	dataBbuff = MemAlloc(UInt8, 2048);


	sb.AppendC(UTF8STRC("http://geocode.mapbar.com/inverse/getInverseGeocoding.jsp?customer=2&detail=1&road=1&zoom=11&latlon="));
	sb.AppendDouble(adjLon);
	sb.AppendC(UTF8STRC(","));
	sb.AppendDouble(adjLat);
	if (lcid == 0x0C04 || lcid == 0x1004 || lcid == 0x1404 || lcid == 0x0404)
	{
		sb.AppendC(UTF8STRC("&cn=2"));
	}
	else
	{
		sb.AppendC(UTF8STRC("&cn=1"));
	}

	cli = Net::HTTPClient::CreateConnect(sockf, 0, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, false);
	while ((readSize = cli->Read(dataBbuff, 2048)) > 0)
	{
		mstm.Write(dataBbuff, readSize);
	}
	DEL_CLASS(cli);
	xmlBuff = mstm.GetBuff(&buffSize);
	NEW_CLASS(fs, IO::FileStream(CSTR("MapBarRevGeo.xml"), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	fs->Write(xmlBuff, buffSize);
	DEL_CLASS(fs);

	*buff = 0;
	NEW_CLASS(xmlDoc, Text::XMLDocument());
	xmlDoc->ParseBuff(&encFact, xmlBuff, buffSize);
	resultNode = xmlDoc->SearchFirstNode(CSTR("/result"));
	if (resultNode)
	{
		node = resultNode->SearchFirstNode(CSTR("/nation"));
		if (node)
		{
			sb.ClearStr();
			node->GetInnerText(sb);
			buff = Text::StrConcatC(buff, sb.ToString(), sb.GetLength());
		}

		node = resultNode->SearchFirstNode(CSTR("/province"));
		if (node)
		{
			sb.ClearStr();
			node->GetInnerText(sb);
			UTF8Char str1[32];
			UTF8Char str2[32];
			Text::StrWChar_UTF8(str1, L"特別行政區");
			Text::StrWChar_UTF8(str2, L"特別行政区");
			if (!Text::StrEquals(sb.ToString(), str1) && !Text::StrEquals(sb.ToString(), str2))
			{
				buff = Text::StrConcatC(buff, sb.ToString(), sb.GetLength());
			}
		}

		node = resultNode->SearchFirstNode(CSTR("/city"));
		if (node)
		{
			sb.ClearStr();
			node->GetInnerText(sb);
			buff = Text::StrConcatC(buff, sb.ToString(), sb.GetLength());
		}

		node = resultNode->SearchFirstNode(CSTR("/dist"));
		if (node)
		{
			sb.ClearStr();
			node->GetInnerText(sb);
			buff = Text::StrConcatC(buff, sb.ToString(), sb.GetLength());
		}

		node = resultNode->SearchFirstNode(CSTR("/area"));
		if (node)
		{
			sb.ClearStr();
			node->GetInnerText(sb);
			buff = Text::StrConcatC(buff, sb.ToString(), sb.GetLength());
		}

		node = resultNode->SearchFirstNode(CSTR("/town"));
		if (node)
		{
			sb.ClearStr();
			node->GetInnerText(sb);
			buff = Text::StrConcatC(buff, sb.ToString(), sb.GetLength());
		}

		node = resultNode->SearchFirstNode(CSTR("/village"));
		if (node)
		{
			sb.ClearStr();
			node->GetInnerText(sb);
			buff = Text::StrConcatC(buff, sb.ToString(), sb.GetLength());
		}

		node = resultNode->SearchFirstNode(CSTR("/road/roadname"));
		if (node)
		{
			sb.ClearStr();
			node->GetInnerText(sb);
			buff = Text::StrConcatC(buff, sb.ToString(), sb.GetLength());
		}

		node = resultNode->SearchFirstNode(CSTR("/poi"));
		if (node)
		{
			sb.ClearStr();
			node->GetInnerText(sb);
			if (sb.ToString()[0])
			{
				buff = Text::StrWChar_UTF8(buff, L", 近");
				buff = Text::StrConcatC(buff, sb.ToString(), sb.GetLength());
			}
		}
	}
	DEL_CLASS(xmlDoc);

	MemFree(dataBbuff);
	return buff;
}
