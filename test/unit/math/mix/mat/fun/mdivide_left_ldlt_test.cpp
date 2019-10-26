#include <test/unit/math/test_ad.hpp>

TEST(MathMixMatFun, mdivideLeftLdlt) {
  auto f = [](const auto& x, const auto& y) {
    return stan::math::mdivide_left_ldlt(stan::test::ldlt_factor(x), y);
  };

  Eigen::MatrixXd a(1, 1);
  a << 2;
  Eigen::MatrixXd b(1, 1);
  b << 3;
  stan::test::expect_ad(f, a, b);

  Eigen::VectorXd g(1);
  g << 3;
  stan::test::expect_ad(f, a, g);

  Eigen::MatrixXd c(2, 2);
  c << 2, 3, 3, 7;
  Eigen::MatrixXd d(2, 2);
  d << 12, 13, 15, 17;
  stan::test::expect_ad(f, c, c);
  stan::test::expect_ad(f, c, d);

  Eigen::VectorXd e(2);
  e << 2, 3;
  stan::test::expect_ad(f, c, e);

  // ill-formed matrix inputs compile then throw at runtime
  Eigen::MatrixXd m33(3, 3);
  m33 << 1, 2, 3, 4, 5, 6, 7, 8, 9;
  Eigen::MatrixXd m44(4, 4);
  m44 << 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16;

  Eigen::VectorXd v3(3);
  v3 << 1, 2, 3;
  Eigen::VectorXd v4(4);
  v4 << 1, 2, 3, 4;

  Eigen::RowVectorXd rv3(3);
  rv3 << 1, 2, 3;
  Eigen::RowVectorXd rv4(4);
  rv4 << 1, 2, 3, 4;

  // ill-formed inputs
  stan::test::expect_ad(f, m33, m44);
  stan::test::expect_ad(f, m33, rv3);
  stan::test::expect_ad(f, m33, rv4);
  stan::test::expect_ad(f, m33, v4);
  stan::test::expect_ad(f, m44, rv3);
  stan::test::expect_ad(f, m44, rv4);
  stan::test::expect_ad(f, m44, v3);
}
