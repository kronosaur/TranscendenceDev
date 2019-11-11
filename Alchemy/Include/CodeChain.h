//	CodeChain.h
//
//	CodeChain is a micro-LISP variant designed for high performance
//	and efficiency.
//
//	Requires Kernel.h
//
//	Basic Coding Rules
//
//	1.	Routines that return an ICCItem * will always increase the refcount
//		if you do not keep the item returned, you must call Discard on the
//		item.
//
//	2.	When passing in ICCItem *, the called routine is responsible for
//		incrementing the refcount if it decides to keep a reference

#pragma once

class CCodeChain;
class CEvalContext;
class ICCItem;

//	Simple definitions

typedef ICCItem *(*PRIMITIVEPROC)(CEvalContext *pCtx, ICCItem *pArguments, DWORD dwData);

class IPrimitiveImpl
	{
	public:
		virtual ICCItem *InvokeCCPrimitive (CEvalContext *pCtx, ICCItem *pArgs, DWORD dwData) { return NULL; }
		virtual bool RegisterCCPrimitives (CCodeChain &CC) { return true; }
	};

#define PPFLAG_SIDEEFFECTS						0x00000001	//	Function has side-effects
#define PPFLAG_NOERRORS							0x00000002	//	Function never returns errors
#define PPFLAG_SYNONYM							0x00000004	//	Function is a synonym (pszDescription is name of function)
#define PPFLAG_CUSTOM_ARG_EVAL					0x00000008	//	Raw args are passed to the function (before evaluation)
#define PPFLAG_METHOD_INVOKE					0x00000010	//	pfFunction points to IPrimitiveImpl

typedef struct
	{
	char *pszName;
	PRIMITIVEPROC pfFunction;
	DWORD dwData;
	char *pszDescription;
	char *pszArguments;
	DWORD dwFlags;
	} PRIMITIVEPROCDEF, *PPRIMITIVEPROCDEF;

struct SPrimitiveDefTable
	{
	PRIMITIVEPROCDEF *pTable;
	int iCount;
	};

//	Error Definitions. These are common result codes returned by basic
//	primitives.

#define CCRESULT_NOTFOUND							1		//	Used for symbol tables, files, etc.
#define CCRESULT_CANCEL								2		//	User canceled operation
#define CCRESULT_DISKERROR							3		//	Out of disk space, etc.

//	A Cons is the primitive list glue.

class CCons
	{
	public:
		ICCItem *m_pItem;
		CCons *m_pNext;
	};

//	Print flags

const DWORD PRFLAG_NO_QUOTES =						0x00000001;
const DWORD	PRFLAG_ENCODE_FOR_DISPLAY =				0x00000002;
const DWORD PRFLAG_FORCE_QUOTES =					0x00000004;

//	Some helper classes

class IItemTransform
	{
	public:
		virtual ~IItemTransform (void) { }

		virtual ICCItem *Transform (ICCItem *pItem);
	};

//	An item is a generic element of a list. This is the basic unit of
//	CodeChain.

