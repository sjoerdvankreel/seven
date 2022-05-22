#ifndef SVN_VST_SUPPORT_IO_STREAM_HPP
#define SVN_VST_SUPPORT_IO_STREAM_HPP

#include <base/source/fstreamer.h>
#include <svn/support/io_stream.hpp>

#include <string>
#include <cstdint>

namespace Svn::Vst {

struct IOStream:
public svn::io_stream
{
private:
  using IBStreamer = Steinberg::IBStreamer;
  IBStreamer* const _streamer;

public:
  IOStream(IBStreamer* streamer);

  bool read_float(float& val) override;
  bool read_int32(std::int32_t& val) override;
  bool read_wstring(std::wstring& val) override;

  bool write_float(float val) override;
  bool write_int32(std::int32_t val) override;
  bool write_wstring(std::wstring const& val) override;
};

inline bool 
IOStream::read_float(float& val)
{ return _streamer->readFloat(val); }

inline bool 
IOStream::write_float(float val)
{ return _streamer->writeFloat(val); }

inline bool 
IOStream::write_int32(std::int32_t val)
{ return _streamer->writeInt32(val); }

} // namespace Svn::Vst
#endif // SVN_VST_SUPPORT_IO_STREAM_HPP