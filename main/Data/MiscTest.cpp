#include "Stdafx.h"
#include "Core/Core.h"
#include "DB/MSSQLConn.h"
#include "IO/ConsoleWriter.h"
#include "IO/ConsoleLogHandler.h"
#include "IO/FileStream.h"
#include "IO/FileUtil.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "IO/Device/AXCAN.h"
#include "IO/Device/BYDC9RHandler.h"
#include "IO/ProtoHdlr/ProtoJMVL01Handler.h"
#include "IO/StmData/MemoryDataRef.h"
#include "Manage/Process.h"
#include "Map/ESRI/ESRIFeatureServer.h"
#include "Math/GeometryTool.h"
#include "Math/WKBReader.h"
#include "Math/WKBWriter.h"
#include "Math/WKTReader.h"
#include "Media/PaperSize.h"
#include "Net/HKOAPI.h"
#include "Net/HTTPClient.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSHManager.h"
#include "Net/SSLEngineFactory.h"
#include "Net/Email/AWSEmailClient.h"
#include "Net/Email/SMTPClient.h"
#include "Parser/FullParserList.h"
#include "Sync/SimpleThread.h"
#include "Text/CPPText.h"
#include "Text/StringTool.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"
#include "Text/XMLReader.h"
#include <stdio.h>

class ProtoListener : public IO::IProtocolHandler::DataListener
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
				if (vec->Equals(vec2, true, false))
					printf("Same as original\r\n");
				vec2.Delete();
			}
		}
		vec.Delete();
	}
	return 0;
}

Int32 SSHTest(NN<Core::IProgControl> progCtrl)
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

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	UOSInt testType = 15;
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
	default:
		return 0;
	}
	
	return WKBTest();
}
