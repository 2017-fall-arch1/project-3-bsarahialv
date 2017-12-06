/** \file play.c
 *  \brief 
 * Author:Bianca Alvarez
 * Instructor: Eric Freudental
 * This is a simple pog game.
 *  This demo creates several layers containing shapes (rectangles for the paddles and cirlce for the ball).
 *  While the CPU is running the green LED is on, and
 *  when the screen does not need to be redrawn the CPU
 *  is turned off along with the green LED.
 */  
#include <msp430.h>
#include <libTimer.h>
#include <lcdutils.h>
#include <lcddraw.h>
#include <p2switches.h>
#include <shape.h>
#include <abCircle.h>

#define GREEN_LED BIT6

/** Play Class
 *
 *  \param bgColor Color of background
 * 
 */



//Background
unsigned int bgColor = COLOR_NAVY;

//Rectangle for Paddles
AbRect rect10 = {abRectGetBounds, abRectCheck, {4,15}}; 

//AbRArrow rightArrow = {abRArrowGetBounds, abRArrowCheck, 30};
 
unsigned int paddle_one; //player 1
unsigned int paddle_two; //player 2

// Boolean for whether screen needs to be redrawn  
int redrawScreen = 1;  

//Fence around playing field 
Region fieldFence;		

//playing field 
AbRectOutline fieldOutline = {	
  abRectOutlineGetBounds, abRectOutlineCheck,   
  {screenWidth/2 - 10, screenHeight/2 - 10}
};

//Layer for snowball
Layer layer5 = {		
  (AbShape *)&circle2,
  {screenWidth-15, screenHeight-15}, 
  {0,0}, {0,0},				    
  COLOR_WHITE,
  0
};


//Layer for rectangle in the middle
Layer layer4 = {
  (AbShape *)&rect10,
  {screenWidth/2, screenHeight/2},
  {0,0}, {0,0},				    
  COLOR_GREEN,
  &layer5
};
  
//Layer for play ball
Layer layer3 = {		/**< Layer with an orange circle */
  (AbShape *)&circle5,
  {(screenWidth/2)+10, (screenHeight/2)+5}, 
  {0,0}, {0,0},				    
  COLOR_WHITE,
  &layer4
};

//Layer for field
Layer fieldLayer = {		
  (AbShape *) &fieldOutline,
  {screenWidth/2, screenHeight/2},
  {0,0}, {0,0},				    
  COLOR_WHITE,
  &layer3
};

