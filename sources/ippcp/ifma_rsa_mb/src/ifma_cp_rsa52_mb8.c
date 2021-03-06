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
//  Purpose: MB RSA.
// 
//  Contents: public RSA encryption / decryption
//
*/

#ifdef IFMA_IPPCP_BUILD
#include "owndefs.h"
#endif /* IFMA_IPPCP_BUILD */

#if !defined(IFMA_IPPCP_BUILD) || (_IPP32E>=_IPP32E_K0)

#include <memory.h>
#include <stdlib.h>

#include "ifma_internal.h"
#include "ifma_internal_cp_layer.h"
#include "ifma_internal_method.h"
#include "rsa_ifma_status.h"

// y = x^65537 mod n
ifma_status ifma_cp_rsa52_public_mb8(const int8u* const from_pa[8],
                                           int8u* const to_pa[8],
                                     const int64u* const n_pa[8],
                                           int expected_rsa_bitsize,
                                     const ifma_RSA_Method* m,
                                           int8u* pBuffer)
{
   const ifma_RSA_Method* meth = m;

   ifma_status stt = 0;
   int buf_no;

   /* test input pointers */
   if(NULL==from_pa || NULL==to_pa || NULL==n_pa) {
      stt = IFMA_SET_STS_ALL(IFMA_STATUS_NULL_PARAM_ERR);
      return stt;
   }
   /* test rsa modulus size */
   if(RSA_1K != expected_rsa_bitsize && RSA_2K != expected_rsa_bitsize &&
      RSA_3K != expected_rsa_bitsize && RSA_4K != expected_rsa_bitsize) {
      stt = IFMA_SET_STS_ALL(IFMA_STATUS_MISMATCH_PARAM_ERR);
      return stt;
   }

   /* check pointers and values */
   for(buf_no=0; buf_no<8; buf_no++) {
      const int8u* inp = from_pa[buf_no];
            int8u* out = to_pa[buf_no];
      const int64u* n = n_pa[buf_no];

      /* if any of pointer NULL set error status */
      if(NULL==inp || NULL==out || NULL==n) {
         stt = IFMA_SET_STS(stt, buf_no, IFMA_STATUS_NULL_PARAM_ERR);
         continue;
      }
   }

   /* test method */
   if(NULL==meth) {
      meth = ifma_cp_RSA_pub65537_Method(expected_rsa_bitsize);
      if(NULL==meth) {
         stt = IFMA_SET_STS_ALL(IFMA_STATUS_NULL_PARAM_ERR);
         return stt;
      }
   }
   /* check if requested operation matched to method's */
   if(RSA_PUB_KEY != OP_RSA_ID(meth->id)) {
      stt = IFMA_SET_STS_ALL(IFMA_STATUS_MISMATCH_PARAM_ERR);
      return stt;
   }
   /* check if requested RSA matched to method's */
   if(expected_rsa_bitsize != BISIZE_RSA_ID(meth->id)) {
      stt = IFMA_SET_STS_ALL(IFMA_STATUS_MISMATCH_PARAM_ERR);
      return stt;
   }

   /*
   // processing
   */
   if( IFMA_IS_ANY_OK_STS(stt) ) {
      int8u* buffer = pBuffer;

      #if !defined(IFMA_IPPCP_BUILD)
      int allocated_buf = 0;
      
      /* check if allocated buffer) */
      if(NULL==buffer) {
         buffer = (int8u*)( malloc(meth->buffSize) );
         if(NULL==buffer) {
            stt = IFMA_SET_STS_ALL(IFMA_STATUS_NULL_PARAM_ERR);
            return stt;
         }
         allocated_buf = 1;
      }
      #endif
   
      ifma_cp_rsa_pub_layer_mb8(from_pa, to_pa, n_pa,
                                expected_rsa_bitsize, meth,
                                buffer);

      #if !defined(IFMA_IPPCP_BUILD)
      /* release buffer */
      if(allocated_buf)
         free(buffer);
      #endif
   }

   return stt;
}

