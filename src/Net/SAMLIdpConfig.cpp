#include "Stdafx.h"
#include "Net/SAMLIdpConfig.h"
#include "Net/URL.h"
#include "Text/XMLReader.h"

Net::SAMLIdpConfig::SAMLIdpConfig(Text::CStringNN signOnLocation, Text::CStringNN logoutLocation, Optional<Crypto::Cert::X509Cert> encryptionCert, Optional<Crypto::Cert::X509Cert> signingCert)
{
	this->signOnLocation = Text::String::New(signOnLocation);
	this->logoutLocation = Text::String::New(logoutLocation);
	this->encryptionCert = encryptionCert;
	this->signingCert = signingCert;
}

Net::SAMLIdpConfig::~SAMLIdpConfig()
{
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
	if (!Net::URL::OpenStream(path, 0, clif, ssl, 10000, log).SetTo(stm))
	{
		return 0;
	}
	{
		Text::XMLReader reader(encFact, stm, Text::XMLReader::PM_XML);
		if (reader.NextElementName().SetTo(s))
		{
			if (s->Equals(CSTR("EntityDescriptor")))
			{
				while (reader.NextElementName().SetTo(s))
				{
					if (s->Equals(CSTR("IDPSSODescriptor")))
					{
						Optional<Text::String> signOnLocation = 0;
						Optional<Text::String> logoutLocation = 0;
						NN<Text::String> nnsignOnLocation;
						NN<Text::String> nnlogoutLocation;
						Optional<Crypto::Cert::X509Cert> encryptionCert = 0;
						Optional<Crypto::Cert::X509Cert> signingCert = 0;
						UOSInt type;
						while (reader.NextElementName().SetTo(s))
						{
							if (s->Equals(CSTR("KeyDescriptor")))
							{
								reader.SkipElement();
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
						if (signOnLocation.SetTo(nnsignOnLocation) && logoutLocation.SetTo(nnlogoutLocation))
						{
							NN<SAMLIdpConfig> cfg;
							NEW_CLASSNN(cfg, SAMLIdpConfig(nnsignOnLocation->ToCString(), nnlogoutLocation->ToCString(), encryptionCert, signingCert));
							nnsignOnLocation->Release();
							nnlogoutLocation->Release();
							stm.Delete();
							return cfg;
						}
						OPTSTR_DEL(signOnLocation);
						OPTSTR_DEL(logoutLocation);
						encryptionCert.Delete();
						signingCert.Delete();
					}
					else
					{
						reader.SkipElement();
					}
				}
			}
		}
	}
	stm.Delete();
	return 0;
}