//Layer for paddle_two
Layer layer1 = {		/**< Layer with a red square */
  (AbShape *)&rect10,
  {(screenWidth/2)+49, screenHeight/2}, /**< center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_GOLD,
  &fieldLayer,
};

//LAyer for paddle_one
Layer layer0 = {		/**< Layer with an orange circle */
  (AbShape *)&rect10,
  {(screenWidth/2)-49, (screenHeight/2)}, /**< bit below & right of center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_RED,
  &layer1,
};

/** Moving Layer
 *  Linked list of layer references
 *  Velocity represents one iteration of change (direction & magnitude)
 */
typedef struct MovLayer_s {
  Layer *layer;
  Vec2 velocity;
  struct MovLayer_s *next;
} MovLayer;

/* initial value of {0,0} will be overwritten */


//Layer 4 doesn't move
MovLayer ml5 = { &layer5, {2,0}, 0 }; 
MovLayer ml3 = { &layer3, {3,3}, 0 }; 
MovLayer ml1 = { &layer1, {0,0}, &ml3 }; //layer not moving
MovLayer ml0 = { &layer0, {0,0}, &ml1 }; //layer not moving

void movLayerDraw(MovLayer *movLayers, Layer *layers)
{
  int row, col;
  MovLayer *movLayer;

  and_sr(~8);			/**< disable interrupts (GIE off) */
  for (movLayer = movLayers; movLayer; movLayer = movLayer->next) { /* for each moving layer */
    Layer *l = movLayer->layer;
    l->posLast = l->pos;
    l->pos = l->posNext;
  }
  or_sr(8);			/**< disable interrupts (GIE on) */


  for (movLayer = movLayers; movLayer; movLayer = movLayer->next) { /* for each moving layer */
    Region bounds;
    layerGetBounds(movLayer->layer, &bounds);
    lcd_setArea(bounds.topLeft.axes[0], bounds.topLeft.axes[1], 
		bounds.botRight.axes[0], bounds.botRight.axes[1]);
    for (row = bounds.topLeft.axes[1]; row <= bounds.botRight.axes[1]; row++) {
      for (col = bounds.topLeft.axes[0]; col <= bounds.botRight.axes[0]; col++) {
	Vec2 pixelPos = {col, row};
	u_int color = bgColor;
	Layer *probeLayer;
	for (probeLayer = layers; probeLayer; 
	     probeLayer = probeLayer->next) { /* probe all layers, in order */
	  if (abShapeCheck(probeLayer->abShape, &probeLayer->pos, &pixelPos)) {
	    color = probeLayer->color;
	    break; 
	  } /* if probe check */
	} // for checking all layers at col, row
	lcd_writeColor(color); 
      } // for col
    } // for row
  } // for moving layer being updated
}	  



//Region fence = {{10,30}, {SHORT_EDGE_PIXELS-10, LONG_EDGE_PIXELS-10}}; /**< Create a fence region */

/** Advances a moving shape within a fence
 *  
 *  \param ml The moving shape to be advanced
 *  \param fence The region which will serve as a boundary for ml
 */
void mlAdvance(MovLayer *ml, Region *fence)
{
  Vec2 newPos;
  u_char axis;
  Region shapeBoundary;
  
  for (; ml; ml = ml->next) {
    vec2Add(&newPos, &ml->layer->posNext, &ml->velocity);
    abShapeGetBounds(ml->layer->abShape, &newPos, &shapeBoundary);
    for (axis = 0; axis < 2; axis ++) {
      if ((shapeBoundary.topLeft.axes[axis] < fence->topLeft.axes[axis]) || 
          (shapeBoundary.botRight.axes[axis] > fence->botRight.axes[axis]) ) {
          
          if((shapeBoundary.topLeft.axes[0] < fence->topLeft.axes[axis])){
              //newPos.axes[0] = (screenWidth/2)+10; 
              //newPos.axes[1] = (screenHeight/2)+5;
              playerScores(0,1); //keeps track of score
          }
          if ((shapeBoundary.botRight.axes[0] > fence->botRight.axes[axis])){
              //newPos.axes[0] = (screenWidth/2)+10;
              //newPos.axes[1] = (screenHeight/2)+5;
              playerScores(1,0); // keeps track of score 
          }
          int velocity = ml->velocity.axes[axis] = -ml->velocity.axes[axis];
          newPos.axes[axis] += (2*velocity);
      }	/**< if outside of fence */
    } /**< for axis */
    ml->layer->posNext = newPos;
  } /**< for ml */
}

//makes ball bounce when it hits the pad 
void checkForCollision(MovLayer *ball, MovLayer *pad){
     Region padBounday;
     Region ballBoundary;
     Vec2 coordinates;
     u_char x;
     
     abShapeGetBounds(pad->layer->abShape, &pad->layer->posNext, &padBounday);
     vec2Add(&coordinates, &ball->layer->pos, &ball->velocity);
     abShapeGetBounds(ball->layer->abShape, &coordinates, &ballBoundary);
 
     if(abShapeCheck(pad->layer->abShape, &pad->layer->pos, &ballBoundary.topLeft) ||
         abShapeCheck(pad->layer->abShape, &pad->layer->pos, &ballBoundary.botRight) ){
         
                 int velocity = ball->velocity.axes[0] = -ball->velocity.axes[0];
                 coordinates.axes[0] += (2*velocity);
                 //buzzer_set_period(1500); //hit sound 
             
         
     }
 }
 


 //keeps track of score 
 void playerScores(u_int score1, u_int score2) {
     paddle_one += score1;
     paddle_two += score2;
     char str[] = {'0','1','2','3','4'};
     char scrs[] = {str[paddle_one], '-', str[paddle_two], '\0'}; 
     drawString5x7(0,0, "   ", COLOR_NAVY, COLOR_WHITE);
     drawString5x7(20,0, "COMP ARCHITECTURE   ", COLOR_WHITE, COLOR_NAVY);
     
     drawString5x7(0,152, "   ", COLOR_NAVY, COLOR_WHITE);
     drawString5x7(20,152, "P1", COLOR_WHITE, COLOR_NAVY);
     drawString5x7(80,152, "P2", COLOR_WHITE, COLOR_NAVY);
     drawString5x7(55,152, scrs, COLOR_WHITE, COLOR_NAVY);
     
     fillRectangle(30,30, 4 , 15, COLOR_ORANGE);
 }
/** Initializes everything, enables interrupts and green LED, 
 *  and handles the rendering for the screen
 */
void main()
{
  P1DIR |= GREEN_LED;		/**< Green led on when CPU on */		
  P1OUT |= GREEN_LED;

  configureClocks();
  lcd_init();
  shapeInit();
  p2sw_init(15);

  shapeInit();

  layerInit(&layer0);
  layerDraw(&layer0);


  layerGetBounds(&fieldLayer, &fieldFence);


  enableWDTInterrupts();      /**< enable periodic interrupt */
  or_sr(0x8);	              /**< GIE (enable interrupts) */


  for(;;) { 
    while (!redrawScreen) { /**< Pause CPU if screen doesn't need updating */
      P1OUT &= ~GREEN_LED;    /**< Green led off witHo CPU */
      or_sr(0x10);	      /**< CPU OFF */
    }
    P1OUT |= GREEN_LED;       /**< Green led on when CPU on */
    redrawScreen = 0;
    movLayerDraw(&ml0, &layer0);
  }
}

/** Watchdog timer interrupt handler. 15 interrupts/sec */
void wdt_c_handler()
{
  static short count = 0;
  P1OUT |= GREEN_LED;		      /**< Green LED on when cpu on */
  count ++;
  
  // to see which player won 
   if(paddle_one == 4 ){
         //clearScreen(COLOR_BLUE); 
        clearScreen(COLOR_BLUE);
         drawString5x7(20,60, "Player 1 Wins!!", COLOR_GREEN, COLOR_BLACK);
         ml5.velocity.axes[0] = 0; 
         ml5.velocity.axes[1] = 0;
         
         //make_note();
     }
     if(paddle_two == 4){
         //clearScreen(COLOR_BLACK); 
         clearScreen(COLOR_BLUE);
         drawString5x7(20,60, "Player 2 Wins!!", COLOR_GREEN, COLOR_BLACK);
         ml5.velocity.axes[0] = 0; 
         ml5.velocity.axes[1] = 0;
         
         //make_note();
     }
  if (count == 15) {
    //checkForCollision(&ml3,&ml1);
    //checkForCollision(&ml3,&ml0);
    //mlAdvance(&ml0, &fieldFence);
    //if (p2sw_read())
      //redrawScreen = 1;
    //count = 0;
      //before reaching winning points 
     if(paddle_one < 4 && paddle_two < 4){
         checkForCollision(&ml3,&ml1);
         checkForCollision(&ml3,&ml0);
         mlAdvance(&ml0, &fieldFence);
         u_int switches = p2sw_read(), i;
         char str[5];
         for (i = 0; i < 4; i++)
             str[i] = (switches & (1<<i)) ? 0 : 1;
         str[4] = 0;
         
         if(str[0]){
             ml0.velocity.axes[0] = 0; 
             ml0.velocity.axes[1] = -5;
         }
         if(str[1]){
             ml0.velocity.axes[0] = 0; 
             ml0.velocity.axes[1] = 5;
         }
         if(str[2]){
             ml1.velocity.axes[0] = 0; 
             ml1.velocity.axes[1] = -5;
         }
         if(str[3]){
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
  P1OUT &= ~GREEN_LED;		    /**< Green LED off when cpu off */
}
