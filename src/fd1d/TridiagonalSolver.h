#ifndef FD1D_TRIDIAGONAL_SOLVER_H
#define FD1D_TRIDIAGONAL_SOLVER_H

/*
* PFTS - Polymer Field Theory Simulator
*
* Copyright 2013, David Morse (morse012@.umn.edu)
* Distributed under the terms of the GNU General Public License.
*/

#include <util/containers/DArray.h>

#include <string>
#include <iostream>

#endif

namespace Fd1d {

   using namespace Util;
  
   class TridiagonalSolver
   {

      /**
      * Constructor.
      */
      TridiagonalSolver();

      /**
      * Allocate memory.
      *
      * \param n dimension of n x n square array.
      */
      void allocate(int n);

      /**
      * Compute the LU decomposition for later use.
      */
      void computeLU(const DArray<double>& d, const DArray<double>& u);

      /**
      * Solve Ax = b.
      */
      void solve(const DArray<double>& b, DArray<double>& x);

   private:

      // Diagonal elements
      DArray<double> d_;

      // Upper off-diagonal elements (unmodified by computeLU)
      DArray<double> u_;

      // Lower off-diagonal elements (replaced by multipliers)
      DArray<double> l_;

      // Work space.
      DArray<double> y_;

      int n_;
   };

}
