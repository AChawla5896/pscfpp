/*
* PSCF - Polymer Self-Consistent Field Theory
*
* Copyright 2016, The Regents of the University of Minnesota
* Distributed under the terms of the GNU General Public License.
*/

#include "UnitCell.h"
#include <util/math/Constants.h>

namespace Pscf
{

   using namespace Util;

   //template class UnitCell<1>;
   //template class UnitCell<2>;
   //template class UnitCell<3>;

   /*
   * Extract a UnitCell<1>::LatticeSystem from an istream as a string.
   */
   std::istream& operator >> (std::istream& in,
                              UnitCell<1>::LatticeSystem& lattice)
   {

      std::string buffer;
      in >> buffer;
      if (buffer == "Lamellar" || buffer == "lamellar") {
         lattice = UnitCell<1>::Lamellar;
      } else {
         UTIL_THROW("Invalid UnitCell<1>::LatticeSystem value input");
      }
      return in;
   }

   /*
   * Insert a UnitCell<1>::LatticeSystem to an ostream as a string.
   */
   std::ostream& operator << (std::ostream& out,
                              UnitCell<1>::LatticeSystem lattice)
   {
      if (lattice == UnitCell<1>::Lamellar) {
         out << "lamellar";
      } else {
         UTIL_THROW("Invalid value of UnitCell<1>::Lamellar");
      }
      return out;
   }

   /*
   * Read the lattice system and set nParameter.
   */
   void UnitCell<1>::setNParameter()
   {
      if (lattice_ == UnitCell<1>::Lamellar) {
         nParameter_ = 1;
      } else {
         UTIL_THROW("Invalid lattice system value");
      }
   }

   /*
   * Set the Bravais and reciprocal lattice parameters.
   */
   void UnitCell<1>::setLattice()
   {
      // Initialization 
      rBasis_[0][0] = 0.0;
      kBasis_[0][0] = 0.0;
      
      // Assigning values
      rBasis_[0][0] = parameters_[0];
      kBasis_[0][0] = 2.0*Constants::Pi/parameters_[0];
      drBasis_[0](0,0) = 1.0;
      
      // Initialization
      dkBasis_[0](0,0)  = 0.0;
      drrBasis_[0](0,0) = 0.0;
      dkkBasis_[0](0,0) = 0.0;

      int p;
      for (p=0; p < nParameter_; ++p){

         dkBasis_[p](0,0) = dkBasis_[p](0,0) 
                          -
                           ((kBasis_[0][0]*drBasis_[p](0,0)*kBasis_[0][0])/(2.0*Constants::Pi));

      }

      for (p=0; p < nParameter_; ++p){
         drrBasis_[p](0,0) += ((rBasis_[0][0]*drBasis_[p](0,0))+(rBasis_[0][0]*drBasis_[p](0,0)));
         dkkBasis_[p](0,0) += ((kBasis_[0][0]*dkBasis_[p](0,0))+(kBasis_[0][0]*dkBasis_[p](0,0)));       
      }
      
   }

   // Two-Dimensional Systems

   /*
   * Extract a UnitCell<2>::LatticeSystem from an istream as a string.
   */
   std::istream& operator >> (std::istream& in,
                              UnitCell<2>::LatticeSystem& lattice)
   {

      std::string buffer;
      in >> buffer;
      if (buffer == "Square" || buffer == "square") {
         lattice = UnitCell<2>::Square;
      } else
      if (buffer == "Rectangular" || buffer == "rectangular") {
         lattice = UnitCell<2>::Rectangular;
      } else
      if (buffer == "Rhombic" || buffer == "rhombic") {
         lattice = UnitCell<2>::Rhombic;
      } else
      if (buffer == "Hexagonal" || buffer == "hexagonal") {
         lattice = UnitCell<2>::Hexagonal;
      } else
      if (buffer == "Oblique" || buffer == "oblique") {
         lattice = UnitCell<2>::Oblique;
      } else {
         UTIL_THROW("Invalid UnitCell<2>::LatticeSystem value input");
      }
      return in;
   }

   /*
   * Insert a UnitCell<2>::LatticeSystem to an ostream as a string.
   */
   std::ostream& operator << (std::ostream& out,
                              UnitCell<2>::LatticeSystem lattice)
   {
      if (lattice == UnitCell<2>::Square) {
         out << "square";
      } else
      if (lattice == UnitCell<2>::Rectangular) {
         out << "rectangular";
      } else
      if (lattice == UnitCell<2>::Rhombic) {
         out << "rhombic";
      } else
      if (lattice == UnitCell<2>::Hexagonal) {
         out << "hexagonal";
      } else
      if (lattice == UnitCell<2>::Oblique) {
         out << "oblique";
      } else {
         UTIL_THROW("This should never happen");
      }
      return out;
   }

