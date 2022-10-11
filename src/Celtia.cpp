/////////////////////////////////////////////////////////////////////
// CELTIA 2D GAME													/
// Programmed By: Craig Foucher										/
// For:			  CIS 487											/
//                Dr. Bruce Maxim (University of Michigan-Dearborn)	/
//				  Final Project										/
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
// This game uses the T3D libraries written by Andre LaMothe.		/
// to compile make sure to include DDRAW.LIB, DSOUND.LIB,			/
// DINPUT.LIB, WINMM.LIB, and of course								/
// T3DLIB1.CPP,T3DLIB2.CPP,T3DLIB3.CPP,								/
/////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// INCLUDES 
////////////////////////////////////////////////////////////

#define INITGUID

#define WIN32_LEAN_AND_MEAN  

#include <windows.h>   // include important windows stuff
#include <windowsx.h> 
#include <mmsystem.h>
#include <iostream.h> // include important C/C++ stuff
#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h> 
#include <math.h>
#include <io.h>
#include <fcntl.h>

#include <ddraw.h>  // directX includes
#include <dsound.h>
#include <dmksctrl.h>
#include <dmusici.h>
#include <dmusicc.h>
#include <dmusicf.h>
#include <dinput.h>
#include "T3DLIB1.h" // game library includes
#include "T3DLIB2.h"
#include "T3DLIB3.h"

///////////////////////////////////////////////////////////
// DEFINES 
///////////////////////////////////////////////////////////

// defines for windows 
#define WINDOW_CLASS_NAME "WINXCLASS"  // class name

#define WINDOW_WIDTH    320   // size of window
#define WINDOW_HEIGHT   240

// lugh directions
#define LUGH_EAST         0  
#define LUGH_NORTH        1
#define LUGH_WEST         2
#define LUGH_SOUTH        3
#define LUGH_SOUTH_ATTACK 4
#define LUGH_NORTH_ATTACK 5
#define LUGH_WEST_ATTACK  6
#define LUGH_EAST_ATTACK  7
#define LUGH_SOUTH_IDLE   8
#define LUGH_NORTH_IDLE   9
#define LUGH_WEST_IDLE   10
#define LUGH_EAST_IDLE   11

// Pages, BOBS per page 
#define NUM_PAGES		 34

// BOB types
#define TREE1			 1
#define TREE2			 2
#define SWAMP1			 3
#define WATER1			 4
#define WATER2			 5

// Enemy BOB types
#define FIRBOLG_TYPE	 1	
#define FOMORIAN_TYPE	 2

// Boss Types
#define EIRIC_TYPE		1
#define BALOR_TYPE		2
#define	DOMNANN_TYPE	3

///////////////////////////////////////////////////////////
// FUNCTION PROTOTYPES 
///////////////////////////////////////////////////////////

// Windows functions
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int ncmdshow);

// game console functions
int Game_Init(void *parms=NULL);
int Game_Shutdown(void *parms=NULL);
int Game_Main(void *parms=NULL);

// animation functions
void Set_Idle_Animation(int curr_direction);
void Set_Attack_Animation(int curr_direction);

// BOB functions
int Create_Page_BOBS();
void Draw_Page_BOBS(int curr_page);

// Enemy BOB functions
void Enemy_AI(BOB_TYP& Enemy, int Speed, int type, int enemyIndex);
void Draw_Enemy_BOBS();
void Player_Hit_FirBolg(int index);

// Enemy Bosses
void Boss_AI(BOB_TYP& Domnann, int Domnann_Speed);
void Player_Hit_Boss(BOB_TYP& Domnann, int type);

///////////////////////////////////////////////////////////
// GLOBALS 
///////////////////////////////////////////////////////////

HWND main_window_handle           = NULL; // save the window handle
HINSTANCE main_instance           = NULL; // save the instance
char buffer[80];                          // used to print text
static int   player_moving = 0;		// tracks player motion
static int	player_attacking = 0;	// tracks player attack motion


/////////////////////////////////////////////////////
// Lugh Character Globals
/////////////////////////////////////////////////////
BOB          Lugh;            // the player Lugh bob

static int Lugh_anim[12][4] = {	{8,9,8,9},
								{3,4,3,4},
								{6,7,6,7},
								{0,1,0,1},
								{10,11,12,10},
								{13,14,15,13},
								{16,17,18,16},
								{21,19,20,21},
								{2,2,2,2},
								{5,5,5,5},
								{6,6,6,6},
								{8,8,8,8}	}; // animation sequence for Lugh (12 positions)
int Lugh_Speed = 3;							   // speed of lugh across the screen

int LUGH_HP = 50;

//////////////////////////////////////////////////////
// BOSS BOB Globals
//////////////////////////////////////////////////////
int Bosses_Dead = 0;
int Minions_Dead = 0;

//////////////////////////////////////////////////////
// Eiric Main Boss
//////////////////////////////////////////////////////
BOB          Eiric;            // the player Eiric bob

static int Eiric_anim[4][4] = {{8,9,8,9},
								{3,5,4,5},
								{6,7,6,7},
								{0,2,1,2}	}; // animation sequence for Eiric
int Eiric_Speed = 3;							   // speed of Eiric across the screen
const int Eiric_w = 72;
const int Eiric_h = 80;
int EIRIC_HP = 150;
int EiricActive = 0;

//////////////////////////////////////////////////////
// Balor Second Boss
//////////////////////////////////////////////////////

BOB          Balor;            // the player Balor bob

static int Balor_anim[4][4] = {{8,9,8,9},
								{3,5,4,5},
								{6,7,6,7},
								{0,2,1,2}	}; // animation sequence for Balor
int Balor_Speed = 3;							   // speed of Balor across the screen
const int Balor_w = 72;
const int Balor_h = 80;
int BALOR_HP = 100;
int BalorActive = 0;

//////////////////////////////////////////////////////
// Domnann First Boss
//////////////////////////////////////////////////////
BOB          Domnann;            // the player Domnann bob

static int Domnann_anim[4][4] = {{9,11,10,11},
								{3,5,4,5},
								{6,8,7,8},
								{0,2,1,2}	}; // animation sequence for Domnann
int Domnann_Speed = 2;							   // speed of Domnann across the screen
const int Domnann_w = 72;
const int Domnann_h = 80;
int DOMNANN_HP = 50;
int DomnannActive = 0;

/////////////////////////////////////////////////////
// FirBolg Minion Bosses
/////////////////////////////////////////////////////
const int NUM_FIRBOLG = 15;
const int FirBolg_w = 56;
const int FirBolg_h = 52;
BOB    FirBolg[NUM_FIRBOLG];            // the FirBolg enemy bob

static int FirBolg_anim[4][4] ={{8,9,8,9},
								{3,5,4,5},
								{6,7,6,7},
								{0,2,1,2} }; // animation sequence for FirBolg
int FirBolg_Speed = 1;							   // speed of FirBolg across the screen

int FirBolgHitPoints[NUM_FIRBOLG];
int FirBolgActive[NUM_FIRBOLG];
int FIRBOLG_HP = 10;

//////////////////////////////////////////////////////
// Fomorian Minion Bosses
//////////////////////////////////////////////////////
const int NUM_FOMORIAN = 15;
const int Fomorian_w = 62;
const int Fomorian_h = 76;
BOB    Fomorian[NUM_FOMORIAN];            // the Fomorian enemy bob

static int Fomorian_anim[4][4] ={{9,11,10,11},
								{3,5,4,5},
								{6,8,7,8},
								{0,2,1,2} }; // animation sequence for Fomorian
int Fomorian_Speed = 1;							   // speed of Fomorian across the screen

int FomorianHitPoints[NUM_FOMORIAN];
int FomorianActive[NUM_FOMORIAN];
int FOMORIAN_HP = 15;

//////////////////////////////////////////////////////
//Background stationary BOB globals
//////////////////////////////////////////////////////

int IsLevel1 = 0;
int IsLevel2 = 0;
int IsLevel3 = 0;

//////////////////////////////////////////////////////

const int NUM_TREE1 = 16;
const int Tree1_w = 82;
const int Tree1_h = 88;
BOB	Tree1[NUM_TREE1];

///////////////////////////////////////////////////////

const int NUM_TREE2 = 16;
const int Tree2_w = 32;
const int Tree2_h = 84;
BOB	Tree2[NUM_TREE2];

///////////////////////////////////////////////////////

const int NUM_SWAMP1 = 10;
const int Swamp1_w = 80;
const int Swamp1_h = 80;
BOB	Swamp1[NUM_SWAMP1];

///////////////////////////////////////////////////////

const int NUM_SWAMP2 = 10;
const int Swamp2_w = 80;
const int Swamp2_h = 80;
BOB	Swamp2[NUM_SWAMP2];

///////////////////////////////////////////////////////

const int NUM_WATER1 = 10;
const int Water1_w = 80;
const int Water1_h = 80;
BOB Water1[NUM_WATER1];

///////////////////////////////////////////////////////

const int NUM_WATER2 = 11;
const int Water2_w = 80;
const int Water2_h = 80;
BOB Water2[NUM_WATER2];

///////////////////////////////////////////////////////

const int NUM_ROCK1 = 10;
const int Rock1_w = 40;
const int Rock1_h = 44;
BOB Rock1[NUM_ROCK1];

///////////////////////////////////////////////////////

const int NUM_ROCK2 = 10;
const int Rock2_w = 40;
const int Rock2_h = 44;
BOB Rock2[NUM_ROCK2];

///////////////////////////////////////////////////////

const int NUM_STONE1 = 2;
const int Stone1_w = 56;
const int Stone1_h = 106;
BOB Stone1[NUM_STONE1];

///////////////////////////////////////////////////////

const int NUM_STONE2 = 2;
const int Stone2_w = 30;
const int Stone2_h = 104;
BOB Stone2[NUM_STONE2];

///////////////////////////////////////////////////////

const int NUM_MOUNTAIN1 = 10;
const int Mountain1_w = 80;
const int Mountain1_h = 80;
BOB Mountain1[NUM_MOUNTAIN1];

///////////////////////////////////////////////////////

const int Bridge1_w = 80;
const int Bridge1_h = 150;
BOB Bridge1;

///////////////////////////////////////////////////////

const int AncientTree_w = 96;
const int AncientTree_h = 126;
BOB AncientTree;

///////////////////////////////////////////////////////
const int NUM_DOOR = 3;
const int Door_w = 40;
const int Door_h = 40;
BOB Door[NUM_DOOR];

///////////////////////////////////////////////////////

const int Castle_w = 256;
const int Castle_h = 192;
BOB Castle;

///////////////////////////////////////////////////////
// Background images
///////////////////////////////////////////////////////

BITMAP_IMAGE intro;						// intro screen
BITMAP_IMAGE gamewin;					// player won game
int IsIntro = 1;
int IsGameWin = 0;
BITMAP_IMAGE landscape[NUM_PAGES];      // the background    landscape 30 pages 5x6
int	curr_page;
int curr_page_x = 3;					// current page of landscape in x direction
int curr_page_y = 4;					// current page of landscape in y direction
const int SCREEN_OFFSET = 125;			// Offset of black top of screen
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
// Music and sounds
///////////////////////////////////////////////////////////

int arrival_sound_id = -1;        // the background music
int attack_sound_id = -2;
int bossattack_sound_id = -3;
int bossdamage_sound_id = -4;
int damage_sound_id = -5;
int end_sound_id = -6;
int evil_sound_id = -7;
int firbolgdamage_sound_id = -8;
int fomoriandamage_sound_id = -9;
int intro_sound_id = -10;
int fomorianattack_sound_id = -11;
int firbolgattack_sound_id = -12;

///////////////////////////////////////////////////////////


// FUNCTIONS //////////////////////////////////////////////

LRESULT CALLBACK WindowProc(HWND hwnd, 
						    UINT msg, 
                            WPARAM wparam, 
                            LPARAM lparam)
{
// this is the main message handler of the system
PAINTSTRUCT	ps;		   // used in WM_PAINT
HDC			hdc;	   // handle to a device context

// what is the message 
switch(msg)
	{	
	case WM_CREATE: 
        {
		// do initialization stuff here
		return(0);
		} break;

    case WM_PAINT:
         {
         // start painting
         hdc = BeginPaint(hwnd,&ps);

         // end painting
         EndPaint(hwnd,&ps);
         return(0);
        } break;

	case WM_DESTROY: 
		{
		// kill the application			
		PostQuitMessage(0);
		return(0);
		} break;

	default:break;

    } // end switch

// process any messages that we didn't take care of 
return (DefWindowProc(hwnd, msg, wparam, lparam));

} // end WinProc

// WINMAIN ////////////////////////////////////////////////

int WINAPI WinMain(	HINSTANCE hinstance,
					HINSTANCE hprevinstance,
					LPSTR lpcmdline,
					int ncmdshow)
{
// this is the winmain function

WNDCLASS winclass;	// this will hold the class we create
HWND	 hwnd;		// generic window handle
MSG		 msg;		// generic message
//HDC      hdc;       // generic dc
//PAINTSTRUCT ps;     // generic paintstruct

// first fill in the window class stucture
winclass.style			= CS_DBLCLKS | CS_OWNDC | 
                          CS_HREDRAW | CS_VREDRAW;
winclass.lpfnWndProc	= WindowProc;
winclass.cbClsExtra		= 0;
winclass.cbWndExtra		= 0;
winclass.hInstance		= hinstance;
winclass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
winclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
winclass.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
winclass.lpszMenuName	= NULL; 
winclass.lpszClassName	= WINDOW_CLASS_NAME;

// register the window class
if (!RegisterClass(&winclass))
	return(0);

// create the window, note the use of WS_POPUP
if (!(hwnd = CreateWindow(WINDOW_CLASS_NAME,		// class
						  "Celtia",					// title
						  WS_POPUP | WS_VISIBLE,
					 	  0,0,						// x,y
						  WINDOW_WIDTH,				// width
                          WINDOW_HEIGHT,			// height
						  NULL,						// handle to parent 
						  NULL,						// handle to menu
						  hinstance,				// instance
						  NULL)))					// creation parms
return(0);

// save the window handle and instance in a global
main_window_handle = hwnd;
main_instance      = hinstance;

// perform all game console specific initialization
Game_Init();

// enter main event loop
while(1)
	{
	if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{ 
		// test if this is a quit
        if (msg.message == WM_QUIT)
           break;
	
		// translate any accelerator keys
		TranslateMessage(&msg);

		// send the message to the window proc
		DispatchMessage(&msg);
		} // end if
    
    // main game processing goes here
    Game_Main();

	} // end while

// shutdown game and release all resources
Game_Shutdown();

// return to Windows like this
return(msg.wParam);

} // end WinMain

