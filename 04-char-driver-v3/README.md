## Improvements from v2 to v3

- Migrated from legacy `register_chrdev` to modern `cdev` interface
  Introduced `alloc_chrdev_region`, `cdev_init`, and `cdev_add`, aligning with current kernel driver model.

- Added support for multiple devices (minor numbers)
  Driver now handles 3 devices using `MKDEV(major, minor)`, improving scalability.

- Introduced device model integration (`class_create`, `device_create`)
  Automatically creates `/dev/mychar0`, `/dev/mychar1`, `/dev/mychar2`, removing need for manual `mknod`.

- Implemented proper error handling with unwind paths
  Uses `goto`-based cleanup to safely release resources in case of partial initialization failures.

- Added structured cleanup in module exit
  Ensures correct teardown order: `device_destroy` -> `class_destroy` -> `cdev_del` -> `unregister_chrdev_region`.

- Introduced `dev_t` for device number management
  Replaces manual major handling with kernel-standard device number abstraction.

- Added `pr_fmt` for consistent logging format
  Prefixes all logs with module name automatically, improving traceability.

- Improved logging practices using `pr_info` and `pr_debug`
  Replaces raw `printk` usage with level-specific logging macros.

## Learnings

- Only undo things that allocate, register, or expose to kernel/userspace
- cdev_init() does NOT need cleanup
  - Why?
  - It does not allocate memory
  - It does not register anything with the kernel
  - It just initializes fields inside struct.
```bash
cdev_init(&my_cdev, &fops);
```
