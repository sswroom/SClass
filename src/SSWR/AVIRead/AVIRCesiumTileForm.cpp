#include "Stdafx.h"
#include "IO/Path.h"
#include "Manage/Process.h"
#include "Net/WebServer/HTTPServerUtil.h"
#include "SSWR/AVIRead/AVIRCesiumTileForm.h"

class AVIRCesiumTileHandler : public Net::WebServer::WebServiceHandler
{
private:
	NN<Data::ArrayListNN<Map::CesiumTile>> tileList;
	Bool lightLevel;

	static Bool __stdcall IndexFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
	{
		NN<AVIRCesiumTileHandler> me = NN<AVIRCesiumTileHandler>::ConvertFrom(svcHdlr);
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

			fetch("tiles.json").then(resp=>resp.json()).then(async (tiles)=>{
				let t;
				let found = false;
				for (t in tiles)
				{
					let tileset = await Cesium.Cesium3DTileset.fromUrl(tiles[t], {
						maximumScreenSpaceError : 1,
						maximumNumberOfLoadedTiles : 3000,
						skipLevelOfDetail: true,
						baseScreenSpaceError: 1024,
						skipScreenSpaceErrorFactor: 16,
						skipLevels: 1,
						immediatelyLoadDesiredLevelOfDetail: false,
						loadSiblings: false,
						cullWithChildrenBounds: true
					});
					viewer.scene.primitives.add(tileset);
					if (!found)
					{
						viewer.zoomTo(tileset);
						found = true;
					}
				}
			});
		</script>
	</head>
	<body style="margin: 0px;">
		<div id="map" style="width: 100%; height: 100vh"></div>
	</body>
</html>

*/		
		Text::CStringNN val;
		if (me->lightLevel)
		{
			val = CSTR("<!DOCTYPE html>\n"
"<html lang=\"en\">\n"
"	<head>\n"
"		<meta charset=\"UTF-8\" />\n"
"		<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\" />\n"
"		<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />\n"
"		<title>Cesium Tile</title>\n"
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
"			fetch(\"tiles.json\").then(resp=>resp.json()).then(async (tiles)=>{\n"
"				let t;\n"
"				let found = false;\n"
"				for (t in tiles)\n"
"				{\n"
"					let tileset = await Cesium.Cesium3DTileset.fromUrl(tiles[t], {\n"
//"						maximumScreenSpaceError : 1,\n"
//"						maximumNumberOfLoadedTiles : 3000,\n"
"						skipLevelOfDetail: true,\n"
"						baseScreenSpaceError: 1024,\n"
"						skipScreenSpaceErrorFactor: 16,\n"
"						skipLevels: 1,\n"
"						immediatelyLoadDesiredLevelOfDetail: false,\n"
"						loadSiblings: false,\n"
"						cullWithChildrenBounds: true\n"
"					});\n"
"					viewer.scene.primitives.add(tileset);\n"
"					if (!found)\n"
"					{\n"
"						viewer.zoomTo(tileset);\n"
"						found = true;\n"
"					}\n"
"				}\n"
"			});\n"
"		</script>\n"
"	</head>\n"
"	<body style=\"margin: 0px;\">\n"
"		<div id=\"map\" style=\"width: 100%; height: 100vh\"></div>\n"
"	</body>\n"
"</html>\n");
		}
		else
		{
			val = CSTR("<!DOCTYPE html>\n"
"<html lang=\"en\">\n"
"	<head>\n"
"		<meta charset=\"UTF-8\" />\n"
"		<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\" />\n"
"		<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />\n"
"		<title>Cesium Tile</title>\n"
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
"			fetch(\"tiles.json\").then(resp=>resp.json()).then(async (tiles)=>{\n"
"				let t;\n"
"				let found = false;\n"
"				for (t in tiles)\n"
"				{\n"
"					let tileset = await Cesium.Cesium3DTileset.fromUrl(tiles[t]);\n"
"					viewer.scene.primitives.add(tileset);\n"
"					if (!found)\n"
"					{\n"
"						viewer.zoomTo(tileset);\n"
"						found = true;\n"
"					}\n"
"				}\n"
"			});\n"
"		</script>\n"
"	</head>\n"
"	<body style=\"margin: 0px;\">\n"
"		<div id=\"map\" style=\"width: 100%; height: 100vh\"></div>\n"
"	</body>\n"
"</html>\n");
		}
		resp->AddDefHeaders(req);
		return Net::WebServer::HTTPServerUtil::SendContent(req, resp, CSTR("text/html"), val);
	}

	static Bool __stdcall TilesFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
	{
		NN<AVIRCesiumTileHandler> me = NN<AVIRCesiumTileHandler>::ConvertFrom(svcHdlr);
		Text::JSONBuilder json(Text::JSONBuilder::OT_ARRAY);
		Text::StringBuilderUTF8 sb;
		UOSInt i = 0;
		UOSInt j = me->tileList->GetCount();
		while (i < j)
		{
			NN<Map::CesiumTile> tile = me->tileList->GetItemNoCheck(i);
			Text::CStringNN name = tile->GetSourceNameObj()->ToCString();
			sb.ClearStr();
			sb.Append(name.Substring(name.LastIndexOf(IO::Path::PATH_SEPERATOR) + 1));
			sb.AppendUTF8Char('/');
			sb.AppendOpt(tile->GetJSONFile());
			json.ArrayAddStr(sb.ToCString());
			i++;
		}
		resp->AddDefHeaders(req);
		return Net::WebServer::HTTPServerUtil::SendContent(req, resp, CSTR("application/json"), json.Build());
	}
public:
	AVIRCesiumTileHandler(NN<Data::ArrayListNN<Map::CesiumTile>> tileList)
	{
		this->tileList = tileList;
		this->lightLevel = true;
		this->SetAllowBrowsing(true);
		this->AddService(CSTR("/index.html"), Net::WebUtil::RequestMethod::HTTP_GET, IndexFunc);
		this->AddService(CSTR("/tiles.json"), Net::WebUtil::RequestMethod::HTTP_GET, TilesFunc);
		UOSInt i = 0;
		UOSInt j = tileList->GetCount();
		while (i < j)
		{
			this->AddTile(tileList->GetItemNoCheck(i));
			i++;
		}
	}

