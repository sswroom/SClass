#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Core/DefaultDrawEngine.h"
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
#include "IO/SystemInfo.h"
#include "IO/USBInfo.h"
#include "Manage/CPUDB.h"
#include "Manage/CPUInfoDetail.h"
#include "Manage/ExceptionRecorder.h"
#include "Math/Math.h"
#include "Math/Unit/Distance.h"
#include "Math/Unit/Pressure.h"
#include "Media/DDCReader.h"
#include "Media/EDID.h"
#include "Media/PaperSize.h"
#include "Media/Printer.h"
#include "Media/VideoCaptureMgr.h"
#include "Net/ConnectionInfo.h"
#include "Net/OSSocketFactory.h"
#include "Sync/Thread.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"
#include "Win32/SMBIOSUtil.h"

#include <stdio.h>

class PrintTest : public Media::IPrintHandler
{
private:
	OSInt pageId;
	Media::IPrintDocument *doc;

public:
	PrintTest()
	{
		this->pageId = 0;
		this->doc = 0;
	}

	virtual ~PrintTest()
	{
	}

	virtual Bool BeginPrint(Media::IPrintDocument *doc)
	{
		Media::PaperSize psize(Media::PaperSize::PT_A4);
		this->pageId = 0;
		this->doc = doc;
		doc->SetDocName((const UTF8Char*)"PrintTest");
		doc->SetNextPageOrientation(Media::IPrintDocument::PO_PORTRAIT);
		doc->SetNextPagePaperSizeMM(psize.GetWidthMM(), psize.GetHeightMM());
		return true;
	}

	virtual Bool PrintPage(Media::DrawImage *printPage) //return has more pages
	{
		UOSInt w = printPage->GetWidth();
		UOSInt h = printPage->GetHeight();
		Double hdpi = printPage->GetHDPI();
		Double vdpi = printPage->GetVDPI();
		Media::DrawPen *p = printPage->NewPenARGB(0xff000000, 1, 0, 0);
		printPage->DrawRect(hdpi * 0.5, vdpi * 0.5, Math::UOSInt2Double(w) - hdpi, Math::UOSInt2Double(h) - vdpi, p, 0);
		printPage->DelPen(p);

		if (this->pageId == 0)
		{
			this->pageId = 1;
			this->doc->SetNextPageOrientation(Media::IPrintDocument::PO_LANDSCAPE);
			return true;
		}
		else
		{
			return false;
		}
	}

	virtual Bool EndPrint(Media::IPrintDocument *doc)
	{
		return true;
	}
};

