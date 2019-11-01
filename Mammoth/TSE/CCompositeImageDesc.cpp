//	CCompositeImageDesc.cpp
//
//	CCompositeImageDesc class
//
//	An entry is one of the following:
//
//	<Image .../>
//
//	<Composite>
//		{ordered list of entries}
//	</Composite>
//
//	<Effect>
//		{some effect tag}
//	</Effect>
//
//	<FilterColorize color="...">
//		{an entry}
//	</FilterColorize>
//
//	<LocationCriteriaTable>
//		<{entry} criteria="..." />
//	</LocationCriteriaTable>
//
//	<Lookup unid="..."/>
//
//	<RotationTable>
//		<{entry} rotation="..." />
//	</RotationTable>
//
//	<Shipwreck class="..."/>
//
//	<Table>
//		{set of entries}
//	</Table>
//
//	The outer-most entry must be one of the following:
//
//	<Image .../>
//
//	<Image>
//		{an entry}
//	</Image>
//
//	<ImageComposite>
//		{ordered list of entries}
//	</ImageComposite>
//
//	<ImageShipwreck class="..." />
//
//	<ImageVariants>
//		{set of entries}
//	</ImageVariants>

#include "PreComp.h"

#define COMPOSITE_TAG							CONSTLIT("Composite")
#define EFFECT_TAG								CONSTLIT("Effect")
#define FILTER_COLORIZE_TAG						CONSTLIT("FilterColorize")
#define IMAGE_TAG								CONSTLIT("Image")
#define IMAGE_COMPOSITE_TAG						CONSTLIT("ImageComposite")
#define IMAGE_EFFECT_TAG						CONSTLIT("ImageEffect")
#define IMAGE_LOOKUP_TAG						CONSTLIT("ImageLookup")
#define IMAGE_SHIPWRECK_TAG						CONSTLIT("ImageShipwreck")
#define IMAGE_VARIANTS_TAG						CONSTLIT("ImageVariants")
#define LOCATION_CRITERIA_TABLE_TAG				CONSTLIT("LocationCriteriaTable")
#define LOOKUP_TAG								CONSTLIT("Lookup")
#define ROTATION_TABLE_TAG						CONSTLIT("RotationTable")
#define SHIPWRECK_TAG							CONSTLIT("Shipwreck")
#define TABLE_TAG								CONSTLIT("Table")
#define VARIANTS_TAG							CONSTLIT("Variants")

#define CHANCE_ATTRIB							CONSTLIT("chance")
#define CLASS_ATTRIB							CONSTLIT("class")
#define COLOR_ATTRIB							CONSTLIT("color")
#define CRITERIA_ATTRIB							CONSTLIT("criteria")
#define EFFECT_ATTRIB							CONSTLIT("effect")
#define HUE_ATTRIB								CONSTLIT("hue")
#define IMAGE_ID_ATTRIB							CONSTLIT("imageID")
#define ROTATION_ATTRIB							CONSTLIT("rotation")
#define SATURATION_ATTRIB						CONSTLIT("saturation")

class CCompositeEntry : public IImageEntry
	{
	public:
		CCompositeEntry (void) { }
		virtual ~CCompositeEntry (void);

		virtual void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override { retTypesUsed->SetAt(m_Image.GetBitmapUNID(), true); }
        virtual IImageEntry *Clone (void) override;
		virtual int GetActualRotation (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers) const override;
		virtual void GetImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers, CObjectImageArray *retImage) override;
		virtual int GetMaxLifetime (void) const override;
		virtual CShipClass *GetShipwreckClass (const CCompositeImageSelector &Selector) const override;
		virtual int GetVariantCount (void) override { return 1; }
		virtual ALERROR InitFromXML (SDesignLoadCtx &Ctx, CIDCounter &IDGen, CXMLElement *pDesc) override;
		virtual void InitSelector (SSelectorInitCtx &InitCtx, CCompositeImageSelector *retSelector) override;
		virtual bool IsConstant (void) override;
		virtual bool IsRotatable (void) const override;
		virtual void MarkImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers) override;
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) override;

	private:
		TArray<IImageEntry *> m_Layers;

		CObjectImageArray m_Image;
	};

class CEffectEntry : public IImageEntry
	{
	public:
		CEffectEntry (void) { }
		virtual ~CEffectEntry (void);

		virtual void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override { if (m_pEffect) m_pEffect->AddTypesUsed(retTypesUsed); }
        virtual IImageEntry *Clone (void) override;
		virtual void GetImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers, CObjectImageArray *retImage) override;
		virtual int GetMaxLifetime (void) const override;
		virtual int GetVariantCount (void) override { return 1; }
		virtual ALERROR InitFromXML (SDesignLoadCtx &Ctx, CIDCounter &IDGen, CXMLElement *pDesc) override;
		virtual bool IsConstant (void) override { return true; }
		virtual void MarkImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers) override;
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) override;

	private:
		CEffectCreatorRef m_pEffect;
	};

class CFilterColorizeEntry : public IImageEntry
	{
	public:
		CFilterColorizeEntry (void) : m_pSource(NULL) { }
		virtual ~CFilterColorizeEntry (void);

		virtual void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override { if (m_pSource) m_pSource->AddTypesUsed(retTypesUsed); }
        virtual IImageEntry *Clone (void) override;
		virtual int GetActualRotation (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers) const override;
		virtual void GetImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers, CObjectImageArray *retImage) override;
		virtual int GetMaxLifetime (void) const override;
		virtual CShipClass *GetShipwreckClass (const CCompositeImageSelector &Selector) const override { return (m_pSource ? m_pSource->GetShipwreckClass(Selector) : NULL); }
		virtual int GetVariantCount (void) override { return 1; }
		virtual ALERROR InitFromXML (SDesignLoadCtx &Ctx, CIDCounter &IDGen, CXMLElement *pDesc) override;
		virtual void InitSelector (SSelectorInitCtx &InitCtx, CCompositeImageSelector *retSelector) override;
		virtual bool IsConstant (void) override;
		virtual bool IsRotatable (void) const override;
		virtual void MarkImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers) override;
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) override;

	private:
		IImageEntry *m_pSource;
		DWORD m_dwHue;
		DWORD m_dwSaturation;
		CG32bitPixel m_rgbColor;
	};

class CImageEntry : public IImageEntry
	{
	public:
        inline ALERROR InitSimpleFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, bool bResolveNow = false, int iDefaultRotationCount = 1) { return m_Image.InitFromXML(Ctx, pDesc, bResolveNow, iDefaultRotationCount); }

		virtual void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override { retTypesUsed->SetAt(m_Image.GetBitmapUNID(), true); }
        virtual IImageEntry *Clone (void) override;
		virtual int GetActualRotation (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers) const override;
		virtual void GetImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers, CObjectImageArray *retImage) override;
		virtual int GetMaxLifetime (void) const override { return m_Image.GetFrameCount() * m_Image.GetTicksPerFrame(); }
        virtual CObjectImageArray &GetSimpleImage (void) override { return m_Image; }
		virtual int GetVariantCount (void) override { return 1; }
		virtual ALERROR InitFromXML (SDesignLoadCtx &Ctx, CIDCounter &IDGen, CXMLElement *pDesc) override;
		virtual bool IsConstant (void) override { return true; }
		virtual bool IsRotatable (void) const override { return (m_Image.GetRotationCount() > 1); }
		virtual void MarkImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers) override { m_Image.MarkImage(); }
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) override { return m_Image.OnDesignLoadComplete(Ctx); }

	private:
		CObjectImageArray m_Image;
	};

