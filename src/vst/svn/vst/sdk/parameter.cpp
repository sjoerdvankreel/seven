#include <svn/vst/sdk/parameter.hpp>
#include <svn/support/param_transform.hpp>
#include <cassert>

using namespace Steinberg;
using namespace Steinberg::Vst;

namespace Svn::Vst {

static double 
stepCount(svn::param_info const& info)
{
  switch (info.type)
  {
  case svn::param_type::real: return 0.0;
  default: return info.max.discrete - info.min.discrete;
  }
}

static double
normalizeDefault(svn::param_info const& info)
{
  switch (info.type)
  {
  case svn::param_type::real: return info.default_.real;
  default: return Parameter::fromDiscrete(info, info.default_.discrete);
  }
}

int32
paramFlags(std::int32_t type)
{
  switch (type)
  {
  case svn::param_type::list: return ParameterInfo::kCanAutomate | ParameterInfo::kIsList;
  default: return ParameterInfo::kCanAutomate;
  }
}

Parameter::
Parameter(std::int32_t index, svn::synth_param const* param):
Steinberg::Vst::Parameter(
  param->detail.c_str(),
  index,
  param->info->unit, 
  normalizeDefault(*param->info), 
  stepCount(*param->info), 
  paramFlags(param->info->type), 
  param->part_index + 1, 
  param->info->item.name),
_param(param) {}

ParamValue 
Parameter::toPlain(ParamValue normalized) const
{
  svn::param_value value;
  switch (_param->info->type)
  {
  case svn::param_type::real: 
    value.real = normalized;
    return svn::param_to_display(*_param->info, value).real;
  default:
    value.discrete = toDiscrete(*_param->info, normalized);
    return svn::param_to_display(*_param->info, value).discrete;
  }
}

ParamValue 
Parameter::toNormalized(ParamValue plain) const
{
  svn::param_value value;
  switch (_param->info->type)
  {
  case svn::param_type::real:
    value.real = plain;
    return svn::param_from_display(*_param->info, value).real;
  default:
    value.discrete = static_cast<std::int32_t>(plain);
    return svn::param_from_display(*_param->info, value).discrete;
  }
}

bool
Parameter::fromString(TChar const* string, ParamValue& normalized) const
{
  svn::param_value value;
  if (!svn::param_parse(*_param->info, string, value)) return false;
  normalized = toNormalized(value.real);
  return true;
}

void 
Parameter::toString(ParamValue normalized, String128 string) const
{
  svn::param_value value;
  switch (_param->info->type)
  {
  case svn::param_type::real: value.real = toPlain(normalized); break;
  default: value.discrete = toDiscrete(*_param->info, normalized); break;
  }
  svn::param_format(*_param->info, value, string, 128);
}

} // namespace Svn::Vst