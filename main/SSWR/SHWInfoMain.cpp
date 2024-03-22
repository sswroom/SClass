#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Data/ByteTool.h"
#include "IO/BuildTime.h"
#include "IO/ConsoleWriter.h"
#include "IO/DeviceDB.h"
#include "IO/FileStream.h"
#include "IO/OS.h"
#include "IO/Path.h"
#include "IO/PCIInfo.h"
#include "IO/PowerInfo.h"
#include "IO/SDCardMgr.h"
#include "IO/SensorManager.h"
#include "IO/SMBIOSUtil.h"
#include "IO/SystemInfo.h"
#include "IO/USBInfo.h"
#include "Manage/CPUDB.h"
#include "Manage/CPUInfoDetail.h"
#include "Manage/ExceptionRecorder.h"
#include "Math/Math.h"
#include "Math/Unit/Distance.h"
#include "Math/Unit/Pressure.h"
#include "Media/DDCReader.h"
#include "Media/DrawEngineFactory.h"
#include "Media/EDID.h"
#include "Media/PaperSize.h"
#include "Media/Printer.h"
#include "Media/VideoCaptureMgr.h"
#include "Net/ConnectionInfo.h"
#include "Net/OSSocketFactory.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"

class PrintTest : public Media::IPrintHandler
{
private:
	OSInt pageId;
	Optional<Media::IPrintDocument> doc;

public:
	PrintTest()
	{
		this->pageId = 0;
		this->doc = 0;
	}

	virtual ~PrintTest()
	{
	}

	virtual Bool BeginPrint(NotNullPtr<Media::IPrintDocument> doc)
	{
		Media::PaperSize psize(Media::PaperSize::PT_A4);
		this->pageId = 0;
		this->doc = doc;
		doc->SetDocName(CSTR("PrintTest"));
		doc->SetNextPageOrientation(Media::IPrintDocument::PageOrientation::Portrait);
		doc->SetNextPagePaperSizeMM(psize.GetWidthMM(), psize.GetHeightMM());
		return true;
	}

	virtual Bool PrintPage(NotNullPtr<Media::DrawImage> printPage) //return has more pages
	{
		UOSInt w = printPage->GetWidth();
		UOSInt h = printPage->GetHeight();
		Double hdpi = printPage->GetHDPI();
		Double vdpi = printPage->GetVDPI();
		NotNullPtr<Media::DrawPen> p = printPage->NewPenARGB(0xff000000, 1, 0, 0);
		printPage->DrawRect(Math::Coord2DDbl(hdpi * 0.5, vdpi * 0.5), Math::Size2DDbl(UOSInt2Double(w) - hdpi, UOSInt2Double(h) - vdpi), p, 0);
		printPage->DelPen(p);

		NotNullPtr<Media::IPrintDocument> doc;
		if (this->pageId == 0 && this->doc.SetTo(doc))
		{
			this->pageId = 1;
			doc->SetNextPageOrientation(Media::IPrintDocument::PageOrientation::Landscape);
			return true;
		}
		else
		{
			return false;
		}
	}

	virtual Bool EndPrint(NotNullPtr<Media::IPrintDocument> doc)
	{
		return true;
	}
};

