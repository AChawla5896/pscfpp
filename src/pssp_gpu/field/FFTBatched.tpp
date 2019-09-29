#ifndef FFT_GPU_BATCHED_TPP
#define FFT_GPU_BATCHED_TPP

/*
* PSCF++ Package 
*
* Copyright 2010 - 2017, The Regents of the University of Minnesota
* Distributed under the terms of the GNU General Public License.
*/

#include "FFTBatched.h"
#include <pssp_gpu/GpuResources.h>

static __global__ void scaleComplexData(cufftComplex* data, cufftReal scale, int size) {
   
   //write code that will scale
   int nThreads = blockDim.x * gridDim.x;
   int startId = blockIdx.x * blockDim.x + threadIdx.x;
   for(int i = startId; i < size; i += nThreads ) {
      data[i].x *= scale;
      data[i].y *= scale;
   }
   
}

namespace Pscf {
namespace Pssp_gpu
{

   using namespace Util;

   /*
   * Default constructor.
   */
   template <int D>
   FFTBatched<D>::FFTBatched()
    : meshDimensions_(0),
      rSize_(0),
      kSize_(0),
      fPlan_(0),
      iPlan_(0),
      isSetup_(false)
   {}

   /*
   * Destructor.
   */
   template <int D>
   FFTBatched<D>::~FFTBatched()
   {
      if (fPlan_) {
         cufftDestroy(fPlan_);
      }
      if (iPlan_) {
         cufftDestroy(iPlan_);
      }
   }

   /*
   * Check and (if necessary) setup mesh dimensions.
   */
   template <int D>
   void FFTBatched<D>::setup(RDField<D>& rField, RDFieldDft<D>& kField)
   {
      // Preconditions
      UTIL_CHECK(!isSetup_);
      IntVec<D> rDimensions = rField.meshDimensions();
      IntVec<D> kDimensions = kField.meshDimensions();
      UTIL_CHECK(rDimensions == kDimensions);

      // Set Mesh dimensions
      rSize_ = 1;
      kSize_ = 1;
      for (int i = 0; i < D; ++i) {
         UTIL_CHECK(rDimensions[i] > 0);
         meshDimensions_[i] = rDimensions[i];
         rSize_ *= rDimensions[i];
         if (i < D - 1) {
            kSize_ *= rDimensions[i];
         } else {
            kSize_ *= (rDimensions[i]/2 + 1);
         }
      }

      //turned off to allow batch solution of MDE
      //UTIL_CHECK(rField.capacity() == rSize_);
      //UTIL_CHECK(kField.capacity() == kSize_);


      // Make FFTW plans (explicit specializations)
      makePlans(rField, kField);

      isSetup_ = true;
   }

   template <int D>
   void FFTBatched<D>::setup(const IntVec<D>& rDim, const IntVec<D>& kDim, int batchSize)
   {
      // Preconditions
      UTIL_CHECK(!isSetup_);

      // Set Mesh dimensions
      rSize_ = 1;
      kSize_ = 1;
      for (int i = 0; i < D; ++i) {
         UTIL_CHECK(rDim[i] > 0);
         meshDimensions_[i] = rDim[i];
         rSize_ *= rDim[i];
         if (i < D - 1) {
            kSize_ *= rDim[i];
         } else {
            kSize_ *= (rDim[i]/2 + 1);
         }
      }

      //turned off to allow batch solution of MDE
      //UTIL_CHECK(rField.capacity() == rSize_);
      //UTIL_CHECK(kField.capacity() == kSize_);


      // Make FFTW plans (explicit specializations)
      makePlans(rDim, kDim, batchSize);

      isSetup_ = true;
   }

