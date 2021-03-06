/*******************************************************************************
* Copyright 2005-2019 Intel Corporation
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
//  Purpose:
//     Cryptography Primitive.
//     DL over Prime Finite Field (EC Key Generation, Validation and Set Up)
// 
//  Contents:
//        ippsDLPGenKeyPair()
//
*/

#include "owndefs.h"
#include "owncp.h"
#include "pcpdlp.h"

/*F*
// Name: ippsDLPGenKeyPair
//
// Purpose: Generate DL (private,public) Key Pair
//
// Returns:                   Reason:
//    ippStsNullPtrErr           NULL == pDL
//                               NULL == pPrivate
//                               NULL == pPublic
//                               NULL == rndFunc
//
//    ippStsContextMatchErr      invalid pDL->idCtx
//                               invalid pPrivate->idCtx
//                               invalid pPublic->idCtx
//
//    ippStsIncompleteContextErr
//                               incomplete context
//
//    ippStsRangeErr             not enough room for:
//                               pPrivate,
//                               pPublic
//
//    ippStsNoErr                no error
//
// Parameters:
//    pPrvKey     pointer to the new privatea key
//    pPubKey     pointer to the corrsponding public key
//    pDL         pointer to the DL context
//    rndFunc     external random generator
//    pRndParam   pointer to the external random generator params
*F*/
IPPFUN(IppStatus, ippsDLPGenKeyPair,(IppsBigNumState* pPrvKey, IppsBigNumState* pPubKey,
                                     IppsDLPState* pDL,
                                     IppBitSupplier rndFunc, void* pRndParam))
{
   /* test DL context */
   IPP_BAD_PTR1_RET(pDL);
   pDL = (IppsDLPState*)( IPP_ALIGNED_PTR(pDL, DLP_ALIGNMENT) );
   IPP_BADARG_RET(!DLP_VALID_ID(pDL), ippStsContextMatchErr);

   /* test flag */
   IPP_BADARG_RET(!DLP_COMPLETE(pDL), ippStsIncompleteContextErr);

   /* test random generator */
   IPP_BAD_PTR1_RET(rndFunc);

   /* test private/public keys */
   IPP_BAD_PTR2_RET(pPrvKey, pPubKey);
   pPrvKey = (IppsBigNumState*)( IPP_ALIGNED_PTR(pPrvKey, BN_ALIGNMENT) );
   pPubKey = (IppsBigNumState*)( IPP_ALIGNED_PTR(pPubKey, BN_ALIGNMENT) );
   IPP_BADARG_RET(!BN_VALID_ID(pPrvKey), ippStsContextMatchErr);
   IPP_BADARG_RET(!BN_VALID_ID(pPubKey), ippStsContextMatchErr);
   IPP_BADARG_RET(DLP_BITSIZER(pDL)>BITSIZE(BNU_CHUNK_T)*BN_ROOM(pPrvKey), ippStsRangeErr);
   IPP_BADARG_RET(DLP_BITSIZEP(pDL)>BITSIZE(BNU_CHUNK_T)*BN_ROOM(pPubKey), ippStsRangeErr);

   {
      /*
      // generate random private key X:  0 < X < R
      */
      BNU_CHUNK_T* pOrder = DLP_R(pDL);
      cpSize ordBitSize = DLP_BITSIZER(pDL);
      cpSize ordLen = BITS_BNU_CHUNK(ordBitSize);
      BNU_CHUNK_T xMask = MASK_BNU_CHUNK(ordBitSize);

    //BNU_CHUNK_T* pY = BN_NUMBER(pPubKey);
      BNU_CHUNK_T* pX = BN_NUMBER(pPrvKey);

    //gsModEngine* pME = DLP_MONTP0(pDL);

      do {
         rndFunc((Ipp32u*)pX, ordBitSize, pRndParam);
         pX[ordLen-1] &= xMask;
      } while( cpEqu_BNU_CHUNK(pX, ordLen, 0) || cpCmp_BNU(pX,ordLen, pOrder,ordLen)>=0 );
      BN_SIZE(pPrvKey) = ordLen;
      BN_SIGN(pPrvKey) = ippBigNumPOS;

      /*
      // compute public key: G^prvKey (mod P)
      */
      cpMontExpBin_BN_sscm(pPubKey, DLP_GENC(pDL), pPrvKey, DLP_MONTP0(pDL));
      cpMontDec_BN(pPubKey, pPubKey, DLP_MONTP0(pDL));

      return ippStsNoErr;
   }
}
