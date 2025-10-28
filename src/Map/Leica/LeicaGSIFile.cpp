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

Optional<Map::Leica::LeicaGSIFile> Map::Leica::LeicaGSIFile::Parse(NN<IO::Stream> fs)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	Text::PString sarr[10];
	OSInt colCount;
	OSInt i;
	Optional<Map::Leica::LeicaGSIFile> retFile = 0;
	IO::StreamReader *reader;
	NEW_CLASS(reader, IO::StreamReader(fs, 65001));
	if (reader->ReadLine(sbuff, 511).SetTo(sptr))
	{
		colCount = Text::StrSplitP(sarr, 10, Text::PString(sbuff, (UOSInt)(sptr - sbuff)), ' ');
		if (sarr[0].StartsWith(UTF8STRC("410001+")))
		{
			if (sarr[0].Equals(UTF8STRC("410001+?......1")))
			{
				NN<Map::Leica::LeicaLevelFile> levelFile;
				NEW_CLASSNN(levelFile, Map::Leica::LeicaLevelFile());
				ParseHeader(sarr, colCount, levelFile);

				while (reader->ReadLine(sbuff, 511).SetTo(sptr))
				{
					colCount = Text::StrSplitP(sarr, 10, Text::PString(sbuff, (UOSInt)(sptr - sbuff)), ' ');
					if (sarr[0].v[0] == '1' && sarr[0].v[1] == '1')
					{
						Leica::LeicaLevelFile::Measurement point;
						Leica::LeicaLevelFile::HeightMeasure height;
						WChar pointId = 0;
						Leica::LeicaLevelFile::ClearMeasurement(point);
						Leica::LeicaLevelFile::ClearHeight(height);
						i = 1;
						while (i < colCount)
						{
							if (sarr[i].v[0] == '8')
							{
								if (sarr[i].v[1] == '3')
								{
									height.elevation = Text::StrToInt32(&sarr[i].v[7]);
								}
							}
							else if (sarr[i].v[0] == '3')
							{
								if (sarr[i].v[1] == '2')
								{
									point.horizonalDistance = Text::StrToInt32(&sarr[i].v[7]);
								}
								else if (sarr[i].v[1] == '3')
								{
									point.heightDiff = Text::StrToInt32(&sarr[i].v[7]);
									pointId = sarr[i].v[2];
								}
								else if (sarr[i].v[1] == '9')
								{
									if (sarr[i].v[2] == '0')
									{
										point.distExtra[0] = Text::StrToInt32(&sarr[i].v[7]);
									}
									else if (sarr[i].v[2] == '1')
									{
										point.distExtra[1] = Text::StrToInt32(&sarr[i].v[7]);
									}
								}
							}
							else if (sarr[i].v[0] == '5')
							{
								if (sarr[i].v[1] == '7')
								{
									if (sarr[i].v[2] == '3')
									{
										height.distExtra[0] = Text::StrToInt32(&sarr[i].v[7]);
									}
									else if (sarr[i].v[2] == '4')
									{
										height.distExtra[1] = Text::StrToInt32(&sarr[i].v[7]);
									}
								}
							}
							i++;
						}
						if (height.elevation >= 0)
						{
							levelFile->AddPointHeight(sarr[0].Substring(7).ToCString(), height);
						}
						if (pointId != 0)
						{
							levelFile->AddMeasurement(sarr[0].Substring(7).ToCString(), point, pointId);
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

void Map::Leica::LeicaGSIFile::ParseHeader(UnsafeArray<Text::PString> sarr, UOSInt colCount, NN<Map::Leica::LeicaGSIFile> file)
{
}
