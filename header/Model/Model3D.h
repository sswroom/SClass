#ifndef _SM_MODEL_MODEL3D
#define _SM_MODEL_MODEL3D
#include "IO/ParsedObject.h"
#include <vector>

namespace Model
{
	class Model3D : public IO::ParsedObject
	{
	public:
		struct Triangle
		{
			Double x1;
			Double y1;
			Double z1;
			Double x2;
			Double y2;
			Double z2;
			Double x3;
			Double y3;
			Double z3;
			UInt32 elementId;
		};

	private:
		std::vector<Triangle> triangles;
		Double minX;
		Double minY;
		Double minZ;
		Double maxX;
		Double maxY;
		Double maxZ;

	public:
		Model3D(NN<Text::String> sourceName);
		virtual ~Model3D();

		virtual IO::ParserType GetParserType() const;
		void AddTriangle(Double x1, Double y1, Double z1, Double x2, Double y2, Double z2, Double x3, Double y3, Double z3, UInt32 elementId);
		UIntOS GetTriangleCount() const;
		const Triangle &GetTriangle(UIntOS index) const;
		void GetBounds(OutParam<Double> minX, OutParam<Double> minY, OutParam<Double> minZ, OutParam<Double> maxX, OutParam<Double> maxY, OutParam<Double> maxZ) const;
	};
}

#endif