//////////////////////////////////////////////////////////////
// CELTIA GAME PROGRAMMING CONSOLE FUNCTIONS /////////////////
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
// Game Init
//////////////////////////////////////////////////////////////

int Game_Init(void *parms)
{
// this function is where you do all the initialization 
// for your game

int index, direction; // looping variables
char filename[80]; // used to build up files names

// start up DirectDraw (replace the parms as you desire)
DDraw_Init(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP);


	// load the intro screen
	Load_Bitmap_File(&bitmap8bit, "BACKGROUNDS/INTRO.BMP");
	// create and load the bitmap image
	Create_Bitmap(&intro, 0,0, 640, 480);
	Load_Image_Bitmap(&intro,&bitmap8bit,0,0,BITMAP_EXTRACT_MODE_ABS);
	Unload_Bitmap_File(&bitmap8bit);

Set_Palette(bitmap8bit.palette);

	// load the gamewin screen
	Load_Bitmap_File(&bitmap8bit, "BACKGROUNDS/GAMEWIN.BMP");
	// create and load the bitmap image
	Create_Bitmap(&gamewin, 0,0, 640, 480);
	Load_Image_Bitmap(&gamewin,&bitmap8bit,0,0,BITMAP_EXTRACT_MODE_ABS);
	Unload_Bitmap_File(&bitmap8bit);

Set_Palette(bitmap8bit.palette);

// load in each page of the scrolling background
for (index = 0; index < NUM_PAGES; index++)
{
	// build up file name
	sprintf(filename,"BACKGROUNDS/ARRIVALBACK%d.BMP",index);

	// load the background
	Load_Bitmap_File(&bitmap8bit, filename);
	// create and load the background bitmap images
	Create_Bitmap(&landscape[index], 0,0, 640, 480);
	Load_Image_Bitmap(&landscape[index],&bitmap8bit,0,0,BITMAP_EXTRACT_MODE_ABS);
	Unload_Bitmap_File(&bitmap8bit);


} // end for index

// set the palette to background image palette
Set_Palette(bitmap8bit.palette);


if(!Create_Page_BOBS())
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////
// create Lugh bob
if (!Create_BOB(&Lugh,100,200, 44,60, 23, 
				BOB_ATTR_MULTI_ANIM | BOB_ATTR_VISIBLE, DDSCAPS_SYSTEMMEMORY))
{
				return 0;
}


// load the Lugh bitmaps
Load_Bitmap_File(&bitmap8bit, "BOBS/LUGHALL.BMP");

for (direction = 0; direction < 23; direction++)
{
	if(direction < 4)
		Load_Frame_BOB(&Lugh,&bitmap8bit,direction, direction,0,BITMAP_EXTRACT_MODE_CELL);
	else if(direction >= 4 && direction < 8)
		Load_Frame_BOB(&Lugh,&bitmap8bit,direction, direction-4,1,BITMAP_EXTRACT_MODE_CELL);  	
	else if(direction >= 8 && direction < 12)
		Load_Frame_BOB(&Lugh,&bitmap8bit,direction, direction-8,2,BITMAP_EXTRACT_MODE_CELL);  	
	else if(direction >= 12 && direction < 16)
		Load_Frame_BOB(&Lugh,&bitmap8bit,direction, direction-12,3,BITMAP_EXTRACT_MODE_CELL);
	else if(direction >= 16 && direction < 20)
		Load_Frame_BOB(&Lugh,&bitmap8bit,direction, direction-16,4,BITMAP_EXTRACT_MODE_CELL); 
	else 
		Load_Frame_BOB(&Lugh,&bitmap8bit,direction, direction-20,5,BITMAP_EXTRACT_MODE_CELL);  	
}

// unload Lugh
Unload_Bitmap_File(&bitmap8bit);     

for (direction = 0; direction < 12; direction++)
{	
	// set the animation sequences for skelaton
	Load_Animation_BOB(&Lugh,direction,4,Lugh_anim[direction]);
}

// set up stating state of skelaton
Set_Animation_BOB(&Lugh,3);
Set_Anim_Speed_BOB(&Lugh, 10);
Set_Vel_BOB(&Lugh, 0,0);
Set_Pos_BOB(&Lugh, 200, 256); 

///////////////////////////////////////////////////////////////////////////////////////////
// create FirBolg bobs
//////////////////////////////////////////////////////////////////////////////////////////
// load the FirBolg bitmaps
Load_Bitmap_File(&bitmap8bit, "BOBS/FIRBOLG.BMP");

for (index = 0; index < NUM_FIRBOLG; index++)
{
	if (!Create_BOB(&FirBolg[index],0,0, FirBolg_w,FirBolg_h, 10, 
					BOB_ATTR_MULTI_ANIM | BOB_ATTR_VISIBLE, DDSCAPS_SYSTEMMEMORY))
	{
					return 0;
	}

	for (direction = 0; direction < 10; direction++)
	{
		if(direction < 4)
			Load_Frame_BOB(&FirBolg[index],&bitmap8bit,direction, direction,0,BITMAP_EXTRACT_MODE_CELL);
		else if(direction >= 4 && direction < 8)
			Load_Frame_BOB(&FirBolg[index],&bitmap8bit,direction, direction-4,1,BITMAP_EXTRACT_MODE_CELL);  	
		else
			Load_Frame_BOB(&FirBolg[index],&bitmap8bit,direction, direction-8,2,BITMAP_EXTRACT_MODE_CELL);  	  	
	}

	for (direction = 0; direction < 4; direction++)
	{	
		// set the animation sequences for FirBolg
		Load_Animation_BOB(&FirBolg[index],direction,4,FirBolg_anim[direction]);
	}
	// set up stating state of skelaton
	Set_Animation_BOB(&FirBolg[index],rand()%4);
	Set_Anim_Speed_BOB(&FirBolg[index], 10);
	Set_Vel_BOB(&FirBolg[index], 0,0);
	Set_Pos_BOB(&FirBolg[index], -100, -100); 

	FirBolgActive[index] = 1;
	FirBolgHitPoints[index] = FIRBOLG_HP;
}

// unload FirBolg
Unload_Bitmap_File(&bitmap8bit);     

///////////////////////////////////////////////////////////////////////////////////////////
// Create Fomorian BOBS
///////////////////////////////////////////////////////////////////////////////////////////
// load the Fomorian bitmaps
Load_Bitmap_File(&bitmap8bit, "BOBS/FOMORIAN.BMP");

for (index = 0; index < NUM_FOMORIAN; index++)
{
	if (!Create_BOB(&Fomorian[index],0,0, Fomorian_w,Fomorian_h, 12, 
					BOB_ATTR_MULTI_ANIM | BOB_ATTR_VISIBLE, DDSCAPS_SYSTEMMEMORY))
	{
					return 0;
	}

	for (direction = 0; direction < 12; direction++)
	{
		if(direction < 4)
			Load_Frame_BOB(&Fomorian[index],&bitmap8bit,direction, direction,0,BITMAP_EXTRACT_MODE_CELL);
		else if(direction >= 4 && direction < 8)
			Load_Frame_BOB(&Fomorian[index],&bitmap8bit,direction, direction-4,1,BITMAP_EXTRACT_MODE_CELL);  	
		else
			Load_Frame_BOB(&Fomorian[index],&bitmap8bit,direction, direction-8,2,BITMAP_EXTRACT_MODE_CELL);  	  	
	}

	for (direction = 0; direction < 4; direction++)
	{	
		// set the animation sequences for Fomorian
		Load_Animation_BOB(&Fomorian[index],direction,4,Fomorian_anim[direction]);
	}
	// set up stating state of skelaton
	Set_Animation_BOB(&Fomorian[index],rand()%4);
	Set_Anim_Speed_BOB(&Fomorian[index], 10);
	Set_Vel_BOB(&Fomorian[index], 0,0);
	Set_Pos_BOB(&Fomorian[index], -100, -100); 

	FomorianActive[index] = 1;
	FomorianHitPoints[index] = FOMORIAN_HP;
}

// unload Fomorian bitmap
Unload_Bitmap_File(&bitmap8bit); 

////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////
// create Eiric bob
if (!Create_BOB(&Eiric,100,200, Eiric_w,Eiric_h, 10, 
				BOB_ATTR_MULTI_ANIM | BOB_ATTR_VISIBLE, DDSCAPS_SYSTEMMEMORY))
{
				return 0;
}

// load the Eiric bitmaps
Load_Bitmap_File(&bitmap8bit, "BOBS/EIRICBOB.BMP");

for (direction = 0; direction < 10; direction++)
{
	if(direction < 4)
		Load_Frame_BOB(&Eiric,&bitmap8bit,direction, direction,0,BITMAP_EXTRACT_MODE_CELL);
	else if(direction >= 4 && direction < 8)
		Load_Frame_BOB(&Eiric,&bitmap8bit,direction, direction-4,1,BITMAP_EXTRACT_MODE_CELL);  	
	else
		Load_Frame_BOB(&Eiric,&bitmap8bit,direction, direction-8,2,BITMAP_EXTRACT_MODE_CELL);  	  	
}

// unload Eiric
Unload_Bitmap_File(&bitmap8bit);     

for (direction = 0; direction < 4; direction++)
{	
	// set the animation sequences for skelaton
	Load_Animation_BOB(&Eiric,direction,4,Eiric_anim[direction]);
}

// set up stating state of skelaton
Set_Animation_BOB(&Eiric,3);
Set_Anim_Speed_BOB(&Eiric, 10);
Set_Vel_BOB(&Eiric, 0,0);
Set_Pos_BOB(&Eiric, -100, -100); 

////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////
// create Balor bob
if (!Create_BOB(&Balor,100,200, Balor_w,Balor_h, 10, 
				BOB_ATTR_MULTI_ANIM | BOB_ATTR_VISIBLE, DDSCAPS_SYSTEMMEMORY))
{
				return 0;
}
// load the Balor bitmaps
Load_Bitmap_File(&bitmap8bit, "BOBS/BALORBOB.BMP");

for (direction = 0; direction < 10; direction++)
{
	if(direction < 4)
		Load_Frame_BOB(&Balor,&bitmap8bit,direction, direction,0,BITMAP_EXTRACT_MODE_CELL);
	else if(direction >= 4 && direction < 8)
		Load_Frame_BOB(&Balor,&bitmap8bit,direction, direction-4,1,BITMAP_EXTRACT_MODE_CELL);  	
	else
		Load_Frame_BOB(&Balor,&bitmap8bit,direction, direction-8,2,BITMAP_EXTRACT_MODE_CELL);  	  	
}

// unload Balor
Unload_Bitmap_File(&bitmap8bit);     

for (direction = 0; direction < 4; direction++)
{	
	// set the animation sequences for skelaton
	Load_Animation_BOB(&Balor,direction,4,Balor_anim[direction]);
}

// set up stating state of skelaton
Set_Animation_BOB(&Balor,3);
Set_Anim_Speed_BOB(&Balor, 10);
Set_Vel_BOB(&Balor, 0,0);
Set_Pos_BOB(&Balor, -100, -100); 

////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////
// create Domnann bob
if (!Create_BOB(&Domnann,100,200, Domnann_w,Domnann_h, 12, 
				BOB_ATTR_MULTI_ANIM | BOB_ATTR_VISIBLE, DDSCAPS_SYSTEMMEMORY))
{
				return 0;
}

// load the Domnann bitmaps
Load_Bitmap_File(&bitmap8bit, "BOBS/DOMNANNBOB.BMP");

for (direction = 0; direction < 12; direction++)
{
	if(direction < 4)
		Load_Frame_BOB(&Domnann,&bitmap8bit,direction, direction,0,BITMAP_EXTRACT_MODE_CELL);
	else if(direction >= 4 && direction < 8)
		Load_Frame_BOB(&Domnann,&bitmap8bit,direction, direction-4,1,BITMAP_EXTRACT_MODE_CELL);  	
	else
		Load_Frame_BOB(&Domnann,&bitmap8bit,direction, direction-8,2,BITMAP_EXTRACT_MODE_CELL);  	  	
}

// unload Domnann
Unload_Bitmap_File(&bitmap8bit);     

for (direction = 0; direction < 4; direction++)
{	
	// set the animation sequences for skelaton
	Load_Animation_BOB(&Domnann,direction,4,Domnann_anim[direction]);
}

// set up stating state of skelaton
Set_Animation_BOB(&Domnann,3);
Set_Anim_Speed_BOB(&Domnann, 10);
Set_Vel_BOB(&Domnann, 0,0);
Set_Pos_BOB(&Domnann, -100, -100); 

////////////////////////////////////////////////////////////////////////////////////////
// Page 0
Set_Pos_BOB(&Fomorian[0], 200, 300);
// Page 1
Set_Pos_BOB(&Fomorian[1], 300, 150);
// Page 2
Set_Pos_BOB(&Fomorian[2], 300, 110);
// Page 3
Set_Pos_BOB(&Fomorian[3], 200, 110);
// Page 4
Set_Pos_BOB(&Fomorian[4], 400, 110);
// Page 5
Set_Pos_BOB(&Fomorian[5], 300, 150);
Set_Pos_BOB(&Fomorian[6], 200, 400);
// Page 6
Set_Pos_BOB(&Fomorian[7], 200, 500);
// Page 7
Set_Pos_BOB(&Fomorian[8], 150, 380);
// Page 8
// Page 9
Set_Pos_BOB(&FirBolg[0], 100, 380);
// Page 10
Set_Pos_BOB(&FirBolg[1], 500, 200);
Set_Pos_BOB(&Fomorian[9], 300, 300);
// Page 11
// Page 12
Set_Pos_BOB(&Fomorian[10], 100, 300);
// Page 13
// Page 14
Set_Pos_BOB(&Fomorian[11], 320, 240);
Set_Pos_BOB(&FirBolg[2], 100, 380);
// Page 15
// Page 16
Set_Pos_BOB(&FirBolg[3], 100, 200);
Set_Pos_BOB(&FirBolg[4], 320, 240);
Set_Pos_BOB(&FirBolg[5], 500, 200);
// Page 17
Set_Pos_BOB(&FirBolg[6], 200, 250);
Set_Pos_BOB(&Fomorian[12], 320, 240);
// Page 18
Set_Pos_BOB(&FirBolg[7], 200, 250);
Set_Pos_BOB(&FirBolg[8], 440, 250);
// Page 19
// Page 20
Set_Pos_BOB(&FirBolg[9], 100, 200);
Set_Pos_BOB(&FirBolg[10], 320, 240);
Set_Pos_BOB(&FirBolg[11], 500, 200);
// Page 21
Set_Pos_BOB(&FirBolg[12], 320, 300);
// Page 22
Set_Pos_BOB(&FirBolg[13], 500, 300);
Set_Pos_BOB(&Fomorian[13], 250, 280);
// Page 23
Set_Pos_BOB(&FirBolg[14], 450, 200);
Set_Pos_BOB(&Fomorian[14], 280, 380);

///////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////
// initialize directinput
DInput_Init();

// acquire the keyboard only
DInput_Init_Keyboard();

// initilize DirectSound
DSound_Init();

// load background sounds
arrival_sound_id = DSound_Load_WAV("SOUNDS/ARRIVAL.WAV");
attack_sound_id  = DSound_Load_WAV("SOUNDS/ATTACK.WAV");
bossattack_sound_id = DSound_Load_WAV("SOUNDS/BOSSATTACK.WAV");
bossdamage_sound_id = DSound_Load_WAV("SOUNDS/BOSSDAMAGE.WAV");
damage_sound_id = DSound_Load_WAV("SOUNDS/DAMAGE.WAV");
end_sound_id = DSound_Load_WAV("SOUNDS/END.WAV");
evil_sound_id = DSound_Load_WAV("SOUNDS/EVIL.WAV");
firbolgdamage_sound_id = DSound_Load_WAV("SOUNDS/FIRBOLGDAMAGE.WAV");
fomoriandamage_sound_id = DSound_Load_WAV("SOUNDS/FOMORIANDAMAGE.WAV");
intro_sound_id = DSound_Load_WAV("SOUNDS/INTRO.WAV");
fomorianattack_sound_id = DSound_Load_WAV("SOUNDS/FOMORIANATTACK.WAV");
firbolgattack_sound_id = DSound_Load_WAV("SOUNDS/FIRBOLGATTACK.WAV");


DSound_Set_Freq(arrival_sound_id, 22050);
DSound_Set_Freq(attack_sound_id, 22050);
DSound_Set_Freq(bossattack_sound_id, 22050);
DSound_Set_Freq(bossdamage_sound_id, 22050);
DSound_Set_Freq(damage_sound_id, 22050);
DSound_Set_Freq(end_sound_id, 22050);
//DSound_Set_Freq(evil_sound_id, 22050);
DSound_Set_Freq(firbolgdamage_sound_id, 22050);
DSound_Set_Freq(fomoriandamage_sound_id, 22050);
DSound_Set_Freq(intro_sound_id, 22050);
DSound_Set_Freq(fomorianattack_sound_id, 22050);
DSound_Set_Freq(firbolgattack_sound_id, 22050);

DSound_Set_Volume(intro_sound_id, 75);
DSound_Set_Volume(arrival_sound_id, 75);
DSound_Set_Volume(end_sound_id, 75);

// start the sounds
//DSound_Play(bossattack_sound_id, 0);
//DSound_Play(bossdamage_sound_id, 0);

DSound_Play(end_sound_id, 0);


// set clipping rectangle to screen extents so objects dont
// mess up at edges
RECT screen_rect = {0,0,screen_width,screen_height};
lpddclipper = DDraw_Attach_Clipper(lpddsback,1,&screen_rect);

// hide the mouse
ShowCursor(FALSE);

// return success
return(1);

} // end Game_Init

