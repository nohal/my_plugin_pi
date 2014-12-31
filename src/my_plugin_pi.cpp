
#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
  #ifdef ocpnUSE_GL
      #include <wx/glcanvas.h>
  #endif
#endif //precompiled headers

#include <wx/fileconf.h>
#include <wx/stdpaths.h>

////////////////////////////////////////////////
#include <wx/sstream.h>
#include <wx/protocol/http.h>
//////////////////////////////////////////

#include "my_plugin_pi.h"
#include <map>
#include <string>

std::map<std::string,PlugIn_Waypoint*> myMap;

//---------------------------------------------------------------------------------------------------------
//
//    MY_PLUGIN PlugIn Implementation
//
//---------------------------------------------------------------------------------------------------------



// the class factories, used to create and destroy instances of the PlugIn

extern "C" DECL_EXP opencpn_plugin* create_pi(void *ppimgr) {
    return new my_plugin_pi(ppimgr);
}


extern "C" DECL_EXP void destroy_pi(opencpn_plugin* p) {
    delete p;
}



#include "icons.h"


//---------------------------------------------------------------------------------------------------------
//
//          PlugIn initialization and de-init
//
//---------------------------------------------------------------------------------------------------------

my_plugin_pi::my_plugin_pi(void *ppimgr)
    :opencpn_plugin_112(ppimgr)
{
      // Create the PlugIn icons
      initialize_images();
	  
	
	  isPlugInActive = false;
     
}

my_plugin_pi::~my_plugin_pi(void)
{
      delete _img_my_plugin_pi;
      delete _img_my_plugin;
     
}

int my_plugin_pi::Init(void)
{
	isPlugInActive =false;
	m_ActiveMarker = NULL;
	
      AddLocaleCatalog( _T("opencpn-my_plugin_pi") );

      

      //    Get a pointer to the opencpn configuration object
      m_pconfig = GetOCPNConfigObject();

    

      // Get a pointer to the opencpn display canvas, to use as a parent for the MY_PLUGIN dialog
      m_parent_window = GetOCPNCanvasWindow();
	  
	 


      //    This PlugIn needs a toolbar icon, so request its insertion if enabled locally
      m_leftclick_tool_id = InsertPlugInTool(_T(""), _img_my_plugin, _img_my_plugin, wxITEM_CHECK,
                                                 _("MY_PLUGIN"), _T(""), NULL,
                                                 MY_PLUGIN_TOOL_POSITION, 0, this);
			
		
      return (WANTS_OVERLAY_CALLBACK |
              WANTS_OPENGL_OVERLAY_CALLBACK |
              WANTS_CURSOR_LATLON       |
              WANTS_TOOLBAR_CALLBACK    |
              INSTALLS_TOOLBAR_TOOL     |
              WANTS_CONFIG              |
              WANTS_PREFERENCES         |
              WANTS_PLUGIN_MESSAGING    |
              WANTS_MOUSE_EVENTS
            );
}

bool my_plugin_pi::DeInit(void)
{
    
    return true;
}

int my_plugin_pi::GetAPIVersionMajor()
{
      return MY_API_VERSION_MAJOR;
}

int my_plugin_pi::GetAPIVersionMinor()
{
      return MY_API_VERSION_MINOR;
}

int my_plugin_pi::GetPlugInVersionMajor()
{
      return PLUGIN_VERSION_MAJOR;
}

int my_plugin_pi::GetPlugInVersionMinor()
{
      return PLUGIN_VERSION_MINOR;
}

wxBitmap *my_plugin_pi::GetPlugInBitmap()
{
      return _img_my_plugin_pi;
}

wxString my_plugin_pi::GetCommonName()
{
      return _T("MY_PLUGIN");
}


wxString my_plugin_pi::GetShortDescription()
{
      return _("MY_PLUGIN PlugIn for OpenCPN");
}


wxString my_plugin_pi::GetLongDescription()
{
      return _("MY_PLUGIN PlugIn for OpenCPN\n\
Provides basic MY_PLUGIN file overlay capabilities for several MY_PLUGIN file types\n\
and a request function to get MY_PLUGIN files by eMail.\n\n\
Supported MY_PLUGIN data include:\n\
- wind direction and speed (at 10 m)\n\
- wind gust\n\
- surface pressure\n\
- rainfall\n\
- cloud cover\n\
- significant wave height and direction\n\
- air surface temperature (at 2 m)\n\
- sea surface temperature\n\
- surface current direction and speed\n\
- Convective Available Potential Energy (CAPE)\n\
- wind, altitude, temperature and relative humidity at 300, 500, 700, 850 hPa." );
}


void my_plugin_pi::SetDefaults(void)
{
}


