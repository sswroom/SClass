#include "Stdafx.h"
#include "Net/SAMLIdpConfig.h"
#include "Net/URL.h"
#include "Parser/FileParser/X509Parser.h"
#include "Text/XMLReader.h"
#include "Text/TextBinEnc/Base64Enc.h"

Net::SAMLIdpConfig::SAMLIdpConfig(Text::CStringNN serviceDispName, Text::CStringNN entityId, Text::CStringNN signOnLocation, Text::CStringNN logoutLocation, Optional<Crypto::Cert::X509Cert> encryptionCert, Optional<Crypto::Cert::X509Cert> signingCert)
{
	this->serviceDispName = Text::String::New(serviceDispName);
	this->entityId = Text::String::New(entityId);
	this->signOnLocation = Text::String::New(signOnLocation);
	this->logoutLocation = Text::String::New(logoutLocation);
	this->encryptionCert = encryptionCert;
	this->signingCert = signingCert;
}

Net::SAMLIdpConfig::~SAMLIdpConfig()
{
	this->serviceDispName->Release();
	this->entityId->Release();
	this->signOnLocation->Release();
	this->logoutLocation->Release();
	this->encryptionCert.Delete();
	this->signingCert.Delete();
}

Optional<Net::SAMLIdpConfig> Net::SAMLIdpConfig::ParseMetadata(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, NN<Text::EncodingFactory> encFact, Text::CStringNN path)
{
	IO::LogTool log;
	NN<IO::Stream> stm;
	NN<Text::String> s;
	UOSInt i;
	UOSInt j;
	NN<Text::XMLAttrib> attr;
	Text::StringBuilderUTF8 sb;
	UnsafeArray<UInt8> buff;
	UOSInt buffSize;
	NN<Crypto::Cert::X509File> file;
	if (!Net::URL::OpenStream(path, 0, clif, ssl, 10000, log).SetTo(stm))
	{
		return nullptr;
	}
	{
		Text::XMLReader reader(encFact, stm, Text::XMLReader::PM_XML);
		if (reader.NextElementName().SetTo(s))
		{
			if (s->Equals(CSTR("EntityDescriptor")))
			{
				Optional<Text::String> serviceDispName = nullptr;
				Optional<Text::String> entityId = nullptr;
				Optional<Text::String> signOnLocation = nullptr;
				Optional<Text::String> logoutLocation = nullptr;
				NN<Text::String> nnserviceDispName;
				NN<Text::String> nnentityId;
				NN<Text::String> nnsignOnLocation;
				NN<Text::String> nnlogoutLocation;
				Optional<Crypto::Cert::X509Cert> encryptionCert = nullptr;
				Optional<Crypto::Cert::X509Cert> signingCert = nullptr;
				UOSInt type;
				i = 0;
				j = reader.GetAttribCount();
				while (i < j)
				{
					attr = reader.GetAttribNoCheck(i);
					if (attr->name.SetTo(s) && s->Equals(CSTR("entityID")))
					{
						if (attr->value.SetTo(s))
						{
							OPTSTR_DEL(entityId);
							entityId = s->Clone();
						}
					}
					i++;
				}
				while (reader.NextElementName().SetTo(s))
				{
					if (s->Equals(CSTR("RoleDescriptor")))
					{
						i = 0;
						j = reader.GetAttribCount();
						while (i < j)
						{
							attr = reader.GetAttribNoCheck(i);
							if (attr->name.SetTo(s) && s->Equals(CSTR("ServiceDisplayName")))
							{
								if (attr->value.SetTo(s))
								{
									OPTSTR_DEL(serviceDispName);
									serviceDispName = s->Clone();
								}
							}
							i++;
						}
						reader.SkipElement();
					}
					else if (s->Equals(CSTR("IDPSSODescriptor")))
					{
						while (reader.NextElementName().SetTo(s))
						{
							if (s->Equals(CSTR("KeyDescriptor")))
							{
								type = 0;
								i = 0;
								j = reader.GetAttribCount();
								while (i < j)
								{
									attr = reader.GetAttribNoCheck(i);
									if (attr->name.SetTo(s) && s->Equals(CSTR("use")))
									{
										if (attr->value.SetTo(s) && s->Equals(CSTR("encryption")))
										{
											type = 1;
										}
										else if (attr->value.SetTo(s) && s->Equals(CSTR("signing")))
										{
											type = 2;
										}
									}
									i++;
								}
								while (reader.NextElementName().SetTo(s))
								{
									if (s->Equals(CSTR("KeyInfo")))
									{
										while (reader.NextElementName().SetTo(s))
										{
											if (s->Equals(CSTR("X509Data")))
											{
												while (reader.NextElementName().SetTo(s))
												{
													if (s->Equals(CSTR("X509Certificate")))
													{
														sb.ClearStr();
														reader.ReadNodeText(sb);
														if (type == 1)
														{
															Text::TextBinEnc::Base64Enc b64;
															buffSize = b64.CalcBinSize(sb.ToCString());
															if (buffSize >= 20)
															{
																buff = MemAllocArr(UInt8, buffSize);
																b64.DecodeBin(sb.ToCString(), buff);
																if (Parser::FileParser::X509Parser::ParseBinary(Data::ByteArray(buff, buffSize)).SetTo(file))
																{
																	if (file->GetFileType() == Crypto::Cert::X509File::FileType::Cert)
																	{
																		encryptionCert.Delete();
																		encryptionCert = NN<Crypto::Cert::X509Cert>::ConvertFrom(file);
																	}
																	else
																	{
																		file.Delete();
																	}
																}
																MemFreeArr(buff);
															}
														}
														else if (type == 2)
														{
															Text::TextBinEnc::Base64Enc b64;
															buffSize = b64.CalcBinSize(sb.ToCString());
															if (buffSize >= 20)
															{
																buff = MemAllocArr(UInt8, buffSize);
																b64.DecodeBin(sb.ToCString(), buff);
																if (Parser::FileParser::X509Parser::ParseBinary(Data::ByteArray(buff, buffSize)).SetTo(file))
																{
																	if (file->GetFileType() == Crypto::Cert::X509File::FileType::Cert)
																	{
																		signingCert.Delete();
																		signingCert = NN<Crypto::Cert::X509Cert>::ConvertFrom(file);
																	}
																	else
																	{
																		file.Delete();
																	}
																}
																MemFreeArr(buff);
															}
														}
													}
													else
													{
														reader.SkipElement();
													}
												}
											}
											else
											{
												reader.SkipElement();
											}
										}
									}
									else
									{
										reader.SkipElement();
									}
								}
							}
							else if (s->Equals(CSTR("SingleLogoutService")))
							{
								type = 0;
								i = 0;
								j = reader.GetAttribCount();
								while (i < j)
								{
									attr = reader.GetAttribNoCheck(i);
									if (attr->name.SetTo(s) && s->Equals(CSTR("Binding")))
									{
										if (attr->value.SetTo(s) && s->Equals(CSTR("urn:oasis:names:tc:SAML:2.0:bindings:HTTP-Redirect")))
										{
											type = 1;
										}
									}
									else if (attr->name.SetTo(s) && s->Equals(CSTR("Location")))
									{
										if (type == 1 && attr->value.SetTo(s))
										{
											OPTSTR_DEL(logoutLocation);
											logoutLocation = s->Clone();
										}
									}
									i++;
								}
								reader.SkipElement();
							}
							else if (s->Equals(CSTR("SingleSignOnService")))
							{
								type = 0;
								i = 0;
								j = reader.GetAttribCount();
								while (i < j)
								{
									attr = reader.GetAttribNoCheck(i);
									if (attr->name.SetTo(s) && s->Equals(CSTR("Binding")))
									{
										if (attr->value.SetTo(s) && s->Equals(CSTR("urn:oasis:names:tc:SAML:2.0:bindings:HTTP-Redirect")))
										{
											type = 1;
										}
									}
									else if (attr->name.SetTo(s) && s->Equals(CSTR("Location")))
									{
										if (type == 1 && attr->value.SetTo(s))
										{
											OPTSTR_DEL(signOnLocation);
											signOnLocation = s->Clone();
										}
									}
									i++;
								}
								reader.SkipElement();
							}
							else
							{
								reader.SkipElement();
							}
						}
					}
					else
					{
						reader.SkipElement();
					}
				}
				if (serviceDispName.SetTo(nnserviceDispName) && entityId.SetTo(nnentityId) && signOnLocation.SetTo(nnsignOnLocation) && logoutLocation.SetTo(nnlogoutLocation))
				{
					NN<SAMLIdpConfig> cfg;
					NEW_CLASSNN(cfg, SAMLIdpConfig(nnserviceDispName->ToCString(), nnentityId->ToCString(), nnsignOnLocation->ToCString(), nnlogoutLocation->ToCString(), encryptionCert, signingCert));
					nnserviceDispName->Release();
					nnentityId->Release();
					nnsignOnLocation->Release();
					nnlogoutLocation->Release();
					stm.Delete();
					return cfg;
				}
				OPTSTR_DEL(serviceDispName);
				OPTSTR_DEL(entityId);
				OPTSTR_DEL(signOnLocation);
				OPTSTR_DEL(logoutLocation);
				encryptionCert.Delete();
				signingCert.Delete();
			}
		}
	}
	stm.Delete();
	return nullptr;
}
