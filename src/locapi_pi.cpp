/***************************************************************************
* $Id: locapi_pi.cpp, v0.2 2013/05/09 nohal Exp $
*
* Project:  OpenCPN
* Purpose:  Windows Location API Plugin
* Author:   Pavel Kalian
*
***************************************************************************
*   Copyright (C) 2013 by Pavel Kalian                                    *
*   $EMAIL$                                                               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
***************************************************************************
*/


#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include <typeinfo>
#include "locapi_pi.h"
#include "icons.h"
#include "nmea0183.h"

// the class factories, used to create and destroy instances of the PlugIn

extern "C" DECL_EXP opencpn_plugin* create_pi ( void *ppimgr )
{
	return new locapi_pi ( ppimgr );
}

extern "C" DECL_EXP void destroy_pi ( opencpn_plugin* p )
{
	delete p;
}

//---------------------------------------------------------------------------------------------------------
//
//    LOCAPI PlugIn Implementation
//
//---------------------------------------------------------------------------------------------------------

locapi_pi::locapi_pi ( void *ppimgr )
	: opencpn_plugin_16 ( ppimgr ), wxTimer ( this )
{
	// Create the PlugIn icons
	initialize_images();

	m_interval = 1000;
}

int locapi_pi::Init ( void )
{
	AddLocaleCatalog ( _T ( "opencpn-locapi_pi" ) );

	m_pconfig = GetOCPNConfigObject();
	LoadConfig();

	status = REPORT_NOT_SUPPORTED;
	if (SUCCEEDED(spLoc.CoCreateInstance(CLSID_Location))) // Create the Location object
	{
		wxLogMessage(_T("LOCAPI: Instance created"));
		// Array of report types of interest. Other ones include IID_ICivicAddressReport
		IID REPORT_TYPES[] = { IID_ILatLongReport };
		// Request permissions for this user account to receive location data for all the
		// types defined in REPORT_TYPES (which is currently just one report)
		if (FAILED(spLoc->RequestPermissions(NULL, REPORT_TYPES, ARRAYSIZE(REPORT_TYPES), TRUE))) // TRUE means a synchronous request
		{
			wxMessageBox( _("Warning: Unable to request permissions to receive location updates.") );
		}		
	}
	else
	{
		wxLogMessage(_T("LOCAPI: Instance creation failed"));
	}

	Start( m_interval, wxTIMER_CONTINUOUS );
	return (
		WANTS_CONFIG
		);
}

bool locapi_pi::DeInit ( void )
{
	SaveConfig();
	if ( IsRunning() ) // Timer started?
	{
		Stop(); // Stop timer
	}

	return true;
}

int locapi_pi::GetAPIVersionMajor()
{
	return MY_API_VERSION_MAJOR;
}

int locapi_pi::GetAPIVersionMinor()
{
	return MY_API_VERSION_MINOR;
}

int locapi_pi::GetPlugInVersionMajor()
{
	return PLUGIN_VERSION_MAJOR;
}

int locapi_pi::GetPlugInVersionMinor()
{
	return PLUGIN_VERSION_MINOR;
}

wxBitmap *locapi_pi::GetPlugInBitmap()
{
	return _img_locapi_pi;
}

wxString locapi_pi::GetCommonName()
{
	return _ ( "LOCAPI" );
}


wxString locapi_pi::GetShortDescription()
{
	return _ ( "Windows Location API position provider plugin for OpenCPN" );
}

wxString locapi_pi::GetLongDescription()
{
	return _ ( "Windows Location API position provider plugin for OpenCPN\n\
			   Reads the location information provided by integrated sensors in Windows 7 and Windows 8 crap\n\
			   like modern ultrabooks and tablets and converts it to a simulated NMEA stream." );
}

bool locapi_pi::LoadConfig ( void )
{
	return false;
}

bool locapi_pi::SaveConfig ( void )
{
	return false;
}

void locapi_pi::Notify()
{
	// Get the report status
	if (spLoc && SUCCEEDED(spLoc->GetReportStatus(IID_ILatLongReport, &status))) 
	{
		wxLogMessage(_T("LOCAPI: Tick, going to get position..."));
		CComPtr<ILocationReport> spLocationReport; // This is our location report object
		CComPtr<ILatLongReport> spLatLongReport; // This is our LatLong report object

		// Get the current location report,
		// then get the ILatLongReport interface from ILocationReport,
		// then ensure it isn't NULL
		if ((SUCCEEDED(spLoc->GetReport(IID_ILatLongReport, &spLocationReport))) &&
			(SUCCEEDED(spLocationReport->QueryInterface(&spLatLongReport))))
		{
			lfPrevLat = lfThisLat;
			lfPrevLng = lfThisLng;

			// Fetch the new latitude & longitude
			spLatLongReport->GetLatitude(&lfThisLat);
			spLatLongReport->GetLongitude(&lfThisLng);
			wxLogMessage(wxString::Format( _T("LOCAPI: Position: %f, %f"), lfThisLat, lfThisLng) );
			GLL stc;
			stc.Position.Latitude.Set( fabs(lfThisLat), lfThisLat > 0 ? _T("N") : _T("N") );
			stc.Position.Longitude.Set( fabs(lfThisLng), lfThisLng > 0 ? _T("E") : _T("W") );
			stc.Talker = _T("GP");
			stc.UTCTime = wxDateTime::Now().ToUTC().Format( _T("%H%M%S") );
			SENTENCE snt;
			stc.Write( snt );

			PushNMEABuffer( snt.Sentence + _T("\r\n") );
		}
	}
}

void locapi_pi::SetInterval( int interval )
{
	m_interval = interval;
	if ( IsRunning() ) // Timer started?
		Start( m_interval, wxTIMER_CONTINUOUS ); // restart timer with new interval
}
