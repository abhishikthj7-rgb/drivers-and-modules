## printk log ordering / missing newline issue

### Bug
Kernel logs appeared merged, delayed, or out of order in QEMU when using `printk()` without a newline.

### Cause
`printk()` does not automatically terminate log entries. Without `\n`, the kernel log buffer treats the message as incomplete, which leads to:
- merged log lines
- incorrect visual ordering in QEMU serial output
- confusing dmesg output

### Fix
Always end printk messages with a newline:
```c
printk(KERN_INFO "Hello world!\n");
```
### Signal of completion
A newline (\n) marks the end of a kernel log entry, allowing proper:
- log flushing
- ordering in dmesg and qemu console output

