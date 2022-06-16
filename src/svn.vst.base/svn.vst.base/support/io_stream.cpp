#include <svn.vst.base/support/io_stream.hpp>

namespace svn::vst::base {

bool 
vst_io_stream::read_int32(std::int32_t& val)
{ 
  Steinberg::int32 sval;
  if(!_streamer->readInt32(sval)) return false;
  val = sval;
  return true;
}

bool
vst_io_stream::read_wstring(std::wstring& val)
{  
  wchar_t chr;
  Steinberg::int32 size;  
  val.clear();
  if(!_streamer->readInt32(size)) return false;
  for (std::int32_t i = 0; i < size; i++)
  {
    if(!_streamer->readChar16(chr)) return false;
    val.append(1, chr);
  }
  return true;
}

bool
vst_io_stream::write_wstring(std::wstring const& val)
{
  auto size = static_cast<std::int32_t>(val.size());
  if(!_streamer->writeInt32(size)) return false;
  for(std::int32_t i = 0; i < size; i++)
    if(!_streamer->writeChar16(val[i])) return false;
  return true;
}

} // namespace svn::vst::base