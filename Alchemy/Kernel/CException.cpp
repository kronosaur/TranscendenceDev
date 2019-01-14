//	CException.cpp
//
//	Implements CException object
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CString CException::GetErrorMessage (void) const

//	GetErrorMessage
//
//	Returns the error message.

	{
	//	If we have a user-provided message, return that.

	if (!m_sMsg.IsBlank())
		return m_sMsg;

	//	Otherwise, do it based on the code

	switch (m_error)
		{
		case ERR_MEMORY:
			return CONSTLIT("Out of memory.");

		case ERR_ENDOFFILE:
			return CONSTLIT("Unexpected end of stream.");

		case ERR_CANCEL:
			return CONSTLIT("Operation cancelled by the user.");

		case ERR_NOTFOUND:
			return CONSTLIT("Not found.");

		case ERR_FILEOPEN:
			return CONSTLIT("Unable to open file.");

		case ERR_WIN32_EXCEPTION:
			return CONSTLIT("Win32 exception.");

		default:
			return CONSTLIT("Unexpected error.");
		}
	}
