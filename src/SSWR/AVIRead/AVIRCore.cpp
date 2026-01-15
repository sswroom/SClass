#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/EXEFile.h"
#include "IO/FileCheck.h"
#include "IO/FileStream.h"
#include "IO/SectorData.h"
#include "IO/MemoryStream.h"
#include "IO/ModemController.h"
#include "IO/Path.h"
#include "IO/Registry.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math_C.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "Net/WebBrowser.h"
#include "Parser/FullParserList.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/AVIRGISForm.h"
#include "SSWR/AVIRead/AVIRGSMModemForm.h"
#include "SSWR/AVIRead/AVIRHexViewerForm.h"
#include "SSWR/AVIRead/AVIRSelStreamForm.h"
#include "SSWR/AVIRead/AVIRCesiumTileForm.h"
#include "Text/MyStringW.h"
#include "UI/GUIForm.h"

void __stdcall SSWR::AVIRead::AVIRCore::FormClosed(AnyType userObj, NN<UI::GUIForm> frm)
{
	NN<SSWR::AVIRead::AVIRCore> me = userObj.GetNN<SSWR::AVIRead::AVIRCore>();
	me->frms.RemoveAt(me->frms.IndexOf(frm));
	if (me->gisForm.OrNull() == frm.Ptr())
	{
		me->gisForm = nullptr;
	}
}

void SSWR::AVIRead::AVIRCore::InitForm(NN<UI::GUIForm> frm)
{
	frm->HandleFormClosed(FormClosed, this);
	this->frms.Add(frm);
}

SSWR::AVIRead::AVIRCore::AVIRCore(NN<UI::GUICore> ui) : vioPinMgr(4)
{
	WChar wbuff[512];
	WChar wbuff2[32];
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	this->ui = ui;
	this->forwardedUI = this->ui->IsForwarded();
	this->currCodePage = 0;
	this->eng = ui->CreateDrawEngine();
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("CacheDir"));
	NEW_CLASSNN(this->parsers, Parser::FullParserList());
	NEW_CLASSNN(this->sockf, Net::OSSocketFactory(true));
	NEW_CLASSNN(this->clif, Net::TCPClientFactory(this->sockf));
	this->ssl = Net::SSLEngineFactory::Create(this->clif, true);
	NEW_CLASSNN(this->browser, Net::WebBrowser(clif, this->ssl, CSTRP(sbuff, sptr)));
	NN<IO::GPIOControl> gpioCtrl;
	NEW_CLASSNN(gpioCtrl, IO::GPIOControl());
	if (gpioCtrl->IsError())
	{
		gpioCtrl.Delete();
		this->gpioCtrl = nullptr;
	}
	else
	{
		this->gpioCtrl = gpioCtrl;
	}
	NN<IO::SiLabDriver> siLabDriver;
	NEW_CLASSNN(siLabDriver, IO::SiLabDriver());
	if (siLabDriver->IsError())
	{
		siLabDriver.Delete();
		this->siLabDriver = nullptr;
	}
	else
	{
		this->siLabDriver = siLabDriver;
	}
	this->parsers->SetEncFactory(this->encFact);
	this->parsers->SetMapManager(this->mapMgr);
	this->parsers->SetWebBrowser(this->browser);
	this->parsers->SetTCPClientFactory(this->clif);
	this->parsers->SetSSLEngine(this->ssl);
	this->parsers->SetLogTool(this->log);
	this->batchLyrs = nullptr;
	this->batchCesiumTiles = nullptr;
	this->batchLoad = false;
	this->gisForm = nullptr;
	this->ui->SetMonitorMgr(&this->monMgr);

	NN<IO::Registry> reg;
	if (IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, L"SSWR", L"AVIRead").SetTo(reg))
	{
		OSInt i = 0;
		while (true)
		{
			Text::StrOSInt(Text::StrConcatC(wbuff2, L"AudioDevice", 11), i);
			if (reg->GetValueStr(wbuff2, wbuff).NotNull())
			{
				NN<Text::String> devName = Text::String::NewNotNull(wbuff);
				this->audDevice.AddDevice(devName->ToCString());
				this->audDevList.Add(devName);
			}
			else
			{
				break;
			}
			i++;
		}
		IO::Registry::CloseRegistry(reg);
	}
}

