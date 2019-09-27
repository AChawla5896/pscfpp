#ifndef PSSP_POLYMER_H
#define PSSP_POLYMER_H

/*
* PSCF - Polymer Self-Consistent Field Theory
*
* Copyright 2016, The Regents of the University of Minnesota
* Distributed under the terms of the GNU General Public License.
*/

#include "Block.h"
#include <pscf/solvers/PolymerTmpl.h>
#include <pssp/field/RField.h>
#include <util/containers/FArray.h>      // member template

namespace Pscf { 
namespace Pssp { 

   /**
   * Descriptor and solver for a polymer species.
   *
   * The block concentrations stored in the constituent Block<D>
   * objects contain the block concentrations (i.e., volume 
   * fractions) computed in the most recent call of the compute 
   * function.
   *
   * The phi() and mu() accessor functions, which are inherited 
   * from PolymerTmp< Block<D> >, return the value of phi (spatial 
   * average volume fraction of a species) or mu (chemical
   * potential) computed in the last call of the compute function.
   * If the ensemble for this species is closed, phi is read from 
   * the parameter file and mu is computed. If the ensemble is
   * open, mu is read from the parameter file and phi is computed.
   *
   * \ingroup Pssp_Solvers_Module
   */
   template <int D>
   class Polymer : public PolymerTmpl< Block<D> >
   {

   public:

      typedef PolymerTmpl< Block<D> > Base;

      typedef typename Block<D>::WField  WField;

      Polymer();

      ~Polymer();

      void setPhi(double phi);

      void setMu(double mu);

      /**
      * Compute solution to MDE and concentrations.
      */ 
      void setupUnitCell(UnitCell<D> const & unitCell);

      /**
      * Compute solution to MDE and concentrations.
      */ 
      void compute(DArray<WField> const & wFields);

      /**
      * Stress with respect to unit cell parameter n.
      */
      double stress(int n) const;

      /**
      * Compute stress from a polymer chain, needs a pointer to basis
      */
      void computeStress();

      using Base::nBlock;
      using Base::block;
      using Base::ensemble;
      using Base::solve;
      using Base::length;

   protected:

      using ParamComposite::setClassName;

   private: 

      /// Pointer to associated UnitCell<D>
      const UnitCell<D>* unitCellPtr_;

      // Stress due to a whole Polymer chain
      FArray<double, 6> stress_;

      using Base::phi_;
      using Base::mu_;

   };

   /// Stress with respect to unit cell parameter n.
   template <int D>
   inline double Polymer<D>::stress(int n) const
   {  return stress_[n]; }
  
   #ifndef PSSP_POLYMER_TPP
   extern template class Polymer<1>;
   extern template class Polymer<2>;
   extern template class Polymer<3>;
   #endif

}
}
// #include "Polymer.tpp"
#endif
