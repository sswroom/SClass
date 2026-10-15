#include "Stdafx.h"
#include "Data/Compress/Deflater.h"
#include "Exporter/GUIJPGExporter.h"
#include "Exporter/PNGExporter.h"
#include "Exporter/WebPExporter.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "IO/ZIPBuilder.h"
#include "IO/StmData/FileData.h"
#include "Map/DrawMapRenderer.h"
#include "Map/MercatorMapView.h"
#include "Map/MercatorTileMap.h"
#include "Math/CoordinateSystemManager.h"
#include "Media/ImageList.h"
#include "Media/StaticImage.h"
#include "SSWR/AVIRead/AVIROSMTileGeneratorForm.h"
#include "Text/JSONBuilder.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/MyString.h"
#include "UI/GUIFileDialog.h"

struct ThreadStatus
{
	Int32 x;
	Int32 y;
	UIntOS level;
	Bool hasRequest;
	Bool isProcessing;
	UIntOS failCount;
	UIntOS successCount;
};

void __stdcall SSWR::AVIRead::AVIROSMTileGeneratorForm::OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIROSMTileGeneratorForm> me = userObj.GetNN<SSWR::AVIRead::AVIROSMTileGeneratorForm>();
	UIntOS i = 0;
	UIntOS nFiles = files.GetCount();
	while (i < nFiles)
	{
		me->AddFile(files[i]->ToCString());
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIROSMTileGeneratorForm::OnExportClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIROSMTileGeneratorForm> me = userObj.GetNN<SSWR::AVIRead::AVIROSMTileGeneratorForm>();
	if (me->layers.GetCount() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please drag and drop at least one map layer"), CSTR("OSM Tile Generator"), me);
		return;
	}

	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"OSMTileGenerator", true);
	dlg->AddFilter(CSTR("*.zip"), CSTR("ZIP archive"));
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->ExportTiles(dlg->GetFileName()->ToCString());
	}
	dlg.Delete();
}

void SSWR::AVIRead::AVIROSMTileGeneratorForm::UpdateBounds()
{
	Math::RectAreaDbl bounds;
	if (!this->env->GetBounds(nullptr, bounds))
	{
		this->txtBounds->SetText(CSTR(""));
		this->btnExport->SetEnabled(false);
		return;
	}

	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("Min X: "));
	sb.AppendDouble(bounds.min.x);
	sb.Append(CSTR("\r\nMin Y: "));
	sb.AppendDouble(bounds.min.y);
	sb.Append(CSTR("\r\nMax X: "));
	sb.AppendDouble(bounds.max.x);
	sb.Append(CSTR("\r\nMax Y: "));
	sb.AppendDouble(bounds.max.y);
	sb.Append(CSTR("\r\nCoordinates: WGS 84"));
	this->txtBounds->SetText(sb.ToCString());
	this->btnExport->SetEnabled(true);
}

void SSWR::AVIRead::AVIROSMTileGeneratorForm::AddFile(Text::CStringNN fileName)
{
	IO::StmData::FileData fd(fileName, false);
	NN<Map::MapDrawLayer> layer;
	if (Optional<Map::MapDrawLayer>::ConvertFrom(this->core->GetParserList()->ParseFileType(fd, IO::ParserType::MapLayer)).SetTo(layer))
	{
		this->layers.Add(layer);
		this->env->AddLayer(nullptr, layer, false);
		this->lbLayers->AddItem(layer->GetName(), layer);
		this->UpdateBounds();
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		sb.Append(CSTR("Cannot parse file as a map layer:\r\n"));
		sb.Append(fileName);
		this->ui->ShowMsgOK(sb.ToCString(), CSTR("OSM Tile Generator"), this);
	}
}