///////////////////////////////////////////////////////////

int Game_Shutdown(void *parms)
{
// this function is where you shutdown your game and
// release all resources that you allocated
int index;

// destroy intro screen
Destroy_Bitmap(&intro);
Destroy_Bitmap(&gamewin);

// kill the landscape
for (index=0; index < NUM_PAGES; index++)
    Destroy_Bitmap(&landscape[index]);

// shut everything down
Destroy_BOB(&Lugh);
Destroy_BOB(&Eiric);
Destroy_BOB(&Balor);
Destroy_BOB(&Domnann);

for (index = 0; index < NUM_FIRBOLG; index++)
	Destroy_BOB(&FirBolg[index]);

for (index = 0; index < NUM_FOMORIAN; index++)
	Destroy_BOB(&Fomorian[index]);
for (index = 0; index < NUM_DOOR; index++)
	Destroy_BOB(&Door[index]);

Destroy_BOB(&AncientTree);

for (index = 0; index < NUM_TREE1; index++)
	Destroy_BOB(&Tree1[index]);
for (index = 0; index < NUM_TREE2; index++)
	Destroy_BOB(&Tree2[index]);
for (index = 0; index < NUM_SWAMP1; index++)
	Destroy_BOB(&Swamp1[index]);
for (index = 0; index < NUM_SWAMP2; index++)
	Destroy_BOB(&Swamp2[index]);
for (index = 0; index < NUM_WATER1; index++)
	Destroy_BOB(&Water1[index]);
for (index = 0; index < NUM_WATER2; index++)
	Destroy_BOB(&Water2[index]);
for (index = 0; index < NUM_ROCK1; index++)
	Destroy_BOB(&Rock1[index]);
for (index = 0; index < NUM_ROCK2; index++)
	Destroy_BOB(&Rock2[index]);
for (index = 0; index < NUM_STONE1; index++)
	Destroy_BOB(&Stone1[index]);
for (index = 0; index < NUM_STONE2; index++)
	Destroy_BOB(&Stone2[index]);
for (index = 0; index < NUM_MOUNTAIN1; index++)
	Destroy_BOB(&Mountain1[index]);

Destroy_BOB(&Bridge1);
Destroy_BOB(&Castle);


// shutdown directdraw last
DDraw_Shutdown();

// now directsound
DSound_Stop_All_Sounds();
DSound_Shutdown();

// shut down directinput
DInput_Shutdown();

// return success
return(1);
} // end Game_Shutdown

//////////////////////////////////////////////////////////

