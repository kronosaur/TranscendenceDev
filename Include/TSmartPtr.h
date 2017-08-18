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

		void Delete (void) { Set(NULL);	}

		void Set (OBJ *pPtr)
			{
			if (m_pPtr)
				m_pPtr->Delete();

			m_pPtr = pPtr;
			}

		void Set (const TSharedPtr<OBJ> &Src)
			{
			*this = Src;
			}

	private:
		OBJ *m_pPtr;
	};

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

	private:
		OBJ *m_pPtr;
	};

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
