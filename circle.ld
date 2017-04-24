/*
 * memmap
 */

SECTIONS
{
	. = 0x8000;
	
	.text : {
		*(.text*)
	}

	_etext = .; /* Can be self-defined - means end of text. */

	.rodata : {
		*(.rodata*)
	}

	__init_start = .;

	.init_array : {
		*(.init_array*)
	}

	__init_end = .;

	.data : {
		*(.data*)
	}

	__bss_start = .;

	.bss : {
		*(.bss*)
	}

	_end = .;
}
