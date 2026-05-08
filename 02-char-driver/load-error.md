## Kernel Module Build Mismatch Issue

### Problem

Running the following inside the Linux source tree before building an external module:

```bash
make defconfig
make modules_prepare
```
can modify the kernel configuration (.config) and related build artifacts.

If a kernel image (bzImage) was built before these commands, and a module is built after, they may become incompatible.

### Symptom
when inserting module:

```bash
insmod char-dr.ko
```
observed below error:

```bash
[   35.437318] char_dr: version magic '7.1.0-rc2-00006-gb3519d93eabb SMP preemp'
insmod: can't insert 'char-dr.ko': invalid module format
```

### Root Cause
- Kernel image and module are built from different build states
- vermagic mismatch between:
  - running kernel
  - compiled module

### key insight
Kernel modules must be built against the exact same kernel build artifacts used to generate the running kernel image.

Same source tree is not enough — build state must also match.

### Correct Workflow
1.Configure Kernel
```bash
make defconfig
```
2.Build Kernel
```bash
make -j$(nproc)
```
3.Prepare for external modules
```bash
make modules_prepare
```
### Rule
Always ensure:
```bash
kernel (bzImage) build state == Module build state
```
