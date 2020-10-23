#ifndef STAN_MATH_REV_FUN_VARIANCE_HPP
#define STAN_MATH_REV_FUN_VARIANCE_HPP

#include <stan/math/rev/meta.hpp>
#include <stan/math/rev/core.hpp>
#include <stan/math/rev/fun/typedefs.hpp>
#include <stan/math/prim/err.hpp>
#include <stan/math/prim/fun/Eigen.hpp>
#include <stan/math/prim/fun/typedefs.hpp>
#include <vector>

namespace stan {
namespace math {
namespace internal {

inline var calc_variance(size_t size, const var* dtrs) {
  vari** varis = ChainableStack::instance_->memalloc_.alloc_array<vari*>(size);
  double* partials
      = ChainableStack::instance_->memalloc_.alloc_array<double>(size);

  Eigen::Map<const vector_v, StackAlignment> dtrs_map(dtrs, size);
  Eigen::Map<vector_vi, StackAlignment>(varis, size) = dtrs_map.vi();
  vector_d dtrs_vals = dtrs_map.val();

  vector_d diff = dtrs_vals.array() - dtrs_vals.mean();
  double size_m1 = size - 1;
  Eigen::Map<vector_d, StackAlignment>(partials, size) = 2 * diff.array() / size_m1;
  double variance = diff.squaredNorm() / size_m1;

  return {new stored_gradient_vari(variance, size, varis, partials)};
}

}  // namespace internal

/**
 * Return the sample variance of the specified standard
 * vector.  Raise domain error if size is not greater than zero.
 *
 * @param[in] v a vector
 * @return sample variance of specified vector
 */
inline var variance(const std::vector<var>& v) {
  check_nonzero_size("variance", "v", v);
  if (v.size() == 1) {
    return var{0.0};
  }
  return {internal::calc_variance(v.size(), &v[0])};
}

/**
 * Return the sample variance of the specified vector, row vector,
 * or matrix.  Raise domain error if size is not greater than
 * zero.
 *
 * @tparam EigMat type inheriting from `EigenBase` that has a `var`
 *  scalar type.
 * @param[in] m input matrix
 * @return sample variance of specified matrix
 */
template <typename EigMat, require_eigen_vt<is_var, EigMat>* = nullptr>
var variance(const EigMat& m) {
  const auto& mat = to_ref(m);
  check_nonzero_size("variance", "m", mat);
  if (mat.size() == 1) {
    return var{0.0};
  }
  return {internal::calc_variance(mat.size(), mat.data())};
}

}  // namespace math
}  // namespace stan
#endif
