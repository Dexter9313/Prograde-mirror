/*
    Copyright (C) 2018 Florian Cabot <florian.cabot@hotmail.fr>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

/*! \defgroup math Mathematics
 * Contains everything related to mathematics, including algebra or numerical
 * analysis.
 *
 *  # Conventions
 *
 *  - Right-handed coordinate systems will be used.
 *  - *z* coordinate (the third one) points "up" whenever it makes sense to
 * refer to an "up" direction.
 *  - Angles with implicit units are in radians
 *
 *  # Notations
 *
 *  In the whole documentation, we will use the following notations.
 *
 * - [x; y; z] describes a mathematical vector in vector-space R^3 with
 * components of respective values x, y and z.
 *
 * - [x; y; z; w] describes a mathematical vector in vector-space R^4 with
 * components of respective values x, y, z and w.
 *
 * - [a b c; d e f; g h i] describes a mathematical 3x3
 * matrix with column-vectors [a; d; g], [b; e; h] and [c; f; i].
 *
 * - [a b c d; e f g h; i j k l; m n o p]
 * describes a mathematical 4x4
 * matrix with column-vectors [a; e; i; m], [b; f; j; n], [c; g; k; o]
 * and [d; h; l; p].
 *
 *  # Authorized libraries
 *
 *  To better control what uses which library, each module has a set of
 * authorized libraries. This is desired mainly in case any library has to be
 * changed or updated. Hopefully in these cases, the least amount of code will
 * have to be rewritten.
 *
 *  The C++ Standard Library is always considered authorized.
 *
 * For this module, these libraries are :
 *  - Boost Math
 *
 *  Any other library usage within this module will have to be discussed in an
 * issue and properly documented if accepted.
 *
 */

/*! \defgroup exceptions Exceptions
 * Contains everything related to exceptions definition and handling
 *
 * Typical usage :
 *
 * \code{.cpp}
 *
 * #include "exceptions/Exceptions.hpp"
 *
 * int main(int argc, char* argv[]) //argv is mandatory
 * {
 *  	BEGIN_EXCEPTIONS
 *
 *  	//some code
 *
 *  	CRITICAL("There's a problem")
 *
 *  	//some code
 *
 *  	END_EXCEPTIONS
 *
 *  	return 0;
 * }
 * \endcode
 *
 * Not every function is detailed in this documentation as they are not supposed
 * to be used anywhere else in the project,
 * so for more details please directly read the Exceptions.hpp file's source
 * code and the corresponding platform specific headers.
 *
 * # Authorized libraries
 *
 *  To better control what uses which library, each module has a set of
 * authorized libraries. This is desired mainly in case any library has to be
 * changed or updated. Hopefully in these cases, the least amount of code will
 * have to be rewritten.
 *
 *  The C++ Standard Library is always considered authorized.
 *
 *  For now, this module can only use the C++ standard library.
 *
 *  Any other library usage within this module will have to be discussed in an
 * issue and properly documented if accepted.
 */

/*! \defgroup phys Physics
 * Contains everything related to physics.
 *
 * This includes orbital mechanics,
 * time management, physical objects definition and behavior, etc... In
 * short, it includes most of the things that are not directly related
 * to display and that describe what Prograde actually simulates.
 *
 *  # Conventions
 *
 *  - If not explicit, color values are given in
 * [ARGB](https://en.wikipedia.org/wiki/RGBA_color_space#ARGB_(word-order))
 * format if 4-dimensional or RGB if 3-dimensional with integer values
 * from 0 (no contribution) to 255 (max contribution).
 *
 *  - Whenever time values are given, they are given in seconds. If it's
 * supposed to be a date, then 0 means "0 seconds since epoch". Epoch time may
 * vary depending on the planetary system or simulation parameters.
 *
 * See also \ref math conventions for additional conventions.
 *
 *  # Notations
 *
 * See also \ref math notations for additional notations.
 *
 *  # Authorized libraries
 *
 *  To better control what uses which library, each module has a set of
 * authorized libraries. This is desired mainly in case any library has to be
 * changed or updated. Hopefully in these cases, the least amount of code will
 * have to be rewritten.
 *
 *  The C++ Standard Library is always considered authorized.
 *
 * For this module, these libraries are :
 *  - Bullet (shipped with Urho3D)
 *
 *  Any other library usage within this module will have to be discussed in an
 * issue and properly documented if accepted.
 *
 */
