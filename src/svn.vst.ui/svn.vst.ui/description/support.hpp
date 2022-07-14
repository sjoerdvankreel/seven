#ifndef SVN_VST_UI_DESCRIPTION_SUPPORT_HPP
#define SVN_VST_UI_DESCRIPTION_SUPPORT_HPP

#include <svn.base/topology/topology_info.hpp>
#include <cstdint>
#include <cstdlib>

namespace svn::vst::ui {

inline svn::base::ui_color constexpr white = { 0xFF, 0xFF, 0xFF };
inline svn::base::ui_color constexpr black = { 0x00, 0x00, 0x00 };

struct color_alpha_t { enum value { transparent, eight, quarter, half, opaque, count }; };
typedef color_alpha_t::value color_alpha;
extern std::uint8_t const color_alphas[];

inline std::int32_t constexpr param_row_height = 19;
inline std::int32_t constexpr param_col1_width = 20;
inline std::int32_t constexpr param_col2_width = 26;
inline std::int32_t constexpr param_col3_width = 28;
inline std::int32_t constexpr header_checkbox_width = 14;

inline std::int32_t constexpr margin = 3;
inline std::int32_t constexpr padding_param_group = 1;
inline std::int32_t constexpr param_total_width = param_col1_width + margin + param_col2_width + margin + param_col3_width;
inline std::int32_t constexpr param_output_col_width = (param_total_width - margin) / 2;
inline std::uint8_t const color_alphas[color_alpha::count] = { 0x00, 0x20, 0x40, 0x80, 0xFF };

} // namespace svn::vst::ui
#endif // SVN_VST_UI_DESCRIPTION_SUPPORT_HPP