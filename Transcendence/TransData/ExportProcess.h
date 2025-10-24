//  ExportProcess.h
//
//  CExportProcess class definition. This class is used to export a game
//  Copyright (c) 2023 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CExportProcess
	{
	public:

		struct SOptions
			{
			CString sOutputFolder;					//	Output folder
			CString sInstallation;					//	Expansion group ID
			};

		static bool ParseOptions (const CXMLElement& CmdLine, SOptions& retOptions);

		CExportProcess (CUniverse &Universe, const SOptions& Options) :
				m_Universe(Universe), m_Options(Options) 
			{ }

		ALERROR Run ();

	private:

		CString ComposeOutputFilespec (const CString& sFilename) const;
		void WriteShipClasses (IWriteStream& Output);

		CUniverse &m_Universe;
		SOptions m_Options;
	};