   /*
   * Execute forward transform.
   */
   template <int D>
   void FFTBatched<D>::forwardTransform(RDField<D>& rField, RDFieldDft<D>& kField)
   {
      // Check dimensions or setup
      if (isSetup_) {
         UTIL_CHECK(rField.capacity() == 2*rSize_);
         UTIL_CHECK(kField.capacity() == 2*kSize_);
      } else {
         //UTIL_CHECK(0);
         //should never reach here in a parallel block. breaks instantly
         setup(rField, kField);
      }

      // Copy rescaled input data prior to work array
      rtype scale = 1.0/rtype(rSize_);
      //scale for every batch
      scaleRealData<<<NUMBER_OF_BLOCKS, THREADS_PER_BLOCK>>>(rField.cDField(), scale, rSize_ * 2);
      
      //perform fft
      #ifdef SINGLE_PRECISION
      if(cufftExecR2C(fPlan_, rField.cDField(), kField.cDField()) != CUFFT_SUCCESS) {
         std::cout<<"CUFFT error: forward"<<std::endl;
         return;
      }
      #else
      if(cufftExecD2Z(fPlan_, rField.cDField(), kField.cDField()) != CUFFT_SUCCESS) {
         std::cout<<"CUFFT error: forward"<<std::endl;
         return;
      }
      #endif

   }

   /*
   * Execute forward transform.
   */
   //can i just corrupt the rfield data?
   template <int D>
   void FFTBatched<D>::forwardTransform
   (cufftReal* rField, cufftComplex* kField, int batchSize)
   {
      // Check dimensions or setup
      if (isSetup_) {
      } else {
         std::cerr<<"Need to setup before running transform"<<std::endl;
         exit(1);
      }

      // Copy rescaled input data prior to work array
      rtype scale = 1.0/rtype(rSize_);
      //scale for every batch
      scaleRealData<<<NUMBER_OF_BLOCKS, THREADS_PER_BLOCK>>>(rField, scale, rSize_ * batchSize);
      
      //perform fft
      #ifdef SINGLE_PRECISION
      if(cufftExecR2C(fPlan_, rField, kField) != CUFFT_SUCCESS) {
         std::cout<<"CUFFT error: forward"<<std::endl;
         return;
      }
      //scaleComplexData<<<NUMBER_OF_BLOCKS, THREADS_PER_BLOCK>>>(kField, scale, kSize_ * batchSize);
      #else
      if(cufftExecD2Z(fPlan_, rField, kField) != CUFFT_SUCCESS) {
         std::cout<<"CUFFT error: forward"<<std::endl;
         return;
      }
      #endif

   }

   /*
   * Execute inverse (complex-to-real) transform.
   */
   template <int D>
   void FFTBatched<D>::inverseTransform(RDFieldDft<D>& kField, RDField<D>& rField)
   {
      if (!isSetup_) {
         //UTIL_CHECK(0);
         setup(rField, kField);
      }

      #ifdef SINGLE_PRECISION
      if(cufftExecC2R(iPlan_, kField.cDField(), rField.cDField()) != CUFFT_SUCCESS) {
         std::cout<<"CUFFT error: inverse"<<std::endl;
         return;
      }
      #else
      if(cufftExecZ2D(iPlan_, kField.cDField(), rField.cDField()) != CUFFT_SUCCESS) {
         std::cout<<"CUFFT error: inverse"<<std::endl;
         return;
      }
      #endif
   
   }


   /*
   * Execute inverse (complex-to-real) transform.
   */
   template <int D>
   void FFTBatched<D>::inverseTransform
   (cufftComplex* kField, cufftReal* rField, int batchSize)
   {
      if (!isSetup_) {
         std::cerr<<"Need to setup before running FFTbatchec"<<std::endl;
      }

      #ifdef SINGLE_PRECISION
      if(cufftExecC2R(iPlan_, kField, rField) != CUFFT_SUCCESS) {
         std::cout<<"CUFFT error: inverse"<<std::endl;
         return;
      }
      #else
      if(cufftExecZ2D(iPlan_, kField, rField) != CUFFT_SUCCESS) {
         std::cout<<"CUFFT error: inverse"<<std::endl;
         return;
      }
      #endif
   
   }

}
}



#endif
