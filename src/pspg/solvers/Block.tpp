#ifndef PSPG_BLOCK_TPP
#define PSPG_BLOCK_TPP

/*
* PSCF - Polymer Self-Consistent Field Theory
*
* Copyright 2016 - 2019, The Regents of the University of Minnesota
* Distributed under the terms of the GNU General Public License.
*/

#include "Block.h"
#include <pspg/GpuResources.h>
#include <pscf/mesh/Mesh.h>
#include <pscf/mesh/MeshIterator.h>
#include <pscf/crystal/shiftToMinimum.h>
#include <util/containers/FMatrix.h>      // member template
#include <util/containers/DArray.h>      // member template
#include <util/containers/FArray.h>      // member template
#include <sys/time.h>

//not a bad idea to rewrite these as functors
static __global__ void pointwiseMul(const cufftReal* a, const cufftReal* b, cufftReal* result, int size) {
   int nThreads = blockDim.x * gridDim.x;
   int startID = blockIdx.x * blockDim.x + threadIdx.x;
   for (int i = startID; i < size; i += nThreads) {
      result[i] = a[i] * b[i];
   }
}

static __global__ void pointwiseFloatMul(const cufftReal* a, const float* b, cufftReal* result, int size) {
   int nThreads = blockDim.x * gridDim.x;
   int startID = blockIdx.x * blockDim.x + threadIdx.x;
   for (int i = startID; i < size; i += nThreads) {
      result[i] = a[i] * b[i];
      // printf("result[%d], =  %d\n", i , result[i]);
   }
}

static __global__ void mulDelKsq(cufftReal* result, const cufftComplex* q1,
                                 const cufftComplex* q2, const cufftReal* delKsq,
                                 int paramN, int kSize, int rSize) {
   int nThreads = blockDim.x * gridDim.x;
   int startID = blockIdx.x * blockDim.x + threadIdx.x;
   for (int i = startID; i < kSize; i += nThreads) {
      result[i] =  cuCmulf(q1[i], cuConjf(q2[i])).x * delKsq[paramN * rSize + i];
   }
}

static __global__ void equalize ( const cufftReal* a, float* result, int size){  //try to add elements of array here itself

    int nThreads = blockDim.x * gridDim.x;
    int startID = blockIdx.x * blockDim.x + threadIdx.x;
    for (int i = startID; i < size; i += nThreads) {
       result [i] = a [i];
    }

}

static __global__ void pointwiseMulUnroll2(const cufftReal* a, const cufftReal* b, cufftReal* result, int size) {
   int nThreads = blockDim.x * gridDim.x;
   int startID = blockIdx.x * blockDim.x * 2 + threadIdx.x * 2;
   cufftReal localResult[2];
   for (int i = startID; i < size; i += nThreads * 2) {
      localResult[0] = a[i] * b[i];
      localResult[1] = a[i + 1] * b[i + 1];
      result[i] = localResult[0];
      result[i + 1] = localResult[1];
      //result[i] = a[i] * b[i];
      //result[i + 1] = a[i + 1] * b[i + 1];

   }
}

static __global__ void pointwiseMulCombi(cufftReal* a,const cufftReal* b, cufftReal* c,const cufftReal* d,const cufftReal* e, int size) {
   //c = a * b
   //a = d * e
   int nThreads = blockDim.x * gridDim.x;
   int startID = blockIdx.x * blockDim.x + threadIdx.x;
   cufftReal tempA;
   for (int i = startID; i < size; i += nThreads) {
      tempA = a[i];
      c[i] = tempA * b[i];
      a[i] = d[i] * e[i];

   }
}


static __global__ void pointwiseMulSameStart(const cufftReal* a, const cufftReal* expW,const cufftReal* expW2,  cufftReal* q1, cufftReal* q2, int size) {
   int nThreads = blockDim.x * gridDim.x;
   int startID = blockIdx.x * blockDim.x + threadIdx.x;
   cufftReal input;
   for (int i = startID; i < size; i += nThreads) {
      input = a[i];
      q1[i] = expW[i] * input;
      q2[i] = expW2[i] * input;
   }
}

