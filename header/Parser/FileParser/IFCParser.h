#ifndef _SM_PARSER_FILEPARSER_IFCPARSER
#define _SM_PARSER_FILEPARSER_IFCPARSER
#include "Data/ArrayListObj.hpp"
#include "IO/FileParser.h"
#include "Math/Transform.h"
#include "Model/Model3D.h"

namespace Parser
{
	namespace FileParser
	{
		class IFCParser : public IO::FileParser
		{
		public:
			struct IFCEntity
			{
				UInt32 id;
				Text::CStringNN type;
				Data::ArrayListObj<Text::CString> args;
			};

			static void SplitArgs(NN<Data::ArrayListObj<Text::CString>> args, Text::CStringNN s);
			static void ParseEntities(NN<Data::ArrayListNN<IFCEntity>> entities, Data::ByteArrayR data);
			static Optional<IFCEntity> Find(NN<Data::ArrayListNN<IFCEntity>> entities, UInt32 id);
			static UInt32 Ref(Text::CString s);
			static Data::ArrayListNative<UInt32> Refs(Text::CString s);
			static Data::ArrayListNative<Double> Numbers(Text::CString s);
			static Math::Vector3 GetPoint(NN<Data::ArrayListNN<IFCEntity>> entities, UInt32 id);
			static Math::Vector3 GetDirection(NN<Data::ArrayListNN<IFCEntity>> entities, UInt32 id, Math::Vector3 defVal);
			static Math::Transform GetAxisPlacement(NN<Data::ArrayListNN<IFCEntity>> entities, UInt32 id);
			static Math::Transform GetObjectPlacement(NN<Data::ArrayListNN<IFCEntity>> entities, UInt32 id, UInt32 depth);
			static void AddPolygon(NN<Model::Model3D> model, NN<Data::ArrayListObj<Math::Vector3>> points, const Math::Transform &t, UInt32 elementId);
			static Data::ArrayListObj<Math::Vector3> GetProfilePoints(NN<Data::ArrayListNN<IFCEntity>> entities, UInt32 profileId);
			static void ParseExtrusion(NN<Model::Model3D> model, NN<Data::ArrayListNN<IFCEntity>> entities, NN<IFCEntity> solid, const Math::Transform &productT, UInt32 elementId);
			static void ParseFacetedBrep(NN<Model::Model3D> model, NN<Data::ArrayListNN<IFCEntity>> entities, UInt32 brepId, const Math::Transform &t, UInt32 elementId);
			static Data::ArrayListObj<Math::Vector3> GetPointList3D(NN<Data::ArrayListNN<IFCEntity>> entities, UInt32 listId);
			static void ParseTriangulatedFaceSet(NN<Model::Model3D> model, NN<Data::ArrayListNN<IFCEntity>> entities, NN<IFCEntity> faceSet, const Math::Transform &t, UInt32 elementId);
			static void ParsePolygonalFaceSet(NN<Model::Model3D> model, NN<Data::ArrayListNN<IFCEntity>> entities, NN<IFCEntity> faceSet, const Math::Transform &t, UInt32 elementId);
			static Math::Transform GetMappingTarget(NN<Data::ArrayListNN<IFCEntity>> entities, UInt32 id);
			static void ParseItem(NN<Model::Model3D> model, NN<Data::ArrayListNN<IFCEntity>> entities, UInt32 itemId, const Math::Transform &t, UInt32 elementId, UInt32 depth);
			static void ParseModel(NN<Model::Model3D> model, NN<Data::ArrayListNN<IFCEntity>> entities);
		public:
			IFCParser();
			virtual ~IFCParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}

#endif