UTF8Char *ByteDisp(UTF8Char *sbuff, UInt64 byteSize)
{
	if (byteSize >= 1073741824)
	{
		return Text::StrConcatC(Text::StrDoubleFmt(sbuff, (Double)byteSize / 1073741824.0, "0.##"), UTF8STRC("GB"));
	}
	else if (byteSize >= 1048576)
	{
		return Text::StrConcatC(Text::StrDoubleFmt(sbuff, (Double)byteSize / 1048576.0, "0.##"), UTF8STRC("MB"));
	}
	else if (byteSize >= 1024)
	{
		return Text::StrConcatC(Text::StrDoubleFmt(sbuff, (Double)byteSize / 1024.0, "0.##"), UTF8STRC("KB"));
	}
	else
	{
		return Text::StrConcatC(Text::StrUInt64(sbuff, byteSize), UTF8STRC("B"));
	}
}

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	Manage::ExceptionRecorder *exHdlr;
	IO::ConsoleWriter *console;

	NotNullPtr<IO::FileStream> fs;
	Text::UTF8Writer *writer;
	UInt64 memSize;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	UOSInt threadCnt = Sync::ThreadUtil::GetThreadCnt();

	MemSetLogFile(UTF8STRC("Memory.log"));
	NEW_CLASS(exHdlr, Manage::ExceptionRecorder(CSTR("SHWInfo.log"), Manage::ExceptionRecorder::EA_CLOSE));
	NEW_CLASS(console, IO::ConsoleWriter());

	IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::ReplaceExt(sbuff, UTF8STRC("txt"));
	NEW_CLASSNN(fs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASS(writer, Text::UTF8Writer(fs));

	Text::StringBuilderUTF8 sb;
	{
		IO::SystemInfo sysInfo;
		console->WriteLineC(UTF8STRC("Computer Info:"));
		writer->WriteLineC(UTF8STRC("Computer Info:"));
		sb.ClearStr();
		sb.AppendC(UTF8STRC("OS: "));
		if ((sptr = IO::OS::GetDistro(sbuff)) != 0)
		{
			sb.AppendP(sbuff, sptr);
		}
		else
		{
			sb.AppendC(UTF8STRC("Unknown"));
		}
		if ((sptr = IO::OS::GetVersion(sbuff)) != 0)
		{
			sb.AppendC(UTF8STRC(" "));
			sb.AppendP(sbuff, sptr);
		}
		console->WriteLineC(sb.ToString(), sb.GetLength());
		writer->WriteLineC(sb.ToString(), sb.GetLength());
		{
			Data::DateTime dt;
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Build Time: "));
			sb.AppendTSNoZone(IO::BuildTime::GetBuildTime().ToUTCTime());
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());
		}

		sb.ClearStr();
		sb.AppendC(UTF8STRC("Platform: "));
		if ((sptr = sysInfo.GetPlatformName(sbuff)) != 0)
		{
			sb.AppendP(sbuff, sptr);
		}
		else
		{
			sb.AppendC(UTF8STRC("-"));
		}
		console->WriteLineC(sb.ToString(), sb.GetLength());
		writer->WriteLineC(sb.ToString(), sb.GetLength());

		sb.ClearStr();
		sb.AppendC(UTF8STRC("Platform SN: "));
		if ((sptr = sysInfo.GetPlatformSN(sbuff)) != 0)
		{
			sb.AppendP(sbuff, sptr);
		}
		else
		{
			sb.AppendC(UTF8STRC("-"));
		}
		console->WriteLineC(sb.ToString(), sb.GetLength());
		writer->WriteLineC(sb.ToString(), sb.GetLength());

		{
			Manage::CPUInfoDetail cpuInfo;
			sb.ClearStr();
			sb.AppendC(UTF8STRC("CPU Name: "));
			if ((sptr = cpuInfo.GetCPUName(sbuff)) != 0)
			{
				sb.AppendP(sbuff, sptr);
			}
			else
			{
				sb.AppendC(UTF8STRC("-"));
			}
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			const Manage::CPUDB::CPUSpec *cpuSpec = 0;
			sb.ClearStr();
			sb.AppendC(UTF8STRC("CPU Model: "));
			if (cpuInfo.GetCPUModel().v)
			{
				sb.Append(cpuInfo.GetCPUModel());
				cpuSpec = Manage::CPUDB::GetCPUSpec(cpuInfo.GetCPUModel());
			}
			else
			{
				sb.AppendC(UTF8STRC("-"));
			}
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			sb.ClearStr();
			sb.AppendC(UTF8STRC("CPU Brand Name: "));
			if (cpuSpec)
			{
				sb.Append(Manage::CPUVendor::GetBrandName(cpuSpec->brand));
				sb.AppendC(UTF8STRC(" "));
				sb.AppendSlow((const UTF8Char*)cpuSpec->name);
			}
			else
			{
				sb.AppendC(UTF8STRC("-"));
			}
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

	#if defined(CPU_X86_32) || defined(CPU_X86_64)
			sb.ClearStr();
			sb.AppendC(UTF8STRC("CPU FamilyId = "));
			sb.AppendI32(cpuInfo.GetFamilyId());
			sb.AppendC(UTF8STRC(", ModelId = "));
			sb.AppendI32(cpuInfo.GetModelId());
			sb.AppendC(UTF8STRC(", Stepping = "));
			sb.AppendI32(cpuInfo.GetStepping());
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());
	#endif

			sb.ClearStr();
			sb.AppendC(UTF8STRC("CPU Thread Count: "));
			sb.AppendUOSInt(threadCnt);
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			i = 0;
			while (true)
			{
				Double temp;

				if (!cpuInfo.GetCPUTemp(i, &temp))
					break;

				sb.ClearStr();
				sb.AppendC(UTF8STRC("CPU Temp "));
				sb.AppendUOSInt(i);
				sb.AppendC(UTF8STRC(": "));
				sb.AppendDouble(temp);
				console->WriteLineC(sb.ToString(), sb.GetLength());
				writer->WriteLineC(sb.ToString(), sb.GetLength());

				i++;
			}
		}

		sb.ClearStr();
		sb.AppendC(UTF8STRC("Total Memory Size: "));
		memSize = sysInfo.GetTotalMemSize();
		sptr = ByteDisp(sbuff, memSize);
		sb.AppendP(sbuff, sptr);
		console->WriteLineC(sb.ToString(), sb.GetLength());
		writer->WriteLineC(sb.ToString(), sb.GetLength());

		sb.ClearStr();
		sb.AppendC(UTF8STRC("Total Usable Memory Size: "));
		memSize = sysInfo.GetTotalUsableMemSize();
		sptr = ByteDisp(sbuff, memSize);
		sb.AppendP(sbuff, sptr);
		console->WriteLineC(sb.ToString(), sb.GetLength());
		writer->WriteLineC(sb.ToString(), sb.GetLength());

		{
			Data::ArrayList<IO::SystemInfo::RAMInfo*> ramList;
			IO::SystemInfo::RAMInfo *ram;
			sysInfo.GetRAMInfo(&ramList);
			i = 0;
			j = ramList.GetCount();
			while (i < j)
			{
				ram = ramList.GetItem(i);
				sb.ClearStr();
				sb.AppendC(UTF8STRC("RAM: "));
				sb.AppendOpt(ram->deviceLocator);
				sb.AppendC(UTF8STRC("\t"));
				sb.AppendOpt(ram->manufacturer);
				sb.AppendC(UTF8STRC("\t"));
				sb.AppendOpt(ram->partNo);
				sb.AppendC(UTF8STRC("\t"));
				sb.AppendOpt(ram->sn);
				sb.AppendC(UTF8STRC("\t"));
				sb.AppendU64(ram->defSpdMHz);
				sb.AppendC(UTF8STRC("\t"));
				sb.AppendU64(ram->confSpdMHz);
				sb.AppendC(UTF8STRC("\t"));
				sb.AppendU32(ram->dataWidth);
				sb.AppendC(UTF8STRC("\t"));
				sb.AppendU32(ram->totalWidth);
				sb.AppendC(UTF8STRC("\t"));
				sb.AppendU64(ram->memorySize);
				console->WriteLineC(sb.ToString(), sb.GetLength());
				writer->WriteLineC(sb.ToString(), sb.GetLength());
				i++;
			}
			sysInfo.FreeRAMInfo(&ramList);
		}
	}
	{
		Data::ArrayList<Media::DDCReader *> readerList;
		Media::DDCReader *reader;
		Media::DDCReader::CreateDDCReaders(&readerList);
		UOSInt edidSize;
		UInt8 *edid;
		i = 0;
		j = readerList.GetCount();
		while (i < j)
		{
			reader = readerList.GetItem(i);
			edid = reader->GetEDID(&edidSize);
			if (edid)
			{
				Media::EDID::EDIDInfo edidInfo;
				if (Media::EDID::Parse(edid, edidInfo))
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("Monitor "));
					sb.AppendUOSInt(i);
					sb.AppendC(UTF8STRC(" - "));
					sb.AppendSlow(edidInfo.monitorName);
					sb.AppendC(UTF8STRC(" ("));
					sb.AppendSlow(edidInfo.vendorName);
					sb.AppendC(UTF8STRC(" "));
					sb.AppendHex16(edidInfo.productCode);
					sb.AppendC(UTF8STRC(")"));
					console->WriteLineC(sb.ToString(), sb.GetLength());
					writer->WriteLineC(sb.ToString(), sb.GetLength());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("Monitor Manufacture: Year "));
					sb.AppendI32(edidInfo.yearOfManu);
					sb.AppendC(UTF8STRC(" Week "));
					sb.AppendU32(edidInfo.weekOfManu);
					console->WriteLineC(sb.ToString(), sb.GetLength());
					writer->WriteLineC(sb.ToString(), sb.GetLength());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("Monitor Size: "));
					sb.AppendDouble(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_INCH, Math_Sqrt(edidInfo.dispPhysicalW_mm * edidInfo.dispPhysicalW_mm + edidInfo.dispPhysicalH_mm * edidInfo.dispPhysicalH_mm)));
					sb.AppendC(UTF8STRC("\" ("));
					sb.AppendU32(edidInfo.dispPhysicalW_mm);
					sb.AppendC(UTF8STRC(" x "));
					sb.AppendU32(edidInfo.dispPhysicalH_mm);
					sb.AppendC(UTF8STRC("mm)"));
					console->WriteLineC(sb.ToString(), sb.GetLength());
					writer->WriteLineC(sb.ToString(), sb.GetLength());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("Monitor Pixels: "));
					sb.AppendU32(edidInfo.pixelW);
					sb.AppendC(UTF8STRC(" x "));
					sb.AppendU32(edidInfo.pixelH);
					console->WriteLineC(sb.ToString(), sb.GetLength());
					writer->WriteLineC(sb.ToString(), sb.GetLength());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("Monitor bit per color: "));
					sb.AppendU32(edidInfo.bitPerColor);
					console->WriteLineC(sb.ToString(), sb.GetLength());
					writer->WriteLineC(sb.ToString(), sb.GetLength());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("Monitor White: x = "));
					sb.AppendDouble(edidInfo.w.x);
					sb.AppendC(UTF8STRC(", y = "));
					sb.AppendDouble(edidInfo.w.y);
					console->WriteLineC(sb.ToString(), sb.GetLength());
					writer->WriteLineC(sb.ToString(), sb.GetLength());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("Monitor Red x = "));
					sb.AppendDouble(edidInfo.r.x);
					sb.AppendC(UTF8STRC(", y = "));
					sb.AppendDouble(edidInfo.r.y);
					console->WriteLineC(sb.ToString(), sb.GetLength());
					writer->WriteLineC(sb.ToString(), sb.GetLength());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("Monitor Green: x = "));
					sb.AppendDouble(edidInfo.g.x);
					sb.AppendC(UTF8STRC(", y = "));
					sb.AppendDouble(edidInfo.g.y);
					console->WriteLineC(sb.ToString(), sb.GetLength());
					writer->WriteLineC(sb.ToString(), sb.GetLength());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("Monitor Blue: x = "));
					sb.AppendDouble(edidInfo.b.x);
					sb.AppendC(UTF8STRC(", y = "));
					sb.AppendDouble(edidInfo.b.y);
					console->WriteLineC(sb.ToString(), sb.GetLength());
					writer->WriteLineC(sb.ToString(), sb.GetLength());
				}
			}
			DEL_CLASS(reader);
			i++;
		}
	}

