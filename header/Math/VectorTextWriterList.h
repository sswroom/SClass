#ifndef _SM_MATH_VECTORTEXTWRITERLIST
#define _SM_MATH_VECTORTEXTWRITERLIST
#include "Data/ArrayListObj.hpp"
#include "Math/VectorTextWriter.h"

namespace Math
{
	class VectorTextWriterList : public Data::ReadingList<Math::VectorTextWriter*>
	{
	private:
		Data::ArrayListObj<Math::VectorTextWriter*> list;
	public:
		VectorTextWriterList();
		virtual ~VectorTextWriterList();

		virtual UOSInt GetCount() const;
		virtual Math::VectorTextWriter* GetItem(UOSInt index) const;
	};
}
#endif