class CLocationCriteriaTableEntry : public IImageEntry
	{
	public:
		virtual ~CLocationCriteriaTableEntry (void);

		virtual void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override;
        virtual IImageEntry *Clone (void) override;
		virtual int GetActualRotation (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers) const override;
		virtual void GetImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers, CObjectImageArray *retImage) override;
		virtual int GetMaxLifetime (void) const override;
		virtual CShipClass *GetShipwreckClass (const CCompositeImageSelector &Selector) const override;
		virtual int GetVariantCount (void) override { return m_Table.GetCount(); }
		virtual ALERROR InitFromXML (SDesignLoadCtx &Ctx, CIDCounter &IDGen, CXMLElement *pDesc) override;
		virtual void InitSelector (SSelectorInitCtx &InitCtx, CCompositeImageSelector *retSelector) override;
		virtual bool IsConstant (void) override { return (m_Table.GetCount() == 0 || ((m_Table.GetCount() == 1) && m_Table[0].pImage->IsConstant())); }
		virtual bool IsRotatable (void) const override;
		virtual void MarkImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers) override;
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) override;

	private:
		struct SEntry
			{
			IImageEntry *pImage;
			CAffinityCriteria Criteria;
			};

		static int CalcLocationAffinity (SSelectorInitCtx &InitCtx, const CAffinityCriteria &Criteria);

		TArray<SEntry> m_Table;
		int m_iDefault;
	};

class CRotationTableEntry : public IImageEntry
	{
	public:
		virtual ~CRotationTableEntry (void);

		virtual void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override;
        virtual IImageEntry *Clone (void) override;
		virtual int GetActualRotation (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers) const override;
		virtual void GetImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers, CObjectImageArray *retImage) override;
		virtual int GetMaxLifetime (void) const override;
		virtual int GetVariantCount (void) override { return m_Table.GetCount(); }
		virtual ALERROR InitFromXML (SDesignLoadCtx &Ctx, CIDCounter &IDGen, CXMLElement *pDesc) override;
		virtual void InitSelector (SSelectorInitCtx &InitCtx, CCompositeImageSelector *retSelector) override;
		virtual bool IsConstant (void) override;
		virtual bool IsRotatable (void) const override { return true; }
		virtual void MarkImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers) override;
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) override;

	private:
		struct SEntry
			{
			IImageEntry *pImage;
			int iRotation;
			};

		TArray<SEntry> m_Table;
	};

class CShipwreckEntry : public IImageEntry
	{
	public:
		virtual void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override { if (m_pClass) m_pClass->AddTypesUsed(retTypesUsed); }
        virtual IImageEntry *Clone (void) override;
		virtual int GetActualRotation (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers) const override;
		virtual void GetImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers, CObjectImageArray *retImage) override;
		virtual int GetMaxLifetime (void) const override { return -1; }
		virtual CShipClass *GetShipwreckClass (const CCompositeImageSelector &Selector) const override;
		virtual int GetVariantCount (void) override;
		virtual ALERROR InitFromXML (SDesignLoadCtx &Ctx, CIDCounter &IDGen, CXMLElement *pDesc) override;
		virtual void InitSelector (SSelectorInitCtx &InitCtx, CCompositeImageSelector *retSelector) override;
		virtual bool IsConstant (void) override { return (m_pClass != NULL); }
		virtual bool IsRotatable (void) const override { return true; }
		virtual bool IsShipwreckDesc (void) const { return true; }
		virtual void MarkImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers) override;
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) override;

		static void GetImage (CShipClass *pClass, int iRotation, CObjectImageArray *retImage);

	private:
		inline int GetRotation (const CCompositeImageModifiers &Modifiers) const { if (m_iRotation != -1) return m_iRotation; return Modifiers.GetRotation(); }

		CShipClassRef m_pClass;
		int m_iRotation = -1;
	};

class CTableEntry : public IImageEntry
	{
	public:
		virtual ~CTableEntry (void);

		virtual void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override;
        virtual IImageEntry *Clone (void) override;
		virtual int GetActualRotation (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers) const override;
		virtual void GetImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers, CObjectImageArray *retImage) override;
		virtual int GetMaxLifetime (void) const override;
		virtual CShipClass *GetShipwreckClass (const CCompositeImageSelector &Selector) const override;
		virtual int GetVariantCount (void) override { return m_Table.GetCount(); }
		virtual ALERROR InitFromXML (SDesignLoadCtx &Ctx, CIDCounter &IDGen, CXMLElement *pDesc) override;
		virtual void InitSelector (SSelectorInitCtx &InitCtx, CCompositeImageSelector *retSelector) override;
		virtual bool IsConstant (void) override { return (m_Table.GetCount() == 0 || ((m_Table.GetCount() == 1) && m_Table[0].pImage->IsConstant())); }
		virtual bool IsRotatable (void) const override;
		virtual void MarkImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers) override;
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) override;

	private:
		struct SEntry
			{
			IImageEntry *pImage;
			int iChance;
			};

		TArray<SEntry> m_Table;
		int m_iTotalChance;
	};

static CObjectImageArray EMPTY_IMAGE;
CCompositeImageDesc CCompositeImageDesc::g_Null;

CCompositeImageDesc::CCompositeImageDesc (void) : 
		m_pDesc(NULL),
		m_pRoot(NULL),
		m_bConstant(true)

//	CCompositeImageDesc constructor

	{
	}

CCompositeImageDesc::CCompositeImageDesc (const CCompositeImageDesc &Src)

//  CCompositeImageDesc constructor

    {
    Copy(Src);
    }

CCompositeImageDesc::~CCompositeImageDesc (void)

//	CCompositeImageDesc destructor

	{
    CleanUp();
	}

CCompositeImageDesc &CCompositeImageDesc::operator= (const CCompositeImageDesc &Src)

//  CCompositeImageDesc operator =

    {
    CleanUp();
    Copy(Src);
    return *this;
    }

void CCompositeImageDesc::CleanUp (void)

//  CleanUp
//
//  Restore to initial state.

    {
	m_pDesc = NULL;

    if (m_pRoot)
        {
        delete m_pRoot;
        m_pRoot = NULL;
        }

    m_bConstant = true;
    }

void CCompositeImageDesc::Copy (const CCompositeImageDesc &Src)

//  Copy
//
//  Copy from source. (We assume that we are empty).

    {
    m_pDesc = Src.m_pDesc;

    if (Src.m_pRoot)
        m_pRoot = Src.m_pRoot->Clone();
    else
        m_pRoot = NULL;

    m_bConstant = Src.m_bConstant;
    }

CCompositeImageDesc::SCacheEntry *CCompositeImageDesc::FindCacheEntry (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers) const

//	FindCacheEntry
//
//	Returns the cached entry (or NULL)

	{
	int i;

	//	If we're constant, then we can short-circuit, since there is only
	//	one selector.

	if (m_bConstant)
		{
		for (i = 0; i < m_Cache.GetCount(); i++)
			if (m_Cache[i].Modifiers == Modifiers)
				return &m_Cache[i];

		return NULL;
		}

	//	Otherwise, look in the cache for the proper selector

	for (i = 0; i < m_Cache.GetCount(); i++)
		if (m_Cache[i].Selector == Selector && m_Cache[i].Modifiers == Modifiers)
			return &m_Cache[i];

	return NULL;
	}

CObjectImageArray &CCompositeImageDesc::GetImage (SGetImageCtx &Ctx, const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers, int *retiFrameIndex) const

//	GetImage
//
//	Returns the image

	{
	CCompositeImageSelector::ETypes iType = Selector.GetType(DEFAULT_SELECTOR_ID);

	//	If we have a root descriptor, then we let it generate an image

	if (m_pRoot)
		{
		//	retiRotation is used as the vertical index into the image.
		//	Since each variant is expected to be its own image, we set to 0.

		if (retiFrameIndex)
			*retiFrameIndex = 0;

		//	Look in the cache

		SCacheEntry *pEntry = FindCacheEntry(Selector, Modifiers);
		if (pEntry)
			return pEntry->Image;

		//	If not in the cache, add a new entry

		pEntry = m_Cache.Insert();
		pEntry->Selector = Selector;
		pEntry->Modifiers = Modifiers;

		//	This case is for backwards compatibility

		if (iType == CCompositeImageSelector::typeShipClass && !m_pRoot->IsShipwreckDesc())
			{
			CShipClass *pClass = Selector.GetShipwreckClass();
			if (pClass)
				{
				CShipwreckEntry::GetImage(pClass, Modifiers.GetRotation(), &pEntry->Image);
				return pEntry->Image;
				}
			}

		//	Generate the image

		m_pRoot->GetImage(Selector, Modifiers, &pEntry->Image);

		//	Apply modifiers

		if (!Modifiers.IsEmpty())
			Modifiers.Apply(Ctx, &pEntry->Image);

		//	Done

		return pEntry->Image;
		}

	//	If the selector has an item image, then we use that

	else if (iType == CCompositeImageSelector::typeItemType)
		{
		if (retiFrameIndex)
			*retiFrameIndex = 0;

		return Selector.GetFlotsamImage(DEFAULT_SELECTOR_ID);
		}

	//	This should never happen

	else if (iType == CCompositeImageSelector::typeShipClass)
		{
		ASSERT(false);
		return EMPTY_IMAGE;
		}
	else
		return EMPTY_IMAGE;
	}