int Game_Main(void *parms)
{
// this is the workhorse of your game it will be called
// continuously in real-time this is like main() in C
// all the calls for you game go here!

int index; // looping var
int          dx,dy;             // general deltas used in collision detection



// start the timing clock
Start_Clock();

// clear the drawing surface
DDraw_Fill_Surface(lpddsback, 0);

// lock back buffer and copy background into it
DDraw_Lock_Back_Surface();

if (IsGameWin)
{
	Draw_Bitmap(&gamewin, back_buffer, back_lpitch, 0);
	// unlock back surface	
	DDraw_Unlock_Back_Surface();
	// read keyboard
	DInput_Read_Keyboard();

	DSound_Stop_Sound(evil_sound_id);
	DSound_Play(end_sound_id, 0);
}
else 
if(IsIntro)
{
	Draw_Bitmap(&intro, back_buffer, back_lpitch, 0);
	// unlock back surface	
	DDraw_Unlock_Back_Surface();
	// read keyboard
	DInput_Read_Keyboard();
}
else
if (LUGH_HP <= 0)
{
	Draw_Bitmap(&landscape[30], back_buffer, back_lpitch, 0);
	// unlock back surface
	DDraw_Unlock_Back_Surface();
	// read keyboard
	DInput_Read_Keyboard();

	DSound_Stop_Sound(arrival_sound_id);
	DSound_Play(intro_sound_id, DSBPLAY_LOOPING);

}
else
{
	curr_page = curr_page_y + (curr_page_x * 5);
	// draw the background reactor image
	Draw_Bitmap(&landscape[curr_page], back_buffer, back_lpitch, 0);


	// unlock back surface
	DDraw_Unlock_Back_Surface();

	// read keyboard
	DInput_Read_Keyboard();

	/////////////////////////////////////////////
	// Reset positions of BOBS
	for(index = 0; index < NUM_TREE1; index++)
	{
		Set_Pos_BOB(&Tree1[index], -100, -100);
	}

	for(index = 0; index < NUM_TREE2; index++)
	{
	Set_Pos_BOB(&Tree2[index], -100, -100);
	}
	for(index = 0; index < NUM_TREE1; index++)
	{
		Set_Pos_BOB(&Tree1[index], -100, -100);
	}	

	for(index = 0; index < NUM_SWAMP1; index++)
	{
		Set_Pos_BOB(&Swamp1[index], -100, -100);
	}
	for(index = 0; index < NUM_SWAMP2; index++)
	{
		Set_Pos_BOB(&Swamp2[index], -100, -100);
	}
	for(index = 0; index < NUM_WATER1; index++)
	{
		Set_Pos_BOB(&Water1[index], -100, -100);
	}

	for(index = 0; index < NUM_WATER2; index++)
	{
		Set_Pos_BOB(&Water2[index], -100, -100);
	}

	for(index = 0; index < NUM_ROCK1; index++)
	{
		Set_Pos_BOB(&Rock1[index], -100, -100);
	}
	for(index = 0; index < NUM_ROCK2; index++)
	{
		Set_Pos_BOB(&Rock2[index], -100, -100);
	}

	for(index = 0; index < NUM_STONE1; index++)
	{
		Set_Pos_BOB(&Stone1[index], -100, -100);
	}
	for(index = 0; index < NUM_STONE2; index++)
	{
		Set_Pos_BOB(&Stone2[index], -100, -100);
	}
	for(index = 0; index < NUM_MOUNTAIN1; index++)
	{
		Set_Pos_BOB(&Mountain1[index], -100, -100);
	}

	Set_Pos_BOB(&Bridge1, -100, -100);
	Set_Pos_BOB(&AncientTree, -100, -100);
	Set_Pos_BOB(&Castle, -100, -100);
	for(index = 0; index < NUM_DOOR; index++)
	{
		Set_Pos_BOB(&Door[index], -100, -100);
	}

//////////////////////////////////////////////
//////////////////////////////////////////////
// Draw Background BOB images
Draw_Page_BOBS(curr_page);

Draw_Enemy_BOBS();

// player is not moving or attacking yet
player_moving = 0;
player_attacking = 0;

// Set animation speed of Lugh
Set_Anim_Speed_BOB(&Lugh, 10);

// allow player to move
if (keyboard_state[DIK_RIGHT])
{
   Lugh.x+=Lugh_Speed;
   dx = Lugh_Speed; dy = 0;

   // set motion flag
   player_moving = 1;

   // check animation needs to change
   if (Lugh.curr_animation != LUGH_EAST)
      Set_Animation_BOB(&Lugh,LUGH_EAST);
}
else
if (keyboard_state[DIK_LEFT])
{
   Lugh.x-=Lugh_Speed;
   dx = -Lugh_Speed; dy = 0;

   // set motion flag
   player_moving = 1;

   // check animation needs to change
   if (Lugh.curr_animation != LUGH_WEST)
      Set_Animation_BOB(&Lugh,LUGH_WEST);
}
if (keyboard_state[DIK_UP])
{
	Lugh.y-=Lugh_Speed;
	dy = -Lugh_Speed; dx = 0;

	   // set motion flag
   player_moving = 1;

   // check animation needs to change
   if (Lugh.curr_animation != LUGH_NORTH)
      Set_Animation_BOB(&Lugh,LUGH_NORTH);
}
else
if (keyboard_state[DIK_DOWN])
{
	Lugh.y+=Lugh_Speed;
	dy = Lugh_Speed; dx = 0;

	// set motion flag
   player_moving = 1;

   // check animation needs to change
   if (Lugh.curr_animation != LUGH_SOUTH)
      Set_Animation_BOB(&Lugh,LUGH_SOUTH);
}
else
if (keyboard_state[DIK_SPACE])
{
	player_attacking = 1;

	Set_Anim_Speed_BOB(&Lugh, 20);
	Set_Attack_Animation(Lugh.curr_animation);
}


if(player_moving)
{
	player_attacking = 0;
	// animate Lugh
	Animate_BOB(&Lugh);
}

else 
if(player_attacking)
{
	player_moving = 0;
	for (index = 0; index < 4; index++)
	{
		DSound_Play(attack_sound_id, 0);
		Animate_BOB(&Lugh);
		Draw_BOB(&Lugh, lpddsback);
	}
}
else
{
	// set the animation to standing still
	Set_Idle_Animation(Lugh.curr_animation);
}

// animate Lugh
Animate_BOB(&Lugh);

// draw Lugh
Draw_BOB(&Lugh, lpddsback);

// Print the player's Hit Points
sprintf(buffer,"%d",LUGH_HP);
// draw some text
Draw_Text_GDI("Hit Points",40,30,RGB(255,255,255),lpddsback);
Draw_Text_GDI(buffer,45,60,RGB(255,255,255),lpddsback);
Draw_Text_GDI("/",68,60,RGB(255,255,255),lpddsback);
sprintf(buffer, "%d", 50);
Draw_Text_GDI(buffer,80,60,RGB(255,255,255),lpddsback);

Draw_Text_GDI("Minions Killed",300,30,RGB(255,255,255),lpddsback);
sprintf(buffer, "%d", Minions_Dead);
Draw_Text_GDI(buffer,345,60,RGB(255,255,255),lpddsback);

Draw_Text_GDI("Bosses Killed",500,30,RGB(255,255,255),lpddsback);
sprintf(buffer, "%d", Bosses_Dead);
Draw_Text_GDI(buffer,545,60,RGB(255,255,255),lpddsback);

/////////////////////////////////////////////////
// Collision Detection
for(index = 0; index < NUM_TREE1; index++)
{
	if(Collision_BOBS(&Lugh, &Tree1[index]))
	{
		Lugh.x -= dx;
		Lugh.y -= dy;
	}
}

for(index = 0; index < NUM_TREE2; index++)
{
	if(Collision_BOBS(&Lugh, &Tree2[index]))
	{
		Lugh.x -= dx;
		Lugh.y -= dy;
	}
}

for(index = 0; index < NUM_SWAMP1; index++)
{
	if(Collision_BOBS(&Lugh, &Swamp1[index]))
	{
		Lugh.x -= dx;
		Lugh.y -= dy;
	}
}

for(index = 0; index < NUM_SWAMP2; index++)
{
	if(Collision_BOBS(&Lugh, &Swamp2[index]))
	{
		Lugh.x -= dx;
		Lugh.y -= dy;
	}
}

for(index = 0; index < NUM_WATER1; index++)
{
	if(Collision_BOBS(&Lugh, &Water1[index]))
	{
		Lugh.x -= dx;
		Lugh.y -= dy;
	}
}

for(index = 0; index < NUM_WATER2; index++)
{
	if(Collision_BOBS(&Lugh, &Water2[index]))
	{
		Lugh.x -= dx;
		Lugh.y -= dy;
	}
}

for(index = 0; index < NUM_ROCK1; index++)
{
	if(Collision_BOBS(&Lugh, &Rock1[index]))
	{
		Lugh.x -= dx;
		Lugh.y -= dy;
	}
}
for(index = 0; index < NUM_ROCK2; index++)
{
	if(Collision_BOBS(&Lugh, &Rock2[index]))
	{
		Lugh.x -= dx;
		Lugh.y -= dy;
	}
}

for(index = 0; index < NUM_STONE1; index++)
{
	if(Collision_BOBS(&Lugh, &Stone1[index]))
	{
		Lugh.x -= dx;
		Lugh.y -= dy;
	}
}
for(index = 0; index < NUM_STONE2; index++)
{
	if(Collision_BOBS(&Lugh, &Stone2[index]))
	{
		Lugh.x -= dx;
		Lugh.y -= dy;
	}
}

for(index = 0; index < NUM_MOUNTAIN1; index++)
{
	if(Collision_BOBS(&Lugh, &Mountain1[index]))
	{
		Lugh.x -= dx;
		Lugh.y -= dy;
	}
}

if(Collision_BOBS(&Lugh, &AncientTree))
{
	Lugh.x -= dx;
	Lugh.y -= dy;
}

if(Collision_BOBS(&Lugh, &Door[0]))
{
	curr_page_x = 6;
	curr_page_y = 1;
	Lugh.x = ((float)-(Lugh.width >> 1));
	Lugh.y = 240;
	DSound_Stop_Sound(arrival_sound_id);
	DSound_Play(evil_sound_id, DSBPLAY_LOOPING);
}
if(Collision_BOBS(&Lugh, &Door[1]))
{
	curr_page_x = 6;
	curr_page_y = 2;
	Lugh.x = 320;
	Lugh.y = SCREEN_HEIGHT - ((float)(Lugh.height >> 1));
	DSound_Stop_Sound(arrival_sound_id);
	DSound_Play(evil_sound_id, DSBPLAY_LOOPING);
}
if(Collision_BOBS(&Lugh, &Door[2]))
{
	curr_page_x = 6;
	curr_page_y = 3;
	Lugh.x = 320;
	Lugh.y = SCREEN_HEIGHT - ((float)(Lugh.height >> 1));
	DSound_Stop_Sound(arrival_sound_id);
	DSound_Play(evil_sound_id, DSBPLAY_LOOPING);
}
if(Collision_BOBS(&Lugh, &Castle))
{
	Lugh.x -= dx;
	Lugh.y -= dy;
}

/////////////////////////////////////////////////


// test for page flip right
if (Lugh.x > SCREEN_WIDTH - (Lugh.width >> 1))
{
// bump back regardless
Lugh.x-=dx;
	// test for page flip
   if (curr_page_x < 5 && curr_page < 30)
   { 
   // scroll to next page to right
   curr_page_x++;	
	// reset character to left edge of screen
	Lugh.x = ((float)-(Lugh.width >> 1));
   
   } // end if

} // end if   
else // page flip left?
if (Lugh.x < -(Lugh.width >> 1))
   {
   // bump back regardless
   Lugh.x= ((float)-(Lugh.width >> 1));

   if (curr_page == 31)
	{
		curr_page_x = 5;
		curr_page_y = 4;
		// reset character to edge of door
		Lugh.x = 638-(Mountain1_w*2)-2*((float)(Lugh.width >> 1));
		Lugh.y = 480-Mountain1_h*2-Door_h;
		DSound_Stop_Sound(evil_sound_id);
		DSound_Play(arrival_sound_id, DSBPLAY_LOOPING);
		
	}
   // test for page flip
   else if (curr_page_x > 0 && curr_page < 30)
      { 
      // scroll to next page to left
      curr_page_x--;

      // reset character to right edge of screen
      Lugh.x = SCREEN_WIDTH - ((float)(Lugh.width >> 1));
   
      } // end if
   } // end if   

// page flip down
else if (Lugh.y > SCREEN_HEIGHT - (Lugh.height >> 1))
   {
   // bump back regardless
   Lugh.y-=dy;
   // test for page down
   
   if(curr_page == 32)
	{
		curr_page_x = 0;
		curr_page_y = 0;
		// reset character to top edge of screen
		Lugh.x = 300+(AncientTree_w/2);
		Lugh.y = 240+AncientTree_h-Door_h+((float)(Lugh.height >> 1));
		DSound_Stop_Sound(evil_sound_id);
		DSound_Play(arrival_sound_id, DSBPLAY_LOOPING);
	}
	else if (curr_page == 33)
	{
		curr_page_x = 3;
		curr_page_y = 0;
		// reset character to right edge of screen
		Lugh.x = (Castle_w/2)-(Door_w/2)+(Water2_w*2+Water2_w/2);
		Lugh.y = Castle_h-Door_h+102 + ((float)(Lugh.height >> 1));
		DSound_Stop_Sound(evil_sound_id);
		DSound_Play(arrival_sound_id, DSBPLAY_LOOPING);
	}
    else if (curr_page_y < 4 && curr_page < 30)
      { 
      // scroll to next page down
      curr_page_y++;

      // reset character to top edge of screen
      Lugh.y = ((float)-(Lugh.height >> 1)) + SCREEN_OFFSET;
		   
      } // end if
	
   } // end if	  
else // page flip up 
if (Lugh.y < (-(Lugh.height >> 1) + SCREEN_OFFSET))
   {
   // bump back regardless
   Lugh.y= ((float)-(Lugh.height >> 1)) + SCREEN_OFFSET;

   // test for page flip up
   if (curr_page_y > 0 && curr_page < 30)
      { 
      // scroll to next page to up
      curr_page_y--;

      // reset character to bottom edge of screen
      Lugh.y = SCREEN_HEIGHT - ((float)(Lugh.height >> 1));
   
      } // end if

   } // end if   

if ((Lugh.x < 0 || Lugh.x > SCREEN_WIDTH) &&
	(Lugh.y < 0 || Lugh.y > SCREEN_HEIGHT))
{
	//Lugh.x= ((float)-(Lugh.width >> 1));
	//Lugh.y = SCREEN_HEIGHT - ((float)(Lugh.height >> 1));
  
	Lugh.x = 320;
	Lugh.y = 340;
	if(curr_page < 25)
		curr_page_x -= 1;
}
}

// flip the surfaces
DDraw_Flip();

// sync to 30ish fps
Wait_Clock(30);

if(IsIntro)
{
	if(KEY_DOWN(VK_SPACE) || keyboard_state[DIK_SPACE])
	{
		IsIntro = 0;
		DSound_Stop_Sound(end_sound_id);
		DSound_Play(arrival_sound_id, DSBPLAY_LOOPING);
	}
}

// check of user is trying to exit
if (KEY_DOWN(VK_ESCAPE) || keyboard_state[DIK_ESCAPE])
    {
    PostMessage(main_window_handle, WM_DESTROY,0,0);

    // stop all sounds
    DSound_Stop_All_Sounds();

    // do a screen transition
    Screen_Transitions(SCREEN_GREENNESS,NULL,0);

    } // end if

// return success
return(1);

} // end Game_Main

//////////////////////////////////////////////////////////

void Set_Idle_Animation(int curr_direction)
{
	switch(curr_direction)
	{
	case LUGH_SOUTH:
	case LUGH_SOUTH_ATTACK: 
		Set_Animation_BOB(&Lugh, LUGH_SOUTH_IDLE);
		return;
	case LUGH_NORTH:
    case LUGH_NORTH_ATTACK: 
		Set_Animation_BOB(&Lugh, LUGH_NORTH_IDLE);
		return;
	case LUGH_WEST:
    case LUGH_WEST_ATTACK: 
		Set_Animation_BOB(&Lugh, LUGH_WEST_IDLE);
		return;
	case LUGH_EAST:
    case LUGH_EAST_ATTACK: 
		Set_Animation_BOB(&Lugh, LUGH_EAST_IDLE);
		return;
	}
}

//////////////////////////////////////////////////////////////
void Set_Attack_Animation(int curr_direction)
{
	switch(curr_direction)
	{
	case LUGH_SOUTH:
	case LUGH_SOUTH_IDLE: 
		Set_Animation_BOB(&Lugh, LUGH_SOUTH_ATTACK);
		return;
	case LUGH_NORTH:
    case LUGH_NORTH_IDLE: 
		Set_Animation_BOB(&Lugh, LUGH_NORTH_ATTACK);
		return;
	case LUGH_WEST:
    case LUGH_WEST_IDLE: 
		Set_Animation_BOB(&Lugh, LUGH_WEST_ATTACK);
		return;
	case LUGH_EAST:
    case LUGH_EAST_IDLE: 
		Set_Animation_BOB(&Lugh, LUGH_EAST_ATTACK);
		return;
	}
}


//////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////