class ICCItem : public CObject
	{
	public:
		enum ValueTypes
			{
			Nil,
			True,
			Integer,
			String,
			List,
			Function,
			Complex,
			Double,
			Vector,
			SymbolTable,
			};

		ICCItem (IObjectClass *pClass);

		//	Increment and decrement ref counts

		virtual ICCItem *Clone (CCodeChain *pCC) = 0;
		virtual ICCItem *CloneContainer (void) const = 0;
		virtual ICCItem *CloneDeep (CCodeChain *pCC) { return Clone(pCC); }
		virtual void Discard (void);
		ICCItem *Reference (void) const { m_dwRefCount++; return const_cast<ICCItem *>(this); }
		virtual void Reset (void) = 0;
		void SetNoRefCount (void) { m_bNoRefCount = true; }

		//	List interface

		void AppendInteger (int iValue);
		void AppendString (const CString &sValue);

		virtual void Append (ICCItem *pValue) { }
		virtual ICCItem *Enum (CEvalContext *pCtx, ICCItem *pCode) = 0;
		virtual int GetCount (void) const = 0;
		virtual ICCItem *GetElement (int iIndex) const = 0;
		virtual ICCItem *GetElement (const CString &sKey) const { return NULL; }
		virtual ICCItem *GetElement (CCodeChain *pCC, int iIndex) const;
        virtual CString GetKey (int iIndex) { return NULL_STR; }
		virtual bool HasReferenceTo (ICCItem *pSrc) { return (pSrc == this); }
		virtual ICCItem *Head (CCodeChain *pCC) = 0;
		bool IsList (void) const { return IsNil() || !IsAtom(); }
		virtual ICCItem *Tail (CCodeChain *pCC) = 0;

		//	Evaluation

		bool IsQuoted (void) const { return m_bQuoted; }
		void SetQuoted (void) { m_bQuoted = true; }
		void ClearQuoted (void) { m_bQuoted = false; }

		//	Errors

		bool IsError (void) { return m_bError; }
		void SetError (void) { m_bError = true; }

		//	Virtuals that must be overridden

		virtual ICCItem *Execute (CEvalContext *pCtx, ICCItem *pArgs);
		virtual bool GetBinding (int *retiFrame, int *retiOffset) { return false; }
		virtual ICCItem *GetFunctionBinding (void) { return NULL; }
		virtual CString GetHelp (void) { return NULL_STR; }
		virtual int GetIntegerValue (void) const { return 0; }
		virtual double GetDoubleValue (void) const { return 0.; }
		virtual CString GetStringValue (void) const { return LITERAL(""); }
		virtual ValueTypes GetValueType (void) const = 0;
		virtual CString GetTypeOf (void);
		virtual bool IsAtom (void) const = 0;
		virtual bool IsAtomTable (void) const { return false; }
		virtual bool IsConstant (void) const { return false; }
		virtual bool IsExpression (void) const { return false; }
		virtual bool IsFunction (void) const = 0;
		virtual bool IsIdentifier (void) const = 0;
		virtual bool IsInteger (void) const = 0;
		virtual bool IsDouble (void) const = 0;
		virtual bool IsLambdaFunction (void) const { return false; }
		virtual bool IsNil (void) const = 0;
		virtual bool IsPrimitive (void) const { return false; }
		virtual bool IsSymbolTable (void) const { return false; }
		virtual bool IsTrue (void) const { return false; }
		virtual CString Print (DWORD dwFlags = 0) = 0;
		virtual void SetBinding (int iFrame, int iOffset) { }
		virtual void SetFunctionBinding (CCodeChain *pCC, ICCItem *pBinding) { }

		//	Miscellaneous utility functions

		bool IsLambdaExpression (void);
		bool IsLambdaSymbol (void);
		bool IsNumber (void) { return (IsInteger() || IsDouble()); }
		void ResetItem (void);

		//	Symbol/Atom table functions

		void AppendAt (const CString &sKey, ICCItem *pValue);
		void DeleteAt (const CString &sKey);
		bool GetBooleanAt (const CString &sKey) const;
		double GetDoubleAt (const CString &sKey, double rDefault = 0.0) const;
		int GetIntegerAt (const CString &sKey, int iDefault = 0) const;
		CString GetStringAt (const CString &sKey, const CString &sDefault = NULL_STR) const;
		void SetAt (const CString &sKey, ICCItem *pValue);
		void SetBooleanAt (const CString &sKey, bool bValue);
		void SetIntegerAt (const CString &sKey, int iValue);
		void SetStringAt (const CString &sKey, const CString &sValue);

		virtual void AddEntry (ICCItem *pKey, ICCItem *pEntry, bool bForceLocalAdd = false) { }
		virtual void AddByOffset (CCodeChain *pCC, int iOffset, ICCItem *pEntry) { ASSERT(false); }
		virtual void DeleteAll (CCodeChain *pCC, bool bLambdaOnly) { }
		virtual void DeleteEntry (ICCItem *pKey) { }
		virtual int FindOffset (CCodeChain *pCC, ICCItem *pKey) { return -1; }
		virtual int FindValue (ICCItem *pValue) { return -1; }
		virtual IItemTransform *GetDefineHook (void) { return NULL; }
		virtual ICCItem *GetParent (void) { return NULL; }
		virtual bool IsLocalFrame (void) { return false; }
		virtual ICCItem *ListSymbols (CCodeChain *pCC) { return NotASymbolTable(); }
		virtual ICCItem *Lookup (CCodeChain *pCC, ICCItem *pKey) { return NotASymbolTable(); }
		virtual ICCItem *LookupByOffset (CCodeChain *pCC, int iOffset) { return NotASymbolTable(); }
		virtual ICCItem *LookupEx (CCodeChain *pCC, ICCItem *pKey, bool *retbFound) { return NotASymbolTable(); }
		virtual void SetDefineHook (IItemTransform *pHook) { }
		virtual void SetParent (ICCItem *pParent) { ASSERT(false); }
		virtual void SetLocalFrame (void) { ASSERT(false); }
		virtual ICCItem *SimpleLookup (CCodeChain *pCC, ICCItem *pKey, bool *retbFound, int *retiOffset) { return NotASymbolTable(); }

		//	Pool access

		ICCItem *GetNextFree() { return (ICCItem *)m_dwRefCount; }
		void SetNextFree (ICCItem *pNext) { m_dwRefCount = (DWORD)pNext; }

		static int Compare (ICCItem *pFirst, ICCItem *pSecond);

	protected:
		void CloneItem (const ICCItem *pItem);

		virtual void DestroyItem (void) { }

		ICCItem *NotASymbolTable (void);

		mutable DWORD m_dwRefCount;				//	Number of references to this item

		DWORD m_bQuoted:1;						//	TRUE if quoted
		DWORD m_bError:1;						//	TRUE if it represents a runtime error
		DWORD m_bNoRefCount:1;					//	TRUE if we don't care about ref count
	};

//	ICCItem SmartPtr