//-------------------------------------------------------------------------------
	IO::SMBIOS *smbios = IO::SMBIOSUtil::GetSMBIOS();
	if (smbios)
	{
		console->WriteLine();
		writer->WriteLine();
		console->WriteLineC(UTF8STRC("SMBIOS:"));
		writer->WriteLineC(UTF8STRC("SMBIOS:"));
		sb.ClearStr();
		smbios->ToString(sb);
		console->WriteLineC(sb.ToString(), sb.GetLength());
		writer->WriteLineC(sb.ToString(), sb.GetLength());

		DEL_CLASS(smbios);
	}
//-------------------------------------------------------------------------------
	IO::PowerInfo::PowerStatus pstatus;
	if (IO::PowerInfo::GetPowerStatus(&pstatus))
	{
		console->WriteLine();
		writer->WriteLine();
		console->WriteLineC(UTF8STRC("Power Info:"));
		writer->WriteLineC(UTF8STRC("Power Info:"));

		sb.ClearStr();
		sb.AppendC(UTF8STRC("AC Status = "));
		if (pstatus.acStatus == IO::PowerInfo::ACS_ON)
		{
			sb.AppendC(UTF8STRC("On"));
		}
		else if (pstatus.acStatus == IO::PowerInfo::ACS_OFF)
		{
			sb.AppendC(UTF8STRC("Off"));
		}
		else
		{
			sb.AppendC(UTF8STRC("Unknown"));
		}
		console->WriteLineC(sb.ToString(), sb.GetLength());
		writer->WriteLineC(sb.ToString(), sb.GetLength());

		sb.ClearStr();
		sb.AppendC(UTF8STRC("Has Battery = "));
		if (pstatus.hasBattery)
		{
			sb.AppendC(UTF8STRC("Yes"));
		}
		else
		{
			sb.AppendC(UTF8STRC("No"));
		}
		console->WriteLineC(sb.ToString(), sb.GetLength());
		writer->WriteLineC(sb.ToString(), sb.GetLength());

		sb.ClearStr();
		sb.AppendC(UTF8STRC("Battery Charging = "));
		if (pstatus.batteryCharging)
		{
			sb.AppendC(UTF8STRC("Yes"));
		}
		else
		{
			sb.AppendC(UTF8STRC("No"));
		}
		console->WriteLineC(sb.ToString(), sb.GetLength());
		writer->WriteLineC(sb.ToString(), sb.GetLength());

		sb.ClearStr();
		sb.AppendC(UTF8STRC("Battery Percent = "));
		sb.AppendU32(pstatus.batteryPercent);
		console->WriteLineC(sb.ToString(), sb.GetLength());
		writer->WriteLineC(sb.ToString(), sb.GetLength());

		sb.ClearStr();
		sb.AppendC(UTF8STRC("Time Left (sec.) = "));
		sb.AppendU32(pstatus.timeLeftSec);
		console->WriteLineC(sb.ToString(), sb.GetLength());
		writer->WriteLineC(sb.ToString(), sb.GetLength());

		sb.ClearStr();
		sb.AppendC(UTF8STRC("Battery Voltage = "));
		sb.AppendDouble(pstatus.batteryVoltage);
		console->WriteLineC(sb.ToString(), sb.GetLength());
		writer->WriteLineC(sb.ToString(), sb.GetLength());

		sb.ClearStr();
		sb.AppendC(UTF8STRC("Battery Charge Current = "));
		sb.AppendDouble(pstatus.batteryChargeCurrent);
		console->WriteLineC(sb.ToString(), sb.GetLength());
		writer->WriteLineC(sb.ToString(), sb.GetLength());

		sb.ClearStr();
		sb.AppendC(UTF8STRC("Battery Temperature = "));
		sb.AppendDouble(pstatus.batteryTemp);
		console->WriteLineC(sb.ToString(), sb.GetLength());
		writer->WriteLineC(sb.ToString(), sb.GetLength());
	}

