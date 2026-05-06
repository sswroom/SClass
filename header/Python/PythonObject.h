#ifndef _SM_PYTHON_PYTHONOBJECT
#define _SM_PYTHON_PYTHONOBJECT
#include "AnyType.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace Python
{
	enum class ObjectType
	{
		Unknown,
		Unicode,
		Float,
		Module,
		Dict,
		Type,
		Function
	};

	class PythonObject
	{
	protected:
		struct ClassData;
		NN<ClassData> clsData;
	public:
		PythonObject(AnyType obj);
		virtual ~PythonObject();

		IntOS GetRefCount() const;
		Bool HasAttr(Text::CStringNN name) const;
		Optional<PythonObject> GetAttr(Text::CStringNN name) const;
		Bool SetAttr(Text::CStringNN name, Optional<PythonObject> obj) const;
		AnyType GetObject() const;
		virtual ObjectType GetObjectType() const;
		void ToPythonString(NN<Text::StringBuilderUTF8> sb) const;
		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;

		static Optional<PythonObject> FromPtr(AnyType obj);
	};

	Text::CStringNN ObjectTypeGetName(Python::ObjectType objType);
}
#endif