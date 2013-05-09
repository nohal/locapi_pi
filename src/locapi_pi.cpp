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
}

int locapi_pi::Init ( void )
{
    AddLocaleCatalog ( _T ( "opencpn-locapi_pi" ) );

    m_pconfig = GetOCPNConfigObject();
    LoadConfig();

    return (
               WANTS_CONFIG
           );
}

bool locapi_pi::DeInit ( void )
{
    SaveConfig();

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
