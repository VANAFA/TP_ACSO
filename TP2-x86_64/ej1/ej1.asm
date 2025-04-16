; /** defines bool y puntero **/
%define NULL 0
%define TRUE 1
%define FALSE 0

section .data

section .text

global string_proc_list_create_asm
global string_proc_node_create_asm
global string_proc_list_add_node_asm
global string_proc_list_concat_asm

; FUNCIONES auxiliares que pueden llegar a necesitar:
extern malloc
extern free
extern str_concat

string_proc_list_create_asm:
    push rbp
    mov rbp, rsp
    
    push rdi
    mov rdi, 16
    call malloc
    test rax, rax
    jz .return_null

    ; list->first list->last 
    mov qword [rax], NULL   ; list->first = NULL
    mov qword [rax + 8], NULL ; list->last = NULL

    pop rdi
    mov rsp, rbp
    pop rbp
    ret

.return_null:
    xor rax, rax
    pop rdi
    mov rsp, rbp
    pop rbp
    ret

string_proc_node_create_asm:
    ; rdi = type
    ; rsi = *hash
    push rbp
    mov rbp, rsp

    push rdi
    push rsi
    mov rdi, 32
    call malloc
    test rax, rax
    jz .return_null

    ; Recupero type and hash
    pop rdx    ; rdx = hash
    pop rcx    ; rcx = type

    mov qword [rax], NULL   ; node->next = NULL
    mov qword [rax + 8], NULL ; node->previous = NULL
    mov qword [rax + 16], rcx ; node->type = type
    
    test rdx, rdx
    jz .cleanup_and_return_null
    
    mov qword [rax + 24], rdx ; node->hash = hash
    
    mov rsp, rbp
    pop rbp
    ret

.cleanup_and_return_null:
    push rax
    mov rdi, rax
    call free
    pop rax

.return_null:
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret

string_proc_list_add_node_asm:
    ; rdi = list
    ; rsi = type
    ; rdx = *hash
    push rbp
    mov rbp, rsp

    test rdi, rdi
    jz .return

    push rdi
    push rsi
    push rdx
    mov rdi, rsi
    mov rsi, rdx
    call string_proc_node_create_asm
    pop rdx
    pop rsi
    pop rdi

    test rax, rax
    jz .return

    ; add node to list
    mov r8, rdi             ; r8 = list
    mov r9, rax             ; r9 = node
    cmp qword [r8], NULL    ; list->first == NULL
    jnz .add_to_end

    ; if list empty
    mov qword [r8], r9      ; list->first = node
    mov qword [r8 + 8], r9  ; list->last = node
    jmp .return

.add_to_end:
    mov r10, qword [r8 + 8] ; r10 = list->last
    mov qword [r10], r9     ; last->next = node
    mov qword [r9 + 8], r10 ; node->previous = last
    mov qword [r8 + 8], r9  ; list->last = node

.return:
    mov rsp, rbp
    pop rbp
    ret

string_proc_list_concat_asm:
    ; rdi = list
    ; rsi = type
    ; rdx = hash
    push rbp
    mov rbp, rsp
    
    push r12
    push r13
    push r14
    push r15
    push rbx

    test rdi, rdi
    jz .return_null
    test rdx, rdx
    jz .return_null

    mov r12, rdi    ; r12 = list
    mov r13, rsi    ; r13 = type
    mov r14, rdx    ; r14 = hash

    mov r15, r14    ; r15 = prefijo
    xor rbx, rbx    ; rbx = 0

    mov rdi, r12           ; rdi = list
    mov rsi, r13           ; rsi = type
    mov rdx, r14           ; rdx = hash
    call string_proc_list_add_node_asm

    mov r8, qword [r12]     ; r8 = list->first
    test r8, r8
    jz .return_result

.loop_nodes:
    mov r9b, byte [r8 + 16] ; r9b = node->type
    cmp r9b, r13b
    jne .next_node

    mov rsi, qword [r8 + 24] ; rsi = node->hash
    cmp rsi, r14
    je .next_node
    
    mov rdi, r15           ; rdi = resultado
    push r8                ; current node
    call str_concat        ; rax = str_concat(result, node->hash)
    
    test rbx, rbx
    jz .first_concat
    
    push rax
    mov rdi, r15
    call free
    pop rax
    
.first_concat:
    pop r8
    mov r15, rax
    mov rbx, TRUE
    
.next_node:
    mov r8, qword [r8]     ; r8 = node->next
    test r8, r8
    jnz .loop_nodes

.return_result:
    mov rax, r15

.restore_and_return:
    pop rbx
    pop r15
    pop r14
    pop r13
    pop r12
    mov rsp, rbp
    pop rbp
    ret

.return_null:
    xor rax, rax
    pop rbx
    pop r15
    pop r14
    pop r13
    pop r12
    mov rsp, rbp
    pop rbp
    ret