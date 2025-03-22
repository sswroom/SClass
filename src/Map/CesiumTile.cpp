#include "Stdafx.h"
#include "IO/StreamDataStream.h"
#include "Map/CesiumTile.h"
#include "Text/JSON.h"
#include "Text/XMLReader.h"

Map::CesiumTile::CesiumTile(NN<IO::PackageFile> pkg, Optional<Text::String> name, Optional<Text::EncodingFactory> encFact) : IO::ParsedObject(name.Or(pkg->GetSourceNameObj()))
{
	this->pkg = pkg->Clone();
	this->srid = 0;
	this->srsOrigin = Math::Vector3(NAN, NAN, NAN);
	this->multijson = false;
	this->jsonFile = 0;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	IO::PackageFile::PackObjectType pot;
	Text::CStringNN fileName;
	NN<IO::StreamData> fd;
	NN<Text::String> s;
	Text::StringBuilderUTF8 sb;
	Text::PString sarr[4];
	UOSInt i = 0;
	UOSInt j = pkg->GetCount();
	while (i < j)
	{
		pot = pkg->GetItemType(i);
		if (pot == IO::PackageFile::PackObjectType::StreamData)
		{
			sptr = pkg->GetItemName(sbuff, i).Or(sbuff);
			fileName = CSTRP(sbuff, sptr);
			if (fileName.Equals(CSTR("metadata.xml")))
			{
				if (pkg->GetItemStmDataNew(i).SetTo(fd))
				{
					{
						IO::StreamDataStream stm(fd);
						Text::XMLReader reader(encFact, stm, Text::XMLReader::PM_XML);
						if (reader.NextElementName().SetTo(s) && s->Equals(CSTR("ModelMetadata")))
						{
							while (reader.NextElementName().SetTo(s))
							{
								if (s->Equals(CSTR("SRS")))
								{
									sb.ClearStr();
									reader.ReadNodeText(sb);
									if (sb.StartsWith(CSTR("EPSG:")))
									{
										this->srid = sb.Substring(5).ToUInt32();
									}
								}
								else if (s->Equals(CSTR("SRSOrigin")))
								{
									sb.ClearStr();
									reader.ReadNodeText(sb);
									if (Text::StrSplitP(sarr, 4, sb, ' ') == 3)
									{
										this->srsOrigin = Math::Vector3(sarr[0].ToDoubleOrNAN(), sarr[1].ToDoubleOrNAN(), sarr[2].ToDoubleOrNAN());
									}
								}
								else
								{
									reader.SkipElement();
								}
							}
						}
					}
					fd.Delete();
				}
			}
			else if (fileName.EndsWith(CSTR(".json")))
			{
				if (pkg->GetItemStmDataNew(i).SetTo(fd))
				{
					Bool valid = false;
					if (fd->GetDataSize() <= 1048576)
					{
						UOSInt len = (UOSInt)fd->GetDataSize();
						UInt8 *buff = MemAlloc(UInt8, len + 1);
						if (fd->GetRealData(0, len, Data::ByteArray(buff, len)) == len)
						{
							buff[len] = 0;
							NN<Text::JSONBase> json;
							if (Text::JSONBase::ParseJSONStr(Text::CStringNN(buff, len)).SetTo(json))
							{
								valid = json->GetValueType(CSTR("asset")) == Text::JSONType::Object &&
									json->GetValueType(CSTR("geometricError")) == Text::JSONType::Number &&
									json->GetValueType(CSTR("root.boundingVolume")) == Text::JSONType::Object;
								json->EndUse();
							}
						}
						MemFree(buff);
					}
					fd.Delete();

					if (valid)
					{
						if (this->jsonFile.SetTo(s))
						{
							s->Release();
							this->multijson = true;
						}
						this->jsonFile = Text::String::New(fileName);
					}
				}
			}
		}
		i++;
	}
}

Map::CesiumTile::~CesiumTile()
{
	this->pkg.Delete();
	OPTSTR_DEL(this->jsonFile);
}

IO::ParserType Map::CesiumTile::GetParserType() const
{
	return IO::ParserType::CesiumTile;
}

Bool Map::CesiumTile::IsError() const
{
	return this->multijson || this->jsonFile.IsNull();
}

NN<IO::PackageFile> Map::CesiumTile::GetPackageFile() const
{
	return this->pkg;
}

Optional<Text::String> Map::CesiumTile::GetJSONFile() const
{
	return this->jsonFile;
}
