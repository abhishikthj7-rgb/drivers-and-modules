# Linux Drivers and Kernel Modules

This repository documents my hands-on journey into Linux kernel development, with a focus on building and understanding device drivers from scratch.

## Repository Structure

- Each directory represents an incremental step in learning and building kernel modules and drivers.
- Example progression:
```bash
01-hello-world/
02-char-driver-v1/
03-char-driver-v2/
04-char-driver-v3/
```

## What this repository covers

- Kernel module basics (module_init, module_exit)
- Kernel logging using printk
- Character driver fundamentals
- Device number allocation (alloc_chrdev_region)
- cdev structure and registration
- Device creation (class_create, device_create)
- User - Kernel communication (read, write)
- Memory handling inside kernel space
- Debugging real kernel issues

## Key Learning Approach

This repository follows an incremental and experiment-driven approach:

- Each version builds on the previous one
- Bugs are documented and fixed
- Concepts are validated through execution (QEMU / real environment)

## How to build and run

### Environment setup
- Not recommended to insert module to the host OS, as faulty kernel code can potentially crash the device
- Running modules in qemu is a safer alternative

### Build
- Modify the KDIR value in the makefile to match with your cloned linux source tree directory
- Navigate to the desired module directory and run:
```bash
make
```

### Run
- Once inside QEMU, run:
```bash
insmod <module_name>.ko
```

### Verify
```bash
dmesg
```

## Current focus
- Strengthening character driver fundamentals

## Debugging insights
- Errors encountered during development are documented in their respective module directories