class ICCItemPtr
	{
	public:
		constexpr ICCItemPtr (void) : m_pPtr(NULL) { }
		constexpr ICCItemPtr (std::nullptr_t) : m_pPtr(NULL) { }

		explicit ICCItemPtr (ICCItem *pPtr) : m_pPtr(pPtr) { }

		explicit ICCItemPtr (ICCItem::ValueTypes iType);
		explicit ICCItemPtr (const CString &sValue);
		explicit ICCItemPtr (int iValue);
		explicit ICCItemPtr (DWORD dwValue);
		explicit ICCItemPtr (double rValue);
		explicit ICCItemPtr (bool bValue);

		ICCItemPtr (const ICCItemPtr &Src);

		ICCItemPtr (ICCItemPtr &&Src) : m_pPtr(Src.m_pPtr)
			{
			Src.m_pPtr = NULL;
			}

		~ICCItemPtr (void);

		ICCItemPtr &operator= (const ICCItemPtr &Src);
		ICCItemPtr &operator= (ICCItem *pSrc);
		operator ICCItem *() const { return m_pPtr; }
		ICCItem * operator->() const { return m_pPtr; }

		explicit operator bool() const { return (m_pPtr != NULL); }

		void Delete (void);

		bool Load (const CString &sCode, CString *retsError);

		void TakeHandoff (ICCItem *pPtr);
		void TakeHandoff (ICCItemPtr &Src);

		void Set (const ICCItemPtr &Src)
			{
			*this = Src;
			}

	private:
		ICCItem *m_pPtr;
	};

//	An atom is a single value

class ICCAtom : public ICCItem
	{
	public:
		ICCAtom (IObjectClass *pClass) : ICCItem(pClass) { }

		//	ICCItem virtuals

		virtual ICCItem *CloneContainer (void) const override { return Reference(); }
		virtual ICCItem *Enum (CEvalContext *pCtx, ICCItem *pCode) override;
		virtual ICCItem *GetElement (int iIndex) const override { return (iIndex == 0 ? Reference() : NULL); }
		virtual int GetCount (void) const override { return 1; }
		virtual ICCItem *Head (CCodeChain *pCC) override { return Reference(); }
		virtual bool IsAtom (void) const override { return true; }
		virtual bool IsInteger (void) const override { return false; }
		virtual bool IsDouble(void) const override { return false; }
		virtual bool IsNil (void) const override { return false; }
		virtual ICCItem *Tail (CCodeChain *pCC) override;
	};

//  A numeral is an atom that represents a double or an integer

class CCNumeral : public ICCAtom
	{
	public:
		CCNumeral(void);

		// ICCItem virtuals
		virtual bool IsConstant (void) const override { return true; }
		virtual bool IsIdentifier(void) const override { return false; }
		virtual bool IsFunction(void) const override { return false; }
		virtual bool IsInteger(void) const override { return false;  }
		virtual bool IsDouble(void) const override { return false;  }
	};

class CCInteger : public CCNumeral
	{
	public:
		CCInteger (void);

		int GetValue (void) { return m_iValue; }
		void SetValue (int iValue) { m_iValue = iValue; }

		//	ICCItem virtuals
		virtual ICCItem *Clone(CCodeChain *pCC) override;
		virtual bool IsInteger(void) const override { return true; }
		virtual bool IsDouble(void) const override { return false; }
		virtual int GetIntegerValue (void) const override { return m_iValue; }
		virtual double GetDoubleValue(void) const override { return double(m_iValue); }
		virtual CString GetStringValue (void) const override { return strFromInt(m_iValue); }
		virtual ValueTypes GetValueType(void) const override { return Integer;  }
		virtual CString Print (DWORD dwFlags = 0) override;
		virtual void Reset(void) override;

	protected:
		virtual void DestroyItem (void) override;

	private:
		int m_iValue;							//	Value of 32-bit integer
	};

//	This is a standard implementation of a double

class CCDouble : public CCNumeral
	{
	public:
		CCDouble(void);

		double GetValue(void) { return m_dValue; }
		void SetValue(double dValue) { m_dValue = dValue; }

		//	ICCItem virtuals
		virtual ICCItem *Clone(CCodeChain *pCC) override;
		virtual bool IsInteger(void) const override { return false; }
		virtual bool IsDouble(void) const override { return true; }
		virtual int GetIntegerValue(void) const override { return int(m_dValue); }
		virtual double GetDoubleValue(void) const override { return m_dValue; }
		virtual CString GetStringValue(void) const override { return strFromDouble(m_dValue); }
		virtual ValueTypes GetValueType(void) const override { return Double;  }
		virtual CString Print (DWORD dwFlags = 0) override;
		virtual void Reset (void) override;

	protected:
		virtual void DestroyItem (void) override;

	private:
		double m_dValue;							//	Value of double
	};

//	Nil class

class CCNil : public ICCAtom
	{
	public:
		CCNil (void);

		//	ICCItem virtuals

		virtual ICCItem *Clone (CCodeChain *pCC) override;
		virtual void Discard (void) override { }
		virtual int GetCount (void) const override { return 0; }
		virtual int GetIntegerValue (void) const override { return 0; }
		virtual CString GetStringValue (void) const override { return LITERAL("Nil"); }
		virtual ValueTypes GetValueType (void) const override { return Nil; }
		virtual bool IsConstant (void) const override { return true; }
		virtual bool IsIdentifier (void) const override { return false; }
		virtual bool IsInteger (void) const override { return true; }
		virtual bool IsDouble(void) const override { return false; }
		virtual bool IsFunction (void) const override { return false; }
		virtual bool IsNil (void) const override { return true; }
		virtual CString Print (DWORD dwFlags = 0) override { return LITERAL("Nil"); }
		virtual void Reset (void) override { }

	protected:
		virtual void DestroyItem (void) override;
	};

