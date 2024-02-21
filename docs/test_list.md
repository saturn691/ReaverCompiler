### List of tests to write for compiler

Array
- For declarations that are not int, e.g. char arr[5]
- For declaration without array size, e.g. int arr[]
- I don't think global variables work properly for arrays - we might just be pseudo-passing

- Rigorous testing for pointers
- Test for double function segfaults
- All floating operations
- Some of the conditional ops

### Notes

- In for loops, `int i = 0` declaration inside the for conditions is not supported until C99
