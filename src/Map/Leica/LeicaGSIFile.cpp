#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/StreamReader.h"
#include "Map/Leica/LeicaGSIFile.h"
#include "Map/Leica/LeicaLevelFile.h"
#include "Text/MyString.h"

Map::Leica::LeicaGSIFile::LeicaGSIFile()
{
}

Map::Leica::LeicaGSIFile::~LeicaGSIFile()
{
}

Map::Leica::LeicaGSIFile *Map::Leica::LeicaGSIFile::Parse(IO::Stream *fs)
{
	WChar sbuff[512];
	WChar *sarr[10];
	OSInt colCount;
	OSInt i;
	Map::Leica::LeicaGSIFile *retFile = 0;
	IO::StreamReader *reader;
	NEW_CLASS(reader, IO::StreamReader(fs, 65001));
	if (reader->ReadLine(sbuff, 511))
	{
		colCount = Text::StrSplit(sarr, 10, sbuff, ' ');
		if (Text::StrStartsWith(sarr[0], L"410001+"))
		{
			if (Text::StrEquals(sarr[0], L"410001+?......1"))
			{
				Map::Leica::LeicaLevelFile *levelFile;
				NEW_CLASS(levelFile, Map::Leica::LeicaLevelFile());
				ParseHeader(sarr, colCount, levelFile);

				while (reader->ReadLine(sbuff, 511))
				{
					colCount = Text::StrSplit(sarr, 10, sbuff, ' ');
					if (sarr[0][0] == '1' && sarr[0][1] == '1')
					{
						Leica::LeicaLevelFile::Measurement point;
						Leica::LeicaLevelFile::HeightMeasure height;
						WChar pointId = 0;
						Leica::LeicaLevelFile::ClearMeasurement(&point);
						Leica::LeicaLevelFile::ClearHeight(&height);
						i = 1;
						while (i < colCount)
						{
							if (sarr[i][0] == '8')
							{
								if (sarr[i][1] == '3')
								{
									height.elevation = Text::StrToInt32(&sarr[i][7]);
								}
							}
							else if (sarr[i][0] == '3')
							{
								if (sarr[i][1] == '2')
								{
									point.horizonalDistance = Text::StrToInt32(&sarr[i][7]);
								}
								else if (sarr[i][1] == '3')
								{
									point.heightDiff = Text::StrToInt32(&sarr[i][7]);
									pointId = sarr[i][2];
								}
								else if (sarr[i][1] == '9')
								{
									if (sarr[i][2] == '0')
									{
										point.distExtra[0] = Text::StrToInt32(&sarr[i][7]);
									}
									else if (sarr[i][2] == '1')
									{
										point.distExtra[1] = Text::StrToInt32(&sarr[i][7]);
									}
								}
							}
							else if (sarr[i][0] == '5')
							{
								if (sarr[i][1] == '7')
								{
									if (sarr[i][2] == '3')
									{
										height.distExtra[0] = Text::StrToInt32(&sarr[i][7]);
									}
									else if (sarr[i][2] == '4')
									{
										height.distExtra[1] = Text::StrToInt32(&sarr[i][7]);
									}
								}
							}
							i++;
						}
						if (height.elevation >= 0)
						{
							levelFile->AddPointHeight(&sarr[0][7], &height);
						}
						if (pointId != 0)
						{
							levelFile->AddMeasurement(&sarr[0][7], &point, pointId);
						}
					}
				}
				retFile = levelFile;
			}
		}
	}
	DEL_CLASS(reader);
	return retFile;
}

void Map::Leica::LeicaGSIFile::ParseHeader(WChar **sarr, OSInt colCount, Map::Leica::LeicaGSIFile *file)
{
}
