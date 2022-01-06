#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "Map/MapBar/MapBarAdjuster.h"
#include "Net/HTTPClient.h"
#include "Net/TCPClient.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilder.h"
#include "Text/XMLDOM.h"

Map::MapBar::MapBarAdjuster::MapBarAdjuster(Net::SocketFactory *sockf, IO::Writer *errWriter)
{
	this->sockf = sockf;
	this->errWriter = errWriter;
}

Map::MapBar::MapBarAdjuster::~MapBarAdjuster()
{
}

Bool Map::MapBar::MapBarAdjuster::AdjustPoints(Double *srcLatLons, Double *destLatLons, Int32 imgWidth, Int32 imgHeight, OSInt nPoints)
{
	UInt8 *buff;
	UInt8 *xmlBuff;
	Net::HTTPClient *cli;
	IO::MemoryStream *mstm;
	Text::XMLDocument *xmlDoc;
	Text::EncodingFactory encFact;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	OSInt currPoint;
	OSInt nextPoint;
	OSInt i;
	OSInt cnt;
	UOSInt readSize;
	UOSInt objCnt;
	Int32 j;
	Text::XMLNode **xmlNodes;
	Text::XMLNode *node;
	Bool valid;

	buff = MemAlloc(UInt8, 2048);
	NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"Map.MapBar.MapBarAdjuster.AdjustPoints.mstm"));
	currPoint = 0;
	while (currPoint < nPoints)
	{
		nextPoint = currPoint + 100;
		if (nextPoint > nPoints)
			nextPoint = nPoints;

		sb.ClearStr();
		sb.AppendC(UTF8STRC("http://geocode.mapbar.com/decode/getLatLon.jsp?latlon="));
		cnt = nextPoint - currPoint;
		i = 0;
		while (i < cnt)
		{
			if (i > 0)
			{
				sb.AppendC(UTF8STRC(";"));
			}
			Text::SBAppendF64(&sb, destLatLons[((currPoint + i) << 1) + 1] = srcLatLons[((currPoint + i) << 1) + 1]);
			sb.AppendC(UTF8STRC(","));
			Text::SBAppendF64(&sb, destLatLons[((currPoint + i) << 1) + 0] = srcLatLons[((currPoint + i) << 1) + 0]);
			i++;
		}
		if (imgWidth != 0 || imgHeight != 0)
		{
			sb.AppendC(UTF8STRC("&width="));
			sb.AppendI32(imgWidth);
			sb.AppendC(UTF8STRC("&height="));
			sb.AppendI32(imgHeight);
		}
		sb.AppendC(UTF8STRC("&customer=2"));

		mstm->Clear();
		cli = Net::HTTPClient::CreateConnect(sockf, 0, sb.ToString(), "GET", false);
		if (cli)
		{
			while ((readSize = cli->Read(buff, 2048)) > 0)
			{
				mstm->Write(buff, readSize);
			}
			DEL_CLASS(cli);
		}

		xmlBuff = mstm->GetBuff(&readSize);
		valid = false;
		NEW_CLASS(xmlDoc, Text::XMLDocument());
		if (xmlDoc->ParseBuff(&encFact, xmlBuff, readSize))
		{
			xmlNodes = xmlDoc->SearchNode((const UTF8Char*)"/result/pois/item", &objCnt);
			if (xmlNodes)
			{
				if (objCnt == cnt)
				{
					valid = true;
					i = objCnt;
					while (i-- > 0)
					{
						node = xmlNodes[i]->SearchFirstNode((const UTF8Char*)"/@id");
						if (node == 0)
						{
							valid = false;
						}
						else
						{
							j = node->value->ToInt32();
							if (j >= 0 && j < cnt)
							{
								node = xmlNodes[i]->SearchFirstNode((const UTF8Char*)"/lat");
								if (node)
								{
									sb2.ClearStr();
									node->GetInnerText(&sb2);
									destLatLons[((currPoint + j) << 1) + 0] = Text::StrToDouble(sb2.ToString());
								}
								else
								{
									valid = false;
								}
								node = xmlNodes[i]->SearchFirstNode((const UTF8Char*)"/lon");
								if (node)
								{
									sb2.ClearStr();
									node->GetInnerText(&sb2);
									destLatLons[((currPoint + j) << 1) + 1] = Text::StrToDouble(sb2.ToString());
								}
								else
								{
									valid = false;
								}
							}
							else
							{
								valid = false;
							}
						}
					}
				}
				xmlDoc->ReleaseSearch(xmlNodes);
			}
		}
		DEL_CLASS(xmlDoc);

		if (!valid)
		{
			this->errWriter->WriteLineC(UTF8STRC("Request result not valid"));
		}

		currPoint = nextPoint;
	}
	DEL_CLASS(mstm);
	MemFree(buff);

	return true;
}