void SSWR::AVIRead::AVIROSMTileGeneratorForm::ExportTiles(Text::CStringNN fileName)
{
	Math::RectAreaDbl bounds;
	if (!this->env->GetBounds(nullptr, bounds))
	{
		this->ui->ShowMsgOK(CSTR("No map layer bounds are available"), CSTR("OSM Tile Generator"), this);
		return;
	}
	Text::StringBuilderUTF8 sb;
	IntOS minLevel = this->cboMinLevel->GetSelectedItem().GetIntOS();
	IntOS maxLevel = this->cboMaxLevel->GetSelectedItem().GetIntOS();
	if (minLevel < 0 || maxLevel < minLevel || maxLevel > 22)
	{
		this->ui->ShowMsgOK(CSTR("Please select a valid level range"), CSTR("OSM Tile Generator"), this);
		return;
	}
	this->txtQuality->GetText(sb);
	Int32 quality;
	if (!sb.ToInt32(quality) || quality < 0 || quality > 100)
	{
		this->ui->ShowMsgOK(CSTR("Please enter a valid quality value"), CSTR("OSM Tile Generator"), this);
		return;
	}

	Double minLon = bounds.min.x;
	Double maxLon = bounds.max.x;
	Double minLat = bounds.min.y;
	Double maxLat = bounds.max.y;
	if (minLon < -180.0) minLon = -180.0;
	if (maxLon > 180.0) maxLon = 180.0;
	if (minLat < -85.051128779806592377796715521925) minLat = -85.051128779806592377796715521925;
	if (maxLat > 85.051128779806592377796715521925) maxLat = 85.051128779806592377796715521925;
	if (minLon > maxLon || minLat > maxLat)
	{
		this->ui->ShowMsgOK(CSTR("The layer bounds do not intersect the Web Mercator area"), CSTR("OSM Tile Generator"), this);
		return;
	}

	UInt64 tileCount = 0;
	UIntOS level = (UIntOS)minLevel;
	while (level <= (UIntOS)maxLevel)
	{
		Int32 maxTile = (Int32)((1U << level) - 1);
		Int32 x1 = Map::MercatorTileMap::Lon2TileX(minLon, level);
		Int32 x2 = Map::MercatorTileMap::Lon2TileX(maxLon, level);
		Int32 y1 = Map::MercatorTileMap::Lat2TileY(maxLat, level);
		Int32 y2 = Map::MercatorTileMap::Lat2TileY(minLat, level);
		if (x1 < 0) x1 = 0;
		if (x2 > maxTile) x2 = maxTile;
		if (y1 < 0) y1 = 0;
		if (y2 > maxTile) y2 = maxTile;
		if (x1 <= x2 && y1 <= y2)
		{
			tileCount += (UInt64)(x2 - x1 + 1) * (UInt64)(y2 - y1 + 1);
		}
		level++;
	}
	if (tileCount == 0)
	{
		this->ui->ShowMsgOK(CSTR("No tiles fall within the layer bounds"), CSTR("OSM Tile Generator"), this);
		return;
	}
	sb.ClearStr();
	sb.Append(CSTR("Generate "));
	sb.AppendU64(tileCount);
	sb.Append(CSTR(" tiles? This may take a long time and use significant disk space."));
	if (!this->ui->ShowMsgYesNo(sb.ToCString(), CSTR("OSM Tile Generator"), this))
		return;

	IO::FileStream fs(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
	{
		this->ui->ShowMsgOK(CSTR("Cannot create the output ZIP file"), CSTR("OSM Tile Generator"), this);
		return;
	}
	IO::ZIPBuilder zip(fs, IO::ZIPOS::UNIX);

	Map::TileMap::TileFormat tileFormat;
	Text::CStringNN formatName;
	Text::CStringNN extension;
	IntOS formatIndex = this->cboFormat->GetSelectedItem().GetIntOS();
	if (formatIndex == 0)
	{
		tileFormat = Map::TileMap::TileFormat::WEBP;
		formatName = CSTR("webp");
		extension = CSTR(".webp");
	}
	else if (formatIndex == 1)
	{
		tileFormat = Map::TileMap::TileFormat::JPG;
		formatName = CSTR("jpg");
		extension = CSTR(".jpg");
	}
	else
	{
		tileFormat = Map::TileMap::TileFormat::PNG;
		formatName = CSTR("png");
		extension = CSTR(".png");
	}

	UIntOS i;
	Text::CStringNN tileName = fileName;
	i = tileName.LastIndexOf(IO::Path::PATH_SEPERATOR);
	if (i != INVALID_INDEX)
	{
		tileName = tileName.Substring(i + 1);
	}
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	i = tileName.LastIndexOf('.');
	if (i != INVALID_INDEX)
	{
		json.ObjectAddStr(CSTR("name"), Text::CStringNN(tileName.v, i));
	}
	else
	{
		json.ObjectAddStr(CSTR("name"), tileName);
	}
	json.ObjectAddStr(CSTR("version"), CSTR("1.0"));
	json.ObjectAddStr(CSTR("description"), CSTR("Generated by AVIRead"));
	json.ObjectAddStr(CSTR("attribution"), CSTR("Generated by AVIRead (sswroom)"));
	json.ObjectAddStr(CSTR("type"), CSTR("overlay"));
	json.ObjectAddStr(CSTR("format"), formatName);
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr = Text::StrUIntOS(sbuff, (UIntOS)minLevel);
	json.ObjectAddStr(CSTR("minzoom"), CSTRP(sbuff, sptr));
	sptr = Text::StrUIntOS(sbuff, (UIntOS)maxLevel);
	json.ObjectAddStr(CSTR("maxzoom"), CSTRP(sbuff, sptr));
	json.ObjectAddStr(CSTR("scale"), CSTR("1.000000"));
	json.ObjectBeginArray(CSTR("bounds"));
	json.ArrayAddFloat64(minLon);
	json.ArrayAddFloat64(minLat);
	json.ArrayAddFloat64(maxLon);
	json.ArrayAddFloat64(maxLat);
	json.ArrayEnd();
	json.ObjectAddStr(CSTR("profile"), CSTR("mercator"));
	Text::CStringNN metadata = json.Build();
	Data::Timestamp now = Data::Timestamp::UtcNow();
	Bool success = zip.AddFile(CSTR("metadata.json"), metadata.v, metadata.leng, now, now, now, Data::Compress::Deflater::CompLevel::BestSpeed, 0);

	NN<Media::DrawEngine> drawEngine = this->core->GetDrawEngine();
	Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
	Map::DrawMapRenderer renderer(drawEngine, this->env, color, nullptr, Map::DrawMapRenderer::DrawType::DT_PIXELDRAW);
	Exporter::GUIJPGExporter jpgExporter;
	Exporter::WebPExporter webpExporter;
	Exporter::PNGExporter pngExporter;
	IO::MemoryStream imageStream(65536);
	level = (UIntOS)minLevel;
	while (success && level <= (UIntOS)maxLevel)
	{
		Int32 maxTile = (Int32)((1U << level) - 1);
		Int32 x1 = Map::MercatorTileMap::Lon2TileX(minLon, level);
		Int32 x2 = Map::MercatorTileMap::Lon2TileX(maxLon, level);
		Int32 y1 = Map::MercatorTileMap::Lat2TileY(maxLat, level);
		Int32 y2 = Map::MercatorTileMap::Lat2TileY(minLat, level);
		if (x1 < 0) x1 = 0;
		if (x2 > maxTile) x2 = maxTile;
		if (y1 < 0) y1 = 0;
		if (y2 > maxTile) y2 = maxTile;
		if (x1 <= x2 && y1 <= y2)
		{
			sptr = Text::StrUIntOS(sbuff, level);
			*sptr++ = '/';
			*sptr = 0;
			success = zip.AddDir(CSTRP(sbuff, sptr), now, now, now, 0);
			Int32 x = x1;
			while (success && x <= x2)
			{
				sptr = Text::StrUIntOS(sbuff, level);
				*sptr++ = '/';
				sptr = Text::StrInt32(sptr, x);
				*sptr++ = '/';
				*sptr = 0;
				success = zip.AddDir(CSTRP(sbuff, sptr), now, now, now, 0);
				Int32 y = y1;
				while (success && y <= y2)
				{
					Math::Coord2DDbl center(
						(Map::MercatorTileMap::TileX2Lon(x, level) + Map::MercatorTileMap::TileX2Lon(x + 1, level)) * 0.5,
						(Map::MercatorTileMap::TileY2Lat(y, level) + Map::MercatorTileMap::TileY2Lat(y + 1, level)) * 0.5);
					Map::MercatorMapView view(Math::Size2DDbl(256, 256), center, 22, 256);
					view.SetDPI(96, 96);
					view.SetMapScale(204094080000.0 / 256.0 / (Double)(1U << level));

					NN<Media::DrawImage> dimg;
					if (!drawEngine->CreateImage32(Math::Size2D<UIntOS>(256, 256), (tileFormat == Map::TileMap::TileFormat::JPG)?Media::AlphaType::AT_IGNORE_ALPHA:Media::AlphaType::AT_ALPHA).SetTo(dimg))
					{
						success = false;
						break;
					}
					dimg->SetHDPI(96);
					dimg->SetVDPI(96);
					if (renderer.DrawMap(dimg, view, nullptr))
					{
						this->env->WaitForLoad(60000);
						renderer.DrawMap(dimg, view, nullptr);
					}

					imageStream.Clear();
					NN<Media::StaticImage> simg;
					if (!dimg->ToStaticImage().SetTo(simg))
					{
						success = false;
					}
					else
					{
						if (!simg->ToB8G8R8A8())
						{
							simg.Delete();
							success = false;
						}
						else if (tileFormat == Map::TileMap::TileFormat::PNG)
						{
							UnsafeArray<UInt8> imgPtr = simg->data;
							UIntOS bpl = simg->GetDataBpl();
							UIntOS py = 0;
							while (py < 256)
							{
								UnsafeArray<UInt8> px = imgPtr + py * bpl;
								UIntOS xPos = 0;
								while (xPos < 256)
								{
									UInt32 grey = (77U * px[2] + 150U * px[1] + 29U * px[0] + 128U) >> 8;
									px[0] = (UInt8)grey;
									px[1] = (UInt8)grey;
									px[2] = (UInt8)grey;
									px[3] = 255;
									px += 4;
									xPos++;
								}
								py++;
							}
							success = pngExporter.ExportImage(imageStream, simg);
						}
						else if (tileFormat == Map::TileMap::TileFormat::JPG)
						{
							NN<Media::ImageList> imageList;
							NEW_CLASSNN(imageList, Media::ImageList(CSTR("tile")));
							imageList->AddImage(simg, 0);
							Optional<IO::FileExporter::ParamData> param = jpgExporter.CreateParam(imageList);
							jpgExporter.SetParamInt32(param, 0, quality);
							success = jpgExporter.ExportFile(imageStream, CSTR("tile.jpg"), imageList, param);
							jpgExporter.DeleteParam(param);
							imageList.Delete();
						}
						else
						{
							NN<Media::ImageList> imageList;
							NEW_CLASSNN(imageList, Media::ImageList(CSTR("tile")));
							imageList->AddImage(simg, 0);
							Optional<IO::FileExporter::ParamData> param = webpExporter.CreateParam(imageList);
							webpExporter.SetParamInt32(param, 0, quality);
							success = webpExporter.ExportFile(imageStream, CSTR("tile.webp"), imageList, param);
							webpExporter.DeleteParam(param);
							imageList.Delete();
						}
					}
					drawEngine->DeleteImage(dimg);
					if (success)
					{
						UTF8Char path[128];
						sptr = Text::StrUIntOS(path, level);
						*sptr++ = '/';
						sptr = Text::StrInt32(sptr, x);
						*sptr++ = '/';
						sptr = Text::StrInt32(sptr, y);
						sptr = extension.ConcatTo(sptr);
						success = zip.AddFile(CSTRP(path, sptr), imageStream.GetBuff(), (UIntOS)imageStream.GetLength(), now, now, now, Data::Compress::Deflater::CompLevel::NoCompression, 0);
					}
					y++;
				}
				x++;
			}
		}
		level++;
	}
	if (success)
	{
		this->ui->ShowMsgOK(CSTR("Tile ZIP export completed"), CSTR("OSM Tile Generator"), this);
	}
	else
	{
		this->ui->ShowMsgOK(CSTR("An error occurred while generating the tile ZIP"), CSTR("OSM Tile Generator"), this);
	}
}

SSWR::AVIRead::AVIROSMTileGeneratorForm::AVIROSMTileGeneratorForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 640, 450, ui)
{
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	NN<Math::CoordinateSystem> csys = Math::CoordinateSystemManager::CreateWGS84Csys();
	NEW_CLASSNN(this->env, Map::MapEnv(CSTR("OSM Tile Generator"), 0xffffffff, csys));

	this->SetText(CSTR("OSM Tile Generator"));
	this->SetFont(nullptr, 8.25, false);
	this->lbLayers = ui->NewListBox(*this, false);
	this->lbLayers->SetRect(4, 4, 620, 216, false);
	this->lblBounds = ui->NewLabel(*this, CSTR("Overall bounds (WGS 84)"));
	this->lblBounds->SetRect(4, 224, 180, 23, false);
	this->txtBounds = ui->NewTextBox(*this, CSTR(""), true);
	this->txtBounds->SetRect(184, 224, 250, 88, false);
	this->txtBounds->SetReadOnly(true);
	this->lblMinLevel = ui->NewLabel(*this, CSTR("Min level"));
	this->lblMinLevel->SetRect(4, 320, 100, 23, false);
	this->cboMinLevel = ui->NewComboBox(*this, false);
	this->cboMinLevel->SetRect(104, 320, 100, 23, false);
	this->lblMaxLevel = ui->NewLabel(*this, CSTR("Max level"));
	this->lblMaxLevel->SetRect(220, 320, 100, 23, false);
	this->cboMaxLevel = ui->NewComboBox(*this, false);
	this->cboMaxLevel->SetRect(320, 320, 100, 23, false);
	UIntOS i = 0;
	while (i <= 22)
	{
		UTF8Char sbuff[16];
		UnsafeArray<UTF8Char> sptr = Text::StrUIntOS(sbuff, i);
		this->cboMinLevel->AddItem(CSTRP(sbuff, sptr), (void*)(IntOS)i);
		this->cboMaxLevel->AddItem(CSTRP(sbuff, sptr), (void*)(IntOS)i);
		i++;
	}
	this->cboMinLevel->SetSelectedIndex(0);
	this->cboMaxLevel->SetSelectedIndex(18);
	this->lblFormat = ui->NewLabel(*this, CSTR("Image format"));
	this->lblFormat->SetRect(4, 348, 100, 23, false);
	this->cboFormat = ui->NewComboBox(*this, false);
	this->cboFormat->SetRect(104, 348, 180, 23, false);
	this->cboFormat->AddItem(CSTR("WebP"), (void*)0);
	this->cboFormat->AddItem(CSTR("JPEG"), (void*)1);
	this->cboFormat->AddItem(CSTR("Greyscale PNG"), (void*)2);
	this->cboFormat->SetSelectedIndex(0);
	this->lblQuality = ui->NewLabel(*this, CSTR("Quality"));
	this->lblQuality->SetRect(304, 348, 100, 23, false);
	this->txtQuality = ui->NewTextBox(*this, CSTR("75"), true);
	this->txtQuality->SetRect(404, 348, 100, 23, false);
	this->btnExport = ui->NewButton(*this, CSTR("Export ZIP"));
	this->btnExport->SetRect(4, 382, 120, 23, false);
	this->btnExport->HandleButtonClick(OnExportClicked, this);
	this->btnExport->SetEnabled(false);
	this->HandleDropFiles(OnFileDrop, this);
}

SSWR::AVIRead::AVIROSMTileGeneratorForm::~AVIROSMTileGeneratorForm()
{
	this->env.Delete();
	UIntOS i = this->layers.GetCount();
	while (i-- > 0)
	{
		this->layers.GetItemNoCheck(i).Delete();
	}
}

void SSWR::AVIRead::AVIROSMTileGeneratorForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