int Create_Page_BOBS()
{
	int index;

////////////////////////////////////////////////////////////////////////////////////
// load the Tree1 bitmaps
Load_Bitmap_File(&bitmap8bit, "BOBS/TREEBOB1.BMP");

// create the Background BOBs
for (index = 0; index < NUM_TREE1; index++)
{
	if(!Create_BOB(&Tree1[index],-100,-100,Tree1_w,Tree1_h,1,BOB_ATTR_VISIBLE, DDSCAPS_SYSTEMMEMORY))
	{
		return 0;
	}
	// load the frame into the BOB
	Load_Frame_BOB(&Tree1[index],&bitmap8bit,0,0,0,BITMAP_EXTRACT_MODE_CELL);

}

// unload Tree1
Unload_Bitmap_File(&bitmap8bit); 

//////////////////////////////////////////////////////////////////////////////////////
// load the Tree2 bitmaps
Load_Bitmap_File(&bitmap8bit, "BOBS/TREEBOB2.BMP");

// create the Background BOBs
for (index = 0; index < NUM_TREE2; index++)
{
	if(!Create_BOB(&Tree2[index],0,0,Tree2_w,Tree2_h,1,BOB_ATTR_VISIBLE, DDSCAPS_SYSTEMMEMORY))
	{
		return 0;
	}
	// load the frame into the BOB
	Load_Frame_BOB(&Tree2[index],&bitmap8bit,0,0,0,BITMAP_EXTRACT_MODE_CELL);

}

// unload Tree2
Unload_Bitmap_File(&bitmap8bit); 

//////////////////////////////////////////////////////////////////////////////////////
// load the Swamp1 bitmaps
Load_Bitmap_File(&bitmap8bit, "BOBS/SWAMPBOB1.BMP");

// create the Background BOBs
for (index = 0; index < NUM_SWAMP1; index++)
{
	if(!Create_BOB(&Swamp1[index],0,0,Swamp1_w,Swamp1_h,1,BOB_ATTR_VISIBLE, DDSCAPS_SYSTEMMEMORY))
	{
		return 0;
	}
	// load the frame into the BOB
	Load_Frame_BOB(&Swamp1[index],&bitmap8bit,0,0,0,BITMAP_EXTRACT_MODE_CELL);

}

// unload Swamp1
Unload_Bitmap_File(&bitmap8bit); 

////////////////////////////////////////////////////////////////////////////////////////
// load the Swamp2 bitmaps
Load_Bitmap_File(&bitmap8bit, "BOBS/SWAMPBOB2.BMP");

// create the Background BOBs
for (index = 0; index < NUM_SWAMP2; index++)
{
	if(!Create_BOB(&Swamp2[index],0,0,Swamp2_w,Swamp2_h,1,BOB_ATTR_VISIBLE, DDSCAPS_SYSTEMMEMORY))
	{
		return 0;
	}
	// load the frame into the BOB
	Load_Frame_BOB(&Swamp2[index],&bitmap8bit,0,0,0,BITMAP_EXTRACT_MODE_CELL);

}

// unload Swamp2
Unload_Bitmap_File(&bitmap8bit);

///////////////////////////////////////////////////////////////////////////////////////////
// load the Water1 bitmaps
Load_Bitmap_File(&bitmap8bit, "BOBS/WATERBOB1.BMP");

// create the Background BOBs
for (index = 0; index < NUM_WATER1; index++)
{
	if(!Create_BOB(&Water1[index],0,0,Water1_w,Water1_h,1,BOB_ATTR_VISIBLE, DDSCAPS_SYSTEMMEMORY))
	{
		return 0;
	}
	// load the frame into the BOB
	Load_Frame_BOB(&Water1[index],&bitmap8bit,0,0,0,BITMAP_EXTRACT_MODE_CELL);

}
// unload Water1
Unload_Bitmap_File(&bitmap8bit); 	

/////////////////////////////////////////////////////////////////////////////////////////////
// load the Water2 bitmaps
Load_Bitmap_File(&bitmap8bit, "BOBS/WATERBOB2.BMP");

// create the Background BOBs
for (index = 0; index < NUM_WATER2; index++)
{
	if(!Create_BOB(&Water2[index],0,0,Water2_w,Water2_h,1,BOB_ATTR_VISIBLE, DDSCAPS_SYSTEMMEMORY))
	{
		return 0;
	}
	// load the frame into the BOB
	Load_Frame_BOB(&Water2[index],&bitmap8bit,0,0,0,BITMAP_EXTRACT_MODE_CELL);

}

// unload Water2
Unload_Bitmap_File(&bitmap8bit); 	

///////////////////////////////////////////////////////////////////////////////////////////
// load the Rock1 bitmaps
Load_Bitmap_File(&bitmap8bit, "BOBS/ROCKBOB1.BMP");

// create the Background BOBs
for (index = 0; index < NUM_ROCK1; index++)
{
	if(!Create_BOB(&Rock1[index],0,0,Rock1_w,Rock1_h,1,BOB_ATTR_VISIBLE, DDSCAPS_SYSTEMMEMORY))
	{
		return 0;
	}
	// load the frame into the BOB
	Load_Frame_BOB(&Rock1[index],&bitmap8bit,0,0,0,BITMAP_EXTRACT_MODE_CELL);

}

// unload Rock1
Unload_Bitmap_File(&bitmap8bit); 

//////////////////////////////////////////////////////////////////////////////////////////
// load the Rock2 bitmaps
Load_Bitmap_File(&bitmap8bit, "BOBS/ROCKBOB2.BMP");

// create the Background BOBs
for (index = 0; index < NUM_ROCK2; index++)
{
	if(!Create_BOB(&Rock2[index],0,0,Rock2_w,Rock2_h,1,BOB_ATTR_VISIBLE, DDSCAPS_SYSTEMMEMORY))
	{
		return 0;
	}
	// load the frame into the BOB
	Load_Frame_BOB(&Rock2[index],&bitmap8bit,0,0,0,BITMAP_EXTRACT_MODE_CELL);

}

// unload Rock2
Unload_Bitmap_File(&bitmap8bit); 	

/////////////////////////////////////////////////////////////////////////////////////////////
// load the Stone1 bitmaps
Load_Bitmap_File(&bitmap8bit, "BOBS/STONEBOB1.BMP");

// create the Background BOBs
for (index = 0; index < NUM_STONE1; index++)
{
	if(!Create_BOB(&Stone1[index],0,0,Stone1_w,Stone1_h,1,BOB_ATTR_VISIBLE, DDSCAPS_SYSTEMMEMORY))
	{
		return 0;
	}
	// load the frame into the BOB
	Load_Frame_BOB(&Stone1[index],&bitmap8bit,0,0,0,BITMAP_EXTRACT_MODE_CELL);

}

// unload Stone1
Unload_Bitmap_File(&bitmap8bit); 

////////////////////////////////////////////////////////////////////////////////////////////
// load the Stone2 bitmaps
Load_Bitmap_File(&bitmap8bit, "BOBS/STONEBOB2.BMP");

// create the Background BOBs
for (index = 0; index < NUM_STONE2; index++)
{
	if(!Create_BOB(&Stone2[index],0,0,Stone2_w,Stone2_h,1,BOB_ATTR_VISIBLE, DDSCAPS_SYSTEMMEMORY))
	{
		return 0;
	}
	// load the frame into the BOB
	Load_Frame_BOB(&Stone2[index],&bitmap8bit,0,0,0,BITMAP_EXTRACT_MODE_CELL);

}

// unload Stone2
Unload_Bitmap_File(&bitmap8bit); 	

///////////////////////////////////////////////////////////////////////////////////////////
// load the Mountain1 bitmaps
Load_Bitmap_File(&bitmap8bit, "BOBS/MOUNTAINBOB1.BMP");

// create the Background BOBs
for (index = 0; index < NUM_MOUNTAIN1; index++)
{
	if(!Create_BOB(&Mountain1[index],0,0,Mountain1_w,Mountain1_h,1,BOB_ATTR_VISIBLE, DDSCAPS_SYSTEMMEMORY))
	{
		return 0;
	}
	// load the frame into the BOB
	Load_Frame_BOB(&Mountain1[index],&bitmap8bit,0,0,0,BITMAP_EXTRACT_MODE_CELL);

}

// unload Mountain1
Unload_Bitmap_File(&bitmap8bit);

///////////////////////////////////////////////////////////////////////////////////////////
// load the Bridge1 bitmaps
Load_Bitmap_File(&bitmap8bit, "BOBS/BRIDGEBOB1.BMP");

// create the Background BOBs

if(!Create_BOB(&Bridge1,0,0,Bridge1_w,Bridge1_h,1,BOB_ATTR_VISIBLE, DDSCAPS_SYSTEMMEMORY))
{
	return 0;
}
// load the frame into the BOB
Load_Frame_BOB(&Bridge1,&bitmap8bit,0,0,0,BITMAP_EXTRACT_MODE_CELL);


// unload Bridge1
Unload_Bitmap_File(&bitmap8bit);

///////////////////////////////////////////////////////////////////////////////////////////
// load the AncientTree bitmaps
Load_Bitmap_File(&bitmap8bit, "BOBS/ANCIENTTREEBOB.BMP");

// create the Background BOBs

if(!Create_BOB(&AncientTree,0,0,AncientTree_w,AncientTree_h,1,BOB_ATTR_VISIBLE, DDSCAPS_SYSTEMMEMORY))
{
	return 0;
}
// load the frame into the BOB
Load_Frame_BOB(&AncientTree,&bitmap8bit,0,0,0,BITMAP_EXTRACT_MODE_CELL);


// unload AncientTree
Unload_Bitmap_File(&bitmap8bit);
//////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////
// load the Door[index] bitmaps
Load_Bitmap_File(&bitmap8bit, "BOBS/DOORBOB.BMP");

// create the Background BOBs
for(index = 0; index < NUM_DOOR; index++)
{
	if(!Create_BOB(&Door[index],0,0,Door_w,Door_h,1,BOB_ATTR_VISIBLE, DDSCAPS_SYSTEMMEMORY))
	{
		return 0;
	}
	// load the frame into the BOB
	Load_Frame_BOB(&Door[index],&bitmap8bit,0,0,0,BITMAP_EXTRACT_MODE_CELL);
}


// unload Door[index]
Unload_Bitmap_File(&bitmap8bit);
//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
// load the Castle bitmaps
Load_Bitmap_File(&bitmap8bit, "BOBS/CASTLEBOB.BMP");

// create the Background BOBs

if(!Create_BOB(&Castle,0,0,Castle_w,Castle_h,1,BOB_ATTR_VISIBLE, DDSCAPS_SYSTEMMEMORY))
{
	return 0;
}
// load the frame into the BOB
Load_Frame_BOB(&Castle,&bitmap8bit,0,0,0,BITMAP_EXTRACT_MODE_CELL);


// unload Castle
Unload_Bitmap_File(&bitmap8bit);
//////////////////////////////////////////////////////////////////////////////////////


	return 1;
}


////////////////////////////////////////////////////////////////////

