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

		CException (ALERROR error, DWORD SEHCode, EXCEPTION_POINTERS* info) :
				m_error(error)
			{
			switch (SEHCode)
				{
				case (EXCEPTION_ACCESS_VIOLATION):
					{
					m_sMsg = CONSTLIT("Win32 Error: EXCEPTION_ACCESS_VIOLATION");
					for (DWORD i = 0; i < info->ExceptionRecord->NumberParameters; i++)
						{
						DWORD dwInfo = info->ExceptionRecord->ExceptionInformation[i];

						//	Figure out what to do with this parameter
						//  https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-exception_record
						// 
						//	First element is a flag indicating the operation.
						// Valid values are 0 (READ), 1 (WRITE), and 8 (DEP: DATA EXECUTION PREVENTION).
						// Anything else is probably memory corruption.

						if (i == 0)
							{
							switch (dwInfo)
								{
								case(0):
									{
									m_sMsg = strCat(m_sMsg, strPatternSubst(CONSTLIT(" - Operation: READ (%x)"), dwInfo));
									break;
									}
								case(1):
									{
									m_sMsg = strCat(m_sMsg, strPatternSubst(CONSTLIT(" - Operation: WRITE (%x)"), dwInfo));
									break;
									}
								case(8):
									{
									m_sMsg = strCat(m_sMsg, strPatternSubst(CONSTLIT(" - Operation: DEP Violation (%x)"), dwInfo));
									break;
									}
								default:
									{
									m_sMsg = strCat(m_sMsg, strPatternSubst(CONSTLIT(" - Operation: Undefined! (%x)"), dwInfo));
									break;
									}
								}
							}

						//	2nd element is the address of the access violation exception

						else if (i == 1)
							{
							m_sMsg = strCat(m_sMsg, strPatternSubst(CONSTLIT(" - ADDR: %x"), dwInfo));
							}

						//	everything after this if in the array is junk

						else
							{
							break;
							}
						}
					break;
					}
				case (EXCEPTION_ARRAY_BOUNDS_EXCEEDED):
					{
					m_sMsg = CONSTLIT("Win32 Error: EXCEPTION_ARRAY_BOUNDS_EXCEEDED");
					break;
					}
				case (EXCEPTION_BREAKPOINT):
					{
					m_sMsg = CONSTLIT("Win32 Error: EXCEPTION_BREAKPOINT");
					break;
					}
				case (EXCEPTION_DATATYPE_MISALIGNMENT):
					{
					m_sMsg = CONSTLIT("Win32 Error: EXCEPTION_DATATYPE_MISALIGNMENT");
					break;
					}
				case (EXCEPTION_FLT_DENORMAL_OPERAND):
					{
					m_sMsg = CONSTLIT("Win32 Error: EXCEPTION_FLT_DENORMAL_OPERAND");
					break;
					}
				case (EXCEPTION_FLT_DIVIDE_BY_ZERO):
					{
					m_sMsg = CONSTLIT("Win32 Error: EXCEPTION_FLT_DIVIDE_BY_ZERO");
					break;
					}
				case (EXCEPTION_FLT_INEXACT_RESULT):
					{
					m_sMsg = CONSTLIT("Win32 Error: EXCEPTION_FLT_INEXACT_RESULT");
					break;
					}
				case (EXCEPTION_FLT_INVALID_OPERATION):
					{
					m_sMsg = CONSTLIT("Win32 Error: EXCEPTION_FLT_INVALID_OPERATION");
					break;
					}
				case (EXCEPTION_FLT_OVERFLOW):
					{
					m_sMsg = CONSTLIT("Win32 Error: EXCEPTION_FLT_OVERFLOW");
					break;
					}
				case (EXCEPTION_FLT_STACK_CHECK):
					{
					m_sMsg = CONSTLIT("Win32 Error: EXCEPTION_FLT_STACK_CHECK");
					break;
					}
				case (EXCEPTION_FLT_UNDERFLOW):
					{
					m_sMsg = CONSTLIT("Win32 Error: EXCEPTION_FLT_UNDERFLOW");
					break;
					}
				case (EXCEPTION_GUARD_PAGE):
					{
					m_sMsg = CONSTLIT("Win32 Error: EXCEPTION_GUARD_PAGE");
					break;
					}
				case (EXCEPTION_ILLEGAL_INSTRUCTION):
					{
					m_sMsg = CONSTLIT("Win32 Error: EXCEPTION_ILLEGAL_INSTRUCTION");
					break;
					}
				case (EXCEPTION_IN_PAGE_ERROR):
					{
					m_sMsg = CONSTLIT("Win32 Error: EXCEPTION_IN_PAGE_ERROR");
					for (DWORD i = 0; i < info->ExceptionRecord->NumberParameters; i++)
						{
						DWORD dwInfo = info->ExceptionRecord->ExceptionInformation[i];

						//	Figure out what to do with this parameter
						//  https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-exception_record
						// 
						//	First element is a flag indicating the operation.
						// Valid values are 0 (READ), 1 (WRITE), and 8 (DEP: DATA EXECUTION PREVENTION).
						// Anything else is probably memory corruption.

						if (i == 0)
							{
							switch (dwInfo)
								{
								case(0):
								{
								m_sMsg = strCat(m_sMsg, strPatternSubst(CONSTLIT(" - Operation: READ (%x)"), dwInfo));
								break;
								}
								case(1):
								{
								m_sMsg = strCat(m_sMsg, strPatternSubst(CONSTLIT(" - Operation: WRITE (%x)"), dwInfo));
								break;
								}
								case(8):
								{
								m_sMsg = strCat(m_sMsg, strPatternSubst(CONSTLIT(" - Operation: DEP Violation (%x)"), dwInfo));
								break;
								}
								default:
								{
								m_sMsg = strCat(m_sMsg, strPatternSubst(CONSTLIT(" - Operation: Undefined! (%x)"), dwInfo));
								break;
								}
								}
							}

						//	2nd element is the address of the access violation exception

						else if (i == 1)
							{
							m_sMsg = strCat(m_sMsg, strPatternSubst(CONSTLIT(" - ADDR: %x"), dwInfo));
							}

						//	3rd element is the underlying NTSTATUS
						//
						//	There are genuinely way too many microsoft ones to add here
						//	Just manually look it up on this page:
						//	https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-erref/596a1078-e883-4972-9bbc-49e60bebca55
						//
						//	If the code doesnt exist there, it may be customer defined.
						//	We can output some basic metadata about the code too based on the code structure.
						//  https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-erref/87fba13e-bf06-450e-83b1-9241dc81e781
						//
						//	(Note that this doesnt care about endianness, it is always in this exact bit order
						//  Field masks:
						//  0b 1100 0000 0000 0000 0000 0000 0000 0000 - severity 00 = success, 01 = info, 10 = warning, 11 = error
						//  0b 0010 0000 0000 0000 0000 0000 0000 0000 - 0 = microsoft 1 = customer
						//  0b 0001 0000 0000 0000 0000 0000 0000 0000 - reserved (should always be 0!)
						//  0b 0000 1111 1111 1111 0000 0000 0000 0000 - facility - indicates numbering space for the code field
						//  0b 0000 0000 0000 0000 1111 1111 1111 1111 - code
						// 
						//	The architectures we support are little-endian though, so thats what we use for our bit masks
						//	

						else if (i == 2)
							{
							BYTE bySeverity = dwInfo >> 30 & 0x03;
							BYTE byCustomer = dwInfo >> 29 & 0x01;
							BYTE byReserved = dwInfo >> 28 & 0x01;	//	we check this bit in case of invalid customer-set codes or memory corruption
							WORD wFacility = dwInfo >> 16 & 0x0FFF;
							WORD wCode = dwInfo & 0x0000FFFF;

							CString sSeverity;
							switch (bySeverity)
								{
								case (0b00):
									{
									CONSTLIT("SUCCESS");
									break;
									}
								case (0b01):
									{
									CONSTLIT("INFO");
									break;
									}
								case (0b10):
									{
									CONSTLIT("WARNING");
									break;
									}
								case (0b11):
									{
									CONSTLIT("ERROR");
									break;
									}
								default:
									CONSTLIT("INVALID");
								}
							CString sCustomer = byCustomer ? CONSTLIT("Customer") : CONSTLIT("Microsoft");
							CString sBad = byReserved ? CONSTLIT("Invalid") : CONSTLIT("");
							m_sMsg = strCat(m_sMsg, strPatternSubst(CONSTLIT(" - NTSTATUS: %x %s (Code by: %s Severity: %s Facility: %x Code: %x)"), dwInfo));
							}

						//	everything after this if in the array is junk

						else
							{
							break;
							}
						}
					break;
					}
				case (EXCEPTION_INT_DIVIDE_BY_ZERO):
					{
					m_sMsg = CONSTLIT("Win32 Error: EXCEPTION_INT_DIVIDE_BY_ZERO");
					break;
					}
				case (EXCEPTION_INT_OVERFLOW):
					{
					m_sMsg = CONSTLIT("Win32 Error: EXCEPTION_INT_OVERFLOW");
					break;
					}
				case (EXCEPTION_INVALID_DISPOSITION):
					{
					m_sMsg = CONSTLIT("Win32 Error: EXCEPTION_INVALID_DISPOSITION");
					break;
					}
				case (EXCEPTION_INVALID_HANDLE):
					{
					m_sMsg = CONSTLIT("Win32 Error: EXCEPTION_INVALID_HANDLE");
					break;
					}
				case (EXCEPTION_NONCONTINUABLE_EXCEPTION):
					{
					m_sMsg = CONSTLIT("Win32 Error: EXCEPTION_NONCONTINUABLE_EXCEPTION");
					break;
					}
				case (EXCEPTION_PRIV_INSTRUCTION):
					{
					m_sMsg = CONSTLIT("Win32 Error: EXCEPTION_PRIV_INSTRUCTION");
					break;
					}
				case (EXCEPTION_SINGLE_STEP):
					{
					m_sMsg = CONSTLIT("Win32 Error: EXCEPTION_SINGLE_STEP");
					break;
					}
				case (EXCEPTION_STACK_OVERFLOW):
					{
					m_sMsg = CONSTLIT("Win32 Error: EXCEPTION_STACK_OVERFLOW");
					break;
					}
				case (STATUS_UNWIND_CONSOLIDATE):
					{
					m_sMsg = CONSTLIT("Win32 Error: STATUS_UNWIND_CONSOLIDATE");
					break;
					}
				default:
					m_sMsg = strPatternSubst(CONSTLIT("Win32 Error: Unidentified SEH error code: %x"), SEHCode);
				}
			}

		ALERROR GetErrorCode (void) const { return m_error; }
		CString GetErrorMessage (void) const;

	private:
		ALERROR m_error;
		CString m_sMsg;
	};

