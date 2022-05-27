

BEGIN_FACTORY_DEF(SVN_VST_COMPANY_NAME, SVN_VST_COMPANY_WEB, SVN_VST_COMPANY_MAIL)
  DEF_CLASS2(INLINE_UID_FROM_FUID(Svn::Vst::ProcessorId), PClassInfo::kManyInstances,
    kVstAudioEffectClass, SVN_VST_PLUGIN_NAME, Steinberg::Vst::kDistributable, SVN_VST_PLUGIN_CATEGORY,
    SVN_VST_PLUGIN_VERSION, kVstVersionString, Svn::Vst::Processor::createInstance)
  DEF_CLASS2(INLINE_UID_FROM_FUID(Svn::Vst::ControllerId), PClassInfo::kManyInstances,
    kVstComponentControllerClass, SVN_VST_CONTROLLER_NAME, 0, "", SVN_VST_PLUGIN_VERSION,
    kVstVersionString, Svn::Vst::Controller::createInstance)
END_FACTORY