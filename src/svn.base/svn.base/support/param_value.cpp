#include <svn.base/support/param_value.hpp>

namespace svn::base {

param_value::param_value(): discrete(0) {}
param_value::param_value(float val): real(val) {}
param_value::param_value(std::int32_t val): discrete(val) {}

} // namespace svn::base