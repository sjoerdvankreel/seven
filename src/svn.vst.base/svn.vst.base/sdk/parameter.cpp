#include <svn.vst.base/sdk/parameter.hpp>
#include <svn.vst.base/support/support.hpp>
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
  default: return param.discrete.max - param.discrete.min;
  }
}

static double
param_default_to_vst_normalized(param_descriptor const& param)
{
  switch (param.type)
  {
  case param_type::real: return param.real.default_;
  default: return discrete_to_vst_normalized(param, param.discrete.default_);
  }
}

static int32
param_flags(param_type type, bool output)
{
  int32 result = ParameterInfo::kCanAutomate;
  if(output) result |= ParameterInfo::kIsReadOnly;
  if(type == param_type::list || type == param_type::knob_list) result |= ParameterInfo::kIsList;
  return result;
}  

parameter::
parameter(std::int32_t index, 
  svn::base::part_info const* part, 
  svn::base::param_info const* param) :
  Parameter(
    to_vst_string(param->runtime_name.c_str()).c_str(),
    index, 
    to_vst_string(param->descriptor->unit).c_str(),
    param_default_to_vst_normalized(*param->descriptor),
    param_step_count(*param->descriptor),
    param_flags(param->descriptor->type, part->descriptor->output),
    param->part_index + 1, 
    to_vst_string(param->descriptor->static_name.short_).c_str()),
  _descriptor(param->descriptor)
{    
  assert(index >= 0);
  assert(param != nullptr);
  setPrecision(param->descriptor->real.precision);
}

void
parameter::toString(ParamValue normalized, String128 string) const
{
  param_value value;
  char str8[128] = { 0 };
  switch (_descriptor->type)
  {
  case param_type::real: value.real = toPlain(normalized); break;
  default: value.discrete = vst_normalized_to_discrete(*_descriptor, normalized); break;
  }
  _descriptor->format(value, str8, 128);
  for(std::size_t i = 0; i < 128; i++)  
    string[i] = str8[i];
}
 
bool
parameter::fromString(TChar const* string, ParamValue& normalized) const
{
  param_value value;
  std::vector<char> str8;
  while(*string != static_cast<TChar>(0))
    str8.push_back(static_cast<char>(*string++));
  str8.push_back('\0');
  if (!_descriptor->parse(str8.data(), value)) return false;
  switch (_descriptor->type)
  {
  case param_type::real: normalized = toNormalized(value.real); break;
  default: normalized = discrete_to_vst_normalized(*_descriptor, value.discrete); break;
  }
  return true;
}

ParamValue
parameter::toPlain(ParamValue normalized) const
{
  switch (_descriptor->type)
  {
  case param_type::real: return _descriptor->real.display.to_range(normalized);
  default: return vst_normalized_to_discrete(*_descriptor, normalized);
  }
}

ParamValue
parameter::toNormalized(ParamValue plain) const
{
  switch (_descriptor->type)
  {
  case param_type::real: return _descriptor->real.display.from_range(plain);
  default: return discrete_to_vst_normalized(*_descriptor, static_cast<std::int32_t>(plain));
  }
}

} // namespace svn::vst::base