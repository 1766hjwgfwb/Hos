extern void console_init();
extern void gdt_init();
extern void interrupt_init();
extern void clock_init();
extern void hang();


char message[] = "Hello, Hos!";
char buf[1024];


void print_message() {
    printk("\n");
    printk("\t\t\t  _   _   ____   _____ \n");
    printk("\t\t\t | | | | / ___| | ____|\n");
    printk("\t\t\t | |_| | \\___ \\ |  _|  \n");
    printk("\t\t\t |  _  |  ___) || |___ \n");
    printk("\t\t\t |_| |_| |____/ |_____|\n");
    printk("\n");
}


void kernel_init() {
    console_init();

    gdt_init();

    interrupt_init();
    // task_init();
    clock_init();

    asm volatile("sti");
    hang();

    return;
}
