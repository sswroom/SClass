#ifndef _SM_PYTHON_PYTHONDICT
#define _SM_PYTHON_PYTHONDICT
#include "Python/PythonObject.h"

namespace Python
{
	class PythonDict : public PythonObject
	{
	public:
		PythonDict(AnyType dict);
		virtual ~PythonDict();

		UnsafeArrayOpt<const UTF8Char> GetKey(UIntOS index) const;
		Optional<PythonObject> GetValue(UIntOS index) const;
		virtual ObjectType GetObjectType() const;
		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
	};
}
#endif