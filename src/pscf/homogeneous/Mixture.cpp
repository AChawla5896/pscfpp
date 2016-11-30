/*
* PSCF - Molecule Self-Consistent Field Theory
*
* Copyright 2016, The Regents of the University of Minnesota
* Distributed under the terms of the GNU General Public License.
*/

#include "Mixture.h"
#include <pscf/inter/Interaction.h>
#include <pscf/math/LuSolver.h>
#include <cmath>

namespace Pscf {
namespace Homogeneous {

   using namespace Util;

   /*
   * Constructor.
   */
   Mixture::Mixture()
    : ParamComposite(),
      molecules_(),
      mu_(),
      phi_(),
      c_(),
      w_(),
      residual_(),
      dX_(),
      dWdC_(),
      dWdPhi_(),
      jacobian_(),
      solverPtr_(0),
      nMolecule_(0),
      nMonomer_(0)
   {  setClassName("Mixture"); }

   /*
   * Destructor.
   */
   Mixture::~Mixture()
   {}

   /*
   * Read all parameters and initialize.
   */
   void Mixture::readParameters(std::istream& in)
   {
      // Precondition
      UTIL_ASSERT(molecules_.capacity() == 0);

      read<int>(in, "nMonomer", nMonomer_);
      c_.allocate(nMonomer_);
      w_.allocate(nMonomer_);

      read<int>(in, "nMolecule", nMolecule_);
      molecules_.allocate(nMolecule_);
      mu_.allocate(nMolecule_);
      phi_.allocate(nMolecule_);
      for (int i = 0; i < nMolecule_; ++i) {
         readParamComposite(in, molecules_[i]);
      }

      validate();
   }

   void Mixture::setNMolecule(int nMolecule)
   {
      UTIL_ASSERT(molecules_.capacity() == 0);
      nMolecule_ = nMolecule;
      molecules_.allocate(nMolecule_);
      mu_.allocate(nMolecule_);
      phi_.allocate(nMolecule_);
   }

   void Mixture::setNMonomer(int nMonomer)
   {
      UTIL_ASSERT(nMonomer_ == 0);
      nMonomer_ = nMonomer;
      c_.allocate(nMonomer_);
      w_.allocate(nMonomer_);
   }

   /*
   * Set molecular and monomer volume fractions.
   */
   void Mixture::setComposition(DArray<double> const & phi)
   {
      validate();
      UTIL_ASSERT(phi.capacity() == nMolecule_);

      // Set molecular volume fractions
      double sum = 0;
      for (int i = 0; i < nMolecule_ - 1; ++i) {
         UTIL_CHECK(phi[i] >= 0.0);
         UTIL_CHECK(phi[i] <= 1.0);
         phi_[i] = phi[i];
         sum += phi[i];
      }
      UTIL_CHECK(sum <= 1.0);
      phi_[nMolecule_ -1] = 1.0 - sum;

      computeC();
   }

   void Mixture::computeC()
   {
      // Initialize monomer fractions to zero
      int k;
      for (k = 0; k < nMonomer_; ++k) {
         c_[k] = 0.0;
      }

      // Compute monomer volume fractions
      double concentration;
      int i, j;
      for (i = 0; i < nMolecule_; ++i) {
         Molecule& mol = molecules_[i];
         concentration = phi_[i]/mol.size();
         for (j = 0; j < molecules_[i].nClump(); ++j) {
            k = mol.clump(j).monomerId();
            c_[k] += concentration*mol.clump(j).size();
         }
      }

   }

   /*
   * Compute thermodynamic properties.
   */
   void Mixture::computeMu(Interaction const & interaction, 
                           double xi)
   {
      UTIL_ASSERT(interaction.nMonomer() == nMonomer_);

      interaction.computeW(c_, w_);

      int imol, iclump, imon;
      double mu, size;
      for (imol = 0; imol < nMolecule_; ++imol) {
         Molecule& mol = molecules_[imol];
         mu = log( phi_[imol] );
         mu += xi*mol.size();
         for (iclump = 0; iclump < mol.nClump(); ++iclump) {
            imon = mol.clump(iclump).monomerId();
            size = mol.clump(iclump).size();
            mu += size*w_[imon];
         }
         mu_[imol] = mu;
      }
   }

