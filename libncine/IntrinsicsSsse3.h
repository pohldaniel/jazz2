// Copyright © 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016,
//             2017, 2018, 2019, 2020, 2021, 2022
//           Vladimír Vondruš <mosra@centrum.cz> and contributors
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

/** @file
	@brief Intrinsics for x86 SSSE3 instructions

	Equivalent to @cpp #include <tmmintrin.h> @ce on most compilers except for GCC
	4.8, where it contains an additional workaround to make the instructions
	available with just the @ref DEATH_ENABLE_SSSE3 function attribute instead of
	having to specify `-mssse3` for the whole compilation unit.
	@see @relativeref{Death,Cpu}, @ref Cpu-usage-target-attributes,
		@ref IntrinsicsSse2.h, @ref IntrinsicsSse3.h, @ref IntrinsicsSse4.h,
		@ref IntrinsicsAvx.h
*/

// See https://gist.github.com/rygorous/f26f5f60284d9d9246f6 for more info. If it wouldn't be for the #define,
// it could be all put into a macro with _Pragma to wrap around the include, but because I do, there has to be one
// wrapper header for each include.

// So users don't need to include the SSE2 and SSE3 headers explicitly before, in correct order
#include "IntrinsicsSse3.h"

#if defined(DEATH_TARGET_GCC) && !defined(DEATH_TARGET_CLANG) && __GNUC__*100 + __GNUC_MINOR__ < 409
#pragma GCC push_options
#pragma GCC target("ssse3")
#pragma push_macro("__SSSE3__")
#if !defined(__SSSE3__)
#	define __SSSE3__
#endif
#endif
#include <tmmintrin.h>
#if defined(DEATH_TARGET_GCC) && !defined(DEATH_TARGET_CLANG) && __GNUC__*100 + __GNUC_MINOR__ < 409
#pragma pop_macro("__SSSE3__")
#pragma GCC pop_options
#endif