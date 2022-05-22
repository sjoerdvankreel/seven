#ifndef SVN_SUPPORT_IO_STREAM_HPP
#define SVN_SUPPORT_IO_STREAM_HPP

#include <string>
#include <cstdint>

namespace svn {

struct io_stream
{
  virtual bool read_float(float& val) = 0;
  virtual bool read_int32(std::int32_t& val) = 0;
  virtual bool read_string(std::string& val) = 0;

  virtual bool write_float(float val) = 0;
  virtual bool write_int32(std::int32_t val) = 0;
  virtual bool write_string(std::string const& val) = 0;

  virtual ~io_stream() = default;
  static bool load(io_stream& stream, struct param_value* param_values);
  static bool save(io_stream& stream, struct param_value* param_values);
};

} // namespace svn
#endif // SVN_SUPPORT_IO_STREAM_HPP