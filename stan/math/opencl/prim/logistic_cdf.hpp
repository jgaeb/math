#ifndef STAN_MATH_OPENCL_PRIM_LOGISTIC_CDF_HPP
#define STAN_MATH_OPENCL_PRIM_LOGISTIC_CDF_HPP
#ifdef STAN_OPENCL

#include <stan/math/prim/meta.hpp>
#include <stan/math/prim/err.hpp>
#include <stan/math/prim/fun/constants.hpp>
#include <stan/math/prim/fun/elt_divide.hpp>
#include <stan/math/prim/fun/elt_multiply.hpp>
#include <stan/math/opencl/kernel_generator.hpp>
#include <stan/math/prim/functor/operands_and_partials.hpp>

namespace stan {
namespace math {

/** \ingroup opencl
 * Returns the logistic cumulative distribution function for the given
 * location, and scale. If given containers of matching sizes
 * returns the product of probabilities.
 *
 * @tparam T_y_cl type of scalar outcome
 * @tparam T_loc_cl type of location
 * @tparam T_scale_cl type of scale
 * @param y (Sequence of) scalar(s).
 * @param mu (Sequence of) location(s).
 * @param sigma (Sequence of) scale(s).
 * @return The log of the product of densities.
 */
template <
    typename T_y_cl, typename T_loc_cl, typename T_scale_cl,
    require_all_prim_or_rev_kernel_expression_t<T_y_cl, T_loc_cl,
                                                T_scale_cl>* = nullptr,
    require_any_not_stan_scalar_t<T_y_cl, T_loc_cl, T_scale_cl>* = nullptr>
return_type_t<T_y_cl, T_loc_cl, T_scale_cl> logistic_cdf(
    const T_y_cl& y, const T_loc_cl& mu, const T_scale_cl& sigma) {
  static const char* function = "logistic_cdf(OpenCL)";
  using T_partials_return = partials_return_t<T_y_cl, T_loc_cl, T_scale_cl>;
  using std::isfinite;
  using std::isnan;

  check_consistent_sizes(function, "Random variable", y, "Location parameter",
                         mu, "Scale parameter", sigma);
  const size_t N = max_size(y, mu, sigma);
  if (N == 0) {
    return 1.0;
  }

  const auto& y_col = as_column_vector_or_scalar(y);
  const auto& mu_col = as_column_vector_or_scalar(mu);
  const auto& sigma_col = as_column_vector_or_scalar(sigma);

  const auto& y_val = value_of(y_col);
  const auto& mu_val = value_of(mu_col);
  const auto& sigma_val = value_of(sigma_col);

  auto check_y_not_nan
      = check_cl(function, "Random variable", y_val, "not NaN");
  auto y_not_nan_expr = !isnan(y_val);
  auto check_mu_finite
      = check_cl(function, "Location parameter", mu_val, "finite");
  auto mu_finite_expr = isfinite(mu_val);
  auto check_sigma_positive_finite
      = check_cl(function, "Scale parameter", sigma_val, "positive finite");
  auto sigma_positive_finite_expr = 0 < sigma_val && isfinite(sigma_val);

  auto any_y_neg_inf
      = colwise_max(constant(0, N, 1) + (y_val == NEGATIVE_INFTY));
  auto cond = y_val == INFTY;
  auto inv_sigma = elt_divide(1.0, sigma_val);
  auto mu_minus_y_div_sigma
      = elt_multiply(mu_val - y_val, inv_sigma);
  auto exp_scaled_diff = exp(mu_minus_y_div_sigma);
  auto Pn
      = elt_divide(1.0, 1.0 + exp_scaled_diff);
  auto P_expr = colwise_prod(select(cond, 1.0, Pn));

  auto y_deriv_tmp = select(cond, 0.0,
                            elt_divide(exp(mu_minus_y_div_sigma - log(sigma_val)
                                           - 2.0 * log1p(exp_scaled_diff)),
                                       Pn));
  auto sigma_deriv_tmp = elt_multiply(y_deriv_tmp, mu_minus_y_div_sigma);

  matrix_cl<double> any_y_neg_inf_cl;
  matrix_cl<double> P_cl;
  matrix_cl<double> mu_deriv_cl;
  matrix_cl<double> y_deriv_cl;
  matrix_cl<double> sigma_deriv_cl;

  results(check_y_not_nan, check_mu_finite, check_sigma_positive_finite,
          any_y_neg_inf_cl, P_cl, mu_deriv_cl, sigma_deriv_cl)
      = expressions(
          y_not_nan_expr, mu_finite_expr, sigma_positive_finite_expr,
          any_y_neg_inf, P_expr,
          calc_if<!is_constant_all<T_y_cl, T_loc_cl>::value>(y_deriv_tmp),
          calc_if<!is_constant<T_scale_cl>::value>(sigma_deriv_tmp));

  if (from_matrix_cl(any_y_neg_inf_cl).maxCoeff()) {
    return 0.0;
  }

  T_partials_return P = (from_matrix_cl(P_cl)).prod();

  auto y_deriv = mu_deriv_cl * P;
  auto mu_deriv = -y_deriv;
  auto sigma_deriv = sigma_deriv_cl * P;

  results(mu_deriv_cl, y_deriv_cl, sigma_deriv_cl)
      = expressions(calc_if<!is_constant<T_loc_cl>::value>(mu_deriv),
                    calc_if<!is_constant<T_y_cl>::value>(y_deriv),
                    calc_if<!is_constant<T_scale_cl>::value>(sigma_deriv));

  operands_and_partials<decltype(y_col), decltype(mu_col), decltype(sigma_col)>
      ops_partials(y_col, mu_col, sigma_col);

  if (!is_constant<T_y_cl>::value) {
    ops_partials.edge1_.partials_ = std::move(y_deriv_cl);
  }
  if (!is_constant<T_loc_cl>::value) {
    ops_partials.edge2_.partials_ = std::move(mu_deriv_cl);
  }
  if (!is_constant<T_scale_cl>::value) {
    ops_partials.edge3_.partials_ = std::move(sigma_deriv_cl);
  }
  return ops_partials.build(P);
}

}  // namespace math
}  // namespace stan
#endif
#endif
