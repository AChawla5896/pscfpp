#ifndef FD1D_ITERATOR_H
#define FD1D_ITERATOR_H

/*
* PFTS - Polymer Field Theory Simulator
*
* Copyright 2013, David Morse (morse012@.umn.edu)
* Distributed under the terms of the GNU General Public License.
*/

#include "Mixture.h"

namespace Pscf {
namespace Fd1d
{

   using namespace Util;

   class Iterator : public ParamComposite
   {

   public:

      /**
      * Constructor.
      */
      Iterator();

      /**
      * Destructor.
      */
      ~Iterator();

      /**
      * Create association with the mixture.
      */
      virtual void setMixture(Mixture& mixture);

      /**
      * Iterate to solution.
      *
      * \return error code: 0 for success, 1 for failure.
      */
      virtual int solve() = 0;

   protected:

      Mixture& mixture();
      
   private:

      // Pointer to associated Mixture object.
      Mixture* mixturePtr_;

   };

   inline
   Mixture& Iterator::mixture()
   {
      UTIL_ASSERT(mixturePtr_);
      return *mixturePtr_;
   }

} // namespace Fd1d
} // namespace Pscf
#endif
