#include <svn.base/support/platform.hpp>

#if WIN32
#define NOMINMAX 1
#include <Windows.h>
#else
#include <time.h>
#endif

#include <cassert>

namespace svn::base {

double
performance_counter()
{
#if WIN32
  bool ok;
  LARGE_INTEGER counter;
  LARGE_INTEGER frequency;
  ok = QueryPerformanceCounter(&counter);
  assert(ok);
  ok = QueryPerformanceFrequency(&frequency);
  assert(ok);
  return static_cast<double>(counter.QuadPart) / static_cast<double>(frequency.QuadPart);
#else
  struct timespec tv;
  clock_gettime(CLOCK_REALTIME, &tv);
  return static_cast<double>(tv.tv_sec) + tv.tv_nsec / 1000000000.0;
#endif
}
 
} // namespace svn::base
