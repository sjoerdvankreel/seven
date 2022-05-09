#ifndef SVN_VST_SEVEN_VST_HPP
#define SVN_VST_SEVEN_VST_HPP

#define SVN_VST_PLUGIN_VERSION "1.0.0.0"
#define SVN_VST_PLUGIN_CATEGORY "Instrument"
#define SVN_VST_COMPANY_WEB "Seven"
#define SVN_VST_COMPANY_NAME "Seven"
#define SVN_VST_COMPANY_MAIL "sjoerdvankreel@gmail.com"

#if NDEBUG
#define SVN_VST_PLUGIN_NAME "Seven"
#define SVN_VST_ORIGINAL_FILENAME "seven.vst3"
#define SVN_VST_CONTROLLER_NAME "SevenController"
#else
#define SVN_VST_PLUGIN_NAME "Seven (Debug)"
#define SVN_VST_ORIGINAL_FILENAME "seven.debug.vst3"
#define SVN_VST_CONTROLLER_NAME "SevenController (Debug)"
#endif

#endif // SVN_VST_SEVEN_VST_HPP
