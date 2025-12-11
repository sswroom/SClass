#ifndef _SM_JAVA_JAVAJOPTIONPANE
#define _SM_JAVA_JAVAJOPTIONPANE
#include "Java/JavaComponent.h"
#include "Text/CString.h"
#include <jni.h>

namespace Java
{
	class JavaJOptionPane
	{
	public:
		enum class OptionType
		{
			YES_NO_OPTION = 0,
			YES_NO_CANCEL_OPTION = 1,
			OK_CANCEL_OPTION = 2
		};

		enum class SelectedOption
		{
			YES_OPTION = 0,
			NO_OPTION = 1,
			CANCEL_OPTION = 2,
			OK_OPTION = 0,
			CLOSED_OPTION = -1
		};

		enum class MessageType
		{
 			ERROR_MESSAGE = 0,
			INFORMATION_MESSAGE = 1,
			WARNING_MESSAGE = 2,
			QUESTION_MESSAGE = 3,
			PLAIN_MESSAGE = -1
		};

		static void ShowMessageDialog(Optional<JavaComponent> parentComponent, Text::CStringNN message, Text::CStringNN title, MessageType messageType);
		static SelectedOption ShowConfirmDialog(Optional<JavaComponent> parentComponent, Text::CStringNN message, Text::CStringNN title, OptionType optionType, MessageType messageType);
		static jclass GetClass();
		static Int32 OptionTypeGetInt(OptionType optionType);
		static Int32 MessageTypeGetInt(MessageType messageType);
	};
}
#endif
