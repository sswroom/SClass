#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Text/MyString.h"
#include "Text/Encoding.h"
#include "IO/Stream.h"
#include "IO/FileStream.h"
#include "IO/StreamReader.h"
#include "Map/MapLayer.h"
#include "Map/MapEngine.h"
#include <windows.h>
namespace Map
{
	typedef struct
	{
		Int32 lineType;
		Int32 lineWidth;
		Int32 color;
		WChar *styles;
	} MAP_LINE_STYLE;

	typedef struct
	{
		Int32 fontType;
		WChar *fontName;
		Int32 fontSize;
		Int32 thick;
		Int32 color;
	} MAP_FONT_STYLE;

	Int32 map_get_color(WChar *str)
	{
		Int32 v = Text::StrHex2Int(str);
		return 0xff000000 | ((v & 0xff) << 16) | (v & 0xff00) | ((v >> 16) & 0xff);
	}
}

Int32 Map::MapLayer::Draw(UInt8 *buff, Int32 width, Int32 height, Double lat, Double lon, Int32 scale, WChar *lang, Int32 imgFormat, Data::ArrayList<Map::MAP_EXTRA*> *drawExtra, Bool *isLayerEmpty)
{
	Int32 retSize = 0;
	WChar fileName[256];
	WChar lineBuff[1024];
	WChar layerName[512];
	WChar *baseDir;
	WChar *strs[10];
	WChar *sptr;
	IO::FileStream *fstm;
	IO::StreamReader *rdr;
	Int32 maxScale;
	Int32 minScale;
	Int32 thisLine;
	Int32 i;
	Int32 lastLine = -1;
	Data::ArrayList<MAP_LINE_STYLE*> *lastLines;
	Int32 lastFont = -1;
	Data::ArrayList<MAP_FONT_STYLE*> *lastFonts;
	MAP_LINE_STYLE *currLine;
	MAP_FONT_STYLE *currFont;
	Bool drawn = false;

	NEW_CLASS(lastLines, Data::ArrayList<MAP_LINE_STYLE*>());
	NEW_CLASS(lastFonts, Data::ArrayList<MAP_FONT_STYLE*>());


	sptr = Text::StrConcat(fileName, L"MapLayer_");
	sptr = Text::StrConcat(sptr, lang);
	sptr = Text::StrConcat(sptr, L".txt");
//	debugMut->Lock();
	NEW_CLASS(fstm, IO::FileStream(fileName, IO::FileStream::FileMode::ReadOnly, IO::FileStream::FileShare::DenyNone));
	if (fstm->IsError())
	{
	}
	else
	{
		LARGE_INTEGER liSt;
		LARGE_INTEGER liEd;
		LARGE_INTEGER liFreq;
		Double spd;

		void *map;
		NEW_CLASS(rdr, IO::StreamReader(fstm));
		while (rdr->ReadLine(lineBuff))
		{
			OSInt strCnt;
			Int32 lyrType;
			strCnt = Text::StrSplitTrim(strs, 10, lineBuff, ',');

			lyrType = Text::StrToInt32(strs[0]);
			switch (lyrType)
			{
			case 1:
				map = Map::MapEngine::Init(width, height, (Int32)(lon * 200000), (Int32)(lat * 200000), map_get_color(strs[1]), scale, Text::StrToInt32(strs[2]), Text::StrToInt32(strs[3]), Text::StrToInt32(strs[4]));
				break;
			case 2:
				baseDir = Text::StrConcat(layerName, strs[1]);
				break;
			case 3:
				thisLine = Text::StrToInt32(strs[1]);
				if (lastLine != thisLine)
				{
					if (lastLines->GetCount() > 0)
					{
						Map::MapEngine::InitLine(map, lastLine, lastLines->GetCount());
						i = 0;
						while (i < lastLines->GetCount())
						{
							currLine = (MAP_LINE_STYLE*)lastLines->GetItem(i);
							Map::MapEngine::SetLine(map, lastLine, i, currLine->lineType, currLine->lineWidth, currLine->color, currLine->styles);
							if (currLine->styles)
							{
								MemFree(currLine->styles);
							}
							MemFree(currLine);
							i++;
						}
					}
					lastLine = thisLine;
					lastLines->Clear();
				}

				if (strCnt == 5)
				{
					currLine = MemAlloc(MAP_LINE_STYLE, 1);
					currLine->lineType = Text::StrToInt32(strs[2]);
					currLine->lineWidth = Text::StrToInt32(strs[3]);
					currLine->color = map_get_color(strs[4]);
					currLine->styles = 0;
					lastLines->Add(currLine);
				}
				else
				{
					i = 6;
					while (i < strCnt)
					{
						strs[i++][-1] = ',';
					}
					sptr = strs[strCnt-1];
					while (*sptr++);
					currLine = MemAlloc(MAP_LINE_STYLE, 1);
					currLine->lineType = Text::StrToInt32(strs[2]);
					currLine->lineWidth = Text::StrToInt32(strs[3]);
					currLine->color = map_get_color(strs[4]);
					currLine->styles = MemAlloc(WChar,  sptr - strs[5]);
					Text::StrConcat(currLine->styles, strs[5]);
					lastLines->Add(currLine);
				}
				break;
			case 5:
				thisLine = Text::StrToInt32(strs[1]);
				if (lastFont != thisLine)
				{
					if (lastFonts->GetCount() > 0)
					{
						Map::MapEngine::InitFont(map, lastFont, lastFonts->GetCount());
						i = 0;
						while (i < lastFonts->GetCount())
						{
							currFont = (MAP_FONT_STYLE*)lastFonts->GetItem(i);
							Map::MapEngine::SetFont(map, lastFont, i, currFont->fontType, currFont->fontName, currFont->fontSize, currFont->thick, currFont->color);
							MemFree(currFont->fontName);
							MemFree(currFont);
							i++;
						}
					}
					lastFont = thisLine;
					lastFonts->Clear();
				}

				currFont = MemAlloc(MAP_FONT_STYLE, 1);
				currFont->fontType = Text::StrToInt32(strs[2]);
				sptr = strs[3];
				while (*sptr++);
				currFont->fontName = MemAlloc(WChar, sptr - strs[3]);
				Text::StrConcat(currFont->fontName, strs[3]);
				currFont->fontSize = (Text::StrToInt32(strs[4]) * 3) >> 2;
				currFont->thick = Text::StrToInt32(strs[5]);
				currFont->color = map_get_color(strs[6]);
				lastFonts->Add(currFont);
				break;
			case 0:
				break;
			case 13:
				break;
			default:
				if (lastLines->GetCount() > 0)
				{
					Map::MapEngine::InitLine(map, lastLine, lastLines->GetCount());
					i = 0;
					while (i < lastLines->GetCount())
					{
						currLine = (MAP_LINE_STYLE*)lastLines->GetItem(i);
						Map::MapEngine::SetLine(map, lastLine, i, currLine->lineType, currLine->lineWidth, currLine->color, currLine->styles);
						if (currLine->styles)
						{
							MemFree(currLine->styles);
						}
						MemFree(currLine);
						i++;
					}
					lastLine = -1;
					lastLines->Clear();
				}
				if (lastFonts->GetCount() > 0)
				{
					Map::MapEngine::InitFont(map, lastFont, lastFonts->GetCount());
					i = 0;
					while (i < lastFonts->GetCount())
					{
						currFont = (MAP_FONT_STYLE*)lastFonts->GetItem(i);
						Map::MapEngine::SetFont(map, lastFont, i, currFont->fontType, currFont->fontName, currFont->fontSize, currFont->thick, currFont->color);
						MemFree(currFont->fontName);
						MemFree(currFont);
						i++;
					}
					lastFont = -1;
					lastFonts->Clear();
				}

				if (lyrType == 6)
				{
					minScale = Text::StrToInt32(strs[2]);
					maxScale = Text::StrToInt32(strs[3]);
					if (scale >= minScale && scale <= maxScale)
					{
						Text::StrConcat(baseDir, strs[1]);
						QueryPerformanceCounter(&liSt);
						Map::MapEngine::DrawLine(map, layerName, maxScale, Text::StrToInt32(strs[4]), &drawn);
						QueryPerformanceCounter(&liEd);
						QueryPerformanceFrequency(&liFreq);
						spd = (liEd.QuadPart - liSt.QuadPart) / (Double)liFreq.QuadPart;
						if (spd > 0.1)
						{
//							wprintf(L"Slow pg: %lf, %s, \n", spd, layerName);
//							wprintf(L"\n");
						}
					}
				}
				else if (lyrType == 7)
				{
					minScale = Text::StrToInt32(strs[2]);
					maxScale = Text::StrToInt32(strs[3]);
					if (scale >= minScale && scale <= maxScale)
					{
						Text::StrConcat(baseDir, strs[1]);
						QueryPerformanceCounter(&liSt);
						Map::MapEngine::FillLine(map, layerName, maxScale, Text::StrToInt32(strs[4]), map_get_color(strs[5]), &drawn);
						QueryPerformanceCounter(&liEd);
						QueryPerformanceFrequency(&liFreq);
						spd = (liEd.QuadPart - liSt.QuadPart) / (Double)liFreq.QuadPart;
						if (spd > 0.1)
						{
//							wprintf(L"Slow pg: %lf, %s, \n", spd, layerName);
//							wprintf(L"\n");
						}
					}
				}
				else if (lyrType == 8)
				{
					minScale = Text::StrToInt32(strs[2]);
					maxScale = Text::StrToInt32(strs[3]);
					if (scale >= minScale && scale <= maxScale)
					{
					}
				}
				else if (lyrType == 9)
				{
					minScale = Text::StrToInt32(strs[2]);
					maxScale = Text::StrToInt32(strs[3]);
					if (scale >= minScale && scale <= maxScale)
					{
						Text::StrConcat(baseDir, strs[1]);
						Map::MapEngine::DrawString(map, layerName, Text::StrToInt32(strs[4]), Text::StrToInt32(strs[5]), Text::StrToInt32(strs[6]), &drawn);
					}
				}
				else if (lyrType == 10)
				{
					minScale = Text::StrToInt32(strs[2]);
					maxScale = Text::StrToInt32(strs[3]);
					if (scale >= minScale && scale <= maxScale)
					{
						Text::StrConcat(baseDir, strs[1]);
						Map::MapEngine::DrawPoints(map, layerName, strs[4], &drawn);
					}
				}
				break;
			}
		}
		DEL_CLASS(rdr);
		Map::MapEngine::EndDraw(map);
		if (drawExtra)
		{
			i = 0;
			while (i < drawExtra->GetCount())
			{
				MAP_EXTRA *ext = (MAP_EXTRA*)drawExtra->GetItem(i);
				if (ext->extraType == 0)
				{
					Map::MapEngine::DrawMarker(map, (Int32)(ext->lon * 200000), (Int32)(ext->lat * 200000), ext->color);
				}
				i++;
			}
		}

		if (map)
		{
			retSize = Map::MapEngine::GetImg(map, buff, imgFormat);
			Map::MapEngine::Close(map);
		}
	}
//	debugMut->Unlock();
	DEL_CLASS(lastFonts);
	DEL_CLASS(lastLines);
	DEL_CLASS(fstm);
	if (isLayerEmpty)
	{
		*isLayerEmpty = !drawn;
	}

	return retSize;
}
