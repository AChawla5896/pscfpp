#ifndef PFTS_BLOCK_H
#define PFTS_BLOCK_H

/*
* PFTS - Polymer Field Theory Simulator
*
* Copyright 2013, David Morse (morse012@.umn.edu)
* Distributed under the terms of the GNU General Public License.
*/

#include <util/containers/Pair.h>

namespace Pfts{ 

   using namespace Util;

   /**
   * A linear homopolymer block within a block copolymer.
   */
   class Block
   {
   public:
  
      /**
      * Constructor.
      */ 
      Block();
   
      /**
      * Set the id for this block.
      *
      * \param id integer index for this block
      */ 
      void setId(unsigned int id);
  
      /**
      * Set indices of associated vertices.
      *
      * \param vertexAId integer id of vertex A
      * \param vertexBId integer id of vertex B
      */ 
      void setVertexIds(int VertexAId, int VertexBId);
  
      /**
      * Set the monomer id.
      *
      * \param monomerId integer id of monomer type (>=0)
      */ 
      void setMonomerId(int monomerId);
  
      /**
      * Set the length of this block.
      *
      * \param length block length (number of monomers).
      */ 
      void setLength(double length);
  
      /**
      * Get the id of this block.
      */ 
      int id() const;
  
      /**
      * Get the monomer type id.
      */ 
      int monomerId() const;
  
      /**
      * Get the pair of associated vertex ids.
      */ 
      const Pair<int>& vertexIds() const;
  
      /**
      * Get the length (number of monomers) in this block.
      */
      double length() const;
    
   private:
   
      int id_;
      int monomerId_;
      Pair<int> vertexIds_;
      double length_;

   };

   /*
   * Get the id of this block.
   */ 
   int Block::id() const
   {  return id_; }

   /*
   * Get the monomer type id.
   */ 
   int Block::monomerId() const
   {  return monomerId_; }

   /*
   * Get the pair of associated vertex ids.
   */ 
   const Pair<int>& Block::vertexIds() const
   {  return vertexIds_; }

   /*
   * Get the length (number of monomers) in this block.
   */
   double Block::length() const
   {  return length_; }
    
} 
#endif 