void Draw_Page_BOBS(int curr_page)
{
	int index = 0;

	switch(curr_page)
	{
	case 0:
		{	// Draw Tree1
			Set_Pos_BOB(&Tree1[0], 0, 100);
			Draw_BOB(&Tree1[0], lpddsback);
			Set_Pos_BOB(&Tree1[1], Tree1_w, 100);
			Draw_BOB(&Tree1[1], lpddsback);
			Set_Pos_BOB(&Tree1[2], Tree1_w*2, 100);
			Draw_BOB(&Tree1[2], lpddsback);
			Set_Pos_BOB(&Tree1[3], Tree1_w*3, 100);
			Draw_BOB(&Tree1[3], lpddsback);
			Set_Pos_BOB(&Tree1[4], Tree1_w*4, 100);
			Draw_BOB(&Tree1[4], lpddsback);
			Set_Pos_BOB(&Tree1[5], Tree1_w*5, 100);
			Draw_BOB(&Tree1[5], lpddsback);
			Set_Pos_BOB(&Tree1[6], Tree1_w*6, 100);
			Draw_BOB(&Tree1[6], lpddsback);
			Set_Pos_BOB(&Tree1[7], 0, Tree1_h +100);
			Draw_BOB(&Tree1[7], lpddsback);
			Set_Pos_BOB(&Tree1[8], 0, 2*Tree1_h +100);
			Draw_BOB(&Tree1[8], lpddsback);
			Set_Pos_BOB(&Tree1[9], 0, 3*Tree1_h+100);
			Draw_BOB(&Tree1[9], lpddsback);
			Set_Pos_BOB(&Tree2[0], Tree1_w, 3*Tree2_h+80);
			Draw_BOB(&Tree2[0], lpddsback);
			Set_Pos_BOB(&Tree2[1], 2*Tree1_w, 1*Tree2_h+100);
			Draw_BOB(&Tree2[1], lpddsback);
			Set_Pos_BOB(&Tree2[2], 120, 2*Tree2_h+100);
			Draw_BOB(&Tree2[2], lpddsback);
			Set_Pos_BOB(&AncientTree, 320, 240);
			Draw_BOB(&AncientTree, lpddsback);
			Set_Pos_BOB(&Door[1], 300+(AncientTree_w/2),240+AncientTree_h-Door_h);
			Draw_BOB(&Door[1], lpddsback);
			return;
		}
	case 1:
		{
			Set_Pos_BOB(&Tree1[0], 0, 100);
			Draw_BOB(&Tree1[0], lpddsback);
			Set_Pos_BOB(&Tree1[1], 0, Tree1_h +100);
			Draw_BOB(&Tree1[1], lpddsback);
			Set_Pos_BOB(&Tree1[2], Tree1_w, 3*Tree2_h+80);
			Draw_BOB(&Tree1[2], lpddsback);
			Set_Pos_BOB(&Tree1[3], 0, 3*Tree1_h+100);
			Draw_BOB(&Tree1[3], lpddsback);
			Set_Pos_BOB(&Tree1[4], 200, 200);
			Draw_BOB(&Tree1[4], lpddsback);
			Set_Pos_BOB(&Tree1[5], 290, 250);
			Draw_BOB(&Tree1[5], lpddsback);
			Set_Pos_BOB(&Tree1[6], 350, 300);			
			Draw_BOB(&Tree1[6], lpddsback);
			Set_Pos_BOB(&Tree1[7], 320, 390);
			Draw_BOB(&Tree1[7], lpddsback);
			Set_Pos_BOB(&Tree1[8], 450, 190);
			Draw_BOB(&Tree1[8], lpddsback);
			Set_Pos_BOB(&Tree2[0], 40, 2*Tree1_h +100);
			Draw_BOB(&Tree2[0], lpddsback);
			Set_Pos_BOB(&Tree2[1], 2*Tree1_w, 1*Tree2_h+100);
			Draw_BOB(&Tree2[1], lpddsback);
			Set_Pos_BOB(&Tree2[2], 120, 2*Tree2_h+80);
			Draw_BOB(&Tree2[2], lpddsback);
			Set_Pos_BOB(&Tree2[3], 500, 280);
			Draw_BOB(&Tree2[3], lpddsback);
			return;
		}
	case 2:
		{
			Set_Pos_BOB(&Tree1[0], 25, 100);
			Draw_BOB(&Tree1[0], lpddsback);
			Set_Pos_BOB(&Tree1[1], 50, Tree1_h +100);
			Draw_BOB(&Tree1[1], lpddsback);
			Set_Pos_BOB(&Tree1[2], Tree1_w+50, 3*Tree2_h+80);
			Draw_BOB(&Tree1[2], lpddsback);
			Set_Pos_BOB(&Tree1[3], 0+50, 3*Tree1_h+100);
			Draw_BOB(&Tree1[3], lpddsback);
			Set_Pos_BOB(&Tree1[4], 250, 200);
			Draw_BOB(&Tree1[4], lpddsback);
			Set_Pos_BOB(&Tree1[5], 290+50, 250);
			Draw_BOB(&Tree1[5], lpddsback);
			Set_Pos_BOB(&Tree1[6], 350+50, 300);			
			Draw_BOB(&Tree1[6], lpddsback);
			Set_Pos_BOB(&Tree1[7], 300, 300);
			Draw_BOB(&Tree1[7], lpddsback);
			Set_Pos_BOB(&Tree2[0], 40+50, 2*Tree1_h +100);
			Draw_BOB(&Tree2[0], lpddsback);
			Set_Pos_BOB(&Tree2[1], 2*Tree1_w+50, 1*Tree2_h+100);
			Draw_BOB(&Tree2[1], lpddsback);
			Set_Pos_BOB(&Tree2[2], 120+50, 2*Tree2_h+80);
			Draw_BOB(&Tree2[2], lpddsback);
			return;
		}
	case 3:
		{
			Set_Pos_BOB(&Tree1[0], Tree1_w, 3*Tree2_h+80);
			Draw_BOB(&Tree1[0], lpddsback);
			Set_Pos_BOB(&Tree1[1], 0, 3*Tree1_h+100);
			Draw_BOB(&Tree1[1], lpddsback);
			Set_Pos_BOB(&Tree1[2], 250, 200);
			Draw_BOB(&Tree1[2], lpddsback);
			Set_Pos_BOB(&Tree1[3], 290, 250);
			Draw_BOB(&Tree1[3], lpddsback);
			Set_Pos_BOB(&Tree1[4], 350+50, 300);			
			Draw_BOB(&Tree1[4], lpddsback);
			Set_Pos_BOB(&Tree1[5], 300, 300);
			Draw_BOB(&Tree1[5], lpddsback);\
			Set_Pos_BOB(&Tree2[0], 40+50, 2*Tree1_h +100);
			Draw_BOB(&Tree2[0], lpddsback);
			Set_Pos_BOB(&Tree2[1], 2*Tree1_w+50, 1*Tree2_h+100);
			Draw_BOB(&Tree2[1], lpddsback);
			Set_Pos_BOB(&Tree2[2], 120+50, 2*Tree2_h+80);
			Draw_BOB(&Tree2[2], lpddsback);
			Set_Pos_BOB(&Tree2[3], 500+50, 280);
			Draw_BOB(&Tree2[3], lpddsback);
			return;
		}
	case 4:
		{
			Set_Pos_BOB(&Tree2[0], 200, 200);
			Draw_BOB(&Tree2[0], lpddsback);
			Set_Pos_BOB(&Tree2[1], 290, 250);
			Draw_BOB(&Tree2[1], lpddsback);
			Set_Pos_BOB(&Tree2[2], 350, 300);			
			Draw_BOB(&Tree2[2], lpddsback);
			Set_Pos_BOB(&Tree2[3], 320, 390);
			Draw_BOB(&Tree2[3], lpddsback);
			Set_Pos_BOB(&Tree2[4], 450, 190);
			Draw_BOB(&Tree2[4], lpddsback);
			Set_Pos_BOB(&Tree2[5], 40, 2*Tree1_h +100);
			Draw_BOB(&Tree2[5], lpddsback);
			Set_Pos_BOB(&Tree2[6], 2*Tree1_w, 1*Tree2_h+100);
			Draw_BOB(&Tree2[6], lpddsback);
			Set_Pos_BOB(&Tree2[7], 500, 280);
			Draw_BOB(&Tree2[7], lpddsback);
			return;
		}
	case 5:
		{
			Set_Pos_BOB(&Tree2[0], 200, 200);
			Draw_BOB(&Tree2[0], lpddsback);
			Set_Pos_BOB(&Tree2[1], 290, 250);
			Draw_BOB(&Tree2[1], lpddsback);
			Set_Pos_BOB(&Tree2[2], 350, 300);			
			Draw_BOB(&Tree2[2], lpddsback);
			Set_Pos_BOB(&Tree2[3], 100, 300);
			Draw_BOB(&Tree2[3], lpddsback);
			Set_Pos_BOB(&Tree1[0], 40, 200);
			Draw_BOB(&Tree1[0], lpddsback);
			Set_Pos_BOB(&Tree1[1], 500, 280);
			Draw_BOB(&Tree1[1], lpddsback);
			Set_Pos_BOB(&Rock1[1], 450, 190);
			Draw_BOB(&Rock1[1], lpddsback);
			return;
		}
	case 6:
		{
			Set_Pos_BOB(&Tree1[0], 200, 200);
			Draw_BOB(&Tree1[0], lpddsback);
			Set_Pos_BOB(&Tree1[1], 290, 250);
			Draw_BOB(&Tree1[1], lpddsback);
			Set_Pos_BOB(&Tree2[2], 350, 300);			
			Draw_BOB(&Tree2[2], lpddsback);
			Set_Pos_BOB(&Tree2[3], 100, 300);
			Draw_BOB(&Tree2[3], lpddsback);
			Set_Pos_BOB(&Tree2[0], 40, 200);
			Draw_BOB(&Tree2[0], lpddsback);
			Set_Pos_BOB(&Tree2[1], 500, 280);
			Draw_BOB(&Tree2[1], lpddsback);
			Set_Pos_BOB(&Rock2[1], 150, 190);
			Draw_BOB(&Rock2[1], lpddsback);
			return;
		}
	case 7:
		{
			Set_Pos_BOB(&Tree2[0], 200, 240);
			Draw_BOB(&Tree2[0], lpddsback);
			Set_Pos_BOB(&Tree2[1], 290, 250);
			Draw_BOB(&Tree2[1], lpddsback);
			Set_Pos_BOB(&Tree2[2], 350, 300);			
			Draw_BOB(&Tree2[2], lpddsback);
			Set_Pos_BOB(&Tree2[3], 450, 190);
			Draw_BOB(&Tree2[3], lpddsback);
			Set_Pos_BOB(&Rock1[0], 300, 390);
			Draw_BOB(&Rock1[0], lpddsback);
			Set_Pos_BOB(&Rock1[1], 40, 2*Tree1_h +100);
			Draw_BOB(&Rock1[1], lpddsback);
			Set_Pos_BOB(&Tree1[0], 2*Tree1_w, 140);
			Draw_BOB(&Tree1[0], lpddsback);
			Set_Pos_BOB(&Rock2[0], 500, 280);
			Draw_BOB(&Rock2[0], lpddsback);
			return;
		}
	
	case 8:
		{
			Set_Pos_BOB(&Tree1[0], 200, 200);
			Draw_BOB(&Tree1[0], lpddsback);
			Set_Pos_BOB(&Tree1[1], 290, 250);
			Draw_BOB(&Tree1[1], lpddsback);
			Set_Pos_BOB(&Tree1[2], 400, 130);			
			Draw_BOB(&Tree1[2], lpddsback);
			Set_Pos_BOB(&Tree1[3], 100, 300);
			Draw_BOB(&Tree1[3], lpddsback);
			Set_Pos_BOB(&Tree2[0], 40, 200);
			Draw_BOB(&Tree2[0], lpddsback);
			Set_Pos_BOB(&Tree2[1], 500, 280);
			Draw_BOB(&Tree2[1], lpddsback);
			Set_Pos_BOB(&Rock2[0], 150, 190);
			Draw_BOB(&Rock2[0], lpddsback);
			Set_Pos_BOB(&Rock1[0], 425, 400);
			Draw_BOB(&Rock1[0], lpddsback);
			return;
		}
	case 9:
		{
			Set_Pos_BOB(&Stone1[0], 300, 100);
			Draw_BOB(&Stone1[0], lpddsback);
			Set_Pos_BOB(&Stone1[1], 300, 300);
			Draw_BOB(&Stone1[1], lpddsback);
			Set_Pos_BOB(&Stone2[0], 150, 200);
			Draw_BOB(&Stone2[0], lpddsback);
			Set_Pos_BOB(&Stone2[1], 475, 200);
			Draw_BOB(&Stone2[1], lpddsback);
			return;
		}
	case 10:
		{
			Set_Pos_BOB(&Tree2[0], 200, 300);
			Draw_BOB(&Tree2[0], lpddsback);
			Set_Pos_BOB(&Tree2[1], 40, 200);
			Draw_BOB(&Tree2[1], lpddsback);
			Set_Pos_BOB(&Rock2[0], 425, 400);
			Draw_BOB(&Rock2[0], lpddsback);
			Set_Pos_BOB(&Rock1[0], 150,190);
			Draw_BOB(&Rock1[0], lpddsback);
			return;
		}
	case 11:
		{
			Set_Pos_BOB(&Tree2[0], 200, 200);
			Draw_BOB(&Tree2[0], lpddsback);
			Set_Pos_BOB(&Tree2[1], 290, 250);
			Draw_BOB(&Tree2[1], lpddsback);
			Set_Pos_BOB(&Tree2[2], 400, 130);			
			Draw_BOB(&Tree2[2], lpddsback);
			Set_Pos_BOB(&Tree1[0], 40, 200);
			Draw_BOB(&Tree1[0], lpddsback);
			Set_Pos_BOB(&Tree1[1], 500, 280);
			Draw_BOB(&Tree1[1], lpddsback);
			Set_Pos_BOB(&Rock1[0], 150, 190);
			Draw_BOB(&Rock1[0], lpddsback);
			Set_Pos_BOB(&Rock2[0], 425, 400);
			Draw_BOB(&Rock2[0], lpddsback);
			return;
		}
	case 12:
		{
			Set_Pos_BOB(&Tree1[0], 200, 200);
			Draw_BOB(&Tree1[0], lpddsback);
			Set_Pos_BOB(&Tree1[2], 400, 130);			
			Draw_BOB(&Tree1[2], lpddsback);
			Set_Pos_BOB(&Tree2[0], 290, 250);
			Draw_BOB(&Tree2[0], lpddsback);
			Set_Pos_BOB(&Tree2[1], 40, 200);
			Draw_BOB(&Tree2[1], lpddsback);
			Set_Pos_BOB(&Tree2[2], 500, 280);
			Draw_BOB(&Tree2[2], lpddsback);
			Set_Pos_BOB(&Rock1[0], 150, 320);
			Draw_BOB(&Rock1[0], lpddsback);
			Set_Pos_BOB(&Rock2[0], 425, 400);
			Draw_BOB(&Rock2[0], lpddsback);
			return;
		}
	case 13:
		{
			Set_Pos_BOB(&Tree1[0], 200, 200);
			Draw_BOB(&Tree1[0], lpddsback);
			Set_Pos_BOB(&Rock1[0], 150, 320);
			Draw_BOB(&Rock1[0], lpddsback);
			Set_Pos_BOB(&Rock1[1], 400, 130);			
			Draw_BOB(&Rock1[1], lpddsback);
			Set_Pos_BOB(&Rock2[0], 425, 400);
			Draw_BOB(&Rock2[0], lpddsback);
			Set_Pos_BOB(&Rock2[1], 40, 200);
			Draw_BOB(&Rock2[1], lpddsback);
			Set_Pos_BOB(&Rock2[2], 500, 280);
			Draw_BOB(&Rock2[2], lpddsback);
			return;
		}
	case 14:
		{
			Set_Pos_BOB(&Rock1[0], 500, 300);
			Draw_BOB(&Rock1[0], lpddsback);
			Set_Pos_BOB(&Rock2[0], 325, 400);
			Draw_BOB(&Rock2[0], lpddsback);
			Set_Pos_BOB(&Rock2[1], 140, 200);
			Draw_BOB(&Rock2[1], lpddsback);
			return;
		}
	case 15:
		{
			Set_Pos_BOB(&Water2[0], Water2_w, 100);
			Draw_BOB(&Water2[0], lpddsback);
			Set_Pos_BOB(&Water2[1], Water2_w, Water2_h + 100);
			Draw_BOB(&Water2[1], lpddsback);
			Set_Pos_BOB(&Water2[2], Water2_w, 2*Water2_h + 100);
			Draw_BOB(&Water2[2], lpddsback);
			Set_Pos_BOB(&Water2[3], Water2_w, 3*Water2_h + 100);
			Draw_BOB(&Water2[3], lpddsback);
			Set_Pos_BOB(&Water2[4], 3*Water2_w, 3*Water2_h + 100);
			Draw_BOB(&Water2[4], lpddsback);
			Set_Pos_BOB(&Water2[4], 2*Water2_w, 3*Water2_h + 100);
			Draw_BOB(&Water2[4], lpddsback);
			Set_Pos_BOB(&Bridge1, 3*Water2_w, 3*Water2_h + 60);
			Draw_BOB(&Bridge1, lpddsback);
			Set_Pos_BOB(&Water2[6], 4*Water2_w, 3*Water2_h + 100);
			Draw_BOB(&Water2[6], lpddsback);
			Set_Pos_BOB(&Water2[7], 5*Water2_w, 3*Water2_h + 100);
			Draw_BOB(&Water2[7], lpddsback);
			Set_Pos_BOB(&Water2[8], 6*Water2_w, 3*Water2_h + 100);
			Draw_BOB(&Water2[8], lpddsback);
			Set_Pos_BOB(&Water2[9], 6*Water2_w, 2*Water2_h + 100);
			Draw_BOB(&Water2[9], lpddsback);
			Set_Pos_BOB(&Water2[9], 6*Water2_w, Water2_h + 100);
			Draw_BOB(&Water2[9], lpddsback);
			Set_Pos_BOB(&Water2[10], 6*Water2_w, 100);
			Draw_BOB(&Water2[10], lpddsback);
			Set_Pos_BOB(&Castle, Water2_w*2+Water2_w/2, 100);
			Draw_BOB(&Castle, lpddsback);
			Set_Pos_BOB(&Door[2], (Castle_w/2)-(Door_w/2)+(Water2_w*2+Water2_w/2), Castle_h-Door_h+101);
			Draw_BOB(&Door[2], lpddsback);
			return;
		}
	case 19:
		{
			for(index = 0; index < 7; index++)
			{
				Set_Pos_BOB(&Water1[index], index * Water1_w + 40, 400);
				Draw_BOB(&Water1[index], lpddsback);
			}
			Set_Pos_BOB(&Rock1[0], Rock1_w, 150);
			Draw_BOB(&Rock1[0], lpddsback);
			Set_Pos_BOB(&Rock2[0], 400, 300);
			Draw_BOB(&Rock2[0], lpddsback);
			return;
		}
	case 25:
		{
			for(index = 0; index < 4; index++)
			{
				Set_Pos_BOB(&Swamp2[index], index * Swamp2_w, 100);
				Draw_BOB(&Swamp2[index], lpddsback);
			}

			Set_Pos_BOB(&Swamp1[0], index * Swamp1_w, 100);
			Draw_BOB(&Swamp1[0], lpddsback);
			for(index += 1; index < 8; index++)
			{
				Set_Pos_BOB(&Swamp2[index-1], index * Swamp2_w, 100);
				Draw_BOB(&Swamp2[index-1], lpddsback);
			}
			Set_Pos_BOB(&Swamp2[7], 7*Swamp2_w, Swamp2_h + 100);
			Draw_BOB(&Swamp2[7], lpddsback);
			Set_Pos_BOB(&Swamp1[1], 7* Swamp1_w, 2*Swamp1_h + 100);
			Draw_BOB(&Swamp1[1], lpddsback);
			Set_Pos_BOB(&Swamp2[8], 7*Swamp2_w, 3*Swamp2_h+100);
			Draw_BOB(&Swamp2[8], lpddsback);
			Set_Pos_BOB(&Swamp1[2], 7* Swamp1_w, 4*Swamp1_h + 100);
			Draw_BOB(&Swamp1[2], lpddsback);

			return;
		}
	case 27:
		{
			for(index = 0; index < 5; index++)
			{
				Set_Pos_BOB(&Mountain1[index], 560,index * Mountain1_h + 125);
				Draw_BOB(&Mountain1[index], lpddsback);
				Set_Pos_BOB(&Mountain1[index+5], 560 - Mountain1_w,index * Mountain1_h + 125);
				Draw_BOB(&Mountain1[index+5], lpddsback);
			}
			return;
		}
	case 28:
		{
			for(index = 0; index < 5; index++)
			{
				Set_Pos_BOB(&Mountain1[index], 560,index * Mountain1_h + 100);
				Draw_BOB(&Mountain1[index], lpddsback);
				Set_Pos_BOB(&Mountain1[index+5], 560 - Mountain1_w,index * Mountain1_h + 100);
				Draw_BOB(&Mountain1[index+5], lpddsback);
			}
			return;
		}
	case 29:
		{
			for(index = 0; index < 5; index++)
			{
				Set_Pos_BOB(&Mountain1[index], 560,index * Mountain1_h + 100);
				Draw_BOB(&Mountain1[index], lpddsback);
				Set_Pos_BOB(&Mountain1[index+5], 560 - Mountain1_w,index * Mountain1_h + 100);
				Draw_BOB(&Mountain1[index+5], lpddsback);
				Set_Pos_BOB(&Door[0], 638-(Mountain1_w*2),480-Mountain1_h*2-Door_h);
				Draw_BOB(&Door[0], lpddsback);
			}
			return;
		}
	}
}


