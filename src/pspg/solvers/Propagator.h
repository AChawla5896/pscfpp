#ifndef PSPG_PROPAGATOR_H
#define PSPG_PROPAGATOR_H

/*
* PSCF - Polymer Self-Consistent Field Theory
*
* Copyright 2016 - 2019, The Regents of the University of Minnesota
* Distributed under the terms of the GNU General Public License.
*/

#include <pscf/solvers/PropagatorTmpl.h> // base class template
#include <pspg/field/RDField.h>          // member template
#include <util/containers/DArray.h>      // member template

namespace Pscf { template <int D> class Mesh; }

namespace Pscf { 
namespace Pspg
{ 

   template <int D> class Block;
   using namespace Util;

   /**
   * MDE solver for one-direction of one block.
   *
   * \ingroup Pspg_Solvers_Module
   */
   template <int D>
   class Propagator : public PropagatorTmpl< Propagator<D> >
   {

   public:

      // Public typedefs

      /**
      * Generic field (function of position).
      */ //where is these two used?
      typedef RDField<D> Field;

      /**
      * Chemical potential field type.
      */ //where is these two used?
      typedef RDField<D> WField;

      /**
      * Monomer concentration field type.
      */
      typedef RDField<D> CField;

      /**
      * Propagator q-field type.
      */
      typedef RDField<D> QField;

      // Member functions

      /**
      * Constructor.
      */
      Propagator();

      /**
      * Destructor.
      */
      ~Propagator();

      /**
      * Associate this propagator with a block.
      *
      * \param block associated Block object.
      */ 
      void setBlock(Block<D>& block);

      /**
      * Associate this propagator with a block.
      * 
      * \param ns number of contour length steps
      * \param mesh spatial discretization mesh
      */ 
      void allocate(int ns, const Mesh<D>& mesh);

      /**
      * Solve the modified diffusion equation (MDE) for this block.
      *
      * This function computes an initial QField at the head of this
      * block, and then solves the modified diffusion equation for 
      * the block to propagate from the head to the tail. The initial
      * QField at the head is computed by pointwise multiplication of
      * of the tail QFields of all source propagators.
      */
      void solve();
  
      /**
      * Solve the MDE for a specified initial condition.
      *
      * This function solves the modified diffusion equation for this
      * block with a specified initial condition, which is given by 
      * head parameter of the function. The function is intended for 
      * use in testing.
      *
      * \param head initial condition of QField at head of block
      */
      void solve(const cufftReal * head);
 
      /**
      * Compute and return partition function for the molecule.
      *
      * This function computes the partition function Q for the 
      * molecule as a spatial average of the initial/head Qfield 
      * for this propagator and the final/tail Qfield of its
      * partner. 
      */ 
      double computeQ();

      /**
      * Return q-field at specified step.
      *
      * \param i step index
      */
      const cufftReal* q(int i) const;

      /**
      * Return q-field at beginning of block (initial condition).
      */
      cufftReal* head() const;

      /**
      * Return q-field at end of block.
      */
      const cufftReal* tail() const;

      /**
      * Get the associated Block object by reference.
      */
      Block<D>& block();

      /**
      * Has memory been allocated for this propagator?
      */
      bool isAllocated() const;

      using PropagatorTmpl< Propagator<D> >::nSource;
      using PropagatorTmpl< Propagator<D> >::source;
      using PropagatorTmpl< Propagator<D> >::partner;
      using PropagatorTmpl< Propagator<D> >::setIsSolved;
      using PropagatorTmpl< Propagator<D> >::isSolved;
      using PropagatorTmpl< Propagator<D> >::hasPartner;

   protected:

      /**
      * Compute initial QField at head from tail QFields of sources.
      */
      void computeHead();

   private:

      float innerProduct(const cufftReal* a, const cufftReal* b, int size);

      // new array purely in device
      cufftReal* qFields_d;
      // Workspace
      // removing this. Does not seem to be used anywhere
      //QField work_;

      /// Pointer to associated Block.
      Block<D>* blockPtr_;

      /// Pointer to associated Mesh
      Mesh<D> const * meshPtr_;

