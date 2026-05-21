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
