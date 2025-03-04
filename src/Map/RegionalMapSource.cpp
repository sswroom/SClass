#include "Stdafx.h"
#include "Crypto/Hash/CRC32RC.h"
#include "IO/Path.h"
#include "Map/CustomTileMap.h"
#include "Map/DrawMapServiceLayer.h"
#include "Map/RegionalMapSource.h"
#include "Map/TileMapLayer.h"
#include "Map/TileMapServiceSource.h"
#include "Map/WebFeatureService.h"
#include "Map/WebMapService.h"
#include "Map/ESRI/ESRIMapServer.h"

#include <stdio.h>

Map::RegionalMapSource::MapInfo Map::RegionalMapSource::maps[] = {
	// https://aginfo.cgk.affrc.go.jp/tmc/layers.html.en
	{UTF8STRCPTR("KBN25000ANF-4326"), UTF8STRCPTR("Japan"), UTF8STRCPTR("AGINFO Naro"), MapType::TMS, 2, UTF8STRCPTR("https://aginfo.cgk.affrc.go.jp/ws/tmc/1.0.0/KBN25000ANF-4326/tilemapresource.xml"),
		UTF8STRCPTR("Fundamental Geospatial Data 25000\r\nAll are drawn. Usual width lines. Filled administrative areas.\r\n\r\nThis service uses Fundamental Geospatial Data published by Geospatial Information Authority of Japan (GSI) with its approval under the article 30 of The Survey Act. (Approval Number ZYOU-SHI No.1028 2015)\r\nThis map is the copy of 1:20000 Formal Map published by Geospatial Information Authority of Japan (GSI) with its approval under the article 29 of The Survey Act. (Approval Number ZYOU-FUKU No.1224 2015)")},
	{UTF8STRCPTR("KBN25000ANF-900913"), UTF8STRCPTR("Japan"), UTF8STRCPTR("AGINFO Naro"), MapType::TMS, 2, UTF8STRCPTR("https://aginfo.cgk.affrc.go.jp/ws/tmc/1.0.0/KBN25000ANF-900913/tilemapresource.xml"),
		UTF8STRCPTR("Fundamental Geospatial Data 25000\r\nAll are drawn. Usual width lines. Filled administrative areas.\r\n\r\nThis service uses Fundamental Geospatial Data published by Geospatial Information Authority of Japan (GSI) with its approval under the article 30 of The Survey Act. (Approval Number ZYOU-SHI No.1028 2015)\r\nThis map is the copy of 1:20000 Formal Map published by Geospatial Information Authority of Japan (GSI) with its approval under the article 29 of The Survey Act. (Approval Number ZYOU-FUKU No.1224 2015)")},
	{UTF8STRCPTR("KBN25000ANF-900913-L"), UTF8STRCPTR("Japan"), UTF8STRCPTR("AGINFO Naro"), MapType::TMS, 2, UTF8STRCPTR("https://aginfo.cgk.affrc.go.jp/ws/tmc/1.0.0/KBN25000ANF-900913-L/tilemapresource.xml"),
		UTF8STRCPTR("Fundamental Geospatial Data 25000\r\nAll are drawn. Usual width lines. Filled administrative areas.\r\n\r\nThis service uses Fundamental Geospatial Data published by Geospatial Information Authority of Japan (GSI) with its approval under the article 30 of The Survey Act. (Approval Number ZYOU-SHI No.1028 2015)\r\nThis map is the copy of 1:20000 Formal Map published by Geospatial Information Authority of Japan (GSI) with its approval under the article 29 of The Survey Act. (Approval Number ZYOU-FUKU No.1224 2015)")},
	{UTF8STRCPTR("KBN25000BBB-4326"), UTF8STRCPTR("Japan"), UTF8STRCPTR("AGINFO Naro"), MapType::TMS, 2, UTF8STRCPTR("https://aginfo.cgk.affrc.go.jp/ws/tmc/1.0.0/KBN25000BBB-4326/tilemapresource.xml"),
		UTF8STRCPTR("Fundamental Geospatial Data 25000\r\nExcepting contours. Thick lines. Not filled administrative area (with only boundaries).\r\n\r\nThis service uses Fundamental Geospatial Data published by Geospatial Information Authority of Japan (GSI) with its approval under the article 30 of The Survey Act. (Approval Number ZYOU-SHI No.1028 2015)\r\nThis map is the copy of 1:20000 Formal Map published by Geospatial Information Authority of Japan (GSI) with its approval under the article 29 of The Survey Act. (Approval Number ZYOU-FUKU No.1224 2015)")},
	{UTF8STRCPTR("KBN25000BBB-900913"), UTF8STRCPTR("Japan"), UTF8STRCPTR("AGINFO Naro"), MapType::TMS, 2, UTF8STRCPTR("https://aginfo.cgk.affrc.go.jp/ws/tmc/1.0.0/KBN25000BBB-900913/tilemapresource.xml"),
		UTF8STRCPTR("Fundamental Geospatial Data 25000\r\nExcepting contours. Thick lines. Not filled administrative area (with only boundaries).\r\n\r\nThis service uses Fundamental Geospatial Data published by Geospatial Information Authority of Japan (GSI) with its approval under the article 30 of The Survey Act. (Approval Number ZYOU-SHI No.1028 2015)\r\nThis map is the copy of 1:20000 Formal Map published by Geospatial Information Authority of Japan (GSI) with its approval under the article 29 of The Survey Act. (Approval Number ZYOU-FUKU No.1224 2015)")},
	{UTF8STRCPTR("KBN25000BBB-900913-L"), UTF8STRCPTR("Japan"), UTF8STRCPTR("AGINFO Naro"), MapType::TMS, 2, UTF8STRCPTR("https://aginfo.cgk.affrc.go.jp/ws/tmc/1.0.0/KBN25000BBB-900913-L/tilemapresource.xml"),
		UTF8STRCPTR("Fundamental Geospatial Data 25000\r\nExcepting contours. Thick lines. Not filled administrative area (with only boundaries).\r\n\r\nThis service uses Fundamental Geospatial Data published by Geospatial Information Authority of Japan (GSI) with its approval under the article 30 of The Survey Act. (Approval Number ZYOU-SHI No.1028 2015)\r\nThis map is the copy of 1:20000 Formal Map published by Geospatial Information Authority of Japan (GSI) with its approval under the article 29 of The Survey Act. (Approval Number ZYOU-FUKU No.1224 2015)")},
	{UTF8STRCPTR("KBN25000CNB-4326"), UTF8STRCPTR("Japan"), UTF8STRCPTR("AGINFO Naro"), MapType::TMS, 2, UTF8STRCPTR("https://aginfo.cgk.affrc.go.jp/ws/tmc/1.0.0/KBN25000CNB-4326/tilemapresource.xml"),
		UTF8STRCPTR("Fundamental Geospatial Data 25000\r\nContours. Usual width lines.\r\n\r\nThis service uses Fundamental Geospatial Data published by Geospatial Information Authority of Japan (GSI) with its approval under the article 30 of The Survey Act. (Approval Number ZYOU-SHI No.1028 2015)\r\nThis map is the copy of 1:20000 Formal Map published by Geospatial Information Authority of Japan (GSI) with its approval under the article 29 of The Survey Act. (Approval Number ZYOU-FUKU No.1224 2015)")},
	{UTF8STRCPTR("KBN25000CNB-900913"), UTF8STRCPTR("Japan"), UTF8STRCPTR("AGINFO Naro"), MapType::TMS, 2, UTF8STRCPTR("https://aginfo.cgk.affrc.go.jp/ws/tmc/1.0.0/KBN25000CNB-900913/tilemapresource.xml"),
		UTF8STRCPTR("Fundamental Geospatial Data 25000\r\nContours. Usual width lines.\r\n\r\nThis service uses Fundamental Geospatial Data published by Geospatial Information Authority of Japan (GSI) with its approval under the article 30 of The Survey Act. (Approval Number ZYOU-SHI No.1028 2015)\r\nThis map is the copy of 1:20000 Formal Map published by Geospatial Information Authority of Japan (GSI) with its approval under the article 29 of The Survey Act. (Approval Number ZYOU-FUKU No.1224 2015)")},
	{UTF8STRCPTR("KBN25000CNB-900913-L"), UTF8STRCPTR("Japan"), UTF8STRCPTR("AGINFO Naro"), MapType::TMS, 2, UTF8STRCPTR("https://aginfo.cgk.affrc.go.jp/ws/tmc/1.0.0/KBN25000CNB-900913-L/tilemapresource.xml"),
		UTF8STRCPTR("Fundamental Geospatial Data 25000\r\nContours. Usual width lines.\r\n\r\nThis service uses Fundamental Geospatial Data published by Geospatial Information Authority of Japan (GSI) with its approval under the article 30 of The Survey Act. (Approval Number ZYOU-SHI No.1028 2015)\r\nThis map is the copy of 1:20000 Formal Map published by Geospatial Information Authority of Japan (GSI) with its approval under the article 29 of The Survey Act. (Approval Number ZYOU-FUKU No.1224 2015)")},
	{UTF8STRCPTR("KBN2500FN-900913"), UTF8STRCPTR("Japan"), UTF8STRCPTR("AGINFO Naro"), MapType::TMS, 2, UTF8STRCPTR("https://aginfo.cgk.affrc.go.jp/ws/tmc/1.0.0/KBN2500FN-900913/tilemapresource.xml"),
		UTF8STRCPTR("Fundamental Geospatial Data 2500\r\nAll figures are drawn (excepting building names). Usual width lines.\r\n\r\nThis service uses Fundamental Geospatial Data published by Geospatial Information Authority of Japan (GSI) with its approval under the article 30 of The Survey Act. (Approval Number ZYOU-SHI No.1028 2015)\r\nThis map is the copy of 1:20000 Formal Map published by Geospatial Information Authority of Japan (GSI) with its approval under the article 29 of The Survey Act. (Approval Number ZYOU-FUKU No.1224 2015)")},
	{UTF8STRCPTR("KBN2500FN-900913-L"), UTF8STRCPTR("Japan"), UTF8STRCPTR("AGINFO Naro"), MapType::TMS, 2, UTF8STRCPTR("https://aginfo.cgk.affrc.go.jp/ws/tmc/1.0.0/KBN2500FN-900913-L/tilemapresource.xml"),
		UTF8STRCPTR("Fundamental Geospatial Data 2500\r\nAll figures are drawn (excepting building names). Usual width lines.\r\n\r\nThis service uses Fundamental Geospatial Data published by Geospatial Information Authority of Japan (GSI) with its approval under the article 30 of The Survey Act. (Approval Number ZYOU-SHI No.1028 2015)\r\nThis map is the copy of 1:20000 Formal Map published by Geospatial Information Authority of Japan (GSI) with its approval under the article 29 of The Survey Act. (Approval Number ZYOU-FUKU No.1224 2015)")},
	{UTF8STRCPTR("KBN2500M-900913"), UTF8STRCPTR("Japan"), UTF8STRCPTR("AGINFO Naro"), MapType::TMS, 2, UTF8STRCPTR("https://aginfo.cgk.affrc.go.jp/ws/tmc/1.0.0/KBN2500M-900913/tilemapresource.xml"),
		UTF8STRCPTR("Fundamental Geospatial Data 2500 Avialable Marks\r\nBlack pixels where \"KBN2500FN\" tile exists. This is instead of \"KBN2500FN\" which are invisible on small scale.\r\n\r\nThis service uses Fundamental Geospatial Data published by Geospatial Information Authority of Japan (GSI) with its approval under the article 30 of The Survey Act. (Approval Number ZYOU-SHI No.1028 2015)\r\nThis map is the copy of 1:20000 Formal Map published by Geospatial Information Authority of Japan (GSI) with its approval under the article 29 of The Survey Act. (Approval Number ZYOU-FUKU No.1224 2015)")},
	{UTF8STRCPTR("KBN2500M-900913-L"), UTF8STRCPTR("Japan"), UTF8STRCPTR("AGINFO Naro"), MapType::TMS, 2, UTF8STRCPTR("https://aginfo.cgk.affrc.go.jp/ws/tmc/1.0.0/KBN2500M-900913-L/tilemapresource.xml"),
		UTF8STRCPTR("Fundamental Geospatial Data 2500 Avialable Marks\r\nBlack pixels where \"KBN2500FN\" tile exists. This is instead of \"KBN2500FN\" which are invisible on small scale.\r\n\r\nThis service uses Fundamental Geospatial Data published by Geospatial Information Authority of Japan (GSI) with its approval under the article 30 of The Survey Act. (Approval Number ZYOU-SHI No.1028 2015)\r\nThis map is the copy of 1:20000 Formal Map published by Geospatial Information Authority of Japan (GSI) with its approval under the article 29 of The Survey Act. (Approval Number ZYOU-FUKU No.1224 2015)")},
	{UTF8STRCPTR("pntms-4326"), UTF8STRCPTR("Japan"), UTF8STRCPTR("AGINFO Naro"), MapType::TMS, 2, UTF8STRCPTR("https://aginfo.cgk.affrc.go.jp/ws/tmc/1.0.0/pntms-4326/tilemapresource.xml"),
		UTF8STRCPTR("Place Name\r\n\r\nThis service uses Fundamental Geospatial Data published by Geospatial Information Authority of Japan (GSI) with its approval under the article 30 of The Survey Act. (Approval Number ZYOU-SHI No.1028 2015)\r\nThis map is the copy of 1:20000 Formal Map published by Geospatial Information Authority of Japan (GSI) with its approval under the article 29 of The Survey Act. (Approval Number ZYOU-FUKU No.1224 2015)")},
	{UTF8STRCPTR("pntms-900913"), UTF8STRCPTR("Japan"), UTF8STRCPTR("AGINFO Naro"), MapType::TMS, 2, UTF8STRCPTR("https://aginfo.cgk.affrc.go.jp/ws/tmc/1.0.0/pntms-900913/tilemapresource.xml"),
		UTF8STRCPTR("Place Name\r\n\r\nThis service uses Fundamental Geospatial Data published by Geospatial Information Authority of Japan (GSI) with its approval under the article 30 of The Survey Act. (Approval Number ZYOU-SHI No.1028 2015)\r\nThis map is the copy of 1:20000 Formal Map published by Geospatial Information Authority of Japan (GSI) with its approval under the article 29 of The Survey Act. (Approval Number ZYOU-FUKU No.1224 2015)")},
	{UTF8STRCPTR("pntms-900913-L"), UTF8STRCPTR("Japan"), UTF8STRCPTR("AGINFO Naro"), MapType::TMS, 2, UTF8STRCPTR("https://aginfo.cgk.affrc.go.jp/ws/tmc/1.0.0/pntms-900913-L/tilemapresource.xml"),
		UTF8STRCPTR("Place Name\r\n\r\nThis service uses Fundamental Geospatial Data published by Geospatial Information Authority of Japan (GSI) with its approval under the article 30 of The Survey Act. (Approval Number ZYOU-SHI No.1028 2015)\r\nThis map is the copy of 1:20000 Formal Map published by Geospatial Information Authority of Japan (GSI) with its approval under the article 29 of The Survey Act. (Approval Number ZYOU-FUKU No.1224 2015)")},
	{UTF8STRCPTR("Tokyo5000-4326"), UTF8STRCPTR("Japan"), UTF8STRCPTR("AGINFO Naro"), MapType::TMS, 2, UTF8STRCPTR("https://aginfo.cgk.affrc.go.jp/ws/tmc/1.0.0/Tokyo5000-4326/tilemapresource.xml"),
		UTF8STRCPTR("Historical Agro-Environment (Kanto)\r\n\r\nThis service uses Fundamental Geospatial Data published by Geospatial Information Authority of Japan (GSI) with its approval under the article 30 of The Survey Act. (Approval Number ZYOU-SHI No.1028 2015)\r\nThis map is the copy of 1:20000 Formal Map published by Geospatial Information Authority of Japan (GSI) with its approval under the article 29 of The Survey Act. (Approval Number ZYOU-FUKU No.1224 2015)")},
	{UTF8STRCPTR("Tokyo5000-900913"), UTF8STRCPTR("Japan"), UTF8STRCPTR("AGINFO Naro"), MapType::TMS, 2, UTF8STRCPTR("https://aginfo.cgk.affrc.go.jp/ws/tmc/1.0.0/Tokyo5000-900913/tilemapresource.xml"),
		UTF8STRCPTR("Historical Agro-Environment (Kanto)\r\n\r\nThis service uses Fundamental Geospatial Data published by Geospatial Information Authority of Japan (GSI) with its approval under the article 30 of The Survey Act. (Approval Number ZYOU-SHI No.1028 2015)\r\nThis map is the copy of 1:20000 Formal Map published by Geospatial Information Authority of Japan (GSI) with its approval under the article 29 of The Survey Act. (Approval Number ZYOU-FUKU No.1224 2015)")},
	{UTF8STRCPTR("Tokyo5000-900913-L"), UTF8STRCPTR("Japan"), UTF8STRCPTR("AGINFO Naro"), MapType::TMS, 2, UTF8STRCPTR("https://aginfo.cgk.affrc.go.jp/ws/tmc/1.0.0/Tokyo5000-900913-L/tilemapresource.xml"),
		UTF8STRCPTR("Historical Agro-Environment (Kanto)\r\n\r\nThis service uses Fundamental Geospatial Data published by Geospatial Information Authority of Japan (GSI) with its approval under the article 30 of The Survey Act. (Approval Number ZYOU-SHI No.1028 2015)\r\nThis map is the copy of 1:20000 Formal Map published by Geospatial Information Authority of Japan (GSI) with its approval under the article 29 of The Survey Act. (Approval Number ZYOU-FUKU No.1224 2015)")},
	{UTF8STRCPTR("Kanto_Rapid-4326"), UTF8STRCPTR("Japan"), UTF8STRCPTR("AGINFO Naro"), MapType::TMS, 2, UTF8STRCPTR("https://aginfo.cgk.affrc.go.jp/ws/tmc/1.0.0/Kanto_Rapid-4326/tilemapresource.xml"),
		UTF8STRCPTR("Historical Agro-Environment (Kanto)\r\n\r\nThis service uses Fundamental Geospatial Data published by Geospatial Information Authority of Japan (GSI) with its approval under the article 30 of The Survey Act. (Approval Number ZYOU-SHI No.1028 2015)\r\nThis map is the copy of 1:20000 Formal Map published by Geospatial Information Authority of Japan (GSI) with its approval under the article 29 of The Survey Act. (Approval Number ZYOU-FUKU No.1224 2015)")},
	{UTF8STRCPTR("Kanto_Rapid-900913"), UTF8STRCPTR("Japan"), UTF8STRCPTR("AGINFO Naro"), MapType::TMS, 2, UTF8STRCPTR("https://aginfo.cgk.affrc.go.jp/ws/tmc/1.0.0/Kanto_Rapid-900913/tilemapresource.xml"),
		UTF8STRCPTR("Historical Agro-Environment (Kanto)\r\n\r\nThis service uses Fundamental Geospatial Data published by Geospatial Information Authority of Japan (GSI) with its approval under the article 30 of The Survey Act. (Approval Number ZYOU-SHI No.1028 2015)\r\nThis map is the copy of 1:20000 Formal Map published by Geospatial Information Authority of Japan (GSI) with its approval under the article 29 of The Survey Act. (Approval Number ZYOU-FUKU No.1224 2015)")},
	{UTF8STRCPTR("Kanto_Rapid-900913-L"), UTF8STRCPTR("Japan"), UTF8STRCPTR("AGINFO Naro"), MapType::TMS, 2, UTF8STRCPTR("https://aginfo.cgk.affrc.go.jp/ws/tmc/1.0.0/Kanto_Rapid-900913-L/tilemapresource.xml"),
		UTF8STRCPTR("Historical Agro-Environment (Kanto)\r\n\r\nThis service uses Fundamental Geospatial Data published by Geospatial Information Authority of Japan (GSI) with its approval under the article 30 of The Survey Act. (Approval Number ZYOU-SHI No.1028 2015)\r\nThis map is the copy of 1:20000 Formal Map published by Geospatial Information Authority of Japan (GSI) with its approval under the article 29 of The Survey Act. (Approval Number ZYOU-FUKU No.1224 2015)")},
	{UTF8STRCPTR("Fukuyama_1899-4326"), UTF8STRCPTR("Japan"), UTF8STRCPTR("AGINFO Naro"), MapType::TMS, 2, UTF8STRCPTR("https://aginfo.cgk.affrc.go.jp/ws/tmc/1.0.0/Fukuyama_1899-4326/tilemapresource.xml"),
		UTF8STRCPTR("Historical Agro-Environment (Fukuyama)\r\n\r\nThis service uses Fundamental Geospatial Data published by Geospatial Information Authority of Japan (GSI) with its approval under the article 30 of The Survey Act. (Approval Number ZYOU-SHI No.1028 2015)\r\nThis map is the copy of 1:20000 Formal Map published by Geospatial Information Authority of Japan (GSI) with its approval under the article 29 of The Survey Act. (Approval Number ZYOU-FUKU No.1224 2015)")},
	{UTF8STRCPTR("Fukuyama_1899-900913"), UTF8STRCPTR("Japan"), UTF8STRCPTR("AGINFO Naro"), MapType::TMS, 2, UTF8STRCPTR("https://aginfo.cgk.affrc.go.jp/ws/tmc/1.0.0/Fukuyama_1899-900913/tilemapresource.xml"),
		UTF8STRCPTR("Historical Agro-Environment (Fukuyama)\r\n\r\nThis service uses Fundamental Geospatial Data published by Geospatial Information Authority of Japan (GSI) with its approval under the article 30 of The Survey Act. (Approval Number ZYOU-SHI No.1028 2015)\r\nThis map is the copy of 1:20000 Formal Map published by Geospatial Information Authority of Japan (GSI) with its approval under the article 29 of The Survey Act. (Approval Number ZYOU-FUKU No.1224 2015)")},
	{UTF8STRCPTR("Fukuyama_1899-900913-L"), UTF8STRCPTR("Japan"), UTF8STRCPTR("AGINFO Naro"), MapType::TMS, 2, UTF8STRCPTR("https://aginfo.cgk.affrc.go.jp/ws/tmc/1.0.0/Fukuyama_1899-900913-L/tilemapresource.xml"),
		UTF8STRCPTR("Historical Agro-Environment (Fukuyama)\r\n\r\nThis service uses Fundamental Geospatial Data published by Geospatial Information Authority of Japan (GSI) with its approval under the article 30 of The Survey Act. (Approval Number ZYOU-SHI No.1028 2015)\r\nThis map is the copy of 1:20000 Formal Map published by Geospatial Information Authority of Japan (GSI) with its approval under the article 29 of The Survey Act. (Approval Number ZYOU-FUKU No.1224 2015)")},
	{UTF8STRCPTR("KSJ_Transportation-900913"), UTF8STRCPTR("Japan"), UTF8STRCPTR("AGINFO Naro"), MapType::TMS, 2, UTF8STRCPTR("https://aginfo.cgk.affrc.go.jp/ws/tmc/1.0.0/KSJ_Transportation-900913/tilemapresource.xml"),
		UTF8STRCPTR("National Land Numerical Information\r\n\r\nThis service uses Fundamental Geospatial Data published by Geospatial Information Authority of Japan (GSI) with its approval under the article 30 of The Survey Act. (Approval Number ZYOU-SHI No.1028 2015)\r\nThis map is the copy of 1:20000 Formal Map published by Geospatial Information Authority of Japan (GSI) with its approval under the article 29 of The Survey Act. (Approval Number ZYOU-FUKU No.1224 2015)")},
	{UTF8STRCPTR("KSJ_Transportation-900913-L"), UTF8STRCPTR("Japan"), UTF8STRCPTR("AGINFO Naro"), MapType::TMS, 2, UTF8STRCPTR("https://aginfo.cgk.affrc.go.jp/ws/tmc/1.0.0/KSJ_Transportation-900913-L/tilemapresource.xml"),
		UTF8STRCPTR("National Land Numerical Information\r\n\r\nThis service uses Fundamental Geospatial Data published by Geospatial Information Authority of Japan (GSI) with its approval under the article 30 of The Survey Act. (Approval Number ZYOU-SHI No.1028 2015)\r\nThis map is the copy of 1:20000 Formal Map published by Geospatial Information Authority of Japan (GSI) with its approval under the article 29 of The Survey Act. (Approval Number ZYOU-FUKU No.1224 2015)")},
	{UTF8STRCPTR("KSJ_Water-900913"), UTF8STRCPTR("Japan"), UTF8STRCPTR("AGINFO Naro"), MapType::TMS, 2, UTF8STRCPTR("https://aginfo.cgk.affrc.go.jp/ws/tmc/1.0.0/KSJ_Water-900913/tilemapresource.xml"),
		UTF8STRCPTR("National Land Numerical Information\r\n\r\nThis service uses Fundamental Geospatial Data published by Geospatial Information Authority of Japan (GSI) with its approval under the article 30 of The Survey Act. (Approval Number ZYOU-SHI No.1028 2015)\r\nThis map is the copy of 1:20000 Formal Map published by Geospatial Information Authority of Japan (GSI) with its approval under the article 29 of The Survey Act. (Approval Number ZYOU-FUKU No.1224 2015)")},
	{UTF8STRCPTR("KSJ_Water-900913-L"), UTF8STRCPTR("Japan"), UTF8STRCPTR("AGINFO Naro"), MapType::TMS, 2, UTF8STRCPTR("https://aginfo.cgk.affrc.go.jp/ws/tmc/1.0.0/KSJ_Water-900913-L/tilemapresource.xml"),
		UTF8STRCPTR("National Land Numerical Information\r\n\r\nThis service uses Fundamental Geospatial Data published by Geospatial Information Authority of Japan (GSI) with its approval under the article 30 of The Survey Act. (Approval Number ZYOU-SHI No.1028 2015)\r\nThis map is the copy of 1:20000 Formal Map published by Geospatial Information Authority of Japan (GSI) with its approval under the article 29 of The Survey Act. (Approval Number ZYOU-FUKU No.1224 2015)")},
	//https://tiles.gsj.jp/tiles/elev/tiles.html
	{UTF8STRCPTR("Overall Elevation Model"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geological Survey of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://tiles.gsj.jp/tiles/elev/mixed/{z}/{y}/{x}.png"),
		UTF8STRCPTR(""),
		0, 17, 122.9, 20.4, 154.0, 45.6},
	{UTF8STRCPTR("Hyogo DSM"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geological Survey of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://tiles.gsj.jp/tiles/elev/hyogodsm/{z}/{y}/{x}.png"),
		UTF8STRCPTR(""),
		2, 17, 134.24587, 34.14648, 135.47185, 35.67539},
	{UTF8STRCPTR("Hyogo DEM"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geological Survey of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://tiles.gsj.jp/tiles/elev/hyogodem/{z}/{y}/{x}.png"),
		UTF8STRCPTR(""),
		2, 17, 134.24587, 34.14648, 135.47185, 35.67539},
	{UTF8STRCPTR("GSI DEM10B"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geological Survey of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://tiles.gsj.jp/tiles/elev/gsidem/{z}/{y}/{x}.png"),
		UTF8STRCPTR(""),
		0, 14, 122.9, 20.4, 154.0, 45.6},
	{UTF8STRCPTR("GSI DEM5A"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geological Survey of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://tiles.gsj.jp/tiles/elev/gsidem5a/{z}/{y}/{x}.png"),
		UTF8STRCPTR(""),
		0, 15, 122.9, 20.4, 154.0, 45.6},
	{UTF8STRCPTR("GSI DEM5B"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geological Survey of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://tiles.gsj.jp/tiles/elev/gsidem5b/{z}/{y}/{x}.png"),
		UTF8STRCPTR(""),
		0, 15, 122.9, 20.4, 154.0, 45.6},
	{UTF8STRCPTR("GSI DEM5C"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geological Survey of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://tiles.gsj.jp/tiles/elev/gsidem5c/{z}/{y}/{x}.png"),
		UTF8STRCPTR(""),
		0, 15, 122.9, 20.4, 154.0, 45.6},
	{UTF8STRCPTR("GSI Nishinojima 20181201"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geological Survey of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://tiles.gsj.jp/tiles/elev/gsi20181201dd5/{z}/{y}/{x}.png"),
		UTF8STRCPTR(""),
		12, 15, 140.8667, 27.23509, 140.89966, 27.25463},
	{UTF8STRCPTR("GSI Nishinojima 20180117"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geological Survey of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://tiles.gsj.jp/tiles/elev/gsi20180117dd5/{z}/{y}/{x}.png"),
		UTF8STRCPTR(""),
		12, 15, 140.8667, 27.23509, 140.89966, 27.25463},
	{UTF8STRCPTR("GSI Nishinojima 20160725"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geological Survey of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://tiles.gsj.jp/tiles/elev/gsi20160725dd5/{z}/{y}/{x}.png"),
		UTF8STRCPTR(""),
		12, 15, 140.8667, 27.23509, 140.89966, 27.25463},
	{UTF8STRCPTR("GSI Nishinojima 20160303"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geological Survey of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://tiles.gsj.jp/tiles/elev/gsi20160303dd5/{z}/{y}/{x}.png"),
		UTF8STRCPTR(""),
		12, 15, 140.8667, 27.23509, 140.89966, 27.25463},
	{UTF8STRCPTR("GSI Nishinojima 20151209"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geological Survey of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://tiles.gsj.jp/tiles/elev/gsi20151209dd5/{z}/{y}/{x}.png"),
		UTF8STRCPTR(""),
		12, 15, 140.8667, 27.23509, 140.89966, 27.25463},
	{UTF8STRCPTR("GSI Nishinojima 20150728"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geological Survey of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://tiles.gsj.jp/tiles/elev/gsi20150728dd5/{z}/{y}/{x}.png"),
		UTF8STRCPTR(""),
		12, 15, 140.8667, 27.23509, 140.89966, 27.25463},
	{UTF8STRCPTR("GSI Nishinojima 20150301"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geological Survey of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://tiles.gsj.jp/tiles/elev/gsi20150301dd5/{z}/{y}/{x}.png"),
		UTF8STRCPTR(""),
		12, 15, 140.8667, 27.23509, 140.89966, 27.25463},
	{UTF8STRCPTR("GSI Nishinojima 20141204"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geological Survey of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://tiles.gsj.jp/tiles/elev/gsi20141204dd5/{z}/{y}/{x}.png"),
		UTF8STRCPTR(""),
		12, 15, 140.8667, 27.23509, 140.89966, 27.25463},
	{UTF8STRCPTR("GSI Nishinojima 20140704"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geological Survey of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://tiles.gsj.jp/tiles/elev/gsi20140704dd5/{z}/{y}/{x}.png"),
		UTF8STRCPTR(""),
		12, 17, 140.8667, 27.23509, 140.89966, 27.25463},
	{UTF8STRCPTR("GSI Nishinojima 20140322"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geological Survey of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://tiles.gsj.jp/tiles/elev/gsi20140322dd5/{z}/{y}/{x}.png"),
		UTF8STRCPTR(""),
		12, 15, 140.8667, 27.23509, 140.89966, 27.25463},
	{UTF8STRCPTR("GSI Nishinojima 20140216"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geological Survey of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://tiles.gsj.jp/tiles/elev/gsi20140216dd5/{z}/{y}/{x}.png"),
		UTF8STRCPTR(""),
		12, 15, 140.8667, 27.23509, 140.89966, 27.25463},
	{UTF8STRCPTR("GSI Nishinojima 20131217"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geological Survey of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://tiles.gsj.jp/tiles/elev/gsi20131217dd5/{z}/{y}/{x}.png"),
		UTF8STRCPTR(""),
		12, 15, 140.8667, 27.23509, 140.89966, 27.25463},
	{UTF8STRCPTR("GSI Nishinojima 20131204"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geological Survey of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://tiles.gsj.jp/tiles/elev/gsi20131204dd5/{z}/{y}/{x}.png"),
		UTF8STRCPTR(""),
		12, 15, 140.8667, 27.23509, 140.89966, 27.25463},
	{UTF8STRCPTR("ASTER GDEM 003"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geological Survey of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://tiles.gsj.jp/tiles/elev/astergdemv3/{z}/{y}/{x}.png"),
		UTF8STRCPTR("ASTER Global Digital Elevation Map Announcement"),
		0, 12, -180, -85.0511, 180, 85.0511},
	{UTF8STRCPTR("Japan 250m Mesh"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geological Survey of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://tiles.gsj.jp/tiles/elev/japan250/{z}/{y}/{x}.png"),
		UTF8STRCPTR(""),
		0, 8, 122, 24, 148, 46},
	{UTF8STRCPTR("GEBCO Grid"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geological Survey of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://tiles.gsj.jp/tiles/elev/gebco/{z}/{y}/{x}.png"),
		UTF8STRCPTR("GEBCO Gridded Bathymetry Data"),
		0, 9, -180, -85.0511, 180, 85.0511},
	{UTF8STRCPTR("GSI DEM GM"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geological Survey of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://tiles.gsj.jp/tiles/elev/gsidemgm/{z}/{y}/{x}.png"),
		UTF8STRCPTR(""),
		0, 8, -180, -85.0511, 180, 85.0511},
	// https://maps.gsi.go.jp/development/ichiran.html
	{UTF8STRCPTR("National Base Map"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geospatial Information Authority of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://cyberjapandata.gsi.go.jp/xyz/std/{z}/{x}/{y}.png"),
		UTF8STRCPTR("Source: Geospatial Information Authority of Japan, Map Tile, National Base Map\r\nThe bathymetric contours are derived from those contained within the GEBCO Digital Atlas, published by the BODC on behalf of IOC and IHO (2003) (https://www.gebco.net)\r\nShoreline data is derived from: United States. National Imagery and Mapping Agency. \"Vector Map Level 0 (VMAP0).\" Bethesda, MD: Denver, CO: The Agency; USGS Information Services, 1997"),
		0, 18, 122.9, 20.4, 154.0, 45.6},
	{UTF8STRCPTR("National Base Map (Pale)"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geospatial Information Authority of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://cyberjapandata.gsi.go.jp/xyz/pale/{z}/{x}/{y}.png"),
		UTF8STRCPTR("Source: Geospatial Information Authority of Japan, Map Tile, National Base Map\r\nShoreline data is derived from: United States. National Imagery and Mapping Agency. \"Vector Map Level 0 (VMAP0).\" Bethesda, MD: Denver, CO: The Agency; USGS Information Services, 1997"),
		0, 18, 122.9, 20.4, 154.0, 45.6},
	{UTF8STRCPTR("National Base Map (English)"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geospatial Information Authority of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://cyberjapandata.gsi.go.jp/xyz/english/{z}/{x}/{y}.png"),
		UTF8STRCPTR("Source: Geospatial Information Authority of Japan, Map Tile, National Base Map\r\nThe bathymetric contours are derived from those contained within the GEBCO Digital Atlas, published by the BODC on behalf of IOC and IHO (2003) (https://www.gebco.net)\r\nShoreline data is derived from: United States. National Imagery and Mapping Agency. \"Vector Map Level 0 (VMAP0).\" Bethesda, MD: Denver, CO: The Agency; USGS Information Services, 1997"),
		0, 18, 122.9, 20.4, 154.0, 45.6},
	{UTF8STRCPTR("Digital Map 25000 (Land Condition)"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geospatial Information Authority of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://cyberjapandata.gsi.go.jp/xyz/lcm25k_2012/{z}/{x}/{y}.png"),
		UTF8STRCPTR("Source: Geospatial Information Authority of Japan, Map Tile, Digital Map 25000 (Land Condition)"),
		10, 16, 122.9, 20.4, 154.0, 45.6},
	{UTF8STRCPTR("Coastal Land Condition new"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geospatial Information Authority of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://cyberjapandata.gsi.go.jp/xyz/ccm1/{z}/{x}/{y}.png"),
		UTF8STRCPTR("Source: Geospatial Information Authority of Japan, Map Tile, Coastal Land Condition new"),
		14, 16, 122.9, 20.4, 154.0, 45.6},
	{UTF8STRCPTR("Coastal Land Condition old"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geospatial Information Authority of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://cyberjapandata.gsi.go.jp/xyz/ccm2/{z}/{x}/{y}.png"),
		UTF8STRCPTR("Source: Geospatial Information Authority of Japan, Map Tile, Coastal Land Condition old"),
		14, 16, 122.9, 20.4, 154.0, 45.6},
	{UTF8STRCPTR("Vocano Base Map"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geospatial Information Authority of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://cyberjapandata.gsi.go.jp/xyz/vbm/{z}/{x}/{y}.png"),
		UTF8STRCPTR("Source: Geospatial Information Authority of Japan, Map Tile, Vocano Base Map"),
		11, 17, 122.9, 20.4, 154.0, 45.6},
	{UTF8STRCPTR("Vocano Base Map Data (Base Map)"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geospatial Information Authority of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://cyberjapandata.gsi.go.jp/xyz/vbmd_bm/{z}/{x}/{y}.png"),
		UTF8STRCPTR("Source: Geospatial Information Authority of Japan, Map Tile, Vocano Base Map Data (Base Map)"),
		11, 18, 122.9, 20.4, 154.0, 45.6},
	{UTF8STRCPTR("Vocano Base Map Data (Color Relief)"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geospatial Information Authority of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://cyberjapandata.gsi.go.jp/xyz/vbmd_colorrel/{z}/{x}/{y}.png"),
		UTF8STRCPTR("Source: Geospatial Information Authority of Japan, Map Tile, Vocano Base Map Data (Color Relief)"),
		11, 18, 122.9, 20.4, 154.0, 45.6},
	{UTF8STRCPTR("Vocano Base Map Data (Photo Map)"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geospatial Information Authority of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://cyberjapandata.gsi.go.jp/xyz/vbmd_pm/{z}/{x}/{y}.png"),
		UTF8STRCPTR("Source: Geospatial Information Authority of Japan, Map Tile, Vocano Base Map Data (Photo Map)"),
		11, 18, 122.9, 20.4, 154.0, 45.6},
	{UTF8STRCPTR("Land Usage Map 20000"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geospatial Information Authority of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://cyberjapandata.gsi.go.jp/xyz/lum200k/{z}/{x}/{y}.png"),
		UTF8STRCPTR("Source: Geospatial Information Authority of Japan, Map Tile, Land Usage Map 20000"),
		11, 14, 122.9, 20.4, 154.0, 45.6},
	{UTF8STRCPTR("Lake Map"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geospatial Information Authority of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://cyberjapandata.gsi.go.jp/xyz/lake1/{z}/{x}/{y}.png"),
		UTF8STRCPTR("Source: Geospatial Information Authority of Japan, Map Tile, Lake Map"),
		11, 17, 122.9, 20.4, 154.0, 45.6},
	{UTF8STRCPTR("Lake Map Data"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geospatial Information Authority of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://cyberjapandata.gsi.go.jp/xyz/lakedata/{z}/{x}/{y}.png"),
		UTF8STRCPTR("Source: Geospatial Information Authority of Japan, Map Tile, Lake Map Data"),
		11, 17, 122.9, 20.4, 154.0, 45.6},
	{UTF8STRCPTR("Blank Map"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geospatial Information Authority of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://cyberjapandata.gsi.go.jp/xyz/blank/{z}/{x}/{y}.png"),
		UTF8STRCPTR("Source: Geospatial Information Authority of Japan, Map Tile, Blank Map"),
		5, 14, 122.9, 20.4, 154.0, 45.6},
	{UTF8STRCPTR("Seamless Photo Map"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geospatial Information Authority of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://cyberjapandata.gsi.go.jp/xyz/seamlessphoto/{z}/{x}/{y}.jpg"),
		UTF8STRCPTR("Source: Geospatial Information Authority of Japan, Map Tile, Seamless Photo Map\r\nData Source: NASA/USGS Landsat-8, Landsat8 Image (GSI,TSIC,GEO Grid/AIST), Landsat8 Image (courtesy of the U.S. Geological Survey), Marine Landscape (GEBCO)\r\nobtained from site https://lpdaac.usgs.gov/data_access maintained by the NASA Land Processes Distributed Active Archive Center (LP DAAC), USGS/Earth Resources Observation and Science (EROS) Center, Sioux Falls, South Dakota, (Year). Source of image data product."),
		0, 18, -180, -85.0511, 180, 85.0511},
	{UTF8STRCPTR("Old Photo (1987-1990)"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geospatial Information Authority of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://cyberjapandata.gsi.go.jp/xyz/gazo4/{z}/{x}/{y}.jpg"),
		UTF8STRCPTR("Source: Geospatial Information Authority of Japan, Map Tile, Old Photo (1987-1990)"),
		10, 17, 122.9, 20.4, 154.0, 45.6},
	{UTF8STRCPTR("Old Photo (1984-1986)"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geospatial Information Authority of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://cyberjapandata.gsi.go.jp/xyz/gazo3/{z}/{x}/{y}.jpg"),
		UTF8STRCPTR("Source: Geospatial Information Authority of Japan, Map Tile, Old Photo (1984-1986)"),
		10, 17, 122.9, 20.4, 154.0, 45.6},
	{UTF8STRCPTR("Old Photo (1979-1983)"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geospatial Information Authority of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://cyberjapandata.gsi.go.jp/xyz/gazo2/{z}/{x}/{y}.jpg"),
		UTF8STRCPTR("Source: Geospatial Information Authority of Japan, Map Tile, Old Photo (1979-1983)"),
		10, 17, 122.9, 20.4, 154.0, 45.6},
	{UTF8STRCPTR("Old Photo (1974-1978)"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geospatial Information Authority of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://cyberjapandata.gsi.go.jp/xyz/gazo1/{z}/{x}/{y}.jpg"),
		UTF8STRCPTR("Source: Geospatial Information Authority of Japan, Map Tile, Old Photo (1974-1978)"),
		10, 17, 122.9, 20.4, 154.0, 45.6},
	{UTF8STRCPTR("Old Photo (1961-1969)"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geospatial Information Authority of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://cyberjapandata.gsi.go.jp/xyz/ort_old10/{z}/{x}/{y}.png"),
		UTF8STRCPTR("Source: Geospatial Information Authority of Japan, Map Tile, Old Photo (1961-1969)"),
		10, 17, 122.9, 20.4, 154.0, 45.6},
	{UTF8STRCPTR("Old Photo (1945-1950)"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geospatial Information Authority of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://cyberjapandata.gsi.go.jp/xyz/ort_USA10/{z}/{x}/{y}.png"),
		UTF8STRCPTR("Source: Geospatial Information Authority of Japan, Map Tile, Old Photo (1945-1950)"),
		10, 17, 122.9, 20.4, 154.0, 45.6},
	{UTF8STRCPTR("Old Photo (1936-1942)"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geospatial Information Authority of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://cyberjapandata.gsi.go.jp/xyz/ort_riku10/{z}/{x}/{y}.png"),
		UTF8STRCPTR("Source: Geospatial Information Authority of Japan, Map Tile, Old Photo (1936-1942)"),
		13, 18, 122.9, 20.4, 154.0, 45.6},
	{UTF8STRCPTR("Old Photo (1928)"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geospatial Information Authority of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://cyberjapandata.gsi.go.jp/xyz/ort_1928/{z}/{x}/{y}.png"),
		UTF8STRCPTR("Source: Geospatial Information Authority of Japan, Map Tile, Old Photo (1928)"),
		13, 18, 122.9, 20.4, 154.0, 45.6},
	{UTF8STRCPTR("National Base Map (Aerial Image)"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geospatial Information Authority of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://cyberjapandata.gsi.go.jp/xyz/ort/{z}/{x}/{y}.jpg"),
		UTF8STRCPTR("Source: Geospatial Information Authority of Japan, Map Tile, National Base Map (Aerial Image)"),
		13, 18, 122.9, 20.4, 154.0, 45.6},
	{UTF8STRCPTR("Air Photo"), UTF8STRCPTR("Japan"), UTF8STRCPTR("Geospatial Information Authority of Japan"), MapType::CustomTile, 0, UTF8STRCPTR("https://cyberjapandata.gsi.go.jp/xyz/airphoto/{z}/{x}/{y}.png"),
		UTF8STRCPTR("Source: Geospatial Information Authority of Japan, Map Tile, Air Photo"),
		5, 18, 122.9, 20.4, 154.0, 45.6},
	{UTF8STRCPTR("District boundary"), UTF8STRCPTR("Hong Kong"), UTF8STRCPTR("Hong Kong Home Affairs Department"), MapType::File, 0, UTF8STRCPTR("https://www.had.gov.hk/psi/hong-kong-administrative-boundaries/hksar_18_district_boundary.json"),
		UTF8STRCPTR("")},
	// https://www.hko.gov.hk
	{UTF8STRCPTR("Radar 64km"), UTF8STRCPTR("Hong Kong"), UTF8STRCPTR("Hong Kong Observatory"), MapType::File, 0, UTF8STRCPTR("https://www.hko.gov.hk/wxinfo/radars/radar_064_kml/Radar_064k.kml"),
		UTF8STRCPTR("")},
	{UTF8STRCPTR("Radar 128km"), UTF8STRCPTR("Hong Kong"), UTF8STRCPTR("Hong Kong Observatory"), MapType::File, 0, UTF8STRCPTR("https://www.hko.gov.hk/wxinfo/radars/radar_128_kml/Radar_128k.kml"),
		UTF8STRCPTR("")},
	{UTF8STRCPTR("Radar 256km"), UTF8STRCPTR("Hong Kong"), UTF8STRCPTR("Hong Kong Observatory"), MapType::File, 0, UTF8STRCPTR("https://www.hko.gov.hk/wxinfo/radars/radar_256_kml/Radar_256k.kml"),
		UTF8STRCPTR("")},
	{UTF8STRCPTR("Daily total rainfall"), UTF8STRCPTR("Hong Kong"), UTF8STRCPTR("Hong Kong Observatory"), MapType::WFS, 0, UTF8STRCPTR("https://portal.csdi.gov.hk/server/services/common/hko_rcd_1634958957456_52030/MapServer/WFSServer"),
		UTF8STRCPTR("")},
	{UTF8STRCPTR("Daily total rainfall"), UTF8STRCPTR("Hong Kong"), UTF8STRCPTR("Hong Kong Observatory"), MapType::WMS, 0, UTF8STRCPTR("https://portal.csdi.gov.hk/server/services/common/hko_rcd_1634958957456_52030/MapServer/WMSServer"),
		UTF8STRCPTR("")},
	{UTF8STRCPTR("Daily total rainfall"), UTF8STRCPTR("Hong Kong"), UTF8STRCPTR("Hong Kong Observatory"), MapType::ESRIMap, 0, UTF8STRCPTR("https://portal.csdi.gov.hk/server/rest/services/common/hko_rcd_1634958957456_52030/FeatureServer"),
		UTF8STRCPTR("")},
	{UTF8STRCPTR("Boundaries of New Towns (for 2006 Population By-census)"), UTF8STRCPTR("Hong Kong"), UTF8STRCPTR("Hong Kong Planning Department"), MapType::File, 0, UTF8STRCPTR("https://www.pland.gov.hk/open_data/newtowns/NewTown_2006.json"),
		UTF8STRCPTR("")},
	{UTF8STRCPTR("Boundaries of New Towns (for 2011 Population Census)"), UTF8STRCPTR("Hong Kong"), UTF8STRCPTR("Hong Kong Planning Department"), MapType::File, 0, UTF8STRCPTR("https://www.pland.gov.hk/open_data/newtowns/NewTown_2011.json"),
		UTF8STRCPTR("")},
	{UTF8STRCPTR("Boundaries of New Towns (for 2016 Population By-census)"), UTF8STRCPTR("Hong Kong"), UTF8STRCPTR("Hong Kong Planning Department"), MapType::File, 0, UTF8STRCPTR("https://www.pland.gov.hk/open_data/newtowns/12_New_Towns.geojson"),
		UTF8STRCPTR("")},
	{UTF8STRCPTR("Boundaries of New Towns (for 2021 Population Census)"), UTF8STRCPTR("Hong Kong"), UTF8STRCPTR("Hong Kong Planning Department"), MapType::File, 0, UTF8STRCPTR("https://www.pland.gov.hk/open_data/newtowns/NewTown_2021.geojson"),
		UTF8STRCPTR("")},
	{UTF8STRCPTR("2020 AA - Areas"), UTF8STRCPTR("Hong Kong"), UTF8STRCPTR("Hong Kong Planning Department"), MapType::File, 0, UTF8STRCPTR("https://www.pland.gov.hk/pland_en/p_study/comp_s/2020AA/spatial_data/2020_AA-Areas.geojson"),
		UTF8STRCPTR("The tables contain detailed survey findings of the 2020 Area Assessments of Industrial Land in the Territory.")},
	{UTF8STRCPTR("Route information of bus"), UTF8STRCPTR("Hong Kong"), UTF8STRCPTR("Hong Kong Transport Department"), MapType::File, 0, UTF8STRCPTR("https://static.data.gov.hk/td/routes-fares-geojson/JSON_BUS.json"),
		UTF8STRCPTR("")},
	{UTF8STRCPTR("Route information of GMB"), UTF8STRCPTR("Hong Kong"), UTF8STRCPTR("Hong Kong Transport Department"), MapType::File, 0, UTF8STRCPTR("https://static.data.gov.hk/td/routes-fares-geojson/JSON_GMB.json"),
		UTF8STRCPTR("")},
	{UTF8STRCPTR("Route information of ferry"), UTF8STRCPTR("Hong Kong"), UTF8STRCPTR("Hong Kong Transport Department"), MapType::File, 0, UTF8STRCPTR("https://static.data.gov.hk/td/routes-fares-geojson/JSON_FERRY.json"),
		UTF8STRCPTR("")},
	{UTF8STRCPTR("Route information of peak tram"), UTF8STRCPTR("Hong Kong"), UTF8STRCPTR("Hong Kong Transport Department"), MapType::File, 0, UTF8STRCPTR("https://static.data.gov.hk/td/routes-fares-geojson/JSON_PTRAM.json"),
		UTF8STRCPTR("")},
	{UTF8STRCPTR("Route information of tram"), UTF8STRCPTR("Hong Kong"), UTF8STRCPTR("Hong Kong Transport Department"), MapType::File, 0, UTF8STRCPTR("https://static.data.gov.hk/td/routes-fares-geojson/JSON_TRAM.json"),
		UTF8STRCPTR("")},
	{UTF8STRCPTR("Traffic Speed"), UTF8STRCPTR("Hong Kong"), UTF8STRCPTR("Hong Kong Transport Department"), MapType::ESRIMap, 2326, UTF8STRCPTR("https://www.map.gov.hk/arcgis2/rest/services/mbatsm_link__d00/MapServer"),
		UTF8STRCPTR("Information about the average traffic speed of major roads in Hong Kong as displayed in this app is provided by the Transport Department. This information is updated every 5 minutes.")},
	{UTF8STRCPTR("Auto Post Information"), UTF8STRCPTR("Hong Kong"), UTF8STRCPTR("Hongkong Post"), MapType::File, 0, UTF8STRCPTR("https://www.hongkongpost.hk/opendata/auto-post.json"),
		UTF8STRCPTR("Auto Post locations, opening hours and facility")},
	{UTF8STRCPTR("iPostal Kiosk Information"), UTF8STRCPTR("Hong Kong"), UTF8STRCPTR("Hongkong Post"), MapType::File, 0, UTF8STRCPTR("https://www.hongkongpost.hk/opendata/ipostal-kiosk.json"),
		UTF8STRCPTR("iPostal Kiosk locations and opening hours")},
	{UTF8STRCPTR("iPostal Station Information"), UTF8STRCPTR("Hong Kong"), UTF8STRCPTR("Hongkong Post"), MapType::File, 0, UTF8STRCPTR("https://www.hongkongpost.hk/opendata/ipostal-station.json"),
		UTF8STRCPTR("iPostal Station locations and opening hours")},
	{UTF8STRCPTR("Mobile Post Office Information"), UTF8STRCPTR("Hong Kong"), UTF8STRCPTR("Hongkong Post"), MapType::File, 0, UTF8STRCPTR("https://www.hongkongpost.hk/opendata/mobile-office.json"),
		UTF8STRCPTR("Mobile post office scheduled stopping time & location")},
	{UTF8STRCPTR("Post Office Information"), UTF8STRCPTR("Hong Kong"), UTF8STRCPTR("Hongkong Post"), MapType::File, 0, UTF8STRCPTR("https://www.hongkongpost.hk/opendata/post-office.json"),
		UTF8STRCPTR("Post office location and opening hours")},
	{UTF8STRCPTR("Street Posting Box Information"), UTF8STRCPTR("Hong Kong"), UTF8STRCPTR("Hongkong Post"), MapType::File, 0, UTF8STRCPTR("https://www.hongkongpost.hk/opendata/street-box.json"),
		UTF8STRCPTR("Street posting box locations and collection time")},
};

UnsafeArray<const Map::RegionalMapSource::MapInfo> Map::RegionalMapSource::GetMapInfos(OutParam<UOSInt> cnt)
{
	cnt.Set(sizeof(maps) / sizeof(maps[0]));
	return maps;
}

Optional<Map::MapDrawLayer> Map::RegionalMapSource::OpenMap(NN<const MapInfo> map, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact, NN<Parser::ParserList> parsers, NN<Net::WebBrowser> browser, NN<Math::CoordinateSystem> envCSys)
{
	Map::MapDrawLayer *layer;
	switch (map->mapType)
	{
	case MapType::TMS:
	{
		NN<Map::TileMapServiceSource> tms;
		NEW_CLASSNN(tms, Map::TileMapServiceSource(clif, ssl, encFact, Text::CStringNN(map->url, map->urlLen)));
		if (tms->IsError())
		{
			tms.Delete();
			return 0;
		}
		if (map->mapTypeParam != 0)
		{
			tms->SetConcurrentCount(map->mapTypeParam);
		}
		NEW_CLASS(layer, Map::TileMapLayer(tms, parsers));
		return layer;
	}
	case MapType::File:
	{
		NN<IO::StreamData> fd;
		if (browser->GetData(Text::CStringNN(map->url, map->urlLen), false, 0).SetTo(fd))
		{
			NN<IO::ParsedObject> pobj;
			if (parsers->ParseFile(fd).SetTo(pobj))
			{
				fd.Delete();
				if (pobj->GetParserType() == IO::ParserType::MapLayer)
				{
					return NN<Map::MapDrawLayer>::ConvertFrom(pobj);
				}
				pobj.Delete();
			}
			else
			{
				fd.Delete();
			}
		}
		return 0;
	}
	case MapType::CustomTile:
	{
		UnsafeArray<UTF8Char> sptr;
		UTF8Char sbuff[512];
		NN<Map::CustomTileMap> tileMap;
		sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("TileMap"));
		*sptr++ = IO::Path::PATH_SEPERATOR;
		Crypto::Hash::CRC32RC crc;
		sptr = Text::StrHexVal32(sptr, crc.CalcDirect(map->url, map->urlLen));
		NEW_CLASSNN(tileMap, Map::CustomTileMap(Text::CStringNN(map->url, map->urlLen), CSTRP(sbuff, sptr), map->minLevel, map->maxLevel, clif, ssl));
		tileMap->SetName(Text::CStringNN(map->name, map->nameLen));
		tileMap->SetBounds(Math::RectAreaDbl(Math::Coord2DDbl(map->boundsX1, map->boundsY1), Math::Coord2DDbl(map->boundsX2, map->boundsY2)));
		NEW_CLASS(layer, Map::TileMapLayer(tileMap, parsers));
		return layer;
	}
	case MapType::ESRIMap:
	{
		NN<Map::ESRI::ESRIMapServer> esriMap;
		NEW_CLASSNN(esriMap, Map::ESRI::ESRIMapServer(Text::CStringNN(map->url, map->urlLen), clif, ssl, map->mapTypeParam != 0));
		if (map->mapTypeParam != 0)
		{
			esriMap->SetSRID((UInt32)map->mapTypeParam);
		}
		NEW_CLASS(layer, Map::DrawMapServiceLayer(esriMap));
		return layer;
	}
	case MapType::WMS:
	{
		NN<Map::WebMapService> wms;
		NEW_CLASSNN(wms, Map::WebMapService(clif, ssl, encFact, Text::CStringNN(map->url, map->urlLen), Map::WebMapService::Version::ANY, envCSys));
		if (wms->IsError())
		{
			printf("RegionalMapSource: Error in loading wms layer\r\n");
			wms.Delete();
			return 0;
		}
		NEW_CLASS(layer, Map::DrawMapServiceLayer(wms));
		return layer;
	}
	case MapType::WFS:
	{
		Map::WebFeatureService wfs(clif, ssl, encFact, Text::CStringNN(map->url, map->urlLen), Map::WebFeatureService::Version::ANY);
		return wfs.LoadAsLayer();
	}
	default:
		return 0;
	}
}

Text::CStringNN Map::RegionalMapSource::MapTypeGetName(MapType val)
{
	switch (val)
	{
	case MapType::TMS:
		return CSTR("TMS");
	case MapType::File:
		return CSTR("File");
	case MapType::CustomTile:
		return CSTR("CustomTile");
	case MapType::ESRIMap:
		return CSTR("ESRIMap");
	case MapType::WMS:
		return CSTR("WMS");
	case MapType::WFS:
		return CSTR("WFS");
	default:
		return CSTR("Unknown");
	}
}