   /*
   * Read the lattice system and set nParameter.
   */
   void UnitCell<2>::setNParameter()
   {
      if (lattice_ == UnitCell<2>::Square) {
         nParameter_ = 1;
      } else
      if (lattice_ == UnitCell<2>::Rhombic) {
         nParameter_ = 1;
      } else
      if (lattice_ == UnitCell<2>::Hexagonal) {
         nParameter_ = 1;
      } else
      if (lattice_ == UnitCell<2>::Rectangular) {
         nParameter_ = 2;
      } else
      if (lattice_ == UnitCell<2>::Oblique) {
         nParameter_ = 3;
      } else {
         UTIL_THROW("Invalid lattice system value");
      }
   }

   /*
   * Set the Bravais and reciprocal lattice parameters.
   */
   void UnitCell<2>::setLattice()
   {
      // Initialize all elements to zero
      int i, j, k;
      for (i = 0; i < 2; ++i) { 
         for (j = 0; j < 2; ++j) { 
            rBasis_[i][j] = 0.0;
            kBasis_[i][j] = 0.0;

            for (k=0; k < 6; ++k){
               drrBasis_[k](i,j)=0.0;
               dkkBasis_[k](i,j)=0.0;
            }

         }
      }

      // Set elements for specific lattice types
      double TwoPi = 2.0*Constants::Pi;
      if (lattice_ == UnitCell<2>::Square) {
         for (i=0; i < 2; ++i) { 
            rBasis_[i][i] = parameters_[0];
            kBasis_[i][i] = TwoPi/parameters_[0];
            drBasis_[0](i,i) = 1.0;
         }
      } else 
      if (lattice_ == UnitCell<2>::Rectangular) {
         for (i=0; i < 2; ++i) { 
            rBasis_[i][i] = parameters_[i];
            kBasis_[i][i] = TwoPi/parameters_[i];
            drBasis_[i](i,i) = 1.0;
         }
      } else {
         UTIL_THROW("Unimplemented 2D lattice type");
      }

      // Compute dkBasis
      int p, q, r, s, t;
      for (p = 0; p < nParameter_; ++p) {
         for (q = 0; q < 2; ++q) {
            for (r = 0; r < 2; ++r) {
               for (s = 0; s < 2; ++s) {
                  for (t = 0; t < 2; ++t) {
                     dkBasis_[p](q,r) 
                       -= kBasis_[q][s]*drBasis_[p](t,s)*kBasis_[t][r];
                  }
               }
               dkBasis_[p](q,r) /= 2.0*Constants::Pi;
            }
         }
      }

      // Compute drrBasis and dkkBasis 
      for (p = 0; p < nParameter_; ++p) {
         for (q = 0; q < 2; ++q) {
            for (r = 0; r < 2; ++r) {
               for (s = 0; s < 2; ++s) {
                  drrBasis_[p](q,r) += rBasis_[q][s]*drBasis_[p](r,s);
                  drrBasis_[p](q,r) += rBasis_[r][s]*drBasis_[p](q,s);
                  dkkBasis_[p](q,r) += kBasis_[q][s]*dkBasis_[p](r,s);
                  dkkBasis_[p](q,r) += kBasis_[r][s]*dkBasis_[p](q,s);
 
               }
            }
         }
      }

   }

   // Three-Dimensional Systems

   /*
   * Extract a UnitCell<3>::LatticeSystem from an istream as a string.
   */
   std::istream& operator >> (std::istream& in,
                              UnitCell<3>::LatticeSystem& lattice)
   {

      std::string buffer;
      in >> buffer;
      if (buffer == "Cubic" || buffer == "cubic") {
         lattice = UnitCell<3>::Cubic;
      } else
      if (buffer == "Tetragonal" || buffer == "tetragonal") {
         lattice = UnitCell<3>::Tetragonal;
      } else
      if (buffer == "Orthorhombic" || buffer == "orthorhombic") {
         lattice = UnitCell<3>::Orthorhombic;
      } else
      if (buffer == "Monoclinic" || buffer == "monoclinic") {
         lattice = UnitCell<3>::Monoclinic;
      } else
      if (buffer == "Triclinic" || buffer == "triclinic") {
         lattice = UnitCell<3>::Triclinic;
      } else
      if (buffer == "Rhombohedral" || buffer == "rhombohedral") {
         lattice = UnitCell<3>::Rhombohedral;
      } else
      if (buffer == "Hexagonal" || buffer == "hexagonal") {
         lattice = UnitCell<3>::Hexagonal;
      } else {
         UTIL_THROW("Invalid UnitCell<3>::LatticeSystem value input");
      }
      return in;
   }

   /*
   * Insert a UnitCell<3>::LatticeSystem to an ostream as a string.
   */
   std::ostream& operator<<(std::ostream& out,
                            UnitCell<3>::LatticeSystem lattice)
   {
      if (lattice == UnitCell<3>::Cubic) {
         out << "cubic";
      } else
      if (lattice == UnitCell<3>::Tetragonal) {
         out << "tetragonal";
      } else
      if (lattice == UnitCell<3>::Orthorhombic) {
         out << "orthorhombic";
      } else
      if (lattice == UnitCell<3>::Monoclinic) {
         out << "monoclinic";
      } else
      if (lattice == UnitCell<3>::Triclinic) {
         out << "triclinic";
      } else
      if (lattice == UnitCell<3>::Rhombohedral) {
         out << "rhombohedral";
      } else
      if (lattice == UnitCell<3>::Hexagonal) {
         out << "hexagonal";
      } else {
         UTIL_THROW("This should never happen");
      }
      return out;
   }

