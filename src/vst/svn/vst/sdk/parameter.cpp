#include <svn/vst/sdk/parameter.hpp>

using namespace Steinberg;
using namespace Steinberg::Vst;

namespace Svn::Vst {

ParamValue 
SvnParameter::toPlain(ParamValue valueNormalized) const
{
  return Parameter::toPlain(valueNormalized);
}

ParamValue 
SvnParameter::toNormalized(ParamValue plainValue) const
{
  return Parameter::toPlain(valueNormalized);
}

void 
SvnParameter::toString(ParamValue valueNormalized, String128 string) const
{
  return Parameter::toPlain(valueNormalized);
}

bool 
SvnParameter::fromString(const TChar* string, ParamValue& valueNormalized) const
{
  return Parameter::fromString(string, valueNormalized);
}

} // namespace Svn::Vst