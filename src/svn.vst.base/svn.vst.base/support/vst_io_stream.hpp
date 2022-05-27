#ifndef SVN_VST_BASE_SUPPORT_VST_IO_STREAM_HPP
#define SVN_VST_BASE_SUPPORT_VST_IO_STREAM_HPP

#include <base/source/fstreamer.h>
#include <svn.base/support/io_stream.hpp>

#include <string>
#include <cstdint>

namespace svn::vst::base {

struct vst_io_stream:
public svn::base::io_stream
{
private:
  Steinberg::IBStreamer* const _streamer;
public:
  vst_io_stream(Steinberg::IBStreamer* streamer);

  bool read_float(float& val) override;
  bool read_int32(std::int32_t& val) override;
  bool read_wstring(std::wstring& val) override;

  bool write_float(float val) override;
  bool write_int32(std::int32_t val) override;
  bool write_wstring(std::wstring const& val) override;
};

} // namespace svn::vst::base
#endif // SVN_VST_BASE_SUPPORT_VST_IO_STREAM_HPP