//-------------------------------------------------------------------------------
	{
		console->WriteLine();
		writer->WriteLine();
		console->WriteLineC(UTF8STRC("Printer Info:"));
		writer->WriteLineC(UTF8STRC("Printer Info:"));
		Data::ArrayListStringNN printerList;
		i = 0;
		j = Media::Printer::GetPrinterCount();
		while (i < j)
		{
			if ((sptr = Media::Printer::GetPrinterName(sbuff, i)) != 0)
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Printer "));
				sb.AppendUOSInt(i);
				sb.AppendC(UTF8STRC(" = "));
				sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
				console->WriteLineC(sb.ToString(), sb.GetLength());
				writer->WriteLineC(sb.ToString(), sb.GetLength());

				if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Bullzip PDF Printer")) ||
					Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Adobe PDF")) ||
					Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("PDF")))
				{
					printerList.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				}
			}

			i++;
		}
		i = printerList.GetCount();
		if (i > 0)
		{
			NotNullPtr<Text::String> s;
			Media::Printer *printer;
			NotNullPtr<Media::DrawEngine> deng = Media::DrawEngineFactory::CreateDrawEngine();
			Data::ArrayIterator<NotNullPtr<Text::String>> it = printerList.Iterator();
			while (it.HasNext())
			{
				s = it.Next();

				sb.ClearStr();
				sb.AppendC(UTF8STRC("Test Printing with "));
				sb.Append(s);
				console->WriteLineC(sb.ToString(), sb.GetLength());
				writer->WriteLineC(sb.ToString(), sb.GetLength());

				NEW_CLASS(printer, Media::Printer(s->ToCString()));
				if (printer->IsError())
				{
					console->WriteLineC(UTF8STRC("Error in opening printer"));
					writer->WriteLineC(UTF8STRC("Error in opening printer"));
				}
				else
				{
					PrintTest *test;
					NEW_CLASS(test, PrintTest());
					NotNullPtr<Media::IPrintDocument> doc;
					if (printer->StartPrint(test, deng).SetTo(doc))
					{
						printer->EndPrint(doc);
					}
					else
					{
						console->WriteLineC(UTF8STRC("Error in start printing"));
						writer->WriteLineC(UTF8STRC("Error in start printing"));
					}
					DEL_CLASS(test);
				}
				DEL_CLASS(printer);

				s->Release();
			}
			deng.Delete();
		}
	}
