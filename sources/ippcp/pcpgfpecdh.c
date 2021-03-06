/*******************************************************************************
* Copyright 2016-2019 Intel Corporation
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
//     Intel(R) Integrated Performance Primitives. Cryptography Primitives.
// 
//     Context:
//        ippsGFpECSharedSecretDH()
//
*/

#include "owndefs.h"
#include "owncp.h"
#include "pcpgfpecstuff.h"





/*F*
//    Name: ippsGFpECSharedSecretDHC
//
// Purpose: Compute Shared Secret (Diffie-Hellman)
//
// Returns:                   Reason:
//    ippStsNullPtrErr           NULL == pEC
//                               NULL == pPrivateA
//                               NULL == pPublicB
//                               NULL == pShare
//
//    ippStsContextMatchErr      illegal pEC->idCtx
//                               pEC->subgroup == NULL
//                               illegal pPrivateA->idCtx
//                               illegal pPublicB->idCtx
//                               illegal pShare->idCtx
//
//    ippStsRangeErr             not enough room for share key
//
//    ippStsShareKeyErr          (infinity) => z
//
//    ippStsNoErr                no errors
//
// Parameters:
//    pPrivateA        pointer to own   private key
//    pPublicB         pointer to alien public  key
//    pShare           pointer to the shared secret value
//    pEC              pointer to the EC context
//    pScratchBuffer   pointer to the scratch buffer
//
*F*/
IPPFUN(IppStatus, ippsGFpECSharedSecretDH,(const IppsBigNumState* pPrivateA, const IppsGFpECPoint* pPublicB,
                                           IppsBigNumState* pShare,
                                           IppsGFpECState* pEC, Ipp8u* pScratchBuffer))
{
   IppsGFpState*  pGF;
   gsModEngine* pGFE;

   /* EC context and buffer */
   IPP_BAD_PTR2_RET(pEC, pScratchBuffer);
   pEC = (IppsGFpECState*)( IPP_ALIGNED_PTR(pEC, ECGFP_ALIGNMENT) );
   IPP_BADARG_RET(!ECP_TEST_ID(pEC), ippStsContextMatchErr);
   IPP_BADARG_RET(!ECP_SUBGROUP(pEC), ippStsContextMatchErr);

   pGF = ECP_GFP(pEC);
   pGFE = GFP_PMA(pGF);

   /* test private (own) key */
   IPP_BAD_PTR1_RET(pPrivateA);
   pPrivateA = (IppsBigNumState*)( IPP_ALIGNED_PTR(pPrivateA, ALIGN_VAL) );
   IPP_BADARG_RET(!BN_VALID_ID(pPrivateA), ippStsContextMatchErr);

   /* test public (other party) key */
   IPP_BAD_PTR1_RET(pPublicB);
   IPP_BADARG_RET( !ECP_POINT_TEST_ID(pPublicB), ippStsContextMatchErr );

   /* test share secret value */
   IPP_BAD_PTR1_RET(pShare);
   pShare = (IppsBigNumState*)( IPP_ALIGNED_PTR(pShare, ALIGN_VAL) );
   IPP_BADARG_RET(!BN_VALID_ID(pShare), ippStsContextMatchErr);
   IPP_BADARG_RET((BN_ROOM(pShare)<GFP_FELEN(pGFE)), ippStsRangeErr);

   {
      int elmLen = GFP_FELEN(pGFE);

      IppsGFpElement elm;
      IppsGFpECPoint T;
      int finite_point;

      /* T = [privateA]pPublicB */
      cpEcGFpInitPoint(&T, cpEcGFpGetPool(1, pEC),0, pEC);
      gfec_MulPoint(&T, pPublicB, BN_NUMBER(pPrivateA), BN_SIZE(pPrivateA), /*ECP_ORDBITSIZE(pEC),*/ pEC, pScratchBuffer);

      /* share = T.x */
      cpGFpElementConstruct(&elm, cpGFpGetPool(1, pGFE), elmLen);
      finite_point = gfec_GetPoint(GFPE_DATA(&elm), NULL, &T, pEC);

      if(finite_point) {
         BNU_CHUNK_T* pShareData = BN_NUMBER(pShare);
         int nsShare = BN_ROOM(pShare);
         /* share = decode(T.x) */
         GFP_METHOD(pGFE)->decode(pShareData, GFPE_DATA(&elm), pGFE);
         cpGFpElementPadd(pShareData+elmLen, nsShare-elmLen, 0);

         BN_SIGN(pShare) = ippBigNumPOS;
         FIX_BNU(pShareData, nsShare);
         BN_SIZE(pShare) = nsShare;
      }

      cpGFpReleasePool(1, pGFE);
      cpEcGFpReleasePool(1, pEC);

      return finite_point? ippStsNoErr : ippStsShareKeyErr;
   }
}