static __global__ void pointwiseMulTwinned(const cufftReal* qr1, const cufftReal* qr2, const cufftReal* expW, cufftReal* q1, cufftReal* q2, int size) {
   int nThreads = blockDim.x * gridDim.x;
   int startID = blockIdx.x * blockDim.x + threadIdx.x;
   cufftReal scale;
   for (int i = startID; i < size; i += nThreads) {
      scale = expW[i];
      q1[i] = qr1[i] * scale;
      q2[i] = qr2[i] * scale;
   }
}

static __global__ void scaleComplexTwinned(cufftComplex* qk1, cufftComplex* qk2, const cufftReal* expksq1, const cufftReal* expksq2, int size) {
   int nThreads = blockDim.x * gridDim.x;
   int startID = blockIdx.x * blockDim.x + threadIdx.x;
   for (int i = startID; i < size; i += nThreads) {
      qk1[i].x *= expksq1[i];
      qk1[i].y *= expksq1[i];
      qk2[i].x *= expksq2[i];
      qk2[i].y *= expksq2[i];
   }
}

static __global__ void scaleComplex(cufftComplex* a, cufftReal* scale, int size) {
   int nThreads = blockDim.x * gridDim.x;
   int startID = blockIdx.x * blockDim.x + threadIdx.x;
   for(int i = startID; i < size; i += nThreads) {
      a[i].x *= scale[i];
      a[i].y *= scale[i];
   }
}

static __global__ void assignExp(cufftReal* expW, const cufftReal* w, int size, float cDs) {
   int nThreads = blockDim.x * gridDim.x;
   int startID = blockIdx.x * blockDim.x + threadIdx.x;
   for(int i = startID; i < size; i += nThreads) {
      expW[i] = expf(-w[i]*cDs);
   }
}

static __global__ void richardsonExp(cufftReal* qNew, const cufftReal* q1, const cufftReal* q2, int size) {
   int nThreads = blockDim.x * gridDim.x;
   int startID = blockIdx.x * blockDim.x + threadIdx.x;
   for (int i = startID; i < size; i += nThreads) {
      qNew[i] = (4.0 * q2[i] - q1[i]) / 3.0;
   }
}

static __global__ void richardsonExpTwinned(cufftReal* qNew, const cufftReal* q1,
   const cufftReal* qr, const cufftReal* expW2, int size) {
   int nThreads = blockDim.x * gridDim.x;
   int startID = blockIdx.x * blockDim.x + threadIdx.x;
   cufftReal q2;
   for (int i = startID; i < size; i += nThreads) {
      q2 = qr[i] * expW2[i];
      qNew[i] = (4.0 * q2 - q1[i]) / 3.0;
   }
}

namespace Pscf {
namespace Pspg {

   using namespace Util;

static __global__ void multiplyScaleQQ(cufftReal* result,
                           const cufftReal* p1,
                           const cufftReal* p2,
                           int size, float scale) {

   int nThreads = blockDim.x * gridDim.x;
   int startID = blockIdx.x * blockDim.x + threadIdx.x;

   for(int i = startID; i < size; i += nThreads) {
      result[i] += scale * p1[i] * p2[i];
   }

}

static __global__ void scaleReal(cufftReal* result, int size, float scale) {
   int nThreads = blockDim.x * gridDim.x;
   int startID = blockIdx.x * blockDim.x + threadIdx.x;

   for (int i = startID; i < size; i += nThreads) {
      result[i] *= scale;
   }
}
   /*
   * Constructor.
   */
   template <int D>
   Block<D>::Block()
    : meshPtr_(0),
      kMeshDimensions_(0),
      ds_(0.0),
      ns_(0)
   {
      propagator(0).setBlock(*this);
      propagator(1).setBlock(*this);
   }

