## Overview
This is a simple Linux character device driver that demonstrates the core concepts of kernel-space to user-space communication using `read` and `write` operations.

It uses a statically allocated kernel buffer and supports basic open, read, and write functionality.

---

## Features

- **Dynamic Major Number Allocation**
  - Uses `register_chrdev(0, ...)` to dynamically obtain a major number.
  - Avoids hardcoding device numbers.

- **Basic File Operations**
  - Implements:
    - `open()`
    - `read()`
    - `write()`

- **Kernel Buffer Management**
  - Uses a fixed-size buffer (`kbuf`) of 100 bytes.
  - Maintains a `curr` pointer to track written data size.

- **Safe User-Kernel Data Transfer**
  - Uses:
    - `copy_to_user()` for read
    - `copy_from_user()` for write
  - Prevents direct unsafe memory access.

- **Offset-Based Reading**
  - Supports sequential reads using `loff_t *offset`.
  - Returns `0` when end of buffer is reached (EOF behavior).

- **Bounds Checking**
  - Prevents buffer overflow using `min()` and size checks.
  - Returns `-ENOSPC` when buffer is full.

- **Null-Termination Handling**
  - Ensures safe string printing in kernel logs.
  - Avoids undefined behavior with `%s` in `printk`.

- **Logging with printk**
  - Provides runtime visibility:
    - Device open
    - Read operations
    - Write operations
    - Initialization and cleanup
