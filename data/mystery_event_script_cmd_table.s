	.include "asm/macros/bit_width.inc"
	.section script_data, "aw"

	.align 2
gMysteryEventScriptCmdTable::
	ptrvalue MEScrCmd_nop                 /* 0x00*/
	ptrvalue MEScrCmd_checkcompat         /* 0x01*/
	ptrvalue MEScrCmd_end                 /* 0x02*/
	ptrvalue MEScrCmd_setmsg              /* 0x03*/
	ptrvalue MEScrCmd_setstatus           /* 0x04*/
	ptrvalue MEScrCmd_runscript           /* 0x05*/
	ptrvalue MEScrCmd_initramscript       /* 0x06*/
	ptrvalue MEScrCmd_setenigmaberry      /* 0x07*/
	ptrvalue MEScrCmd_giveribbon          /* 0x08*/
	ptrvalue MEScrCmd_givenationaldex     /* 0x09*/
	ptrvalue MEScrCmd_addrareword         /* 0x0a*/
	ptrvalue MEScrCmd_setrecordmixinggift /* 0x0b*/
	ptrvalue MEScrCmd_givepokemon         /* 0x0c*/
	ptrvalue MEScrCmd_addtrainer          /* 0x0d*/
	ptrvalue MEScrCmd_enableresetrtc      /* 0x0e*/
	ptrvalue MEScrCmd_checksum            /* 0x0f*/
	ptrvalue MEScrCmd_crc                 /* 0x10*/
gMysteryEventScriptCmdTableEnd::
