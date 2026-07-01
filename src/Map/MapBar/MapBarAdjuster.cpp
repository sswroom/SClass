#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "Map/MapBar/MapBarAdjuster.h"
#include "Net/HTTPClient.h"
#include "Net/TCPClient.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilder.hpp"
#include "Text/XMLDOM.h"

Map::MapBar::MapBarAdjuster::MapBarAdjuster(NN<Net::SocketFactory> sockf, IO::Writer *errWriter)
{
	this->sockf = sockf;
	this->errWriter = errWriter;
}

Map::MapBar::MapBarAdjuster::~MapBarAdjuster()
{
}

Bool Map::MapBar::MapBarAdjuster::AdjustPoints(Double *srcLatLons, Double *destLatLons, Int32 imgWidth, Int32 imgHeight, IntOS nPoints)
{
	UnsafeArray<UInt8> buff;
	UnsafeArray<UInt8> xmlBuff;
	NN<Net::HTTPClient> cli;
	NN<IO::MemoryStream> mstm;
	NN<Text::XMLDocument> xmlDoc;
	Text::EncodingFactory encFact;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	IntOS currPoint;
	IntOS nextPoint;
	IntOS i;
	IntOS cnt;
	UIntOS readSize;
	UIntOS objCnt;
	Int32 j;
	UnsafeArray<NN<Text::XMLNode>> xmlNodes;
	NN<Text::XMLNode> node;
	Bool valid;

	buff = MemAllocArr(UInt8, 2048);
	NEW_CLASSNN(mstm, IO::MemoryStream());
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
			sb.AppendDouble(destLatLons[((currPoint + i) << 1) + 1] = srcLatLons[((currPoint + i) << 1) + 1]);
			sb.AppendC(UTF8STRC(","));
			sb.AppendDouble(destLatLons[((currPoint + i) << 1) + 0] = srcLatLons[((currPoint + i) << 1) + 0]);
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
		cli = Net::HTTPClient::CreateConnect(sockf, nullptr, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, false);
		while ((readSize = cli->Read(Data::ByteArray(buff, 2048))) > 0)
		{
			mstm->Write(Data::ByteArrayR(buff, readSize));
		}
		cli.Delete();

		xmlBuff = mstm->GetBuff(readSize);
		valid = false;
		NEW_CLASSNN(xmlDoc, Text::XMLDocument());
		if (xmlDoc->ParseBuff(encFact, xmlBuff, readSize))
		{
			xmlNodes = xmlDoc->SearchNode(CSTR("/result/pois/item"), objCnt);
			if (objCnt == cnt)
			{
				valid = true;
				i = objCnt;
				while (i-- > 0)
				{
					if (!xmlNodes[i]->SearchFirstNode(CSTR("/@id")).SetTo(node))
					{
						valid = false;
					}
					else
					{
						j = Text::String::OrEmpty(node->value)->ToInt32();
						if (j >= 0 && j < cnt)
						{
							if (xmlNodes[i]->SearchFirstNode(CSTR("/lat")).SetTo(node))
							{
								sb2.ClearStr();
								node->GetInnerText(sb2);
								destLatLons[((currPoint + j) << 1) + 0] = sb2.ToDoubleOrNAN();
							}
							else
							{
								valid = false;
							}
							if (xmlNodes[i]->SearchFirstNode(CSTR("/lon")).SetTo(node))
							{
								sb2.ClearStr();
								node->GetInnerText(sb2);
								destLatLons[((currPoint + j) << 1) + 1] = sb2.ToDoubleOrNAN();
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
		xmlDoc.Delete();

		if (!valid)
		{
			this->errWriter->WriteLine(CSTR("Request result not valid"));
		}

		currPoint = nextPoint;
	}
	mstm.Delete();
	MemFreeArr(buff);

	return true;
}
