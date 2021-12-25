;------------------------------------------------------------------------------
;
; Copyright (c) 2006 - 2019, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
; Module Name:
;
;   SetJump.Asm
;
; Abstract:
;
;   Implementation of SetJump() on x64.
;
;------------------------------------------------------------------------------

;------------------------------------------------------------------------------
;
; Copyright (c) 2019 - 2021, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
; Abstract:
;
;   This file provides macro definitions for NASM files.
;
;------------------------------------------------------------------------------

%macro SAVEPREVSSP     0
    DB 0xF3, 0x0F, 0x01, 0xEA
%endmacro

%macro CLRSSBSY_RAX    0
    DB 0xF3, 0x0F, 0xAE, 0x30
%endmacro

%macro RSTORSSP_RAX    0
    DB 0xF3, 0x0F, 0x01, 0x28
%endmacro

%macro SETSSBSY        0
    DB 0xF3, 0x0F, 0x01, 0xE8
%endmacro

%macro READSSP_RAX     0
    DB 0xF3, 0x48, 0x0F, 0x1E, 0xC8
%endmacro

%macro INCSSP_RAX      0
    DB 0xF3, 0x48, 0x0F, 0xAE, 0xE8
%endmacro

;
; Macro for the PVALIDATE instruction, defined in AMD APM volume 3.
; NASM feature request URL: https://bugzilla.nasm.us/show_bug.cgi?id=3392753
;
%macro PVALIDATE       0
    DB 0xF2, 0x0F, 0x01, 0xFF
%endmacro

;
; Macro for the RMPADJUST instruction, defined in AMD APM volume 3.
; NASM feature request URL: https://bugzilla.nasm.us/show_bug.cgi?id=3392754
;
%macro RMPADJUST       0
    DB 0xF3, 0x0F, 0x01, 0xFE
%endmacro

; NASM provides built-in macros STRUC and ENDSTRUC for structure definition.
; For example, to define a structure called mytype containing a longword,
; a word, a byte and a string of bytes, you might code
;
; struc   mytype
;
;  mt_long:      resd    1
;  mt_word:      resw    1
;  mt_byte:      resb    1
;  mt_str:       resb    32
;
; endstruc
;
; Below macros are help to map the C types and the RESB family of pseudo-instructions.
; So that the above structure definition can be coded as
;
; struc   mytype
;
;  mt_long:      CTYPE_UINT32    1
;  mt_word:      CTYPE_UINT16    1
;  mt_byte:      CTYPE_UINT8     1
;  mt_str:       CTYPE_CHAR8     32
;
; endstruc
%define CTYPE_UINT64    resq
%define CTYPE_INT64     resq
%define CTYPE_UINT32    resd
%define CTYPE_INT32     resd
%define CTYPE_UINT16    resw
%define CTYPE_INT16     resw
%define CTYPE_BOOLEAN   resb
%define CTYPE_UINT8     resb
%define CTYPE_CHAR8     resb
%define CTYPE_INT8      resb

%define CTYPE_UINTN     resq
%define CTYPE_INTN      resq


    DEFAULT REL
    SECTION .text

extern ASM_PFX(InternalAssertJumpBuffer)
extern ASM_PFX(PcdGet32 (PcdControlFlowEnforcementPropertyMask))

;------------------------------------------------------------------------------
; UINTN
; EFIAPI
; SetJump (
;   OUT     BASE_LIBRARY_JUMP_BUFFER  *JumpBuffer
;   );
;------------------------------------------------------------------------------
global ASM_PFX(SetJump)
ASM_PFX(SetJump):
    push    rcx
    add     rsp, -0x20
    call    ASM_PFX(InternalAssertJumpBuffer)
    add     rsp, 0x20
    pop     rcx
    pop     rdx

    xor     rax, rax
    mov     [rcx + 0xF8], rax            ; save 0 to SSP

    mov     eax, [ASM_PFX(PcdGet32 (PcdControlFlowEnforcementPropertyMask))]
    test    eax, eax
    jz      CetDone
    mov     rax, cr4
    bt      eax, 23                      ; check if CET is enabled
    jnc     CetDone

    mov     rax, 1
    INCSSP_RAX                           ; to read original SSP
    READSSP_RAX
    mov     [rcx + 0xF8], rax            ; save SSP

CetDone:

    mov     [rcx], rbx
    mov     [rcx + 8], rsp
    mov     [rcx + 0x10], rbp
    mov     [rcx + 0x18], rdi
    mov     [rcx + 0x20], rsi
    mov     [rcx + 0x28], r12
    mov     [rcx + 0x30], r13
    mov     [rcx + 0x38], r14
    mov     [rcx + 0x40], r15
    mov     [rcx + 0x48], rdx
    ; save non-volatile fp registers
    stmxcsr [rcx + 0x50]
    movdqu  [rcx + 0x58], xmm6
    movdqu  [rcx + 0x68], xmm7
    movdqu  [rcx + 0x78], xmm8
    movdqu  [rcx + 0x88], xmm9
    movdqu  [rcx + 0x98], xmm10
    movdqu  [rcx + 0xA8], xmm11
    movdqu  [rcx + 0xB8], xmm12
    movdqu  [rcx + 0xC8], xmm13
    movdqu  [rcx + 0xD8], xmm14
    movdqu  [rcx + 0xE8], xmm15
    xor     rax, rax
    jmp     rdx

