SECTIONS {
    . = 0x400000;

    .startup : {
        .tmp/startup.o (.text);
    }

    .app1 : {
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
