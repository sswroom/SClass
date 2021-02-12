#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/StringBuilder.h"
#include "Text/XMLDOM.h"
#include "IO/MemoryStream.h"
#include "IO/FileStream.h"
#include "Net/TCPClient.h"
#include "Net/HTTPClient.h"
#include "Map/MapBar/MapBarRevGeo.h"

Map::MapBar::MapBarRevGeo::MapBarRevGeo(Net::SocketFactory *sockf, IO::Writer *errWriter, Map::MapBar::MapBarAdjuster *adjuster, Int32 imgWidth, Int32 imgHeight)
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

WChar *Map::MapBar::MapBarRevGeo::SearchName(WChar *buff, Double lat, Double lon, Int32 lcid)
{
	Double srcPt[2];
	Double destPt[2];
	srcPt[0] = lat;
	srcPt[1] = lon;
	this->adjuster->AdjustPoints(srcPt, destPt, this->imgWidth, this->imgHeight, 1);
	return SearchNameAdjusted(buff, destPt[0], destPt[1], lcid);
}

WChar *Map::MapBar::MapBarRevGeo::CacheName(WChar *buff, Double lat, Double lon, Int32 lcid)
{
	return SearchName(buff, lat, lon, lcid);
}

WChar *Map::MapBar::MapBarRevGeo::SearchNameAdjusted(WChar *buff, Double adjLat, Double adjLon, Int32 lcid)
{
	Text::StringBuilder sb;
	Net::HTTPClient *cli;
	IO::MemoryStream mstm;
	OSInt readSize;
	UInt8 *dataBbuff;
	UInt8 *xmlBuff;
	OSInt buffSize;
	IO::FileStream *fs;
	Text::XMLDocument *xmlDoc;
	Text::XMLNode *resultNode;
	Text::XMLNode *node;
	Text::EncodingFactory encFact;

	dataBbuff = MemAlloc(UInt8, 2048);


	sb.Append(L"http://geocode.mapbar.com/inverse/getInverseGeocoding.jsp?customer=2&detail=1&road=1&zoom=11&latlon=");
	sb.Append(adjLon);
	sb.Append(L",");
	sb.Append(adjLat);
	if (lcid == 0x0C04 || lcid == 0x1004 || lcid == 0x1404 || lcid == 0x0404)
	{
		sb.Append(L"&cn=2");
	}
	else
	{
		sb.Append(L"&cn=1");
	}

	NEW_CLASS(cli, Net::HTTPClient(sockf, sb.ToString(), L"GET", false));
	while ((readSize = cli->Read(dataBbuff, 2048)) > 0)
	{
		mstm.Write(dataBbuff, readSize);
	}
	DEL_CLASS(cli);
	xmlBuff = mstm.GetBuff(&buffSize);
	NEW_CLASS(fs, IO::FileStream(L"MapBarRevGeo.xml", IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE));
	fs->Write(xmlBuff, buffSize);
	DEL_CLASS(fs);

	*buff = 0;
	NEW_CLASS(xmlDoc, Text::XMLDocument());
	xmlDoc->ParseBuff(&encFact, xmlBuff, buffSize);
	resultNode = xmlDoc->SearchFirstNode(L"/result");
	if (resultNode)
	{
		node = resultNode->SearchFirstNode(L"/nation");
		if (node)
		{
			sb.ClearStr();
			node->GetInnerText(&sb);
			buff = Text::StrConcat(buff, sb.ToString());
		}

		node = resultNode->SearchFirstNode(L"/province");
		if (node)
		{
			sb.ClearStr();
			node->GetInnerText(&sb);
			if (Text::StrCompare(sb.ToString(), L"特別行政區") != 0 && Text::StrCompare(sb.ToString(), L"特別行政区") != 0)
			{
				buff = Text::StrConcat(buff, sb.ToString());
			}
		}

		node = resultNode->SearchFirstNode(L"/city");
		if (node)
		{
			sb.ClearStr();
			node->GetInnerText(&sb);
			buff = Text::StrConcat(buff, sb.ToString());
		}

		node = resultNode->SearchFirstNode(L"/dist");
		if (node)
		{
			sb.ClearStr();
			node->GetInnerText(&sb);
			buff = Text::StrConcat(buff, sb.ToString());
		}

		node = resultNode->SearchFirstNode(L"/area");
		if (node)
		{
			sb.ClearStr();
			node->GetInnerText(&sb);
			buff = Text::StrConcat(buff, sb.ToString());
		}

		node = resultNode->SearchFirstNode(L"/town");
		if (node)
		{
			sb.ClearStr();
			node->GetInnerText(&sb);
			buff = Text::StrConcat(buff, sb.ToString());
		}

		node = resultNode->SearchFirstNode(L"/village");
		if (node)
		{
			sb.ClearStr();
			node->GetInnerText(&sb);
			buff = Text::StrConcat(buff, sb.ToString());
		}

		node = resultNode->SearchFirstNode(L"/road/roadname");
		if (node)
		{
			sb.ClearStr();
			node->GetInnerText(&sb);
			buff = Text::StrConcat(buff, sb.ToString());
		}

		node = resultNode->SearchFirstNode(L"/poi");
		if (node)
		{
			sb.ClearStr();
			node->GetInnerText(&sb);
			if (sb.ToString()[0])
			{
				buff = Text::StrConcat(buff, L", 近");
				buff = Text::StrConcat(buff, sb.ToString());
			}
		}
	}
	DEL_CLASS(xmlDoc);

	MemFree(dataBbuff);
	return buff;
}
