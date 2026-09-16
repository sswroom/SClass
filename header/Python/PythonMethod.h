#ifndef _SM_PYTHON_PYTHONMETHOD
#define _SM_PYTHON_PYTHONMETHOD
#include "Python/PythonObject.h"

namespace Python
{
	class PythonMethod : public PythonObject
	{
	public:
		PythonMethod(AnyType function);
		virtual ~PythonMethod();

		virtual ObjectType GetObjectType() const;
		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
	};
}
#endif