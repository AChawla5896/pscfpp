#ifndef PSSP_FIELD_IO_H
#define PSSP_FIELD_IO_H

/*
* PSCF - Polymer Self-Consistent Field Theory
*
* Copyright 2016 - 2019, The Regents of the University of Minnesota
* Distributed under the terms of the GNU General Public License.
*/

#include <pssp/solvers/Mixture.h>          // member
#include <pssp/field/FFT.h>                // member
#include <pssp/basis/Basis.h>              // member
#include <pssp/field/RField.h>             // function parameter
#include <pssp/field/RFieldDft.h>          // function parameter

#include <pscf/crystal/UnitCell.h>         // member
#include <pscf/mesh/Mesh.h>                // member

#include <util/misc/FileMaster.h>          // member
#include <util/containers/DArray.h>        // function parameter
#include <util/containers/Array.h>         // function parameter

namespace Pscf {
namespace Pssp
{
   using namespace Util;
   using namespace Pscf;

   /**
   * File input/output operations for fields in several file formats.
   *
   * \ingroup Pssp_Field_Module
   */
   template <int D>
   class FieldIo 
   {

   public:

      /**
      * Constructor.
      */
      FieldIo();

      /**
      * Destructor.
      */
      ~FieldIo();

      /**
      * Get and store addresses of associated objects.
      *
      * \param mixture  associated Mixture<D> object (solves MDEs)
      * \param unitCell associated crystallographic UnitCell<D>
      * \param mesh  associated spatial discretization Mesh<D>
      * \param fft   associated FFT object for fast transforms
      * \param groupName space group name string
      * \param basis  associated Basis object
      * \param fileMaster  associated FileMaster (for file paths)
      */
      void associate(Mixture<D>& mixture,
                     UnitCell<D>& unitCell,
                     Mesh<D>& mesh,
                     FFT<D>& fft,
                     std::string& groupName,
                     Basis<D>& basis,
                     FileMaster& fileMaster);

      /**
      * Read concentration or chemical potential field components from file.
      *
      * This function reads components in a symmetry adapted basis from 
      * file in.
      *
      * The capacity of DArray fields is equal to nMonomer, and element
      * fields[i] is a DArray containing components of the field 
      * associated with monomer type i.
      *
      * \param in input stream (i.e., input file)
      * \param fields array of fields (symmetry adapted basis components)
      */
      void 
      readFieldsBasis(std::istream& in, DArray< DArray <double> >& fields);

      /**
      * Read concentration or chemical potential field components from file.
      *
      * This function opens an input file with the specified filename, 
      * reads components in symmetry-adapted form from that file, and 
      * closes the file.
      *
      * \param filename name of input file
      * \param fields array of fields (symmetry adapted basis components)
      */
      void readFieldsBasis(std::string filename, 
                           DArray< DArray <double> >& fields);

      /**
      * Write concentration or chemical potential field components to file.
      *
      * This function writes components in a symmetry adapted basis.
      *
      * \param out output stream (i.e., output file)
      * \param fields array of fields (symmetry adapted basis components)
      */
      void writeFieldsBasis(std::ostream& out, 
                            DArray< DArray <double> > const & fields);

      /**
      * Write concentration or chemical potential field components to file.
      *
      * This function opens an output file with the specified filename, 
      * writes components in symmetry-adapted form to that file, and then
      * closes the file. 
      *
      * \param filename name of input file
      * \param fields array of fields (symmetry adapted basis components)
      */
      void writeFieldsBasis(std::string filename, 
                            DArray< DArray <double> > const & fields);

      /**
      * Read array of RField objects (fields on an r-space grid) from file.
      *
      * The capacity of array fields is equal to nMonomer, and element
      * fields[i] is the RField<D> associated with monomer type i.
      * 
      * \param in input stream (i.e., input file)
      * \param fields array of RField fields (r-space grid)
      */
      void readFieldsRGrid(std::istream& in, DArray< RField<D> >& fields);

      /**
      * Read array of RField objects (fields on an r-space grid) from file.
      *
      * The capacity of array fields is equal to nMonomer, and element
      * fields[i] is the RField<D> associated with monomer type i.
      * 
      * This function opens an input file with the specified filename, 
      * reads fields in RField<D> real-space grid format from that file, 
      * and then closes the file. 
      *
      * \param filename name of input file
      * \param fields array of RField fields (r-space grid)
      */
      void readFieldsRGrid(std::string filename, DArray< RField<D> >& fields);

      /**
      * Write array of RField objects (fields on an r-space grid) to file.
      *
      * \param out output stream (i.e., output file)
      * \param fields array of RField fields (r-space grid)
      */
      void writeFieldsRGrid(std::ostream& out, 
                            DArray< RField<D> > const& fields);

