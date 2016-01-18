#ifndef PSCF_MIXTURE_TMPL_H
#define PSCF_MIXTURE_TMPL_H

/*
* PFTS - Polymer Field Theory Simulator
*
* Copyright 2013, David Morse (morse012@.umn.edu)
* Distributed under the terms of the GNU General Public License.
*/

#include <pscf/Monomer.h>
#include <util/param/ParamComposite.h>
#include <util/containers/DArray.h>

namespace Pscf
{

   using namespace Util;

   /**
   * Template for classes that represent an entire SCFT system.
   */
   template <class TP, class TS>
   class MixtureTmpl : public ParamComposite
   {
   public:

      // Typedefs

      /// Polymer species type.
      typedef TP Polymer;

      /// Solvent species type.
      typedef TS Solvent;

      /// Polymer species type.
      typedef typename TP::Propagator Propagator;

      /// Chemical potential field.
      typedef typename TP::Propagator::WField WField;

      /// Monomer concentration field.
      typedef typename TP::Propagator::CField CField;

      /**
      * Read parameters from file and initialize.
      */
      virtual void readParameters(std::istream& in);

      /**
      * Compute ideal gas properties for all species.
      */
      virtual void compute();

      /**
      * Get a Monomer type descriptor.
      *
      * \param id integer monomer type index
      */
      Monomer& monomer(int id);

      /**
      * Get a polymer object.
      *
      * \param id integer polymer species index
      */
      Polymer& polymer(int id);

      /**
      * Set a solvent solver object.
      *
      * \param id integer solvent species index
      */
      Solvent& solvent(int id);

      /**
      * Return chemical potential field for a specific monomer type.
      *
      * \param monomerId integer monomer type index
      */
      WField& wField(int monomerId);

      /**
      * Array of all chemical potential fields.
      */
      DArray<WField>& wFields();

      /**
      * Return concentration field for specific monomer type.
      *
      * \param monomerId integer monomer type index
      */
      CField& cField(int id);

      /**
      * Get number of monomers.
      */
      int nMonomer() const;

      /**
      * Get number of polymer species.
      */
      int nPolymer() const;

      /**
      * Get number of solvent (point particle) species.
      */
      int nSolvent() const;

   private:

      /**
      * Array of monomer type descriptors.
      */
      DArray<Monomer> monomers_;

      /**
      * Array of chemical potential fields for monomer types.
      *
      * Indexed by monomer typeId, size = nMonomer.
      */
      DArray<WField> wFields_;

      /**
      * Array of concentration fields for monomer types.
      *
      * Indexed by monomer typeId, size = nMonomer.
      */
      DArray<CField> cFields_;

      /**
      * Array of polymer species objects.
      *
      * Size = nPolymer.
      */
      DArray<Polymer> polymers_;

      /**
      * Array of solvent species objects.
      *
      * Size = nSolvent.
      */
      DArray<Solvent> solvents_;

      /**
      * Number of monomer types.
      */
      int nMonomer_; 

      /**
      * Number of polymer species.
      */
      int nPolymer_;

      /**
      * Number of solvent species.
      */
      int nSolvent_;

   };

   // Inline member functions

   template <class TP, class TS>
   inline int MixtureTmpl<TP,TS>::nMonomer() const
   {  return nMonomer_; }

   template <class TP, class TS>
   inline int MixtureTmpl<TP,TS>::nPolymer() const
   {  return nPolymer_; }

   template <class TP, class TS>
   inline int MixtureTmpl<TP,TS>::nSolvent() const
   {  return nSolvent_; }

   template <class TP, class TS>
   inline Monomer& MixtureTmpl<TP,TS>::monomer(int id)
   {  return monomers_[id]; }

   template <class TP, class TS>
   inline TP& MixtureTmpl<TP,TS>::polymer(int id)
   {  return polymers_[id]; }

   template <class TP, class TS>
   inline TS& MixtureTmpl<TP,TS>::solvent(int id)
   {  return solvents_[id]; }

   template <class TP, class TS>
   inline 
   DArray< typename MixtureTmpl<TP,TS>::WField >& 
   MixtureTmpl<TP,TS>::wFields()
   {  return wFields_; }

   template <class TP, class TS>
   inline typename MixtureTmpl<TP,TS>::WField& MixtureTmpl<TP,TS>::wField(int id)
   {  return wFields_[id]; }

   template <class TP, class TS>
   inline typename MixtureTmpl<TP,TS>::CField& MixtureTmpl<TP,TS>::cField(int id)
   {  return cFields_[id]; }

   // Non-inline member functions

   /*
   * Read all parameters and initialize.
   */
   template <class TP, class TS>
   void MixtureTmpl<TP,TS>::readParameters(std::istream& in)
   {
      // Monomers
      read<int>(in, "nMonomer", nMonomer_);
      monomers_.allocate(nMonomer_);
      readDArray< Monomer >(in, "monomers", monomers_, nMonomer_);

      // Allocate arrays of per-monomer fields
      wFields_.allocate(nMonomer_);
      cFields_.allocate(nMonomer_);

      // Polymers 
      read<int>(in, "nPolymer", nPolymer_);
      polymers_.allocate(nPolymer_);
      for (int i = 0; i < nPolymer_; ++i) {
         readParamComposite(in, polymers_[i]);
      }

      // Set kuhn lengths for all blocks
      double kuhn;
      int monomerId;
      for (int i = 0; i < nPolymer_; ++i) {
         for (int j = 0; j < polymer(i).nBlock(); ++j) {
            monomerId = polymer(i).block(j).monomerId();
            kuhn = monomer(monomerId).step();
            polymer(i).block(j).setKuhn(kuhn);
         }
      }
   }

   /*
   * Solve MDE, compute concentrations and free energy.
   */
   template <class TP, class TS>
   void MixtureTmpl<TP,TS>::compute()
   {
      for (int i = 0; i < nPolymer_; ++i) {
         polymer(i).compute(wFields_);
      }
   }

}
#endif
