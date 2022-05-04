# Lockless Circular Buffer
Lockless circular buffer in a Single-Producer Single-Consumer scenario.

## Demo
`clang buffer.c -o buffer -pthread`

`./buffer`

## Implementation
Every buffer node has a `void*` pointer and a status `volatile` boolean which indicates whether the node has been read or not.
_reader_ and _rriter_ can use this status flag to determine whether the buffer is full or not.

For demonstrating purposes, the _writer_ puts in the buffer random integer and the _reader_ simply prints it to `stdout`
