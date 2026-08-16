@ The first .byte argument of each macro below is an index into gFieldEffectScriptFuncs

	.macro field_eff_loadtiles address:req
	.byte 0
	ptrvalue \address
	.endm

	.macro field_eff_loadfadedpal address:req
	.byte 1
	ptrvalue \address
	.endm

	.macro field_eff_loadpal address:req
	.byte 2
	ptrvalue \address
	.endm

	.macro field_eff_callnative address:req
	.byte 3
	ptrvalue \address
	.endm

	.macro field_eff_end
	.byte 4
	.endm

	.macro field_eff_loadgfx_callnative tiles_address:req, palette_address:req, function_address:req
	.byte 5
	ptrvalue \tiles_address
	ptrvalue \palette_address
	ptrvalue \function_address
	.endm

	.macro field_eff_loadtiles_callnative tiles_address:req, function_address:req
	.byte 6
	ptrvalue \tiles_address
	ptrvalue \function_address
	.endm

	.macro field_eff_loadfadedpal_callnative palette_address:req, function_address:req
	.byte 7
	ptrvalue \palette_address
	ptrvalue \function_address
	.endm
