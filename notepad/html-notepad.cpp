
#include "sciter-x-window.hpp"

#include <functional>
#include "version.h"

#if defined(WINDOWS)
  #include <VersionHelpers.h>
  #define FLAGS (SW_TITLEBAR | SW_RESIZEABLE | SW_CONTROLS | SW_MAIN | SW_GLASSY)
#elif defined(OSX)
  #define FLAGS (SW_TITLEBAR | SW_RESIZEABLE | SW_CONTROLS | SW_MAIN | SW_GLASSY)
#else
  #define FLAGS (SW_TITLEBAR | SW_RESIZEABLE | SW_CONTROLS | SW_MAIN)
#endif

class frame: public sciter::window
{
public:

  frame(bool needs_debugger) : window( FLAGS | (needs_debugger ? SW_ENABLE_DEBUG:0))
  {
  }

  BEGIN_FUNCTION_MAP
    FUNCTION_0("argv", get_argv);
    FUNCTION_0("version", get_version);
  END_FUNCTION_MAP

  sciter::value  get_argv() {
    std::vector<sciter::value> vargs;
    for (auto& arg : sciter::application::argv())
      vargs.push_back(sciter::value(arg));
    return sciter::value::make_array(UINT(vargs.size()), &vargs[0]);
  }

  sciter::value  get_version() {
    return sciter::value(WSTR(APP_VERSION));
  }

};

#include "resources.cpp"

int uimain(std::function<int()> run ) {

  SciterSetOption(NULL, SCITER_SET_SCRIPT_RUNTIME_FEATURES,
    ALLOW_FILE_IO |
    ALLOW_SOCKET_IO |
    ALLOW_EVAL |
    ALLOW_SYSINFO);

#ifdef WINDOWS
  SciterSetOption(NULL, SCITER_SET_GFX_LAYER, GFX_LAYER_D2D);
#endif // WINDOWS

  sciter::archive::instance().open(aux::elements_of(resources)); // bind resources[] (defined in "resources.cpp") with the archive

  auto args = sciter::application::argv();
  bool needs_debugger = std::find(args.cbegin(), args.cend(), WSTR("-debug")) != args.cend();

  aux::asset_ptr<frame> pwin = new frame(needs_debugger);

  // note: this:://app URL is dedicated to the sciter::archive content associated with the application

#if defined(WINDOWS)
  if(IsWindows7SP1OrGreater())
    pwin->load( WSTR("this://app/frame-windows.htm") );
  else
    pwin->load(WSTR("this://app/frame-else.htm"));
#elif defined(OSX)
  pwin->load( WSTR("this://app/frame-mac.htm") );
#else
  pwin->load( WSTR("this://app/frame-else.htm") );
#endif
  pwin->expand();

  return run();

}