UTF8Char *ByteDisp(UTF8Char *sbuff, UInt64 byteSize)
{
	if (byteSize >= 1073741824)
	{
		return Text::StrConcat(Text::StrDoubleFmt(sbuff, (Double)byteSize / 1073741824.0, "0.##"), (const UTF8Char*)"GB");
	}
	else if (byteSize >= 1048576)
	{
		return Text::StrConcat(Text::StrDoubleFmt(sbuff, (Double)byteSize / 1048576.0, "0.##"), (const UTF8Char*)"MB");
	}
	else if (byteSize >= 1024)
	{
		return Text::StrConcat(Text::StrDoubleFmt(sbuff, (Double)byteSize / 1024.0, "0.##"), (const UTF8Char*)"KB");
	}
	else
	{
		return Text::StrConcat(Text::StrUInt64(sbuff, byteSize), (const UTF8Char*)"B");
	}
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Manage::ExceptionRecorder *exHdlr;
	IO::ConsoleWriter *console;

	IO::FileStream *fs;
	Text::UTF8Writer *writer;
	IO::SystemInfo sysInfo;
	UInt64 memSize;
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[512];
	UTF8Char u8buff[128];
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	UOSInt threadCnt = Sync::Thread::GetThreadCnt();

	MemSetLogFile((const UTF8Char*)"Memory.log");
	NEW_CLASS(exHdlr, Manage::ExceptionRecorder((const UTF8Char*)"SHWInfo.log", Manage::ExceptionRecorder::EA_CLOSE));
	NEW_CLASS(console, IO::ConsoleWriter());

	IO::Path::GetProcessFileName(sbuff);
	IO::Path::ReplaceExt(sbuff, (const UTF8Char*)"txt");
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASS(writer, Text::UTF8Writer(fs));

	console->WriteLine((const UTF8Char*)"Computer Info:");
	writer->WriteLine((const UTF8Char*)"Computer Info:");
	sb.ClearStr();
	sb.Append((const UTF8Char*)"OS: ");
	if (IO::OS::GetDistro(sbuff))
	{
		sb.Append(sbuff);
	}
	else
	{
		sb.Append((const UTF8Char*)"Unknown");
	}
	if (IO::OS::GetVersion(sbuff))
	{
		sb.Append((const UTF8Char*)" ");
		sb.Append(sbuff);
	}
	console->WriteLine(sb.ToString());
	writer->WriteLine(sb.ToString());
	{
		Data::DateTime dt;
		sb.ClearStr();
		sb.Append((const UTF8Char*)"Build Time: ");
		IO::BuildTime::GetBuildTime(&dt);
		dt.ToUTCTime();
		sb.AppendDate(&dt);
		console->WriteLine(sb.ToString());
		writer->WriteLine(sb.ToString());
	}

	sb.ClearStr();
	sb.Append((const UTF8Char*)"Platform: ");
	if (sysInfo.GetPlatformName(sbuff))
	{
		sb.Append(sbuff);
	}
	else
	{
		sb.Append((const UTF8Char*)"-");
	}
	console->WriteLine(sb.ToString());
	writer->WriteLine(sb.ToString());

	sb.ClearStr();
	sb.Append((const UTF8Char*)"Platform SN: ");
	if (sysInfo.GetPlatformSN(sbuff))
	{
		sb.Append(sbuff);
	}
	else
	{
		sb.Append((const UTF8Char*)"-");
	}
	console->WriteLine(sb.ToString());
	writer->WriteLine(sb.ToString());

	{
		Manage::CPUInfoDetail cpuInfo;
		sb.ClearStr();
		sb.Append((const UTF8Char*)"CPU Name: ");
		if (cpuInfo.GetCPUName(u8buff))
		{
			sb.Append(u8buff);
		}
		else
		{
			sb.Append((const UTF8Char*)"-");
		}
		console->WriteLine(sb.ToString());
		writer->WriteLine(sb.ToString());

		const Manage::CPUDB::CPUSpec *cpuSpec = 0;
		sb.ClearStr();
		sb.Append((const UTF8Char*)"CPU Model: ");
		if (cpuInfo.GetCPUModel())
		{
			sb.Append(cpuInfo.GetCPUModel());
			cpuSpec = Manage::CPUDB::GetCPUSpec(cpuInfo.GetCPUModel());
		}
		else
		{
			sb.Append((const UTF8Char*)"-");
		}
		console->WriteLine(sb.ToString());
		writer->WriteLine(sb.ToString());

		sb.ClearStr();
		sb.Append((const UTF8Char*)"CPU Brand Name: ");
		if (cpuSpec)
		{
			sb.Append(Manage::CPUVendor::GetBrandName(cpuSpec->brand));
			sb.Append((const UTF8Char*)" ");
			sb.Append((const UTF8Char*)cpuSpec->name);
		}
		else
		{
			sb.Append((const UTF8Char*)"-");
		}
		console->WriteLine(sb.ToString());
		writer->WriteLine(sb.ToString());

#if defined(CPU_X86_32) || defined(CPU_X86_64)
		sb.ClearStr();
		sb.Append((const UTF8Char*)"CPU FamilyId = ");
		sb.AppendI32(cpuInfo.GetFamilyId());
		sb.Append((const UTF8Char*)", ModelId = ");
		sb.AppendI32(cpuInfo.GetModelId());
		sb.Append((const UTF8Char*)", Stepping = ");
		sb.AppendI32(cpuInfo.GetStepping());
		console->WriteLine(sb.ToString());
		writer->WriteLine(sb.ToString());
#endif

		sb.ClearStr();
		sb.Append((const UTF8Char*)"CPU Thread Count: ");
		sb.AppendUOSInt(threadCnt);
		console->WriteLine(sb.ToString());
		writer->WriteLine(sb.ToString());

		i = 0;
		while (true)
		{
			Double temp;

			if (!cpuInfo.GetCPUTemp(i, &temp))
				break;

			sb.ClearStr();
			sb.Append((const UTF8Char*)"CPU Temp ");
			sb.AppendUOSInt(i);
			sb.Append((const UTF8Char*)": ");
			Text::SBAppendF64(&sb, temp);
			console->WriteLine(sb.ToString());
			writer->WriteLine(sb.ToString());

			i++;
		}
	}

	sb.ClearStr();
	sb.Append((const UTF8Char*)"Total Memory Size: ");
	memSize = sysInfo.GetTotalMemSize();
	ByteDisp(sbuff, memSize);
	sb.Append(sbuff);
	console->WriteLine(sb.ToString());
	writer->WriteLine(sb.ToString());

	sb.ClearStr();
	sb.Append((const UTF8Char*)"Total Usable Memory Size: ");
	memSize = sysInfo.GetTotalUsableMemSize();
	ByteDisp(sbuff, memSize);
	sb.Append(sbuff);
	console->WriteLine(sb.ToString());
	writer->WriteLine(sb.ToString());

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
			sb.Append((const UTF8Char*)"RAM: ");
			if (ram->deviceLocator)
			{
				sb.Append(ram->deviceLocator);
			}
			sb.Append((const UTF8Char*)"\t");
			if (ram->manufacturer)
			{
				sb.Append(ram->manufacturer);
			}
			sb.Append((const UTF8Char*)"\t");
			if (ram->partNo)
			{
				sb.Append(ram->partNo);
			}
			sb.Append((const UTF8Char*)"\t");
			if (ram->sn)
			{
				sb.Append(ram->sn);
			}
			sb.Append((const UTF8Char*)"\t");
			sb.AppendU64(ram->defSpdMHz);
			sb.Append((const UTF8Char*)"\t");
			sb.AppendU64(ram->confSpdMHz);
			sb.Append((const UTF8Char*)"\t");
			sb.AppendU32(ram->dataWidth);
			sb.Append((const UTF8Char*)"\t");
			sb.AppendU32(ram->totalWidth);
			sb.Append((const UTF8Char*)"\t");
			sb.AppendU64(ram->memorySize);
			console->WriteLine(sb.ToString());
			writer->WriteLine(sb.ToString());
			i++;
		}
		sysInfo.FreeRAMInfo(&ramList);
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
				if (Media::EDID::Parse(edid, &edidInfo))
				{
					sb.ClearStr();
					sb.Append((const UTF8Char*)"Monitor ");
					sb.AppendUOSInt(i);
					sb.Append((const UTF8Char*)" - ");
					sb.Append(edidInfo.monitorName);
					sb.Append((const UTF8Char*)" (");
					sb.Append(edidInfo.vendorName);
					sb.Append((const UTF8Char*)" ");
					sb.AppendHex16(edidInfo.productCode);
					sb.Append((const UTF8Char*)")");
					console->WriteLine(sb.ToString());
					writer->WriteLine(sb.ToString());

					sb.ClearStr();
					sb.Append((const UTF8Char*)"Monitor Manufacture: Year ");
					sb.AppendI32(edidInfo.yearOfManu);
					sb.Append((const UTF8Char*)" Week ");
					sb.AppendU32(edidInfo.weekOfManu);
					console->WriteLine(sb.ToString());
					writer->WriteLine(sb.ToString());

					sb.ClearStr();
					sb.Append((const UTF8Char*)"Monitor Size: ");
					Text::SBAppendF64(&sb, Math::Unit::Distance::Convert(Math::Unit::Distance::DU_CENTIMETER, Math::Unit::Distance::DU_INCH, Math::Sqrt(edidInfo.dispPhysicalW * edidInfo.dispPhysicalW + edidInfo.dispPhysicalH * edidInfo.dispPhysicalH)));
					sb.Append((const UTF8Char*)"\" (");
					sb.AppendU32(edidInfo.dispPhysicalW);
					sb.Append((const UTF8Char*)" x ");
					sb.AppendU32(edidInfo.dispPhysicalH);
					sb.Append((const UTF8Char*)"cm)");
					console->WriteLine(sb.ToString());
					writer->WriteLine(sb.ToString());

					sb.ClearStr();
					sb.Append((const UTF8Char*)"Monitor White: x = ");
					Text::SBAppendF64(&sb, edidInfo.wx);
					sb.Append((const UTF8Char*)", y = ");
					Text::SBAppendF64(&sb, edidInfo.wy);
					console->WriteLine(sb.ToString());
					writer->WriteLine(sb.ToString());

					sb.ClearStr();
					sb.Append((const UTF8Char*)"Monitor Red x = ");
					Text::SBAppendF64(&sb, edidInfo.rx);
					sb.Append((const UTF8Char*)", y = ");
					Text::SBAppendF64(&sb, edidInfo.ry);
					console->WriteLine(sb.ToString());
					writer->WriteLine(sb.ToString());

					sb.ClearStr();
					sb.Append((const UTF8Char*)"Monitor Green: x = ");
					Text::SBAppendF64(&sb, edidInfo.gx);
					sb.Append((const UTF8Char*)", y = ");
					Text::SBAppendF64(&sb, edidInfo.gy);
					console->WriteLine(sb.ToString());
					writer->WriteLine(sb.ToString());

					sb.ClearStr();
					sb.Append((const UTF8Char*)"Monitor Blue: x = ");
					Text::SBAppendF64(&sb, edidInfo.bx);
					sb.Append((const UTF8Char*)", y = ");
					Text::SBAppendF64(&sb, edidInfo.by);
					console->WriteLine(sb.ToString());
					writer->WriteLine(sb.ToString());
				}
			}
			DEL_CLASS(reader);
			i++;
		}
	}