int CCompositeImageDesc::GetMaxLifetime (void) const

//	GetMaxLifetime
//
//	Get maximum lifetime

	{
	if (m_pRoot == NULL)
		return 0;

	return m_pRoot->GetMaxLifetime();
	}

size_t CCompositeImageDesc::GetMemoryUsage (void) const

//	GetMemoryUsage
//
//	Returns the amount of memory used.

	{
	int i;
	size_t dwTotal = 0;

	//	We count only cached images that no one else owns.

	for (i = 0; i < m_Cache.GetCount(); i++)
		{
		//	If this image has an UNID, then we don't own it.

		if (m_Cache[i].Image.GetBitmapUNID() != 0)
			continue;

		//	If this entry is for a shipwreck, then we don't own it.

		if (HasShipwreckClass(m_Cache[i].Selector))
			continue;

		//	Add to total

		dwTotal += m_Cache[i].Image.GetMemoryUsage();
		}

	return dwTotal;
	}

CObjectImageArray &CCompositeImageDesc::GetSimpleImage (void)

//  GetSimpleImage
//
//  Returns a simple image (only for descriptors initialized with 
//  InitSimpleFromXML).

    {
    if (m_pRoot == NULL)
        return EMPTY_IMAGE;

    return m_pRoot->GetSimpleImage();
    }

bool CCompositeImageDesc::HasShipwreckClass (const CCompositeImageSelector &Selector, CShipClass **retpClass) const

//	HasShipwreckClass
//
//	Returns TRUE if we're using a shipwreck as an image.

	{
	CShipClass *pWreckClass;

	//	Selector based:

	if (pWreckClass = Selector.GetShipwreckClass())
		{ }

	//	Not class if null

	else if (m_pRoot == NULL)
		pWreckClass = NULL;

	//	Otherwise, ask the root

	else
		pWreckClass = m_pRoot->GetShipwreckClass(Selector);

	//	Done

	if (retpClass)
		*retpClass = pWreckClass;

	return (pWreckClass != NULL);
	}

ALERROR CCompositeImageDesc::InitAsShipwreck (SDesignLoadCtx &Ctx)

//	InitAsShipwreck
//
//	Initialize as a shipwreck (actual class is defined at create time).

	{
	ALERROR error;

	CIDCounter IDGen;
	m_pRoot = new CShipwreckEntry;

	if (error = m_pRoot->InitFromXML(Ctx, IDGen, NULL))
		return error;

	m_bConstant = m_pRoot->IsConstant();

	//	Done

	return NOERROR;
	}

ALERROR CCompositeImageDesc::InitEntryFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CIDCounter &IDGen, IImageEntry **retpEntry)

//	InitEntryFromXML
//
//	Initializes the desc from XML

	{
	ALERROR error;

	IImageEntry *pEntry;
	if (strEquals(pDesc->GetTag(), IMAGE_COMPOSITE_TAG) || strEquals(pDesc->GetTag(), COMPOSITE_TAG))
		pEntry = new CCompositeEntry;
	else if (strEquals(pDesc->GetTag(), IMAGE_EFFECT_TAG) || strEquals(pDesc->GetTag(), EFFECT_TAG))
		pEntry = new CEffectEntry;
	else if (strEquals(pDesc->GetTag(), TABLE_TAG) || strEquals(pDesc->GetTag(), IMAGE_VARIANTS_TAG) || strEquals(pDesc->GetTag(), VARIANTS_TAG))
		pEntry = new CTableEntry;
	else if (strEquals(pDesc->GetTag(), FILTER_COLORIZE_TAG))
		pEntry = new CFilterColorizeEntry;
	else if (strEquals(pDesc->GetTag(), LOCATION_CRITERIA_TABLE_TAG))
		pEntry = new CLocationCriteriaTableEntry;
	else if (strEquals(pDesc->GetTag(), ROTATION_TABLE_TAG))
		pEntry = new CRotationTableEntry;
	else if (strEquals(pDesc->GetTag(), SHIPWRECK_TAG) || strEquals(pDesc->GetTag(), IMAGE_SHIPWRECK_TAG))
		pEntry = new CShipwreckEntry;
	else if (strEquals(pDesc->GetTag(), LOOKUP_TAG) || strEquals(pDesc->GetTag(), IMAGE_LOOKUP_TAG))
		{
		DWORD dwUNID = pDesc->GetAttributeInteger(IMAGE_ID_ATTRIB);
		CCompositeImageType *pEntry = Ctx.GetUniverse().FindCompositeImageType(dwUNID);
		if (pEntry == NULL)
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Unable to find composite image type: %08x."), dwUNID);
			return ERR_FAIL;
			}

		return InitEntryFromXML(Ctx, pEntry->GetDesc(), IDGen, retpEntry);
		}

	//	Otherwise, assume that this is either a plain image or an arbitrary tag with
	//	content elements representing the actual image composition.

	else
		{
		if (pDesc->GetContentElementCount() == 0)
			pEntry = new CImageEntry;
		else
			return InitEntryFromXML(Ctx, pDesc->GetContentElement(0), IDGen, retpEntry);
		}

	//	Init

	if (error = pEntry->InitFromXML(Ctx, IDGen, pDesc))
		return error;

	*retpEntry = pEntry;

	return NOERROR;
	}

ALERROR CCompositeImageDesc::InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	InitFromXML
//
//	Initializes the desc from XML

	{
	ASSERT(m_pRoot == NULL);

	//	Keep the XML around and parse it at Bind time. We need to do this to
	//	support shared ImageComposite types, which may not be loaded yet at this
	//	point.

	m_pDesc = pDesc;

	return NOERROR;
	}

void CCompositeImageDesc::InitSelector (SSelectorInitCtx &InitCtx, CCompositeImageSelector *retSelector)

//	InitSelector
//
//	Initializes the selector based on the variants

	{
	if (m_pRoot == NULL)
		return;

	m_pRoot->InitSelector(InitCtx, retSelector);
	}

ALERROR CCompositeImageDesc::InitSimpleFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, bool bResolveNow, int iDefaultRotationCount)

//  InitSimpleFromXML
//
//  Initialize as a single, simple image. This is mostly used for backwards
//  compatibility with CObjectImageArray.

    {
    ALERROR error;

    ASSERT(m_pRoot == NULL);

    //  No need to keep a descriptor, because we just have an image (plus
    //  bind code uses this to initialize).

    m_pDesc = NULL;

    //  Create a simple image descriptor

    CImageEntry *pImage = new CImageEntry;
    if (error = pImage->InitSimpleFromXML(Ctx, pDesc, bResolveNow, iDefaultRotationCount))
        return error;

    m_pRoot = pImage;
    m_bConstant = true;

    return NOERROR;
    }

void CCompositeImageDesc::MarkImage (void)

//	MarkImage
//
//	Marks all images

	{
	if (m_pRoot == NULL)
		return;

	m_pRoot->MarkImage(CCompositeImageSelector(), CCompositeImageModifiers());
	}

void CCompositeImageDesc::MarkImage (SGetImageCtx &Ctx, const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers)

//	MarkImage
//
//	Marks the image in use

	{
	if (m_pRoot)
		m_pRoot->MarkImage(Selector, Modifiers);

	GetImage(Ctx, Selector, Modifiers).MarkImage();
	}

bool CCompositeImageDesc::NeedsShipwreckClass (void) const

//	NeedsShipwreckClass
//
//	This returns TRUE if we need the shipwreck class to be passed in by the 
//	creator. This happens when we're creating a shipwreck and when the shipwreck
//	station type does not define its own image (it comes from the ship class).

	{
	//	If no descriptor, then we need it.

	if (m_pRoot == NULL)
		return true;

	//	If we're a shipwreck descriptor and we don't define a class, then we 
	//	need one.

	return (m_pRoot->IsShipwreckDesc() && m_pRoot->GetShipwreckClass(CCompositeImageSelector()) == NULL);
	}

