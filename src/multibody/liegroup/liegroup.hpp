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

#ifndef __se3_lie_group_hpp__
#define __se3_lie_group_hpp__

#include "pinocchio/assert.hpp"
#include "pinocchio/multibody/liegroup/vector-space.hpp"
#include "pinocchio/multibody/liegroup/cartesian-product.hpp"
#include "pinocchio/multibody/liegroup/special-orthogonal.hpp"
#include "pinocchio/multibody/liegroup/special-euclidean.hpp"

// #include "pinocchio/multibody/joint/joint.hpp"

namespace se3 {
  template<typename Joint>
  struct LieGroup {
    typedef VectorSpaceOperation<Joint::NQ> type;
  };

  template<> struct LieGroup <JointModelSpherical> {
    typedef SpecialOrthogonalOperation type;
  };
  template<> struct LieGroup <JointModelFreeFlyer> {
    typedef SpecialEuclideanOperation type;
  };
  template<> struct LieGroup <JointModelPlanar> {
    typedef SpecialEuclidean1Operation type;
  };
  template<int Axis> struct LieGroup <JointModelRevoluteUnbounded<Axis> > {
    typedef SpecialOrthogonal1Operation type;
  };

  // TODO REMOVE: For testing purposes only
  // template<>
  // struct LieGroup <JointModelTranslation> {
    // typedef CartesianProductOperation<
      // CartesianProductOperation<typename LieGroup<JointModelPX>::type, typename LieGroup<JointModelPY>::type>,
      // typename LieGroup<JointModelPZ>::type
      // > type;
  // };
}

#endif // ifndef __se3_lie_group_hpp__
