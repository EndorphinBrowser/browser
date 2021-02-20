#!/bin/bash

FILES="ast.ts tr_TR.ts de_DE.ts hu_HU.ts es_CR.ts zh_TW.ts cs_CZ.ts sr_RS@latin.ts fr_FR.ts pt_BR.ts es.ts sr_RS.ts ms.ts da_DK.ts sk_SK.ts ru_RU.ts he_IL.ts pt_PT.ts ko_KR.ts fr_CA.ts nl.ts el_GR.ts ca.ts pl_PL.ts nb_NO.ts uk.ts zh_CN.ts gl.ts et_EE.ts fi_FI.ts it_IT.ts ja_JP.ts gl.ts"
for FILE in $FILES; do
/usr/lib/qt5/bin/lupdate @ts_lst_file -ts "src/locale/${FILE}" -noobsolete
done