//-------------------------------------------------------------------------------
	{
		Media::VideoCaptureMgr *videoMgr;
		Data::ArrayList<Media::VideoCaptureMgr::DeviceInfo*> devList;
		Media::VideoCaptureMgr::DeviceInfo *dev;
		Media::IVideoCapture *capture;
		Media::IVideoCapture::VideoFormat fmts[512];
		Char fmtName[5];
		console->WriteLine();
		writer->WriteLine();
		console->WriteLineC(UTF8STRC("Video Capture Info:"));
		writer->WriteLineC(UTF8STRC("Video Capture Info:"));
		NEW_CLASS(videoMgr, Media::VideoCaptureMgr());
		videoMgr->GetDeviceList(&devList);
		i = 0;
		j = devList.GetCount();
		if (j > 0)
		{
			fmtName[4] = 0;
			while (i < j)
			{
				dev = devList.GetItem(i);
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Device "));
				sb.AppendUOSInt(i);
				sb.AppendC(UTF8STRC(": "));
				sb.Append(videoMgr->GetDevTypeName(dev->devType));
				sb.AppendC(UTF8STRC(", "));
				sb.AppendSlow(dev->devName);
				console->WriteLineC(sb.ToString(), sb.GetLength());
				writer->WriteLineC(sb.ToString(), sb.GetLength());

				capture = videoMgr->CreateDevice(dev->devType, dev->devId);
				if (capture)
				{
					sb.ClearStr();
					capture->GetInfo(sb);
					console->WriteLineC(sb.ToString(), sb.GetLength());
					writer->WriteLineC(sb.ToString(), sb.GetLength());
					console->WriteLineC(UTF8STRC("Format List:"));
					writer->WriteLineC(UTF8STRC("Format List:"));
					k = 0;
					l = capture->GetSupportedFormats(fmts, 512);
					while (k < l)
					{
						sb.ClearStr();
						sb.AppendC(UTF8STRC("Format "));
						if (fmts[k].info.fourcc == 0)
						{
							sb.AppendC(UTF8STRC("RGB"));
						}
						else
						{
							*(UInt32*)fmtName = fmts[k].info.fourcc;
							sb.AppendSlow((const UTF8Char*)fmtName);
						}
						sb.AppendC(UTF8STRC(", bpp = "));
						sb.AppendU32(fmts[k].info.storeBPP);
						sb.AppendC(UTF8STRC(", size = "));
						sb.AppendUOSInt(fmts[k].info.dispSize.x);
						sb.AppendC(UTF8STRC(" x "));
						sb.AppendUOSInt(fmts[k].info.dispSize.y);
						sb.AppendC(UTF8STRC(", rate = "));
						sb.AppendDouble(fmts[k].frameRateNorm / (Double)fmts[k].frameRateDenorm);
						console->WriteLineC(sb.ToString(), sb.GetLength());
						writer->WriteLineC(sb.ToString(), sb.GetLength());
						k++;
					}
					DEL_CLASS(capture);
				}

				i++;
			}
		}
		videoMgr->FreeDeviceList(&devList);

		DEL_CLASS(videoMgr);
	}
