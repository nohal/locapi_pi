// locapi.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

// Array of report types of interest.
IID REPORT_TYPES[] = { IID_ILatLongReport, IID_ICivicAddressReport };
class CInitializeATL : public CAtlExeModuleT<CInitializeATL>{};
CInitializeATL g_InitializeATL; 
// Initializes ATL for this application. This also does CoInitialize for us

void PrintReportStatus(LOCATION_REPORT_STATUS status)
{

	switch (status)
	{
	case REPORT_RUNNING:
		wprintf(L"Report ready.\n");
		break;
	case REPORT_NOT_SUPPORTED:
		wprintf(L"No devices detected.\n");
		break;
	case REPORT_ERROR:
		wprintf(L"Report error.\n");
		break;
	case REPORT_ACCESS_DENIED:
		wprintf(L"Access denied to report.\n");
		break;
	case REPORT_INITIALIZING:
		wprintf(L"Report is initializing.\n");
		break;
	}
}



void PrintLatLongReport(ILatLongReport *pLatLongReport)
{
	DOUBLE latitude = 0, longitude = 0, altitude = 0;
	DOUBLE errorRadius = 0, altitudeError = 0;
	// Print the Latitude
	if (SUCCEEDED(pLatLongReport->GetLatitude(&latitude)))
	{
		wprintf(L"Latitude: %f\n", latitude);
	}

	// Print the Longitude
	if (SUCCEEDED(pLatLongReport->GetLongitude(&longitude)))
	{
		wprintf(L"Longitude: %f\n", longitude);
	}

	// Print the Altitude
	if (SUCCEEDED(pLatLongReport->GetAltitude(&altitude)))
	{
		wprintf(L"Altitude: %f\n", altitude);
	}
	else
	{
		// Altitude is optional and may not be available
		wprintf(L"Altitude: Not available.\n");
	}

	// Print the Error Radius
	if (SUCCEEDED(pLatLongReport->GetErrorRadius(&errorRadius)))
	{
		wprintf(L"Error Radius: %f\n", errorRadius);
	}

	// Print the Altitude Error
	if (SUCCEEDED(pLatLongReport->GetAltitudeError(&altitudeError)))
	{
		wprintf(L"Altitude Error: %f\n", altitudeError);
	}
	else
	{
		// Altitude Error is optional and may not be available
		wprintf(L"Altitude Error: Not available.\n");
	}
}
// This is a helper function that allows us to print a GUID to the // console in a friendly format

void PrintGUID(const GUID guid)
{
	wprintf(L"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}\n",
		guid.Data1,
		guid.Data2,
		guid.Data3,
		guid.Data4[0],
		guid.Data4[1],
		guid.Data4[2],
		guid.Data4[3],
		guid.Data4[4],
		guid.Data4[5],
		guid.Data4[6],
		guid.Data4[7]);
}
// This is a helper function that allows us to print a SYSTEMTIME to // the console in a friendly format
void PrintTimestamp(const SYSTEMTIME time)
{
	wprintf(L"Timestamp: YY:%d, MM:%d, DD:%d, HH:%d, MM:%d, SS:%d, MS:%d\n",
		time.wYear,
		time.wMonth,
		time.wDay,
		time.wHour,
		time.wMinute,
		time.wSecond,
		time.wMilliseconds);
}



int _tmain(int argc, _TCHAR* argv[])
{
	CComPtr<ILocation> spLocation; // This is the main Location interface

	// Create the Location object
	if (FAILED(spLocation.CoCreateInstance(CLSID_Location)))    
	{
		wprintf(L"Failed to create Location COM object.\n");
		return 1;
	}

	// Request permissions for this user account to receive location data // for all the types defined in REPORT_TYPES
	if (FAILED(spLocation->RequestPermissions(NULL, REPORT_TYPES, ARRAYSIZE(REPORT_TYPES), TRUE))) // TRUE means a synchronous 
		// request
	{
		wprintf(L"Warning: Unable to request permissions.\n");
	}

	LOCATION_REPORT_STATUS status = REPORT_NOT_SUPPORTED; 
	// Get the status of this report type
	if (FAILED(spLocation->GetReportStatus(IID_ILatLongReport, &status))) 
	{
		wprintf(L"Error: Unable to obtain lat/long report status.\n");
		return 1;
	}

	wprintf(L"Lat./long. Report status: ");
	PrintReportStatus(status);


	if (status == REPORT_RUNNING)
	{
		// This is our location report object
		CComPtr<ILocationReport> spLocationReport;
		// This is our LatLong report object
		CComPtr<ILatLongReport> spLatLongReport; 
		// Get the current location report,
		// then get the ILatLongReport interface from ILocationReport,
		// then ensure it isn't NULL
		if ((SUCCEEDED(spLocation->GetReport(IID_ILatLongReport, 
			&spLocationReport))) &&
			(SUCCEEDED(spLocationReport->QueryInterface(
			IID_PPV_ARGS(&spLatLongReport)))) &&
			(NULL != spLatLongReport.p))
		{
			// Print the Timestamp
			SYSTEMTIME systemTime;
			if (SUCCEEDED(spLatLongReport->GetTimestamp(&systemTime)))
			{
				PrintTimestamp(systemTime);
			}
			// Print the Sensor ID GUID
			GUID sensorID = {0};
			if (SUCCEEDED(spLatLongReport->GetSensorID(&sensorID)))
			{
				wprintf(L"SensorID: ");
				PrintGUID(sensorID);
			}// Print the report
			wprintf(L"\nReport:\n");
			PrintLatLongReport(spLatLongReport);
		}
	}

	return 0;
}
