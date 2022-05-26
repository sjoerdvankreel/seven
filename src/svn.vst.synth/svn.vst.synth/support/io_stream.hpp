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

  bool read_double(double& val) override;
  bool read_int32(std::int32_t& val) override;
  bool read_wstring(std::wstring& val) override;

  bool write_double(double val) override;
  bool write_int32(std::int32_t val) override;
  bool write_wstring(std::wstring const& val) override;
};

inline IOStream::
IOStream(IBStreamer* streamer):
_streamer(streamer) {}

inline bool 
IOStream::read_double(double& val)
{ return _streamer->readDouble(val); }

inline bool 
IOStream::write_double(double val)
{ return _streamer->writeDouble(val); }

inline bool 
IOStream::write_int32(std::int32_t val)
{ return _streamer->writeInt32(val); }

} // namespace Svn::Vst
#endif // SVN_VST_SUPPORT_IO_STREAM_HPP