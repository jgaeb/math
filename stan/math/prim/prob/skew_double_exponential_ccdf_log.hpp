#ifndef STAN_MATH_PRIM_PROB_SKEW_DOUBLE_EXPONENTIAL_CCDF_LOG_HPP
#define STAN_MATH_PRIM_PROB_SKEW_DOUBLE_EXPONENTIAL_CCDF_LOG_HPP

#include <stan/math/prim/meta.hpp>
#include <stan/math/prim/prob/skew_double_exponential_lccdf.hpp>

namespace stan {
namespace math {

/** \ingroup prob_dists
 * @deprecated use <code>skew_double_exponential_lccdf</code>
 */
template <typename T_y, typename T_loc, typename T_scale, typename T_skewness>
inline return_type_t<T_y, T_loc, T_scale, T_skewness>
skew_double_exponential_ccdf_log(const T_y& y, const T_loc& mu,
                                 const T_scale& sigma, const T_skewness& tau) {
  return skew_double_exponential_lccdf<T_y, T_loc, T_scale, T_skewness>(
      y, mu, sigma, tau);
}

}  // namespace math
}  // namespace stan
#endif
