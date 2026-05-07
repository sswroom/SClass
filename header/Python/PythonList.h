#ifndef _SM_PYTHON_PYTHONLIST
#define _SM_PYTHON_PYTHONLIST
#include "Python/PythonObject.h"

namespace Python
{
	class PythonList : public PythonObject
	{
	public:
		PythonList(AnyType listObj);
		PythonList();
		virtual ~PythonList();

		UIntOS GetCount() const;
		Optional<PythonObject> GetNewItem(UIntOS index) const;
		void Append(NN<PythonObject> item) const;
		virtual ObjectType GetObjectType() const;
	};
}
#endif