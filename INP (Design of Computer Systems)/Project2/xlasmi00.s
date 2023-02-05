; Author : Michal Ľaš


; Projekt 2 - INP 2022
; Vernamova sifra na architekture MIPS64

; DATA SEGMENT
                .data
login:          .asciiz "xlasmi00"  ; my login
cipher:         .space  17  ; encrypted login

params_sys5:    .space  8



; CODE SEGMENT
                .text


main:   
                ; KEY_1 = 12, KEY_2 = -1

                xor  r1, r1, r1         ; r1 = index = 0

                ; MAIN LOOP
loop:
                lb r6, login(r1)        ; r6 = curr_char = login[index]

                addi r4, r6, -96        ; if (isalpha(curr_char)) => continue | else => end 
                bgez r4, is_alpha
                b end
is_alpha:
                xor     r4, r4, r4      ; tmp = 0
                andi    r4, r1, 1       ; if (index % 2 == 0) => use_key_1 | else => use_key_2
                bnez    r4, use_key_2
;use_key_1:
                xor     r4, r4, r4      ; tmp = 0
                addi    r6, r6, 12      ; curr_char = curr_char + KEY_1

                addi    r4, r6, -122    ; if ((curr_char - 122) > 0) => Overwfol | else => continue
                bgez    r4, Overfolw 
                b       continue
Overfolw:
                beqz    r4, continue    ;if ((curr_char - 122) == 0) => continue | else => Overflow
                addi    r6, r6, -26     ; curr_char = curr_char - 26
                b       continue
use_key_2:
                xor     r4, r4, r4      ; tmp = 0
                addi    r6, r6, -1      ; curr_char = curr_char - KEY_2

                addi    r4, r6, -97     ; if ((curr_char - 97) > 0) => continue | else => Underflow
                bgez    r4, continue
Underflow:
                beqz    r4, continue    ; if ((curr_char -97) == 0) => continue | else => Underflow
                addi    r6, r6, 26      ; curr_char = curr_char + 26

continue:      

                sb      r6, cipher(r1)  ; put r6 to cipher[index]
                addi    r1, r1, 1       ; index++
                b       loop

end:
                addi    r1, r1, 1       ; index++
                sb      r0, cipher(r1)

                xor     r4, r4, r4      ; tmp = 0
                daddi   r4, r0, cipher
                jal     print_string    ; print with print_string

                syscall 0   ; halt

print_string:
                sw      r4, params_sys5(r0)
                daddi   r14, r0, params_sys5 
                syscall 5
                jr      r31