//	True class

class CCTrue : public ICCAtom
	{
	public:
		CCTrue (void);

		//	ICCItem virtuals

		virtual ICCItem *Clone (CCodeChain *pCC) override;
		virtual void Discard (void) override { }
		virtual int GetIntegerValue (void) const override { return 1; }
		virtual CString GetStringValue (void) const override { return LITERAL("True"); }
		virtual ValueTypes GetValueType (void) const override { return True; }
		virtual bool IsConstant (void) const override { return true; }
		virtual bool IsIdentifier (void) const override { return false; }
		virtual bool IsFunction (void) const override { return false; }
		virtual bool IsTrue (void) const override { return true; }
		virtual CString Print (DWORD dwFlags = 0) override { return LITERAL("True"); }
		virtual void Reset (void) override { }

	protected:
		virtual void DestroyItem (void) override;
	};

//	A string is an atom that represents a sequence of characters

class ICCString : public ICCAtom
	{
	public:
		ICCString (IObjectClass *pClass) : ICCAtom(pClass) { }

		//	ICCItem virtuals

		virtual ValueTypes GetValueType (void) const override { return String; }
		virtual bool IsConstant (void) const override { return IsQuoted(); }
		virtual bool IsIdentifier (void) const override { return true; }
		virtual bool IsFunction (void) const override { return false; }
	};

//	This is the standard implementation of a string

class CCString : public ICCString
	{
	public:
		CCString (void);

		CString GetValue (void) { return m_sValue; }
		static CString Print (const CString &sString, DWORD dwFlags = 0);
		void SetValue (CString sValue) { m_sValue = sValue; }

		//	ICCItem virtuals

		virtual ICCItem *Clone (CCodeChain *pCC) override;
		virtual bool GetBinding (int *retiFrame, int *retiOffset) override;
		virtual double GetDoubleValue (void) const override { return strToDouble(m_sValue, 0.0); }
		virtual int GetIntegerValue (void) const override { return strToInt(m_sValue, 0); }
		virtual ICCItem *GetFunctionBinding (void) override { if (m_pBinding) return m_pBinding->Reference(); else return NULL; }
		virtual CString GetStringValue (void) const override { return m_sValue; }
		virtual CString Print (DWORD dwFlags = 0) override;
		virtual void SetBinding (int iFrame, int iOffset) override;
		virtual void SetFunctionBinding (CCodeChain *pCC, ICCItem *pBinding) override;
		virtual void Reset (void) override;

	protected:
		virtual void DestroyItem (void) override;

	private:
		CString m_sValue;						//	Value of string
		int m_dwBinding;						//	Index into binding
		ICCItem *m_pBinding;					//	Function binding
	};

//	This is a primitive function definition

class CCPrimitive : public ICCAtom
	{
	public:
		CCPrimitive (void);

		void SetProc (PRIMITIVEPROCDEF *pDef, IPrimitiveImpl *pImpl);

		//	ICCItem virtuals

		virtual ICCItem *Clone (CCodeChain *pCC) override;
		virtual ICCItem *Execute (CEvalContext *pCtx, ICCItem *pArgs) override;
		virtual CString GetHelp (void) override { return m_sDesc; }
		virtual CString GetStringValue (void) const override { return m_sName; }
		virtual ValueTypes GetValueType (void) const override { return Function; }
		virtual bool IsIdentifier (void) const override { return false; }
		virtual bool IsFunction (void) const override { return true; }
		virtual bool IsPrimitive (void) const override { return true; }
		virtual CString Print (DWORD dwFlags = 0) override;
		virtual void Reset (void) override;

	protected:
		virtual void DestroyItem (void) override;

	private:
		CString m_sName;
		void *m_pfFunction;
		CString m_sArgPattern;
		CString m_sDesc;
		DWORD m_dwData;
		DWORD m_dwFlags;
	};

//	This is a lambda structure

class CCLambda : public ICCAtom
	{
	public:
		CCLambda (void);

		ICCItem *CreateFromList (ICCItem *pList, bool bArgsOnly);
		void SetLocalSymbols (CCodeChain *pCC, ICCItem *pSymbols);

		//	ICCItem virtuals

		virtual ICCItem *Clone (CCodeChain *pCC) override;
		virtual ICCItem *Execute (CEvalContext *pCtx, ICCItem *pArgs) override;
		virtual CString GetStringValue (void) const override { return LITERAL("[lambda expression]"); }
		virtual ValueTypes GetValueType (void) const override { return Function; }
		virtual bool IsIdentifier (void) const override { return false; }
		virtual bool IsFunction (void) const override { return true; }
		virtual bool IsLambdaFunction (void) const override { return true; }
		virtual CString Print (DWORD dwFlags = 0) override;
		virtual void Reset (void) override;

	protected:
		virtual void DestroyItem (void) override;

	private:
		ICCItem *m_pArgList;
		ICCItem *m_pCode;
		ICCItem *m_pLocalSymbols;
	};

