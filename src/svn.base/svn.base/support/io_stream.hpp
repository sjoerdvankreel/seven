#ifndef SVN_BASE_SUPPORT_IO_STREAM_HPP
#define SVN_BASE_SUPPORT_IO_STREAM_HPP

#include <svn.base/topology/topology_info.hpp>
#include <string>
#include <cstdint>

namespace svn::base {

class io_stream
{
public:
  virtual bool read_float(float& val) = 0;
  virtual bool read_int32(std::int32_t& val) = 0;
  virtual bool read_wstring(std::wstring& val) = 0;

  virtual bool write_float(float val) = 0;
  virtual bool write_int32(std::int32_t val) = 0;
  virtual bool write_wstring(std::wstring const& val) = 0;

  virtual ~io_stream() = default;
  bool load(topology_info const& topology, union param_value* state);
  bool save(topology_info const& topology, union param_value const* state);
};

} // namespace svn::base
#endif // SVN_BASE_SUPPORT_IO_STREAM_HPP