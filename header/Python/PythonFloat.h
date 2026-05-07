#ifndef _SM_PYTHON_PYTHONFLOAT
#define _SM_PYTHON_PYTHONFLOAT
#include "Python/PythonObject.h"

namespace Python
{
	class PythonFloat : public PythonObject
	{
	public:
		PythonFloat(AnyType floatObj);
		PythonFloat(Double value);
		virtual ~PythonFloat();

		Double GetValue() const;
		virtual ObjectType GetObjectType() const;
	};
}
#endif