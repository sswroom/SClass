#ifndef _SM_PYTHON_PYTHONFUNCTION
#define _SM_PYTHON_PYTHONFUNCTION
#include "Python/PythonObject.h"

namespace Python
{
	class PythonFunction : public PythonObject
	{
	public:
		PythonFunction(AnyType function);
		virtual ~PythonFunction();

		virtual ObjectType GetObjectType() const;
		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
	};
}
#endif