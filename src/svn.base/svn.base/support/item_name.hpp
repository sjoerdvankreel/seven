#ifndef SVN_BASE_SUPPORT_ITEM_NAME_HPP
#define SVN_BASE_SUPPORT_ITEM_NAME_HPP

namespace svn::base {

struct item_name
{
  wchar_t const* const short_; // Short name, e.g. "Pw".
  wchar_t const* const detail; // Long name, e.g. "Pulse width".
  item_name(wchar_t const* short_, wchar_t const* detail);
};

} // namespace svn::base
#endif // SVN_BASE_SUPPORT_ITEM_NAME_HPP