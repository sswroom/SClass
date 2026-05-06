#ifndef _SM_PYTHON_PYTHONMODULE
#define _SM_PYTHON_PYTHONMODULE
#include "Python/PythonObject.h"

namespace Python
{
	class PythonModule : public PythonObject
	{
	public:
		PythonModule(AnyType module);
		virtual ~PythonModule();
	};
}
#endif