SSWR::AVIRead::AVIRCore::~AVIRCore()
{
	UOSInt i;
	this->CloseAllForm();
	this->parsers.Delete();
	this->browser.Delete();
	this->ssl.Delete();
	this->clif.Delete();
	this->sockf.Delete();
	this->eng.Delete();
	this->ui->SetMonitorMgr(nullptr);
	this->gpioCtrl.Delete();
	this->siLabDriver.Delete();
	i = this->audDevList.GetCount();
	while (i-- > 0)
	{
		OPTSTR_DEL(this->audDevList.GetItem(i));
	}
}

void SSWR::AVIRead::AVIRCore::OpenGSMModem(Optional<IO::Stream> modemPort)
{
	NN<SSWR::AVIRead::AVIRGSMModemForm> frm;
	NEW_CLASSNN(frm, SSWR::AVIRead::AVIRGSMModemForm(nullptr, ui, *this, modemPort));
	InitForm(frm);
	frm->Show();
}

Optional<IO::Stream> SSWR::AVIRead::AVIRCore::OpenStream(OptOut<IO::StreamType> st, Optional<UI::GUIForm> ownerFrm, Int32 defBaudRate, Bool allowReadOnly)
{
	Optional<IO::Stream> retStm = nullptr;
	SSWR::AVIRead::AVIRSelStreamForm frm(nullptr, this->ui, *this, allowReadOnly, this->ssl, this->GetLog());
	if (defBaudRate != 0)
	{
		frm.SetInitBaudRate(defBaudRate);
	}
	if (frm.ShowDialog(ownerFrm) == UI::GUIForm::DR_OK)
	{
		retStm = frm.GetStream();
		st.Set(frm.GetStreamType());
	}
	return retStm;
}

void SSWR::AVIRead::AVIRCore::OpenHex(NN<IO::StreamData> fd, Optional<IO::FileAnalyse::FileAnalyser> fileAnalyse)
{
	NN<SSWR::AVIRead::AVIRHexViewerForm> frm;
	NEW_CLASSNN(frm, SSWR::AVIRead::AVIRHexViewerForm(nullptr, ui, *this));
	InitForm(frm);
	frm->SetData(fd, fileAnalyse);
	frm->Show();
}

void SSWR::AVIRead::AVIRCore::BeginLoad()
{
	this->batchLoad = true;
}

void SSWR::AVIRead::AVIRCore::EndLoad()
{
	this->batchLoad = false;
	NN<Data::ArrayListNN<Map::MapDrawLayer>> batchLyrs;
	if (this->batchLyrs.SetTo(batchLyrs))
	{
		NN<AVIRead::AVIRGISForm> gisForm;
		NN<Map::MapEnv> env;
		NN<Map::MapView> view;
		NN<Math::CoordinateSystem> csys;
		if (batchLyrs->GetCount() > 0)
		{
			csys = batchLyrs->GetItemNoCheck(0)->GetCoordinateSystem();
			view = batchLyrs->GetItemNoCheck(0)->CreateMapView(Math::Size2DDbl(320, 240));
			NEW_CLASSNN(env, Map::MapEnv(CSTR("Untitled"), 0xffc0c0ff, csys->Clone()));
		}
		else
		{
			csys = Math::CoordinateSystemManager::CreateWGS84Csys();
			NEW_CLASSNN(env, Map::MapEnv(CSTR("Untitled"), 0xffc0c0ff, csys->Clone()));
			view = env->CreateMapView(Math::Size2DDbl(320, 240));
		}
		NEW_CLASSNN(gisForm, AVIRead::AVIRGISForm(nullptr, this->ui, *this, env, view));
		gisForm->AddLayers(batchLyrs);
		this->batchLyrs.Delete();
		InitForm(gisForm);
		gisForm->Show();
	}

	NN<Data::ArrayListNN<Map::CesiumTile>> batchCesiumTiles;
	if (this->batchCesiumTiles.SetTo(batchCesiumTiles))
	{
		NN<AVIRead::AVIRCesiumTileForm> tileForm;
		NEW_CLASSNN(tileForm, AVIRead::AVIRCesiumTileForm(nullptr, this->ui, *this, batchCesiumTiles));
		this->batchCesiumTiles.Delete();
		InitForm(tileForm);
		tileForm->Show();
	}
}

