OUTPUT_FORMAT("elf32-powerpcle")
OUTPUT_ARCH(powerpc:common)

ENTRY(_main)

SECTIONS {
	. = 0x00401000;
	. = ALIGN(4096);
	__executable_start = .;

	.text : ALIGN(4096) { *(.text) *(.text.*) }
	.sdata : ALIGN(4096) { *(.sdata) *(.sdata.*) }
	.data : ALIGN(4096) { *(.data) *(.data.*) }
	script_data : ALIGN(4096) { *(script_data) *(script_data.*) }
	.idata : ALIGN(4096) { *(.idata) *(.idata.*) }
	.pdata : ALIGN(4096) { *(.pdata) *(.pdata.*) }
	.rodata : ALIGN(4096) { *(.rodata) *(.rodata.*) }
	.got : ALIGN(4096) { *(.got) *(.got.*) }
	.got1 : ALIGN(4096) { *(.got1) *(.got1.*) }
	.eh_frame : ALIGN(4096) { *(.eh_frame) *(.eh_frame.*) }
	.eh_frame_hdr : ALIGN(4096) { *(.eh_frame_hdr) *(.eh_frame_hdr.*) }

	. = ALIGN(4096);
	__self_end = .;

	__bss_start = .;
	.bss : ALIGN(4096) { *(.bss .bss.*) } :NONE = 0
	.sbss : ALIGN(4096) { *(.sbss) }
	__bss_end = .;

	. = ALIGN(4096);
	_end = .;
}