ifma_status ifma_cp_rsa52_private_mb8(const int8u* const from_pa[8],
                                            int8u* const to_pa[8],
                                      const int64u* const d_pa[8],
                                      const int64u* const n_pa[8],
                                            int expected_rsa_bitsize,
                                      const ifma_RSA_Method* m,
                                            int8u* pBuffer)
{
   const ifma_RSA_Method* meth = m;

   ifma_status stt = 0;
   int buf_no;

   /* test input pointers */
   if(NULL==from_pa || NULL==to_pa || NULL==d_pa || NULL==n_pa) {
      stt = IFMA_SET_STS_ALL(IFMA_STATUS_NULL_PARAM_ERR);
      return stt;
   }
   /* test rsa modulus size */
   if(RSA_1K != expected_rsa_bitsize && RSA_2K != expected_rsa_bitsize &&
      RSA_3K != expected_rsa_bitsize && RSA_4K != expected_rsa_bitsize) {
      stt = IFMA_SET_STS_ALL(IFMA_STATUS_MISMATCH_PARAM_ERR);
      return stt;
   }

   /* check pointers and values */
   for(buf_no=0; buf_no<8; buf_no++) {
      const int8u* inp = from_pa[buf_no];
            int8u* out = to_pa[buf_no];
      const int64u* d = d_pa[buf_no];
      const int64u* n = n_pa[buf_no];

      /* if any of pointer NULL set error status */
      if(NULL==inp || NULL==out || NULL==d || NULL==n) {
         stt = IFMA_SET_STS(stt, buf_no, IFMA_STATUS_NULL_PARAM_ERR);
         continue;
      }
   }

   /* test method */
   if(NULL==meth) {
      meth = ifma_cp_RSA_private_Method(expected_rsa_bitsize);
      if(NULL==meth) {
         stt = IFMA_SET_STS_ALL(IFMA_STATUS_NULL_PARAM_ERR);
         return stt;
      }
   }
   /* check if requested operation matched to method's */
   if(RSA_PRV2_KEY != OP_RSA_ID(meth->id)) {
      stt = IFMA_SET_STS_ALL(IFMA_STATUS_MISMATCH_PARAM_ERR);
      return stt;
   }
   /* check if requested RSA matched to method's */
   if(expected_rsa_bitsize != BISIZE_RSA_ID(meth->id)) {
      stt = IFMA_SET_STS_ALL(IFMA_STATUS_MISMATCH_PARAM_ERR);
      return stt;
   }

   /*
   // processing
   */
   if( IFMA_IS_ANY_OK_STS(stt) ) {
      int8u* buffer = pBuffer;

      #if !defined(IFMA_IPPCP_BUILD)
      int allocated_buf = 0;

      /* check if allocated buffer) */
      if(NULL==buffer) {
         buffer = (int8u*)( malloc(meth->buffSize) );
         if(NULL==buffer) {
            stt = IFMA_SET_STS_ALL(IFMA_STATUS_NULL_PARAM_ERR);
            return stt;
         }
         allocated_buf = 1;
      }
      #endif
   
      ifma_cp_rsa_prv2_layer_mb8(from_pa, to_pa, d_pa, n_pa,
                                expected_rsa_bitsize, meth,
                                buffer);

      #if !defined(IFMA_IPPCP_BUILD)
      /* release buffer */
      if(allocated_buf)
         free(buffer);
      #endif
   }

   return stt;
}

ifma_status ifma_cp_rsa52_private_ctr_mb8(const int8u* const from_pa[8],
                                                int8u* const to_pa[8],
                                          const int64u* const p_pa[8],
                                          const int64u* const q_pa[8],
                                          const int64u* const dp_pa[8],
                                          const int64u* const dq_pa[8],
                                          const int64u* const iq_pa[8],
                                                int expected_rsa_bitsize,
                                          const ifma_RSA_Method* m,
                                                int8u* pBuffer)
{
   const ifma_RSA_Method* meth = m;

   ifma_status stt = 0;
   int buf_no;

   /* test input pointers */
      if(NULL==p_pa || NULL==q_pa || NULL==dp_pa || NULL==dq_pa || NULL==iq_pa) {
      stt = IFMA_SET_STS_ALL(IFMA_STATUS_NULL_PARAM_ERR);
      return stt;
   }
   /* test rsa modulus size */
   if(RSA_1K != expected_rsa_bitsize && RSA_2K != expected_rsa_bitsize &&
      RSA_3K != expected_rsa_bitsize && RSA_4K != expected_rsa_bitsize) {
      stt = IFMA_SET_STS_ALL(IFMA_STATUS_MISMATCH_PARAM_ERR);
      return stt;
   }

   /* check pointers and values */
   for(buf_no=0; buf_no<8; buf_no++) {
      const int8u* inp = from_pa[buf_no];
            int8u* out = to_pa[buf_no];
      const int64u* p = p_pa[buf_no];
      const int64u* q = q_pa[buf_no];
      const int64u* dp = dp_pa[buf_no];
      const int64u* dq = dq_pa[buf_no];
      const int64u* iq = iq_pa[buf_no];

      /* if any of pointer NULL set error status */
      if(NULL==inp || NULL==out || NULL==q || NULL==p || NULL==dq || NULL==dp || NULL==iq) {
         stt = IFMA_SET_STS(stt, buf_no, IFMA_STATUS_NULL_PARAM_ERR);
         continue;
      }
   }

   /* test method */
   if(NULL==meth) {
      meth = ifma_cp_RSA_private_ctr_Method(expected_rsa_bitsize);
      if(NULL==meth) {
         stt = IFMA_SET_STS_ALL(IFMA_STATUS_NULL_PARAM_ERR);
         return stt;
      }
   }
   /* check if requested operation matched to method's */
   if(RSA_PRV5_KEY != OP_RSA_ID(meth->id)) {
      stt = IFMA_SET_STS_ALL(IFMA_STATUS_MISMATCH_PARAM_ERR);
      return stt;
   }
   /* check if requested RSA matched to method's */
   if(expected_rsa_bitsize != BISIZE_RSA_ID(meth->id)) {
      stt = IFMA_SET_STS_ALL(IFMA_STATUS_MISMATCH_PARAM_ERR);
      return stt;
   }

   /*
   // processing
   */
   if( IFMA_IS_ANY_OK_STS(stt) ) {
      int8u* buffer = pBuffer;

      #if !defined(IFMA_IPPCP_BUILD)
      int allocated_buf = 0;

      /* check if allocated buffer) */
      if(NULL==buffer) {
         buffer = (int8u*)( malloc(meth->buffSize) );
         if(NULL==buffer) {
            stt = IFMA_SET_STS_ALL(IFMA_STATUS_NULL_PARAM_ERR);
            return stt;
         }
         allocated_buf = 1;
      }
      #endif
   
      ifma_cp_rsa_prv5_layer_mb8(from_pa, to_pa, p_pa, q_pa, dp_pa, dq_pa, iq_pa,
                                expected_rsa_bitsize, meth,
                                buffer);

      #if !defined(IFMA_IPPCP_BUILD)
      /* release buffer */
      if(allocated_buf)
         free(buffer);
      #endif
   }

   return stt;
}

#endif