Bool SSWR::AVIRead::AVIRCore::LoadData(NN<IO::StreamData> data, Optional<IO::PackageFile> pkgFile)
{
	NN<IO::ParsedObject> pobj;
	if (this->parsers->ParseFile(data, pkgFile).SetTo(pobj))
	{
		OpenObject(pobj);
		return true;
	}
	else
	{
		return false;
	}
}

Bool SSWR::AVIRead::AVIRCore::LoadDataType(NN<IO::StreamData> data, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType)
{
	NN<IO::ParsedObject> pobj;
	if (this->parsers->ParseFile(data, pkgFile, targetType).SetTo(pobj))
	{
		OpenObject(pobj);
		return true;
	}
	else
	{
		return false;
	}
}

NN<Parser::ParserList> SSWR::AVIRead::AVIRCore::GetParserList()
{
	return this->parsers;
}

NN<Map::MapManager> SSWR::AVIRead::AVIRCore::GetMapManager()
{
	return this->mapMgr;
}

NN<Media::ColorManager> SSWR::AVIRead::AVIRCore::GetColorManager()
{
	return this->colorMgr;
}

NN<Net::SocketFactory> SSWR::AVIRead::AVIRCore::GetSocketFactory()
{
	return this->sockf;
}

NN<Net::TCPClientFactory> SSWR::AVIRead::AVIRCore::GetTCPClientFactory()
{
	return this->clif;
}

NN<Media::DrawEngine> SSWR::AVIRead::AVIRCore::GetDrawEngine()
{
	return this->eng;
}

NN<Text::EncodingFactory> SSWR::AVIRead::AVIRCore::GetEncFactory()
{
	return this->encFact;
}

Optional<IO::SiLabDriver> SSWR::AVIRead::AVIRCore::GetSiLabDriver()
{
	return this->siLabDriver;
}

NN<Net::WebBrowser> SSWR::AVIRead::AVIRCore::GetWebBrowser()
{
	return this->browser;
}

NN<IO::VirtualIOPinMgr> SSWR::AVIRead::AVIRCore::GetVirtualIOPinMgr()
{
	return this->vioPinMgr;
}

Optional<IO::GPIOControl> SSWR::AVIRead::AVIRCore::GetGPIOControl()
{
	return this->gpioCtrl;
}

NN<Media::AudioDevice> SSWR::AVIRead::AVIRCore::GetAudioDevice()
{
	return this->audDevice;
}

UInt32 SSWR::AVIRead::AVIRCore::GetCurrCodePage()
{
	if (this->currCodePage == 0)
		return Text::EncodingFactory::GetSystemCodePage();
	else
		return this->currCodePage;
}
void SSWR::AVIRead::AVIRCore::SetCodePage(UInt32 codePage)
{
	this->currCodePage = codePage;
	this->parsers->SetCodePage(codePage);
	this->exporters.SetCodePage(codePage);
}

NN<IO::LogTool> SSWR::AVIRead::AVIRCore::GetLog()
{
	return this->log;
}

Double SSWR::AVIRead::AVIRCore::GetMonitorHDPI(Optional<MonitorHandle> hMonitor)
{
	if (this->forwardedUI)
	{
		return this->ui->GetMagnifyRatio(hMonitor) * 96.0;
	}
	else
	{
		return this->monMgr.GetMonitorHDPI(hMonitor);
	}
}