//-------------------------------------------------------------------------------
	{
		UInt32 ipv4;
		UInt8 mac[16];
		Data::Timestamp ts;
		Net::ConnectionInfo *connInfo;
		Data::ArrayList<Net::ConnectionInfo*> connInfoList;
		NotNullPtr<Net::SocketFactory> sockf;
		NEW_CLASSNN(sockf, Net::OSSocketFactory(true));
		sockf->GetConnInfoList(&connInfoList);
		console->WriteLine();
		writer->WriteLine();
		console->WriteLineC(UTF8STRC("Network Info:"));
		writer->WriteLineC(UTF8STRC("Network Info:"));
		i = 0;
		j = connInfoList.GetCount();
		while (i < j)
		{
			connInfo = connInfoList.GetItem(i);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Connection "));
			sb.AppendUOSInt(i);
			sb.AppendC(UTF8STRC(":"));
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			if ((sptr = connInfo->GetName(sbuff)) != 0)
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Name: "));
				sb.AppendP(sbuff, sptr);
				console->WriteLineC(sb.ToString(), sb.GetLength());
				writer->WriteLineC(sb.ToString(), sb.GetLength());
			}
			if ((sptr = connInfo->GetDescription(sbuff)) != 0)
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Description: "));
				sb.AppendP(sbuff, sptr);
				console->WriteLineC(sb.ToString(), sb.GetLength());
				writer->WriteLineC(sb.ToString(), sb.GetLength());
			}
			if ((sptr = connInfo->GetDNSSuffix(sbuff)) != 0)
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("DNS Suffix: "));
				sb.AppendP(sbuff, sptr);
				console->WriteLineC(sb.ToString(), sb.GetLength());
				writer->WriteLineC(sb.ToString(), sb.GetLength());
			}
			k = 0;
			while (k < 32)
			{
				if ((ipv4 = connInfo->GetIPAddress(k)) == 0)
				{
					break;
				}
				sptr = Net::SocketUtil::GetIPv4Name(sbuff, ipv4);
				sb.ClearStr();
				sb.AppendC(UTF8STRC("IPv4 Address: "));
				sb.AppendP(sbuff, sptr);
				console->WriteLineC(sb.ToString(), sb.GetLength());
				writer->WriteLineC(sb.ToString(), sb.GetLength());
				k++;
			}
			k = 0;
			while (k < 32)
			{
				if ((ipv4 = connInfo->GetDNSAddress(k)) == 0)
				{
					break;
				}
				sptr = Net::SocketUtil::GetIPv4Name(sbuff, ipv4);
				sb.ClearStr();
				sb.AppendC(UTF8STRC("DNS Address: "));
				sb.AppendP(sbuff, sptr);
				console->WriteLineC(sb.ToString(), sb.GetLength());
				writer->WriteLineC(sb.ToString(), sb.GetLength());
				k++;
			}
			if ((ipv4 = connInfo->GetDefaultGW()) != 0)
			{
				sptr = Net::SocketUtil::GetIPv4Name(sbuff, ipv4);
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Default Gateway: "));
				sb.AppendP(sbuff, sptr);
				console->WriteLineC(sb.ToString(), sb.GetLength());
				writer->WriteLineC(sb.ToString(), sb.GetLength());
			}
			if ((k = connInfo->GetPhysicalAddress(mac, 16)) > 0)
			{
				printf("size = %d\r\n", (UInt32)k);
				sptr = Text::StrHexBytes(sbuff, mac, k, '-');
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Physical Address: "));
				sb.AppendP(sbuff, sptr);
				console->WriteLineC(sb.ToString(), sb.GetLength());
				writer->WriteLineC(sb.ToString(), sb.GetLength());
			}
			sb.ClearStr();
			sb.AppendC(UTF8STRC("MTU: "));
			sb.AppendU32(connInfo->GetMTU());
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			sb.ClearStr();
			sb.AppendC(UTF8STRC("DHCP Enabled: "));
			if (connInfo->IsDhcpEnabled())
			{
				sb.AppendC(UTF8STRC("True"));
			}
			else
			{
				sb.AppendC(UTF8STRC("False"));
			}
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			if (connInfo->IsDhcpEnabled())
			{
				sptr = Net::SocketUtil::GetIPv4Name(sbuff, connInfo->GetDhcpServer());
				sb.ClearStr();
				sb.AppendC(UTF8STRC("DHCP Server: "));
				sb.AppendP(sbuff, sptr);
				console->WriteLineC(sb.ToString(), sb.GetLength());
				writer->WriteLineC(sb.ToString(), sb.GetLength());

				ts = connInfo->GetDhcpLeaseTime();
				if (ts.ToTicks() != 0)
				{
					sptr = ts.ToString(sbuff);
					sb.ClearStr();
					sb.AppendC(UTF8STRC("DHCP Lease Time: "));
					sb.AppendP(sbuff, sptr);
					console->WriteLineC(sb.ToString(), sb.GetLength());
					writer->WriteLineC(sb.ToString(), sb.GetLength());
				}

				ts = connInfo->GetDhcpLeaseExpire();
				if (ts.ToTicks() != 0)
				{
					sptr = ts.ToString(sbuff);
					sb.ClearStr();
					sb.AppendC(UTF8STRC("DHCP Lease Time: "));
					sb.AppendP(sbuff, sptr);
					console->WriteLineC(sb.ToString(), sb.GetLength());
					writer->WriteLineC(sb.ToString(), sb.GetLength());
				}
			}

			sb.ClearStr();
			sb.AppendC(UTF8STRC("Connection Type: "));
			sb.Append(Net::ConnectionInfo::ConnectionTypeGetName(connInfo->GetConnectionType()));
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			sb.ClearStr();
			sb.AppendC(UTF8STRC("Connection Status: "));
			sb.Append(Net::ConnectionInfo::ConnectionStatusGetName(connInfo->GetConnectionStatus()));
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());
			console->WriteLine();
			writer->WriteLine();

			DEL_CLASS(connInfo);
			i++;
		}
		sockf.Delete();
	}
//-------------------------------------------------------------------------------
	{
		Data::ArrayList<IO::USBInfo*> usbList;
		IO::USBInfo *usb;
		const IO::DeviceDB::USBDeviceInfo *dev;
		IO::USBInfo::GetUSBList(usbList);
		console->WriteLine();
		writer->WriteLine();
		console->WriteLineC(UTF8STRC("USB Info:"));
		writer->WriteLineC(UTF8STRC("USB Info:"));
		i = 0;
		j = usbList.GetCount();
		while (i < j)
		{
			usb = usbList.GetItem(i);
			dev = IO::DeviceDB::GetUSBInfo(usb->GetVendorId(), usb->GetProductId(), usb->GetRevision());
			sb.ClearStr();
			sb.AppendC(UTF8STRC("USB "));
			sb.AppendHex16(usb->GetVendorId());
			sb.AppendC(UTF8STRC(":"));
			sb.AppendHex16(usb->GetProductId());
			sb.AppendC(UTF8STRC(" "));
			sb.AppendHex16(usb->GetRevision());
			sb.AppendC(UTF8STRC(" Dev:"));
			sb.Append(usb->GetDispName());
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			sb.ClearStr();
			sb.AppendC(UTF8STRC("USB "));
			sb.AppendHex16(usb->GetVendorId());
			sb.AppendC(UTF8STRC(":"));
			sb.AppendHex16(usb->GetProductId());
			sb.AppendC(UTF8STRC(" "));
			sb.AppendHex16(usb->GetRevision());
			sb.AppendC(UTF8STRC(" DB: "));
			if (dev)
			{
				if (dev->dispName)
				{
					sb.AppendSlow((const UTF8Char*)dev->dispName);
				}
				else
				{
					sb.AppendC(UTF8STRC("?"));
				}

				if (dev->productName)
				{
					sb.AppendC(UTF8STRC(" ("));
					sb.AppendSlow((const UTF8Char*)dev->productName);
					sb.AppendC(UTF8STRC(")"));
				}
			}
			else
			{
				sb.AppendC(UTF8STRC("?"));
			}
			sb.AppendC(UTF8STRC(" Vendor = "));
			sb.Append(IO::DeviceDB::GetUSBVendorName(usb->GetVendorId()));
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			DEL_CLASS(usb);
			i++;
		}
	}
