#ifndef SVN_BASE_SUPPORT_NOTE_NAME_HPP
#define SVN_BASE_SUPPORT_NOTE_NAMES_HPP

#include <svn.base/support/item_name.hpp>

namespace svn::base {

enum note_name {
  c, c_sharp, d, d_sharp, e, f, f_sharp, g, g_sharp, a, a_sharp, b, count
};

extern item_name const note_names[];

} // namespace svn::base
#endif // SVN_BASE_SUPPORT_NOTE_NAME_HPP