//-------------------------------------------------------------------------------
	Win32::SMBIOS *smbios = Win32::SMBIOSUtil::GetSMBIOS();
	if (smbios)
	{
		console->WriteLine();
		writer->WriteLine();
		console->WriteLine((const UTF8Char*)"SMBIOS:");
		writer->WriteLine((const UTF8Char*)"SMBIOS:");
		sb.ClearStr();
		smbios->ToString(&sb);
		console->WriteLine(sb.ToString());
		writer->WriteLine(sb.ToString());

		DEL_CLASS(smbios);
	}
//-------------------------------------------------------------------------------
	IO::PowerInfo::PowerStatus pstatus;
	if (IO::PowerInfo::GetPowerStatus(&pstatus))
	{
		console->WriteLine();
		writer->WriteLine();
		console->WriteLine((const UTF8Char*)"Power Info:");
		writer->WriteLine((const UTF8Char*)"Power Info:");

		sb.ClearStr();
		sb.Append((const UTF8Char*)"AC Status = ");
		if (pstatus.acStatus == IO::PowerInfo::ACS_ON)
		{
			sb.Append((const UTF8Char*)"On");
		}
		else if (pstatus.acStatus == IO::PowerInfo::ACS_OFF)
		{
			sb.Append((const UTF8Char*)"Off");
		}
		else
		{
			sb.Append((const UTF8Char*)"Unknown");
		}
		console->WriteLine(sb.ToString());
		writer->WriteLine(sb.ToString());

		sb.ClearStr();
		sb.Append((const UTF8Char*)"Has Battery = ");
		sb.Append(pstatus.hasBattery?(const UTF8Char*)"Yes":(const UTF8Char*)"No");
		console->WriteLine(sb.ToString());
		writer->WriteLine(sb.ToString());

		sb.ClearStr();
		sb.Append((const UTF8Char*)"Battery Charging = ");
		sb.Append(pstatus.batteryCharging?(const UTF8Char*)"Yes":(const UTF8Char*)"No");
		console->WriteLine(sb.ToString());
		writer->WriteLine(sb.ToString());

		sb.ClearStr();
		sb.Append((const UTF8Char*)"Battery Percent = ");
		sb.AppendU32(pstatus.batteryPercent);
		console->WriteLine(sb.ToString());
		writer->WriteLine(sb.ToString());

		sb.ClearStr();
		sb.Append((const UTF8Char*)"Time Left (sec.) = ");
		sb.AppendU32(pstatus.timeLeftSec);
		console->WriteLine(sb.ToString());
		writer->WriteLine(sb.ToString());

		sb.ClearStr();
		sb.Append((const UTF8Char*)"Battery Voltage = ");
		Text::SBAppendF64(&sb, pstatus.batteryVoltage);
		console->WriteLine(sb.ToString());
		writer->WriteLine(sb.ToString());

		sb.ClearStr();
		sb.Append((const UTF8Char*)"Battery Charge Current = ");
		Text::SBAppendF64(&sb, pstatus.batteryChargeCurrent);
		console->WriteLine(sb.ToString());
		writer->WriteLine(sb.ToString());

		sb.ClearStr();
		sb.Append((const UTF8Char*)"Battery Temperature = ");
		Text::SBAppendF64(&sb, pstatus.batteryTemp);
		console->WriteLine(sb.ToString());
		writer->WriteLine(sb.ToString());
	}

