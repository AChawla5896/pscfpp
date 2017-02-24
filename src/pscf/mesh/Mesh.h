#ifndef PSSP_MESH_H
#define PSSP_MESH_H

/*
* PSCF - Polymer Self-Consistent Field Theory
*
* Copyright 2016, The Regents of the University of Minnesota
* Distributed under the terms of the GNU General Public License.
*/

#include <pscf/math/IntVec.h>

namespace Pscf 
{

   using namespace Util;

   /**
   * Description of a regular grid of points in a periodic domain.
   *
   * The coordinates of a point on a grid form an IntVec<D>, referred
   * to here as a grid position. Each element of a grid position must
   * lie in the range 0 <= position[i] < dimension(i), where i indexes
   * a Cartesian axis, and dimension(i) is the dimension of the grid
   * along axis i.
   *
   * Each grid position is also assigned a non-negative integer rank.
   * Mesh position ranks are ordered sequentially like elements in
   * a multi-dimensional C array, with the last coordinate being the
   * most rapidly varying.
   *
   * \ingroup Pscf_Pssp_Module
   */
   template <int D>
   class Mesh
   {

   public:

      /**
      * Default constructor
      */
      Mesh();

      /**
      * Constructor
      *
      * \param dimensions  IntVec<D> of grid dimensions
      */
      Mesh(const IntVec<D>& dimensions);

      // Compiler default destructor.

      // Compiler copy constructor.

      /**
      * Set the grid dimensions in all directions.
      *
      * \param dimensions  IntVec<D> of grid dimensions.
      */
      void setDimensions(const IntVec<D>& dimensions);

      //void setUnitCell(const UnitCell& unitCell)

      /**
      * Get an IntVec<D> of the grid dimensions.
      */
      IntVec<D> dimensions() const;

      /**
      * Get grid dimension along Cartesian direction i.
      *
      * \param i  index of Cartesian direction 0 <=i < Util::Dimension
      */
      int dimension(int i) const;

      /**
      * Get total number of grid points.
      */
      int size() const;

      /**
      * Get the position IntVec<D> of a grid point with a specified rank.
      *
      * \param rank  integer rank of a grid point.
      * \return IntVec<D> containing coordinates of specified point.
      */
      IntVec<D> position(int rank) const;

      /**
      * Get the rank of a grid point with specified position.
      *
      * \param position  integer position of a grid point
      * \return integer rank of specified grid point
      */
      int rank(const IntVec<D>& position) const;

      /**
      * Is this coordinate in range?
      *
      * \param coordinate  coordinate value for direction i
      * \param i  index for Cartesian direction
      * \return true iff 0 <= coordinate < dimension(i).
      */
      bool isInMesh(int coordinate, int i) const;

      /**
      * Is this IntVec<D> grid position within the grid?
      *
      * Returns true iff 0 <= coordinate[i] < dimension(i) for all i.
      *
      * \param position  grid point position
      * \return true iff 0 <= coordinate[i] < dimension(i) for all i.
      */
      bool isInMesh(IntVec<D>& position) const;

      /**
      * Shift a periodic coordinate into range.
      *
      * Upon return, the coordinate will be shifted to lie within the
      * range 0 <= coordinate < dimension(i) by subtracting an integer
      * multiple of dimension(i), giving coordinate - shift*dimension(i).
      * The return value is the required integer `shift'.
      *
      * \param coordinate  coordinate in Cartesian direction i.
      * \param i  index of Cartesian direction, i >= 0.
      * \return multiple of dimension(i) subtracted from input value.
      */
      int shift(int& coordinate, int i) const;

      /**
      * Shift a periodic position into primary grid.
      *
      * Upon return, each element of the parameter position is shifted
      * to lie within the range 0 <= position[i] < dimension(i) by
      * adding or subtracting an integer multiple of dimension(i). The
      * IntVec<D> of shift values is returned.
      *
      * \param position  IntVec<D> position within a grid.
      * \return IntVec<D> of integer shifts.
      */
      IntVec<D> shift(IntVec<D>& position) const;

   private:

      /// Dimensions of grid
      IntVec<D> dimensions_;

      /// Number of elements per increment in each direction
      IntVec<D> offsets_;

      /// Total number of grid points
      int size_;

   };

   // Inline member functions

   template <int D>
   inline IntVec<D> Mesh<D>::dimensions() const
   {  return dimensions_; }

   template <int D>
   inline int Mesh<D>::dimension(int i) const
   {
      assert(i >=0);
      assert(i < Dimension);
      return dimensions_[i];
   }

   template <int D>
   inline int Mesh<D>::size() const
   {  return size_; }

}
#include "Mesh.tpp"
#endif
