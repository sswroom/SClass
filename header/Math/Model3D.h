#ifndef _SM_MATH_MODEL3D
#define _SM_MATH_MODEL3D
#include "Data/ArrayListNN.hpp"
#include "IO/ParsedObject.h"
#include "Math/Vector3D.h"

namespace Math
{
	class Model3D : public IO::ParsedObject
	{
	public:
		enum class ItemType
		{
			Triangle
		};
		struct ModelItem
		{
			UInt32 elementId;
			ItemType type;
		};

		struct Triangle : public ModelItem
		{
			Vector3D pt1;
			Vector3D pt2;
			Vector3D pt3;
		};

	private:
		Data::ArrayListNN<ModelItem> itemList;
		Vector3D minPt;
		Vector3D maxPt;

	public:
		Model3D(NN<Text::String> sourceName);
		virtual ~Model3D();

		virtual IO::ParserType GetParserType() const;
		void AddTriangle(Vector3D pt1, Vector3D pt2, Vector3D pt3, UInt32 elementId);
		UIntOS GetCount() const;
		NN<ModelItem> GetItemNoCheck(UIntOS index) const;
		void GetBounds(OutParam<Vector3D> minPt, OutParam<Vector3D> maxPt) const;
	};
}

#endif