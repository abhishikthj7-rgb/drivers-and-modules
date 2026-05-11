### ERROR

```bash
/home/mi/Downloads/linux/include/linux/compiler_types.h:699:45: error: call to ‘__compiletime_assert_380’ declared with attribute error: min(msg_len - *offset, len) signedness error 699 | _compiletime_assert(condition, msg, __compiletime_assert_, __COUNTER__) | ^
```

- What the error means
```c
min(msg_len - *offset, len) //signedness error
```
- Kernel’s min() macro is type-strict. It refuses to compare signed vs unsigned

- Variables:
```c
int msg_len;        // signed
loff_t *offset;    // signed (usually long long)
size_t len;        // unsigned
```

- For the below expression, output is signed because both the operands are signed:
```c
msg_len - *offset 
```

- During comparison:
```
min( signed , unsigned )
```

### Why above comparison is not allowed:

- C might convert signed to unsigned
- -1 or any negative number might become a huge positive number
- Kernel blocks this at compile time
- make both arguments of min the same size_t type

