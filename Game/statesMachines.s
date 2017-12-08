 #include <msp430.h>
 #include "stateMachines.h"
 #include "led.h"

 ;The toggle_red method form stateMachines.c
 
 ;assembly 
 
    .file "stateMachines.c"

state: .byte 0
    
red_on: 
        .byte 0

;green_on:
 ;       .byte 0

       
       
       .text
jt:    .word default ; jt[0]
        .word option1; jt[1]
        .word option2; jt[2]

toggle_red:
            cmp.b #3, &state
            jnc default
            
            mov &state, r12
            add r12, r12
            mov jt(r12), r0 ;jmp jt[state]
            
            
option1: mov.b #1, &red_on
        mov #1, r12
        jmp end


option2: mov.b #0, &red_on
        mov #0, r12
        jmp end

          
default: pop #1    
end: pop r0; return

        
