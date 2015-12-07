#ifndef PFTS_PROPAGATOR_STUB_H
#define PFTS_PROPAGATOR_STUB_H

/*
* PFTS - Polymer Field Theory Simulator
*
* Copyright 2013, David Morse (morse012@.umn.edu)
* Distributed under the terms of the GNU General Public License.
*/

#include <pfts/PropagatorTmpl.h>
#include <util/containers/DArray.h>

namespace Pfts{ 

   class PropagatorStub;

   class PropagatorStub : public PropagatorTmpl<PropagatorStub>
   {

   public:

      // Public typedefs

      /**
      * Chemical potential field type.
      */
      typedef DArray<double> WField;

      /**
      * Monomer concentration field type.
      */
      typedef DArray<double> CField;

      // Member functions

      /**
      * Constructor.
      */
      PropagatorStub(){}

      /**
      * Solve the modified diffusion equation for this block.
      *
      * \param w chemical potential field for appropriate monomer type
      */
      void solve(const WField& w)
      {
         setIsComplete(true);
      };
  
      /**
      * Integrate to calculate monomer concentration for this block
      */ 
      void integrate(const PropagatorStub& partner, CField integral)
      {};
   
   };

} 
#endif