//-------------------------------------------------------------------------------
	{
		Data::ArrayList<IO::PCIInfo*> pciList;
		IO::PCIInfo *pci;
		const IO::DeviceDB::DeviceInfo *dev;
		IO::PCIInfo::GetPCIList(pciList);
		console->WriteLine();
		writer->WriteLine();
		console->WriteLineC(UTF8STRC("PCI Info:"));
		writer->WriteLineC(UTF8STRC("PCI Info:"));
		i = 0;
		j = pciList.GetCount();
		while (i < j)
		{
			pci = pciList.GetItem(i);
			dev = IO::DeviceDB::GetPCIInfo(pci->GetVendorId(), pci->GetProductId());
			sb.ClearStr();
			sb.AppendC(UTF8STRC("PCI "));
			sb.AppendHex16(pci->GetVendorId());
			sb.AppendC(UTF8STRC(":"));
			sb.AppendHex16(pci->GetProductId());
			sb.AppendC(UTF8STRC(" Dev:"));
			sb.Append(pci->GetDispName());
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			sb.ClearStr();
			sb.AppendC(UTF8STRC("PCI "));
			sb.AppendHex16(pci->GetVendorId());
			sb.AppendC(UTF8STRC(":"));
			sb.AppendHex16(pci->GetProductId());
			sb.AppendC(UTF8STRC(" DB: "));
			if (dev)
			{
				if (dev->dispName)
				{
					sb.AppendSlow((const UTF8Char*)dev->dispName);
				}
				else
				{
					sb.AppendC(UTF8STRC("?"));
				}

				if (dev->productName)
				{
					sb.AppendC(UTF8STRC(" ("));
					sb.AppendSlow((const UTF8Char*)dev->productName);
					sb.AppendC(UTF8STRC(")"));
				}
			}
			else
			{
				sb.AppendC(UTF8STRC("?"));
			}
			sb.AppendC(UTF8STRC(" Vendor = "));
			sb.Append(IO::DeviceDB::GetPCIVendorName(pci->GetVendorId()));
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			DEL_CLASS(pci);
			i++;
		}
	}
//-------------------------------------------------------------------------------
	{
		Data::ArrayList<IO::SDCardInfo*> sdList;
		IO::SDCardInfo *sdcard;
		IO::SDCardMgr::GetCardList(sdList);
		console->WriteLine();
		writer->WriteLine();
		console->WriteLineC(UTF8STRC("SD Card Info:"));
		writer->WriteLineC(UTF8STRC("SD Card Info:"));
		i = 0;
		j = sdList.GetCount();
		while (i < j)
		{
			UTF8Char oemId[6];

			sdcard = sdList.GetItem(i);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("SD Card "));
			sb.AppendUOSInt(i);
			sb.AppendC(UTF8STRC(" - "));
			sb.Append(sdcard->GetName());
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			sb.ClearStr();
			sb.AppendC(UTF8STRC("Manufacturer ID (MID): "));
			sb.AppendI32(sdcard->GetManufacturerID());
			sb.AppendC(UTF8STRC(" ("));
			sb.Append(IO::SDCardInfo::GetManufacturerName(sdcard->GetManufacturerID()));
			sb.AppendC(UTF8STRC(")"));
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			sb.ClearStr();
			sb.AppendC(UTF8STRC("OEM ID (OID): "));
			WriteMInt16(oemId, sdcard->GetOEMID());
			if (oemId[0] >= 'A' && oemId[0] <= 'z' && oemId[1] >= 'A' && oemId[1] <= 'z')
			{
				oemId[2] = 0;
				sb.AppendC(oemId, 2);
			}
			else
			{
				sb.AppendC(UTF8STRC("0x"));
				sb.AppendHex16(sdcard->GetOEMID());
			}
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			sb.ClearStr();
			sb.AppendC(UTF8STRC("Product Name (PMN): "));
			sptr = (UTF8Char*)sdcard->GetProductName((Char*)oemId);
			sb.AppendP(oemId, sptr);
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			sb.ClearStr();
			sb.AppendC(UTF8STRC("Product Revision (PRV): "));
			oemId[0] = sdcard->GetProductRevision();
			sb.AppendI32(oemId[0] >> 4);
			sb.AppendUTF8Char('.');
			sb.AppendI32(oemId[0] & 15);
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			sb.ClearStr();
			sb.AppendC(UTF8STRC("Product Serial Number (PSN): 0x"));
			sb.AppendHex32(sdcard->GetSerialNo());
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			sb.ClearStr();
			sb.AppendC(UTF8STRC("Manufacturing Date (MDT): "));
			sb.AppendI32(Data::DateTimeUtil::DispYearI32(sdcard->GetManufacturingYear()));
			sb.AppendUTF8Char('/');
			sb.AppendU32(sdcard->GetManufacturingMonth());
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			sb.ClearStr();
			sb.AppendC(UTF8STRC("Card Capacity: "));
			sb.AppendI64(sdcard->GetCardCapacity());
			sb.AppendC(UTF8STRC(" Bytes"));
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			sb.ClearStr();
			sb.AppendC(UTF8STRC("Card Speed: "));
			sb.AppendI64(sdcard->GetMaxTranRate());
			sb.AppendC(UTF8STRC(" bit/s"));
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			console->WriteLine();
			writer->WriteLine();
			DEL_CLASS(sdcard);
			i++;
		}
	}
