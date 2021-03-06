#pragma once

#include <ceres/jet.h>

#include <cmath>

#define Cos cos
#define Sin sin
#define Power pow

namespace reference_line {
struct CurveSegment {
  double theta0_;
  double kappa0_;
  double dkappa0_;
  double x0_;
  double y0_;
  double theta1_;
  double kappa1_;
  double dkappa1_;
  double x1_;
  double y1_;
  double delta_;

  double a_, b_, c_, d_;

  /**
   * ctor of Curve segment
   * @param theta0 theta at start
   * @param kappa0 kappa at start
   * @param x0 x at start
   * @param y0 y at start
   * @param theta1 theta at end
   * @param kappa1 kappa at end
   * @param x1 x at end
   * @param y1 y at end
   * @param delta delta of Curve segment
   */
  CurveSegment(double theta0, double kappa0, double dkappa0, double x0,
               double y0, double theta1, double kappa1, double dkappa1,
               double x1, double y1, double delta);

  /**
   * evaluate theta at s
   * @param s the Curve length
   * @return theta at s
   */
  double theta(double s) const;

  /**
   * evaluate kappa at s
   * @param s the Curve length
   * @return the kappa at s
   */
  double kappa(double s) const;

  /**
   * evaluate dkappa at s
   * @param s the Curve length
   * @return the dkappa at s
   */
  double dkappa(double s) const;

  /**
   * evaluate x at s
   * @param s the Curve length
   * @return the x at s
   */
  double x(double s) const;

  /**
   * evaluate y at s
   * @param s the Curve length
   * @return the y at s
   */
  double y(double s) const;
};

struct xy_error_functor {
  xy_error_functor() = default;

  template <typename T>
  bool operator()(const T *const node0, const T *const node1,
                  const T *const dist, T *residual) const {
    // do not use std::sin std::cos std::pow, use sin cos pow instead
    T theta0 = node0[0];
    T kappa0 = node0[1];
    T x0 = node0[3];
    T y0 = node0[4];

    T theta1 = node1[0];
    T kappa1 = node1[1];
    T x1 = node1[3];
    T y1 = node1[4];
    T delta = dist[0];

    // calculate coefficient
    // theta = a + bs + cs^2 + ds^3
    T a = theta0;
    T b = kappa0;
    T c = (-2.0 * kappa0) / delta - kappa1 / delta -
          (3.0 * theta0) / (delta * delta) + (3.0 * theta1) / (delta * delta);
    T d = kappa0 / (delta * delta) + kappa1 / (delta * delta) +
          (2.0 * theta0) / (delta * delta * delta) -
          (2.0 * theta1) / (delta * delta * delta);

    // calculate dx using Curve function
    T dx = 0.06474248308443546 * delta *
               cos(a + 0.025446043828620812 * b * delta +
                   0.0006475011465280913 * c * (delta * delta) +
                   0.000016476342553636037 * d * (delta * delta * delta)) +
           0.13985269574463816 * delta *
               cos(a + 0.12923440720030277 * b * delta +
                   0.01670153200441367 * c * (delta * delta) +
                   0.002158412587927285 * d * (delta * delta * delta)) +
           0.1909150252525593 * delta *
               cos(a + 0.29707742431130146 * b * delta +
                   0.08825499603543704 * c * (delta * delta) +
                   0.02621856690481176 * d * (delta * delta * delta)) +
           0.20897959183673434 * delta *
               cos(a + 0.5 * b * delta + 0.25 * c * (delta * delta) +
                   0.125 * d * (delta * delta * delta)) +
           0.1909150252525593 * delta *
               cos(a + 0.7029225756886985 * b * delta +
                   0.49410014741283415 * c * (delta * delta) +
                   0.347314148267595 * d * (delta * delta * delta)) +
           0.13985269574463816 * delta *
               cos(a + 0.8707655927996972 * b * delta +
                   0.7582327176038082 * c * (delta * delta) +
                   0.6602429618244054 * d * (delta * delta * delta)) +
           0.06474248308443546 * delta *
               cos(a + 0.9745539561713792 * b * delta +
                   0.9497554134892865 * c * (delta * delta) +
                   0.9255878956111683 * d * (delta * delta * delta));
    T x_diff = x1 - x0 - dx;
    residual[0] = x_diff;

    // calculate dy using Curve function

    T dy = delta *
           (0.06474248308443546 *
                sin(a + 0.025446043828620812 * b * delta +
                    0.0006475011465280913 * c * (delta * delta) +
                    0.000016476342553636037 * d * (delta * delta * delta)) +
            0.1909150252525593 *
                sin(a + 0.29707742431130146 * b * delta +
                    0.08825499603543704 * c * (delta * delta) +
                    0.02621856690481176 * d * (delta * delta * delta)) +
            0.20897959183673434 *
                sin(a + 0.5 * b * delta + 0.25 * c * (delta * delta) +
                    0.125 * d * (delta * delta * delta)) +
            0.1909150252525593 *
                sin(a + 0.7029225756886985 * b * delta +
                    0.49410014741283415 * c * (delta * delta) +
                    0.347314148267595 * d * (delta * delta * delta)) +
            0.13985269574463816 *
                sin(a + 0.8707655927996972 * b * delta +
                    0.7582327176038082 * c * (delta * delta) +
                    0.6602429618244054 * d * (delta * delta * delta)) +
            0.06474248308443546 *
                sin(a + 0.9745539561713792 * b * delta +
                    0.9497554134892865 * c * (delta * delta) +
                    0.9255878956111683 * d * (delta * delta * delta)) +
            0.13985269574463816 *
                sin(a + delta * (0.12923440720030277 * b +
                                 0.01670153200441367 * c * delta +
                                 0.002158412587927285 * d * (delta * delta))));
    T y_diff = y1 - y0 - dy;
    residual[1] = y_diff;
    return true;
  }
};

struct objective_functor {
  objective_functor() = default;