ALERROR CCompositeImageDesc::OnDesignLoadComplete (SDesignLoadCtx &Ctx)

//	OnDesignLoadComplete
//
//	Handle deferred loading

	{
	ALERROR error;

	//	If no XML then this is a simple image (e.g., from a CShipClass, which 
	//	does not support compound images and which uses InitSimpleFromXML
	//	to initialize).

	if (m_pDesc == NULL)
        {
        if (m_pRoot)
            {
            if (error = m_pRoot->OnDesignLoadComplete(Ctx))
                return error;
            }

		return NOERROR;
        }

	//	Clean up our previous load, if necessary.

	if (m_pRoot)
		{
		delete m_pRoot;
		m_pRoot = NULL;
		}

	//	Now that all types are loaded, parse the directives

	CIDCounter IDGen;
	if (error = InitEntryFromXML(Ctx, m_pDesc, IDGen, &m_pRoot))
		return error;

	m_bConstant = m_pRoot->IsConstant();

	//	Load
	//
	//	NOTE: We rely on the fact that the cache is empty before OnDesignLoadComplete.
	//	If any calls to GetImage are made before this call, then the cache will have
	//	an invalid (NULL) image.

	if (error = m_pRoot->OnDesignLoadComplete(Ctx))
		return error;

	return NOERROR;
	}

void CCompositeImageDesc::Reinit (void)

//	Reinit
//
//	Reinitialize

	{
	m_Cache.DeleteAll();
	}

//  IImageEntry ----------------------------------------------------------------

CObjectImageArray &IImageEntry::GetSimpleImage (void)

//  GetSimpleImage
//
//  Default impl

    {
    return EMPTY_IMAGE;
    }

//	CCompositeEntry ------------------------------------------------------------

CCompositeEntry::~CCompositeEntry (void)

//	CCompositeEntry destructor

	{
	int i;

	for (i = 0; i < m_Layers.GetCount(); i++)
		delete m_Layers[i];
	}

IImageEntry *CCompositeEntry::Clone (void)

//  Clone
//
//  Returns a new copy.

    {
    int i;

    CCompositeEntry *pDest = new CCompositeEntry;
    pDest->m_dwID = m_dwID;

    pDest->m_Layers.InsertEmpty(m_Layers.GetCount());
    for (i = 0; i < m_Layers.GetCount(); i++)
        pDest->m_Layers[i] = (m_Layers[i] ? m_Layers[i]->Clone() : NULL);

    pDest->m_Image = m_Image;

    return pDest;
    }

int CCompositeEntry::GetActualRotation (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers) const

//	GetActualRotation
//
//	Returns the actual rotation of the image given the selector and modifiers.

	{
	int i;

	//	We return the rotation for the first rotatable layer.

	for (i = 0; i < m_Layers.GetCount(); i++)
		if (m_Layers[i]->IsRotatable())
			return m_Layers[i]->GetActualRotation(Selector, Modifiers);

	return 0;
	}

void CCompositeEntry::GetImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers, CObjectImageArray *retImage)

//	GetImage
//
//	Fills in the image

	{
	int i;

	//	Null case

	if (m_Layers.GetCount() == 0)
		{
		*retImage = EMPTY_IMAGE;
		return;
		}

	//	Get all the layers

	TArray<CObjectImageArray> Result;
	Result.InsertEmpty(m_Layers.GetCount());
	for (i = 0; i < m_Layers.GetCount(); i++)
		m_Layers[i]->GetImage(Selector, Modifiers, &Result[i]);

	//	Create the composited image
	//
	//	First we need to determine the size of the final image, based
	//	on the size and position of each layer.

	int xMin = 0;
	int xMax = 0;
	int yMin = 0;
	int yMax = 0;

	for (i = 0; i < m_Layers.GetCount(); i++)
		{
		CObjectImageArray &LayerImage = Result[i];
		const RECT &rcRect = LayerImage.GetImageRect();

		int xImageOffset = 0;
		int yImageOffset = 0;

		int xMaxImage = (RectWidth(rcRect) / 2) + xImageOffset;
		int xMinImage = xMaxImage - RectWidth(rcRect);
		int yMaxImage = (RectHeight(rcRect) / 2) + yImageOffset;
		int yMinImage = yMaxImage - RectHeight(rcRect);

		xMin = Min(xMin, xMinImage);
		xMax = Max(xMax, xMaxImage);
		yMin = Min(yMin, yMinImage);
		yMax = Max(yMax, yMaxImage);
		}

	//	Create destination image

	int cxWidth = xMax - xMin;
	int cyHeight = yMax - yMin;
	if (cxWidth <= 0 || cyHeight <= 0)
		{
		*retImage = EMPTY_IMAGE;
		return;
		}

	CG32bitImage *pComp = new CG32bitImage;
	pComp->Create(cxWidth, cyHeight, CG32bitImage::alpha1, CG32bitPixel::Null());

	int xCenter = cxWidth / 2;
	int yCenter = cyHeight / 2;

	//	Blt on the destination

	for (i = 0; i < m_Layers.GetCount(); i++)
		{
		CObjectImageArray &LayerImage = Result[i];
		const RECT &rcRect = LayerImage.GetImageRect();

		//	Paint the image

		LayerImage.PaintImage(*pComp,
				xCenter,
				yCenter,
				0,
				0);
		}

	//	Initialize an image

	RECT rcFinalRect;
	rcFinalRect.left = 0;
	rcFinalRect.top = 0;
	rcFinalRect.right = cxWidth;
	rcFinalRect.bottom = cyHeight;

	CObjectImageArray Comp;
	Comp.InitFromBitmap(pComp, rcFinalRect, 0, 0, true);

	//	Done

	retImage->TakeHandoff(Comp);
	}

int CCompositeEntry::GetMaxLifetime (void) const

//	GetMaxLifetime
//
//	Returns the maximum lifetime

	{
	int i;
	int iMaxLifetime = 0;

	for (i = 0; i < m_Layers.GetCount(); i++)
		{
		int iLifetime = m_Layers[i]->GetMaxLifetime();
		if (iLifetime > iMaxLifetime)
			iMaxLifetime = iLifetime;
		}

	return iMaxLifetime;
	}

CShipClass *CCompositeEntry::GetShipwreckClass (const CCompositeImageSelector &Selector) const

//	GetShipwreckClass
//
//	Returns the shipwreck class (if any).

	{
	int i;
	CShipClass *pClass;

	//	We return the class for the first layer we find

	for (i = 0; i < m_Layers.GetCount(); i++)
		if (pClass = m_Layers[i]->GetShipwreckClass(Selector))
			return pClass;

	//	Not found

	return NULL;
	}

ALERROR CCompositeEntry::InitFromXML (SDesignLoadCtx &Ctx, CIDCounter &IDGen, CXMLElement *pDesc)

//	InitFromXML
//
//	Initialize from XML

	{
	ALERROR error;
	int i;

	m_dwID = IDGen.GetID();

	//	Load each sub-entry in turn

	int iCount = pDesc->GetContentElementCount();
	if (iCount == 0)
		{
		::kernelDebugLogPattern("Warning: No entries in composite image for %08x.", (Ctx.pType ? Ctx.pType->GetUNID() : 0));
		return NOERROR;
		}

	m_Layers.InsertEmpty(iCount);
	for (i = 0; i < iCount; i++)
		{
		if (error = CCompositeImageDesc::InitEntryFromXML(Ctx, pDesc->GetContentElement(i), IDGen, &m_Layers[i]))
			return error;
		}

	//	Done

	return NOERROR;
	}

void CCompositeEntry::InitSelector (SSelectorInitCtx &InitCtx, CCompositeImageSelector *retSelector)

//	InitSelector
//
//	Initializes the selector

	{
	int i;

	for (i = 0; i < m_Layers.GetCount(); i++)
		m_Layers[i]->InitSelector(InitCtx, retSelector);
	}

bool CCompositeEntry::IsConstant (void)

//	IsConstant
//
//	Returns TRUE if this is a constant entry

	{
	int i;

	for (i = 0; i < m_Layers.GetCount(); i++)
		if (!m_Layers[i]->IsConstant())
			return false;

	return true;
	}

