#ifndef SVN_SUPPORT_IO_STREAM_HPP
#define SVN_SUPPORT_IO_STREAM_HPP

#include <string>
#include <cstdint>

namespace svn {

struct io_stream
{
  virtual bool read_double(double& val) = 0;
  virtual bool read_int32(std::int32_t& val) = 0;
  virtual bool read_wstring(std::wstring& val) = 0;

  virtual bool write_double(double val) = 0;
  virtual bool write_int32(std::int32_t val) = 0;
  virtual bool write_wstring(std::wstring const& val) = 0;

  virtual ~io_stream() = default;
  static bool load(io_stream& stream, struct param_value* param_values);
  static bool save(io_stream& stream, struct param_value* param_values);
};

} // namespace svn
#endif // SVN_SUPPORT_IO_STREAM_HPP