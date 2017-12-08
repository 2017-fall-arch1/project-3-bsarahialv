 
.arch msp430g2553

//The wdt_c_handler() method from play.c
/*C code:

void wdt_c_handler()
{
  static short count = 0;
  P1OUT |= GREEN_LED;		      
  count ++;
  
   if(paddle_one == 6 ){
       //set the screen to the winner color
       clearScreen(COLOR_RED);
      
       drawString5x7(20,60, "    Winner!", COLOR_WHITE, COLOR_RED);
       drawString5x7(20,100, "    Player 1", COLOR_WHITE, COLOR_RED);
       //DESIGN BORDERS
       fillRectangle(0, 10, 10 , 10, COLOR_WHITE);
       fillRectangle(0, 30, 10 , 10, COLOR_WHITE);
       fillRectangle(0, 50, 10 , 10, COLOR_WHITE);
       fillRectangle(0, 70, 10 , 10, COLOR_WHITE);
       fillRectangle(0, 90, 10 , 10, COLOR_WHITE);
       fillRectangle(0, 110, 10 , 10, COLOR_WHITE);
       fillRectangle(0, 130, 10 , 10, COLOR_WHITE);
       fillRectangle(0, 150, 10 , 10, COLOR_WHITE);
       fillRectangle(120, 10, 10 , 10, COLOR_WHITE);
       fillRectangle(120, 30, 10 , 10, COLOR_WHITE);
       fillRectangle(120, 50, 10 , 10, COLOR_WHITE);
       fillRectangle(120, 70, 10 , 10, COLOR_WHITE);
       fillRectangle(120, 90, 10 , 10, COLOR_WHITE);
       fillRectangle(120, 110, 10 , 10, COLOR_WHITE);
       fillRectangle(120, 130, 10 , 10, COLOR_WHITE);
       fillRectangle(120, 150, 10 , 10, COLOR_WHITE);
       
       //FOR SOUND--> SOUND WHILE CLEAR SCREEN
       int count =6;
       while(count>0){
           buzzer_init();
           enableWDTInterrupts();
           buzzer_advance_frequency();
           count = count -1;
           
        }
     }
     if(paddle_two == 6){
         //set the screen to the winner color
         clearScreen(COLOR_GOLD);

         drawString5x7(20,60, "    Winner!", COLOR_BLACK, COLOR_GOLD);
         drawString5x7(20,100, "    Player 2", COLOR_BLACK, COLOR_GOLD);
        // DESIGN BORDERS 
         fillRectangle(0, 10, 10 , 10, COLOR_BLACK);
         fillRectangle(0, 30, 10 , 10, COLOR_BLACK);
         fillRectangle(0, 50, 10 , 10, COLOR_BLACK);
         fillRectangle(0, 70, 10 , 10, COLOR_BLACK);
         fillRectangle(0, 90, 10 , 10, COLOR_BLACK);
         fillRectangle(0, 110, 10 , 10, COLOR_BLACK);
         fillRectangle(0, 130, 10 , 10, COLOR_BLACK);
         fillRectangle(0, 150, 10 , 10, COLOR_BLACK);
         fillRectangle(120, 10, 10 , 10, COLOR_BLACK);
         fillRectangle(120, 30, 10 , 10, COLOR_BLACK);
         fillRectangle(120, 50, 10 , 10, COLOR_BLACK);
         fillRectangle(120, 70, 10 , 10, COLOR_BLACK);
         fillRectangle(120, 90, 10 , 10, COLOR_BLACK);
         fillRectangle(120, 110, 10 , 10, COLOR_BLACK);
         fillRectangle(120, 130, 10 , 10, COLOR_BLACK);
         fillRectangle(120, 150, 10 , 10, COLOR_BLACK);
         
         //FOR SOUND--> SOUND WHILE CLEAR SCREEN
         int count =6;
        while(count>0){
              buzzer_init();
            buzzer_advance_frequency();
            enableWDTInterrupts(); 
            count = count -1;
                   
        }

     }
     
     
  if (count == 15) {

     if(paddle_one < 6 && paddle_two < 6){
         Bouncing(&ml3,&ml1);
         Bouncing(&ml3,&ml0);
         Bouncing(&ml3,&ml4);
         mlAdvance(&ml0, &fieldFence);
         u_int switches = p2sw_read(), i;
         char str[5];
         for (i = 0; i < 4; i++)
             str[i] = (switches & (1<<i)) ? 0 : 1;
         str[4] = 0;
         
         
         //FOR SWITCHES
         //Left
         if(str[1]){
             buzzer_init2();
             enableWDTInterrupts();
             ml0.velocity.axes[0] = 0; 
             ml0.velocity.axes[1] = -5;
             
         }
         
         //Right
         if(str[0]){
             buzzer_init2();
             enableWDTInterrupts();
             ml0.velocity.axes[0] = 0; 
             ml0.velocity.axes[1] = 5;
         }
         if(str[3]){
             buzzer_init2();
             enableWDTInterrupts();
             ml1.velocity.axes[0] = 0; 
             ml1.velocity.axes[1] = -5;
         }
         if(str[2]){
             buzzer_init2();
             enableWDTInterrupts();
             ml1.velocity.axes[0] = 0; 
             ml1.velocity.axes[1] = 5;
         }
         if(!str[0] && !str[1]){
             ml0.velocity.axes[0] = 0; 
             ml0.velocity.axes[1] = 0;
         }
         if(!str[2] && !str[3]){
             ml1.velocity.axes[0] = 0; 
             ml1.velocity.axes[1] = 0;
         }
         if (p2sw_read())
             redrawScreen = 1;
         count = 0;
     }
  } 
  P1OUT &= ~GREEN_LED;		    
}*/