bool CCompositeEntry::IsRotatable (void) const

//	IsRotatable
//
//	Returns TRUE if we use rotation.

	{
	int i;

	for (i = 0; i < m_Layers.GetCount(); i++)
		if (m_Layers[i]->IsRotatable())
			return true;

	return false;
	}

void CCompositeEntry::MarkImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers)

//	MarkImage
//
//	Mark all images

	{
	int i;

	for (i = 0; i < m_Layers.GetCount(); i++)
		m_Layers[i]->MarkImage(Selector, Modifiers);
	}

ALERROR CCompositeEntry::OnDesignLoadComplete (SDesignLoadCtx &Ctx)

//	OnDesignLoadComplete
//
//	Bind design

	{
	ALERROR error;
	int i;
	
	for (i = 0; i < m_Layers.GetCount(); i++)
		{
		if (error = m_Layers[i]->OnDesignLoadComplete(Ctx))
			return error;
		}

	return NOERROR;
	}

//	CEffectEntry ---------------------------------------------------------------

CEffectEntry::~CEffectEntry (void)

//	CEffectEntry destructor

	{
	}

IImageEntry *CEffectEntry::Clone (void)

//  Clone
//
//  Create a new copy

    {
    CEffectEntry *pDest = new CEffectEntry;
    pDest->m_dwID = m_dwID;

    pDest->m_pEffect = m_pEffect;
    return pDest;
    }

void CEffectEntry::GetImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers, CObjectImageArray *retImage)

//	GetImage
//
//	Fills in the image

	{
	//	Short circuit

	if (m_pEffect.IsEmpty())
		{
		*retImage = EMPTY_IMAGE;
		return;
		}

	//	Create a painter

	CCreatePainterCtx Ctx;
	IEffectPainter *pPainter = m_pEffect->CreatePainter(Ctx);
	if (pPainter == NULL)
		{
		*retImage = EMPTY_IMAGE;
		return;
		}

	//	Get the painter bounds

	RECT rcBounds;
	pPainter->GetBounds(&rcBounds);
	int cxWidth = RectWidth(rcBounds);
	int cyHeight = RectHeight(rcBounds);

	//	Manual compositing

	bool bCanComposite = pPainter->CanPaintComposite();

	//	Create a resulting image

	CG32bitImage *pDest = new CG32bitImage;
	pDest->Create(cxWidth, cyHeight, CG32bitImage::alpha8, (bCanComposite ? CG32bitPixel::Null() : CG32bitPixel(0)));

	//	Set up paint context

	SViewportPaintCtx PaintCtx;

	//	Since we don't have an object, we use the viewport center to indicate
	//	the center of the object.

	PaintCtx.xCenter = -rcBounds.left;
	PaintCtx.yCenter = -rcBounds.top;

	//	Paint

	pPainter->PaintComposite(*pDest, (cxWidth / 2), (cyHeight / 2), PaintCtx);

	//	Initialize an image

	RECT rcFinalRect;
	rcFinalRect.left = 0;
	rcFinalRect.top = 0;
	rcFinalRect.right = cxWidth;
	rcFinalRect.bottom = cyHeight;

	CObjectImageArray Comp;
	Comp.InitFromBitmap(pDest, rcFinalRect, 0, 0, true);

	//	Done

	retImage->TakeHandoff(Comp);
	pPainter->Delete();
	}

int CEffectEntry::GetMaxLifetime (void) const

//	GetMaxLifetime
//
//	Returns the maximum lifetime

	{
	return 0;
	}

ALERROR CEffectEntry::InitFromXML (SDesignLoadCtx &Ctx, CIDCounter &IDGen, CXMLElement *pDesc)

//	InitFromXML
//
//	Initialize from XML

	{
	ALERROR error;

	m_dwID = IDGen.GetID();

	//	Load

	if (error = m_pEffect.LoadEffect(Ctx,
			NULL_STR,
			pDesc,
			pDesc->GetAttribute(EFFECT_ATTRIB)))
		return error;

	//	Done

	return NOERROR;
	}

void CEffectEntry::MarkImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers)

//	MarkImage
//
//	Mark all images

	{
	if (m_pEffect)
		m_pEffect->MarkImages();
	}

ALERROR CEffectEntry::OnDesignLoadComplete (SDesignLoadCtx &Ctx)

//	OnDesignLoadComplete
//
//	Bind design

	{
	ALERROR error;

	if (error = m_pEffect.Bind(Ctx))
		return error;

	return NOERROR;
	}

//	CFilterColorizeEntry -------------------------------------------------------

CFilterColorizeEntry::~CFilterColorizeEntry (void)

//	CFilterColorizeEntry destructor

	{
	if (m_pSource)
		delete m_pSource;
	}

IImageEntry *CFilterColorizeEntry::Clone (void)

//  Clone
//
//  Create a new copy

    {
    CFilterColorizeEntry *pDest = new CFilterColorizeEntry;
    pDest->m_dwID = m_dwID;
    pDest->m_pSource = (m_pSource ? m_pSource->Clone() : NULL);
    pDest->m_dwHue = m_dwHue;
    pDest->m_dwSaturation = m_dwSaturation;
    pDest->m_rgbColor = m_rgbColor;

    return pDest;
    }

int CFilterColorizeEntry::GetActualRotation (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers) const

//	GetActualRotation
//
//	Returns the actual rotation given selector and modifiers

	{
	if (m_pSource == NULL)
		return 0;

	return m_pSource->GetActualRotation(Selector, Modifiers);
	}

void CFilterColorizeEntry::GetImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers, CObjectImageArray *retImage)

//	GetImage
//
//	Fills in the image

	{
	//	Null case

	if (m_pSource == NULL)
		{
		*retImage = EMPTY_IMAGE;
		return;
		}

	//	Get the source image (which we want to colorize)

	CObjectImageArray Source;
	m_pSource->GetImage(Selector, Modifiers, &Source);
	const RECT &rcSource = Source.GetImageRect();
	CG32bitImage &SourceImage = Source.GetImage(NULL_STR);
	int cxWidth = RectWidth(rcSource);
	int cyHeight = RectHeight(rcSource);
	if (!Source.IsLoaded() || cxWidth == 0 || cyHeight == 0)
		{
		*retImage = EMPTY_IMAGE;
		return;
		}

	//	Create the destination image

	CG32bitImage *pDest = new CG32bitImage;
	pDest->Create(cxWidth, cyHeight, SourceImage.GetAlphaType());

	//	Blt the to the destination with colorization

	CGDraw::CopyColorize(*pDest,
			0,
			0,
			SourceImage,
			rcSource.left,
			rcSource.top,
			cxWidth,
			cyHeight,
			(Metric)m_dwHue,
			(Metric)m_dwSaturation / 100.0);

	//	Initialize an image

	RECT rcFinalRect;
	rcFinalRect.left = 0;
	rcFinalRect.top = 0;
	rcFinalRect.right = cxWidth;
	rcFinalRect.bottom = cyHeight;

	CObjectImageArray Comp;
	Comp.InitFromBitmap(pDest, rcFinalRect, 0, 0, true);

	//	Done

	retImage->TakeHandoff(Comp);
	}

int CFilterColorizeEntry::GetMaxLifetime (void) const

//	GetMaxLifetime
//
//	Returns the maximum lifetime

	{
	if (m_pSource == NULL)
		return 0;

	return m_pSource->GetMaxLifetime();
	}

ALERROR CFilterColorizeEntry::InitFromXML (SDesignLoadCtx &Ctx, CIDCounter &IDGen, CXMLElement *pDesc)

//	InitFromXML
//
//	Initialize from XML

	{
	ALERROR error;

	m_dwID = IDGen.GetID();

	CString sColor;
	if (pDesc->FindAttribute(COLOR_ATTRIB, &sColor))
		{
		CG32bitPixel rgbColor = ::LoadRGBColor(sColor);
		CGRealHSB hsbColor = CGRealHSB::FromRGB(rgbColor);
		m_dwHue = (DWORD)hsbColor.GetHue();
		m_dwSaturation = (DWORD)(hsbColor.GetSaturation() * 100.0);
		}
	else
		{
		m_dwHue = pDesc->GetAttributeIntegerBounded(HUE_ATTRIB, 0, 359, 0);
		m_dwSaturation = pDesc->GetAttributeIntegerBounded(SATURATION_ATTRIB, 0, 100, 100);
		}

	//	Load sub-entry

	if (pDesc->GetContentElementCount() == 0)
		return NOERROR;

	if (error = CCompositeImageDesc::InitEntryFromXML(Ctx, pDesc->GetContentElement(0), IDGen, &m_pSource))
		return error;

	//	Done

	return NOERROR;
	}

