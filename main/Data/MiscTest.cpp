#include "Stdafx.h"
#include "Core/Core.h"
#include "Crypto/Encrypt/JasyptEncryptor.h"
#include "DB/MSSQLConn.h"
#include "IO/ConsoleWriter.h"
#include "IO/ConsoleLogHandler.h"
#include "IO/DirectoryPackage.h"
#include "IO/FileStream.h"
#include "IO/FileUtil.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "IO/Device/AXCAN.h"
#include "IO/Device/BYDC9RHandler.h"
#include "IO/ProtoHdlr/ProtoJMVL01Handler.h"
#include "IO/StmData/FileData.h"
#include "IO/StmData/MemoryDataRef.h"
#include "Manage/Process.h"
#include "Map/ESRI/ESRIFeatureServer.h"
#include "Map/ESRI/FileGDBDir.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/GeometryTool.h"
#include "Math/Triangle.h"
#include "Math/WKBReader.h"
#include "Math/WKBWriter.h"
#include "Math/WKTReader.h"
#include "Media/DrawEngineFactory.h"
#include "Media/PaperSize.h"
#include "Net/HKOAPI.h"
#include "Net/HTTPClient.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSHManager.h"
#include "Net/SSLEngineFactory.h"
#include "Net/Email/AWSEmailClient.h"
#include "Net/Email/SMTPClient.h"
#include "Net/Google/GoogleFCMv1.h"
#include "Net/Google/GoogleIdentityToolkit.h"
#include "Net/Google/GoogleOAuth2.h"
#include "Net/Google/GoogleServiceAccount.h"
#include "Parser/FullParserList.h"
#include "Parser/ObjParser/FileGDB2Parser.h"
#include "Sync/SimpleThread.h"
#include "Text/CPPText.h"
#include "Text/StringTool.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"
#include "Text/XMLReader.h"
#include <stdio.h>

class ProtoListener : public IO::ProtocolHandler::DataListener
{
public:
	virtual void DataParsed(NN<IO::Stream> stm, AnyType stmObj, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UOSInt cmdSize)
	{
		printf("Received cmdType 0x%x, size=%d\r\n", cmdType, (UInt32)cmdSize);
	}

