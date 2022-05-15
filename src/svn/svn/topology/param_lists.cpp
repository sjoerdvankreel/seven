#include <svn/topology/param_lists.hpp>

namespace svn {

param_item const 
oscillator_types[oscillator_type::count] =
{
  { L"Blep", L"Bandlimited step" },
  { L"Blamp", L"Bandlimited ramp" },
  { L"Dsf", L"Discrete summation formulae" }
};

param_item const
filter_types[filter_type::count] =
{
  { L"Ladder", L"Ladder" },
  { L"StVar", L"State variable" }
};

} // namespace svn