#ifndef PSSP_R_FIELD_DFT_H
#define PSSP_R_FIELD_DFT_H

/*
* PSCF++ Package 
*
* Copyright 2010 - 2017, The Regents of the University of Minnesota
* Distributed under the terms of the GNU General Public License.
*/

#include "Field.h"
#include <pscf/math/IntVec.h>
#include <util/global.h>

#include <fftw3.h>

namespace Pssp
{

   using namespace Util;
   using namespace Pscf;

   /**
   * Field of real double precision values on an FFT mesh.
   */
   template <int D>
   class RFieldDFT : public Field<fftw_complex>
   {

   public:

      /**
      * Default constructor.
      */
      RFieldDFT();

      /**
      * Copy constructor.
      *
      * Allocates new memory and copies all elements by value.
      *
      *\param other the RFieldDFT to be copied.
      */
      RFieldDFT(const RFieldDFT<D>& other);

      /**
      * Destructor.
      *
      * Deletes underlying C array, if allocated previously.
      */
      virtual ~RFieldDFT();

      /**
      * Assignment operator.
      *
      * If this Field is not allocated, allocates and copies all elements.
      *
      * If this and the other Field are both allocated, the capacities must
      * be exactly equal. If so, this method copies all elements.
      *
      * \param other the RHS Field
      */
      RFieldDFT<D>& operator = (const RFieldDFT<D>& other);

      using Field<fftw_complex>::allocate;

      /**
      * Allocate the underlying C array for an FFT grid.
      *
      * \throw Exception if the RFieldDFT is already allocated.
      *
      * \param dimensions vector containing number of grid points in each direction.
      */
      void allocate(const IntVec<D>& meshDimensions);

      /**
      * Get the dimensions of the grid for which this was allocated.
      *
      * \throw Exception if dimensions of space do not match.
      *
      * \param dimensions vector containing number of grid points in each direction.
      */
      void getMeshDimensions(IntVec<D>& meshDimensions) const;

      /**
      * Serialize a Field to/from an Archive.
      *
      * \param ar       archive
      * \param version  archive version id
      */
      template <class Archive>
      void serialize(Archive& ar, const unsigned int version);

   private:

      // Vector containing number of grid points in each direction.
      IntVec<D> meshDimensions_;

   };

   /*
   * Allocate the underlying C array for an FFT grid.
   */
   template <int D>
   void RFieldDFT<D>::allocate(const IntVec<D>& meshDimensions)
   {
      int size = 1;
      for (int i = 0; i < D; ++i) {
         UTIL_CHECK(meshDimensions[i] > 0);
         meshDimensions_[i] = meshDimensions[i];
         if (i < D - 1) {
            size *= meshDimensions[i];
         } else {
            size *= (meshDimensions[i]/2 + 1);
         }
      }
      Field<fftw_complex>::allocate(size);
   }

   /*
   * Get the dimensions of the grid for which this was allocated.
   */
   template <int D>
   void RFieldDFT<D>::getMeshDimensions(IntVec<D>& meshDimensions) const
   {
      for (int i = 0; i < D; ++i) {
         meshDimensions[i] = meshDimensions_[i];
      }
   }

   /*
   * Serialize a Field to/from an Archive.
   */
   template <int D>
   template <class Archive>
   void RFieldDFT<D>::serialize(Archive& ar, const unsigned int version)
   {
      Field<fftw_complex>::serialize(ar, version);
      ar & meshDimensions_;
   }

}
#include "RFieldDFT.tpp"
#endif
