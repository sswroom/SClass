#ifndef _SM_PYTHON_PYTHONMODULE
#define _SM_PYTHON_PYTHONMODULE
#include "Python/PythonObject.h"

namespace Python
{
	class PythonDict;
	class PythonModule : public PythonObject
	{
	public:
		PythonModule(AnyType module);
		virtual ~PythonModule();

		UnsafeArrayOpt<const UTF8Char> GetName() const;
		UnsafeArrayOpt<const UTF8Char> GetFileName() const;
		Optional<PythonDict> GetDict() const;
		virtual ObjectType GetObjectType() const;
		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
	};
}
#endif