void CFilterColorizeEntry::InitSelector (SSelectorInitCtx &InitCtx, CCompositeImageSelector *retSelector)

//	InitSelector
//
//	Initializes the selector

	{
	if (m_pSource)
		m_pSource->InitSelector(InitCtx, retSelector);
	}

bool CFilterColorizeEntry::IsConstant (void)

//	IsConstant
//
//	Returns TRUE if this is a constant entry

	{
	if (m_pSource == NULL)
		return true;

	return m_pSource->IsConstant();
	}

bool CFilterColorizeEntry::IsRotatable (void) const

//	IsRotatable
//
//	Returns TRUE if this entry uses rotation

	{
	if (m_pSource == NULL)
		return false;

	return m_pSource->IsRotatable();
	}

void CFilterColorizeEntry::MarkImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers)

//	MarkImage
//
//	Mark all images

	{
	if (m_pSource)
		m_pSource->MarkImage(Selector, Modifiers);
	}

ALERROR CFilterColorizeEntry::OnDesignLoadComplete (SDesignLoadCtx &Ctx)

//	OnDesignLoadComplete
//
//	Bind design

	{
	ALERROR error;

	if (m_pSource == NULL)
		return NOERROR;

	if (error = m_pSource->OnDesignLoadComplete(Ctx))
		return error;

	return NOERROR;
	}

//	CImageEntry ----------------------------------------------------------------

IImageEntry *CImageEntry::Clone (void)

//  Clone
//
//  Create a new copy

    {
    CImageEntry *pDest = new CImageEntry;
    pDest->m_dwID = m_dwID;
    pDest->m_Image = m_Image;

    return pDest;
    }

int CImageEntry::GetActualRotation (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers) const

//	GetActualRotation
//
//	Returns the actual rotation given selector and modifiers

	{
	//	If we have no rotations then its always 0.

	if (m_Image.GetRotationCount() <= 1)
		return 0;

	//	Otherwise, we pick compute the frame and get back the rotation.

	int iFrame = CIntegralRotationDesc::GetFrameIndex(m_Image.GetRotationCount(), Modifiers.GetRotation());
	return CIntegralRotationDesc::GetRotationAngle(m_Image.GetRotationCount(), iFrame);
	}

void CImageEntry::GetImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers, CObjectImageArray *retImage)

//	GetImage
//
//	Returns the appropriate image.

	{
	//	If we have no rotations then we always return the full image.

	if (m_Image.GetRotationCount() <= 1 || Modifiers.ReturnFullImage())
		{
		*retImage = m_Image;
		return;
		}

	//	Otherwise, we use the rotation in the modifiers to compose a new image 
	//	with just that frame.

	int iFrame = CIntegralRotationDesc::GetFrameIndex(m_Image.GetRotationCount(), Modifiers.GetRotation());
	retImage->InitFromFrame(m_Image, 0, iFrame);
	}

ALERROR CImageEntry::InitFromXML (SDesignLoadCtx &Ctx, CIDCounter &IDGen, CXMLElement *pDesc)

//	InitFromXML
//
//	Initialize from XML

	{
	ALERROR error;

	m_dwID = IDGen.GetID();

	//	Initialize the image

	if (error = m_Image.InitFromXML(Ctx, pDesc))
		return error;

	return NOERROR;
	}

//	CLocationCriteriaTableEntry ------------------------------------------------

CLocationCriteriaTableEntry::~CLocationCriteriaTableEntry (void)

//	CTableEntry destructor

	{
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		delete m_Table[i].pImage;
	}

void CLocationCriteriaTableEntry::AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed)

//	AddTypesUsed
//
//	Add types used by this entry

	{
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		m_Table[i].pImage->AddTypesUsed(retTypesUsed);
	}

int CLocationCriteriaTableEntry::CalcLocationAffinity (SSelectorInitCtx &InitCtx, const CAffinityCriteria &Criteria)

//	CalcLocationAffinity
//
//	Computes the location affinity.

	{
	if (InitCtx.pSystem)
		return InitCtx.pSystem->CalcLocationAffinity(Criteria, InitCtx.sLocAttribs, InitCtx.vObjPos);
	else
		return Criteria.CalcWeight([InitCtx](const CString &sAttrib) { return ::HasModifier(InitCtx.sLocAttribs, sAttrib); });
	}

IImageEntry *CLocationCriteriaTableEntry::Clone (void)

//  Clone
//
//  Create a new copy

    {
    int i;

    CLocationCriteriaTableEntry *pDest = new CLocationCriteriaTableEntry;
    pDest->m_dwID = m_dwID;

    pDest->m_Table.InsertEmpty(m_Table.GetCount());
    for (i = 0; i < m_Table.GetCount(); i++)
        {
        pDest->m_Table[i].pImage = (m_Table[i].pImage ? m_Table[i].pImage->Clone() : NULL);
        pDest->m_Table[i].Criteria = m_Table[i].Criteria;
        }

    pDest->m_iDefault = m_iDefault;

    return pDest;
    }

int CLocationCriteriaTableEntry::GetActualRotation (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers) const

//	GetActualRotation
//
//	Returns the actual rotation given selector and modifiers

	{
	int iIndex = Selector.GetVariant(GetID());
	if (iIndex < 0 || iIndex >= m_Table.GetCount())
		return 0;

	return m_Table[iIndex].pImage->GetActualRotation(Selector, Modifiers);
	}

void CLocationCriteriaTableEntry::GetImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers, CObjectImageArray *retImage)

//	GetImage
//
//	Returns the image

	{
	int iIndex = Selector.GetVariant(GetID());
	if (iIndex < 0 || iIndex >= m_Table.GetCount())
		{
		*retImage = EMPTY_IMAGE;
		return;
		}

	m_Table[iIndex].pImage->GetImage(Selector, Modifiers, retImage);
	}

int CLocationCriteriaTableEntry::GetMaxLifetime (void) const

//	GetMaxLifetime
//
//	Returns the maximum lifetime

	{
	int i;
	int iMaxLifetime = 0;

	for (i = 0; i < m_Table.GetCount(); i++)
		{
		int iLifetime = m_Table[i].pImage->GetMaxLifetime();
		if (iLifetime > iMaxLifetime)
			iMaxLifetime = iLifetime;
		}

	return iMaxLifetime;
	}

CShipClass *CLocationCriteriaTableEntry::GetShipwreckClass (const CCompositeImageSelector &Selector) const

//	GetShipwreckClass
//
//	Returns the shipwreck class (if any).

	{
	int iIndex = Selector.GetVariant(GetID());
	if (iIndex < 0 || iIndex >= m_Table.GetCount())
		return NULL;

	return m_Table[iIndex].pImage->GetShipwreckClass(Selector);
	}

ALERROR CLocationCriteriaTableEntry::InitFromXML (SDesignLoadCtx &Ctx, CIDCounter &IDGen, CXMLElement *pDesc)

//	InitFromXML
//
//	Initialize from XML

	{
	ALERROR error;
	int i;

	m_dwID = IDGen.GetID();
	m_iDefault = -1;

	//	Load each sub-entry in turn

	int iCount = pDesc->GetContentElementCount();
	if (iCount == 0)
		return NOERROR;

	m_Table.InsertEmpty(iCount);
	for (i = 0; i < iCount; i++)
		{
		CXMLElement *pItem = pDesc->GetContentElement(i);

		if (error = CCompositeImageDesc::InitEntryFromXML(Ctx, pItem, IDGen, &m_Table[i].pImage))
			return error;

		//	Load the criteria

		CString sCriteria = pItem->GetAttribute(CRITERIA_ATTRIB);
		if (error = m_Table[i].Criteria.Parse(sCriteria, &Ctx.sError))
			return error;

		if (m_iDefault == -1 && m_Table[i].Criteria.MatchesDefault())
			m_iDefault = i;
		}

	//	If we don't have a default, the pick the last item.

	if (m_iDefault == -1 
			&& m_Table.GetCount() > 0)
		m_iDefault = m_Table.GetCount() - 1;

	//	Done

	return NOERROR;
	}

