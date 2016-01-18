#ifndef PSCF_MIXTURE_STUB_H
#define PSCF_MIXTURE_STUB_H

/*
* PFTS - Polymer Field Theory Simulator
*
* Copyright 2013, David Morse (morse012@.umn.edu)
* Distributed under the terms of the GNU General Public License.
*/

#include "PolymerStub.h"
#include "SolventStub.h"
#include <pscf/MixtureTmpl.h>

namespace Pscf
{ 

   class MixtureStub 
    : public MixtureTmpl<PolymerStub, SolventStub>
   {

   public:

      MixtureStub()
      {  setClassName("Mixture"); }
   
   };

} 
#endif
