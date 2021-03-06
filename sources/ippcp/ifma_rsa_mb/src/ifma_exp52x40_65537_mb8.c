/*******************************************************************************
* Copyright 2019 Intel Corporation
* All Rights Reserved.
*
* If this  software was obtained  under the  Intel Simplified  Software License,
* the following terms apply:
*
* The source code,  information  and material  ("Material") contained  herein is
* owned by Intel Corporation or its  suppliers or licensors,  and  title to such
* Material remains with Intel  Corporation or its  suppliers or  licensors.  The
* Material  contains  proprietary  information  of  Intel or  its suppliers  and
* licensors.  The Material is protected by  worldwide copyright  laws and treaty
* provisions.  No part  of  the  Material   may  be  used,  copied,  reproduced,
* modified, published,  uploaded, posted, transmitted,  distributed or disclosed
* in any way without Intel's prior express written permission.  No license under
* any patent,  copyright or other  intellectual property rights  in the Material
* is granted to  or  conferred  upon  you,  either   expressly,  by implication,
* inducement,  estoppel  or  otherwise.  Any  license   under such  intellectual
* property rights must be express and approved by Intel in writing.
*
* Unless otherwise agreed by Intel in writing,  you may not remove or alter this
* notice or  any  other  notice   embedded  in  Materials  by  Intel  or Intel's
* suppliers or licensors in any way.
*
*
* If this  software  was obtained  under the  Apache License,  Version  2.0 (the
* "License"), the following terms apply:
*
* You may  not use this  file except  in compliance  with  the License.  You may
* obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
*
*
* Unless  required  by   applicable  law  or  agreed  to  in  writing,  software
* distributed under the License  is distributed  on an  "AS IS"  BASIS,  WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
* See the   License  for the   specific  language   governing   permissions  and
* limitations under the License.
*******************************************************************************/

/* 
// 
//  Purpose: MB RSA-2048.
// 
//  Contents: 20x52-bit exponentiation, e=65537
//
*/

#ifdef IFMA_IPPCP_BUILD
#include "owndefs.h"
#endif /* IFMA_IPPCP_BUILD */

#if !defined(IFMA_IPPCP_BUILD) || (_IPP32E>=_IPP32E_K0)

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#pragma warning(disable: 4310) // cast truncates constant value in MSVC
#endif

#include "ifma_internal.h"
#include "immintrin.h"


#define USE_AMS
#ifdef USE_AMS
    #define SQUARE_52x40_mb8(out, Y, mod, k0) \
         AMS52x40_diagonal_mb8((int64u*)out, (int64u*)Y, (int64u*)mod, (int64u*)k0);

    #ifdef USE_AMS_5x
    #define SQUARE_5x52x40_mb8(out, Y, mod, k0) \
         AMS5x52x40_diagonal_mb8((int64u*)out, (int64u*)Y, (int64u*)mod, (int64u*)k0);
    #else
    #define SQUARE_5x52x40_mb8(out, Y, mod, k0) \
         AMS52x40_diagonal_mb8((int64u*)out, (int64u*)Y, (int64u*)mod, (int64u*)k0); \
         AMS52x40_diagonal_mb8((int64u*)out, (int64u*)out, (int64u*)mod, (int64u*)k0); \
         AMS52x40_diagonal_mb8((int64u*)out, (int64u*)out, (int64u*)mod, (int64u*)k0); \
         AMS52x40_diagonal_mb8((int64u*)out, (int64u*)out, (int64u*)mod, (int64u*)k0); \
         AMS52x40_diagonal_mb8((int64u*)out, (int64u*)out, (int64u*)mod, (int64u*)k0);
    #endif
#else
    #define SQUARE_52x40_mb8(out, Y, mod, k0) \
         ifma_amm52x40_mb8((int64u*)out, (int64u*)Y, (int64u*)Y, (int64u*)mod, (int64u*)k0);
    #define SQUARE_5x52x40_mb8(out, Y, mod, k0) \
         ifma_amm52x40_mb8((int64u*)out, (int64u*)Y, (int64u*)Y, (int64u*)mod, (int64u*)k0); \
         ifma_amm52x40_mb8((int64u*)out, (int64u*)out, (int64u*)out, (int64u*)mod, (int64u*)k0); \
         ifma_amm52x40_mb8((int64u*)out, (int64u*)out, (int64u*)out, (int64u*)mod, (int64u*)k0); \
         ifma_amm52x40_mb8((int64u*)out, (int64u*)out, (int64u*)out, (int64u*)mod, (int64u*)k0); \
         ifma_amm52x40_mb8((int64u*)out, (int64u*)out, (int64u*)out, (int64u*)mod, (int64u*)k0);
#endif

#define BITSIZE_MODULUS (RSA_2K)
#define LEN52           (NUMBER_OF_DIGITS(BITSIZE_MODULUS,DIGIT_SIZE))  //40

void EXP52x40_pub65537_mb8(int64u out[][8],
                     const int64u base[][8],
                     const int64u modulus[][8],
                     const int64u toMont[][8],
                     const int64u k0[8])
{
   /* allocate stack for red(undant) result Y and multiplier X */
   __ALIGN64 int64u red_Y[LEN52][8];
   __ALIGN64 int64u red_X[LEN52][8];

   /* convert base into redundant domain */
   ifma_amm52x40_mb8((int64u*)red_X, (int64u*)base, (int64u*)toMont, (int64u*)modulus, (int64u*)k0);

   /* exponentition 65537 = 0x10001 */
   SQUARE_52x40_mb8((int64u*)red_Y, (int64u*)red_X, (int64u*)modulus, (int64u*)k0);
   SQUARE_5x52x40_mb8((int64u*)red_Y, (int64u*)red_Y, (int64u*)modulus, (int64u*)k0);
   SQUARE_5x52x40_mb8((int64u*)red_Y, (int64u*)red_Y, (int64u*)modulus, (int64u*)k0);
   SQUARE_5x52x40_mb8((int64u*)red_Y, (int64u*)red_Y, (int64u*)modulus, (int64u*)k0);
   ifma_amm52x40_mb8((int64u*)red_Y, (int64u*)red_Y, (int64u*)red_X, (int64u*)modulus, (int64u*)k0);

   /* convert result back in regular 2^52 domain */
   zero_mb8(red_X, LEN52);
   _mm512_store_si512(red_X, _mm512_set1_epi64(1));
   ifma_amm52x40_mb8((int64u*)out, (int64u*)red_Y, (int64u*)red_X, (int64u*)modulus, (int64u*)k0);
}

#endif
