#ifndef FD1D_PROPAGATOR_H
#define FD1D_PROPAGATOR_H

/*
* PFTS - Polymer Field Theory Simulator
*
* Copyright 2013, David Morse (morse012@.umn.edu)
* Distributed under the terms of the GNU General Public License.
*/

#include <chem/PropagatorTmpl.h>       // base class template
#include <fd1d/TridiagonalSolver.h>    // member
#include <util/containers/DArray.h>    // member template

namespace Fd1d{ 

   using namespace Util;
   using namespace Chem;

   class Propagator : public PropagatorTmpl<Propagator>
   {

   public:

      // Public typedefs

      /**
      * Chemical potential field type.
      */
      typedef DArray<double> WField;

      /**
      * Monomer concentration field type.
      */
      typedef DArray<double> CField;

      /**
      * Propagator q-field type.
      */
      typedef DArray<double> QField;

      // Member functions

      /**
      * Constructor.
      */
      Propagator();

      /**
      * Initialize grids and allocate required memory.
      *
      * \param ns number of contour grid points
      * \param nx number of spatial grid points
      * \param dx spatial grid step size
      * \param step monomer statistical step size
      */
      void init(int ns, int nx, double dx, double step);

      /**
      * Solve the modified diffusion equation for this block.
      *
      * \param w chemical potential field for appropriate monomer type
      */
      void solve(const WField& w);
  
      /**
      * Integrate to calculate monomer concentration for this block
      */ 
      void integrate(CField& integral);

      /**
      * Return q-field at beginning of block.
      */
      const QField& head() const;

      /**
      * Return q-field at end of block.
      */
      const QField& tail() const;

   protected:

      void setup(const WField& w);

      void step(int i);
 
   private:
      
      DArray<QField> qFields_;

      // Elements of tridiagonal matrices used in propagation
      DArray<double> dA_;
      DArray<double> dB_;
      DArray<double> uA_;
      DArray<double> uB_;

      // Work vector
      DArray<double> v_;

      // Solver used in Crank-Nicholson algorithm
      TridiagonalSolver solver_;

      /// Contour length step size.
      double ds_;

      /// Spatial grid step size.
      double dx_;

      /// Monomer statistical segment length.
      double step_;

      /// Number of contour length steps = # grid points - 1.
      int ns_;

      /// Number of spatial grid points.
      int nx_;

   };

   /*
   * Return q-field at beginning of block.
   */
   inline Propagator::QField const& Propagator::head() const
   {  return qFields_[0]; }

   /*
   * Return q-field at end of block, after solution.
   */
   inline Propagator::QField const& Propagator::tail() const
   {  return qFields_[ns_]; }

} 
#endif