   /*
   * Read the lattice system and set nParameter.
   */
   void UnitCell<3>::setNParameter()
   {
      if (lattice_ == UnitCell<3>::Cubic) {
         nParameter_ = 1;
      } else
      if (lattice_ == UnitCell<3>::Tetragonal) {
         nParameter_ = 2;
      } else
      if (lattice_ == UnitCell<3>::Orthorhombic) {
         nParameter_ = 3;
      } else
      if (lattice_ == UnitCell<3>::Monoclinic) {
         nParameter_ = 4;
      } else
      if (lattice_ == UnitCell<3>::Triclinic) {
         nParameter_ = 6;
      } else
      if (lattice_ == UnitCell<3>::Rhombohedral) {
         nParameter_ = 2;
      } else
      if (lattice_ == UnitCell<3>::Hexagonal) {
         nParameter_ = 2;
      } else {
         UTIL_THROW("Invalid value");
      }
   }

   /*
   * Set the Bravais and reciprocal lattice parameters.
   */
   void UnitCell<3>::setLattice()
   {
      // Initialize all elements to zero
      int i, j, k;
      for (i = 0; i < 3; ++i) { 
         for (j = 0; j < 3; ++j) { 
            rBasis_[i][j] = 0.0;
            kBasis_[i][j] = 0.0;
            for (k=0; k < 6; ++k){
               drBasis_[k](i,j)=0.0;
               dkBasis_[k](i,j)=0.0;
               drrBasis_[k](i,j)=0.0;
               dkkBasis_[k](i,j)=0.0;
            }
         }
      }

      // Set elements for specific lattice types
      double TwoPi = 2.0*Constants::Pi;
      if (lattice_ == UnitCell<3>::Cubic) {
         for (i=0; i < 3; ++i) { 
            rBasis_[i][i] = parameters_[0];
            kBasis_[i][i] = TwoPi/parameters_[0];
            drBasis_[0](i,i) = 1.0;
         }
      } else 
      if (lattice_ == UnitCell<3>::Tetragonal) {
         rBasis_[0][0] = parameters_[0];
         rBasis_[1][1] = rBasis_[0][0];
         rBasis_[2][2] = parameters_[1];
         kBasis_[0][0] = TwoPi/parameters_[0];
         kBasis_[1][1] = kBasis_[0][0];
         kBasis_[2][2] = TwoPi/parameters_[1];
         drBasis_[0](0,0) = 1.0;
         drBasis_[0](1,1) = 1.0;
         drBasis_[1](2,2) = 1.0;
      } else 
      if (lattice_ == UnitCell<3>::Orthorhombic) {
         for (i=0; i < 3; ++i) { 
            rBasis_[i][i] = parameters_[i];
            kBasis_[i][i] = TwoPi/parameters_[i];
            drBasis_[i](i,i) = 1.0;
         }
      } else {
         UTIL_THROW("Unimplemented 3D lattice type");
      }

       int p, q, r, s, t;
       for (p=0; p < nParameter_; ++p){
          for (q=0; q < 3; ++q){
             for (r=0; r<3; ++r){
                for (s=0; s<3; ++s){
                   for (t=0; t<3; ++t){ 

                      //dkBasis_[p](q,r) = dkBasis_[p](q,r)-((kBasis_[q][s]*drBasis_[p](t,s)*kBasis_[t][r])/(2.0*Constants::Pi));
 
                      dkBasis_[p](q,r) -= 
                              kBasis_[q][s]*drBasis_[p](t,s)*kBasis_[t][r];
                   }
                }
                dkBasis_[p](q,r) /= 2.0*Constants::Pi;
             }
          }
       }
 
       for (p=0; p < nParameter_; ++p) {
          for (q=0; q < 3; ++q) {
             for (r=0; r<3; ++r) {
                for (s=0; s<3; ++s) {
 
                   drrBasis_[p](q,r) = drrBasis_[p](q,r)+((rBasis_[q][s]*drBasis_[p](r,s))+(rBasis_[r][s]*drBasis_[p](q,s)));// These indices differ from those in fortran
                   //  drrBasis_[p](q,r) = drrBasis_[p](q,r)+((rBasis_[q][s]*drBasis_[p](s,r))+(rBasis_[r][s]*drBasis_[p](s,q)));
 
                   dkkBasis_[p](q,r) = dkkBasis_[p](q,r)+((kBasis_[q][s]*dkBasis_[p](r,s))+(kBasis_[r][s]*dkBasis_[p](q,s)));
 
                }
             }
          }
       }

   }

}
