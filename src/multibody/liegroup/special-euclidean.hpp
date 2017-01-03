//
// Copyright (c) 2016 CNRS
//
// This file is part of Pinocchio
// Pinocchio is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// Pinocchio is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Lesser Public License for more details. You should have
// received a copy of the GNU Lesser General Public License along with
// Pinocchio If not, see
// <http://www.gnu.org/licenses/>.

#ifndef __se3_special_euclidean_operation_hpp__
#define __se3_special_euclidean_operation_hpp__

#include <limits>

#include <pinocchio/spatial/fwd.hpp>
#include <pinocchio/spatial/se3.hpp>
#include <pinocchio/multibody/liegroup/operation-base.hpp>

#include <pinocchio/multibody/liegroup/vector-space.hpp>
#include <pinocchio/multibody/liegroup/cartesian-product.hpp>
#include <pinocchio/multibody/liegroup/special-orthogonal.hpp>

namespace se3
{
  struct SpecialEuclideanOperation;
  template <> struct traits<SpecialEuclideanOperation> {
    typedef double Scalar;
    enum {
      NQ = 7,
      NV = 6
    };
    typedef Eigen::Matrix<Scalar,NQ,1> ConfigVector_t;
    typedef Eigen::Matrix<Scalar,NV,1> TangentVector_t;
  };

  struct SpecialEuclideanOperation : public LieGroupOperationBase <SpecialEuclideanOperation>
  {
    typedef CartesianProductOperation <VectorSpaceOperation<3>, SpecialOrthogonalOperation> R3crossSO3_t;
    typedef SpecialEuclideanOperation LieGroupDerived;
    SE3_LIE_GROUP_TYPEDEF;

    typedef Eigen::Quaternion<Scalar> Quaternion_t;
    typedef Eigen::Map<      Quaternion_t> QuaternionMap_t;
    typedef Eigen::Map<const Quaternion_t> ConstQuaternionMap_t;
    typedef SE3 Transformation_t;

    template <class ConfigL_t, class ConfigR_t, class Tangent_t>
    static void difference_impl(const Eigen::MatrixBase<ConfigL_t> & q0,
                                const Eigen::MatrixBase<ConfigR_t> & q1,
                                const Eigen::MatrixBase<Tangent_t> & d)
    {
      ConstQuaternionMap_t p0 (q0.derived().template tail<4>().data());
      ConstQuaternionMap_t p1 (q1.derived().template tail<4>().data());
      const_cast < Eigen::MatrixBase<Tangent_t>& > (d)
        = log6(  SE3(p0.matrix(), q0.derived().template head<3>()).inverse()
               * SE3(p1.matrix(), q1.derived().template head<3>())).toVector();
    }

    template <class ConfigIn_t, class Velocity_t, class ConfigOut_t>
    static void integrate_impl(const Eigen::MatrixBase<ConfigIn_t> & q,
                               const Eigen::MatrixBase<Velocity_t> & v,
                               const Eigen::MatrixBase<ConfigOut_t> & qout)
    {
      ConfigOut_t& out = (const_cast< Eigen::MatrixBase<ConfigOut_t>& >(qout)).derived();
      ConstQuaternionMap_t quat(q.derived().template tail<4>().data());
      QuaternionMap_t res_quat (out.template tail<4>().data());

      SE3 M0 (quat.matrix(), q.derived().template head<3>());
      SE3 M1 (M0 * exp6(Motion(v)));

      out.template head<3>() = M1.translation();
      res_quat = M1.rotation();
      // Norm of qs might be epsilon-different to 1, so M1.rotation might be epsilon-different to a rotation matrix.
      // It is then safer to re-normalized after converting M1.rotation to quaternion.
      firstOrderNormalize(res_quat);
    }

    // interpolate_impl use default implementation.
    // template <class ConfigL_t, class ConfigR_t, class ConfigOut_t>
    // static void interpolate_impl(const Eigen::MatrixBase<ConfigL_t> & q0,
                                 // const Eigen::MatrixBase<ConfigR_t> & q1,
                                 // const Scalar& u,
                                 // const Eigen::MatrixBase<ConfigOut_t>& qout)
    // {
    // }

