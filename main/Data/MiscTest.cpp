#include "Stdafx.h"
#include "Core/Core.h"
#include "DB/MSSQLConn.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/FileUtil.h"
#include "IO/Path.h"
#include "IO/Device/AXCAN.h"
#include "IO/Device/BYDC9RHandler.h"
#include "IO/ProtoHdlr/ProtoJMVL01Handler.h"
#include "IO/StmData/MemoryDataRef.h"
#include "Manage/Process.h"
#include "Map/ESRI/ESRIFeatureServer.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "Parser/FullParserList.h"
#include "Text/CPPText.h"
#include "Text/StringTool.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"
#include "Text/XMLReader.h"
#include <stdio.h>

class ProtoListener : public IO::IProtocolHandler::DataListener
{
public:
	virtual void DataParsed(NotNullPtr<IO::Stream> stm, void *stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize)
	{
		printf("Received cmdType 0x%x, size=%d\r\n", cmdType, (UInt32)cmdSize);
	}

	virtual void DataSkipped(NotNullPtr<IO::Stream> stm, void *stmObj, const UInt8 *buff, UOSInt buffSize)
	{

	}
};

Int32 Test0()
{
	UInt8 data[4096];
	UOSInt dataSize = 0;
	UOSInt dataLeft;
	ProtoListener listener;
	IO::ProtoHdlr::ProtoJMVL01Handler protoHdlr(listener, 0);
	IO::FileStream fs(CSTR("/home/sswroom/Progs/Temp/20220519 JM-VL01/1652961383648_B6EF576F_4418r.dat"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	while (true)
	{
		dataLeft = fs.Read(Data::ByteArray(&data[dataSize], 4096 - dataSize));
		if (dataLeft == 0)
		{
			break;
		}
		dataSize += dataLeft;
		dataLeft = protoHdlr.ParseProtocol(fs, 0, 0, Data::ByteArrayR(data, dataSize));
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
	Text::CStringNN srcFile = CSTR("/home/sswroom/Progs/Temp/20230302 ArcGIS CSys/pcs.html");
	Text::CStringNN destFile = CSTR("/home/sswroom/Progs/Temp/20230302 ArcGIS CSys//pcs.txt");
	Text::EncodingFactory encFact;

	IO::ConsoleWriter console;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	IO::FileStream srcFS(srcFile, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (!srcFS.IsError())
	{
		IO::FileStream destFS(destFile, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		Text::UTF8Writer writer(destFS);
		Text::XMLReader reader(&encFact, srcFS, Text::XMLReader::PM_HTML);
		UOSInt rowType = 0;
		while (reader.ReadNext())
		{
			if (reader.GetNodeType() == Text::XMLNode::NodeType::Element && reader.GetNodeTextNN()->Equals(UTF8STRC("TABLE")))
			{
				while (!reader.NextElementName().IsNull())
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
						if (!reader.NextElementName().IsNull())
						{
							reader.ReadNodeText(sb);
							if (!reader.NextElementName().IsNull())
							{
								sb2.ClearStr();
								reader.ReadNodeText(sb2);
								sb.AppendC(UTF8STRC(", UTF8STRC("));
								Text::CPPText::ToCPPString(sb, sb2.v, sb2.leng);
								sb.AppendUTF8Char(')');
								while (!reader.NextElementName().IsNull())
								{
									reader.SkipElement();
								}
							}
						}
					}
					else
					{
						rowType = 1;
						if (!reader.NextElementName().IsNull())
						{
							sb2.ClearStr();
							reader.ReadNodeText(sb2);
							sb.AppendC(UTF8STRC(", UTF8STRC("));
							Text::CPPText::ToCPPString(sb, sb2.v, sb2.leng);
							sb.AppendUTF8Char(')');
							while (!reader.NextElementName().IsNull())
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
	Text::CStringNN latStr = CSTR("22.362539668716,22.35545519929,22.354899539392,22.356447443597,22.361984037077,22.362738108049");
	Text::CStringNN lonStr = CSTR("114.08715410232,114.08588809966,114.09921331405,114.11195917129,114.11129398346,114.08721847534");
	Double lat = 22.361138;
	Double lon = 114.09073483333;
	Data::ArrayList<Double> lats;
	Data::ArrayList<Double> lons;
	if (Text::StringTool::SplitAsDouble(latStr, ',', lats) && Text::StringTool::SplitAsDouble(lonStr, ',', lons))
	{
		printf("InPolygon = %d\r\n", InZone(&lats, &lons, Math::Coord2DDbl(lon, lat))?1:0);
	}
	return 0;
}

Int32 RenameFileTest()
{
	Text::CString path = CSTR("/media/sswroom/Extreme SSD/PBG/3D_Mesh/");
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UTF8Char *sptr;
	UTF8Char *sptrEnd;
	UTF8Char *sptrEnd2;
	UTF8Char *sptr2;
	UTF8Char *sptr2End;
	sptr = path.ConcatTo(sbuff);
	sptrEnd = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	IO::Path::FindFileSession *sess = IO::Path::FindFile(CSTRP(sbuff, sptrEnd));
	IO::Path::FindFileSession *sess2;
	if (sess)
	{
		IO::Path::PathType pt;
		while ((sptrEnd = IO::Path::FindNextFile(sptr, sess, 0, &pt, 0)) != 0)
		{
			if (sptr[0] != '.' && pt == IO::Path::PathType::Directory)
			{
				sptr2 = sptrEnd;
				*sptr2++ = IO::Path::PATH_SEPERATOR;
				sptr2 = Text::StrConcatC(sptr2, UTF8STRC("Scene"));
				if (IO::Path::GetPathType(CSTRP(sbuff, sptr2)) == IO::Path::PathType::Directory)
				{
					sptr2 = sptrEnd;
					*sptr2++ = IO::Path::PATH_SEPERATOR;
					sptr2 = Text::StrConcatC(sptr2, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
					sess2 = IO::Path::FindFile(CSTRP(sbuff, sptr2));
					if (sess2)
					{
						sptr2 = Text::StrConcatC(sbuff2, sbuff, (UOSInt)(sptrEnd - sbuff));
						*sptr2++ = IO::Path::PATH_SEPERATOR;
						sptr2 = Text::StrConcatC(sptr2, sptr, (UOSInt)(sptrEnd - sptr));
						IO::Path::CreateDirectory(CSTRP(sbuff2, sptr2));
						*sptrEnd++ = IO::Path::PATH_SEPERATOR;
						*sptr2++ = IO::Path::PATH_SEPERATOR;
						while ((sptrEnd2 = IO::Path::FindNextFile(sptrEnd, sess2, 0, &pt, 0)) != 0)
						{
							if (sptrEnd[0] != '.' && pt == IO::Path::PathType::Directory)
							{
								sptr2End = Text::StrConcatC(sptr2, sptrEnd, (UOSInt)(sptrEnd2 - sptrEnd));
								IO::FileUtil::MoveDir(CSTRP(sbuff, sptrEnd2), CSTRP(sbuff2, sptr2End), IO::FileUtil::FileExistAction::Fail, 0, 0);
							}
						}
						IO::Path::FindFileClose(sess2);
					}
				}
			}
		}
		IO::Path::FindFileClose(sess);
	}
	return 0;
}

class TestCANHandler : public IO::Device::BYDC9RHandler
{
public:
	TestCANHandler()
	{
	}
	virtual ~TestCANHandler()
	{
	}

/*	virtual void VehicleSpeed(Double speedkmHr)
	{
		printf("Vehicle Speed: %lf\r\n", speedkmHr);
	}*/

	virtual void VehicleDoor(DoorStatus door1, DoorStatus door2)
	{
//		printf("Vehicle Doors: door1 = %s, door2 = %s\r\n", DoorStatusGetName(door1).v, DoorStatusGetName(door2).v);
	}

	virtual void BatteryLevel(Double percent)
	{
//		printf("Battery Level: %lf\r\n", percent);
	}

	virtual void MotorRPM(Int32 rpm)
	{
//		printf("Motor RPM: %d\r\n", rpm);
	}

	virtual void BatteryChargedTime(UInt32 minutes)
	{
//		printf("Battery Charged time (minutes): %d\r\n", minutes);
	}

	virtual void LeftMotorMode(MotorMode mode)
	{
//		printf("Left Motor Mode: %s\r\n", MotorModeGetName(mode).v);
	}

	virtual void RightMotorMode(MotorMode mode)
	{
//		printf("RIght Motor Mode: %s\r\n", MotorModeGetName(mode).v);
	}

	virtual void CarbinDoor(Bool backOpened, Bool leftOpened, Bool rightOpened)
	{
//		printf("Carbin Door: Back = %d, Left = %d, Right = %d\r\n", backOpened?1:0, leftOpened?1:0, rightOpened?1:0);
	}

	virtual void PowerMode(PowerStatus status)
	{
		printf("Power Mode: %s\r\n", PowerStatusGetName(status).v);
	}

	virtual void BatteryCharging(ChargingStatus status)
	{
		printf("Battery Charging: %s\r\n", ChargingStatusGetName(status).v);
	}

	virtual void OKLED(Bool ledOn)
	{
//		printf("OK LED:  %d\r\n", ledOn?1:0);
	}
};

Int32 AXCAN_BYDC9RTest()
{
	Text::CStringNN filePath = CSTR("/home/sswroom/Progs/Temp/20230511 ITT CANBUS/Sampledata.txt");
	IO::FileStream fs(filePath, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	Text::UTF8Reader reader(fs);
	TestCANHandler hdlr;
	IO::Device::AXCAN axcan(&hdlr);
	axcan.ParseReader(&reader);
	return 0;
}

Int32 ProcessExecTest()
{
	Text::StringBuilderUTF8 sb;
	Manage::Process::ExecuteProcess(CSTR("pkgconf --libs libpq"), sb);
	while (sb.EndsWith('\r') || sb.EndsWith('\n'))
	{
		sb.RemoveChars(1);
	}
	printf("Return %s\r\n", sb.ToString());
	return 0;
}

Int32 CFBTimeTest()
{
	UInt64 t = 0x01BAB44B13921E80;
	UTF8Char sbuff[64];
	Data::Timestamp::FromFILETIME(&t, 0).ToString(sbuff);
	printf("Time = %s\r\n", sbuff);
	return 0;
}

Int32 ESRIFeatureServerTest()
{
	Net::OSSocketFactory sockf(true);
	Optional<Net::SSLEngine> ssl = Net::SSLEngineFactory::Create(sockf, false);
	{
		Map::ESRI::ESRIFeatureServer svr(CSTR("https://portal.csdi.gov.hk/server/rest/services/common/hko_rcd_1634958957456_52030/FeatureServer"), sockf, ssl);
		NotNullPtr<Map::ESRI::ESRIFeatureServer::LayerInfo> info;
		if (svr.GetLayerInfo().SetTo(info))
		{
			info.Delete();
		}
	}
	ssl.Delete();
	return 0;
}

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	return ESRIFeatureServerTest();
}
