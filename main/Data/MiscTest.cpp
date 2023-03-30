#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/ProtoHdlr/ProtoJMVL01Handler.h"
#include "Text/CPPText.h"
#include "Text/StringTool.h"
#include "Text/UTF8Writer.h"
#include "Text/XMLReader.h"
#include <stdio.h>

class ProtoListener : public IO::IProtocolHandler::DataListener
{
public:
	virtual void DataParsed(IO::Stream *stm, void *stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize)
	{
		printf("Received cmdType 0x%x, size=%d\r\n", cmdType, (UInt32)cmdSize);
	}

	virtual void DataSkipped(IO::Stream *stm, void *stmObj, const UInt8 *buff, UOSInt buffSize)
	{

	}
};

Int32 Test0()
{
	UInt8 data[4096];
	UOSInt dataSize = 0;
	UOSInt dataLeft;
	ProtoListener listener;
	IO::ProtoHdlr::ProtoJMVL01Handler protoHdlr(&listener, 0);
	IO::FileStream fs(CSTR("/home/sswroom/Progs/Temp/20220519 JM-VL01/1652961383648_B6EF576F_4418r.dat"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	while (true)
	{
		dataLeft = fs.Read(&data[dataSize], 4096 - dataSize);
		if (dataLeft == 0)
		{
			break;
		}
		dataSize += dataLeft;
		dataLeft = protoHdlr.ParseProtocol(0, 0, 0, data, dataSize);
		if (dataLeft == dataSize)
		{

		}
		else if (dataLeft == 0)
		{
			dataSize = 0;
		}
		else
		{
			MemCopyO(data, &data[dataSize - dataLeft], dataLeft);
			dataSize = dataLeft;
		}
	}
	return 0;
}

Int32 Test1()
{
	Text::CString srcFile = CSTR("/home/sswroom/Progs/Temp/20230302 ArcGIS CSys/pcs.html");
	Text::CString destFile = CSTR("/home/sswroom/Progs/Temp/20230302 ArcGIS CSys//pcs.txt");
	Text::EncodingFactory encFact;

	IO::ConsoleWriter console;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	IO::FileStream srcFS(srcFile, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (!srcFS.IsError())
	{
		IO::FileStream destFS(destFile, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		Text::UTF8Writer writer(&destFS);
		Text::XMLReader reader(&encFact, &srcFS, Text::XMLReader::PM_HTML);
		UOSInt rowType = 0;
		while (reader.ReadNext())
		{
			if (reader.GetNodeType() == Text::XMLNode::NodeType::Element && reader.GetNodeText()->Equals(UTF8STRC("TABLE")))
			{
				while (reader.NextElement())
				{
					if (rowType == 0)
					{
						rowType = 1;
						reader.SkipElement();
					}
					else if (rowType == 1)
					{
						rowType = 2;
						sb.ClearStr();
						sb.AppendUTF8Char('{');
						if (reader.NextElement())
						{
							reader.ReadNodeText(&sb);
							if (reader.NextElement())
							{
								sb2.ClearStr();
								reader.ReadNodeText(&sb2);
								sb.AppendC(UTF8STRC(", UTF8STRC("));
								Text::CPPText::ToCPPString(&sb, sb2.v, sb2.leng);
								sb.AppendUTF8Char(')');
								while (reader.NextElement())
								{
									reader.SkipElement();
								}
							}
						}
					}
					else
					{
						rowType = 1;
						if (reader.NextElement())
						{
							sb2.ClearStr();
							reader.ReadNodeText(&sb2);
							sb.AppendC(UTF8STRC(", UTF8STRC("));
							Text::CPPText::ToCPPString(&sb, sb2.v, sb2.leng);
							sb.AppendUTF8Char(')');
							while (reader.NextElement())
							{
								reader.SkipElement();
							}
						}
						sb.AppendC(UTF8STRC("},"));
						writer.WriteLineCStr(sb.ToCString());
					}
				}
				break;
			}
		}
	}
	return 0;
}

Bool InZone(Data::ArrayList<Double> *lats, Data::ArrayList<Double> *lons, Math::Coord2DDbl pos)
{
	Double thisPtX;
	Double thisPtY;
	Double lastPtX;
	Double lastPtY;
	UOSInt j;
	UOSInt l;
	Int32 leftCnt = 0;
	Double tmpX;

	l = lons->GetCount();
	lastPtX = lons->GetItem(0);
	lastPtY = lats->GetItem(0);
	while (l-- > 0)
	{
		thisPtX = lons->GetItem(l);
		thisPtY = lats->GetItem(l);
		j = 0;
		if (lastPtY > pos.y)
			j += 1;
		if (thisPtY > pos.y)
			j += 1;

		printf("j = %d\r\n", (UInt32)j);
		if (j == 1)
		{
			tmpX = lastPtX - (lastPtX - thisPtX) * (lastPtY - pos.y) / (lastPtY - thisPtY);
			printf("tmpX = %lf, x = %lf\r\n", tmpX, pos.x);
			if (tmpX < pos.x)
				leftCnt++;
		}
		lastPtX = thisPtX;
		lastPtY = thisPtY;
	}

	printf("leftCnt = %d\r\n", (UInt32)leftCnt);
	return (leftCnt & 1) != 0;
}

Int32 InPolygonTest()
{
	Text::CString latStr = CSTR("22.362539668716,22.35545519929,22.354899539392,22.356447443597,22.361984037077,22.362738108049");
	Text::CString lonStr = CSTR("114.08715410232,114.08588809966,114.09921331405,114.11195917129,114.11129398346,114.08721847534");
	Double lat = 22.361138;
	Double lon = 114.09073483333;
	Data::ArrayList<Double> lats;
	Data::ArrayList<Double> lons;
	if (Text::StringTool::SplitAsDouble(latStr, ',', &lats) && Text::StringTool::SplitAsDouble(lonStr, ',', &lons))
	{
		printf("InPolygon = %d\r\n", InZone(&lats, &lons, Math::Coord2DDbl(lon, lat))?1:0);
	}
	return 0;
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	return InPolygonTest();
}
