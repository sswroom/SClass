#ifndef _SM_PYTHON_PYTHONOBJECT
#define _SM_PYTHON_PYTHONOBJECT
#include "AnyType.h"
#include "Text/CString.h"

namespace Python
{
	class PythonObject
	{
	private:
		struct ClassData;
		NN<ClassData> clsData;
	public:
		PythonObject(AnyType obj);
		virtual ~PythonObject();

		IntOS GetRefCount() const;
		Bool HasAttr(Text::CStringNN name) const;
		Optional<PythonObject> GetAttr(Text::CStringNN name) const;
		Bool SetAttr(Text::CStringNN name, Optional<PythonObject> obj) const;
		static Optional<PythonObject> FromPtr(AnyType obj);
	};
}
#endif