void SSWR::AVIRead::AVIRCore::SetMonitorHDPI(Optional<MonitorHandle> hMonitor, Double monitorHDPI)
{
	if (!this->forwardedUI)
	{
		this->monMgr.SetMonitorHDPI(hMonitor, monitorHDPI);
	}
}

Double SSWR::AVIRead::AVIRCore::GetMonitorDDPI(Optional<MonitorHandle> hMonitor)
{
	if (this->forwardedUI)
	{
		return 96.0;
	}
	else
	{
		return this->monMgr.GetMonitorDDPI(hMonitor);
	}
}

void SSWR::AVIRead::AVIRCore::SetMonitorDDPI(Optional<MonitorHandle> hMonitor, Double monitorDDPI)
{
	if (!this->forwardedUI)
	{
		this->monMgr.SetMonitorDDPI(hMonitor, monitorDDPI);
	}
}

NN<Media::MonitorMgr> SSWR::AVIRead::AVIRCore::GetMonitorMgr()
{
	return this->monMgr;
}

void SSWR::AVIRead::AVIRCore::SetAudioDeviceList(Optional<Data::ArrayListStringNN> devList)
{
	NN<Data::ArrayListStringNN> nndevList;
	WChar wbuff[32];
	UOSInt i;
	i = this->audDevList.GetCount();
	while (i-- > 0)
	{
		OPTSTR_DEL(this->audDevList.GetItem(i));
	}
	this->audDevList.Clear();
	NN<IO::Registry> reg;
	if (IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, L"SSWR", L"AVIRead").SetTo(reg))
	{
		if (!devList.SetTo(nndevList))
		{
			reg->DelValue(L"AudioDevice0");
		}
		else
		{
			Data::ArrayIterator<NN<Text::String>> it = nndevList->Iterator();
			i = 0;
			while (it.HasNext())
			{
				Text::StrUOSInt(Text::StrConcat(wbuff, L"AudioDevice"), i);
				UnsafeArray<const WChar> wptr = Text::StrToWCharNew(it.Next()->v);
				reg->SetValue(wbuff, wptr);
				Text::StrDelNew(wptr);
				i++;
			}
			Text::StrUOSInt(Text::StrConcat(wbuff, L"AudioDevice"), i);
			reg->DelValue(wbuff);
		}
		IO::Registry::CloseRegistry(reg);
	}
	this->audDevice.ClearDevices();
	if (devList.SetTo(nndevList))
	{
		Data::ArrayIterator<NN<Text::String>> it = nndevList->Iterator();
		NN<Text::String> s;
		while (it.HasNext())
		{
			s = it.Next();
			this->audDevice.AddDevice(s->ToCString());
			this->audDevList.Add(s->Clone());
			i++;
		}
	}
}

NN<Data::ArrayListStringNN> SSWR::AVIRead::AVIRCore::GetAudioDeviceList()
{
	return this->audDevList;
}

Int32 SSWR::AVIRead::AVIRCore::GetAudioAPIType()
{
	return this->audAPIType;
}

Optional<Media::AudioRenderer> SSWR::AVIRead::AVIRCore::BindAudio(Optional<Media::AudioSource> audSrc)
{
	return this->audDevice.BindAudio(audSrc);
}

Bool SSWR::AVIRead::AVIRCore::GenLinePreview(NN<Media::DrawImage> img, NN<Media::DrawEngine> eng, Double lineThick, UInt32 lineColor, NN<Media::ColorConv> colorConv)
{
	NN<Media::DrawPen> p;
	NN<Media::DrawBrush> b;
	Double dpi = img->GetHDPI();
	b = img->NewBrushARGB(colorConv->ConvRGB8(0xffffffff));
	img->DrawRect(Math::Coord2DDbl(0, 0), img->GetSize().ToDouble(), nullptr, b);
	img->DelBrush(b);

	p = img->NewPenARGB(colorConv->ConvRGB8(lineColor), lineThick * dpi / 96.0, nullptr, 0);
	img->DrawLine(0, UOSInt2Double(img->GetHeight()) * 0.5, UOSInt2Double(img->GetWidth()), UOSInt2Double(img->GetHeight()) * 0.5, p);
	img->DelPen(p);
	return true;
}