      /**
      * Write array of RField objects (fields on an r-space grid) to file.
      *
      * This function opens an output file with the specified filename, 
      * writes fields in RField<D> real-space grid format to that file, 
      * and then closes the file.
      *
      * \param filename  name of output file
      * \param fields  array of RField fields (r-space grid)
      */
      void writeFieldsRGrid(std::string filename,
                            DArray< RField<D> > const& fields);

      /**
      * Read array of RFieldDft objects (k-space fields) from file.
      *
      * The capacity of the array is equal to nMonomer, and element
      * fields[i] is the discrete Fourier transform of the field for 
      * monomer type i.
      * 
      * \param in  input stream (i.e., input file)
      * \param fields  array of RFieldDft fields (k-space grid)
      */
      void readFieldsKGrid(std::istream& in, 
                           DArray< RFieldDft<D> >& fields);

      /**
      * Read array of RFieldDft objects (k-space fields) from file.
      *
      * This function opens a file with name filename, reads discrete
      * Fourier components (Dft) of fields from that file, and closes 
      * the file. 
      *
      * The capacity of the array is equal to nMonomer, and element
      * fields[i] is the discrete Fourier transform of the field for 
      * monomer type i.
      * 
      * \param filename  name of input file
      * \param fields  array of RFieldDft fields (k-space grid)
      */
      void readFieldsKGrid(std::string filename, 
                           DArray< RFieldDft<D> >& fields);

      /**
      * Write array of RFieldDft objects (k-space fields) to file.
      *
      * The capacity of the array fields is equal to nMonomer. Element
      * fields[i] is the discrete Fourier transform of the field for 
      * monomer type i.
      * 
      * \param out output stream (i.e., output file)
      * \param fields array of RFieldDft fields 
      */
      void writeFieldsKGrid(std::ostream& out, 
                            DArray< RFieldDft<D> > const& fields);
   
      /**
      * Write array of RFieldDft objects (k-space fields) to a file.
      *
      * This function opens a file with name filename, writes discrete
      * Fourier transform components (DFT) components of fields to that 
      * file, and closes the file. 
      *
      * \param filename  name of output file.
      * \param fields  array of RFieldDft fields (k-space grid)
      */
      void writeFieldsKGrid(std::string filename, 
                           DArray< RFieldDft<D> > const& fields);

      /**
      * Write header for field file (fortran pscf format)
      *
      * \param out output stream (i.e., output file)
      */
      void writeFieldHeader(std::ostream& out) const;

   private:

      // Pointers to associated objects.

      /// Pointer to mixture object (solves MDE for all species).
      Mixture<D>* mixturePtr_;

      /// Pointer to crystallographic unit cell.
      UnitCell<D>* unitCellPtr_;

      /// Pointer to spatial discretization mesh.
      Mesh<D>* meshPtr_;

      /// Pointer to FFT object.
      FFT<D>* fftPtr_;

      /// Pointer to group name string
      std::string* groupNamePtr_;

      /// Pointer to a Basis object
      Basis<D>* basisPtr_;

      /// Pointer to Filemaster (holds paths to associated I/O files).
      FileMaster* fileMasterPtr_;

      // Private accessor functions:

      /// Get Mixture by reference.
      Mixture<D>& mixture()
      {  return *mixturePtr_; }

      /// Get Mixture by const reference.
      Mixture<D> const& mixture() const
      {  return *mixturePtr_; }

      /// Get UnitCell by reference.
      UnitCell<D>& unitCell()
      {  return *unitCellPtr_; }

      /// Get UnitCell by const reference.
      UnitCell<D> const & unitCell() const
      {  return *unitCellPtr_; }

      /// Get spatial discretization mesh by reference.
      Mesh<D>& mesh()
      {  return *meshPtr_; }

      /// Get spatial discretization mesh by const reference.
      Mesh<D> const & mesh() const
      {  return *meshPtr_; }

      /// Get FFT object by reference.
      FFT<D>& fft()
      {  return *fftPtr_; }

      /// Get group name string by reference.
      std::string& groupName()
      {  return *groupNamePtr_; }

      /// Get group name string by const reference.
      std::string const & groupName() const
      {  return *groupNamePtr_; }
      /// Get Basis by reference.
      Basis<D>& basis()
      {  return *basisPtr_; }

      /// Get FileMaster by reference.
      FileMaster& fileMaster()
      {  return *fileMasterPtr_; }

      /**
      * Reader header of field file (fortran pscf format)
      *
      * \param in input stream (i.e., input file)
      */
      void readFieldHeader(std::istream& in);

   };

   #ifndef PSSP_FIELD_IO_TPP
   extern template class FieldIo<1>;
   extern template class FieldIo<2>;
   extern template class FieldIo<3>;
   #endif

} // namespace Pssp
} // namespace Pscf
#endif