void CLocationCriteriaTableEntry::InitSelector (SSelectorInitCtx &InitCtx, CCompositeImageSelector *retSelector)

//	InitSelector
//
//	Initializes the selector

	{
	int i;

	//	Generate the table based on criteria and location

	TProbabilityTable<int> ProbTable;
	for (i = 0; i < m_Table.GetCount(); i++)
		{
		//	Compute the probability of this entry at the
		//	given location.

		int iChance = CalcLocationAffinity(InitCtx, m_Table[i].Criteria);
		if (iChance > 0)
			ProbTable.Insert(i, iChance);
		}

	//	If none match, then add the default element

	if (ProbTable.GetCount() == 0 && m_iDefault != -1)
		ProbTable.Insert(m_iDefault, 1000);

	//	Roll

	if (ProbTable.GetCount() > 0)
		{
		int iRoll = ProbTable.RollPos();
		int iIndex = ProbTable[iRoll];
		retSelector->AddVariant(GetID(), iIndex);
		m_Table[iIndex].pImage->InitSelector(InitCtx, retSelector);
		}
	}

bool CLocationCriteriaTableEntry::IsRotatable (void) const

//	IsRotatable
//
//	Returns TRUE if we use rotation.

	{
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		if (m_Table[i].pImage->IsRotatable())
			return true;

	return false;
	}

void CLocationCriteriaTableEntry::MarkImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers)

//	MarkImage
//
//	Mark all images

	{
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		m_Table[i].pImage->MarkImage(Selector, Modifiers);
	}

ALERROR CLocationCriteriaTableEntry::OnDesignLoadComplete (SDesignLoadCtx &Ctx)

//	OnDesignLoadComplete
//
//	Bind design

	{
	ALERROR error;
	int i;
	
	for (i = 0; i < m_Table.GetCount(); i++)
		{
		if (error = m_Table[i].pImage->OnDesignLoadComplete(Ctx))
			return error;
		}

	return NOERROR;
	}

//	CRotationTableEntry --------------------------------------------------------

CRotationTableEntry::~CRotationTableEntry (void)

//	CRotationTableEntry destructor

	{
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		delete m_Table[i].pImage;
	}

void CRotationTableEntry::AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed)

//	AddTypesUsed
//
//	Add types used by this entry

	{
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		m_Table[i].pImage->AddTypesUsed(retTypesUsed);
	}

IImageEntry *CRotationTableEntry::Clone (void)

//  Clone
//
//  Create a new copy

    {
    int i;

    CRotationTableEntry *pDest = new CRotationTableEntry;
    pDest->m_dwID = m_dwID;

    pDest->m_Table.InsertEmpty(m_Table.GetCount());
    for (i = 0; i < m_Table.GetCount(); i++)
        {
        m_Table[i].pImage = (m_Table[i].pImage ? m_Table[i].pImage->Clone() : NULL);
        m_Table[i].iRotation = m_Table[i].iRotation;
        }

    return pDest;
    }

int CRotationTableEntry::GetActualRotation (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers) const

//	GetActualRotation
//
//	Returns the actual rotation given selector and modifiers

	{
	int i;

	//	Look for the closest rotation to the given one.

	int iBest = -1;
	int iBestDiff = 360;
	for (i = 0; i < m_Table.GetCount(); i++)
		{
		int iDiff = Absolute(AngleOffset(Modifiers.GetRotation(), m_Table[i].iRotation));
		if (iDiff < iBestDiff)
			{
			iBest = i;
			iBestDiff = iDiff;
			}
		}

	if (iBest == -1)
		return 0;

	return m_Table[iBest].iRotation;
	}

void CRotationTableEntry::GetImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers, CObjectImageArray *retImage)

//	GetImage
//
//	Returns the image

	{
	int i;

	//	Look for the closest rotation to the given one.

	int iBest = -1;
	int iBestDiff = 360;
	for (i = 0; i < m_Table.GetCount(); i++)
		{
		int iDiff = Absolute(AngleOffset(Modifiers.GetRotation(), m_Table[i].iRotation));
		if (iDiff < iBestDiff)
			{
			iBest = i;
			iBestDiff = iDiff;
			}
		}

	if (iBest == -1)
		{
		*retImage = EMPTY_IMAGE;
		return;
		}

	m_Table[iBest].pImage->GetImage(Selector, Modifiers, retImage);
	}

int CRotationTableEntry::GetMaxLifetime (void) const

//	GetMaxLifetime
//
//	Returns the maximum lifetime

	{
	int i;
	int iMaxLifetime = 0;

	for (i = 0; i < m_Table.GetCount(); i++)
		{
		int iLifetime = m_Table[i].pImage->GetMaxLifetime();
		if (iLifetime > iMaxLifetime)
			iMaxLifetime = iLifetime;
		}

	return iMaxLifetime;
	}

ALERROR CRotationTableEntry::InitFromXML (SDesignLoadCtx &Ctx, CIDCounter &IDGen, CXMLElement *pDesc)

//	InitFromXML
//
//	Initialize from XML

	{
	ALERROR error;
	int i;

	m_dwID = IDGen.GetID();

	//	Load each sub-entry in turn

	int iCount = pDesc->GetContentElementCount();
	if (iCount == 0)
		return NOERROR;

	m_Table.InsertEmpty(iCount);
	for (i = 0; i < iCount; i++)
		{
		CXMLElement *pItem = pDesc->GetContentElement(i);

		if (error = CCompositeImageDesc::InitEntryFromXML(Ctx, pItem, IDGen, &m_Table[i].pImage))
			return error;

		//	Load the chance

		m_Table[i].iRotation = AngleMod(pItem->GetAttributeIntegerBounded(ROTATION_ATTRIB, -360, 360, 1));
		}

	//	Done

	return NOERROR;
	}

void CRotationTableEntry::InitSelector (SSelectorInitCtx &InitCtx, CCompositeImageSelector *retSelector)

//	InitSelector
//
//	Initializes the selector

	{
	int i;

	//	Since the rotation could change at runtime, we initialize all selectors.

	for (i = 0; i < m_Table.GetCount(); i++)
		m_Table[i].pImage->InitSelector(InitCtx, retSelector);
	}

bool CRotationTableEntry::IsConstant (void)

//	IsConstant
//
//	Returns TRUE if we don't change based on selector.

	{
	int i;

	//	Since we only change due to rotation (which comes from modifiers), we're
	//	constant as long as all our images are constant.

	for (i = 0; i < m_Table.GetCount(); i++)
		if (!m_Table[i].pImage->IsConstant())
			return false;

	return true;
	}

void CRotationTableEntry::MarkImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers)

//	MarkImage
//
//	Mark all images

	{
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		m_Table[i].pImage->MarkImage(Selector, Modifiers);
	}

ALERROR CRotationTableEntry::OnDesignLoadComplete (SDesignLoadCtx &Ctx)

//	OnDesignLoadComplete
//
//	Bind design

	{
	ALERROR error;
	int i;
	
	for (i = 0; i < m_Table.GetCount(); i++)
		{
		if (error = m_Table[i].pImage->OnDesignLoadComplete(Ctx))
			return error;
		}

	return NOERROR;
	}

//	CShipwreckEntry ------------------------------------------------------------

IImageEntry *CShipwreckEntry::Clone (void)

//	Clone
//
//	Clone the entry

	{
	CShipwreckEntry *pDest = new CShipwreckEntry;
	pDest->m_dwID = m_dwID;
	pDest->m_pClass = m_pClass;
	pDest->m_iRotation = m_iRotation;

	return pDest;
	}

int CShipwreckEntry::GetActualRotation (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers) const

//	GetActualRotation
//
//	Returns the actual rotation given the selector

	{
	if (m_pClass == NULL)
		return 0;

	int iFrameCount = m_pClass->GetImage().GetRotationCount();
	if (iFrameCount <= 1)
		return 0;

	return CIntegralRotationDesc::GetRotationAngle(iFrameCount, CIntegralRotationDesc::GetFrameIndex(iFrameCount, GetRotation(Modifiers)));
	}

