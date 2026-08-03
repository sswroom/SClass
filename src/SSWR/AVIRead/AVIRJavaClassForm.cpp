#include "Stdafx.h"
#include "IO/Java/JavaAnnotation.h"
#include "IO/Java/JavaArrayValue.h"
#include "IO/Java/JavaMethod.h"
#include "IO/Java/JavaStringValue.h"
#include "Net/WebUtil.h"
#include "SSWR/AVIRead/AVIRJavaClassForm.h"

void __stdcall SSWR::AVIRead::AVIRJavaClassForm::OnMethodsSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRJavaClassForm> me = userObj.GetNN<SSWR::AVIRead::AVIRJavaClassForm>();
	Text::StringBuilderUTF8 sb;
	me->clsFile->MethodsGetDetail((UIntOS)me->lbMethods->GetSelectedIndex(), 0, true, sb);
	me->txtMethods->SetText(sb.ToCString());
}

SSWR::AVIRead::AVIRJavaClassForm::AVIRJavaClassForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IO::Java::JavaClass> clsFile) : UI::GUIForm(parent, 1024, 768, ui)
{
	Text::StringBuilderUTF8 sb;
	this->SetFont(nullptr, 8.25, false);
	this->clsFile = clsFile;
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	sb.AppendC(UTF8STRC("Java Class - "));
	sb.Append(clsFile->GetSourceNameObj());
	this->SetText(sb.ToCString());

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpFileStruct = this->tcMain->AddTabPage(CSTR("File Structure"));
	this->txtFileStruct = ui->NewTextBox(this->tpFileStruct, CSTR(""), true);
	this->txtFileStruct->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtFileStruct->SetReadOnly(true);
	sb.ClearStr();
	this->clsFile->FileStructDetail(sb);
	this->txtFileStruct->SetText(sb.ToCString());

	this->tpFields = this->tcMain->AddTabPage(CSTR("Fields"));
	this->lbFields = ui->NewListBox(this->tpFields, false);
	this->lbFields->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpMethods = this->tcMain->AddTabPage(CSTR("Methods"));
	this->lbMethods = ui->NewListBox(this->tpMethods, false);
	this->lbMethods->SetRect(0, 0, 100, 300, false);
	this->lbMethods->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lbMethods->HandleSelectionChange(OnMethodsSelChg, this);
	this->vspMethods = ui->NewVSplitter(this->tpMethods, 3, false);
	this->txtMethods = ui->NewTextBox(this->tpMethods, CSTR(""), true);
	this->txtMethods->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtMethods->SetReadOnly(true);

	this->tpDecompile = this->tcMain->AddTabPage(CSTR("Decompile"));
	this->txtDecompile = ui->NewTextBox(this->tpDecompile, CSTR(""), true);
	this->txtDecompile->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtDecompile->SetReadOnly(true);

	this->ctrlRest = nullptr;

	UIntOS i = 0;
	UIntOS j = this->clsFile->FieldsGetCount();
	UIntOS k;
	while (i < j)
	{
		sb.ClearStr();
		this->clsFile->FieldsGetDecl(i, sb);
		this->lbFields->AddItem(sb.ToCString(), (void*)i);
		i++;
	}

	i = 0;
	j = this->clsFile->MethodsGetCount();
	while (i < j)
	{
		sb.ClearStr();
		this->clsFile->MethodsGetDecl(i, sb);
		this->lbMethods->AddItem(sb.ToCString(), (void*)i);
		i++;
	}

	sb.ClearStr();
	this->clsFile->DecompileFile(sb);
	this->txtDecompile->SetText(sb.ToCString());

	UnsafeArray<UInt8> annoPtr;
	UInt32 annoLen;
	if (this->clsFile->GetAnnotations(annoLen).SetTo(annoPtr))
	{
		Data::ArrayListNN<IO::Java::JavaAnnotation> annoList;
		IO::Java::JavaElementValue::ParseAnnotations(annoList, this->clsFile, annoPtr, annoPtr + annoLen);
		NN<IO::Java::JavaAnnotation> anno;
		Bool isRestController = false;
		Optional<Text::String> mapPath = nullptr;
		i = 0;
		j = annoList.GetCount();
		while (i < j)
		{
			anno = annoList.GetItemNoCheck(i);
			NN<Text::String> typeStr = anno->GetAnnoType()->GetTypeStr();
			if (typeStr->Equals(UTF8STRC("Lorg/springframework/web/bind/annotation/RestController;")))
			{
				isRestController = true;
			}
			else if (typeStr->Equals(UTF8STRC("Lorg/springframework/web/bind/annotation/RequestMapping;")))
			{
				NN<IO::Java::JavaElementValue> value;
				if (anno->GetValue(CSTR("value")).SetTo(value))
				{
					if (value->GetElementType() == IO::Java::JavaElementValue::ElementType::Array)
					{
						NN<IO::Java::JavaArrayValue> arr = NN<IO::Java::JavaArrayValue>::ConvertFrom(value);
						if (arr->GetItem(0).SetTo(value))
						{
							if (value->GetElementType() == IO::Java::JavaElementValue::ElementType::String)
							{
								OPTSTR_DEL(mapPath);
								mapPath = NN<IO::Java::JavaStringValue>::ConvertFrom(value)->GetString()->Clone();
							}
						}
					}
					else if (value->GetElementType() == IO::Java::JavaElementValue::ElementType::String)
					{
						OPTSTR_DEL(mapPath);
						mapPath = NN<IO::Java::JavaStringValue>::ConvertFrom(value)->GetString()->Clone();
					}
				}
			}
			i++;
		}
		annoList.DeleteAll();
		if (isRestController)
		{
			NN<Controller> ctrl;
			ctrl = MemAllocNN(Controller);
			this->ctrlRest = ctrl;
			ctrl->tp = this->tcMain->AddTabPage(CSTR("Controller"));
			ctrl->pnlPath = ui->NewPanel(ctrl->tp);
			ctrl->pnlPath->SetRect(0, 0, 100, 31, false);
			ctrl->pnlPath->SetDockType(UI::GUIControl::DOCK_TOP);
			ctrl->lblPath = ui->NewLabel(ctrl->pnlPath, CSTR("Path:"));
			ctrl->lblPath->SetRect(4, 4, 100, 23, false);
			ctrl->txtPath = ui->NewTextBox(ctrl->pnlPath, CSTR(""));
			ctrl->txtPath->SetRect(104, 4, 400, 23, false);
			ctrl->txtPath->SetReadOnly(true);
			ctrl->lvMain = ui->NewListView(ctrl->tp, UI::ListViewStyle::Table, 4);
			ctrl->lvMain->SetDockType(UI::GUIControl::DOCK_FILL);
			ctrl->lvMain->AddColumn(CSTR("Path"), 200);
			ctrl->lvMain->AddColumn(CSTR("Method"), 80);
			ctrl->lvMain->AddColumn(CSTR("Return Type"), 200);
			ctrl->lvMain->AddColumn(CSTR("Method"), 500);
			ctrl->mapPath = mapPath;
			NN<Text::String> path;
			if (mapPath.SetTo(path))
			{
				ctrl->txtPath->SetText(path->ToCString());
			}

			Text::StringBuilderUTF8 sb;
			Data::ArrayListStringNN importList;
			i = 0;
			j = this->clsFile->GetMethodCount();
			while (i < j)
			{
				UnsafeArray<UInt8> methodPtr;
				NN<IO::Java::JavaMethod> method;
				NN<IO::Java::JavaAnnotation> anno;
				if (this->clsFile->GetMethod(i).SetTo(methodPtr))
				{
					if (IO::Java::JavaMethod::ParseMethod(this->clsFile, methodPtr).SetTo(method))
					{
						k = method->GetAnnotationCount();
						while (k-- > 0)
						{
							if (method->GetAnnotation(k).SetTo(anno))
							{
								Net::WebUtil::RequestMethod reqMethod = Net::WebUtil::RequestMethod::Unknown;
								if (anno->GetAnnoType()->GetTypeStr()->Equals(UTF8STRC("Lorg/springframework/web/bind/annotation/GetMapping;")))
								{
									reqMethod = Net::WebUtil::RequestMethod::HTTP_GET;
								}
								else if (anno->GetAnnoType()->GetTypeStr()->Equals(UTF8STRC("Lorg/springframework/web/bind/annotation/PostMapping;")))
								{
									reqMethod = Net::WebUtil::RequestMethod::HTTP_POST;
								}
								else if (anno->GetAnnoType()->GetTypeStr()->Equals(UTF8STRC("Lorg/springframework/web/bind/annotation/PutMapping;")))
								{
									reqMethod = Net::WebUtil::RequestMethod::HTTP_PUT;
								}
								else if (anno->GetAnnoType()->GetTypeStr()->Equals(UTF8STRC("Lorg/springframework/web/bind/annotation/DeleteMapping;")))
								{
									reqMethod = Net::WebUtil::RequestMethod::HTTP_DELETE;
								}
								else if (anno->GetAnnoType()->GetTypeStr()->Equals(UTF8STRC("Lorg/springframework/web/bind/annotation/PatchMapping;")))
								{
									reqMethod = Net::WebUtil::RequestMethod::HTTP_PATCH;
								}
								if (reqMethod != Net::WebUtil::RequestMethod::Unknown)
								{
									NN<IO::Java::JavaElementValue> value;
									if (anno->GetValue(CSTR("value")).SetTo(value))
									{
										if (value->GetElementType() == IO::Java::JavaElementValue::ElementType::Array)
										{
											NN<IO::Java::JavaArrayValue>::ConvertFrom(value)->GetItem(0).SetTo(value);
										}

										if (value->GetElementType() == IO::Java::JavaElementValue::ElementType::String)
										{
											sb.ClearStr();
											sb.AppendOpt(mapPath);
											sb.Append(NN<IO::Java::JavaStringValue>::ConvertFrom(value)->GetString());
											k = ctrl->lvMain->AddItem(sb.ToCString(), 0);
											ctrl->lvMain->SetSubItem(k, 1, Net::WebUtil::RequestMethodGetName(reqMethod));
											sb.ClearStr();
											method->GetReturnType()->ToString(sb, importList, nullptr);
											ctrl->lvMain->SetSubItem(k, 2, sb.ToCString());
											sb.ClearStr();
											method->ToDeclarationNameParams(sb, importList, nullptr);
											ctrl->lvMain->SetSubItem(k, 3, sb.ToCString());
										}
									}
									break;
								}
							}
						}
						method.Delete();
					}
				}
				i++;
			}
			importList.FreeAll();
		}
		else
		{
			OPTSTR_DEL(mapPath);
		}
	}
}

SSWR::AVIRead::AVIRJavaClassForm::~AVIRJavaClassForm()
{
	this->clsFile.Delete();
	NN<Controller> ctrl;
	if (this->ctrlRest.SetTo(ctrl))
	{
		OPTSTR_DEL(ctrl->mapPath);
		MemFreeNN(ctrl);
	}
}

void SSWR::AVIRead::AVIRJavaClassForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