//-------------------------------------------------------------------------------
	{
		console->WriteLine();
		writer->WriteLine();
		console->WriteLine((const UTF8Char*)"Printer Info:");
		writer->WriteLine((const UTF8Char*)"Printer Info:");
		Data::ArrayList<const UTF8Char *> printerList;
		i = 0;
		j = Media::Printer::GetPrinterCount();
		while (i < j)
		{
			if (Media::Printer::GetPrinterName(sbuff, i))
			{
				sb.ClearStr();
				sb.Append((const UTF8Char*)"Printer ");
				sb.AppendUOSInt(i);
				sb.Append((const UTF8Char*)" = ");
				sb.Append(sbuff);
				console->WriteLine(sb.ToString());
				writer->WriteLine(sb.ToString());

				if (Text::StrEquals(sbuff, (const UTF8Char*)"Bullzip PDF Printer") || Text::StrEquals(sbuff, (const UTF8Char*)"Adobe PDF") || Text::StrEquals(sbuff, (const UTF8Char*)"PDF"))
				{
					printerList.Add(Text::StrCopyNew(sbuff));
				}
			}

			i++;
		}
		i = printerList.GetCount();
		if (i > 0)
		{
			const UTF8Char *csptr;
			Media::Printer *printer;
			Media::DrawEngine *deng = Core::DefaultDrawEngine::CreateDrawEngine();
			while (i-- > 0)
			{
				csptr = printerList.GetItem(i);

				if (deng)
				{
					sb.ClearStr();
					sb.Append((const UTF8Char*)"Test Printing with ");
					sb.Append(csptr);
					console->WriteLine(sb.ToString());
					writer->WriteLine(sb.ToString());

					NEW_CLASS(printer, Media::Printer(csptr));
					if (printer->IsError())
					{
						console->WriteLine((const UTF8Char*)"Error in opening printer");
						writer->WriteLine((const UTF8Char*)"Error in opening printer");
					}
					else
					{
						PrintTest *test;
						NEW_CLASS(test, PrintTest());
						Media::IPrintDocument *doc = printer->StartPrint(test, deng);
						if (doc)
						{
							printer->EndPrint(doc);
						}
						else
						{
							console->WriteLine((const UTF8Char*)"Error in start printing");
							writer->WriteLine((const UTF8Char*)"Error in start printing");
						}
						DEL_CLASS(test);
					}
					DEL_CLASS(printer);
				}

				Text::StrDelNew(csptr);
			}
			if (deng)
			{
				DEL_CLASS(deng);
			}
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
		console->WriteLine((const UTF8Char*)"Video Capture Info:");
		writer->WriteLine((const UTF8Char*)"Video Capture Info:");
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
				sb.Append((const UTF8Char*)"Device ");
				sb.AppendUOSInt(i);
				sb.Append((const UTF8Char*)": ");
				sb.Append(videoMgr->GetDevTypeName(dev->devType));
				sb.Append((const UTF8Char*)", ");
				sb.Append(dev->devName);
				console->WriteLine(sb.ToString());
				writer->WriteLine(sb.ToString());

				capture = videoMgr->CreateDevice(dev->devType, dev->devId);
				if (capture)
				{
					sb.ClearStr();
					capture->GetInfo(&sb);
					console->WriteLine(sb.ToString());
					writer->WriteLine(sb.ToString());
					console->WriteLine((const UTF8Char*)"Format List:");
					writer->WriteLine((const UTF8Char*)"Format List:");
					k = 0;
					l = capture->GetSupportedFormats(fmts, 512);
					while (k < l)
					{
						sb.ClearStr();
						sb.Append((const UTF8Char*)"Format ");
						if (fmts[k].info.fourcc == 0)
						{
							sb.Append((const UTF8Char*)"RGB");
						}
						else
						{
							*(UInt32*)fmtName = fmts[k].info.fourcc;
							sb.Append((const UTF8Char*)fmtName);
						}
						sb.Append((const UTF8Char*)", bpp = ");
						sb.AppendU32(fmts[k].info.storeBPP);
						sb.Append((const UTF8Char*)", size = ");
						sb.AppendUOSInt(fmts[k].info.dispWidth);
						sb.Append((const UTF8Char*)" x ");
						sb.AppendUOSInt(fmts[k].info.dispHeight);
						sb.Append((const UTF8Char*)", rate = ");
						Text::SBAppendF64(&sb, fmts[k].frameRateNorm / (Double)fmts[k].frameRateDenorm);
						console->WriteLine(sb.ToString());
						writer->WriteLine(sb.ToString());
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
		Data::DateTime *dt;
		Net::ConnectionInfo *connInfo;
		Data::ArrayList<Net::ConnectionInfo*> connInfoList;
		Net::SocketFactory *sockf;
		NEW_CLASS(sockf, Net::OSSocketFactory(true));
		sockf->GetConnInfoList(&connInfoList);
		console->WriteLine();
		writer->WriteLine();
		console->WriteLine((const UTF8Char*)"Network Info:");
		writer->WriteLine((const UTF8Char*)"Network Info:");
		i = 0;
		j = connInfoList.GetCount();
		while (i < j)
		{
			connInfo = connInfoList.GetItem(i);
			sb.ClearStr();
			sb.Append((const UTF8Char*)"Connection ");
			sb.AppendUOSInt(i);
			sb.Append((const UTF8Char*)":");
			console->WriteLine(sb.ToString());
			writer->WriteLine(sb.ToString());

			if (connInfo->GetName(sbuff))
			{
				sb.ClearStr();
				sb.Append((const UTF8Char*)"Name: ");
				sb.Append(sbuff);
				console->WriteLine(sb.ToString());
				writer->WriteLine(sb.ToString());
			}
			if (connInfo->GetDescription(sbuff))
			{
				sb.ClearStr();
				sb.Append((const UTF8Char*)"Description: ");
				sb.Append(sbuff);
				console->WriteLine(sb.ToString());
				writer->WriteLine(sb.ToString());
			}
			if (connInfo->GetDNSSuffix(sbuff))
			{
				sb.ClearStr();
				sb.Append((const UTF8Char*)"DNS Suffix: ");
				sb.Append(sbuff);
				console->WriteLine(sb.ToString());
				writer->WriteLine(sb.ToString());
			}
			k = 0;
			while (k < 32)
			{
				if ((ipv4 = connInfo->GetIPAddress(k)) == 0)
				{
					break;
				}
				Net::SocketUtil::GetIPv4Name(sbuff, ipv4);
				sb.ClearStr();
				sb.Append((const UTF8Char*)"IPv4 Address: ");
				sb.Append(sbuff);
				console->WriteLine(sb.ToString());
				writer->WriteLine(sb.ToString());
				k++;
			}
			k = 0;
			while (k < 32)
			{
				if ((ipv4 = connInfo->GetDNSAddress(k)) == 0)
				{
					break;
				}
				Net::SocketUtil::GetIPv4Name(sbuff, ipv4);
				sb.ClearStr();
				sb.Append((const UTF8Char*)"DNS Address: ");
				sb.Append(sbuff);
				console->WriteLine(sb.ToString());
				writer->WriteLine(sb.ToString());
				k++;
			}
			if ((ipv4 = connInfo->GetDefaultGW()) != 0)
			{
				Net::SocketUtil::GetIPv4Name(sbuff, ipv4);
				sb.ClearStr();
				sb.Append((const UTF8Char*)"Default Gateway: ");
				sb.Append(sbuff);
				console->WriteLine(sb.ToString());
				writer->WriteLine(sb.ToString());
			}
			if ((k = connInfo->GetPhysicalAddress(mac, 16)) > 0)
			{
				Text::StrHexBytes(sbuff, mac, k, '-');
				sb.ClearStr();
				sb.Append((const UTF8Char*)"Physical Address: ");
				sb.Append(sbuff);
				console->WriteLine(sb.ToString());
				writer->WriteLine(sb.ToString());
			}
			sb.ClearStr();
			sb.Append((const UTF8Char*)"MTU: ");
			sb.AppendU32(connInfo->GetMTU());
			console->WriteLine(sb.ToString());
			writer->WriteLine(sb.ToString());

			sb.ClearStr();
			sb.Append((const UTF8Char*)"DHCP Enabled: ");
			sb.Append(connInfo->IsDhcpEnabled()?(const UTF8Char*)"True":(const UTF8Char*)"False");
			console->WriteLine(sb.ToString());
			writer->WriteLine(sb.ToString());

			if (connInfo->IsDhcpEnabled())
			{
				Net::SocketUtil::GetIPv4Name(sbuff, connInfo->GetDhcpServer());
				sb.ClearStr();
				sb.Append((const UTF8Char*)"DHCP Server: ");
				sb.Append(sbuff);
				console->WriteLine(sb.ToString());
				writer->WriteLine(sb.ToString());

				dt = connInfo->GetDhcpLeaseTime();
				if (dt)
				{
					dt->ToString(sbuff);
					sb.ClearStr();
					sb.Append((const UTF8Char*)"DHCP Lease Time: ");
					sb.Append(sbuff);
					console->WriteLine(sb.ToString());
					writer->WriteLine(sb.ToString());
				}

				dt = connInfo->GetDhcpLeaseExpire();
				if (dt)
				{
					dt->ToString(sbuff);
					sb.ClearStr();
					sb.Append((const UTF8Char*)"DHCP Lease Time: ");
					sb.Append(sbuff);
					console->WriteLine(sb.ToString());
					writer->WriteLine(sb.ToString());
				}
			}

			sb.ClearStr();
			sb.Append((const UTF8Char*)"Connection Type: ");
			sb.Append(Net::ConnectionInfo::GetConnectionTypeName(connInfo->GetConnectionType()));
			console->WriteLine(sb.ToString());
			writer->WriteLine(sb.ToString());

			sb.ClearStr();
			sb.Append((const UTF8Char*)"Connection Status: ");
			sb.Append(Net::ConnectionInfo::GetConnectionStatusName(connInfo->GetConnectionStatus()));
			console->WriteLine(sb.ToString());
			writer->WriteLine(sb.ToString());
			console->WriteLine();
			writer->WriteLine();

			DEL_CLASS(connInfo);
			i++;
		}
		DEL_CLASS(sockf);
	}
//-------------------------------------------------------------------------------
	{
		Data::ArrayList<IO::USBInfo*> usbList;
		IO::USBInfo *usb;
		const IO::DeviceDB::USBDeviceInfo *dev;
		IO::USBInfo::GetUSBList(&usbList);
		console->WriteLine();
		writer->WriteLine();
		console->WriteLine((const UTF8Char*)"USB Info:");
		writer->WriteLine((const UTF8Char*)"USB Info:");
		i = 0;
		j = usbList.GetCount();
		while (i < j)
		{
			usb = usbList.GetItem(i);
			dev = IO::DeviceDB::GetUSBInfo(usb->GetVendorId(), usb->GetProductId(), usb->GetRevision());
			sb.ClearStr();
			sb.Append((const UTF8Char*)"USB ");
			sb.AppendHex16(usb->GetVendorId());
			sb.Append((const UTF8Char*)":");
			sb.AppendHex16(usb->GetProductId());
			sb.Append((const UTF8Char*)" ");
			sb.AppendHex16(usb->GetRevision());
			sb.Append((const UTF8Char*)" Dev:");
			sb.Append(usb->GetDispName());
			console->WriteLine(sb.ToString());
			writer->WriteLine(sb.ToString());

			sb.ClearStr();
			sb.Append((const UTF8Char*)"USB ");
			sb.AppendHex16(usb->GetVendorId());
			sb.Append((const UTF8Char*)":");
			sb.AppendHex16(usb->GetProductId());
			sb.Append((const UTF8Char*)" ");
			sb.AppendHex16(usb->GetRevision());
			sb.Append((const UTF8Char*)" DB: ");
			if (dev)
			{
				if (dev->dispName)
				{
					sb.Append((const UTF8Char*)dev->dispName);
				}
				else
				{
					sb.Append((const UTF8Char*)"?");
				}

				if (dev->productName)
				{
					sb.Append((const UTF8Char*)" (");
					sb.Append((const UTF8Char*)dev->productName);
					sb.Append((const UTF8Char*)")");
				}
			}
			else
			{
					sb.Append((const UTF8Char*)"?");
			}
			sb.Append((const UTF8Char*)" Vendor = ");
			sb.Append(IO::DeviceDB::GetUSBVendorName(usb->GetVendorId()));
			console->WriteLine(sb.ToString());
			writer->WriteLine(sb.ToString());

			DEL_CLASS(usb);
			i++;
		}
	}
//-------------------------------------------------------------------------------
	{
		Data::ArrayList<IO::PCIInfo*> pciList;
		IO::PCIInfo *pci;
		const IO::DeviceDB::PCIDeviceInfo *dev;
		IO::PCIInfo::GetPCIList(&pciList);
		console->WriteLine();
		writer->WriteLine();
		console->WriteLine((const UTF8Char*)"PCI Info:");
		writer->WriteLine((const UTF8Char*)"PCI Info:");
		i = 0;
		j = pciList.GetCount();
		while (i < j)
		{
			pci = pciList.GetItem(i);
			dev = IO::DeviceDB::GetPCIInfo(pci->GetVendorId(), pci->GetProductId());
			sb.ClearStr();
			sb.Append((const UTF8Char*)"PCI ");
			sb.AppendHex16(pci->GetVendorId());
			sb.Append((const UTF8Char*)":");
			sb.AppendHex16(pci->GetProductId());
			sb.Append((const UTF8Char*)" Dev:");
			sb.Append(pci->GetDispName());
			console->WriteLine(sb.ToString());
			writer->WriteLine(sb.ToString());

			sb.ClearStr();
			sb.Append((const UTF8Char*)"PCI ");
			sb.AppendHex16(pci->GetVendorId());
			sb.Append((const UTF8Char*)":");
			sb.AppendHex16(pci->GetProductId());
			sb.Append((const UTF8Char*)" DB: ");
			if (dev)
			{
				if (dev->dispName)
				{
					sb.Append((const UTF8Char*)dev->dispName);
				}
				else
				{
					sb.Append((const UTF8Char*)"?");
				}

				if (dev->productName)
				{
					sb.Append((const UTF8Char*)" (");
					sb.Append((const UTF8Char*)dev->productName);
					sb.Append((const UTF8Char*)")");
				}
			}
			else
			{
				sb.Append((const UTF8Char*)"?");
			}
			sb.Append((const UTF8Char*)" Vendor = ");
			sb.Append(IO::DeviceDB::GetPCIVendorName(pci->GetVendorId()));
			console->WriteLine(sb.ToString());
			writer->WriteLine(sb.ToString());

			DEL_CLASS(pci);
			i++;
		}
	}
//-------------------------------------------------------------------------------
	{
		Data::ArrayList<IO::SDCardInfo*> sdList;
		IO::SDCardInfo *sdcard;
		IO::SDCardMgr::GetCardList(&sdList);
		console->WriteLine();
		writer->WriteLine();
		console->WriteLine((const UTF8Char*)"SD Card Info:");
		writer->WriteLine((const UTF8Char*)"SD Card Info:");
		i = 0;
		j = sdList.GetCount();
		while (i < j)
		{
			UTF8Char oemId[6];

			sdcard = sdList.GetItem(i);
			sb.ClearStr();
			sb.Append((const UTF8Char*)"SD Card ");
			sb.AppendUOSInt(i);
			sb.Append((const UTF8Char*)" - ");
			sb.Append(sdcard->GetName());
			console->WriteLine(sb.ToString());
			writer->WriteLine(sb.ToString());

			sb.ClearStr();
			sb.Append((const UTF8Char*)"Manufacturer ID (MID): ");
			sb.AppendI32(sdcard->GetManufacturerID());
			sb.Append((const UTF8Char*)" (");
			sb.Append(IO::SDCardInfo::GetManufacturerName(sdcard->GetManufacturerID()));
			sb.Append((const UTF8Char*)")");
			console->WriteLine(sb.ToString());
			writer->WriteLine(sb.ToString());

			sb.ClearStr();
			sb.Append((const UTF8Char*)"OEM ID (OID): ");
			WriteMInt16(oemId, sdcard->GetOEMID());
			if (oemId[0] >= 'A' && oemId[0] <= 'z' && oemId[1] >= 'A' && oemId[1] <= 'z')
			{
				oemId[2] = 0;
				sb.Append(oemId);
			}
			else
			{
				sb.Append((const UTF8Char*)"0x");
				sb.AppendHex16(sdcard->GetOEMID());
			}
			console->WriteLine(sb.ToString());
			writer->WriteLine(sb.ToString());

			sb.ClearStr();
			sb.Append((const UTF8Char*)"Product Name (PMN): ");
			sdcard->GetProductName((Char*)oemId);
			sb.Append(oemId);
			console->WriteLine(sb.ToString());
			writer->WriteLine(sb.ToString());

			sb.ClearStr();
			sb.Append((const UTF8Char*)"Product Revision (PRV): ");
			oemId[0] = sdcard->GetProductRevision();
			sb.AppendI32(oemId[0] >> 4);
			sb.AppendChar('.', 1);
			sb.AppendI32(oemId[0] & 15);
			console->WriteLine(sb.ToString());
			writer->WriteLine(sb.ToString());

			sb.ClearStr();
			sb.Append((const UTF8Char*)"Product Serial Number (PSN): 0x");
			sb.AppendHex32(sdcard->GetSerialNo());
			console->WriteLine(sb.ToString());
			writer->WriteLine(sb.ToString());

			sb.ClearStr();
			sb.Append((const UTF8Char*)"Manufacturing Date (MDT): ");
			sb.AppendU32(sdcard->GetManufacturingYear());
			sb.AppendChar('/', 1);
			sb.AppendU32(sdcard->GetManufacturingMonth());
			console->WriteLine(sb.ToString());
			writer->WriteLine(sb.ToString());

			sb.ClearStr();
			sb.Append((const UTF8Char*)"Card Capacity: ");
			sb.AppendI64(sdcard->GetCardCapacity());
			sb.Append((const UTF8Char*)" Bytes");
			console->WriteLine(sb.ToString());
			writer->WriteLine(sb.ToString());

			sb.ClearStr();
			sb.Append((const UTF8Char*)"Card Speed: ");
			sb.AppendI64(sdcard->GetMaxTranRate());
			sb.Append((const UTF8Char*)" bit/s");
			console->WriteLine(sb.ToString());
			writer->WriteLine(sb.ToString());

			console->WriteLine();
			writer->WriteLine();
			DEL_CLASS(sdcard);
			i++;
		}
	}
//-------------------------------------------------------------------------------
	{
		IO::SensorManager sensorMgr;
		const UTF8Char *csptr;
		IO::Sensor *sensor;
		console->WriteLine();
		writer->WriteLine();
		console->WriteLine((const UTF8Char*)"Sensors:");
		writer->WriteLine((const UTF8Char*)"Sensors:");
		sb.ClearStr();
		sb.Append((const UTF8Char*)"Count = ");
		sb.AppendUOSInt(j = sensorMgr.GetSensorCnt());
		console->WriteLine(sb.ToString());
		writer->WriteLine(sb.ToString());
		i = 0;
		while (i < j)
		{
			sensor = sensorMgr.CreateSensor(i);
			if (sensor)
			{
				sb.ClearStr();
				sb.Append((const UTF8Char*)"Sensor ");
				sb.AppendUOSInt(i);
				sb.Append((const UTF8Char*)", Name = ");
				csptr = sensor->GetName();
				if (csptr)
				{
					sb.Append(csptr);
				}
				sb.Append((const UTF8Char*)", Vendor = ");
				csptr = sensor->GetVendor();
				if (csptr)
				{
					sb.Append(csptr);
				}
				sb.Append((const UTF8Char*)", Type = ");
				sb.Append(IO::Sensor::GetSensorTypeName(sensor->GetSensorType()));
				console->WriteLine(sb.ToString());
				writer->WriteLine(sb.ToString());

				switch (sensor->GetSensorType())
				{
				case IO::Sensor::SensorType::Accelerometer:
					if (sensor->EnableSensor())
					{
						Double x;
						Double y;
						Double z;
						if (sensor->GetSensorAccelerator()->ReadAcceleration(&x, &y, &z))
						{
							Math::Unit::Acceleration::AccelerationUnit aunit = sensor->GetSensorAccelerator()->GetAccelerationUnit();

							sb.ClearStr();
							sb.Append((const UTF8Char*)"Accelerometer x = ");
							Text::SBAppendF64(&sb, x);
							sb.Append(Math::Unit::Acceleration::GetUnitShortName(aunit));
							sb.Append((const UTF8Char*)", y = ");
							Text::SBAppendF64(&sb, y);
							sb.Append(Math::Unit::Acceleration::GetUnitShortName(aunit));
							sb.Append((const UTF8Char*)", z = ");
							Text::SBAppendF64(&sb, z);
							sb.Append(Math::Unit::Acceleration::GetUnitShortName(aunit));
							console->WriteLine(sb.ToString());
							writer->WriteLine(sb.ToString());
						}
						sensor->DisableSensor();
					}
					break;
				case IO::Sensor::SensorType::Pressure:
					if (sensor->EnableSensor())
					{
						Double pressure;
						if (sensor->GetSensorPressure()->ReadPressure(&pressure))
						{
							sb.ClearStr();
							sb.Append((const UTF8Char*)"Pressure = ");
							Text::SBAppendF64(&sb, pressure);
							sb.Append(Math::Unit::Pressure::GetUnitShortName(sensor->GetSensorPressure()->GetPressureUnit()));
							console->WriteLine(sb.ToString());
							writer->WriteLine(sb.ToString());
						}
						sensor->DisableSensor();
					}
					break;
				case IO::Sensor::SensorType::Magnetometer:
					if (sensor->EnableSensor())
					{
						Double x;
						Double y;
						Double z;
						if (sensor->GetSensorMagnetometer()->ReadMagneticField(&x, &y, &z))
						{
							Math::Unit::MagneticField::MagneticFieldUnit mfunit = sensor->GetSensorMagnetometer()->GetMagneticFieldUnit();

							sb.ClearStr();
							sb.Append((const UTF8Char*)"Magnetometer x = ");
							Text::SBAppendF64(&sb, x);
							sb.Append(Math::Unit::MagneticField::GetUnitShortName(mfunit));
							sb.Append((const UTF8Char*)", y = ");
							Text::SBAppendF64(&sb, y);
							sb.Append(Math::Unit::MagneticField::GetUnitShortName(mfunit));
							sb.Append((const UTF8Char*)", z = ");
							Text::SBAppendF64(&sb, z);
							sb.Append(Math::Unit::MagneticField::GetUnitShortName(mfunit));
							console->WriteLine(sb.ToString());
							writer->WriteLine(sb.ToString());
						}
						sensor->DisableSensor();
					}
					break;
				case IO::Sensor::SensorType::Orientation:
				case IO::Sensor::SensorType::Unknown:
				default:
					break;
				}
				DEL_CLASS(sensor);
			}
			i++;
		}
	}
//-------------------------------------------------------------------------------
	DEL_CLASS(writer);

	DEL_CLASS(fs);

	DEL_CLASS(console);
	DEL_CLASS(exHdlr);
	return 0;
}