  template <typename T>
  bool operator()(const T *const dist, T *residual) const {
    // do not use std::sin std::cos std::pow, use sin cos pow instead
    T delta = dist[0];

    residual[0] = delta * 10.0;


    return true;
  }
};

struct connection_point_functor {
  connection_point_functor() = default;

  template <typename T>
  bool operator()(const T *const node0, const T *const node1,
                  const T *const node2, const T *const dist,
                  T *residual) const {
    // do not use std::sin std::cos std::pow, use sin cos pow instead
    T theta0 = node0[0];
    T kappa0 = node0[1];
    T dkappa0 = node0[2];

    T theta1 = node1[0];
    T kappa1 = node1[1];
    T dkappa1 = node1[2];

    T delta = dist[0];

    T theta2 = node2[0];
    T kappa2 = node2[1];
    T delta1 = dist[1];

    // calculate coefficient
    // theta = a + bs + cs^2 + ds^3
    // kappa = b + 2cs + 3ds^2
    // dkappa = 2c + 6ds
    T a = theta0;
    T b = kappa0;
    T c = (-2.0 * kappa0) / delta - kappa1 / delta -
          (3.0 * theta0) / (delta * delta) + (3.0 * theta1) / (delta * delta);
    T d = kappa0 / (delta * delta) + kappa1 / (delta * delta) +
          (2.0 * theta0) / (delta * delta * delta) -
          (2.0 * theta1) / (delta * delta * delta);

    T a1 = theta1;
    T b1 = kappa1;
    T c1 = (-2.0 * kappa1) / delta1 - kappa2 / delta1 -
           (3.0 * theta1) / (delta1 * delta1) +
           (3.0 * theta2) / (delta1 * delta1);
    T d1 = kappa1 / (delta1 * delta1) + kappa2 / (delta1 * delta1) +
           (2.0 * theta1) / (delta1 * delta1 * delta1) -
           (2.0 * theta2) / (delta1 * delta1 * delta1);

    T cost_theta0 =
        a + b * delta + c * delta * delta + d * delta * delta * delta;

    T cost_theta1 =
        a1 + b1 * delta1 + c1 * delta1 * delta1 + d1 * delta1 * delta1 * delta1;
    // kappa = b + 2cs + 3ds^2
    T cost_kappa0 = b + 2.0 * c * delta + 3.0 * d * delta * delta;
    // dkappa = 2c + 6ds
    T cost_dkappa0 = 2.0 * c + 6.0 * d * delta;

    residual[0] = 1.0 * (theta1 - cost_theta0);
    residual[1] = 10.0 * (kappa1 - cost_kappa0);
    residual[2] = 100.0 * (dkappa1 - cost_dkappa0);
    return true;
  }
};

}  // namespace reference_line
