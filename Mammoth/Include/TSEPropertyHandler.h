//	TSEPropertyHandler.h
//
//	Transcendence Utilities
//	Copyright 2019 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

#include <array>
#include <functional>

template <class OBJ>
class TPropertyHandler
	{
	public:
		struct SPropertyDef
			{
			LPSTR pProperty;
			LPSTR pShortDesc;
			std::function<ICCItemPtr(const OBJ &, const CString &)> fnGet;
			std::function<bool(OBJ &, const CString &, ICCItem *, CString *)> fnSet;
			};

		TPropertyHandler (void)
			{
			}

		template <int N> TPropertyHandler (const std::array<SPropertyDef, N> &Table)
			{
			for (int i = 0; i < N; i++)
				m_Table.SetAt(CString(Table[i].pProperty), Table[i]);
			}

		bool FindProperty (const OBJ &Obj, const CString &sProperty, ICCItemPtr &retpValue) const
			{
			const SPropertyDef *pEntry = m_Table.GetAt(sProperty);
			if (pEntry == NULL)
				return false;

			try
				{
				retpValue = pEntry->fnGet(Obj, sProperty);
				}
			catch (...)
				{
				retpValue = ICCItemPtr(strPatternSubst(CONSTLIT("Crash getting property: %s"), sProperty));
				}

			return true;
			}

		ICCItemPtr GetProperty (const OBJ &Obj, const CString &sProperty) const
			{
			ICCItemPtr pValue;
			if (!FindProperty(Obj, sProperty, pValue))
				return ICCItemPtr(ICCItem::Nil);

			return pValue;
			}

		ICCItemPtr GetProperty (const OBJ &Obj, int iIndex) const
			{
			if (iIndex < 0 || iIndex >= m_Table.GetCount())
				return ICCItemPtr(ICCItem::Nil);

			try
				{
				return m_Table[iIndex].fnGet(Obj, CString(m_Table[iIndex].sProperty));
				}
			catch (...)
				{
				return ICCItemPtr(strPatternSubst(CONSTLIT("Crash getting property: %s"), sProperty));
				}
			}

		int GetPropertyCount (void) const { return m_Table.GetCount(); }

		const CString &GetPropertyName (int iIndex) const
			{
			if (iIndex < 0 || iIndex >= m_Table.GetCount())
				return NULL_STR;

			return m_Table.GetKey(iIndex);
			}

		bool SetProperty (OBJ &Obj, const CString &sProperty, ICCItem *pValue, CString *retsError = NULL)
			{
			const SPropertyDef *pEntry = m_Table.GetAt(sProperty);
			if (pEntry == NULL || pEntry->fnSet == NULL)
				{
				if (retsError)
					*retsError = strPatternSubst("Invalid property: %s", sProperty);
				return false;
				}

			try
				{
				return pEntry->fnSet(Obj, sProperty, pValue, retsError);
				}
			catch (...)
				{
				if (retsError)
					*retsError = strPatternSubst("Crash setting property: %s", sProperty);
				return false;
				}
			}

	private:
		TSortMap<CString, SPropertyDef> m_Table;
	};