//	A list is a list of items

class ICCList : public ICCItem
	{
	public:
		ICCList (IObjectClass *pClass) : ICCItem(pClass) { }

		//	ICCItem virtuals

		virtual ValueTypes GetValueType (void) const override { return List; }
		virtual bool IsAtom (void) const override { return false; }
		virtual bool IsFunction (void) const override { return false; }
		virtual bool IsIdentifier (void) const override { return false; }
		virtual bool IsInteger (void) const override { return false; }
		virtual bool IsDouble(void) const override { return false; }
		virtual bool IsNil (void) const override { return (GetCount() == 0); }
	};

//	This is a linked-list implementation of a list

class CCLinkedList : public ICCList
	{
	public:
		CCLinkedList (void);
		virtual ~CCLinkedList (void);

		void CreateIndex (void) const;
		void RemoveElement (CCodeChain *pCC, int iIndex);
		void ReplaceElement (CCodeChain *pCC, int iIndex, ICCItem *pNewItem);
		void Shuffle (CCodeChain *pCC);
		void Sort (CCodeChain *pCC, int iOrder, ICCItem *pSortIndex = NULL);
		ICCItem *GetFlattened(CCodeChain *pCC, ICCItem *pResult);
		ICCItem *IsValidVectorContent (CCodeChain *pCC);
		
		//	ICCItem virtuals

		virtual void Append (ICCItem *pValue) override;
		virtual ICCItem *Clone (CCodeChain *pCC) override;
		virtual ICCItem *CloneContainer (void) const override;
		virtual ICCItem *CloneDeep (CCodeChain *pCC) override;
		virtual ICCItem *Enum (CEvalContext *pCtx, ICCItem *pCode) override;
		virtual int GetCount (void) const override { return m_iCount; }
		virtual ICCItem *GetElement (int iIndex) const override;
		virtual bool HasReferenceTo (ICCItem *pSrc) override;
		virtual ICCItem *Head (CCodeChain *pCC) override { return GetElement(0); }
		virtual bool IsConstant (void) const override { return (IsQuoted() || GetCount() == 0); }
		virtual bool IsExpression (void) const override { return (GetCount() > 0); }
		virtual CString Print (DWORD dwFlags = 0) override;
		virtual ICCItem *Tail (CCodeChain *pCC) override;
		virtual void Reset (void) override;

	protected:
		virtual void DestroyItem (void) override;

	private:
		void QuickSort (int iLeft, int iRight, int iOrder);
		void QuickSortLists (int iKeyIndex, int iLeft, int iRight, int iOrder);
		void QuickSortStructs (const CString &sSortKey, int iLeft, int iRight, int iOrder);
		void UpdateLinksFromIndex (void);

		CCons *m_pFirst;						//	First element in the list
		CCons *m_pLast;							//	Last element in the list
		int m_iCount;							//	Number of elements

		mutable CCons **m_pIndex;				//	GetElement array
	};

//	This is an array of 32-bit integers
class CCVectorOld : public ICCList
	{
	public:
		CCVectorOld (void);
		CCVectorOld (CCodeChain *pCC);
		virtual ~CCVectorOld (void);

		int *GetArray (void);
		bool SetElement (int iIndex, int iElement);
		ICCItem *SetSize (CCodeChain *pCC, int iNewSize);

		//	ICCItem virtuals

		virtual ICCItem *Clone (CCodeChain *pCC) override;
		virtual ICCItem *CloneContainer (void) const override { return Reference(); }	//	LATER
		virtual ICCItem *Enum (CEvalContext *pCtx, ICCItem *pCode) override;
		virtual int GetCount (void) const override { return m_iCount; }
		virtual ICCItem *GetElement (int iIndex) const override;
		virtual ICCItem *Head (CCodeChain *pCC) override { return GetElement(0); }
		virtual CString Print (DWORD dwFlags = 0) override;
		virtual ICCItem *Tail (CCodeChain *pCC) override;
		virtual void Reset (void) override;

	protected:
		virtual void DestroyItem (void) override;

	private:
		CCodeChain *m_pCC;						//	CodeChain
		int m_iCount;							//	Number of elements
		int *m_pData;							//	Array of elements
	};

//	**UNDER CONSTRUCTION**
//	A vector of Numeral items

class ICCVector : public ICCItem
{
public:
	ICCVector(IObjectClass *pClass) : ICCItem(pClass) { }

	//	ICCItem virtuals

	virtual ValueTypes GetValueType (void) const override { return Vector; }
	virtual bool IsAtom (void) const override { return false; }
	virtual bool IsFunction (void) const override { return false; }
	virtual bool IsIdentifier (void) const override { return false; }
	virtual bool IsInteger (void) const override { return false; }
	virtual bool IsDouble (void) const override { return false; }
	virtual bool IsNil (void) const override { return (GetCount() == 0); }
};

