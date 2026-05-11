### Error 

```bash
/modules # mknod /dev/mychar c 247 0 
/modules # cat /dev/mychar 
[ 153.315419] Device opened 
cat: read error: Invalid argument
```

### cause

- This line:
```c
int msg_len = strlen(msg);
```

- strlen() returns size_t (unsigned)
- msg_len int (signed)
- Later :

```c
size_t remaining = msg_len - *offset;
```

- Here we are mixing: msg_len (signed0) - offset (signed 64-bit) 
- then assigned to : size_t remaining (unsigned)
- This can silently go wrong and lead to weird behavior like the above
- declare msg_len with the type size_t


