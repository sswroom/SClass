#ifndef _SM_MAP_CESIUMTILE
#define _SM_MAP_CESIUMTILE
#include "IO/PackageFile.h"
#include "Math/Vector3.h"
#include "Text/EncodingFactory.h"

namespace Map
{
	class CesiumTile : public IO::ParsedObject
	{
	private:
		NN<IO::PackageFile> pkg;
		Bool metadataFound;
		UInt32 srid;
		Math::Vector3 srsOrigin;
		Optional<Text::String> jsonFile;
	public:
		CesiumTile(NN<IO::PackageFile> pkg, Optional<Text::String> name, Optional<Text::EncodingFactory> encFact);
		virtual ~CesiumTile();

		virtual IO::ParserType GetParserType() const;
		Bool IsError() const;
		NN<IO::PackageFile> GetPackageFile() const;
		Optional<Text::String> GetJSONFile() const;
	};
}
#endif