class CCVector : public ICCVector
	{
	public:
		CCVector (void);
		CCVector (CCodeChain *pCC);
		virtual ~CCVector (void);

		TArray<double> GetDataArray (void) { return m_vData; }
		TArray<int> GetShapeArray (void) { return m_vShape; }
		ICCItem *SetElementsByIndices (CCodeChain *pCC, CCLinkedList *pIndices, CCLinkedList *pData);
		ICCItem *SetDataArraySize (CCodeChain *pCC, int iNewSize);
		ICCItem *SetShapeArraySize (CCodeChain *pCC, int iNewSize);
		void SetContext (CCodeChain *pCC) { m_pCC = pCC;  }
		void SetShape (CCodeChain *pCC, TArray<int> vNewShape) { m_vShape = vNewShape; }
		void SetArrayData (CCodeChain *pCC, TArray<double> vNewData) { m_vData = vNewData; }
		CString CCVector::PrintWithoutShape (CCodeChain *pCC, DWORD dwFlags);
		
		void Append (CCodeChain *pCC, ICCItem *pItem, ICCItem **retpError = NULL);
		void Sort (CCodeChain *pCC, int iOrder, int iIndex = -1);

		//	ICCItem virtuals

		virtual ICCItem *Clone (CCodeChain *pCC) override;
		virtual ICCItem *CloneContainer (void) const override { return Reference(); }
		virtual ICCItem *Enum (CEvalContext *pCtx, ICCItem *pCode) override;
		virtual int GetCount (void) const override { return m_vData.GetCount(); }
		virtual ICCItem *GetElement (int iIndex) const override;
		virtual ICCItem *Head (CCodeChain *pCC) override { return GetElement(0); }
		virtual CString Print (DWORD dwFlags = 0) override;
		virtual ICCItem *Tail (CCodeChain *pCC) override;
		virtual void Reset (void) override;

		virtual int GetShapeCount (void) { return m_vShape.GetCount(); }
		virtual int GetDimension (void) { return m_vShape.GetCount(); }
		virtual ICCItem *SetElement (int iIndex, double dValue);
		virtual ICCItem *IndexVector (CCodeChain *pCC, ICCItem *pIndices);

	protected:
		virtual void DestroyItem (void) override;

	private:
		CCodeChain *m_pCC;						//	CodeChain
		TArray<double> m_vData;				//	Array of elements
		TArray<int> m_vShape;					//  Shape
	};

//	This is an atom table object

class CCAtomTable : public ICCAtom
	{
	public:
		CCAtomTable (void);

		//	ICCItem virtuals

		virtual ICCItem *Clone (CCodeChain *pCC) override;
		virtual ValueTypes GetValueType (void) const override { return Complex; }
		virtual bool IsIdentifier (void) const override { return false; }
		virtual bool IsFunction (void) const override { return false; }
		virtual bool IsAtomTable (void) const override { return true; }
		virtual CString Print (DWORD dwFlags = 0) override;
		virtual void Reset (void) override;

		virtual void AddEntry (ICCItem *pAtom, ICCItem *pEntry, bool bForceLocalAdd = false) override;
		virtual ICCItem *ListSymbols (CCodeChain *pCC) override;
		virtual ICCItem *Lookup (CCodeChain *pCC, ICCItem *pAtom) override;
		virtual ICCItem *LookupEx (CCodeChain *pCC, ICCItem *pAtom, bool *retbFound) override;

	protected:
		virtual void DestroyItem (void);

	private:
		CDictionary m_Table;
	};

//	This is a symbol table object

class CCSymbolTable : public ICCList
	{
	public:
		CCSymbolTable (void);

		//	ICCItem virtuals

		virtual ICCItem *Clone (CCodeChain *pCC) override;
		virtual ICCItem *CloneContainer (void) const override;
		virtual ICCItem *CloneDeep (CCodeChain *pCC) override;
		virtual ValueTypes GetValueType (void) const override { return SymbolTable; }
		virtual bool IsConstant (void) const override;
		virtual bool IsIdentifier (void) const override { return false; }
		virtual bool IsFunction (void) const override { return false; }
		virtual bool IsLocalFrame (void) override { return m_bLocalFrame; }
		virtual bool IsSymbolTable (void) const override { return true; }
		virtual CString Print (DWORD dwFlags = 0) override;
		virtual void Reset (void) override;

		//	List interface

		virtual ICCItem *Enum (CEvalContext *pCtx, ICCItem *pCode) override { ASSERT(false); return NULL; }
		virtual int GetCount (void) const override { return m_Symbols.GetCount(); }
		virtual ICCItem *GetElement (int iIndex) const override;
		virtual ICCItem *GetElement (const CString &sKey) const override;
		virtual ICCItem *GetElement (CCodeChain *pCC, int iIndex) const override;
        virtual CString GetKey (int iIndex) override { return m_Symbols.GetKey(iIndex); }
		virtual bool HasReferenceTo (ICCItem *pSrc) override;
		virtual ICCItem *Head (CCodeChain *pCC) override { return GetElement(0); }
		virtual ICCItem *Tail (CCodeChain *pCC) override { return GetElement(1); }

		//	Symbols

		virtual void AddByOffset (CCodeChain *pCC, int iOffset, ICCItem *pEntry) override;
		virtual void AddEntry (ICCItem *pKey, ICCItem *pEntry, bool bForceLocalAdd = false) override;
		virtual void DeleteAll (CCodeChain *pCC, bool bLambdaOnly) override;
		virtual void DeleteEntry (ICCItem *pKey) override;
		virtual int FindOffset (CCodeChain *pCC, ICCItem *pKey) override;
		virtual int FindValue (ICCItem *pValue) override;
		virtual IItemTransform *GetDefineHook (void) override { return m_pDefineHook; }
		virtual ICCItem *GetParent (void) override { return m_pParent; }
		virtual ICCItem *ListSymbols (CCodeChain *pCC) override;
		virtual ICCItem *Lookup (CCodeChain *pCC, ICCItem *pKey) override;
		virtual ICCItem *LookupByOffset (CCodeChain *pCC, int iOffset) override;
		virtual ICCItem *LookupEx (CCodeChain *pCC, ICCItem *pKey, bool *retbFound) override;
		virtual void SetDefineHook (IItemTransform *pHook) override { m_pDefineHook = pHook; }
		virtual void SetLocalFrame (void) override { m_bLocalFrame = true; }
		virtual void SetParent (ICCItem *pParent) override { m_pParent = pParent->Reference(); }
		virtual ICCItem *SimpleLookup (CCodeChain *pCC, ICCItem *pKey, bool *retbFound, int *retiOffset) override;

	protected:
		virtual void DestroyItem (void) override;

	private:
		CSymbolTable m_Symbols;
		ICCItem *m_pParent;
		bool m_bLocalFrame;

		IItemTransform *m_pDefineHook;
	};

