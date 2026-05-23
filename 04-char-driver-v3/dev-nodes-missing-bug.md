## Device Nodes Not Appearing in Userspace

### Problem
After booting the minimal Linux system, device nodes (e.g., `/dev/mychar`) were not visible in userspace, even though the driver was successfully loaded.

### Root Cause
The `/dev` directory was not backed by a proper device filesystem. Without a mounted device filesystem, the kernel cannot expose device nodes to userspace.

### Solution
Mount `devtmpfs` in the init script:

```bash
mount -t devtmpfs none /dev
```

### Explanation

- devtmpfs is a kernel-managed filesystem that automatically populates /dev with device nodes.
- It eliminates the need for manual mknod in most cases.
- Required in minimal systems (e.g., BusyBox-based rootfs) where udev is not running.

### Result

- After mounting devtmpfs, device nodes appear correctly in /dev, and user applications can interact with the character driver.

### Minimal init script example

```bash
#! /bin/sh
mount -t proc none /proc
mount -t sysfs none /sys
mount -t devtmpfs none /dev

echo "Hello from your own linux system!"
exec /bin/sh
```