//-------------------------------------------------------------------------------
	{
		IO::SensorManager sensorMgr;
		Text::CString cstr;
		NotNullPtr<IO::Sensor> sensor;
		console->WriteLine();
		writer->WriteLine();
		console->WriteLineC(UTF8STRC("Sensors:"));
		writer->WriteLineC(UTF8STRC("Sensors:"));
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Count = "));
		sb.AppendUOSInt(j = sensorMgr.GetSensorCnt());
		console->WriteLineC(sb.ToString(), sb.GetLength());
		writer->WriteLineC(sb.ToString(), sb.GetLength());
		i = 0;
		while (i < j)
		{
			if (sensorMgr.CreateSensor(i).SetTo(sensor))
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Sensor "));
				sb.AppendUOSInt(i);
				sb.AppendC(UTF8STRC(", Name = "));
				cstr = sensor->GetName();
				if (cstr.leng)
				{
					sb.Append(cstr);
				}
				sb.AppendC(UTF8STRC(", Vendor = "));
				cstr = sensor->GetVendor();
				if (cstr.leng)
				{
					sb.Append(cstr);
				}
				sb.AppendC(UTF8STRC(", Type = "));
				sb.Append(IO::Sensor::GetSensorTypeName(sensor->GetSensorType()));
				console->WriteLineC(sb.ToString(), sb.GetLength());
				writer->WriteLineC(sb.ToString(), sb.GetLength());

				NotNullPtr<IO::SensorAccelerometer> sensorAcc;
				NotNullPtr<IO::SensorPressure> sensorPres;
				NotNullPtr<IO::SensorMagnetometer> sensorMag;
				NotNullPtr<IO::SensorLight> sensorLight;
				switch (sensor->GetSensorType())
				{
				case IO::Sensor::SensorType::Accelerometer:
					if (sensor->EnableSensor() && sensor->GetSensorAccelerator().SetTo(sensorAcc))
					{
						Math::Vector3 acc;
						if (sensorAcc->ReadAcceleration(acc))
						{
							Math::Unit::Acceleration::AccelerationUnit aunit = sensorAcc->GetAccelerationUnit();

							sb.ClearStr();
							sb.AppendC(UTF8STRC("Accelerometer x = "));
							sb.AppendDouble(acc.val[0]);
							sb.Append(Math::Unit::Acceleration::GetUnitShortName(aunit));
							sb.AppendC(UTF8STRC(", y = "));
							sb.AppendDouble(acc.val[1]);
							sb.Append(Math::Unit::Acceleration::GetUnitShortName(aunit));
							sb.AppendC(UTF8STRC(", z = "));
							sb.AppendDouble(acc.val[2]);
							sb.Append(Math::Unit::Acceleration::GetUnitShortName(aunit));
							console->WriteLineC(sb.ToString(), sb.GetLength());
							writer->WriteLineC(sb.ToString(), sb.GetLength());
						}
						sensor->DisableSensor();
					}
					break;
				case IO::Sensor::SensorType::Pressure:
					if (sensor->EnableSensor() && sensor->GetSensorPressure().SetTo(sensorPres))
					{
						Double pressure;
						if (sensorPres->ReadPressure(pressure))
						{
							sb.ClearStr();
							sb.AppendC(UTF8STRC("Pressure = "));
							sb.AppendDouble(pressure);
							sb.Append(Math::Unit::Pressure::GetUnitShortName(sensorPres->GetPressureUnit()));
							console->WriteLineC(sb.ToString(), sb.GetLength());
							writer->WriteLineC(sb.ToString(), sb.GetLength());
						}
						sensor->DisableSensor();
					}
					break;
				case IO::Sensor::SensorType::Magnetometer:
					if (sensor->EnableSensor() && sensor->GetSensorMagnetometer().SetTo(sensorMag))
					{
						Math::Vector3 mag;
						if (sensorMag->ReadMagneticField(mag))
						{
							Math::Unit::MagneticField::MagneticFieldUnit mfunit = sensorMag->GetMagneticFieldUnit();

							sb.ClearStr();
							sb.AppendC(UTF8STRC("Magnetometer x = "));
							sb.AppendDouble(mag.val[0]);
							sb.Append(Math::Unit::MagneticField::GetUnitShortName(mfunit));
							sb.AppendC(UTF8STRC(", y = "));
							sb.AppendDouble(mag.val[1]);
							sb.Append(Math::Unit::MagneticField::GetUnitShortName(mfunit));
							sb.AppendC(UTF8STRC(", z = "));
							sb.AppendDouble(mag.val[2]);
							sb.Append(Math::Unit::MagneticField::GetUnitShortName(mfunit));
							console->WriteLineC(sb.ToString(), sb.GetLength());
							writer->WriteLineC(sb.ToString(), sb.GetLength());
						}
						sensor->DisableSensor();
					}
					break;
				case IO::Sensor::SensorType::Light:
					if (sensor->EnableSensor() && sensor->GetSensorLight().SetTo(sensorLight))
					{
						Double lux;
						Double colorTemp;
						if (sensorLight->ReadLights(lux, colorTemp))
						{
							sb.ClearStr();
							sb.AppendC(UTF8STRC("Light: lux = "));
							sb.AppendDouble(lux);
							sb.AppendC(UTF8STRC(", colorTemp = "));
							sb.AppendDouble(colorTemp);
							console->WriteLineC(sb.ToString(), sb.GetLength());
							writer->WriteLineC(sb.ToString(), sb.GetLength());
						}
						sensor->DisableSensor();
					}
					break;
				case IO::Sensor::SensorType::Location:
				case IO::Sensor::SensorType::Orientation:
				case IO::Sensor::SensorType::Unknown:
				default:
					break;
				}
				sensor.Delete();
			}
			i++;
		}
	}
//-------------------------------------------------------------------------------
	DEL_CLASS(writer);

	fs.Delete();

	DEL_CLASS(console);
	DEL_CLASS(exHdlr);
	return 0;
}