///////////////////////////////////////////////////////////////////
void Enemy_AI(BOB_TYP& Enemy, int Speed, int type, int enemyIndex)
{
	if(Enemy.x < 0 && Enemy.y < 0)
	{
		Enemy.x = 300;
		Enemy.y = 125;
	}

	float xDis = Lugh.x - Enemy.x;
	float yDis = Lugh.y - Enemy.y;
	int dx = 0; 
	int dy = 0;
	int index;

	// Go East or South
	if (xDis > 0)
	{
		if (yDis > 0)
		{
			// Go East
			if (xDis > yDis)
			{
				Enemy.x += Speed;
				dx = Speed; dy = 0;
				if (Enemy.curr_animation != LUGH_EAST)
					Set_Animation_BOB(&Enemy,LUGH_EAST);
			}
			// Go South
			else
			{
				Enemy.y += Speed;
				dy = Speed; dx = 0;
				if (Enemy.curr_animation != LUGH_SOUTH)
					Set_Animation_BOB(&Enemy,LUGH_SOUTH);
			}
		}
		else 
		{
				// Go East
			if (xDis > (-1*yDis))
			{
				Enemy.x += Speed;
				dx = Speed; dy = 0;
				if (Enemy.curr_animation != LUGH_EAST)
					Set_Animation_BOB(&Enemy,LUGH_EAST);
			}
			// Go North
			else
			{
				Enemy.y -= Speed;
				dy = -Speed; dx = 0;
				if (Enemy.curr_animation != LUGH_NORTH)
					Set_Animation_BOB(&Enemy,LUGH_NORTH);
			}
		}
	}
	else if (xDis < 0)
	{
		if(yDis > 0)
		{
			// Go West
			if((-1*xDis) > yDis)
			{
				Enemy.x -= Speed;
				dx = -Speed; dy = 0;
				if (Enemy.curr_animation != LUGH_WEST)
					Set_Animation_BOB(&Enemy,LUGH_WEST);
			}
			// Go South
			else 
			{
				Enemy.y += Speed;
				dy = Speed; dx = 0;
				if (Enemy.curr_animation != LUGH_SOUTH)
					Set_Animation_BOB(&Enemy,LUGH_SOUTH);
			}
		}
		else
		{
			// Go West
			if((-1*xDis) > (-1*yDis))
			{
				Enemy.x -= Speed;
				dx = -Speed; dy = 0;
				if (Enemy.curr_animation != LUGH_WEST)
					Set_Animation_BOB(&Enemy,LUGH_WEST);
			}
			// Go North
			else
			{
				Enemy.y -= Speed;
				dy = -Speed; dx = 0;
				if (Enemy.curr_animation != LUGH_NORTH)
					Set_Animation_BOB(&Enemy,LUGH_NORTH);
			}
		}
	}

/////////////////////////////////////////////////
// Collision Detection
for(index = 0; index < NUM_TREE1; index++)
{
	if(Collision_BOBS(&Enemy, &Tree1[index]))
	{
		Enemy.x -= dx;
		Enemy.y -= dy;
	}
}

for(index = 0; index < NUM_TREE2; index++)
{
	if(Collision_BOBS(&Enemy, &Tree2[index]))
	{
		Enemy.x -= dx;
		Enemy.y -= dy;
	}
}

for(index = 0; index < NUM_SWAMP1; index++)
{
	if(Collision_BOBS(&Enemy, &Swamp1[index]))
	{
		Enemy.x -= dx;
		Enemy.y -= dy;
	}
}

for(index = 0; index < NUM_SWAMP2; index++)
{
	if(Collision_BOBS(&Enemy, &Swamp2[index]))
	{
		Enemy.x -= dx;
		Enemy.y -= dy;
	}
}

for(index = 0; index < NUM_WATER1; index++)
{
	if(Collision_BOBS(&Enemy, &Water1[index]))
	{
		Enemy.x -= dx;
		Enemy.y -= dy;
	}
}

for(index = 0; index < NUM_WATER2; index++)
{
	if(Collision_BOBS(&Enemy, &Water2[index]))
	{
		Enemy.x -= dx;
		Enemy.y -= dy;
	}
}

for(index = 0; index < NUM_ROCK1; index++)
{
	if(Collision_BOBS(&Enemy, &Rock1[index]))
	{
		Enemy.x -= dx;
		Enemy.y -= dy;
	}
}
for(index = 0; index < NUM_ROCK2; index++)
{
	if(Collision_BOBS(&Enemy, &Rock2[index]))
	{
		Enemy.x -= dx;
		Enemy.y -= dy;
	}
}

for(index = 0; index < NUM_STONE1; index++)
{
	if(Collision_BOBS(&Enemy, &Stone1[index]))
	{
		Enemy.x -= dx;
		Enemy.y -= dy;
	}
}
for(index = 0; index < NUM_STONE2; index++)
{
	if(Collision_BOBS(&Enemy, &Stone2[index]))
	{
		Enemy.x -= dx;
		Enemy.y -= dy;
	}
}

for(index = 0; index < NUM_MOUNTAIN1; index++)
{
	if(Collision_BOBS(&Enemy, &Mountain1[index]))
	{
		Enemy.x -= dx;
		Enemy.y -= dy;
	}
}
if(Collision_BOBS(&Enemy, &AncientTree))
{
	Lugh.x -= dx;
	Lugh.y -= dy;
}


/////////////////////////////////////////////////

// end of page?
if (Enemy.x > SCREEN_WIDTH - (Enemy.width >> 1))
   {
   // bump back 
   Enemy.x-=dx;

   } // end if   
else // end of page?
if (Enemy.x < -(Enemy.width >> 1))
   {
   // bump back 
   Enemy.x= ((float)-(Enemy.width >> 1));

   } // end if   

// end of page?
else if (Enemy.y > SCREEN_HEIGHT - (Enemy.height >> 1))
   {
   // bump back 
   Enemy.y-=dy;

   } // end if	  
else // end of page? 
if (Enemy.y < (-(Enemy.height >> 1) + SCREEN_OFFSET))
   {
   // bump back 
   Enemy.y= ((float)-(Enemy.height >> 1)) + SCREEN_OFFSET;

   } // end if   
}
///////////////////////////////////////////////////////////////////
void Player_Hit_FirBolg(int index)
{
	if(Collision_BOBS(&FirBolg[index], &Lugh))
	{	
		if (player_attacking)
		{
			FirBolgHitPoints[index] -= 5;
			if(FirBolg[index].curr_animation == LUGH_SOUTH)
				FirBolg[index].y -= (FirBolg[index].height+2*FirBolg_Speed);
			else
			if(FirBolg[index].curr_animation == LUGH_NORTH)
				FirBolg[index].y += (FirBolg[index].height+2*FirBolg_Speed);
			else
			if(FirBolg[index].curr_animation == LUGH_WEST)
				FirBolg[index].x += (FirBolg[index].width+2*FirBolg_Speed);
			else
			if(FirBolg[index].curr_animation == LUGH_EAST)
				FirBolg[index].x -= (FirBolg[index].width+2*FirBolg_Speed);
			DSound_Play(firbolgdamage_sound_id, 0);
		}
		else
		{
			LUGH_HP -= 5;
			
			if(Lugh.curr_animation == LUGH_SOUTH)
				Lugh.y -= (Lugh.height+2*Lugh_Speed);
			else
			if(Lugh.curr_animation == LUGH_NORTH)
				Lugh.y += (Lugh.height+2*Lugh_Speed);
			else
			if(Lugh.curr_animation == LUGH_WEST)
				Lugh.x += (Lugh.width+2*Lugh_Speed);
			else
			if(Lugh.curr_animation == LUGH_EAST)
				Lugh.x -= (Lugh.width+2*Lugh_Speed);

			DSound_Play(damage_sound_id, 0);
			DSound_Play(firbolgattack_sound_id, 0);
		}
	}
	if (FirBolgHitPoints[index] <= 0)
	{
		Set_Pos_BOB(&FirBolg[index], -100, -100);
		FirBolgHitPoints[index] = FIRBOLG_HP;
		FirBolgActive[index] = 0;
		Minions_Dead++;
	}

}

///////////////////////////////////////////////////////////////////
void Player_Hit_Fomorian(int index)
{
	
	if(Collision_BOBS(&Fomorian[index], &Lugh))
	{	
		if (player_attacking)
		{
			FomorianHitPoints[index] -= 5;
			if(Fomorian[index].curr_animation == LUGH_SOUTH)
				Fomorian[index].y -= (Fomorian[index].height+2*Fomorian_Speed);
			else
			if(Fomorian[index].curr_animation == LUGH_NORTH)
				Fomorian[index].y += (Fomorian[index].height+2*Fomorian_Speed);
			else
			if(Fomorian[index].curr_animation == LUGH_WEST)
				Fomorian[index].x += (Fomorian[index].width+2*Fomorian_Speed);
			else
			if(Fomorian[index].curr_animation == LUGH_EAST)
				Fomorian[index].x -= (Fomorian[index].width+2*Fomorian_Speed);
			
			DSound_Play(fomoriandamage_sound_id, 0);
		}
		else
		{
			LUGH_HP -= 5;
			
			if(Lugh.curr_animation == LUGH_SOUTH)
				Lugh.y -= (Lugh.height+2*Lugh_Speed);
			else
			if(Lugh.curr_animation == LUGH_NORTH)
				Lugh.y += (Lugh.height+2*Lugh_Speed);
			else
			if(Lugh.curr_animation == LUGH_WEST)
				Lugh.x += (Lugh.width+2*Lugh_Speed);
			else
			if(Lugh.curr_animation == LUGH_EAST)
				Lugh.x -= (Lugh.width+2*Lugh_Speed);
			DSound_Play(damage_sound_id, 0);
			DSound_Play(fomorianattack_sound_id, 0);

		}
	}
	if (FomorianHitPoints[index] <= 0)
	{
		Set_Pos_BOB(&Fomorian[index], -100, -100);
		FomorianHitPoints[index] = FOMORIAN_HP;
		FomorianActive[index] = 0;
		Minions_Dead++;
	}

}

