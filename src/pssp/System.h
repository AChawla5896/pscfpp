#ifndef PSSP_SYSTEM_H
#define PSSP_SYSTEM_H

/*
* PSCF - Polymer Self-Consistent Field Theory
*
* Copyright 2016, The Regents of the University of Minnesota
* Distributed under the terms of the GNU General Public License.
*/

#include <util/param/ParamComposite.h>     // base class
#include <pssp/solvers/Mixture.h>          // member
#include <pscf/mesh/Mesh.h>                // member
#include <pscf/unitCell/UnitCell.h>        // member
#include <pscf/homogeneous/Mixture.h>      // member
#include <util/misc/FileMaster.h>          // member
#include <util/containers/DArray.h>        // member template
#include <util/containers/Array.h>         // function parameter

namespace Pscf {

   class Interaction;

namespace Pssp
{

   class Iterator;
   class Sweep;
   class SweepFactory;
   using namespace Util;

   /**
   * Main class in SCFT simulation of one system.
   *
   * \ingroup Pscf_Pssp_Module
   */
   template <int D>
   class System : public ParamComposite
   {

   public:

      /// Base class for WField and CField
      typedef DArray<double> Field;

      /// Monomer chemical potential field type.
      typedef Propagator<D>::WField WField;

      /// Monomer concentration / volume fraction field type.
      typedef Propagator<D>::CField CField;

      /**
      * Constructor.
      */
      System();

      /**
      * Destructor.
      */
      ~System();

      /// \name Lifetime (Actions)
      //@{

      /**
      * Process command line options.
      */
      void setOptions(int argc, char **argv);

      /**
      * Read input parameters (with opening and closing lines).
      *
      * \param in input parameter stream
      */
      virtual void readParam(std::istream& in);

      /**
      * Read input parameters from default param file.
      */
      void readParam();

      /**
      * Read input parameters (without opening and closing lines).
      *
      * \param in input parameter stream
      */
      virtual void readParameters(std::istream& in);

      /**
      * Read command script.
      * 
      * \param in command script file.
      */
      void readCommands(std::istream& in);

      /**
      * Read commands from default command file.
      */
      void readCommands();

      /**
      * Compute free energy density and pressure for current fields.
      *
      * This function should be called after a successful call of
      * iterator().solve(). Resulting values are returned by the 
      * freeEnergy() and pressure() accessor functions.
      */
      void computeFreeEnergy();

      /**
      * Output thermodynamic properties to a file. 
      *
      * This function outputs Helmholtz free energy per monomer,
      * pressure (in units of kT per monomer volume), and the
      * volume fraction and chemical potential of each species.
      *
      * \param out output stream 
      */
      void outputThermo(std::ostream& out);

      //@}
      /// \name Fields
      //@{

      /**
      * Get array of all chemical potential fields.
      *
      * The array capacity is equal to the number of monomer types.
      */
      DArray<WField>& wFields();

      /**
      * Get chemical potential field for a specific monomer type.
      *
      * \param monomerId integer monomer type index
      */
      WField& wField(int monomerId);

      /**
      * Get array of all chemical potential fields.
      *
      * The array capacity is equal to the number of monomer types.
      */
      DArray<CField>& cFields();

      /**
      * Get chemical potential field for a specific monomer type.
      *
      * \param monomerId integer monomer type index
      */
      CField& cField(int monomerId);

      /**
      * Read chemical potential fields from file.
      *
      * \param in input stream (i.e., input file)
      */
      void readWFields(std::istream& in);

      /**
      * Write concentration or chemical potential fields to file.
      *
      * \param out output stream (i.e., output file)
      * \param fields array of fields for different species
      */
      void writeFields(std::ostream& out, Array<Field> const & fields);

      //@}
      /// \name Accessors (get objects by reference)
      //@{

      /**
      * Get Mixture by reference.
      */
      Mixture& mixture();

      /**
      * Get spatial discretization mesh by reference.
      */
      Mesh<D>& mesh();

      /**
      * Get spatial unitCell (including grid info) by reference.
      */
      UnitCell<D>& unitCell();

      /**
      * Get interaction (i.e., excess free energy model) by reference.
      */
      Interaction& interaction();

      /**
      * Get the Iterator by reference.
      */
      Iterator& iterator();

      /**
      * Get homogeneous mixture (for reference calculations).
      */
      Homogeneous::Mixture& homogeneous();

      /**
      * Get FileMaster by reference.
      */
      FileMaster& fileMaster();

