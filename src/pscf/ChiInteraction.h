#ifndef PSCF_CHI_INTERACTION_H
#define PSCF_CHI_INTERACTION_H

/*
* PSCF - Polymer Self-Consistent Field Theory
*
* Copyright 2016, The Regents of the University of Minnesota
* Distributed under the terms of the GNU General Public License.
*/

#include "Interaction.h"             // base class
#include <util/global.h>                  

namespace Pscf {

   using namespace Util;

   /**
   * Flory-Huggins excess free energy model.
   *
   * \ingroup Pscf_Fd1d_Module
   */
   class ChiInteraction : public  Interaction
   {

   public:

      /**
      * Constructor.
      */
      ChiInteraction();

      /**
      * Destructor.
      */
      virtual ~ChiInteraction();

      /**
      * Read chi parameters.
      *
      * Must be called after setNMonomer.
      */
      virtual void readParameters(std::istream& in);

      /**
      * Compute excess Helmholtz free energy per monomer.
      *
      * \param c array of concentrations, for each type (input)
      */
      virtual 
      double fHelmholtz(Array<double> const & c) const;

      /**
      * Compute chemical potential from concentration and pressure.
      *
      * \param c array of concentrations, for each type (input)
      * \param p Lagrange multiplier / pressure 
      * \param w array of chemical potentials for types (ouptut) 
      */
      virtual 
      void computeW(Array<double> const & c, double p, 
                    Array<double>& w) const;

      /**
      * Compute concentration from chemical potential field.
      *
      * \param w array of chemical potentials for types (inut) 
      * \param c array of concentrations, for each type (output)
      */
      virtual 
      void computeC(Array<double> const & w, Array<double>& c) 
      const;

      /**
      * Compute second derivatives of free energy.
      *
      * Upon return, the elements of the square matrix dWdC, are
      * given by derivatives dWdC(i,j) = dW(i)/dC(j), which are
      * also second derivatives of the interaction free energy. 
      * For the Flory-Huggins chi parameter model, this is simply 
      * given by the chi matrix dWdC(i,j) = chi(i, j).
      *
      * \param c array of concentrations, for each type (input)
      * \param dWdC matrix of derivatives (output) 
      */
      virtual 
      void computeDwDc(Array<double> const & w, Matrix<double>& dWdC)
      const;

      /**
      * Return one element of the chi matrix.
      *
      * \param i row index
      * \param j column index
      */
      double chi(int i, int j);

   private:

      DMatrix<double> chi_;

      DMatrix<double> chiInverse_;

   };

   // Inline function

   inline double ChiInteraction::chi(int i, int j)
   {  return chi_(i, j); }

} // namespace Pscf
#endif
