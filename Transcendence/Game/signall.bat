@rem "signtool.exe" sign /f "c:\users\GPM\Documents\Kronosaur\Keys\Kronosaur Productions Code Signing.pfx" /p %1 /t http://timestamp.verisign.com/scripts/timestamp.dll /v "Transcendence.exe" "TransData.exe" "TransCompiler.exe" "TransWorkshop.exe"
@rem "signtool.exe" sign /f "c:\users\GPM\Documents\Kronosaur\Keys\Kronosaur Productions Code Signing.pfx" /p %1 /t http://timestamp.comodoca.com/authenticode /v "Transcendence.exe" "TransData.exe" "TransCompiler.exe" "TransWorkshop.exe"

@rem NOTE: Must have the USB key inserted, and have SafeNet Authentication Client installed: https://sectigo.com/knowledge-base/detail/SafeNet-Download-for-Sectigo-EV-Code-Document-Signing-Certificates-using-an-EToken-Smartcard/kA03l000000o6kL
@rem After SAC software installed, export the certificates to the local store, but leave the USB key in (because it has the private keys). Password is zip+CX asset propercase (and is set on the USB key)

@"signtool.exe" sign /fd SHA256 /sha1 617d44857fd173f7f87877d6a6ece3cfe605c33f /t http://timestamp.comodoca.com/authenticode /v "Transcendence.exe" "TransData.exe" "TransCompiler.exe" "TransWorkshop.exe"
