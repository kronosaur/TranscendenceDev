//	TSmartPtr.h
//
//	TSmartPtr Class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

template <class OBJ> class TSharedPtr
	{
	public:
		constexpr TSharedPtr (void) : m_pPtr(NULL) { }
		constexpr TSharedPtr (std::nullptr_t) : m_pPtr(NULL) { }

		explicit TSharedPtr (OBJ *pPtr) : m_pPtr(pPtr)

		TSharedPtr (const TSharedPtr<OBJ> &Src)
			{
			if (Src.m_pPtr)
				m_pPtr = Src.m_pPtr->AddRef();
			else
				m_pPtr = NULL;
			}

		~TSharedPtr (void)
			{
			if (m_pPtr)
				m_pPtr->Delete();
			}

		TSharedPtr<OBJ> &operator= (const TSharedPtr<OBJ> &Src)
			{
			if (m_pPtr)
				m_pPtr->Delete();

			if (Src.m_pPtr)
				m_pPtr = Src.m_pPtr->AddRef();
			else
				m_pPtr = NULL;

			return *this;
			}

		operator OBJ *() const { return m_pPtr; }
		OBJ * operator->() const { return m_pPtr; }

		explicit operator bool() const { return (m_pPtr != NULL); }

	private:
		OBJ *m_pPtr;
	};
