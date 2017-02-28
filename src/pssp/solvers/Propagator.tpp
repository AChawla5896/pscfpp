#ifndef PSSP_PROPAGATOR_TPP
#define PSSP_PROPAGATOR_TPP

/*
* PSCF - Polymer Self-Consistent Field Theory
*
* Copyright 2016, The Regents of the University of Minnesota
* Distributed under the terms of the GNU General Public License.
*/

#include "Propagator.h"
#include "Block.h"

// #include <fd1d/domain/Domain.h>

namespace Pscf { 
namespace Pssp {

   using namespace Util;

   /*
   * Constructor.
   */
   template <int D>
   Propagator<D>::Propagator()
    : blockPtr_(0),
      ns_(0),
      nx_(0),
      isAllocated_(false)
   {}

   /*
   * Destructor.
   */
   template <int D>
   Propagator<D>::~Propagator()
   {}

   template <int D>
   void Propagator<D>::allocate(int ns, int nx)
   {
      ns_ = ns;
      nx_ = nx;
      qFields_.allocate(ns);
      for (int i = 0; i < ns; ++i) {
         qFields_[i].allocate(nx);
      }
      isAllocated_ = true;
   }

   template <int D>
   bool Propagator<D>::isAllocated() const
   {  return isAllocated_; }

   #if 0
   /*
   * Compute initial head QField from final tail QFields of sources.
   */
   template <int D>
   void Propagator<D>::computeHead()
   {

      // Reference to head of this propagator
      QField& qh = qFields_[0];

      // Initialize qh field to 1.0 at all grid points
      int ix;
      for (ix = 0; ix < nx_; ++ix) {
         qh[ix] = 1.0;
      }

      // Pointwise multiply tail QFields of all sources
      for (int is = 0; is < nSource(); ++is) {
         if (!source(is).isSolved()) {
            UTIL_THROW("Source not solved in computeHead");
         }
         QField const& qt = source(is).tail();
         for (ix = 0; ix < nx_; ++ix) {
            qh[ix] *= qt[ix];
         }
      }
   }

   /*
   * Solve the modified diffusion equation for this block.
   */
   template <int D>
   void Propagator<D>::solve()
   {
      computeHead();
      for (int iStep = 0; iStep < ns_ - 1; ++iStep) {
         block().step(qFields_[iStep], qFields_[iStep + 1]);
      }
      setIsSolved(true);
   }

   /*
   * Solve the modified diffusion equation with specified initial field.
   */
   template <int D>
   void Propagator<D>::solve(const Propagator<D>::QField& head) 
   {
      // Initialize initial (head) field
      QField& qh = qFields_[0];
      for (int i = 0; i < nx_; ++i) {
         qh[i] = head[i];
      }

      // Setup solver and solve
      for (int iStep = 0; iStep < ns_ - 1; ++iStep) {
         block().step(qFields_[iStep], qFields_[iStep + 1]);
      }
      setIsSolved(true);
   }

   /*
   * Integrate to calculate monomer concentration for this block
   */
   template <int D>
   double Propagator<D>::computeQ()
   {
      if (!isSolved()) {
         UTIL_THROW("Propagator is not solved.");
      }
      if (!hasPartner()) {
         UTIL_THROW("Propagator has no partner set.");
      }
      if (!partner().isSolved()) {
         UTIL_THROW("Partner propagator is not solved");
      }
      QField const& qh = head();
      QField const& qt = partner().tail();
      return block().domain().innerProduct(qh, qt);
   }
   #endif

}
}
#endif