    template <class ConfigL_t, class ConfigR_t>
    static double squaredDistance_impl(const Eigen::MatrixBase<ConfigL_t> & q0,
                                       const Eigen::MatrixBase<ConfigR_t> & q1)
    {
      TangentVector_t t;
      difference_impl(q0, q1, t);
      return t.squaredNorm();
    }

    template <class Config_t>
    static void random_impl (const Eigen::MatrixBase<Config_t>& qout)
    {
      R3crossSO3_t::random(qout);
    }

    template <class ConfigL_t, class ConfigR_t, class ConfigOut_t>
    static void randomConfiguration_impl(const Eigen::MatrixBase<ConfigL_t> & lower,
                                         const Eigen::MatrixBase<ConfigR_t> & upper,
                                         const Eigen::MatrixBase<ConfigOut_t> & qout)
    { 
      R3crossSO3_t::randomConfiguration(lower, upper, qout);
    } 

    template <class ConfigL_t, class ConfigR_t>
    static bool isSameConfiguration_impl(const Eigen::MatrixBase<ConfigL_t> & q0,
                                         const Eigen::MatrixBase<ConfigR_t> & q1,
                                         const Scalar & prec)
    {
      return R3crossSO3_t::isSameConfiguration(q0, q1, prec);
    }
  }; // struct SpecialEuclideanOperation

  struct SpecialEuclidean1Operation;
  template <> struct traits<SpecialEuclidean1Operation> {
    typedef double Scalar;
    enum {
      NQ = 4,
      NV = 3
    };
    typedef Eigen::Matrix<Scalar,NQ,1> ConfigVector_t;
    typedef Eigen::Matrix<Scalar,NV,1> TangentVector_t;
  };

  struct SpecialEuclidean1Operation : public LieGroupOperationBase <SpecialEuclidean1Operation>
  {
    typedef CartesianProductOperation <VectorSpaceOperation<2>, SpecialOrthogonal1Operation> R2crossSO1_t;
    typedef SpecialEuclidean1Operation LieGroupDerived;
    SE3_LIE_GROUP_TYPEDEF;

    template <class ConfigL_t, class ConfigR_t, class Tangent_t>
    static void difference_impl(const Eigen::MatrixBase<ConfigL_t> & q0,
                                const Eigen::MatrixBase<ConfigR_t> & q1,
                                const Eigen::MatrixBase<Tangent_t> & d)
    {
      SE3 M0(SE3::Identity()); forwardKinematics(M0, q0);
      SE3 M1(SE3::Identity()); forwardKinematics(M1, q1);

      Motion nu(log6(M0.inverse()*M1)); // TODO: optimize implementation
      
      Tangent_t& out = const_cast< Eigen::MatrixBase<Tangent_t>& >(d).derived();
      out.template head<2>() = nu.linear().head<2>();
      out(2) = nu.angular()(2);
    }

