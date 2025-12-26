SECTIONS {
    . = 0x800000;

    .startup : {
        .tmp/startup.o (.text);
    }

    .app : {
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
