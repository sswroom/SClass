#ifndef _SM_TEXT_VSPROJECT
#define _SM_TEXT_VSPROJECT
#include "Data/ArrayList.h"
#include "Text/CodeProject.h"

namespace Text
{
	class VSFile : public Text::CodeFile
	{
	private:
		Text::String *fileName;
	public:
		VSFile(Text::String *fileName);
		VSFile(Text::CString fileName);
		virtual ~VSFile();

		virtual Text::String *GetFileName();
	};

	class VSProjContainer
	{
	public:
		virtual void AddChild(Text::CodeObject *obj) = 0;
	};

	class VSContainer : public Text::CodeContainer, public VSProjContainer
	{
	private:
		Text::String *contName;
		Data::ArrayList<CodeObject*> *childList;
	public:
		VSContainer(Text::String *contName);
		VSContainer(Text::CString contName);
		virtual ~VSContainer();

		virtual void SetContainerName(Text::CString contName);
		virtual Text::String *GetContainerName();

		virtual UOSInt GetChildCount();
		virtual CodeObject *GetChildObj(UOSInt index);

		virtual void AddChild(Text::CodeObject *obj);
	};

	class VSProject : public Text::CodeProject, public VSProjContainer 
	{
	public:
		typedef enum
		{
			VSV_UNKNOWN,
			VSV_VS6,
			VSV_VS71,
			VSV_VS8,
			VSV_VS9,
			VSV_VS10,
			VSV_VS11,
			VSV_VS12
		} VisualStudioVersion;
	private:
		VisualStudioVersion ver;
		Text::String *projName;
		Data::ArrayList<CodeObject*> *childList;
	public:
		VSProject(Text::CString name, VisualStudioVersion ver);
		virtual ~VSProject();

		virtual ProjectType GetProjectType();
		virtual void SetProjectName(Text::String *projName);
		virtual void SetProjectName(Text::CString projName);
		virtual Text::String *GetContainerName();

		virtual UOSInt GetChildCount();
		virtual CodeObject *GetChildObj(UOSInt index);

		virtual void AddChild(Text::CodeObject *obj);
		VisualStudioVersion GetVSVersion();
	};
}
#endif
