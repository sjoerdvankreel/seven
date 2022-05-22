#include <svn/vst/support/io_stream.hpp>

namespace Svn::Vst {

bool 
IOStream::read_float(float& val)
{
  return _stream->read
}

bool
IOStream::read_int32(std::int32_t& val);
bool
IOStream::read_wstring(std::wstring& val);

bool
IOStream::write_float(float val);
bool
IOStream::write_int32(std::int32_t val);
bool
IOStream::write_wstring(std::wstring const& val);

} // namespace Svn::Vst