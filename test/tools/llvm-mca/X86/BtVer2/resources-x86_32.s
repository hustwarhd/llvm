# NOTE: Assertions have been autogenerated by utils/update_mca_test_checks.py
# RUN: llvm-mca -mtriple=i686-unknown-unknown -mcpu=btver2 -instruction-tables < %s | FileCheck %s

aaa

aad
aad $7

aam
aam $7

aas

bound %bx, (%eax)
bound %ebx, (%eax)

daa

das

into

leave

salc

# CHECK:      Instruction Info:
# CHECK-NEXT: [1]: #uOps
# CHECK-NEXT: [2]: Latency
# CHECK-NEXT: [3]: RThroughput
# CHECK-NEXT: [4]: MayLoad
# CHECK-NEXT: [5]: MayStore
# CHECK-NEXT: [6]: HasSideEffects (U)

# CHECK:      [1]    [2]    [3]    [4]    [5]    [6]    Instructions:
# CHECK-NEXT:  1      100   0.50                        aaa
# CHECK-NEXT:  1      100   0.50                        aad
# CHECK-NEXT:  1      100   0.50                        aad	$7
# CHECK-NEXT:  1      100   0.50                        aam
# CHECK-NEXT:  1      100   0.50                        aam	$7
# CHECK-NEXT:  1      100   0.50                        aas
# CHECK-NEXT:  1      100   0.50                  U     bound	%bx, (%eax)
# CHECK-NEXT:  1      100   0.50                  U     bound	%ebx, (%eax)
# CHECK-NEXT:  1      100   0.50                        daa
# CHECK-NEXT:  1      100   0.50                        das
# CHECK-NEXT:  1      100   0.50                  U     into
# CHECK-NEXT:  1      1     0.50    *                   leave
# CHECK-NEXT:  1      1     0.50                  U     salc

# CHECK:      Resources:
# CHECK-NEXT: [0]   - JALU0
# CHECK-NEXT: [1]   - JALU1
# CHECK-NEXT: [2]   - JDiv
# CHECK-NEXT: [3]   - JFPA
# CHECK-NEXT: [4]   - JFPM
# CHECK-NEXT: [5]   - JFPU0
# CHECK-NEXT: [6]   - JFPU1
# CHECK-NEXT: [7]   - JLAGU
# CHECK-NEXT: [8]   - JMul
# CHECK-NEXT: [9]   - JSAGU
# CHECK-NEXT: [10]  - JSTC
# CHECK-NEXT: [11]  - JVALU0
# CHECK-NEXT: [12]  - JVALU1
# CHECK-NEXT: [13]  - JVIMUL

# CHECK:      Resource pressure per iteration:
# CHECK-NEXT: [0]    [1]    [2]    [3]    [4]    [5]    [6]    [7]    [8]    [9]    [10]   [11]   [12]   [13]
# CHECK-NEXT: 6.50   6.50    -      -      -      -      -      -      -      -      -      -      -      -

# CHECK:      Resource pressure by instruction:
# CHECK-NEXT: [0]    [1]    [2]    [3]    [4]    [5]    [6]    [7]    [8]    [9]    [10]   [11]   [12]   [13]   Instructions:
# CHECK-NEXT: 0.50   0.50    -      -      -      -      -      -      -      -      -      -      -      -     aaa
# CHECK-NEXT: 0.50   0.50    -      -      -      -      -      -      -      -      -      -      -      -     aad
# CHECK-NEXT: 0.50   0.50    -      -      -      -      -      -      -      -      -      -      -      -     aad	$7
# CHECK-NEXT: 0.50   0.50    -      -      -      -      -      -      -      -      -      -      -      -     aam
# CHECK-NEXT: 0.50   0.50    -      -      -      -      -      -      -      -      -      -      -      -     aam	$7
# CHECK-NEXT: 0.50   0.50    -      -      -      -      -      -      -      -      -      -      -      -     aas
# CHECK-NEXT: 0.50   0.50    -      -      -      -      -      -      -      -      -      -      -      -     bound	%bx, (%eax)
# CHECK-NEXT: 0.50   0.50    -      -      -      -      -      -      -      -      -      -      -      -     bound	%ebx, (%eax)
# CHECK-NEXT: 0.50   0.50    -      -      -      -      -      -      -      -      -      -      -      -     daa
# CHECK-NEXT: 0.50   0.50    -      -      -      -      -      -      -      -      -      -      -      -     das
# CHECK-NEXT: 0.50   0.50    -      -      -      -      -      -      -      -      -      -      -      -     into
# CHECK-NEXT: 0.50   0.50    -      -      -      -      -      -      -      -      -      -      -      -     leave
# CHECK-NEXT: 0.50   0.50    -      -      -      -      -      -      -      -      -      -      -      -     salc
