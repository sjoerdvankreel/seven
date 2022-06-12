#ifndef SVN_VST_UI_GENERATOR_DESCRIPTION_UI_DESCRIPTION
#define SVN_VST_UI_GENERATOR_DESCRIPTION_UI_DESCRIPTION

#include <cstdint>
#include <cstdlib>

namespace svn::vst::ui_generator {

struct rgb { std::uint8_t r, g, b; };
inline rgb constexpr white = { 0xFF, 0xFF, 0xFF };
inline rgb constexprblack = { 0x00, 0x00, 0x00 };
inline std::size_t constexpr color_count = 12;
extern rgb const color_values[];

struct color_alpha_t { enum value { transparent, eight, quarter, half, opaque, count }; };
typedef color_alpha_t::value color_alpha;
extern std::uint8_t const color_alphas[];

inline std::int32_t constexpr param_row_height = 20;
inline std::int32_t constexpr param_col1_width = 20;
inline std::int32_t constexpr param_col2_width = 26;
inline std::int32_t constexpr param_col3_width = 28;

inline std::int32_t constexpr margin = 3;
inline std::int32_t constexpr padding_param_group = 1;
inline std::int32_t constexpr param_total_width = param_col1_width + margin + param_col2_width + margin + param_col3_width;
inline std::int32_t constexpr param_output_col_width = (param_total_width - margin) / 2;

} // namespace svn::vst::ui_generator
#endif // SVN_VST_UI_GENERATOR_DESCRIPTION_UI_DESCRIPTION