   /*
   * Destructor.
   */
   template <int D>
   Block<D>::~Block()
   {
      delete[] temp_;
      cudaFree(d_temp_);

      delete[] expKsq_host;
      delete[] expKsq2_host;
   }

   template <int D>
   void Block<D>::setDiscretization(double ds, const Mesh<D>& mesh)
   {
      UTIL_CHECK(mesh.size() > 1);
      UTIL_CHECK(ds > 0.0);

      // Set association to mesh
      meshPtr_ = &mesh;

      // Set contour length discretization
      ns_ = floor(length()/ds + 0.5) + 1;
      if (ns_%2 == 0) {
         ns_ += 1;
      }

      ds_ = length()/double(ns_ - 1);

      // Compute Fourier space kMeshDimensions_
      for (int i = 0; i < D; ++i) {
         if (i < D - 1) {
            kMeshDimensions_[i] = mesh.dimensions()[i];
         } else {
            kMeshDimensions_[i] = mesh.dimensions()[i]/2 + 1;
         }
      }

      kSize_ = 1;
      for(int i = 0; i < D; ++i) {
         kSize_ *= kMeshDimensions_[i];
      }

      // Allocate work arrays
      expKsq_.allocate(kMeshDimensions_);
      expKsq2_.allocate(kMeshDimensions_);
      expW_.allocate(mesh.dimensions());
      expW2_.allocate(mesh.dimensions());
      qr_.allocate(mesh.dimensions());
      qr2_.allocate(mesh.dimensions());
      qk_.allocate(mesh.dimensions());
      qk2_.allocate(mesh.dimensions());
      q1_.allocate(mesh.dimensions());
      q2_.allocate(mesh.dimensions());

      propagator(0).allocate(ns_, mesh);
      propagator(1).allocate(ns_, mesh);
      cudaMalloc((void**)&qkBatched_, ns_ * kSize_ * sizeof(cufftComplex));
      cudaMalloc((void**)&qk2Batched_, ns_ * kSize_ * sizeof(cufftComplex));
      cField().allocate(mesh.dimensions());

      cudaMalloc((void**)&d_temp_, NUMBER_OF_BLOCKS * sizeof(cufftReal));
      temp_ = new cufftReal[NUMBER_OF_BLOCKS];

      expKsq_host = new cufftReal[kSize_];
      expKsq2_host = new cufftReal[kSize_];
   }

   /*
   * Setup data that depend on the unit cell parameters.
   */
   template <int D>
   void
   Block<D>::setupUnitCell(const UnitCell<D>& unitCell, const WaveList<D>& wavelist)
   {
      //what does this do?
      nParams_ = unitCell.nParameter();
      MeshIterator<D> iter;
      // std::cout << "kDimensions = " << kMeshDimensions_ << std::endl;
      iter.setDimensions(kMeshDimensions_);
      IntVec<D> G, Gmin;
      double Gsq;
      double factor = -1.0*kuhn()*kuhn()*ds_/6.0;
      // std::cout << "factor      = " << factor << std::endl;

      //setup expKsq values on Host then transfer to device
      int kSize = 1;
      for(int i = 0; i < D; ++i) {
         kSize *= kMeshDimensions_[i];
      }

      int i;
      for (iter.begin(); !iter.atEnd(); ++iter) {
         i = iter.rank();
         //G = iter.position();
         //Gmin = shiftToMinimum(G, mesh().dimensions(), unitCell);

         /*
         for(int l = 0; l < D; l++) {
            if(Gmin[l] != wavelist.minImage(iter.rank())[l]) {
               std::cout<<Gmin[l]<<' '<<wavelist.minImage(iter.rank())[l]<<'\n';
               std::cout<<"This is the bug\n";
            }
            }*/
         Gsq = unitCell.ksq(wavelist.minImage(iter.rank()));
         //expKsq_[i] = exp(Gsq*factor);
         expKsq_host[i] = exp(Gsq*factor);
         expKsq2_host[i] = exp(Gsq*factor / 2);
       //         std::cout << i    << "  "
       //         << Gmin << "  "
                 //  << Gsq  << "  "
       // << temp[i] << std::endl;
      }
      cudaMemcpy(expKsq_.cDField(), expKsq_host, kSize * sizeof(cufftReal), cudaMemcpyHostToDevice);
      cudaMemcpy(expKsq2_.cDField(), expKsq2_host, kSize * sizeof(cufftReal), cudaMemcpyHostToDevice);

   }