//assembly

    .file "play.c"
    
LED_GREEN:
        .space  1
        .global LEDS
        .size   LEDS, 1
        
P1DIR:
        .space  1
        .global P1OUT
        .size   P1OUT, 1
P1OUT:
        .space  1
    
.text
        .global paddle_one
        mov paddle_one, r12
        mov paddle_two, r13
        
count:
        .long 6
        mov count, r14
        
state 1: cmp r12, #6
        JNE OUT /*JUMP IF NOT EQUAL*/
        call clearScreen
        call drawString5x7
        call drawString5x7
        call fillRectangle
        call fillRectangle
        call fillRectangle
        call fillRectangle
        call fillRectangle
        call fillRectangle
        call fillRectangle
        call fillRectangle
        call fillRectangle
        call fillRectangle
        call fillRectangle
        call fillRectangle
        call fillRectangle
        call fillRectangle
        call fillRectangle
        call fillRectangle
        
LOOP:   cmp r14, #0
        call buzzer_init
        call enableWDTInterrupts
        call buzzer_advance_frequency
        sub #1, r14
        
        OUT: next state

state 2: cmp r13, #6
        JNE OUT /*JUMP IF NOT EQUAL*/
        call clearScreen
        call drawString5x7
        call drawString5x7
        call fillRectangle
        call fillRectangle
        call fillRectangle
        call fillRectangle
        call fillRectangle
        call fillRectangle
        call fillRectangle
        call fillRectangle
        call fillRectangle
        call fillRectangle
        call fillRectangle
        call fillRectangle
        call fillRectangle
        call fillRectangle
        call fillRectangle
        call fillRectangle
        
LOOP:   cmp r14, #0
        call buzzer_init
        call enableWDTInterrupts
        call buzzer_advance_frequency
        sub #1, r14
        
        OUT: next state


state 3: cmp r14, #15 //IF COUNT is equal to 15
        JNE DONE /*JUMP IF NOT EQUAL*/
        cmp r12, #6
        cmp r13, #6
        call Bouncing
        call Bouncing
        call mlAdvance
        
        
        
        

        
        
        
        
        
        
        DONE: pop r0
