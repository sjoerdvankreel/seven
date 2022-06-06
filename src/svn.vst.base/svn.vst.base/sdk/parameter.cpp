#include <svn.vst.base/sdk/parameter.hpp>
#include <svn.base/support/param_value.hpp>
#include <cassert>

using namespace svn::base;
using namespace Steinberg;
using namespace Steinberg::Vst;

namespace svn::vst::base {

static double 
param_step_count(param_descriptor const& param)
{
  switch (param.type)
  {
  case param_type::real: return 0.0;
  default: return param.max.discrete - param.min.discrete;
  }
}

static double
param_default_to_vst_normalized(param_descriptor const& param)
{
  switch (param.type)
  {
  case param_type::real: return param.default_.real;
  default: return parameter::discrete_to_vst_normalized(param, param.default_.discrete);
  }
}

static int32
param_flags(param_type type, bool output)
{
  int32 result = ParameterInfo::kCanAutomate;
  if(output) result |= ParameterInfo::kIsReadOnly;
  if(type == param_type::list) result |= ParameterInfo::kIsList;
  return result;
}

// For output params.
parameter::
parameter(std::int32_t index, svn::base::param_descriptor const* descriptor):
Parameter(
  descriptor->static_name.detail, index, descriptor->unit,
  param_default_to_vst_normalized(*descriptor),
  param_step_count(*descriptor),
  param_flags(descriptor->type, true),
  0, 
  descriptor->static_name.short_),
  _descriptor(descriptor)
{    
  assert(index >= 0);
  assert(descriptor != nullptr);
}

// For input params.
parameter::
parameter(std::int32_t index, svn::base::runtime_param const* param) :
  Parameter(
    param->runtime_name.c_str(), index, param->descriptor->unit,
    param_default_to_vst_normalized(*param->descriptor),
    param_step_count(*param->descriptor),
    param_flags(param->descriptor->type, false),
    param->part_index + 1,
    param->descriptor->static_name.short_),
  _descriptor(param->descriptor)
{
  assert(index >= 0);
  assert(param != nullptr);
}

ParamValue
parameter::toPlain(ParamValue normalized) const
{
  param_value value;
  switch (_descriptor->type)
  {
  case param_type::real: 
    value.real = normalized;
    return _descriptor->to_display(value).real;
  default:
    value.discrete = vst_normalized_to_discrete(*_descriptor, normalized);
    return _descriptor->to_display(value).discrete;
  }
}

ParamValue 
parameter::toNormalized(ParamValue plain) const
{
  param_value value;
  switch (_descriptor->type)
  {
  case param_type::real:
    value.real = plain;
    return _descriptor->from_display(value).real;
  default:
    value.discrete = static_cast<std::int32_t>(plain);
    return _descriptor->from_display(value).discrete;
  }
}

void
parameter::toString(ParamValue normalized, String128 string) const
{
  param_value value;
  switch (_descriptor->type)
  {
  case param_type::real: value.real = toPlain(normalized); break;
  default: value.discrete = vst_normalized_to_discrete(*_descriptor, normalized); break;
  }
  _descriptor->format(value, string, 128);
}

bool
parameter::fromString(TChar const* string, ParamValue& normalized) const
{
  param_value value;
  if (!_descriptor->parse(string, value)) return false;
  switch (_descriptor->type)
  {
  case param_type::real: normalized = toNormalized(value.real); break;
  default: normalized = discrete_to_vst_normalized(*_descriptor, value.discrete); break;
  }
  return true;
}

} // namespace svn::vst::base