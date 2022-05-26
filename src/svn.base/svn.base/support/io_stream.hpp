#ifndef SVN_BASE_SUPPORT_IO_STREAM_HPP
#define SVN_BASE_SUPPORT_IO_STREAM_HPP

#include <string>
#include <cstdint>

namespace svn::base {

struct io_stream
{
  virtual bool read_float(float& val) = 0;
  virtual bool read_int32(std::int32_t& val) = 0;
  virtual bool read_wstring(std::wstring& val) = 0;

  virtual bool write_float(float val) = 0;
  virtual bool write_int32(std::int32_t val) = 0;
  virtual bool write_wstring(std::wstring const& val) = 0;

  virtual ~io_stream() = default;
  static bool load(io_stream& stream, struct runtime_topology const& topology, union param_value* state);
  static bool save(io_stream& stream, struct runtime_topology const& topology, union param_value const* state);
};

} // namespace svn::base
#endif // SVN_BASE_SUPPORT_IO_STREAM_HPP