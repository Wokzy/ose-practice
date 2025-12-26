SECTIONS {
    . = 0x7C00;

    .boot : {
        .tmp/boot.o (.text);
    }

    .lib : {
        .tmp/lib.o (.text);
    }

    .kernel : {
        *(.text);
        *(.rodata);
        *(.rodata.*);
        *(.data);
        *(.bss);
    }

    /DISCARD/ : {
        *(.eh_frame)
        *(.note.gnu.property)
        *(.comment)
    }
}
