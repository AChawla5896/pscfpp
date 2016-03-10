/*
* PSCF - Polymer Self-Consistent Field Theory
*
* Copyright 2016, The Regents of the University of Minnesota
* Distributed under the terms of the GNU General Public License.
*/

#include "Mixture.h"
#include "Domain.h"

#include <cmath>

namespace Pscf { 
namespace Fd1d
{ 

   Mixture::Mixture()
   {  setClassName("Mixture"); }

   Mixture::~Mixture()
   {}

   void Mixture::readParameters(std::istream& in)
   {
      MixtureTmpl<Polymer, Solvent>::readParameters(in);
      read(in, "ds", ds_);

      UTIL_CHECK(nMonomer() > 0);
      UTIL_CHECK(nPolymer()+ nSolvent() > 0);
      UTIL_CHECK(ds_ > 0);
   }

   void Mixture::setDomain(Domain const& domain)
   {
      UTIL_CHECK(nMonomer() > 0);
      UTIL_CHECK(nPolymer()+ nSolvent() > 0);
      UTIL_CHECK(ds_ > 0);

      domainPtr_ = &domain;

      // Set discretization for all blocks
      int i, j;
      for (i = 0; i < nPolymer(); ++i) {
         for (j = 0; j < polymer(i).nBlock(); ++j) {
            polymer(i).block(j).setDiscretization(domain, ds_);
         }
      }

   }

   /*
   * Compute concentrations (but not total free energy).
   */
   void Mixture::compute(DArray<Mixture::WField> const & wFields, 
                         DArray<Mixture::CField>& cFields)
   {
      UTIL_CHECK(domainPtr_);
      UTIL_CHECK(domain().nx() > 0);
      UTIL_CHECK(nMonomer() > 0);
      UTIL_CHECK(nPolymer() + nSolvent() > 0);
      UTIL_CHECK(wFields.capacity() == nMonomer());
      UTIL_CHECK(cFields.capacity() == nMonomer());

      int nx = domain().nx();
      int nm = nMonomer();
      int i, j, k;

      // Clear all monomer concentration fields
      for (i = 0; i < nm; ++i) {
         UTIL_CHECK(cFields[i].capacity() == nx);
         UTIL_CHECK(wFields[i].capacity() == nx);
         for (j = 0; j < nx; ++j) {
            cFields[i][j] = 0.0;
         }
      }

      // Solve MDE for all polymers
      for (i = 0; i < nPolymer(); ++i) {
         polymer(i).compute(wFields);
      }

      // Accumulate monomer concentration fields
      for (i = 0; i < nPolymer(); ++i) {
         for (j = 0; j < polymer(i).nBlock(); ++j) {
            int monomerId = polymer(i).block(j).monomerId();
            UTIL_CHECK(monomerId >= 0);
            UTIL_CHECK(monomerId < nm);
            CField& monomerField = cFields[monomerId];
            CField& blockField = polymer(i).block(j).cField();
            for (k = 0; k < nx; ++k) {
               monomerField[k] += blockField[k];
            }
         }
      }
   }

   /*
   * Compute concentrations and total free energy.
   */
   void Mixture::compute(DArray<Mixture::WField> const & wFields, 
                         DArray<Mixture::CField>& cFields,
                         Interaction& interaction)
   {

      // Solve single species problems to compute concentrations.
      compute(wFields, cFields);

      fHelmholtz_ = 0.0;
 
      // Compute ideal gas contributions to fHelhmoltz_
      double phi, mu, length;
      for (int i = 0; i < nPolymer(); ++i) {
         phi = polymer(i).phi();
         mu = polymer(i).mu();
         length = polymer(i).length();
         fHelmholtz_ += phi*( mu - 1.0 )/length;
      }

      // Apply Legendre transform subtraction
      int nm = nMonomer();
      for (int i = 0; i < nm; ++i) {
         fHelmholtz_ -= 
                  domain().innerProduct(wFields[i], cFields[i]);
      }

      // Add average interaction free energy density per monomer
      int nx = domain().nx();
      if (!f_.isAllocated()) f_.allocate(nx);
      if (!c_.isAllocated()) c_.allocate(nm);
      int j;
      for (int i = 0; i < nx; ++i) {
         for (j = 0; j < nm; ++j) {
            c_[j] = cFields[j][i];
         }
         f_[i] = interaction.fHelmholtz(c_);
      }
      fHelmholtz_ = + domain().spatialAverage(f_);

      // Compute pressure
      pressure_ = -fHelmholtz_;
      for (int i = 0; i < nPolymer(); ++i) {
         phi = polymer(i).phi();
         mu = polymer(i).mu();
         length = polymer(i).length();
         pressure_ += phi*mu/length;
      }

   }

} // namespace Fd1d
} // namespace Pscf
