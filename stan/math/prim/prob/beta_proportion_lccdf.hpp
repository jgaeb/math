#ifndef STAN_MATH_PRIM_PROB_BETA_PROPORTION_LCCDF_HPP
#define STAN_MATH_PRIM_PROB_BETA_PROPORTION_LCCDF_HPP

#include <stan/math/prim/meta.hpp>
#include <stan/math/prim/err.hpp>
#include <stan/math/prim/fun/digamma.hpp>
#include <stan/math/prim/fun/beta.hpp>
#include <stan/math/prim/fun/grad_reg_inc_beta.hpp>
#include <stan/math/prim/fun/inc_beta.hpp>
#include <stan/math/prim/fun/inv.hpp>
#include <stan/math/prim/fun/log.hpp>
#include <stan/math/prim/fun/max_size.hpp>
#include <stan/math/prim/fun/size.hpp>
#include <stan/math/prim/fun/size_zero.hpp>
#include <stan/math/prim/fun/value_of.hpp>
#include <cmath>

namespace stan {
namespace math {

/** \ingroup prob_dists
 * Returns the beta log complementary cumulative distribution function
 * for specified probability, location, and precision parameters:
 * beta_proportion_lccdf(y | mu, kappa) = beta_lccdf(y | mu * kappa, (1 -
 * mu) * kappa).  Any arguments other than scalars must be containers of
 * the same size.  With non-scalar arguments, the return is the sum of
 * the log ccdfs with scalars broadcast as necessary.
 *
 * @tparam T_y type of y
 * @tparam T_loc type of location parameter
 * @tparam T_prec type of precision parameter
 * @param y (Sequence of) scalar(s) between zero and one
 * @param mu (Sequence of) location parameter(s)
 * @param kappa (Sequence of) precision parameter(s)
 * @return log probability or sum of log of probabilities
 * @throw std::domain_error if mu is outside (0, 1)
 * @throw std::domain_error if kappa is nonpositive
 * @throw std::domain_error if 1 - y is not a valid probability
 * @throw std::invalid_argument if container sizes mismatch
 */
template <typename T_y, typename T_loc, typename T_prec>
return_type_t<T_y, T_loc, T_prec> beta_proportion_lccdf(const T_y& y,
                                                        const T_loc& mu,
                                                        const T_prec& kappa) {
  using T_partials_return = partials_return_t<T_y, T_loc, T_prec>;
  static const char* function = "beta_proportion_lccdf";
  check_positive(function, "Location parameter", mu);
  check_less(function, "Location parameter", mu, 1.0);
  check_positive_finite(function, "Precision parameter", kappa);
  check_not_nan(function, "Random variable", y);
  check_nonnegative(function, "Random variable", y);
  check_less_or_equal(function, "Random variable", y, 1.0);
  check_consistent_sizes(function, "Random variable", y, "Location parameter",
                         mu, "Precision parameter", kappa);

  if (size_zero(y, mu, kappa)) {
    return 0;
  }

  using std::exp;
  using std::log;
  using std::pow;
  T_partials_return ccdf_log(0.0);
  operands_and_partials<T_y, T_loc, T_prec> ops_partials(y, mu, kappa);

  scalar_seq_view<T_y> y_vec(y);
  scalar_seq_view<T_loc> mu_vec(mu);
  scalar_seq_view<T_prec> kappa_vec(kappa);
  size_t size_kappa = stan::math::size(kappa);
  size_t size_mu_kappa = max_size(mu, kappa);
  size_t N = max_size(y, mu, kappa);

  VectorBuilder<!is_constant_all<T_loc, T_prec>::value, T_partials_return,
                T_loc, T_prec>
      digamma_mukappa(size_mu_kappa);
  VectorBuilder<!is_constant_all<T_loc, T_prec>::value, T_partials_return,
                T_loc, T_prec>
      digamma_kappa_mukappa(size_mu_kappa);
  VectorBuilder<!is_constant_all<T_loc, T_prec>::value, T_partials_return,
                T_prec>
      digamma_kappa(size_kappa);

  if (!is_constant_all<T_loc, T_prec>::value) {
    for (size_t i = 0; i < size_mu_kappa; i++) {
      const T_partials_return kappa_dbl = value_of(kappa_vec[i]);
      const T_partials_return mukappa_dbl = value_of(mu_vec[i]) * kappa_dbl;
      digamma_mukappa[i] = digamma(mukappa_dbl);
      digamma_kappa_mukappa[i] = digamma(kappa_dbl - mukappa_dbl);
    }

    for (size_t i = 0; i < size_kappa; i++) {
      digamma_kappa[i] = digamma(value_of(kappa_vec[i]));
    }
  }

  for (size_t n = 0; n < N; n++) {
    const T_partials_return y_dbl = value_of(y_vec[n]);
    const T_partials_return mu_dbl = value_of(mu_vec[n]);
    const T_partials_return kappa_dbl = value_of(kappa_vec[n]);
    const T_partials_return mukappa_dbl = mu_dbl * kappa_dbl;
    const T_partials_return kappa_mukappa_dbl = kappa_dbl - mukappa_dbl;
    const T_partials_return betafunc_dbl = beta(mukappa_dbl, kappa_mukappa_dbl);
    const T_partials_return Pn
        = 1 - inc_beta(mukappa_dbl, kappa_mukappa_dbl, y_dbl);

    ccdf_log += log(Pn);

    const T_partials_return inv_Pn
        = is_constant_all<T_y, T_loc, T_prec>::value ? 0 : inv(Pn);

    if (!is_constant_all<T_y>::value) {
      ops_partials.edge1_.partials_[n] -= pow(1 - y_dbl, kappa_mukappa_dbl - 1)
                                          * pow(y_dbl, mukappa_dbl - 1) * inv_Pn
                                          / betafunc_dbl;
    }

    T_partials_return g1 = 0;
    T_partials_return g2 = 0;

    if (!is_constant_all<T_loc, T_prec>::value) {
      grad_reg_inc_beta(g1, g2, mukappa_dbl, kappa_mukappa_dbl, y_dbl,
                        digamma_mukappa[n], digamma_kappa_mukappa[n],
                        digamma_kappa[n], betafunc_dbl);
    }
    if (!is_constant_all<T_loc>::value) {
      ops_partials.edge2_.partials_[n] -= kappa_dbl * (g1 - g2) * inv_Pn;
    }
    if (!is_constant_all<T_prec>::value) {
      ops_partials.edge3_.partials_[n]
          -= (g1 * mu_dbl + g2 * (1 - mu_dbl)) * inv_Pn;
    }
  }
  return ops_partials.build(ccdf_log);
}

}  // namespace math
}  // namespace stan
#endif