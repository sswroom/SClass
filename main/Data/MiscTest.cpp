#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/ProtoHdlr/ProtoJMVL01Handler.h"
#include "Text/CPPText.h"
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

Int32 MyMain(Core::IProgControl *progCtrl)
{
	return Test1();
}
