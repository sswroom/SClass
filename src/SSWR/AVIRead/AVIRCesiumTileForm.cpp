#include "Stdafx.h"
#include "Manage/Process.h"
#include "Net/WebServer/HTTPServerUtil.h"
#include "SSWR/AVIRead/AVIRCesiumTileForm.h"

class AVIRCesiumTileHandler : public Net::WebServer::WebServiceHandler
{
private:
	static Bool __stdcall IndexFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
	{
/*
<!DOCTYPE html>
<html lang="en">
	<head>
		<meta charset="UTF-8" />
		<meta http-equiv="X-UA-Compatible" content="IE=edge" />
		<meta name="viewport" content="width=device-width, initial-scale=1.0" />
		<title>Cesium Tile</title>
		<link rel="manifest" href="manifest.json" />
		<meta name="theme-color" content="#181c25" />
		<script src="https://cesium.com/downloads/cesiumjs/releases/1.127/Build/Cesium/Cesium.js"></script>
		<link href="https://cesium.com/downloads/cesiumjs/releases/1.127/Build/Cesium/Widgets/widgets.css" rel="stylesheet">
		<script type="module">
			let providerViewModels = [];

			providerViewModels.push(new Cesium.ProviderViewModel({
				name : 'OpenStreetMap',
				iconUrl : Cesium.buildModuleUrl('Widgets/Images/ImageryProviders/openStreetMap.png'),
				tooltip : 'OpenStreetMap (OSM) is a collaborative project to create a free editable map of the world. \nhttp://www.openstreetmap.org',
				creationFunction : function() {
					return new Cesium.OpenStreetMapImageryProvider({
						url : '//tile.openstreetmap.org/'
					});
				}
			}));

			let viewer = new Cesium.Viewer('map', {
				timeline:false,
				animation:false,
				vrButton:true,
				sceneModePicker:false,
				infoBox:true,
				scene3DOnly:true,
				selectedImageryProviderViewModel : providerViewModels[0],
			});

			viewer.scene.camera.setView({
				destination: Cesium.Cartesian3.fromDegrees(114.2, 22.4, 10000)
			});

			const paramsString = window.location.search;
			const searchParams = new URLSearchParams(paramsString);
			let f = searchParams.get("f");
			if (f)
			{
				let tileset = await Cesium.Cesium3DTileset.fromUrl("data/"+searchParams.get("f"), {
					maximumScreenSpaceError : 1,
					maximumNumberOfLoadedTiles : 3000
				});
				viewer.scene.primitives.add(tileset);
				viewer.zoomTo(tileset);
			}
		</script>
	</head>
	<body style="margin: 0px;">
		<div id="map" style="width: 100%; height: 100vh"></div>
	</body>
</html>

*/		
		Text::CStringNN val = CSTR("<!DOCTYPE html>\n"
"<html lang=\"en\">\n"
"	<head>\n"
"		<meta charset=\"UTF-8\" />\n"
"		<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\" />\n"
"		<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />\n"
"		<title>Cesium Tile</title>\n"
"		<link rel=\"manifest\" href=\"manifest.json\" />\n"
"		<meta name=\"theme-color\" content=\"#181c25\" />\n"
"		<script src=\"https://cesium.com/downloads/cesiumjs/releases/1.127/Build/Cesium/Cesium.js\"></script>\n"
"		<link href=\"https://cesium.com/downloads/cesiumjs/releases/1.127/Build/Cesium/Widgets/widgets.css\" rel=\"stylesheet\">\n"
"		<script type=\"module\">\n"
"			let providerViewModels = [];\n"
"\n"
"			providerViewModels.push(new Cesium.ProviderViewModel({\n"
"				name : 'OpenStreetMap',\n"
"				iconUrl : Cesium.buildModuleUrl('Widgets/Images/ImageryProviders/openStreetMap.png'),\n"
"				tooltip : 'OpenStreetMap (OSM) is a collaborative project to create a free editable map of the world. \\nhttp://www.openstreetmap.org',\n"
"				creationFunction : function() {\n"
"					return new Cesium.OpenStreetMapImageryProvider({\n"
"						url : '//tile.openstreetmap.org/'\n"
"					});\n"
"				}\n"
"			}));\n"
"\n"
"			let viewer = new Cesium.Viewer('map', {\n"
"				timeline:false,\n"
"				animation:false,\n"
"				vrButton:true,\n"
"				sceneModePicker:false,\n"
"				infoBox:true,\n"
"				scene3DOnly:true,\n"
"				selectedImageryProviderViewModel : providerViewModels[0],\n"
"			});\n"
"\n"
"			viewer.scene.camera.setView({\n"
"				destination: Cesium.Cartesian3.fromDegrees(114.2, 22.4, 10000)\n"
"			});\n"
"\n"
"			const paramsString = window.location.search;\n"
"			const searchParams = new URLSearchParams(paramsString);\n"
"			let f = searchParams.get(\"f\");\n"
"			if (f)\n"
"			{\n"
"				let tileset = await Cesium.Cesium3DTileset.fromUrl(\"data/\"+searchParams.get(\"f\"), {\n"
"					maximumScreenSpaceError : 1,\n"
"					maximumNumberOfLoadedTiles : 3000\n"
"				});\n"
"				viewer.scene.primitives.add(tileset);\n"
"				viewer.zoomTo(tileset);\n"
"			}\n"
"		</script>\n"
"	</head>\n"
"	<body style=\"margin: 0px;\">\n"
"		<div id=\"map\" style=\"width: 100%; height: 100vh\"></div>\n"
"	</body>\n"
"</html>");
		resp->AddDefHeaders(req);
		return Net::WebServer::HTTPServerUtil::SendContent(req, resp, CSTR("text/html"), val);
	}
public:
	AVIRCesiumTileHandler(NN<Map::CesiumTile> tile)
	{
		this->SetAllowBrowsing(true);
		this->AddPackage(CSTR("data"), tile->GetPackageFile()->Clone());
		this->AddService(CSTR("/index.html"), Net::WebUtil::RequestMethod::HTTP_GET, IndexFunc);
	}

