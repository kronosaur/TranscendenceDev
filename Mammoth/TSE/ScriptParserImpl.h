//	ScriptParserImpl.cpp
//
//	CScript class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CScriptParser
	{
	public:
		CScriptParser (const CDesignCollection &Design, const CDesignType *pSource, const char *pBuffer, int iLength);

		bool ParseNextParagraph (CScript::SParagraph *retParagraph);

	private:
		static constexpr int MAX_PARAGRAPH_SIZE = 16 * 1024;

		enum class State
			{
			none,

			start,
			normalText,
			normalTextEoL,
			alignMarker,
			centeredMarker,
			centeredText,
			centeredTextEoL,
			variable,
			variableError,
			variableParam,
			done,
			};

		void CreateParagraphFromBuffer (CScript::ParagraphStyle iStyle, CScript::SParagraph *retParagraph);
		void FlushToBuffer (void);
		void WriteLanguageEntryToBuffer (const CString &sID);

		const CDesignCollection &m_Design;
		const CDesignType *m_pSource = NULL;
		const char *m_pBuffer = NULL;
		const char *m_pBufferEnd = NULL;

		const char *m_pPos = NULL;
		State m_iState = State::start;
		const char *m_pStart = NULL;
		CMemoryWriteStream m_Buffer;

		State m_iPrevState = State::none;
		const CDesignType *m_pType = NULL;
		CString m_sID;
	};
