/*
* PSCF - Polymer Self-Consistent Field Theory
*
* Copyright 2016, The Regents of the University of Minnesota
* Distributed under the terms of the GNU General Public License.
*/

#include "ChiInteraction.h"

namespace Pscf {
   
   using namespace Util;

   ChiInteraction::ChiInteraction()
    : Interaction()
   {  setClassName("ChiInteraction"); }

   ChiInteraction::~ChiInteraction()
   {}

   void ChiInteraction::readParameters(std::istream& in)
   {
      UTIL_CHECK(nMonomer() > 0);
      chi_.allocate(nMonomer(), nMonomer());
      readDSymmMatrix(in, "chi", chi_, nMonomer());
   }

   double ChiInteraction::fHelmholtz(Array<double> const & c) const
   {
      int i, j;
      double sum = 0.0;
      for (i = 0; i < nMonomer(); ++i) {
         for (j = 0; j < nMonomer(); ++j) {
            sum += chi_(i, j)* c[i]*c[j];
         }
      }
      return 0.5*sum;
   }

   void 
   ChiInteraction::computeW(Array<double> const & c, double p, 
                            Array<double>& w) const
   {
      int i, j;
      for (i = 0; i < nMonomer(); ++i) {
         w[i] = p;
         for (j = 0; j < nMonomer(); ++j) {
            w[i] += chi_(i, j)* c[j];
         }
      }
   }

   void 
   ChiInteraction::computeC(Array<double> const & w, Array<double>& c)
   const
   {}

   void 
   ChiInteraction::computeDwDc(Array<double> const & c, Matrix<double>& dWdC)
   const
   {
      int i, j;
      for (i = 0; i < nMonomer(); ++i) {
         for (j = 0; j < nMonomer(); ++j) {
            dWdC(i, j) = chi_(i, j);
         }
      }
   }

} // namespace Pscf