void CShipwreckEntry::GetImage (CShipClass *pClass, int iRotation, CObjectImageArray *retImage)

//	GetImage
//
//	Returns an image.

	{
	const CShipwreckDesc &WreckDesc = pClass->GetWreckDesc();
	CObjectImageArray *pWreckImage = WreckDesc.GetWreckImage(pClass, iRotation);
	if (pWreckImage == NULL)
		{
		*retImage = EMPTY_IMAGE;
		return;
		}

	*retImage = *pWreckImage;
	}

void CShipwreckEntry::GetImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers, CObjectImageArray *retImage)

//	GetImage
//
//	Returns an image.

	{
	CShipClass *pClass = GetShipwreckClass(Selector);
	if (pClass == NULL)
		{
		*retImage = EMPTY_IMAGE;
		return;
		}

	GetImage(pClass, GetRotation(Modifiers), retImage);
	}

CShipClass *CShipwreckEntry::GetShipwreckClass (const CCompositeImageSelector &Selector) const

//	GetShipwreckClass
//
//	Returns the class.

	{
	//	If we have a class, then we use that.

	if (m_pClass)
		return m_pClass;

	//	Otherwise, return the selector class (if any)

	else
		return Selector.GetShipwreckClass();
	}

int CShipwreckEntry::GetVariantCount (void)

//	GetVariantCount
//
//	Return the number of valid variants.

	{
	return 1;
	}

ALERROR CShipwreckEntry::InitFromXML (SDesignLoadCtx &Ctx, CIDCounter &IDGen, CXMLElement *pDesc)

//	InitFromXML
//
//	Initialize from XML

	{
	ALERROR error;

	m_dwID = IDGen.GetID();

	//	Get the class (pDesc can be NULL if we're creating a generic shipwreck).

	if (pDesc)
		{
		if (error = m_pClass.LoadUNID(Ctx, pDesc->GetAttribute(CLASS_ATTRIB)))
			return error;
		}

	return NOERROR;
	}

void CShipwreckEntry::InitSelector (SSelectorInitCtx &InitCtx, CCompositeImageSelector *retSelector)

//	InitSelector
//
//	Initialize selector

	{
	}

void CShipwreckEntry::MarkImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers)

//	MarkImage
//
//	Mark images being used.

	{
	CShipClass *pClass = GetShipwreckClass(Selector);
	if (pClass == NULL)
		return;

	//	Mark the underlying class image (because we may need it to create a wreck
	//	image).

	pClass->MarkImages(false);

	//	Mark any cached wreck images.

	const CShipwreckDesc &WreckDesc = pClass->GetWreckDesc();
	WreckDesc.MarkImages(pClass, GetRotation(Modifiers));
	}

ALERROR CShipwreckEntry::OnDesignLoadComplete (SDesignLoadCtx &Ctx)

//	OnDesignLoadComplete
//
//	Bind design

	{
	ALERROR error;

	if (error = m_pClass.Bind(Ctx))
		return error;

	return NOERROR;
	}

//	CTableEntry ----------------------------------------------------------------

CTableEntry::~CTableEntry (void)

//	CTableEntry destructor

	{
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		delete m_Table[i].pImage;
	}

void CTableEntry::AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed)

//	AddTypesUsed
//
//	Add types used by this entry

	{
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		m_Table[i].pImage->AddTypesUsed(retTypesUsed);
	}

IImageEntry *CTableEntry::Clone (void)

//  Clone
//
//  Create a new copy

    {
    int i;

    CTableEntry *pDest = new CTableEntry;
    pDest->m_dwID = m_dwID;

    pDest->m_Table.InsertEmpty(m_Table.GetCount());
    for (i = 0; i < m_Table.GetCount(); i++)
        {
        m_Table[i].pImage = (m_Table[i].pImage ? m_Table[i].pImage->Clone() : NULL);
        m_Table[i].iChance = m_Table[i].iChance;
        }

    pDest->m_iTotalChance = m_iTotalChance;

    return pDest;
    }

int CTableEntry::GetActualRotation (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers) const

//	GetActualRotation
//
//	Returns the actual rotation given selector and modifiers

	{
	int iIndex = Selector.GetVariant(GetID());
	if (iIndex < 0 || iIndex >= m_Table.GetCount())
		return 0;

	return m_Table[iIndex].pImage->GetActualRotation(Selector, Modifiers);
	}

void CTableEntry::GetImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers, CObjectImageArray *retImage)

//	GetImage
//
//	Returns the image

	{
	int iIndex = Selector.GetVariant(GetID());
	if (iIndex < 0 || iIndex >= m_Table.GetCount())
		{
		*retImage = EMPTY_IMAGE;
		return;
		}

	m_Table[iIndex].pImage->GetImage(Selector, Modifiers, retImage);
	}

int CTableEntry::GetMaxLifetime (void) const

//	GetMaxLifetime
//
//	Returns the maximum lifetime

	{
	int i;
	int iMaxLifetime = 0;

	for (i = 0; i < m_Table.GetCount(); i++)
		{
		int iLifetime = m_Table[i].pImage->GetMaxLifetime();
		if (iLifetime > iMaxLifetime)
			iMaxLifetime = iLifetime;
		}

	return iMaxLifetime;
	}

CShipClass *CTableEntry::GetShipwreckClass (const CCompositeImageSelector &Selector) const

//	GetShipwreckClass
//
//	Returns the shipwreck class (if any).

	{
	int iIndex = Selector.GetVariant(GetID());
	if (iIndex < 0 || iIndex >= m_Table.GetCount())
		return NULL;

	return m_Table[iIndex].pImage->GetShipwreckClass(Selector);
	}

ALERROR CTableEntry::InitFromXML (SDesignLoadCtx &Ctx, CIDCounter &IDGen, CXMLElement *pDesc)

//	InitFromXML
//
//	Initialize from XML

	{
	ALERROR error;
	int i;

	m_dwID = IDGen.GetID();

	//	Load each sub-entry in turn

	int iCount = pDesc->GetContentElementCount();
	if (iCount == 0)
		return NOERROR;

	m_iTotalChance = 0;
	m_Table.InsertEmpty(iCount);
	for (i = 0; i < iCount; i++)
		{
		CXMLElement *pItem = pDesc->GetContentElement(i);

		if (error = CCompositeImageDesc::InitEntryFromXML(Ctx, pItem, IDGen, &m_Table[i].pImage))
			return error;

		//	Load the chance

		m_Table[i].iChance = pItem->GetAttributeIntegerBounded(CHANCE_ATTRIB, 0, -1, 1);
		m_iTotalChance += m_Table[i].iChance;
		}

	//	Done

	return NOERROR;
	}

void CTableEntry::InitSelector (SSelectorInitCtx &InitCtx, CCompositeImageSelector *retSelector)

//	InitSelector
//
//	Initializes the selector

	{
	int i;

	//	Roll dice and pick an entry

	int iRoll = mathRandom(1, m_iTotalChance);
	for (i = 0; i < m_Table.GetCount(); i++)
		if (iRoll <= m_Table[i].iChance)
			{
			retSelector->AddVariant(GetID(), i);
			m_Table[i].pImage->InitSelector(InitCtx, retSelector);
			return;
			}
		else
			iRoll -= m_Table[i].iChance;
	}

bool CTableEntry::IsRotatable (void) const

//	IsRotatable
//
//	Returns TRUE if we use rotation.

	{
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		if (m_Table[i].pImage->IsRotatable())
			return true;

	return false;
	}

void CTableEntry::MarkImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers)

//	MarkImage
//
//	Mark all images

	{
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		m_Table[i].pImage->MarkImage(Selector, Modifiers);
	}

ALERROR CTableEntry::OnDesignLoadComplete (SDesignLoadCtx &Ctx)

//	OnDesignLoadComplete
//
//	Bind design

	{
	ALERROR error;
	int i;
	
	for (i = 0; i < m_Table.GetCount(); i++)
		{
		if (error = m_Table[i].pImage->OnDesignLoadComplete(Ctx))
			return error;
		}

	return NOERROR;
	}