   /*
   * Setup the contour length step algorithm.
   */
   template <int D>
   void
   Block<D>::setupSolver(Block<D>::WField const& w)
   {
      // Preconditions
      int nx = mesh().size();
      UTIL_CHECK(nx > 0);

      // Populate expW_
      // std::cout << std::endl;
      // expW_[i] = exp(-0.5*w[i]*ds_);
      assignExp<<<NUMBER_OF_BLOCKS, THREADS_PER_BLOCK>>>(expW_.cDField(), w.cDField(), nx, (float)0.5* ds_);
      assignExp << <NUMBER_OF_BLOCKS, THREADS_PER_BLOCK >> >(expW2_.cDField(), w.cDField(), nx, (float)0.25 * ds_);

   }

   /*
   * Integrate to calculate monomer concentration for this block
   */
   template <int D>
   void Block<D>::computeConcentration(double prefactor)
   {
      // Preconditions
      int nx = mesh().size();
      UTIL_CHECK(nx > 0);
      UTIL_CHECK(ns_ > 0);
      UTIL_CHECK(ds_ > 0);
      UTIL_CHECK(propagator(0).isAllocated());
      UTIL_CHECK(propagator(1).isAllocated());
      UTIL_CHECK(cField().capacity() == nx)

      // Initialize cField to zero at all points
      //cField()[i] = 0.0;
      assignUniformReal<<<NUMBER_OF_BLOCKS, THREADS_PER_BLOCK>>>(cField().cDField(), 0.0, nx);

      Pscf::Pspg::Propagator<D> const & p0 = propagator(0);
      Pscf::Pspg::Propagator<D> const & p1 = propagator(1);


      //cudaDeviceSynchronize();
      multiplyScaleQQ << <NUMBER_OF_BLOCKS, THREADS_PER_BLOCK >> > (cField().cDField(), p0.q(0), p1.q(ns_ - 1), nx, 1.0);
      multiplyScaleQQ << <NUMBER_OF_BLOCKS, THREADS_PER_BLOCK >> > (cField().cDField(), p0.q(ns_-1), p1.q(0), nx, 1.0);
      for (int j = 1; j < ns_ - 1; j += 2) {
        //odd indices
         multiplyScaleQQ << <NUMBER_OF_BLOCKS, THREADS_PER_BLOCK >> > (cField().cDField(), p0.q(j), p1.q(ns_ - 1 - j), nx, 4.0);
      }
      for (int j = 2; j < ns_ - 2; j += 2) {
         //even indices
         multiplyScaleQQ << <NUMBER_OF_BLOCKS, THREADS_PER_BLOCK >> > (cField().cDField(), p0.q(j), p1.q(ns_ - 1 - j), nx, 2.0);
      }

    // cufftReal* tempVal = new cufftReal;

     //cudaMemcpy(tempVal, cField().cDField(), 1 * sizeof(cufftReal), cudaMemcpyDeviceToHost);
     //std::cout << "This is unscaled concentration " << *tempVal << std::endl;
     //std::cout << "This is ds_ " << ds_ << std::endl;
     //delete tempVal;

     scaleReal << <NUMBER_OF_BLOCKS, THREADS_PER_BLOCK >> > (cField().cDField(), nx, (float)(prefactor *ds_ / 3.0));
     //cudaDeviceSynchronize();


   }

