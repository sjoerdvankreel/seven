#ifndef SVN_VST_SYNTH_SYNTH_HPP
#define SVN_VST_SYNTH_SYNTH_HPP

#define SVN_VST_SYNTH_VERSION "1.0.0.0"
#define SVN_VST_SYNTH_CATEGORY "Instrument"
#define SVN_VST_SYNTH_COMPANY_WEB "Seven"
#define SVN_VST_SYNTH_COMPANY_NAME "Seven"
#define SVN_VST_SYNTH_COMPANY_MAIL "sjoerdvankreel@gmail.com"

#if NDEBUG
#define SVN_VST_SYNTH_NAME "Synth"
#define SVN_VST_SYNTH_CONTROLLER_NAME "SynthController"
#define SVN_VST_SYNTH_ORIGINAL_FILENAME "seven.vst.synth.vst3"
#else
#define SVN_VST_SYNTH_NAME "SynthDebug"
#define SVN_VST_SYNTH_CONTROLLER_NAME "SynthDebugController"
#define SVN_VST_SYNTH_ORIGINAL_FILENAME "seven.vst.synth.debug.vst3"
#endif

#endif // SVN_VST_SYNTH_SYNTH_HPP
