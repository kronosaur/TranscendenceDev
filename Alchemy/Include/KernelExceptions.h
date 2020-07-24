//	KernelExceptions.h
//
//	Kernel definitions.
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

//	Exceptions

class CException
	{
	public:
		CException (ALERROR error, const CString &sMsg = NULL_STR) : 
				m_error(error),
				m_sMsg(sMsg)
			{ }

		ALERROR GetErrorCode (void) const { return m_error; }
		CString GetErrorMessage (void) const;

	private:
		ALERROR m_error;
		CString m_sMsg;
	};