   template <int D>
   void Block<D>::setupFFT() {
      if(!fft_.isSetup()) {
         //std::cout<<"setting up batch fft with the following values"<<std::endl;
         //std::cout<<mesh().dimensions()<<'\n'<<kMeshDimensions_<<'\n'<<ns_<<'\n';
         fft_.setup(qr_, qk_);
         fftBatched_.setup(mesh().dimensions(), kMeshDimensions_, ns_);
      }
   }

   /*
   * Propagate solution by one step.
   */
   //step have to be done in gpu
   template <int D>
   void Block<D>::step(const cufftReal* q, cufftReal* qNew)
   {
      // Check real-space mesh sizes
      int nx = mesh().size();
      UTIL_CHECK(nx > 0);
      UTIL_CHECK(qr_.capacity() == nx);
      UTIL_CHECK(expW_.capacity() == nx);

      // Fourier-space mesh sizes
      int nk = qk_.capacity();
      UTIL_CHECK(expKsq_.capacity() == nk);

      // Apply pseudo-spectral algorithm

     pointwiseMulSameStart << <NUMBER_OF_BLOCKS, THREADS_PER_BLOCK >> >
        (q, expW_.cDField(), expW2_.cDField(), qr_.cDField(), qr2_.cDField(), nx);
     fft_.forwardTransform(qr_, qk_);
     fft_.forwardTransform(qr2_, qk2_);
     scaleComplexTwinned << <NUMBER_OF_BLOCKS, THREADS_PER_BLOCK >> >
        (qk_.cDField(), qk2_.cDField(), expKsq_.cDField(), expKsq2_.cDField(), nk);
     fft_.inverseTransform(qk_, qr_);
     fft_.inverseTransform(qk2_, q2_);
     pointwiseMulTwinned << <NUMBER_OF_BLOCKS, THREADS_PER_BLOCK >> >
        (qr_.cDField(), q2_.cDField(), expW_.cDField(), q1_.cDField(), qr_.cDField(), nx);
     fft_.forwardTransform(qr_, qk_);
     scaleComplex << <NUMBER_OF_BLOCKS, THREADS_PER_BLOCK >> >(qk_.cDField(), expKsq2_.cDField(), nk);
     fft_.inverseTransform(qk_, qr_);
     richardsonExpTwinned << <NUMBER_OF_BLOCKS, THREADS_PER_BLOCK >> > (qNew, q1_.cDField(),
        qr_.cDField(), expW2_.cDField(), nx);
     //remove the use of q2


   }


