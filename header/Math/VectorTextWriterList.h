#ifndef _SM_MATH_VECTORTEXTWRITERLIST
#define _SM_MATH_VECTORTEXTWRITERLIST
#include "Data/ArrayListObj.hpp"
#include "Math/VectorTextWriter.h"

namespace Math
{
	class VectorTextWriterList : public Data::ReadingList<Optional<Math::VectorTextWriter>>
	{
	private:
		Data::ArrayListNN<Math::VectorTextWriter> list;
	public:
		VectorTextWriterList();
		virtual ~VectorTextWriterList();

		virtual UIntOS GetCount() const;
		virtual Optional<Math::VectorTextWriter> GetItem(UIntOS index) const;
	};
}
#endif