      /**
      * Get precomputed Helmoltz free energy per monomer / kT.
      *
      * The value retrieved by this function is computed by the
      * computeFreeEnergy() function.
      */
      double fHelmholtz() const;

      /**
      * Get precomputed pressure x monomer volume kT.
      *
      * The value retrieved by this function is computed by the
      * computeFreeEnergy() function.
      */
      double pressure() const;

      //@}

   private:

      /**
      * Mixture object (solves MDE for all species).
      */
      Mixture<D> mixture_;

      /**
      * Spatial discretization mesh.
      */
      Mesh<D> mesh_;

      /**
      * Spatial unitCell and grid definition.
      */
      UnitCell<D> unitCell_;

      /**
      * Filemaster (holds paths to associated I/O files).
      */
      FileMaster fileMaster_;

      /**
      * Homogeneous mixture, for reference.
      */
      Homogeneous::Mixture homogeneous_;

      /**
      * Pointer to Interaction (excess free energy model).
      */
      Interaction* interactionPtr_;

      /**
      * Pointer to associated iterator.
      */
      Iterator* iteratorPtr_;

      /**
      * Pointer to associated Sweep object
      */
      Sweep* sweepPtr_;

      /**
      * Pointer to associated Sweep object
      */
      SweepFactory* sweepFactoryPtr_;

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
      * Work array (size = # of grid points).
      */
      DArray<double> f_;

      /**
      * Work array (size = # of monomer types).
      */
      DArray<double> c_;

      /**
      * Helmholtz free energy per monomer / kT.
      */
      double fHelmholtz_;

      /**
      * Pressure times monomer volume / kT.
      */
      double pressure_;

      /**
      * Has the mixture been initialized?
      */
      bool hasMixture_;

      /**
      * Has the Mesh been initialized?
      */
      bool hasMesh_;

      /**
      * Has the UnitCell been initialized?
      */
      bool hasUnitCell_;

      /**
      * Have initial chemical potential fields been read from file?
      */
      bool hasFields_;

      /**
      * Does this system have a Sweep object?
      */
      bool hasSweep_;

      /**
      * Allocate memory for fields (private)
      */
      void allocateFields();

      /**
      * Initialize Homogeneous::Mixture object.
      */
      void initHomogeneous();

   };

   // Inline member functions

   /*
   * Get the associated Mixture object.
   */
   template <int D>
   inline Mixture<D>& System<D>::mixture()
   { return mixture_; }

   /*
   * Get the mesh.
   */
   template <int D>
   inline Mesh<D>& System<D>::mesh()
   { return mesh_; }

   /*
   * Get the UnitCell<D>.
   */
   template <int D>
   inline UnitCell<D>& System<D>::unitCell()
   { return unitCell_; }

   /*
   * Get the FileMaster.
   */
   template <int D>
   inline FileMaster& System<D>::fileMaster()
   {  return fileMaster_; }

   /*
   * Get the Homogeneous::Mixture object.
   */
   template <int D>
   inline 
   Homogeneous::Mixture& System<D>::homogeneous()
   {  return homogeneous_; }

   /*
   * Get the Interaction (excess free energy model).
   */
   template <int D>
   inline Interaction& System<D>::interaction()
   {
      UTIL_ASSERT(interactionPtr_);
      return *interactionPtr_;
   }

   /*
   * Get the Iterator (excess free energy model).
   */
   template <int D>
   inline Iterator& System<D>::iterator()
   {
      UTIL_ASSERT(iteratorPtr_);
      return *iteratorPtr_;
   }

   /*
   * Get an array of all monomer excess chemical potential fields.
   */
   template <int D>
   inline 
   DArray< System<D>::WField >& System<D>::wFields()
   {  return wFields_; }

   /*
   * Get a single monomer excess chemical potential field.
   */
   template <int D>
   inline 
   System<D>::WField& System<D>::wField(int id)
   {  return wFields_[id]; }

   /*
   * Get array of all monomer concentration fields.
   */
   template <int D>
   inline
   DArray< System<D>::CField >& System<D>::cFields()
   {  return cFields_; }

   /*
   * Get a single monomer concentration field.
   */
   template <int D>
   inline System<D>::CField& System<D>::cField(int id)
   {  return cFields_[id]; }

   /*
   * Get precomputed Helmoltz free energy per monomer / kT.
   */
   template <int D>
   inline double System<D>::fHelmholtz() const
   {  return fHelmholtz_; }

   /*
   * Get precomputed pressure (units of kT / monomer volume).
   */
   template <int D>
   inline double System<D>::pressure() const
   {  return pressure_; }

} // namespace Pssp
} // namespace Pscf
#endif