   /*
   * Integrate to Stress exerted by the chain for this block
   * For many reasons, the code is written in away that tries
   * to function with basis functions but does nothing
   * to actually ensure its correctness.
   * To optimize it, I am rewritting it to only allow
   * the symmetry I since none of the code is correct in the first place
   */
   template <int D>
   void Block<D>::computeStress(WaveList<D>& wavelist, double prefactor)
   {
      // Preconditions

      int nx = mesh().size();
      UTIL_CHECK(nx > 0);
      UTIL_CHECK(ns_ > 0);
      UTIL_CHECK(ds_ > 0);
      UTIL_CHECK(propagator(0).isAllocated());
      UTIL_CHECK(propagator(1).isAllocated());

      double dels, normal, increment;
      normal = 3.0*6.0;

      //dont use a compile time array.....
      FArray<double, 6> dQ;

      int i;
      for (i = 0; i < 6; ++i) {
         dQ [i] = 0.0;
         stress_[i] = 0.0;
      }

      Pscf::Pspg::Propagator<D> const & p0 = propagator(0);
      Pscf::Pspg::Propagator<D> const & p1 = propagator(1);

      fftBatched_.forwardTransform(p0.head(), qkBatched_, ns_);
      fftBatched_.forwardTransform(p1.head(), qk2Batched_, ns_);
      cudaMemset(qr2_.cDField(), 0, mesh().size() * sizeof(int));

      for (int j = 0; j < ns_ ; ++j) {
         //basis.convertFieldDftToComponents(qkBatched_ + (j* kSize_) , q1_.cDField());

         //basis.convertFieldDftToComponents(qk2Batched_ + (kSize_ * (ns_ - 1 - j)) , q2_.cDField());

         dels = ds_;

         if (j != 0 && j != ns_ - 1) {
            if (j % 2 == 0) {
               dels = dels*2.0;
            } else {
               dels = dels*4.0;
            }
         }

         for (int n = 0; n < nParams_ ; ++n) {
            //do i need this?
            mulDelKsq<<<NUMBER_OF_BLOCKS, THREADS_PER_BLOCK >>>
               (qr2_.cDField(), qkBatched_ + (j * kSize_), qk2Batched_ + (kSize_ * (ns_ -1 -j)),
                wavelist.dkSq(), n , kSize_, nx);

            /*if(j == 0) {
               cufftReal* temp = new cufftReal[mesh().size()];
               cufftComplex* complex = new cufftComplex[kSize_];

               std::cout<<"real propagator 1\n";
               cudaMemcpy(temp, p0.head() + (mesh().size()* j), sizeof(cufftReal) * mesh().size(), cudaMemcpyDeviceToHost);
               for(int i = 0; i < mesh().size(); i++) {
                  std::cout<<temp[i]<<std::endl;
               }

               std::cout<<"output complex 1\n";
               cudaMemcpy(complex, qk2Batched_ + ( j *kSize_), sizeof(cufftComplex) * kSize_, cudaMemcpyDeviceToHost);
               for(int i = 0; i < kSize_; i++) {
                  std::cout<<complex[i].x <<' '<<complex[i].y<<std::endl;
               }

               std::cout<<"real propagator 2\n";
               cudaMemcpy(temp, p1.head() + (mesh().size() * (ns_ - 1 - j)), sizeof(cufftReal) * mesh().size(), cudaMemcpyDeviceToHost);
               for(int i = 0; i < mesh().size(); i++) {
                  std::cout<<temp[i]<<std::endl;
               }

               std::cout<<"output complex2\n";
               cudaMemcpy(complex, qk2Batched_ + ((ns_ - 1 - j)*kSize_), sizeof(cufftComplex) * kSize_, cudaMemcpyDeviceToHost);
               for(int i = 0; i < kSize_; i++) {
                  std::cout<<complex[i].x <<' '<<complex[i].y<<std::endl;
               }

               std::cout<<"final result\n";
               cudaMemcpy(temp, qr2_.cDField(), sizeof(cufftReal) * kSize_, cudaMemcpyDeviceToHost);
               for(int i = 0; i < kSize_; i++) {
                  std::cout<<temp[i]<<std::endl;
               }
               //exit(1);
               }*/
            increment = reductionH(qr2_, mesh().size());
            increment = (increment * kuhn() * kuhn() * dels)/normal;
            dQ [n] = dQ[n]-increment;
         }
      }
      // Normalize
      for (i = 0; i < nParams_; ++i) {
         stress_[i] = stress_[i] - (dQ[i] * prefactor);
      }
   }

   template<int D>
   cufftReal Block<D>::reductionH(const RDField<D>& a, int size) {
      reduction <<< NUMBER_OF_BLOCKS/2 , THREADS_PER_BLOCK, THREADS_PER_BLOCK*sizeof(cufftReal) >> >
         (d_temp_, a.cDField(), size);
      cudaMemcpy(temp_, d_temp_, NUMBER_OF_BLOCKS/2  * sizeof(cufftReal), cudaMemcpyDeviceToHost);
      cufftReal final = 0;
      cufftReal c = 0;
      for (int i = 0; i < NUMBER_OF_BLOCKS/2 ; ++i) {
         cufftReal y = temp_[i] - c;
         cufftReal t = final + y;
         c = (t - final) - y;
         final = t;
      }
      return final;
   }

}
}
#endif
