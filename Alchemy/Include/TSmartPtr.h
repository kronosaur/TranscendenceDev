//	TSmartPtr.h
//
//	TSmartPtr Class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

#include <type_traits>

template <class OBJ> class TRefCounted
	{
	public:
		OBJ *AddRef (void)
			{
			m_dwRefCount++;
			return (OBJ *)this; 
			}

		void Delete (void)
			{
			if (--m_dwRefCount <= 0)
				delete (OBJ *)this;
			}

	private:
		DWORD m_dwRefCount = 1;
	};

template <class OBJ> class TSharedPtr
	{
	public:
		constexpr TSharedPtr (void) : m_pPtr(NULL) { }
		constexpr TSharedPtr (std::nullptr_t) : m_pPtr(NULL) { }

		explicit TSharedPtr (OBJ *pPtr) : m_pPtr(pPtr) { }

		TSharedPtr (const TSharedPtr<OBJ> &Src)
			{
			if (Src.m_pPtr)
				m_pPtr = Src.m_pPtr->AddRef();
			else
				m_pPtr = NULL;
			}

		TSharedPtr (TSharedPtr<OBJ> &&Src) : m_pPtr(Src.m_pPtr)
			{
			Src.m_pPtr = NULL;
			}

		template<class OBJ2,
				class = typename std::enable_if<std::is_convertible<OBJ2 *, OBJ *>::value, void>::type>
		TSharedPtr (const TSharedPtr<OBJ2>& Src) _NOEXCEPT
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

		TSharedPtr<OBJ> &operator= (OBJ *pSrc)
			{
			if (m_pPtr)
				m_pPtr->Delete();

			if (pSrc)
				m_pPtr = pSrc->AddRef();
			else
				m_pPtr = NULL;

			return *this;
			}

		operator OBJ *() const { return m_pPtr; }
		OBJ * operator->() const { return m_pPtr; }

		explicit operator bool() const { return (m_pPtr != NULL); }

		void Delete (void) { if (m_pPtr) m_pPtr->Delete(); m_pPtr = NULL; }

		void Set (const TSharedPtr<OBJ> &Src)
			{
			*this = Src;
			}

		void TakeHandoff (OBJ *pPtr)
			{
			if (m_pPtr)
				m_pPtr->Delete();

			m_pPtr = pPtr;
			}

		void TakeHandoff (TSharedPtr<OBJ> &Src)
			{
			if (m_pPtr)
				m_pPtr->Delete();

			m_pPtr = Src.m_pPtr;
			Src.m_pPtr = NULL;
			}

	private:
		OBJ *m_pPtr;
	};

template <class OBJ> bool operator== (const TSharedPtr<OBJ> &lhs, const TSharedPtr<OBJ> &rhs) { return (OBJ *)lhs == (OBJ *)rhs; }
template <class OBJ> bool operator!= (const TSharedPtr<OBJ> &lhs, const TSharedPtr<OBJ> &rhs) { return (OBJ *)lhs != (OBJ *)rhs; }

template <class OBJ> bool operator== (const TSharedPtr<OBJ> &lhs, int rhs) { return ((int)(OBJ *)lhs == rhs); }
template <class OBJ> bool operator== (int lhs, const TSharedPtr<OBJ> &rhs) { return (lhs == (int)(OBJ *)rhs); }
template <class OBJ> bool operator== (const TSharedPtr<OBJ> &lhs, std::nullptr_t rhs) { return !(bool)lhs; }
template <class OBJ> bool operator== (std::nullptr_t lhs, const TSharedPtr<OBJ> &rhs) { return !(bool)rhs; }

template <class OBJ> bool operator!= (const TSharedPtr<OBJ> &lhs, int rhs) { return ((int)(OBJ *)lhs != rhs); }
template <class OBJ> bool operator!= (int lhs, const TSharedPtr<OBJ> &rhs) { return (lhs != (int)(OBJ *)rhs); }
template <class OBJ> bool operator!= (const TSharedPtr<OBJ> &lhs, std::nullptr_t rhs) { return (bool)lhs; }
template <class OBJ> bool operator!= (std::nullptr_t lhs, const TSharedPtr<OBJ> &rhs) { return (bool)rhs; }

