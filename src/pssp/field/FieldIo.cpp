/*
* PSCF - Polymer Self-Consistent Field Theory
*
* Copyright 2016, The Regents of the University of Minnesota
* Distributed under the terms of the GNU General Public License.
*/

#include "FieldIo.h"

namespace Pscf {
namespace Pssp
{

   template <>
   class FieldIo<1>;

   template <>
   class FieldIo<3>;

} // namespace Pscf::Pssp
} // namespace Pscf