//	Item pools

template <class ItemClass>
class CCItemPool
	{
	public:
		CCItemPool (void);
		~CCItemPool (void);

		void CleanUp (void);
		ICCItem *CreateItem (void);
		void DestroyItem (ICCItem *pItem);
		int GetCount (void) const { CSmartLock Lock(m_cs); return m_iCount; }

	private:
		mutable CCriticalSection m_cs;
		ICCItem *m_pFreeList = NULL;
		ItemClass **m_pBackbone = NULL;
		int m_iBackboneSize = 0;
		int m_iCount = 0;
	};

class CConsPool
	{
	public:
		CConsPool (void);
		~CConsPool (void);

		void CleanUp (void);
		CCons *CreateCons (void);
		void DestroyCons (CCons *pCons);
		int GetCount (void) const { CSmartLock Lock(m_cs); return m_iCount; }

	private:
		mutable CCriticalSection m_cs;
		CCons *m_pFreeList;
		CCons **m_pBackbone;
		int m_iBackboneSize;
		int m_iCount;
	};

//	Misc structures

class CEvalContext
	{
	public:
		CCodeChain *pCC;
		ICCItem *pLexicalSymbols;
		ICCItem *pLocalSymbols;

		LPVOID pExternalCtx;
	};

//	This is the main CodeChain context

