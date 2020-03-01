cmake_minimum_required(VERSION 2.8.8)
project(goldendict-locale LANGUAGES C CXX)


foreach(X
	ar_SA	ay_WI	be_BY	be_BY@latin	bg_BG	cs_CZ	de_DE	el_GR	eo_EO
	es_BO	es_ES	fa_IR	fi_FI	fr_FR	it_IT	ja_JP	ko_KR	lt_LT
	nl_NL	pl_PL	pt_BR	qu_WI	ru_RU	sk_SK	sq_AL	sr_SR	sv_SE
	tk_TM	tr_TR	uk_UA	vi_VN	zh_CN	zh_TW
)
	add_custom_command(OUTPUT ${X}.qm
		COMMAND lrelease 
		${CMAKE_CURRENT_SOURCE_DIR}/${X}.ts
		 ${X}.qm
	)

endforeach(X)

set( LOCALE_FILES
	ar_SA.qm
	ay_WI.qm
	be_BY.qm
	be_BY@latin.qm
	bg_BG.qm
	cs_CZ.qm
	de_DE.qm
	el_GR.qm
	eo_EO.qm
	es_AR.qm
	es_BO.qm
	es_ES.qm
	fa_IR.qm
	fi_FI.qm
	fr_FR.qm
	it_IT.qm
	ja_JP.qm
	ko_KR.qm
	lt_LT.qm
	mk_MK.qm
	nl_NL.qm
	pl_PL.qm
	pt_BR.qm
	qu_WI.qm
	ru_RU.qm
	sk_SK.qm
	sq_AL.qm
	sr_SR.qm
	sv_SE.qm
	tg_TJ.qm
	tk_TM.qm
	tr_TR.qm
	uk_UA.qm
	vi_VN.qm
	zh_CN.qm
	zh_TW.qm
)
install(FILES ${LOCALE_FILES} DESTINATION share/goldendict/locale)