Bool SSWR::AVIRead::AVIRCore::GenLineStylePreview(NN<Media::DrawImage> img, NN<Media::DrawEngine> eng, NN<Map::MapEnv> env, UOSInt lineStyle, NN<Media::ColorConv> colorConv)
{
	Math::Size2D<UOSInt> size = img->GetSize();
	Double dpi = img->GetHDPI();
	if (lineStyle >= env->GetLineStyleCount())
	{
		NN<Media::DrawFont> f = img->NewFontPt(CSTR("Arial"), 9, Media::DrawEngine::DFS_ANTIALIAS, 0);
		NN<Media::DrawBrush> b = img->NewBrushARGB(colorConv->ConvRGB8(0xffffffff));
		img->DrawRect(Math::Coord2DDbl(0, 0), size.ToDouble(), nullptr, b);
		img->DelBrush(b);
		b = img->NewBrushARGB(colorConv->ConvRGB8(0xff000000));
		img->DrawString(Math::Coord2DDbl(0, 0), CSTR("No line style"), f, b);
		img->DelBrush(b);
		img->DelFont(f);
		return false;
	}

	NN<Media::DrawPen> p;
	NN<Media::DrawBrush> b;
	b = img->NewBrushARGB(colorConv->ConvRGB8(0xffc0c0c0));
	img->DrawRect(Math::Coord2DDbl(0, 0), size.ToDouble(), nullptr, b);
	img->DelBrush(b);

	UInt32 color;
	UOSInt layerId = 0;
	Double thick;
	UnsafeArrayOpt<UInt8> pattern;
	UOSInt npattern;

	while (env->GetLineStyleLayer(lineStyle, layerId++, color, thick, pattern, npattern))
	{
		p = img->NewPenARGB(colorConv->ConvRGB8(color), thick * dpi / 96.0, pattern, npattern);
		img->DrawLine(0, UOSInt2Double(size.y >> 1), UOSInt2Double(size.x), UOSInt2Double(size.y >> 1), p);
		img->DelPen(p);
	}
	return true;
}

Bool SSWR::AVIRead::AVIRCore::GenFontStylePreview(NN<Media::DrawImage> img, NN<Media::DrawEngine> eng, NN<Map::MapEnv> env, UOSInt fontStyle, NN<Media::ColorConv> colorConv)
{
	Math::Size2D<UOSInt> size = img->GetSize();
	Double dpi = img->GetHDPI();
	
	if (fontStyle >= env->GetFontStyleCount())
	{
		NN<Media::DrawFont> f = img->NewFontPt(CSTR("Arial"), 9.0, Media::DrawEngine::DFS_ANTIALIAS, 0);
		NN<Media::DrawBrush> b = img->NewBrushARGB(colorConv->ConvRGB8(0xffffffff));
		img->DrawRect(Math::Coord2DDbl(0, 0), size.ToDouble(), nullptr, b);
		img->DelBrush(b);
		b = img->NewBrushARGB(colorConv->ConvRGB8(0xff000000));
		img->DrawString(Math::Coord2DDbl(0, 0), CSTR("No font style"), f, b);
		img->DelBrush(b);
		img->DelFont(f);
		return false;
	}

	NN<Media::DrawFont> f;
	NN<Media::DrawBrush> b;
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	Math::Coord2DDbl refPos;
	b = img->NewBrushARGB(colorConv->ConvRGB8(0xffffffff));
	img->DrawRect(Math::Coord2DDbl(0, 0), size.ToDouble(), nullptr, b);
	img->DelBrush(b);

	NN<Text::String> fontName;
	Double fontSizePt;
	Bool bold;
	UInt32 fontColor;
	UOSInt buffSize;
	UInt32 buffColor;
	Math::Size2DDbl sz;

	if (env->GetFontStyle(fontStyle, fontName, fontSizePt, bold, fontColor, buffSize, buffColor))
	{
		buffSize = (UOSInt)Double2Int32(UOSInt2Double(buffSize) * dpi / 96.0);
		if (!env->GetFontStyleName(fontStyle, sbuff).SetTo(sptr))
		{
			sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Style ")), fontStyle);
		}
		f = img->NewFontPt(fontName->ToCString(), fontSizePt, bold?((Media::DrawEngine::DrawFontStyle)(Media::DrawEngine::DFS_BOLD | Media::DrawEngine::DFS_ANTIALIAS)):Media::DrawEngine::DFS_ANTIALIAS, this->currCodePage);
		sz = img->GetTextSize(f, CSTRP(sbuff, sptr));
		refPos = (size.ToDouble() - sz) * 0.5;
		if (buffSize > 0)
		{
			b = img->NewBrushARGB(colorConv->ConvRGB8(buffColor));
			img->DrawStringB(refPos, CSTRP(sbuff, sptr), f, b, buffSize);
			img->DelBrush(b);
		}
		b = img->NewBrushARGB(colorConv->ConvRGB8(fontColor));
		img->DrawString(refPos, CSTRP(sbuff, sptr), f, b);
		img->DelBrush(b);

		img->DelFont(f);
	}
	return true;
}