int my_plugin_pi::GetToolbarToolCount(void)
{
      return 1;
}

bool my_plugin_pi::MouseEventHook( wxMouseEvent &event )
{
   
    return false;
}

void my_plugin_pi::ShowPreferencesDialog( wxWindow* parent )
{
    
}

void my_plugin_pi::OnToolbarToolCallback(int id)
{
	if(isPlugInActive)
	{
		isPlugInActive = false;
		return ;
	}
	else
	{
		isPlugInActive = true;
	}


	PlugIn_Waypoint *pwaypoint = new PlugIn_Waypoint( 0.0, 0.0,
                    _T("icon_ident"), _T("wp_name"),
                     _T("GUID") );				 
	AddSingleWaypoint(  pwaypoint,  true);

	
	PlugIn_Waypoint *pwaypoint1 = new PlugIn_Waypoint( 0.1, 0.1,
                    _T("icon_ident"), _T("wp_name"),
                     _T("GUID1") );				 
	AddSingleWaypoint(  pwaypoint1,  true);

	
	PlugIn_Waypoint *pwaypoint2 = new PlugIn_Waypoint( 0.5, 0.5,
                    _T("icon_ident"), _T("wp_name"),
                     _T("GUID2") );					 
	AddSingleWaypoint(  pwaypoint2,  true);
	
	
	myMap["GUID"] = pwaypoint;
	myMap["GUID1"] = pwaypoint1;
	myMap["GUID2"] = pwaypoint2;
	
    RequestRefresh(m_parent_window); // refresh mainn window
	
}



bool my_plugin_pi::RenderOverlay(wxDC &dc, PlugIn_ViewPort *vp)
{
    m_vp = *vp;
    return true;
}

bool my_plugin_pi::RenderGLOverlay(wxGLContext *pcontext, PlugIn_ViewPort *vp)
{
    m_vp = *vp;
    return true;
}

void my_plugin_pi::SetCursorLatLon(double lat, double lon)
{

	if(!isPlugInActive)
		return ;
		
	wxPoint cur;
    GetCanvasPixLL( &m_vp, &cur, lat, lon );
	
	
	
	
	for (std::map<std::string,PlugIn_Waypoint*>::iterator it=myMap.begin(); it!=myMap.end(); ++it)
	{
		PlugIn_Waypoint *marker = it->second;
		
		 if(  PointInLLBox( &m_vp, marker->m_lat, marker->m_lon ) )
		 {
		 
			//wxMessageBox(wxString::Format(wxT("%f"),marker->m_lat) + _T(" ") +wxString::Format(wxT("%f"),marker->m_lon));
			wxPoint pl;
            GetCanvasPixLL( &m_vp, &pl, marker->m_lat, marker->m_lon );
            if (pl.x > cur.x - 10 && pl.x < cur.x + 10 && pl.y > cur.y - 10 && pl.y < cur.y + 10)
            {
                m_ActiveMarker = marker;
                
				wxMessageBox(_T("SetCursorLatLon"));
				
                break;
            }
		 }
	}
	
  
}

bool my_plugin_pi::PointInLLBox( PlugIn_ViewPort *vp, double x, double y )
{
    double Marge = 0.;
    double m_minx = vp->lon_min;
    double m_maxx = vp->lon_max;
    double m_miny = vp->lat_min;
    double m_maxy = vp->lat_max;

    //    Box is centered in East lon, crossing IDL
    if(m_maxx > 180.)
    {
        if( x < m_maxx - 360.)
            x +=  360.;

        if (  x >= (m_minx - Marge) && x <= (m_maxx + Marge) &&
            y >= (m_miny - Marge) && y <= (m_maxy + Marge) )
            return TRUE;
        return FALSE;
    }

    //    Box is centered in Wlon, crossing IDL
    else if(m_minx < -180.)
    {
        if(x > m_minx + 360.)
            x -= 360.;

        if (  x >= (m_minx - Marge) && x <= (m_maxx + Marge) &&
            y >= (m_miny - Marge) && y <= (m_maxy + Marge) )
            return TRUE;
        return FALSE;
    }

    else
    {
        if (  x >= (m_minx - Marge) && x <= (m_maxx + Marge) &&
            y >= (m_miny - Marge) && y <= (m_maxy + Marge) )
            return TRUE;
        return FALSE;
    }
}

void my_plugin_pi::OnContextMenuItemCallback(int id)
{
   
}


void my_plugin_pi::SetPluginMessage(wxString &message_id, wxString &message_body)
{
    
}


void my_plugin_pi::SendTimelineMessage(wxDateTime time)
{
   
}


void my_plugin_pi::initLoginDialog(wxWindow* parent)
{

	
}


