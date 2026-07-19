# Intermediate Representation

## Operations

| Operation                       | Dest     | Source   | Imm      | Cmp | BrDst    | Func | Notes |
|---------------------------------|----------|----------|----------|-----|----------|------|-------|
| NOP                             |          |          |          |     |          |      |
| Unary:                          |          |          |          |     |          |      |
| &nbsp;&nbsp;&nbsp;&nbsp;CONVERT | 1        | 1        |          |     |          |      | Src and Dst must be different types
| &nbsp;&nbsp;&nbsp;&nbsp;BITCAST | 1        | 1        |          |     |          |      | Src and Dst must be one type
| &nbsp;&nbsp;&nbsp;&nbsp;COPY    | 1        | 0 \|\| 1 | optional |     |          |      | Source can be VReg or Immediate. Src and Dst must be one type
| &nbsp;&nbsp;&nbsp;&nbsp;NEG     | 1        | 1        |          |     |          |      | Src and Dst must be one type
| Binary:                         |          |          |          |     |          |      | Src and Dst must be one type
| &nbsp;&nbsp;&nbsp;&nbsp;ADD     | 1        | 2        |          |     |          |      |
| &nbsp;&nbsp;&nbsp;&nbsp;SUB     | 1        | 2        |          |     |          |      |
| &nbsp;&nbsp;&nbsp;&nbsp;MUL     | 1        | 2        |          |     |          |      |
| &nbsp;&nbsp;&nbsp;&nbsp;DIV     | 1        | 2        |          |     |          |      |
| &nbsp;&nbsp;&nbsp;&nbsp;REM     | 1        | 2        |          |     |          |      |
| &nbsp;&nbsp;&nbsp;&nbsp;SHL     | 1        | 1        | int      |     |          |      | Int only
| &nbsp;&nbsp;&nbsp;&nbsp;SHR     | 1        | 1        | int      |     |          |      | Int only
| &nbsp;&nbsp;&nbsp;&nbsp;AND     | 1        | 2        |          |     |          |      | Int only
| &nbsp;&nbsp;&nbsp;&nbsp;OR      | 1        | 2        |          |     |          |      | Int only
| &nbsp;&nbsp;&nbsp;&nbsp;XOR     | 1        | 2        |          |     |          |      | Int only
| &nbsp;&nbsp;&nbsp;&nbsp;CMP     | 1        | 2        |          | +   |          |      | Returns 0 or 1 in Int VReg. Src must be one type
| RET                             |          | 0 \|\| 1 |          |     |          |      |
| BR                              |          | 0 \|\| 1 |          |     | 1 \|\| 2 |      | Branch taken if Src[0] VReg != 0. Unconditional if no VReg. BrDst[0] is fallthrough
| LOAD                            | 1        | 1        |          |     |          |      | Src[0] - address
| STORE                           |          | 2        |          |     |          |      | Src[0] - address, Src[1] - value
| CALL                            | 0 \|\| 1 | >= 0     |          |     |          |      |
| PHI                             | 1        | >= 2     |          |     |          |      | Source VirtRegister's must have DefBlock
| FUNC_DEF                        | >= 0     |          |          |     |          |      | Dst contains VReg's for function arguments
| ALLOCA                          | 1        | 0 \|\| 1 | 1        |     |          |      | Dst contains address, Source or Immediate contains size in **bytes**. Src and Dst must be Int

## Notes

Each BasicBlock must end with terminal instruction: `RET` or `BR`

### Loops

`while` and `for` must be generated in `do while` style:

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

Function's first BasicBlock must start with `FUNC_DEF`, because it introduces virual registers for arguments

### Alloca

MIR builder can only parse `ALLOCA` with Immediate argument. VReg must be folded earlier
