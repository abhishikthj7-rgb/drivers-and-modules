### Error

```bash
make
make -C ~/Downloads/linux M=/home/abhishik/Downloads/repos/drivers-and-modules/04-char-driver-v3 modules
make[1]: Entering directory '/home/abhishik/Downloads/linux'
make[2]: Entering directory '/home/abhishik/Downloads/repos/drivers-and-modules/04-char-driver-v3'
  CC [M]  char-dr-v3.o
char-dr-v3.c:14: error: "pr_fmt" redefined [-Werror]
   14 | #define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
      | 
In file included from /home/abhishik/Downloads/linux/include/asm-generic/bug.h:31,
                 from /home/abhishik/Downloads/linux/arch/x86/include/asm/bug.h:193,
                 from /home/abhishik/Downloads/linux/arch/x86/include/asm/alternative.h:9,
                 from /home/abhishik/Downloads/linux/arch/x86/include/asm/barrier.h:5,
                 from /home/abhishik/Downloads/linux/include/linux/list.h:11,
                 from /home/abhishik/Downloads/linux/include/linux/module.h:12,
                 from char-dr-v3.c:1:
/home/abhishik/Downloads/linux/include/linux/printk.h:401: note: this is the location of the previous definition
  401 | #define pr_fmt(fmt) fmt
      | 
cc1: all warnings being treated as errors
make[4]: *** [/home/abhishik/Downloads/linux/scripts/Makefile.build:289: char-dr-v3.o] Error 1
make[3]: *** [/home/abhishik/Downloads/linux/Makefile:2141: .] Error 2
make[2]: *** [/home/abhishik/Downloads/linux/Makefile:248: __sub-make] Error 2
make[2]: Leaving directory '/home/abhishik/Downloads/repos/drivers-and-modules/04-char-driver-v3'
make[1]: *** [Makefile:248: __sub-make] Error 2
make[1]: Leaving directory '/home/abhishik/Downloads/linux'
make: *** [Makefile:7: all] Error 2
```
### Cause
- Kernel headers have already defined pr_fmt
- It is redefined later in the module after including kernel headers, which triggers: -Werror
- Werror -> treats warning as error -> build fails

### Correct Usage
- pr_fmt must be defined BEFORE any kernel headers are included.
