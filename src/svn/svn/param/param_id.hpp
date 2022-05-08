#ifndef SVN_PARAM_PARAM_ID_HPP
#define SVN_PARAM_PARAM_ID_HPP

namespace svn {

struct param_id_t
{
  enum value
  {
    unit1_gain,
    unit1_panning,
    unit2_gain,
    unit2_panning,
    count
  };
};

typedef param_id_t::value param_id;

} // namespace svn
#endif // SVN_PARAM_PARAM_ID_HPP