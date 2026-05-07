#ifndef _SM_PYTHON_PYTHONBOOL
#define _SM_PYTHON_PYTHONBOOL
#include "Python/PythonObject.h"

namespace Python
{
	class PythonBool : public PythonObject
	{
	public:
		PythonBool(AnyType boolObj);
		PythonBool(Bool value);
		virtual ~PythonBool();

		Bool GetValue() const;
		virtual ObjectType GetObjectType() const;
	};
}
#endif