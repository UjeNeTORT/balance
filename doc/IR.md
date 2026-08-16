# Intermediate Representation

## Operations

| Operation                       | Dest     | Source   | Imm      | Cmp | BrDst    | Func | Notes |
|---------------------------------|----------|----------|----------|-----|----------|------|-------|
| NOP                             |          |          |          |     |          |      |
| Unary:                          |          |          |          |     |          |      |
| &nbsp;&nbsp;&nbsp;&nbsp;CONVERT | 1        | 1        |          |     |          |      | Src and Dst must be different types
| &nbsp;&nbsp;&nbsp;&nbsp;BITCAST | 1        | 1        |          |     |          |      | Src and Dst must be one type
| &nbsp;&nbsp;&nbsp;&nbsp;COPY    | 1        | 0 \|\| 1 | 1 \|\| 0 |     |          |      | Source can be VReg or Immediate. Src and Dst must be one type. String Immediate represents label for global variable
| &nbsp;&nbsp;&nbsp;&nbsp;NEG     | 1        | 1        |          |     |          |      | Src and Dst must be one type
| Binary:                         |          |          |          |     |          |      | Src and Dst must be one type
| &nbsp;&nbsp;&nbsp;&nbsp;ADD     | 1        | 2        |          |     |          |      |
| &nbsp;&nbsp;&nbsp;&nbsp;SUB     | 1        | 2        |          |     |          |      |
| &nbsp;&nbsp;&nbsp;&nbsp;MUL     | 1        | 2        |          |     |          |      |
| &nbsp;&nbsp;&nbsp;&nbsp;DIV     | 1        | 2        |          |     |          |      |
| &nbsp;&nbsp;&nbsp;&nbsp;REM     | 1        | 2        |          |     |          |      | Int only
| &nbsp;&nbsp;&nbsp;&nbsp;SHL     | 1        | 2        |          |     |          |      | Int only. Shift in Src[1]
| &nbsp;&nbsp;&nbsp;&nbsp;SHR     | 1        | 2        |          |     |          |      | Int only. Shift in Src[1]
| &nbsp;&nbsp;&nbsp;&nbsp;AND     | 1        | 2        |          |     |          |      | Int only
| &nbsp;&nbsp;&nbsp;&nbsp;OR      | 1        | 2        |          |     |          |      | Int only
| &nbsp;&nbsp;&nbsp;&nbsp;XOR     | 1        | 2        |          |     |          |      | Int only
| RET                             |          | 0 \|\| 1 |          |     |          |      |
| BR                              |          | 0 \|\| 2 |          | +   | 1 \|\| 2 |      | Branch taken if Src[1] `Cmp` Src[2] is true. Unconditional if no VReg and Cmp. BrDst[0] is fallthrough
| LOAD                            | 1        | 1        |          |     |          |      | Src[0] - absolute address
| STORE                           |          | 2        |          |     |          |      | Src[0] - absolute address, Src[1] - value
| CALL                            | 0 \|\| 1 | >= 0     |          |     |          | +    |
| PHI                             | 1        | 2        |          |     |          |      | Source VirtRegister's must have DefBlock
| FUNC_DEF                        | >= 1     |          |          |     |          |      | Dst[0] - stack frame start address. Dst[>=1] are VReg's for function arguments

## Notes

Each BasicBlock must end with terminal instruction: `RET` or `BR`

### Loops

`while` and `for` must be generated in `do while` manner:

```
    %0 = CMP...
    BR Skip %0 EmptyBB
EmptyBB:
    BR LoopHeader
LoopHeader:
    ...
    %1 = CMP...
    BR Skip %1 LoopHeader

Skip:
    ...
```

This is conventional for loop invariant code motion optimizations. They will move instructions in `EmptyBB`

### Functions

Function's first BasicBlock must start with `FUNC_DEF`, because it introduces virual registers for arguments. One function can have only one `FUNC_DEF`

Also it introduces stack frame start address and allocates it (substracts from `SP`)

Stack frame deallocation is performed before each `RET`

Return address, according to RISC-V ABI, is stored in `ra`. To save it, it is pushed to stack before each `CALL` and popped after it

Return value is returned in `a0`