	virtual void DataSkipped(NN<IO::Stream> stm, AnyType stmObj, UnsafeArray<const UInt8> buff, UOSInt buffSize)
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
						writer.WriteLine(sb.ToCString());
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
	Text::CStringNN path = CSTR("/media/sswroom/Extreme SSD/PBG/3D_Mesh/");
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptrEnd;
	UnsafeArray<UTF8Char> sptrEnd2;
	UnsafeArray<UTF8Char> sptr2;
	UnsafeArray<UTF8Char> sptr2End;
	sptr = path.ConcatTo(sbuff);
	sptrEnd = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	NN<IO::Path::FindFileSession> sess;
	NN<IO::Path::FindFileSession> sess2;
	if (IO::Path::FindFile(CSTRP(sbuff, sptrEnd)).SetTo(sess))
	{
		IO::Path::PathType pt;
		while (IO::Path::FindNextFile(sptr, sess, 0, pt, 0).SetTo(sptrEnd))
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
					if (IO::Path::FindFile(CSTRP(sbuff, sptr2)).SetTo(sess2))
					{
						sptr2 = Text::StrConcatC(sbuff2, sbuff, (UOSInt)(sptrEnd - sbuff));
						*sptr2++ = IO::Path::PATH_SEPERATOR;
						sptr2 = Text::StrConcatC(sptr2, sptr, (UOSInt)(sptrEnd - sptr));
						IO::Path::CreateDirectory(CSTRP(sbuff2, sptr2));
						*sptrEnd++ = IO::Path::PATH_SEPERATOR;
						*sptr2++ = IO::Path::PATH_SEPERATOR;
						while (IO::Path::FindNextFile(sptrEnd, sess2, 0, pt, 0).SetTo(sptrEnd2))
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
		printf("Power Mode: %s\r\n", PowerStatusGetName(status).v.Ptr());
	}

	virtual void BatteryCharging(ChargingStatus status)
	{
		printf("Battery Charging: %s\r\n", ChargingStatusGetName(status).v.Ptr());
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
	IO::Device::AXCAN axcan(hdlr);
	axcan.ParseReader(reader);
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
	printf("Return %s\r\n", sb.ToPtr());
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
	Net::TCPClientFactory clif(sockf);
	Optional<Net::SSLEngine> ssl = Net::SSLEngineFactory::Create(clif, false);
	{
		Map::ESRI::ESRIFeatureServer svr(CSTR("https://portal.csdi.gov.hk/server/rest/services/common/hko_rcd_1634958957456_52030/FeatureServer"), clif, ssl);
		NN<Map::ESRI::ESRIFeatureServer::LayerInfo> info;
		if (svr.GetLayerInfo().SetTo(info))
		{
			info.Delete();
		}
	}
	ssl.Delete();
	return 0;
}

Int32 HKOTest()
{
	Net::OSSocketFactory sockf(true);
	Net::TCPClientFactory clif(sockf);
	Optional<Net::SSLEngine> ssl = Net::SSLEngineFactory::Create(clif, false);
	Text::StringBuilderUTF8 sb;
	Net::HKOAPI::Language lang = Net::HKOAPI::Language::En;

	if (false)
	{
		NN<Net::HKOAPI::LocalWeatherForecast> resp;
		if (Net::HKOAPI::GetLocalWeatherForecast(clif, ssl, lang).SetTo(resp))
		{
			sb.ClearStr();
			resp->ToString(sb);
			printf("Local Weather Forecast:\r\n%s\r\n", sb.v.Ptr());
			resp.Delete();
		}
	}

	if (false)
	{
		NN<Net::HKOAPI::NineDayWeatherForecast> resp;
		if (Net::HKOAPI::Get9DayWeatherForecast(clif, ssl, lang).SetTo(resp))
		{
			sb.ClearStr();
			resp->ToString(sb);
			printf("9-Day Weather Forecast:\r\n%s\r\n", sb.v.Ptr());
			resp.Delete();
		}
	}

	if (false)
	{
		NN<Net::HKOAPI::CurrentWeatherReport> resp;
		if (Net::HKOAPI::GetCurrentWeatherReport(clif, ssl, lang).SetTo(resp))
		{
			sb.ClearStr();
			resp->ToString(sb);
			printf("Current Weather Report:\r\n%s\r\n", sb.v.Ptr());
			resp.Delete();
		}
	}

	if (false)
	{
		NN<Net::HKOAPI::WeatherWarningSummary> resp;
		if (Net::HKOAPI::GetWeatherWarningSummary(clif, ssl, lang).SetTo(resp))
		{
			sb.ClearStr();
			resp->ToString(sb);
			printf("Weather Warning Summary:\r\n%s\r\n", sb.v.Ptr());
			resp.Delete();
		}
	}

	/////////////////////////////
	if (false)
	{
		NN<Net::HKOAPI::WeatherWarningInfo> resp;
		if (Net::HKOAPI::GetWeatherWarningInfo(clif, ssl, lang).SetTo(resp))
		{
			sb.ClearStr();
			resp->ToString(sb);
			printf("Weather Warning Information:\r\n%s\r\n", sb.v.Ptr());
			resp.Delete();
		}
	}

	if (false)
	{
		NN<Net::HKOAPI::SpecialWeatherTips> resp;
		if (Net::HKOAPI::GetSpecialWeatherTips(clif, ssl, lang).SetTo(resp))
		{
			sb.ClearStr();
			resp->ToString(sb);
			printf("Special Weather Tips:\r\n%s\r\n", sb.v.Ptr());
			resp.Delete();
		}
	}

	if (false)
	{
		NN<Net::HKOAPI::QuickEarthquakeMessages> resp;
		if (Net::HKOAPI::GetQuickEarthquakeMessages(clif, ssl, lang).SetTo(resp))
		{
			sb.ClearStr();
			resp->ToString(sb);
			printf("Quick Earthquake Messages:\r\n%s\r\n", sb.v.Ptr());
			resp.Delete();
		}
	}

	//////////////////////////////
	if (false)
	{
		NN<Net::HKOAPI::LocallyFeltEarthTremorReport> resp;
		if (Net::HKOAPI::GetLocallyFeltEarthTremorReport(clif, ssl, lang).SetTo(resp))
		{
			sb.ClearStr();
			resp->ToString(sb);
			printf("Locally Felt Earth Tremor Report:\r\n%s\r\n", sb.v.Ptr());
			resp.Delete();
		}
	}

	if (false)
	{
		NN<Net::HKOAPI::LunarDate> resp;
		if (Net::HKOAPI::GetLunarDate(clif, ssl, Data::Date::Today()).SetTo(resp))
		{
			sb.ClearStr();
			resp->ToString(sb);
			printf("Lunar Date:\r\n%s\r\n", sb.v.Ptr());
			resp.Delete();
		}
	}

	{
		NN<Net::HKOAPI::HourlyRainfall> resp;
		if (Net::HKOAPI::GetHourlyRainfall(clif, ssl, lang).SetTo(resp))
		{
			sb.ClearStr();
			resp->ToString(sb);
			printf("Rainfall in The Past Hour from Automatic Weather Station API:\r\n%s\r\n", sb.v.Ptr());
			resp.Delete();
		}
	}

	ssl.Delete();
	return 0;
}

Int32 WKBTest()
{
	NN<Math::Geometry::Vector2D> vec;
	Math::WKTReader wkt(2326);
	if (wkt.ParseWKT(CSTR("POLYGON((0 0,0 1,1 1,1 0))").v).SetTo(vec))
	{
		Math::WKBWriter wkbw(false);
		printf("Parsed WKT\r\n");
		IO::MemoryStream mstm;
		if (wkbw.Write(mstm, vec))
		{
			printf("Write WKB\r\n");
			Math::WKBReader wkbr(2326);
			NN<Math::Geometry::Vector2D> vec2;
			if (wkbr.ParseWKB(mstm.GetBuff(), (UOSInt)mstm.GetLength(), 0).SetTo(vec2))
			{
				printf("Parsed WKB\r\n");
				if (vec->Equals(vec2, true, false, false))
					printf("Same as original\r\n");
				vec2.Delete();
			}
		}
		vec.Delete();
	}
	return 0;
}

Int32 SSHTest(NN<Core::ProgControl> progCtrl)
{
	Net::OSSocketFactory sockf(false);
	Net::SSHManager ssh(sockf);
	NN<Net::SSHClient> cli;
	if (ssh.CreateClient(CSTR("192.168.0.15"), 22, CSTR("sswroom"), CSTR("testing")).SetTo(cli))
	{
		if (!cli->CreateForward(11111, CSTR("192.168.0.25"), 12345).IsNull())
		{
			progCtrl->WaitForExit(progCtrl);
		}
		cli.Delete();
	}
	return 0;
}

Int32 PaperSize()
{
	Media::PaperSize paperSize(Media::PaperSize::PT_A3);
	Math::Size2DDbl size = paperSize.GetSizeMM();
	printf("A3 pixel size = %lf x %lf\r\n", Math::Unit::Distance::Convert(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_PIXEL, size.GetWidth()), Math::Unit::Distance::Convert(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_PIXEL, size.GetHeight()));
	return 0;
}

Int32 CurveToLine()
{
	Math::Coord2DDbl pt1 = Math::Coord2DDbl(835736.32193, 818912.22624);
	Math::Coord2DDbl pt2 = Math::Coord2DDbl(835744.47885, 818897.20205);
	Math::Coord2DDbl pt3 = Math::Coord2DDbl(835789.34862, 818831.0624);

//	Math::Coord2DDbl pt1 = Math::Coord2DDbl(821507.5140000004, 834004.4700000024);
//	Math::Coord2DDbl pt2 = Math::Coord2DDbl(821507.5140000004, 833989.0700000024);
//	Math::Coord2DDbl pt3 = Math::Coord2DDbl(821507.5140000004, 834004.4700000024);

	Data::ArrayListA<Math::Coord2DDbl> pts;
	Math::GeometryTool::ArcToLine(pt1, pt2, pt3, 2.5, pts);
	/*
	835736.3219300006 818912.2262400016,
	835737.4551645218 818910.0627034007,
	835738.5991263686 818907.9048195927,
	835739.7537873982 818905.7526418566,
	835740.9191191493 818903.6062231957,
	835742.0950928627 818901.4656165092,
	835743.2816795706 818899.330874592,
	835744.4788500005 818897.2020500023,
	835750.2213875352 818887.3193698822,
	835756.1919737755 818877.5727702527,
	835762.387385696 818867.9675125479,
	835768.8042788802 818858.50878188,
	835775.4391894041 818849.2016842215,
	835782.288535565 818840.0512437521,
	835789.3486200012 818831.062400002*/
	UOSInt i = 0;
	UOSInt j = pts.GetCount();
	while (i < j)
	{
		Math::Coord2DDbl pt = pts.GetItem(i);
		printf("%lf %lf\r\n", pt.x, pt.y);
		i++;
	}
	return 0;
}

Int32 SQLConvFunc()
{
	Text::CStringNN srcFile = CSTR("application_location.sql");
	Text::CStringNN destFile = CSTR("application_location_upd.sql");
	IO::FileStream srcFS(srcFile, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (!srcFS.IsError())
	{
		Text::PString sarr[7];
		Text::StringBuilderUTF8 sbSrc;
		Text::StringBuilderUTF8 sbDest;
		Text::UTF8Reader reader(srcFS);
		IO::FileStream destFS(destFile, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!destFS.IsError())
		{
			Text::UTF8Writer writer(destFS);
			while (reader.ReadLine(sbSrc, 65536))
			{
				UOSInt i = sbSrc.IndexOf(CSTR(") VALUES ("));
				if (i != INVALID_INDEX && Text::StrSplitTrimP(sarr, 7, sbSrc.Substring(i + 10), ',') == 6)
				{
					if (!sarr[1].Equals(CSTR("NULL")))
					{
						sbDest.ClearStr();
						sbDest.Append(CSTR("update \"application_location\" set \"burial_location_point\" = "));
						sbDest.Append(sarr[1]);
						sbDest.Append(CSTR(" where \"burial_no_gen\" = "));
						sbDest.Append(sarr[0]);
						sbDest.AppendUTF8Char(';');
						writer.WriteLine(sbDest.ToCString());
					}
				}
				sbSrc.ClearStr();
			}
		}
	}
	return 0;
}

Int32 SMTPProxyTest()
{
	Text::CStringNN smtpHost = CSTR("");
	UInt16 smtpPort = 465;
	Text::CStringNN smtpUser = CSTR("");
	Text::CStringNN smtpPwd = CSTR("");
	Text::CStringNN smtpTo = CSTR("");

	Text::CStringNN proxyHost = CSTR("127.0.0.1");
	UInt16 proxyPort = 8080;
	Text::CString proxyUser = 0;
	Text::CString proxyPwd = 0;	
	Net::Email::SMTPConn::ConnType connType = Net::Email::SMTPConn::ConnType::STARTTLS;
	Net::OSSocketFactory sockf(true);
	Net::TCPClientFactory clif(sockf);
	Optional<Net::SSLEngine> ssl = Net::SSLEngineFactory::Create(clif, true);
	IO::ConsoleWriter console;
	IO::LogTool log;
	IO::ConsoleLogHandler logHdlr(console);
	log.AddLogHandler(logHdlr, IO::LogHandler::LogLevel::Raw);
	clif.SetProxy(proxyHost, proxyPort, proxyUser, proxyPwd);
	Net::Email::SMTPClient cli(clif, ssl, smtpHost, smtpPort, connType, log, 15000);
	cli.SetPlainAuth(smtpUser, smtpPwd);
	Net::Email::EmailMessage msg;
	msg.SetFrom(0, smtpUser);
	msg.AddTo(0, smtpTo);
	msg.SetSubject(CSTR("SMTP Proxy Test"));
	msg.SetSentDate(Data::Timestamp::Now());
	msg.SetContent(CSTR("Message Content to display"), CSTR("text/plain"));
	cli.Send(msg);
	ssl.Delete();
	return 0;
}

Int32 HTTPSProxyCliTest()
{
	Net::OSSocketFactory sockf(true);
	Net::TCPClientFactory clif(sockf);
	clif.SetProxy(CSTR("localhost"), 8080, 0, 0);
	Optional<Net::SSLEngine> ssl = Net::SSLEngineFactory::Create(clif, true);
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(clif, ssl, CSTR("https://a.tile.openstreetmap.org/13/6693/3571.png"), Net::WebUtil::RequestMethod::HTTP_GET, false);
	IO::MemoryStream mstm;
	cli->ReadAllContent(mstm, 8192, 1048576);
	printf("Status = %d, Read size = %lld\r\n", (Int32)cli->GetRespStatus(), mstm.GetLength());
	cli.Delete();
	ssl.Delete();
	return 0;
}

Int32 AWSEmailTest()
{
	Text::CStringNN smtpUser = CSTR("");
	Text::CStringNN smtpTo = CSTR("sswroom@yahoo.com");

	Net::Email::AWSEmailClient cli(CSTR("us-east-2"));
	Net::Email::EmailMessage msg;
	msg.SetFrom(0, smtpUser);
	msg.AddTo(0, smtpTo);
	msg.SetSubject(CSTR("AWS Email Test"));
	msg.SetSentDate(Data::Timestamp::Now());
	msg.SetContent(CSTR("Message Content to display"), CSTR("text/plain"));
	cli.Send(msg);
	return 0;
}

Int32 JasyptTest()
{
	Text::CStringNN text = CSTR("sbdW6Uuf+wHmgWOKlZNp1P7nww7vkBsXHcpnbLcbInBoVMFxzTqeY9cvc0fktysc");
	Text::CStringNN password = CSTR("");
	Crypto::Encrypt::JasyptEncryptor encryptor(Crypto::Encrypt::JasyptEncryptor::KA_PBEWITHHMACSHA512, Crypto::Encrypt::JasyptEncryptor::CA_AES256, password.ToByteArray());
	UInt8 buff[256];
	UOSInt buffLeng = encryptor.DecryptB64(text, buff);
	Text::StringBuilderUTF8 sb;
	sb.AppendHexBuff(buff, buffLeng, ' ', Text::LineBreakType::CRLF);
	printf("%s\r\n", sb.ToPtr());
	return 0;
}

Int32 FirebaseTest()
{
	Text::CStringNN key = CSTR("");
	Text::CStringNN userEmail = CSTR("");
	Text::CStringNN password = CSTR("");
	Net::OSSocketFactory sockf(true);
	Net::TCPClientFactory clif(sockf);
	Optional<Net::SSLEngine> ssl = Net::SSLEngineFactory::Create(clif, true);
	{
		Net::Google::GoogleIdentityToolkit identityToolkit(clif, ssl, key);
		NN<Net::Google::VerifyPasswordResponse> resp;
		if (identityToolkit.SignInWithPassword(userEmail, password).SetTo(resp))
		{
			printf("localId: %s\r\n", resp->GetLocalId()->v.Ptr());
			printf("email: %s\r\n", resp->GetEmail()->v.Ptr());
			printf("displayName: %s\r\n", resp->GetDisplayName()->v.Ptr());
			printf("idToken: %s\r\n", resp->GetIdToken()->v.Ptr());
			printf("registered: %s\r\n", resp->IsRegistered()?"true":"false");
			printf("refreshToken: %s\r\n", resp->GetRefreshToken()->v.Ptr());
			printf("expiresIn: %d\r\n", resp->GetExpiresIn());
			resp.Delete();
		}
	}
	ssl.Delete();
	return 0;
}

Int32 FCMTest()
{
	Text::CStringNN serviceAccountPath = CSTR("");
	Text::CStringNN devToken = CSTR("");
	Text::CStringNN message = CSTR("Message Test");
	NN<Net::Google::GoogleServiceAccount> serviceAccount;
	Net::OSSocketFactory sockf(true);
	Net::TCPClientFactory clif(sockf);
	Optional<Net::SSLEngine> ssl = Net::SSLEngineFactory::Create(clif, true);
	if (Net::Google::GoogleServiceAccount::FromFile(serviceAccountPath).SetTo(serviceAccount))
	{
		printf("Service Account loaded\r\n");
		NN<Crypto::Token::JWToken> token;
		if (serviceAccount->ToJWT(ssl, CSTR("https://www.googleapis.com/auth/firebase.messaging")).SetTo(token))
		{
			printf("Token generated\r\n");
			token.Delete();
		}
		NN<Net::Google::AccessTokenResponse> accessToken;
		Net::Google::GoogleOAuth2 oauth2(clif, ssl);
		if (oauth2.GetServiceToken(serviceAccount, CSTR("https://www.googleapis.com/auth/firebase.messaging")).SetTo(accessToken))
		{
			printf("Access Token Received\r\n");
			Net::Google::GoogleFCMv1 fcm(clif, ssl, serviceAccount->GetProjectId()->ToCString());
			if (fcm.SendMessage(accessToken->GetAccessToken()->ToCString(), devToken, message, 0))
			{
				printf("Message sent\r\n");
			}
			accessToken.Delete();
		}
		serviceAccount.Delete();
	}
	ssl.Delete();
	return 0;
}

Int32 WKTParseTest()
{
	Text::CStringNN wkt = CSTR("MULTISURFACE(CURVEPOLYGON(COMPOUNDCURVE((842567.3156800009 832330.1409100014 1.455191522836685e-11,842567.2657700003 832330.1128300018 1.455191522836685e-11),CIRCULARSTRING(842567.2657700003 832330.1128300018 1.455191522836685e-11,842566.7210711593 832330.8805971046 1.455191522836685e-11,842566.0020000012 832331.4881300014 1.455191522836685e-11),CIRCULARSTRING(842566.0020000012 832331.4881300014 1.455191522836685e-11,842565.8385874868 832332.5051478536 1.455191522836685e-11,842565.0646200011 832333.1848600021 1.455191522836685e-11),(842565.0646200011 832333.1848600021 1.455191522836685e-11,842565.0683066615 832333.2980516681 1.455191522836685e-11,842565.0644830568 832333.4112387824 1.455191522836685e-11,842565.0531659766 832333.523923578 1.455191522836685e-11,842565.0344051906 832333.6356104948 1.455191522836685e-11,842565.0082832044 832333.7458083615 1.455191522836685e-11,842564.9749148959 832333.8540325551 1.455191522836685e-11,842564.9344470106 832333.9598071322 1.455191522836685e-11,842564.8870575163 832334.0626669227 1.455191522836685e-11,842564.8329548202 832334.1621595743 1.455191522836685e-11,842564.7723768527 832334.2578475431 1.455191522836685e-11,842564.7055900003 832334.3493100013 1.455191522836685e-11),CIRCULARSTRING(842564.7055900003 832334.3493100013 1.455191522836685e-11,842564.5449891737 832334.790209516 1.455191522836685e-11,842564.7166300005 832335.2269300016 1.455191522836685e-11),CIRCULARSTRING(842564.7166300005 832335.2269300016 1.455191522836685e-11,842565.1870831299 832335.757059924 1.455191522836685e-11,842565.6462600007 832336.2969900016 1.455191522836685e-11),CIRCULARSTRING(842565.6462600007 832336.2969900016 1.455191522836685e-11,842566.5406707474 832336.3243946538 1.455191522836685e-11,842567.2401300006 832336.8825000021 1.455191522836685e-11),CIRCULARSTRING(842567.2401300006 832336.8825000021 1.455191522836685e-11,842568.1801812444 832337.0550075418 1.455191522836685e-11,842568.9234300003 832337.6558700017 1.455191522836685e-11),CIRCULARSTRING(842568.9234300003 832337.6558700017 1.455191522836685e-11,842570.5656364598 832338.1227308686 1.455191522836685e-11,842571.9658700003 832339.0995400026 1.455191522836685e-11),CIRCULARSTRING(842571.9658700003 832339.0995400026 1.455191522836685e-11,842572.0571008102 832339.1911950521 1.455191522836685e-11,842572.1460100011 832339.2851100022 1.455191522836685e-11),CIRCULARSTRING(842572.1460100011 832339.2851100022 1.455191522836685e-11,842572.3198267969 832339.2539867671 1.455191522836685e-11,842572.4963800003 832339.2571800015 1.455191522836685e-11),CIRCULARSTRING(842572.4963800003 832339.2571800015 1.455191522836685e-11,842572.8366235968 832339.3678564239 1.455191522836685e-11,842573.107470001 832339.6016500014 1.455191522836685e-11),CIRCULARSTRING(842573.107470001 832339.6016500014 1.455191522836685e-11,842573.1381435977 832339.6436185387 1.455191522836685e-11,842573.1663600009 832339.687280001 1.455191522836685e-11),CIRCULARSTRING(842573.1663600009 832339.687280001 1.455191522836685e-11,842573.1786984638 832339.7082564686 1.455191522836685e-11,842573.1904700006 832339.7295600027 1.455191522836685e-11),CIRCULARSTRING(842573.1904700006 832339.7295600027 1.455191522836685e-11,842574.0090999137 832339.8109131628 1.455191522836685e-11,842574.8078300011 832340.0079100019 1.455191522836685e-11),(842574.8078300011 832340.0079100019 1.455191522836685e-11,842574.8372836552 832340.0163595647 1.455191522836685e-11,842574.8672411175 832340.0227987721 1.455191522836685e-11,842574.8975653387 832340.0271981631 1.455191522836685e-11,842574.9281175917 832340.0295376117 1.455191522836685e-11,842574.9587581059 832340.0298064153 1.455191522836685e-11,842574.9893467065 832340.0280033438 1.455191522836685e-11,842575.019743457 832340.0241366465 1.455191522836685e-11,842575.0498092978 832340.0182240122 1.455191522836685e-11,842575.0794066834 832340.0102924907 1.455191522836685e-11,842575.1084002118 832340.0003783666 1.455191522836685e-11,842575.1366572427 832339.9885269957 1.455191522836685e-11,842575.1640485061 832339.974792595 1.455191522836685e-11,842575.1904486921 832339.9592379972 1.455191522836685e-11,842575.215737025 832339.9419343616 1.455191522836685e-11,842575.2397978155 832339.922960849 1.455191522836685e-11,842575.2625209898 832339.9024042593 1.455191522836685e-11,842575.2838025942 832339.8803586353 1.455191522836685e-11,842575.3035452693 832339.8569248313 1.455191522836685e-11,842575.321658696 832339.8322100523 1.455191522836685e-11,842575.3380600094 832339.8063273637 1.455191522836685e-11,842575.352674176 832339.7793951741 1.455191522836685e-11,842575.3654343391 832339.7515366929 1.455191522836685e-11,842575.3762821234 832339.7228793673 1.455191522836685e-11,842575.3851679024 832339.6935542987 1.455191522836685e-11,842575.3920510256 832339.6636956441 1.455191522836685e-11,842575.396900001 832339.6334400008 1.455191522836685e-11),(842575.396900001 832339.6334400008 1.455191522836685e-11,842575.4012900003 832339.6016500014 1.455191522836685e-11),CIRCULARSTRING(842575.4012900003 832339.6016500014 1.455191522836685e-11,842575.4400448111 832339.4262823569 1.455191522836685e-11,842575.5022500007 832339.2578000016 1.455191522836685e-11),CIRCULARSTRING(842575.5022500007 832339.2578000016 1.455191522836685e-11,842575.5372128116 832339.18579006 1.455191522836685e-11,842575.5764800011 832339.1160300019 1.455191522836685e-11),CIRCULARSTRING(842575.5764800011 832339.1160300019 1.455191522836685e-11,842575.6943494062 832338.9509181767 1.455191522836685e-11,842575.8361300006 832338.8058200012 1.455191522836685e-11),CIRCULARSTRING(842575.8361300006 832338.8058200012 1.455191522836685e-11,842575.9682470703 832337.8550342414 1.455191522836685e-11,842576.4679100011 832337.0354100018 1.455191522836685e-11),CIRCULARSTRING(842576.4679100011 832337.0354100018 1.455191522836685e-11,842576.6065136728 832336.5511632567 1.455191522836685e-11,842576.9408100003 832336.1744000017 1.455191522836685e-11),CIRCULARSTRING(842576.9408100003 832336.1744000017 1.455191522836685e-11,842577.026210799 832335.6691271131 1.455191522836685e-11,842577.1628800006 832335.1752500012 1.455191522836685e-11),CIRCULARSTRING(842577.1628800006 832335.1752500012 1.455191522836685e-11,842577.2322921163 832335.1903656535 1.455191522836685e-11,842577.3012200006 832335.2075700015 1.455191522836685e-11),CIRCULARSTRING(842577.3012200006 832335.2075700015 1.455191522836685e-11,842577.6556895529 832335.3328345839 1.455191522836685e-11,842577.9856900004 832335.5129500012 1.455191522836685e-11),CIRCULARSTRING(842577.9856900004 832335.5129500012 1.455191522836685e-11,842578.7573265256 832335.3491375212 1.455191522836685e-11,842579.5433100005 832335.4161100014 1.455191522836685e-11),CIRCULARSTRING(842579.5433100005 832335.4161100014 1.455191522836685e-11,842580.1195890592 832335.2811152781 1.455191522836685e-11,842580.6265800007 832335.5865400023 1.455191522836685e-11),CIRCULARSTRING(842580.6265800007 832335.5865400023 1.455191522836685e-11,842581.279838961 832335.4081048502 1.455191522836685e-11,842581.9428300009 832335.5460600015 1.455191522836685e-11),CIRCULARSTRING(842581.9428300009 832335.5460600015 1.455191522836685e-11,842582.2980332985 832335.5790439492 1.455191522836685e-11,842582.5915800007 832335.3763400017 1.455191522836685e-11),CIRCULARSTRING(842582.5915800007 832335.3763400017 1.455191522836685e-11,842582.6085359524 832335.3509484991 1.455191522836685e-11,842582.6239800005 832335.3246100022 1.455191522836685e-11),CIRCULARSTRING(842582.6239800005 832335.3246100022 1.455191522836685e-11,842582.6378558787 832335.2974170063 1.455191522836685e-11,842582.6501200003 832335.2694600019 1.455191522836685e-11),CIRCULARSTRING(842582.6501200003 832335.2694600019 1.455191522836685e-11,842582.6607301829 832335.2408303017 1.455191522836685e-11,842582.6696500004 832335.2116300016 1.455191522836685e-11),CIRCULARSTRING(842582.6696500004 832335.2116300016 1.455191522836685e-11,842582.6768475531 832335.1819589115 1.455191522836685e-11,842582.6823000004 832335.1519200019 1.455191522836685e-11),(842582.6823000004 832335.1519200019 1.455191522836685e-11,842582.6834500003 832335.1439200024 1.455191522836685e-11,842582.6807700003 832324.6439200025 1.455191522836685e-11,842582.6807600007 832324.591260001 1.455191522836685e-11),(842582.6807600007 832324.591260001 1.455191522836685e-11,842582.6244786279 832324.5926071223 1.455191522836685e-11,842582.5682292595 832324.5902791795 1.455191522836685e-11,842582.5122516161 832324.5842860555 1.455191522836685e-11,842582.4567842634 832324.5746532919 1.455191522836685e-11,842582.4020635917 832324.5614219415 1.455191522836685e-11,842582.3483228094 832324.5446483937 1.455191522836685e-11,842582.2957909488 832324.5244041341 1.455191522836685e-11,842582.2446918897 832324.5007754394 1.455191522836685e-11,842582.1952434063 832324.47386301 1.455191522836685e-11,842582.1476562373 832324.4437815415 1.455191522836685e-11,842582.10213319 832324.410659235 1.455191522836685e-11,842582.0588682742 832324.3746372509 1.455191522836685e-11,842582.0180458759 832324.3358691075 1.455191522836685e-11,842581.979840001 832324.2945200017 1.455191522836685e-11),(842581.979840001 832324.2945200017 1.455191522836685e-11,842581.7676824037 832324.3648807667 1.455191522836685e-11,842581.551201292 832324.4205351335 1.455191522836685e-11,842581.3314147835 832324.4612213468 1.455191522836685e-11,842581.1093565445 832324.4867480576 1.455191522836685e-11,842580.8860709257 832324.4969952128 1.455191522836685e-11,842580.6626080498 832324.4919146192 1.455191522836685e-11,842580.4400188734 832324.471530171 1.455191522836685e-11,842580.219350244 832324.4359377376 1.455191522836685e-11,842580.0016399769 832324.3853047117 1.455191522836685e-11,842579.787911974 832324.3198692229 1.455191522836685e-11,842579.5791714078 832324.2399390173 1.455191522836685e-11,842579.3764000003 832324.1458900011 1.455191522836685e-11),CIRCULARSTRING(842579.3764000003 832324.1458900011 1.455191522836685e-11,842578.5542027094 832324.3355469665 1.455191522836685e-11,842577.7112200007 832324.2987700011 1.455191522836685e-11),(842577.7112200007 832324.2987700011 1.455191522836685e-11,842577.5409249271 832324.3747220298 1.455191522836685e-11,842577.3664972554 832324.4406322045 1.455191522836685e-11,842577.1885285729 832324.4962770253 1.455191522836685e-11,842577.0076224604 832324.5414677728 1.455191522836685e-11,842576.8243924605 832324.5760511822 1.455191522836685e-11,842576.639460001 832324.5999100021 1.455191522836685e-11),(842576.639460001 832324.5999100021 1.455191522836685e-11,842576.6320700012 832329.7327900024 1.455191522836685e-11,842574.8222700012 832333.6165300011 1.455191522836685e-11,842567.3156800009 832330.1409100014 1.455191522836685e-11))))");
	Math::WKTReader reader(2326);
	NN<Math::Geometry::Vector2D> vec;
	if (reader.ParseWKT(wkt.v).SetTo(vec))
	{
		printf("WKT parse successfully\r\n");
		vec.Delete();
	}
	else
	{
		printf("Error in parsing WKT\r\n");
	}
	return 0;
}

Int32 FGDBTest()
{
	Text::CStringNN fgdbPath = CSTR("/home/sswroom/ProgsHome/PROGS/StoneRoad/0_req/20241118 EMSD POC/3DPN_P2.gdb");
	/*
	-Building
	BuiltStructurePolygon
	Site
	-SubSite
	-HydroPolygon
	LandCoverVector2
	-BMSslope
	-VerticalCutPolygon
	TransportPolygon
	UtilityPolygon
	*/
	IO::DirectoryPackage dpkg(fgdbPath);
	NN<Map::ESRI::FileGDBDir> fgdb;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	if (Map::ESRI::FileGDBDir::OpenDir(dpkg).SetTo(fgdb))
	{
		NN<Map::ESRI::FileGDBTable> table;
		NN<DB::DBReader> r;
		NN<Text::String> s;
		if (fgdb->GetTable(CSTR("PedestrianRoute")).SetTo(table))
		{
			printf("File Name: %s\r\n", table->GetFileName()->v.Ptr());
			if (table->OpenReader(0, 0, 0, 0, 0).SetTo(r))
			{
				while (r->ReadNext())
				{
					if (r->GetInt32(0) == 1)
					{
						printf("Row Ofst: 0x%llx\r\n", r->GetRowFileOfst());
						i = 0;
						j = r->ColCount();
						while (i < j)
						{
							sbuff[0] = 0;
							sptr = r->GetName(i, sbuff).Or(sbuff);
							if (r->GetNewStr(i).SetTo(s))
							{
								printf("%s: %s\r\n", sbuff, s->v.Ptr());
								s->Release();
							}
							else
							{
								printf("%s: <null>\r\n", sbuff);
							}
							i++;
						}
						break;
					}
				}
				fgdb->CloseReader(r);
			}
		}
		fgdb.Delete();
	}
	return 0;
}

Int32 BezierCurveTest()
{
	Math::Coord2DDbl p0 = Math::Coord2DDbl(845172.9181900005, 821621.7819300014);
	Math::Coord2DDbl p1 = Math::Coord2DDbl(845171.43902494, 821622.01600076);
	Math::Coord2DDbl p2 = Math::Coord2DDbl(845170.00585973, 821622.37980569);
	Math::Coord2DDbl p3 = Math::Coord2DDbl(845168.6187000004, 821622.8733400012);
	Data::ArrayListA<Math::Coord2DDbl> linePts;
	Math::GeometryTool::BezierCurveToLine(p0, p1, p2, p3, 10, linePts);
	Math::Coord2DDbl pt;
	UOSInt i = 0;
	UOSInt j = linePts.GetCount();
	while (i < j)
	{
		pt = linePts.GetItem(i);
		printf("%lf, %lf\r\n", pt.x, pt.y);
		i++;
	}
	return 0;
}

Int32 CSysTest()
{
	NN<Math::CoordinateSystem> csys;
	if (Math::CoordinateSystemManager::SRCreateCSys(3407).SetTo(csys))
	{
		NN<Math::CoordinateSystem> wgs84 = Math::CoordinateSystemManager::CreateWGS84Csys();
		Math::Coord2DDbl newPos = Math::CoordinateSystem::Convert(csys, wgs84, Math::Coord2DDbl(116194.35, 72762.37));
		printf("%lf, %lf\r\n", newPos.x, newPos.y);
		wgs84.Delete();
		csys.Delete();
	}
	return 0;
}

Int32 ClamAVTest()
{
	Text::CStringNN fileName = CSTR("");
	UnsafeArray<UInt8> fileBuff;
	UInt64 fileLength;
	UTF8Char sbuff[2048];
	UOSInt readSize;
	IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	fileLength = fs.GetLength();
	if (fileLength == 0 || fileLength > 10485760 || fs.IsError())
	{
		printf("File is not valid\r\n");
	}
	else
	{
		fileBuff = MemAllocArr(UInt8, (UOSInt)fileLength + 5);
		if (fs.Read(Data::ByteArray(fileBuff + 4, (UOSInt)fileLength)) == fileLength)
		{
			Net::OSSocketFactory sockf(false);
			Net::TCPClient cli(sockf, CSTR("localhost"), 3310, 8000);
			if (cli.IsConnectError())
			{
				printf("Connect error\r\n");
			}
			else
			{
				WriteMUInt32(&fileBuff[0], (UInt32)fileLength);
				WriteMUInt32(&fileBuff[(UOSInt)fileLength + 4], 0);
				cli.SetTimeout(50000);
				cli.Write(CSTR("zINSTREAM\0").ToByteArray());
				cli.WriteCont(fileBuff, (UOSInt)fileLength + 8);
				readSize = cli.Read(Data::ByteArray(sbuff, 2047));
				sbuff[readSize] = 0;
				printf("Reply: %s\r\n", sbuff);
			}
		}
		else
		{
			printf("Error in reading file\r\n");
		}
		MemFreeArr(fileBuff);
	}
	return 0;
}

Int32 ECDSATest()
{
	Net::OSSocketFactory sockf(true);
	Net::TCPClientFactory clif(sockf);
	Optional<Crypto::Cert::X509Key> key;
	NN<Net::SSLEngine> ssl;
	if (Net::SSLEngineFactory::Create(clif, true).SetTo(ssl))
	{
		key = ssl->GenerateECDSAKey(Crypto::Cert::X509File::ECName::secp384r1);
		if (key.NotNull())
		{
			printf("Key generated\r\n");
		}
		else
		{
			printf("Key generate failed\r\n");
		}
		ssl.Delete();
	}
	return 0;
}

struct MTDrawStatus
{
	NN<Media::DrawEngine> drawEng;
	NN<Media::ImageList> img1;
	NN<Media::ImageList> img2;
};

void __stdcall MTDrawThread(NN<Sync::Thread> thread)
{
	NN<MTDrawStatus> status = thread->GetUserObj().GetNN<MTDrawStatus>();
	NN<Media::DrawImage> dimg;
	NN<Media::DrawImage> dimg2;
	NN<Media::RasterImage> sImg;
	UOSInt cnt = 10000;
	while (cnt-- > 0)
	{
		if (status->drawEng->CreateImage32(Math::Size2D<UOSInt>(60, 60), Media::AT_ALPHA).SetTo(dimg))
		{
			if (status->img1->GetImage(0, 0).SetTo(sImg))
			{
				sImg->info.atype = Media::AT_IGNORE_ALPHA;
				if (status->drawEng->ConvImage(sImg, 0).SetTo(dimg2))
				{
					dimg->DrawImagePt(dimg2, Math::Coord2DDbl(UOSInt2Double((60 - dimg2->GetWidth()) >> 1), UOSInt2Double((60 - dimg2->GetHeight()) >> 1)));
					status->drawEng->DeleteImage(dimg2);
				}
			}
			if (status->img2->GetImage(0, 0).SetTo(sImg) && status->drawEng->ConvImage(sImg, 0).SetTo(dimg2))
			{
				dimg->DrawImagePt(dimg2, Math::Coord2DDbl(UOSInt2Double((60 - dimg2->GetWidth()) >> 1), UOSInt2Double((60 - dimg2->GetHeight()) >> 1)));
				status->drawEng->DeleteImage(dimg2);
			}
		}
	}
}

Int32 MTDrawTest()
{
	Text::CStringNN file1 = CSTR("");
	Text::CStringNN file2 = CSTR("");
	NN<Media::DrawEngine> drawEng = Media::DrawEngineFactory::CreateDrawEngine();
	Parser::FullParserList parsers;
	Optional<Media::ImageList> img1;
	Optional<Media::ImageList> img2;
	{
		IO::StmData::FileData fd1(file1, false);
		IO::StmData::FileData fd2(file2, false);
		img1 = Optional<Media::ImageList>::ConvertFrom(parsers.ParseFileType(fd1, IO::ParserType::ImageList));
		img2 = Optional<Media::ImageList>::ConvertFrom(parsers.ParseFileType(fd2, IO::ParserType::ImageList));
	}
	MTDrawStatus status;
	if (img1.SetTo(status.img1) && img2.SetTo(status.img2))
	{
		status.drawEng = drawEng;
		Sync::Thread thread(MTDrawThread, &status, CSTR("MTDrawTest"));
		thread.StartMulti(10);
		thread.WaitForEnd();
	}
	img1.Delete();
	img2.Delete();
	drawEng.Delete();
	return 0;
}

Int32 QuadrilateralTest()
{
	Math::RectAreaDbl rect(Math::Coord2DDbl(0, 0), Math::Coord2DDbl(5, 5));
	Math::Quadrilateral quad = rect.ToQuadrilateral();
	printf("Test 1: %d\r\n", quad.InsideOrTouch(Math::Coord2DDbl(2, 2)));
	printf("Test 2: %d\r\n", quad.InsideOrTouch(Math::Coord2DDbl(0, 0)));
	printf("Test 3: %d\r\n", quad.InsideOrTouch(Math::Coord2DDbl(0, 2)));
	printf("Test 4: %d\r\n", quad.InsideOrTouch(Math::Coord2DDbl(2, 0)));
	printf("Test 5: %d\r\n", quad.InsideOrTouch(Math::Coord2DDbl(5, 5)));
	printf("Test 6: %d\r\n", quad.InsideOrTouch(Math::Coord2DDbl(-1, 2)));
	printf("Test 7: %d\r\n", quad.InsideOrTouch(Math::Coord2DDbl(2, -1)));
	printf("Test 8: %d\r\n", quad.InsideOrTouch(Math::Coord2DDbl(6, 2)));
	printf("Test 9: %d\r\n", quad.InsideOrTouch(Math::Coord2DDbl(2, 6)));
	printf("Test 10: %d\r\n", quad.InsideOrTouch(Math::Coord2DDbl(5.001, 2)));
	return 0;
}

Int32 CoordRotateTest()
{
	Double angle = 10 * Math::PI / 180;
	UOSInt i = 0;
	UOSInt j = 36;
	Math::Coord2DDbl pt = Math::Coord2DDbl(1, 0);
	Math::Coord2DDbl c = Math::Coord2DDbl(1, 1);
	while (i < j)
	{
		printf("Step %d: (%lf, %lf)\r\n", (Int32)i, pt.x, pt.y);
		pt = pt.Rotate(angle, c);
		i++;
	}
	return 0;
}

Int32 TriangleRemapTest()
{
	Math::Triangle srcTri(Math::Coord2DDbl(-1, -1),
		Math::Coord2DDbl(1, -1),
		Math::Coord2DDbl(-1, 1));
	Math::Triangle destTri(Math::Coord2DDbl(1, 0),
		Math::Coord2DDbl(2, 2),
		Math::Coord2DDbl(0, 2));
	Math::Coord2DDbl pt(1, 1);
	Math::Coord2DDbl res = srcTri.Remap(pt, destTri);
	printf("Dest Pos = (%lf, %lf)\r\n", res.x, res.y);
	return 0;
}

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UOSInt testType = 27;
	switch (testType)
	{
	case 0:
		return Test0();
	case 1:
		return Test1();
	case 2:
		return InPolygonTest();
	case 3:
		return RenameFileTest();
	case 4:
		return AXCAN_BYDC9RTest();
	case 5:
		return ProcessExecTest();
	case 6:
		return CFBTimeTest();
	case 7:
		return ESRIFeatureServerTest();
	case 8:
		return HKOTest();
	case 9:
		return WKBTest();
	case 10:
		return SSHTest(progCtrl);
	case 11:
		return PaperSize();
	case 12:
		return CurveToLine();
	case 13:
		return SQLConvFunc();
	case 14:
		return SMTPProxyTest();
	case 15:
		return HTTPSProxyCliTest();
	case 16:
		return AWSEmailTest();
	case 17:
		return JasyptTest();
	case 18:
		return FirebaseTest();
	case 19:
		return FCMTest();
	case 20:
		return WKTParseTest();
	case 21:
		return FGDBTest();
	case 22:
		return BezierCurveTest();
	case 23:
		return CSysTest();
	case 24:
		return ClamAVTest();
	case 25:
		return ECDSATest();
	case 26:
		return MTDrawTest();
	case 27:
		return QuadrilateralTest();
	case 28:
		return CoordRotateTest();
	case 29:
		return TriangleRemapTest();
	default:
		return 0;
	}
	
	return WKBTest();
}