      /// Number of contour length steps = # grid points - 1.
      int ns_;

      /// Is this propagator allocated?
      bool isAllocated_;

	  //work array for inner product. Allocated and free-d in con and des
	  cufftReal* d_temp_;
	  cufftReal* temp_;

   };

   // Inline member functions

   /*
   * Return q-field at beginning of block.
   */
   template <int D>
   inline 
   cufftReal* Propagator<D>::head() const
   {  return qFields_d; }

   /*
   * Return q-field at end of block, after solution.
   */
   template <int D>
   inline 
   const cufftReal* Propagator<D>::tail() const
   {  return qFields_d + ((ns_-1) * meshPtr_->size()); }

   /*
   * Return q-field at specified step.
   */
   template <int D>
   inline 
   const cufftReal* Propagator<D>::q(int i) const
   {  return qFields_d + (i * meshPtr_->size()); }

   /*
   * Get the associated Block object.
   */
   template <int D>
   inline 
   Block<D>& Propagator<D>::block()
   {
      assert(blockPtr_);  
      return *blockPtr_; 
   }

   template <int D>
   inline 
   bool Propagator<D>::isAllocated() const
   {  return isAllocated_; }

   /*
   * Associate this propagator with a block and direction
   */
   template <int D>
   inline 
   void Propagator<D>::setBlock(Block<D>& block)
   {  blockPtr_ = &block; }


   #ifndef PSPG_PROPAGATOR_TPP
   // Suppress implicit instantiation
   extern template class Propagator<1>;
   extern template class Propagator<2>;
   extern template class Propagator<3>;
   #endif

}
}

__global__ 
void assignUniformReal(cufftReal* result, cufftReal uniform, int size);

__global__ 
void assignReal(cufftReal* result, const cufftReal* rhs, int size);

__global__ 
void inPlacePointwiseMul(cufftReal* a, const cufftReal* b, int size);

template<unsigned int blockSize>
__global__ void deviceInnerProduct(cufftReal* c, const cufftReal* a,
   const cufftReal* b, int size) {
   //int nThreads = blockDim.x * gridDim.x;
   int startID = blockIdx.x * blockDim.x + threadIdx.x;

   //do all pointwise multiplication
   volatile extern __shared__ cufftReal cache[];
   cufftReal temp = 0;
   temp += a[startID] * b[startID];
   cache[threadIdx.x] = temp;

   __syncthreads();

   if(blockSize >= 512) {
      if (threadIdx.x < 256){
         cache[threadIdx.x] += cache[threadIdx.x + 256];
      }
      __syncthreads();
   }
   if(blockSize >= 256) {
      if (threadIdx.x < 128){
         cache[threadIdx.x] += cache[threadIdx.x + 128];
      }
      __syncthreads();
   }
   if(blockSize >= 128) {
      if (threadIdx.x < 64){
         cache[threadIdx.x] += cache[threadIdx.x + 64];
      }
      __syncthreads();
   }
   //reduce operation
   //256/2 -- needs to be power of two
   //for (int j = blockDim.x / 2; j > 32; j /= 2) {
   //   if (threadIdx.x < j) {
   //      cache[threadIdx.x] += cache[threadIdx.x + j];
   //   }
   //   __syncthreads();
   //}

   if (threadIdx.x < 32) {
      if(blockSize >= 64) cache[threadIdx.x] += cache[threadIdx.x + 32];
      if(blockSize >= 32) cache[threadIdx.x] += cache[threadIdx.x + 16];
      if(blockSize >= 16) cache[threadIdx.x] += cache[threadIdx.x + 8];
      if(blockSize >= 8) cache[threadIdx.x] += cache[threadIdx.x + 4];
      if(blockSize >= 4) cache[threadIdx.x] += cache[threadIdx.x + 2];
      if(blockSize >= 2) cache[threadIdx.x] += cache[threadIdx.x + 1];
   }

   if (threadIdx.x == 0) {
      c[blockIdx.x] = cache[0];
   }
}
//#include "Propagator.tpp" 
#endif
