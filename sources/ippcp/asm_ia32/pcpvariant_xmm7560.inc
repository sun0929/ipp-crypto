;===============================================================================
; Copyright 2016-2019 Intel Corporation
; All Rights Reserved.
;
; If this  software was obtained  under the  Intel Simplified  Software License,
; the following terms apply:
;
; The source code,  information  and material  ("Material") contained  herein is
; owned by Intel Corporation or its  suppliers or licensors,  and  title to such
; Material remains with Intel  Corporation or its  suppliers or  licensors.  The
; Material  contains  proprietary  information  of  Intel or  its suppliers  and
; licensors.  The Material is protected by  worldwide copyright  laws and treaty
; provisions.  No part  of  the  Material   may  be  used,  copied,  reproduced,
; modified, published,  uploaded, posted, transmitted,  distributed or disclosed
; in any way without Intel's prior express written permission.  No license under
; any patent,  copyright or other  intellectual property rights  in the Material
; is granted to  or  conferred  upon  you,  either   expressly,  by implication,
; inducement,  estoppel  or  otherwise.  Any  license   under such  intellectual
; property rights must be express and approved by Intel in writing.
;
; Unless otherwise agreed by Intel in writing,  you may not remove or alter this
; notice or  any  other  notice   embedded  in  Materials  by  Intel  or Intel's
; suppliers or licensors in any way.
;
;
; If this  software  was obtained  under the  Apache License,  Version  2.0 (the
; "License"), the following terms apply:
;
; You may  not use this  file except  in compliance  with  the License.  You may
; obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
;
;
; Unless  required  by   applicable  law  or  agreed  to  in  writing,  software
; distributed under the License  is distributed  on an  "AS IS"  BASIS,  WITHOUT
; WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
;
; See the   License  for the   specific  language   governing   permissions  and
; limitations under the License.
;===============================================================================

;               Intel(R) Integrated Performance Primitives
;                   Cryptographic Primitives (ippcp)
;
;     Purpose:
;        Update standard ippCP variant
;        for code size optimization
;

%ifdef _XMM7560_
%if (_IPP >= _IPP_P8)

%assign OFF 0
%assign ON  1

;%assign _USE_C_cpAdd_BNU_  ON
;%assign _USE_C_cpSub_BNU_  ON
;%assign _USE_C_cpInc_BNU_  ON
;%assign _USE_C_cpAddMulDgt_BNU_  ON
;%assign _USE_C_cpSubMulDgt_BNU_  ON
%assign _USE_C_cpMulAdc_BNU_school_  ON
%assign _USE_C_cpSqrAdc_BNU_school_  ON
%assign _USE_C_cpMontRedAdc_BNU_     ON

%assign _DISABLE_ECP_256R1_HARDCODED_BP_TBL_  OFF
%assign _DISABLE_ECP_384R1_HARDCODED_BP_TBL_  OFF

%endif
%endif
