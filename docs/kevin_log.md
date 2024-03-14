# Kevin's Tmp Debugging Notes

- For test 505, using `result = result + arr[i]` gives a different gen_asm compared to `result += arr[i]`, so you'll have to check if `+=` works properly / **efficiently**
- Let's try to make it not register spill? I don't want to allocate the extra s registers
- I don't know if assignment works: `int arr[] = {1,2};` so you'll have to write a test for that
- `int i` gives random parse error (the photo in dms)

Current problem:
- Array doesn't seem to be working when passing in as function argument, unless it is being passed in as a pointer