class CCodeChain
	{
	public:
		struct SLinkOptions
			{
			int iOffset = 0;				//	Offset into string
			bool bNullIfEmpty = false;		//	If we have a blank string, return NULL

			int iLinked = 0;				//	Returns number of characters we parsed.
			int iCurLine = 1;				//	Updated to return current line.
			};

		CCodeChain (void);
		virtual ~CCodeChain (void);

		ALERROR Boot (void);
		void CleanUp (void);

		//	Create/Destroy routines

		static ICCItem *CreateAtomTable (void);
		static ICCItem *CreateBool (bool bValue);
		static CCons *CreateCons (void) { return m_ConsPool.CreateCons(); }
		static ICCItem *CreateError (const CString &sError, ICCItem *pData = NULL);
		static ICCItem *CreateErrorCode (int iErrorCode);
		static ICCItem *CreateInteger (int iValue);
		static ICCItem *CreateDouble (double dValue);
		static ICCItem *CreateLambda (ICCItem *pList, bool bArgsOnly);
		static ICCItem *CreateLinkedList (void);
		static ICCItem *CreateLiteral (const CString &sString);
		static ICCItem *CreateNil (void) { return &m_Nil; }
		static ICCItem *CreateNumber (double dValue);
		static ICCItem *CreatePrimitive (PRIMITIVEPROCDEF *pDef, IPrimitiveImpl *pImpl);
		static ICCItem *CreateString (const CString &sString);
		static ICCItem *CreateSymbolTable (void);
		static ICCItem *CreateSystemError (ALERROR error);
		static ICCItem *CreateTrue (void) { return &m_True; }
		static ICCItem *CreateVariant (const CString &sValue);
		ICCItem *CreateVectorOld (int iSize);
		ICCItem *CreateFilledVector (double dScalar, TArray<int> vShape);
		ICCItem *CreateVectorGivenContent (TArray<int> vShape, CCLinkedList *pContentList);
		ICCItem *CreateVectorGivenContent (TArray<int> vShape, TArray<double> vContentList);
		static void DestroyAtomTable (ICCItem *pItem) { m_AtomTablePool.DestroyItem(pItem); }
		static void DestroyCons (CCons *pCons) { m_ConsPool.DestroyCons(pCons); }
		static void DestroyInteger (ICCItem *pItem) { m_IntegerPool.DestroyItem(pItem); }
		static void DestroyDouble (ICCItem *pItem) { m_DoublePool.DestroyItem(pItem); }
		static void DestroyLambda (ICCItem *pItem) { m_LambdaPool.DestroyItem(pItem); }
		static void DestroyLinkedList (ICCItem *pItem) { m_ListPool.DestroyItem(pItem); }
		static void DestroyPrimitive (ICCItem *pItem) { m_PrimitivePool.DestroyItem(pItem); }
		static void DestroyString (ICCItem *pItem) { m_StringPool.DestroyItem(pItem); }
		static void DestroySymbolTable (ICCItem *pItem) { m_SymbolTablePool.DestroyItem(pItem); }
		static void DestroyVectorOld (ICCItem *pItem) { delete pItem; }
		static void DestroyVector(ICCItem *pItem) { m_VectorPool.DestroyItem(pItem); }
		static ICCItemPtr IncValue (ICCItem *pValue, ICCItem *pInc = NULL);

		//	Evaluation and parsing routines

		ICCItem *Apply (ICCItem *pFunc, ICCItem *pArgs, LPVOID pExternalCtx);
		ICCItem *GetNil (void) { return &m_Nil; }
		ICCItem *GetTrue (void) { return &m_True; }
		ICCItem *Eval (CEvalContext *pEvalCtx, ICCItem *pItem);
		static ICCItem *Link (const CString &sString, SLinkOptions &Options = SLinkOptions());
		ICCItem *LookupGlobal (const CString &sGlobal, LPVOID pExternalCtx);
		ICCItem *TopLevel (ICCItem *pItem, LPVOID pExternalCtx);
		static CString Unlink (ICCItem *pItem);

		//	Extensions

		ALERROR DefineGlobal (const CString &sVar, ICCItem *pValue);
		ALERROR DefineGlobalInteger (const CString &sVar, int iValue);
		ALERROR DefineGlobalString (const CString &sVar, const CString &sValue);
		void DiscardAllGlobals (void);
		ICCItem *EvaluateArgs (CEvalContext *pCtx, ICCItem *pArgs, const CString &sArgValidation);
		IItemTransform *GetGlobalDefineHook (void) const { return m_pGlobalSymbols->GetDefineHook(); }
		ICCItem *GetGlobals (void) { return m_pGlobalSymbols; }
		ICCItem *ListGlobals (void);
		ICCItem *LookupFunction (CEvalContext *pCtx, ICCItem *pName);
		ICCItem *PoolUsage (void);
		ALERROR RegisterPrimitive (PRIMITIVEPROCDEF *pDef, IPrimitiveImpl *pImpl = NULL);
		ALERROR RegisterPrimitives (const SPrimitiveDefTable &Table);
		void SetGlobalDefineHook (IItemTransform *pHook) { m_pGlobalSymbols->SetDefineHook(pHook); }
		void SetGlobals (ICCItem *pGlobals) { m_pGlobalSymbols->Discard(); m_pGlobalSymbols = pGlobals->Reference(); }

		//	Miscellaneous

		bool HasIdentifier (ICCItem *pCode, const CString &sIdentifier);

		static ICCItemPtr SystemSelfTest (void);

	private:
		static ICCItem *CreateDoubleIfPossible (const CString &sString);
		static ICCItem *CreateIntegerIfPossible (const CString &sString);
		static ICCItem *CreateParseError (int iLine, const CString &sError);
		ICCItem *EvalLiteralStruct (CEvalContext *pCtx, ICCItem *pItem);
		static ICCItem *LinkFragment (const CString &sString, int iOffset = 0, int *retiLinked = NULL, int *ioiCurLine = NULL);
		ICCItem *Lookup (CEvalContext *pCtx, ICCItem *pItem);
		static char *SkipWhiteSpace (char *pPos, int *ioiLine);

		ICCItem *m_pGlobalSymbols;

		static CCNil m_Nil;
		static CCTrue m_True;
		static CCItemPool<CCInteger> m_IntegerPool;
		static CCItemPool<CCDouble> m_DoublePool;
		static CCItemPool<CCString> m_StringPool;
		static CCItemPool<CCLinkedList> m_ListPool;
		static CCItemPool<CCPrimitive> m_PrimitivePool;
		static CCItemPool<CCAtomTable> m_AtomTablePool;
		static CCItemPool<CCSymbolTable> m_SymbolTablePool;
		static CCItemPool<CCLambda> m_LambdaPool;
		static CCItemPool<CCVector> m_VectorPool;
		static CConsPool m_ConsPool;
	};

//	Libraries

ALERROR pageLibraryInit (CCodeChain &CC);

//	Utilities

#define HELPER_COMPARE_COERCE_COMPATIBLE		0x00000001
#define HELPER_COMPARE_COERCE_FULL				0x00000002

int HelperCompareItems (ICCItem *pFirst, ICCItem *pSecond, DWORD dwCoerceFlags = HELPER_COMPARE_COERCE_COMPATIBLE);
int HelperCompareItemsLists (ICCItem *pFirst, ICCItem *pSecond, int iKeyIndex, bool bCoerce = true);
int HelperCompareItemsStructs (ICCItem *pFirst, ICCItem *pSecond, const CString &sSortKey, bool bCoerce = true);