	virtual ~AVIRCesiumTileHandler()
	{
	}

	void AddTile(NN<Map::CesiumTile> tile)
	{
		Text::CStringNN name = tile->GetSourceNameObj()->ToCString();
		UOSInt i = name.LastIndexOf(IO::Path::PATH_SEPERATOR);
		name = name.Substring(i + 1);
		this->AddPackage(name, tile->GetPackageFile()->Clone());
	}

	void SetLightLevel(Bool lightLevel)
	{
		this->lightLevel = lightLevel;
	}
};

void __stdcall SSWR::AVIRead::AVIRCesiumTileForm::OnOpenClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCesiumTileForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCesiumTileForm>();
	Text::StringBuilderUTF8 sb;
	me->txtURL->GetText(sb);
	Manage::Process::OpenPath(sb.ToCString());
}

void __stdcall SSWR::AVIRead::AVIRCesiumTileForm::OnLightLevelChkChg(AnyType userObj, Bool newValue)
{
	NN<SSWR::AVIRead::AVIRCesiumTileForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCesiumTileForm>();
	NN<AVIRCesiumTileHandler>::ConvertFrom(me->hdlr)->SetLightLevel(newValue);
}

SSWR::AVIRead::AVIRCesiumTileForm::AVIRCesiumTileForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Data::ArrayListNN<Map::CesiumTile>> tiles) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Cesium Tile"));
	this->SetFont(nullptr, 8.25, false);
	this->tileList.AddAll(tiles);
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlCtrl = ui->NewPanel(*this);
	this->pnlCtrl->SetRect(0, 0, 100, 80, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblPort = ui->NewLabel(this->pnlCtrl, CSTR("Port"));
	this->lblPort->SetRect(4, 4, 100, 23, false);
	this->txtPort = ui->NewTextBox(this->pnlCtrl, CSTR(""));
	this->txtPort->SetRect(104, 4, 100, 23, false);
	this->txtPort->SetReadOnly(true);
	this->lblURL = ui->NewLabel(this->pnlCtrl, CSTR("Port"));
	this->lblURL->SetRect(4, 28, 100, 23, false);
	this->txtURL = ui->NewTextBox(this->pnlCtrl, CSTR(""));
	this->txtURL->SetRect(104, 28, 600, 23, false);
	this->txtURL->SetReadOnly(true);
	this->btnOpen = ui->NewButton(this->pnlCtrl, CSTR("Open"));
	this->btnOpen->SetRect(704, 28, 75, 23, false);
	this->btnOpen->HandleButtonClick(OnOpenClicked, this);
	this->chkLightLevel = ui->NewCheckBox(this->pnlCtrl, CSTR("Light Level"), true);
	this->chkLightLevel->SetRect(104, 52, 100, 23, false);
	this->chkLightLevel->HandleCheckedChange(OnLightLevelChkChg, this);
	this->lbTiles = ui->NewListBox(*this, false);
	this->lbTiles->SetDockType(UI::GUIControl::DOCK_FILL);
	UOSInt i = 0;
	UOSInt j = tiles->GetCount();
	while (i < j)
	{
		NN<Map::CesiumTile> tile = tiles->GetItemNoCheck(i);
		Text::CStringNN name = tile->GetSourceNameObj()->ToCString();
		this->lbTiles->AddItem(name.Substring(name.LastIndexOf(IO::Path::PATH_SEPERATOR) + 1), tile);
		i++;
	}
	NEW_CLASSNN(this->hdlr, AVIRCesiumTileHandler(this->tileList));
	NEW_CLASSNN(this->listener, Net::WebServer::WebListener(core->GetTCPClientFactory(), 0, this->hdlr, 0, 30, 1, 6, CSTR("CesiumTile/1.0"), false, Net::WebServer::KeepAlive::Always, true));
	UInt16 port = this->listener->GetListenPort();
	if (port != 0)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendU16(port);
		this->txtPort->SetText(sb.ToCString());
		sb.ClearStr();
		sb.Append(CSTR("http://127.0.0.1:"));
		sb.AppendU16(port);
		sb.Append(CSTR("/index.html"));
		this->txtURL->SetText(sb.ToCString());

		OnOpenClicked(this);
	}
}

SSWR::AVIRead::AVIRCesiumTileForm::~AVIRCesiumTileForm()
{
	this->listener.Delete();
	this->hdlr.Delete();
	this->tileList.DeleteAll();
}

void SSWR::AVIRead::AVIRCesiumTileForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRCesiumTileForm::AddTile(NN<Map::CesiumTile> tile)
{
	this->tileList.Add(tile);
	NN<AVIRCesiumTileHandler>::ConvertFrom(this->hdlr)->AddTile(tile);
	Text::CStringNN name = tile->GetSourceNameObj()->ToCString();
	this->lbTiles->AddItem(name.Substring(name.LastIndexOf(IO::Path::PATH_SEPERATOR) + 1), tile);
}

void SSWR::AVIRead::AVIRCesiumTileForm::AddTiles(NN<Data::ArrayListNN<Map::CesiumTile>> tiles)
{
	UOSInt i = 0;
	UOSInt j = tiles->GetCount();
	while (i < j)
	{
		this->AddTile(tiles->GetItemNoCheck(i));
		i++;
	}	
}
