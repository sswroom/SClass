#ifndef _SM_PYTHON_PYTHONTYPE
#define _SM_PYTHON_PYTHONTYPE
#include "Python/PythonObject.h"

namespace Python
{
	class PythonUnicode;
	class PythonType : public PythonObject
	{
	public:
		PythonType(AnyType t);
		virtual ~PythonType();

		Optional<PythonUnicode> GetName() const;
		virtual ObjectType GetObjectType() const;
		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
	};
}
#endif