	virtual ~AVIRCesiumTileHandler()
	{
	}

};

void __stdcall SSWR::AVIRead::AVIRCesiumTileForm::OnOpenClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCesiumTileForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCesiumTileForm>();
	Text::StringBuilderUTF8 sb;
	me->txtURL->GetText(sb);
	Manage::Process::OpenPath(sb.ToCString());
}

SSWR::AVIRead::AVIRCesiumTileForm::AVIRCesiumTileForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::CesiumTile> tile) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Cesium Tile"));
	this->SetFont(0, 0, 8.25, false);
	this->tile = tile;
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblPort = ui->NewLabel(*this, CSTR("Port"));
	this->lblPort->SetRect(4, 4, 100, 23, false);
	this->txtPort = ui->NewTextBox(*this, CSTR(""));
	this->txtPort->SetRect(104, 4, 100, 23, false);
	this->txtPort->SetReadOnly(true);
	this->lblURL = ui->NewLabel(*this, CSTR("Port"));
	this->lblURL->SetRect(4, 28, 100, 23, false);
	this->txtURL = ui->NewTextBox(*this, CSTR(""));
	this->txtURL->SetRect(104, 28, 600, 23, false);
	this->txtURL->SetReadOnly(true);
	this->btnOpen = ui->NewButton(*this, CSTR("Open"));
	this->btnOpen->SetRect(704, 28, 75, 23, false);
	this->btnOpen->HandleButtonClick(OnOpenClicked, this);

	NEW_CLASSNN(this->hdlr, AVIRCesiumTileHandler(this->tile));
	NEW_CLASSNN(this->listener, Net::WebServer::WebListener(core->GetTCPClientFactory(), 0, this->hdlr, 0, 30, 1, 4, CSTR("CesiumTile/1.0"), false, Net::WebServer::KeepAlive::Always, true));
	UInt16 port = this->listener->GetListenPort();
	if (port != 0)
	{
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		Text::StringBuilderUTF8 sb;
		sb.AppendU16(port);
		this->txtPort->SetText(sb.ToCString());
		sb.ClearStr();
		sb.Append(CSTR("http://127.0.0.1:"));
		sb.AppendU16(port);
		sb.Append(CSTR("/index.html"));
		NN<Text::String> f;
		if (this->tile->GetJSONFile().SetTo(f))
		{
			sb.Append(CSTR("?f="));
			sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, f->v);
			sb.AppendP(sbuff, sptr);
		}
		this->txtURL->SetText(sb.ToCString());

		OnOpenClicked(this);
	}
}

SSWR::AVIRead::AVIRCesiumTileForm::~AVIRCesiumTileForm()
{
	this->listener.Delete();
	this->tile.Delete();
}

void SSWR::AVIRead::AVIRCesiumTileForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