    template <class ConfigIn_t, class Velocity_t, class ConfigOut_t>
    static void integrate_impl(const Eigen::MatrixBase<ConfigIn_t> & q,
                               const Eigen::MatrixBase<Velocity_t> & vs,
                               const Eigen::MatrixBase<ConfigOut_t> & qout)
    {
      ConfigOut_t& out = (const_cast< Eigen::MatrixBase<ConfigOut_t>& >(qout)).derived();
      typedef Eigen::Matrix<Scalar, 2, 2> Matrix22;
      typedef Eigen::Matrix<Scalar, 2, 1> Vector2;

      const double& c0 = q(2), s0 = q(3);
      Matrix22 R0;
      R0 << c0, -s0, s0, c0;
      
      const double& t = vs[2];
      const double theta = std::fabs(t);

      if(theta > 1e-14)
      {
        // vs = [ x, y, t ]
        // w = [ 0, 0, t ]
        // v = [ x, y, 0 ]
        // Considering only the 2x2 top left corner:
        // Sp = [ 0, -1; 1, 0],
        // if t > 0: S = Sp
        // else    : S = -Sp
        // S / t = Sp / |t|
        // S * S = - I2
        // R = I2 + ( 1 - ct) / |t| * S + ( 1 - st / |t| ) * S * S
        //   =      ( 1 - ct) / |t| * S +       st / |t|   * I2
        //
        // Ru = exp3 (w)
        // tu = R * v = (1 - ct) / |t| * S * v + st / t * v
        //
        // M0 * Mu = ( R0 * Ru, R0 * tu + t0 )

        // FIXME remove this copy.
        Vector2 v = vs.template head<2>();
        Eigen::Matrix<Scalar,2,1> cst;
        SINCOS (t, &cst[1], &cst[0]);
        const Scalar inv_theta = 1/theta;
        const Scalar c_coeff = (1.-cst[0]) * inv_theta;
        const Scalar s_coeff = std::fabs(cst[1]) * inv_theta;
        const Vector2 Sp_v (-v[1], v[0]);

        if (t > 0) out.template head<2>() = q.template head<2>() + R0 * (s_coeff * v + c_coeff * Sp_v);
        else       out.template head<2>() = q.template head<2>() + R0 * (s_coeff * v - c_coeff * Sp_v);
        out.template tail<2>() = R0 * cst;
      }
      else
      {
        // cos(t) ~ 1 - t^2 / 2
        // sin(t) ~ t
        out.template head<2>() = q.template head<2>() + R0*vs.template head<2>();
        out(2) = c0 * 1 - s0 * t;
        out(3) = s0 * 1 + c0 * t;
      }
    }

    // interpolate_impl use default implementation.
    // template <class ConfigL_t, class ConfigR_t, class ConfigOut_t>
    // static void interpolate_impl(const Eigen::MatrixBase<ConfigL_t> & q0,
                                 // const Eigen::MatrixBase<ConfigR_t> & q1,
                                 // const Scalar& u,
                                 // const Eigen::MatrixBase<ConfigOut_t>& qout)

    // template <class ConfigL_t, class ConfigR_t>
    // static double squaredDistance_impl(const Eigen::MatrixBase<ConfigL_t> & q0,
                                       // const Eigen::MatrixBase<ConfigR_t> & q1)

    template <class Config_t>
    static void random_impl (const Eigen::MatrixBase<Config_t>& qout)
    {
      R2crossSO1_t::random(qout);
    }

    template <class ConfigL_t, class ConfigR_t, class ConfigOut_t>
    static void randomConfiguration_impl(const Eigen::MatrixBase<ConfigL_t> & lower,
                                         const Eigen::MatrixBase<ConfigR_t> & upper,
                                         const Eigen::MatrixBase<ConfigOut_t> & qout)
    { 
      R2crossSO1_t::randomConfiguration(lower, upper, qout);
    } 

    template <class ConfigL_t, class ConfigR_t>
    static bool isSameConfiguration_impl(const Eigen::MatrixBase<ConfigL_t> & q0,
                                         const Eigen::MatrixBase<ConfigR_t> & q1,
                                         const Scalar & prec)
    {
      return R2crossSO1_t::isSameConfiguration(q0, q1, prec);
    }

    private:
    template<typename V>
    static void forwardKinematics(SE3 & M, const Eigen::MatrixBase<V>& q)
    {
      EIGEN_STATIC_ASSERT_SAME_VECTOR_SIZE(ConfigVector_t,V);

      const double& c_theta = q(2),
                    s_theta = q(3);
      
      M.rotation().topLeftCorner<2,2>() << c_theta, -s_theta, s_theta, c_theta;
      M.translation().head<2>() = q.template head<2>();
    }
  }; // struct SpecialEuclidean1Operation

} // namespace se3

#endif // ifndef __se3_special_euclidean_operation_hpp__