///////////////////////////////////////////////////////////////////
void Draw_Enemy_BOBS()
{
	switch(curr_page)
	{
	case 0:
		{
			if(FomorianActive[0])
			{
				Enemy_AI(Fomorian[0], Fomorian_Speed, FOMORIAN_TYPE, 0);
				Player_Hit_Fomorian(0);
				Animate_BOB(&Fomorian[0]);
				Draw_BOB(&Fomorian[0], lpddsback);
			}
			return;
		}
	case 1:
		{

			if(FomorianActive[1])
			{
				Enemy_AI(Fomorian[1], Fomorian_Speed, FOMORIAN_TYPE, 1);
				Player_Hit_Fomorian(1);
				Animate_BOB(&Fomorian[1]);
				Draw_BOB(&Fomorian[1], lpddsback);
			}
			
			return;
		}
	case 2:
		{	
			if(FomorianActive[2])
			{
				Enemy_AI(Fomorian[2], Fomorian_Speed, FOMORIAN_TYPE, 2);
				Player_Hit_Fomorian(2);
				Animate_BOB(&Fomorian[2]);
				Draw_BOB(&Fomorian[2], lpddsback);
			}
			return;
		}
	case 3:
		{
			if(FomorianActive[3])
			{
				Enemy_AI(Fomorian[3], Fomorian_Speed, FOMORIAN_TYPE, 3);
				Player_Hit_Fomorian(3);
				Animate_BOB(&Fomorian[3]);
				Draw_BOB(&Fomorian[3], lpddsback);
			}
			
			return;
		}
	case 4:
		{
			if(FomorianActive[4])
			{
				Enemy_AI(Fomorian[4], Fomorian_Speed, FOMORIAN_TYPE, 4);
				Player_Hit_Fomorian(4);
				Animate_BOB(&Fomorian[4]);
				Draw_BOB(&Fomorian[4], lpddsback);
			}
			return;
		}
	case 5:
		{
			if(FomorianActive[5])
			{
				Enemy_AI(Fomorian[5], Fomorian_Speed, FOMORIAN_TYPE, 5);
				Player_Hit_Fomorian(5);
				Animate_BOB(&Fomorian[5]);
				Draw_BOB(&Fomorian[5], lpddsback);
			}
			if(FomorianActive[6])
			{
				Enemy_AI(Fomorian[6], Fomorian_Speed, FOMORIAN_TYPE, 6);
				Player_Hit_Fomorian(6);
				Animate_BOB(&Fomorian[6]);
				Draw_BOB(&Fomorian[6], lpddsback);
			}
			return;
		}
	case 6:
		{
			if(FomorianActive[7])
			{
				Enemy_AI(Fomorian[7], Fomorian_Speed, FOMORIAN_TYPE, 7);
				Player_Hit_Fomorian(7);
				Animate_BOB(&Fomorian[7]);
				Draw_BOB(&Fomorian[7], lpddsback);
			}
			return;
		}
	case 7:
		{
			if(FomorianActive[8])
			{
				Enemy_AI(Fomorian[8], Fomorian_Speed, FOMORIAN_TYPE, 8);
				Player_Hit_Fomorian(8);
				Animate_BOB(&Fomorian[8]);
				Draw_BOB(&Fomorian[8], lpddsback);
			}
			return;
		}
	case 8:
		{
			return;
		}
	case 9:
		{
			if(FirBolgActive[0])
			{
				Enemy_AI(FirBolg[0], FirBolg_Speed, FIRBOLG_TYPE, 0);
				Player_Hit_FirBolg(0);
				Animate_BOB(&FirBolg[0]);
				Draw_BOB(&FirBolg[0], lpddsback);
			}
			return;
		}
	case 10:
		{
			if(FomorianActive[9])
			{
				Enemy_AI(Fomorian[9], Fomorian_Speed, FOMORIAN_TYPE, 9);
				Player_Hit_Fomorian(9);
				Animate_BOB(&Fomorian[9]);
				Draw_BOB(&Fomorian[9], lpddsback);
			}
			if(FirBolgActive[1])
			{
				Enemy_AI(FirBolg[1], FirBolg_Speed, FIRBOLG_TYPE, 1);
				Player_Hit_FirBolg(1);
				Animate_BOB(&FirBolg[1]);
				Draw_BOB(&FirBolg[1], lpddsback);
			}
			return;
		}
	case 11:
		{

			return;
		}
	case 12:
		{
			if(FomorianActive[10])
			{
				Enemy_AI(Fomorian[10], Fomorian_Speed, FOMORIAN_TYPE, 10);
				Player_Hit_Fomorian(10);
				Animate_BOB(&Fomorian[10]);
				Draw_BOB(&Fomorian[10], lpddsback);
			}
			return;
		}
	case 13:
		{
			return;
		}
	case 14:
		{
			if(FomorianActive[11])
			{
				Enemy_AI(Fomorian[11], Fomorian_Speed, FOMORIAN_TYPE, 11);
				Player_Hit_Fomorian(11);
				Animate_BOB(&Fomorian[11]);
				Draw_BOB(&Fomorian[11], lpddsback);
			}
			if(FirBolgActive[2])
			{
				Enemy_AI(FirBolg[2], FirBolg_Speed, FIRBOLG_TYPE, 2);
				Player_Hit_FirBolg(2);
				Animate_BOB(&FirBolg[2]);
				Draw_BOB(&FirBolg[2], lpddsback);
			}
			return;
		}
	case 15:
		{
			return;
		}
	case 16:
		{
			if(FirBolgActive[3])
			{
				Enemy_AI(FirBolg[3], FirBolg_Speed, FIRBOLG_TYPE, 3);
				Player_Hit_FirBolg(3);
				Animate_BOB(&FirBolg[3]);
				Draw_BOB(&FirBolg[3], lpddsback);
			}
			if(FirBolgActive[4])
			{
				Enemy_AI(FirBolg[4], FirBolg_Speed, FIRBOLG_TYPE, 4);
				Player_Hit_FirBolg(4);
				Animate_BOB(&FirBolg[4]);
				Draw_BOB(&FirBolg[4], lpddsback);
			}
			if(FirBolgActive[5])
			{
				Enemy_AI(FirBolg[5], FirBolg_Speed, FIRBOLG_TYPE, 5);
				Player_Hit_FirBolg(5);
				Animate_BOB(&FirBolg[5]);
				Draw_BOB(&FirBolg[5], lpddsback);
			}
			return;
		}
	case 17:
		{
			if(FomorianActive[12])
			{
				Enemy_AI(Fomorian[12], Fomorian_Speed, FOMORIAN_TYPE, 12);
				Player_Hit_Fomorian(12);
				Animate_BOB(&Fomorian[12]);
				Draw_BOB(&Fomorian[12], lpddsback);
			}
			if(FirBolgActive[6])
			{
				Enemy_AI(FirBolg[6], FirBolg_Speed, FIRBOLG_TYPE, 6);
				Player_Hit_FirBolg(6);
				Animate_BOB(&FirBolg[6]);
				Draw_BOB(&FirBolg[6], lpddsback);
			}
			return;
		}
	case 18:
		{
			if(FirBolgActive[7])
			{
				Enemy_AI(FirBolg[7], FirBolg_Speed, FIRBOLG_TYPE, 7);
				Player_Hit_FirBolg(7);
				Animate_BOB(&FirBolg[7]);
				Draw_BOB(&FirBolg[7], lpddsback);
			}
			if(FirBolgActive[8])
			{
				Enemy_AI(FirBolg[8], FirBolg_Speed, FIRBOLG_TYPE, 8);
				Player_Hit_FirBolg(8);
				Animate_BOB(&FirBolg[8]);
				Draw_BOB(&FirBolg[8], lpddsback);
			}
			return;
		}
	case 19:
		{	
			return;
		}
	case 20:
		{
			if(FirBolgActive[9])
			{
				Enemy_AI(FirBolg[9], FirBolg_Speed, FIRBOLG_TYPE, 9);
				Player_Hit_FirBolg(9);
				Animate_BOB(&FirBolg[9]);
				Draw_BOB(&FirBolg[9], lpddsback);
			}
			if(FirBolgActive[10])
			{
				Enemy_AI(FirBolg[10], FirBolg_Speed, FIRBOLG_TYPE, 10);
				Player_Hit_FirBolg(10);
				Animate_BOB(&FirBolg[10]);
				Draw_BOB(&FirBolg[10], lpddsback);
			}
			if(FirBolgActive[11])
			{
				Enemy_AI(FirBolg[11], FirBolg_Speed, FIRBOLG_TYPE, 11);
				Player_Hit_FirBolg(11);
				Animate_BOB(&FirBolg[11]);
				Draw_BOB(&FirBolg[11], lpddsback);
			}
			return;
		}
	case 21:
		{
			if(FirBolgActive[12])
			{
				Enemy_AI(FirBolg[12], FirBolg_Speed, FIRBOLG_TYPE, 12);
				Player_Hit_FirBolg(12);
				Animate_BOB(&FirBolg[12]);
				Draw_BOB(&FirBolg[12], lpddsback);
			}
			return;
		}
	case 22:
		{
			if(FomorianActive[13])
			{
				Enemy_AI(Fomorian[13], Fomorian_Speed, FOMORIAN_TYPE, 13);
				Player_Hit_Fomorian(13);
				Animate_BOB(&Fomorian[13]);
				Draw_BOB(&Fomorian[13], lpddsback);
			}
			if(FirBolgActive[13])
			{
				Enemy_AI(FirBolg[13], FirBolg_Speed, FIRBOLG_TYPE, 13);
				Player_Hit_FirBolg(13);
				Animate_BOB(&FirBolg[13]);
				Draw_BOB(&FirBolg[13], lpddsback);
			}
			return;
		}
	case 23:
		{
			if(FomorianActive[14])
			{
				Enemy_AI(Fomorian[14], Fomorian_Speed, FOMORIAN_TYPE, 14);
				Player_Hit_Fomorian(14);
				Animate_BOB(&Fomorian[14]);
				Draw_BOB(&Fomorian[14], lpddsback);
			}
			if(FirBolgActive[14])
			{
				Enemy_AI(FirBolg[14], FirBolg_Speed, FIRBOLG_TYPE, 14);
				Player_Hit_FirBolg(14);
				Animate_BOB(&FirBolg[14]);
				Draw_BOB(&FirBolg[14], lpddsback);
			}
			return;
		}
	case 24:
		{
			return;
		}
	case 25:
		{
			return;
		}
	case 26:
		{
			return;
		}
	case 27:
		{
			return;
		}
	case 28:
		{
			return;
		}
	case 29:
		{
			return;
		}
	case 31:
		{	if(Bosses_Dead == 0)
			{	DomnannActive = 1;
				Boss_AI(Domnann, Domnann_Speed);
				Player_Hit_Boss(Domnann, 1);
				Animate_BOB(&Domnann);
				Draw_BOB(&Domnann, lpddsback);
			}
			return;
		}
	case 32:
	{
			if(Bosses_Dead == 1)
			{
				BalorActive = 1;
				Boss_AI(Balor, Balor_Speed);
				Player_Hit_Boss(Balor, 2);
				Animate_BOB(&Balor);
				Draw_BOB(&Balor, lpddsback);
			}
			return;
	}
	case 33:
		{
			if(Bosses_Dead > 1)
			{
				EiricActive = 1;
				Boss_AI(Eiric, Eiric_Speed);
				Player_Hit_Boss(Eiric, 3);
				Animate_BOB(&Eiric);
				Draw_BOB(&Eiric, lpddsback);
			}
			return;
		}
	}
}
/////////////////////////////////////////////////////////
void Boss_AI(BOB_TYP& Boss, int Speed)
{

if(Boss.x < 0 && Boss.y < 0)
	{
		Boss.x = 300;
		Boss.y = 125;
	}

	float xDis = Lugh.x - Boss.x;
	float yDis = Lugh.y - Boss.y;
	int dx = 0; 
	int dy = 0;

	// Go East or South
	if (xDis > 0)
	{
		if (yDis > 0)
		{
			// Go East
			if (xDis > yDis)
			{
				Boss.x += Speed;
				dx = Speed; dy = 0;
				if (Boss.curr_animation != LUGH_EAST)
					Set_Animation_BOB(&Boss,LUGH_EAST);
			}
			// Go South
			else
			{
				Boss.y += Speed;
				dy = Speed; dx = 0;
				if (Boss.curr_animation != LUGH_SOUTH)
					Set_Animation_BOB(&Boss,LUGH_SOUTH);
			}
		}
		else 
		{
				// Go East
			if (xDis > (-1*yDis))
			{
				Boss.x += Speed;
				dx = Speed; dy = 0;
				if (Boss.curr_animation != LUGH_EAST)
					Set_Animation_BOB(&Boss,LUGH_EAST);
			}
			// Go North
			else
			{
				Boss.y -= Speed;
				dy = -Speed; dx = 0;
				if (Boss.curr_animation != LUGH_NORTH)
					Set_Animation_BOB(&Boss,LUGH_NORTH);
			}
		}
	}
	else if (xDis < 0)
	{
		if(yDis > 0)
		{
			// Go West
			if((-1*xDis) > yDis)
			{
				Boss.x -= Speed;
				dx = -Speed; dy = 0;
				if (Boss.curr_animation != LUGH_WEST)
					Set_Animation_BOB(&Boss,LUGH_WEST);
			}
			// Go South
			else 
			{
				Boss.y += Speed;
				dy = Speed; dx = 0;
				if (Boss.curr_animation != LUGH_SOUTH)
					Set_Animation_BOB(&Boss,LUGH_SOUTH);
			}
		}
		else
		{
			// Go West
			if((-1*xDis) > (-1*yDis))
			{
				Boss.x -= Speed;
				dx = -Speed; dy = 0;
				if (Boss.curr_animation != LUGH_WEST)
					Set_Animation_BOB(&Boss,LUGH_WEST);
			}
			// Go North
			else
			{
				Boss.y -= Speed;
				dy = -Speed; dx = 0;
				if (Boss.curr_animation != LUGH_NORTH)
					Set_Animation_BOB(&Boss,LUGH_NORTH);
			}
		}
	}
/////////////////////////////////////////////////

// end of page?
if (Boss.x > SCREEN_WIDTH - (Boss.width >> 1))
   {
   // bump back 
   Boss.x-=dx;

   } // end if   
else // end of page?
if (Boss.x < -(Boss.width >> 1))
   {
   // bump back 
   Boss.x= ((float)-(Boss.width >> 1));

   } // end if   

// end of page?
else if (Boss.y > SCREEN_HEIGHT - (Boss.height >> 1))
   {
   // bump back 
   Boss.y-=dy;

   } // end if	  
else // end of page? 
if (Boss.y < (-(Boss.height >> 1) + SCREEN_OFFSET))
   {
   // bump back 
   Boss.y= ((float)-(Boss.height >> 1)) + SCREEN_OFFSET;

   } // end if   
}	


/////////////////////////////////////////////////////////
void Player_Hit_Boss(BOB_TYP& Enemy, int type)
{
	int damage;

	if (type == 1)
		damage = 5;
	else if (type == 2)
		damage = 10;
	else
		damage = 15;

		if(Collision_BOBS(&Enemy, &Lugh))
		{	
			if (player_attacking)
			{
				
				if(type == 1)
				{
					DOMNANN_HP-= 5;
					if(Enemy.curr_animation == LUGH_SOUTH)
						Enemy.y -= (Enemy.height+2*Domnann_Speed);
					else
					if(Enemy.curr_animation == LUGH_NORTH)
						Enemy.y += (Enemy.height+2*Domnann_Speed);
					else
					if(Enemy.curr_animation == LUGH_WEST)
						Enemy.x += (Enemy.width+2*Domnann_Speed);
					else
					if(Enemy.curr_animation == LUGH_EAST)
						Enemy.x -= (Enemy.width+2*Domnann_Speed);
				}
				else if (type == 2)
				{
					BALOR_HP -= 5;
					if(Enemy.curr_animation == LUGH_SOUTH)
						Enemy.y -= (Enemy.height+2*Balor_Speed);
					else
					if(Enemy.curr_animation == LUGH_NORTH)
						Enemy.y += (Enemy.height+2*Balor_Speed);
					else
					if(Enemy.curr_animation == LUGH_WEST)
						Enemy.x += (Enemy.width+2*Balor_Speed);
					else
					if(Enemy.curr_animation == LUGH_EAST)
						Enemy.x -= (Enemy.width+2*Balor_Speed);
				}
			
				else if (type == 3)
				{	
					EIRIC_HP -= 5;
					if(Enemy.curr_animation == LUGH_SOUTH)
						Enemy.y -= (Enemy.height+2*Eiric_Speed);
					else
					if(Enemy.curr_animation == LUGH_NORTH)
						Enemy.y += (Enemy.height+2*Eiric_Speed);
					else
					if(Enemy.curr_animation == LUGH_WEST)
						Enemy.x += (Enemy.width+2*Eiric_Speed);
					else
					if(Enemy.curr_animation == LUGH_EAST)
						Enemy.x -= (Enemy.width+2*Eiric_Speed);
				}
				DSound_Play(bossdamage_sound_id, 0);
			}
			if (!player_attacking)
			{
				LUGH_HP -= 5;
				
				if(Lugh.curr_animation == LUGH_SOUTH)
					Lugh.y -= (Lugh.height+2*Lugh_Speed);
				else
				if(Lugh.curr_animation == LUGH_NORTH)
					Lugh.y += (Lugh.height+2*Lugh_Speed);
				else
				if(Lugh.curr_animation == LUGH_WEST)
					Lugh.x += (Lugh.width+2*Lugh_Speed);
				else
				if(Lugh.curr_animation == LUGH_EAST)
					Lugh.x -= (Lugh.width+2*Lugh_Speed);
			
				DSound_Play(damage_sound_id, 0);
				DSound_Play(bossattack_sound_id, 0);

			}
		}

	if (DOMNANN_HP <= 0)
	{
		Set_Pos_BOB(&Domnann, -100, -100);
		DomnannActive = 0;
		Bosses_Dead = 1;
	if (BALOR_HP <= 0)
	{
		Set_Pos_BOB(&Balor, -100, -100);
		BalorActive = 0;
		Bosses_Dead = 2;
	}
	if (EIRIC_HP <= 0)
	{
		Set_Pos_BOB(&Eiric, -100, -100);
		EiricActive = 0;
		Bosses_Dead = 3;
		IsGameWin = 1;
	}

	}
  
}

//////////////////////////////////////////////////////////
