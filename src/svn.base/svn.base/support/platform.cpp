#include <svn.base/support/platform.hpp>

#if WIN32
#define NOMINMAX 1
#include <Windows.h>
#endif

#include <cassert>

namespace svn::base {

double
performance_counter()
{
  bool ok;
  LARGE_INTEGER counter;
  LARGE_INTEGER frequency;
  ok = QueryPerformanceCounter(&counter);
  assert(ok);
  ok = QueryPerformanceFrequency(&frequency);
  assert(ok);
  return static_cast<double>(counter.QuadPart) / static_cast<double>(frequency.QuadPart);
}
 
} // namespace svn::base
