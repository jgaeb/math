#ifndef STAN_MATH_REV_CORE_START_NESTED_HPP
#define STAN_MATH_REV_CORE_START_NESTED_HPP

#include <stan/math/rev/core/chainablestack.hpp>

namespace stan {
namespace math {

/**
 * Record the current position so that <code>recover_memory_nested()</code>
 * can find it.
 */
static inline void start_nested() {
  ChainableStack::instance_->nested_var_stack_sizes_.push_back(
      ChainableStack::instance_->var_stack_.size());
  ChainableStack::instance_->nested_var_nochain_stack_sizes_.push_back(
      ChainableStack::instance_->var_nochain_stack_.size());
  ChainableStack::instance_->nested_var_alloc_stack_starts_.push_back(
      ChainableStack::instance_->var_alloc_stack_.size());
  stack_mem.start_nested();
}

}  // namespace math
}  // namespace stan
#endif