Bool SSWR::AVIRead::AVIRCore::GenFontPreview(NN<Media::DrawImage> img, NN<Media::DrawEngine> eng, Text::CStringNN fontName, Double fontSizePt, UInt32 fontColor, NN<Media::ColorConv> colorConv)
{
	if (fontName.leng == 0)
	{
		fontName = CSTR("Arial");
	}
	Math::Size2DDbl sz;
	NN<Media::DrawFont> f;
	NN<Media::DrawBrush> b;
	b = img->NewBrushARGB(colorConv->ConvRGB8(0xffffffff));
	img->DrawRect(Math::Coord2DDbl(0, 0), img->GetSize().ToDouble(), nullptr, b);
	img->DelBrush(b);

	b = img->NewBrushARGB(colorConv->ConvRGB8(fontColor));
	f = img->NewFontPt(fontName, fontSizePt, Media::DrawEngine::DFS_ANTIALIAS, this->currCodePage);
	sz = img->GetTextSize(f, fontName);
	img->DrawString(Math::Coord2DDbl((UOSInt2Double(img->GetWidth()) - sz.x) * 0.5, (UOSInt2Double(img->GetHeight()) - sz.y) * 0.5), fontName, f, b);
	img->DelFont(f);
	img->DelBrush(b);
	return true;
}

void SSWR::AVIRead::AVIRCore::ShowForm(NN<UI::GUIForm> frm)
{
	frm->Show();
	this->InitForm(frm);
}

void SSWR::AVIRead::AVIRCore::CloseAllForm()
{
	NN<UI::GUIForm> frm;
	UOSInt i = this->frms.GetCount();
	while (i-- > 0)
	{
		if (this->frms.GetItem(i).SetTo(frm))
		{
			frm->Close();
		}
	}
}

void SSWR::AVIRead::AVIRCore::SetGISForm(Optional<SSWR::AVIRead::AVIRGISForm> frm)
{
	this->gisForm = frm;
}

Optional<SSWR::AVIRead::AVIRGISForm> SSWR::AVIRead::AVIRCore::GetGISForm()
{
	return this->gisForm;
}

Text::CStringNN SSWR::AVIRead::AVIRCore::IOPinTypeGetName(IOPinType iopt)
{
	switch (iopt)
	{
	case AVIRCore::IOPT_GPIO:
		return CSTR("GPIO");
	case AVIRCore::IOPT_VIOPIN:
		return CSTR("VirtualIOPin");
	default:
		return CSTR("Unknown");
	}
}
