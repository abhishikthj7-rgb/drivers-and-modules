## Improvements from v1 to v2

- Introduced per-open device state using `struct my_state` instead of global variables  
  Each file instance now maintains its own buffer and offset, enabling isolation between multiple users/processes.

- Replaced static global buffer with dynamic allocation (`kmalloc`)  
  Memory is now allocated at `open()` time and freed at `release()`, aligning with kernel resource lifecycle practices.

- Used `file->private_data` to store driver-specific state  
  This is the standard kernel mechanism to associate per-file context with operations like read/write.

- Added `.owner = THIS_MODULE` in `file_operations`  
  Prevents module from being unloaded while it is still in use, ensuring safety.

- Implemented `release()` to clean up allocated memory  
  Ensures no memory leaks by freeing per-instance state when the file is closed.

- Removed dependency on null-terminated strings  
  Switched to length-based handling (`state->curr`), treating buffer as raw bytes, which is the correct kernel approach.

- Safer printk using precision specifier `%.*s`  
  Avoids reliance on `'\0'` and prevents buffer over-read when printing.

- Improved buffer handling logic  
  Removed unnecessary `-1` reservation for null terminator, utilizing full buffer capacity correctly.

- Refactored read/write paths to use per-instance buffer  
  All operations now use `state->buf` instead of shared memory, improving correctness and scalability.

- Added debug visibility with state pointer logging  
  Helps verify that each open gets a unique state, useful during development and debugging.