template <class OBJ> class TUniquePtr
	{
	public:
		constexpr TUniquePtr (void) : m_pPtr(NULL) { }
		constexpr TUniquePtr (std::nullptr_t) : m_pPtr(NULL) { }

		explicit TUniquePtr (OBJ *pPtr) : m_pPtr(pPtr) { }

		TUniquePtr (const TUniquePtr<OBJ> &Src)
			{
			if (Src.m_pPtr)
				m_pPtr = new OBJ(*Src.m_pPtr);
			else
				m_pPtr = NULL;
			}

		TUniquePtr (TUniquePtr<OBJ> &&Src) : m_pPtr(Src.m_pPtr)
			{
			Src.m_pPtr = NULL;
			}

		~TUniquePtr (void)
			{
			if (m_pPtr)
				delete m_pPtr;
			}

		TUniquePtr<OBJ> &operator= (const TUniquePtr<OBJ> &Src)
			{
			if (m_pPtr)
				delete m_pPtr;

			if (Src.m_pPtr)
				m_pPtr = new OBJ(*Src.m_pPtr);
			else
				m_pPtr = NULL;

			return *this;
			}

		operator OBJ *() const { return m_pPtr; }
		OBJ * operator->() const { return m_pPtr; }

		explicit operator bool() const { return (m_pPtr != NULL); }

		void Delete (void) { Set(NULL); }

		OBJ *GetHandoff (void)
			{
			OBJ *pPtr = m_pPtr;
			m_pPtr = NULL;
			return pPtr;
			}

		void Set (OBJ *pPtr)
			{
			if (m_pPtr)
				delete m_pPtr;

			m_pPtr = pPtr;
			}

		void Set (const TUniquePtr<OBJ> &Src)
			{
			*this = Src;
			}

		void TakeHandoff (TUniquePtr<OBJ> &Src)
			{
			if (m_pPtr)
				delete m_pPtr;

			m_pPtr = Src.m_pPtr;
			Src.m_pPtr = NULL;
			}

		void TakeHandoff (OBJ *&pPtr)
			{
			if (m_pPtr)
				delete m_pPtr;

			m_pPtr = pPtr;
			pPtr = NULL;
			}

	private:
		OBJ *m_pPtr;
	};

template <class OBJ> bool operator== (const TUniquePtr<OBJ> &lhs, const TUniquePtr<OBJ> &rhs) { return (OBJ *)lhs == (OBJ *)rhs; }
template <class OBJ> bool operator!= (const TUniquePtr<OBJ> &lhs, const TUniquePtr<OBJ> &rhs) { return (OBJ *)lhs != (OBJ *)rhs; }

template <class OBJ> bool operator== (const TUniquePtr<OBJ> &lhs, int rhs) { return ((int)(OBJ *)lhs == rhs); }
template <class OBJ> bool operator== (int lhs, const TUniquePtr<OBJ> &rhs) { return (lhs == (int)(OBJ *)rhs); }
template <class OBJ> bool operator== (const TUniquePtr<OBJ> &lhs, std::nullptr_t rhs) { return !(bool)lhs; }
template <class OBJ> bool operator== (std::nullptr_t lhs, const TUniquePtr<OBJ> &rhs) { return !(bool)rhs; }

template <class OBJ> bool operator!= (const TUniquePtr<OBJ> &lhs, int rhs) { return ((int)(OBJ *)lhs != rhs); }
template <class OBJ> bool operator!= (int lhs, const TUniquePtr<OBJ> &rhs) { return (lhs != (int)(OBJ *)rhs); }
template <class OBJ> bool operator!= (const TUniquePtr<OBJ> &lhs, std::nullptr_t rhs) { return (bool)lhs; }
template <class OBJ> bool operator!= (std::nullptr_t lhs, const TUniquePtr<OBJ> &rhs) { return (bool)rhs; }

template <class OBJ> class TUniquePtr<OBJ[]>
	{
	public:
		constexpr TUniquePtr (void) : m_pPtr(NULL) { }
		constexpr TUniquePtr (std::nullptr_t) : m_pPtr(NULL) { }

		explicit TUniquePtr (OBJ *pPtr) : m_pPtr(pPtr) { }

		TUniquePtr (const TUniquePtr<OBJ> &Src) =delete;

		TUniquePtr (TUniquePtr<OBJ> &&Src) : m_pPtr(Src.m_pPtr)
			{
			Src.m_pPtr = NULL;
			}

		~TUniquePtr (void)
			{
			if (m_pPtr)
				delete [] m_pPtr;
			}

		TUniquePtr<OBJ> &operator= (const TUniquePtr<OBJ> &Src) =delete;
		TUniquePtr<OBJ[]> &operator= (const TUniquePtr<OBJ[]> &Src) =delete;

		OBJ & operator[](size_t i) const { return m_pPtr[i]; }

		explicit operator bool() const { return (m_pPtr != NULL); }

		void Delete (void) { Set(NULL); }

		void Set (OBJ *pPtr)
			{
			if (m_pPtr)
				delete [] m_pPtr;

			m_pPtr = pPtr;
			}

	private:
		OBJ *m_pPtr;
	};