   /*
   * Compute composition from chemical potentials.
   */
   void Mixture::computePhi(Interaction const & interaction, 
                           DArray<double> const & mu, 
                           DArray<double> const & phi, 
                           double& xi)
   {
      UTIL_ASSERT(interaction.nMonomer() == nMonomer_);

      // Allocate residual and jacobian on first use.
      if (residual_.capacity() == 0) {
         residual_.allocate(nMonomer_);
         dX_.allocate(nMolecule_);
         dWdC_.allocate(nMonomer_, nMonomer_);
         dWdPhi_.allocate(nMonomer_, nMolecule_);
         jacobian_.allocate(nMolecule_, nMolecule_);
         solverPtr_ = new LuSolver();
         solverPtr_->allocate(nMolecule_);
      }

      // Compute initial state, with guess for xi.
      setComposition(phi);
      computeMu(interaction, xi);

      double error;
      double epsilon = 1.0E-8;
      computeResidual(mu, error);

      // std::cout << "mu[0] =" << mu[0] << std::endl;
      // std::cout << "mu[1] =" << mu[1] << std::endl;
      // std::cout << "mu_[0] =" << mu_[0] << std::endl;
      // std::cout << "mu_[1] =" << mu_[1] << std::endl;
      // std::cout << "residual[0] =" << residual_[0] << std::endl;
      // std::cout << "residual[1] =" << residual_[1] << std::endl;
      // std::cout << "error     ="  << error << std::endl;

      if (error < epsilon) return;

      double s1;  // clump size
      double v1;  // molecule size
      double f1;  // clump fraction
      int m1, m2; // molecule type indices
      int c1;     // clump index
      int t1, t2; // monomer type indices

      for (int it = 0; it < 50; ++it) {

         // Compute matrix of derivative dWdC (C = monomer fraction)
         interaction.computeDwDc(c_, dWdC_);

         // Compute matrix derivative dWdPhi (Phi = molecule fraction)
         for (t1 = 0; t1 < nMonomer_; ++t1) {
            for (m1 = 0; m1 < nMolecule_; ++m1) {
               dWdPhi_(t1, m1) = 0.0;
            }
         }
         for (m1 = 0; m1 < nMolecule_; ++m1) {
            v1 = molecule(m1).size();
            for (c1 = 0; c1 < molecule(m1).nClump(); ++c1) {
               t1 = molecule(m1).clump(c1).monomerId();
               s1 = molecule(m1).clump(c1).size();
               f1 = s1/v1;
               for (t2 = 0; t2 < nMonomer_; ++t2) {
                  dWdPhi_(t2, m1) += dWdC_(t2, t1)*f1;
               }
            }
         }

         // Compute matrix d(mu)/d(Phi), stored in jacobian_
         for (m1 = 0; m1 < nMolecule_; ++m1) {
            for (m2 = 0; m2 < nMolecule_; ++m2) {
               jacobian_(m1, m2) = 0.0;
            }
            jacobian_(m1, m1) = 1.0/phi_[m1];
         }
         for (m1 = 0; m1 < nMolecule_; ++m1) {
            v1 = molecule(m1).size();
            for (c1 = 0; c1 < molecule(m1).nClump(); ++c1) {
               t1 = molecule(m1).clump(c1).monomerId();
               s1 = molecule(m1).clump(c1).size();
               for (m2 = 0; m2 < nMolecule_; ++m2) {
                  jacobian_(m1, m2) += s1*dWdPhi_(t1, m2);
               }
            }
         }

         // Modify treatment of incompressibility
         int mLast = nMolecule_ - 1;
         for (m1 = 0; m1 < nMolecule_; ++m1) {
            for (m2 = 0; m2 < nMolecule_ - 1; ++m2) {
               jacobian_(m1, m2) -= jacobian_(m1, mLast);
            }
            // Derivative of mu_[m1] with respect to xi
            jacobian_(m1, mLast) = molecule(m1).size();
         }

         solverPtr_->computeLU(jacobian_);
         solverPtr_->solve(residual_, dX_);
         // std::cout << "dPhi[0] =" << dX_[0] << std::endl;
         // std::cout << "dxi     ="  << dX_[1] << std::endl;

         // Update phi and xi fields
         double sum = 0.0;
         for (m1 = 0; m1 < nMolecule_ - 1; ++m1) {
            phi_[m1] = phi_[m1] - dX_[m1];
            sum += phi_[m1];
         }
         phi_[mLast] = 1.0 - sum;
         xi = xi - dX_[mLast];

         // Compute and test residual
         computeC();
         computeMu(interaction, xi);
         computeResidual(mu, error);
         //std::cout << "mu[0] =" << mu[0] << std::endl;
         //std::cout << "mu[1] =" << mu[1] << std::endl;
         //std::cout << "mu_[0] =" << mu_[0] << std::endl;
         //std::cout << "mu_[1] =" << mu_[1] << std::endl;
         //std::cout << "residual[0] =" << residual_[0] << std::endl;
         //std::cout << "residual[1] =" << residual_[1] << std::endl;
         //std::cout << "error     ="  << error << std::endl;

         if (error < epsilon) return;
      }

      UTIL_THROW("Failed to converge");
   }

   void Mixture::computeResidual(DArray<double> const & mu, double& error)
   {
      error = 0.0;
      for (int i = 0; i < nMonomer_; ++i) {
         residual_[i] = mu_[i] - mu[i];
         if (std::abs(residual_[i]) > error) {
            error = std::abs(residual_[i]);
         }
      }
   }

   /*
   * Check validity after completing initialization.
   */
   void Mixture::validate() const
   {
      UTIL_ASSERT(nMolecule_ > 0);
      UTIL_ASSERT(nMonomer_ > 0);
      for (int i = 0; i < nMolecule_; ++i) {
         Molecule const & mol = molecules_[i];
         UTIL_ASSERT(mol.nClump() > 0);
         for (int j = 0; j < mol.nClump(); ++j) {
            UTIL_ASSERT(mol.clump(j).monomerId() < nMonomer_);
         }
      }
   }

} // namespace Homogeneous
} // namespace Pscf
