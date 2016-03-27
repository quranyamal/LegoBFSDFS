#pragma config(StandardModel, "EV3_REMBOT")


//==========================================================================
/////QUEUE//////
#define Head(Q) (Q).HEAD
#define Tail(Q) (Q).TAIL
#define InfoHead(Q) (Q).T[(Q).HEAD]
#define InfoTail(Q) (Q).T[(Q).TAIL]
#define maxEl(Q) (Q).MaxEl
#define Nil 0

/*** Definisi elemen dan address ***/
typedef int infotype;

typedef int address; /* indeks tabel */

/* *** Definisi Type Queue *** */
typedef struct {
	infotype T[10]; /* tabel penyimpan elemen */
	address HEAD; /*alamat penghapusan*/
	address TAIL; /*alamat penambahan*/
	int MaxEl; /*maksimum banyaknya elemen queue*/
} Queue;

int IsEmpty(Queue Q) {
	return(Head(Q)==Nil && Tail(Q)==Nil);
}

int IsFull(Queue Q) {
	int m = Head(Q) - Tail(Q);
	return((m==1) || (m==(-1*maxEl(Q)+1)));
}

int NBElmt (Queue Q)
{
	int n;
	if (IsEmpty(Q)) n = 0;
	else if (Head(Q) <= Tail(Q)) n = Tail(Q) - Head(Q) + 1;
	else n = Q.MaxEl - Tail(Q) + Head(Q) + 1;
	return n;
}

void CreateEmpty (Queue *Q)
{
	maxEl(*Q) = 50;
	Head(*Q) = Nil;
	Tail(*Q) = Nil;
}

void Add (Queue *Q,infotype X)
{
	if (IsEmpty(*Q))
	{
		Tail(*Q) = 1;
		Head(*Q) = 1;
	}
	else
	{
		Tail(*Q) += 1;
		if (Tail(*Q) == (*Q).MaxEl + 1) Tail(*Q) = 1;
	}
	InfoTail(*Q) = X;
	displayTextLine(3,"Add %d to Queue",X);
}

void Del (Queue *Q,infotype *X)
{
	(*X) = InfoHead(*Q);
	if (Head(*Q) == Tail(*Q))	{
		Head(*Q) = Nil;
		Tail(*Q) = Nil;
	}
	else {
		Head(*Q)++;
		if (Head(*Q) == maxEl(*Q) + 1) Head(*Q) = 1;
	}
	displayTextLine(3,"Del %d from Queue",(*X));
}

//==========================================================================

#define red 10
#define green 20
#define blue 30
#define notrgb 99
#define threshold 65

#define left -1
#define straight 0
#define right 1
#define first -2

//#include "ADT_BFS.h"


int mv=0,BW=0;
int isChecked=0;

void steerRight(){
	motor[leftMotor]  = 15;
    motor[rightMotor] = 55;
}

void steerLeft(){
   motor[leftMotor]  = 55;
   motor[rightMotor] = 15;
}

void turnBack(){
	motor[leftMotor] = 100;
	motor[rightMotor] = -100;
	sleep(500);
}


int colorCheck(int r, int g , int b) {
	if ( r > (g+b) ){
		return red;
	}
	else if ( g > (r+b)) {
		return green;
	}
	else if ( b> (r+g) ) {
		return blue;
	}
	else {
		return notrgb;
	}
}

void turnRight(){
	moveMotorTarget(leftMotor,225,200);
	moveMotorTarget(rightMotor,-225,200);
	while ( getMotorMoving(leftMotor) || getMotorMoving(rightMotor) ) {
			sleep(1);
	}
}

void exitBFS(){
	motor[leftMotor] = 100;
	motor[rightMotor] = 100;
	delay(1000);
	turnBack();
	motor[leftMotor] = 0;
	motor[rightMotor] = 0;
	displayTextLine(1, "PROGRAM EXIT");
}

void moveForward(int degree){
	displayTextLine(4,"Moving Forward | %d", ++mv);
	moveMotorTarget(leftMotor,degree,100);
	moveMotorTarget(rightMotor,degree,100);
	while ( getMotorMoving(leftMotor) || getMotorMoving(rightMotor) ) {
		sleep(1);
	}
	displayTextLine(4,"");
}

void initialMove(){
	moveForward(700);
}

void turnRight(){
	moveMotorTarget(leftMotor,225,200);
	moveMotorTarget(rightMotor,-225,200);
	while ( getMotorMoving(leftMotor) || getMotorMoving(rightMotor) ) {
		sleep(1);
	}
}
void turnLeft(){
	moveMotorTarget(leftMotor,-220,200);
	moveMotorTarget(rightMotor,220,200);
	while ( getMotorMoving(leftMotor) || getMotorMoving(rightMotor) ) {
		sleep(1);
	}
}
void turn(int direction, int degree){
	//turn toward a by x degrees
	int rigtPower = direction==right ? -80:80;
}

void checkBranch(Queue *Q){
	//cek kanan
	moveForward(-150);
	displayTextLine(3,"Checking Branch");
	moveMotorTarget(leftMotor,225,200);
	moveMotorTarget(rightMotor,-225,200);
	while ( getMotorMoving(leftMotor) || getMotorMoving(rightMotor) ) {
		sleep(1);
	}
	motor[leftMotor] = 50;
	motor[rightMotor] = 90;
	int tDelay = 4500, i=0;
	int nBranch=0, blck=0;
	while(i<tDelay){
		if(getColorReflected(colorSensor) < threshold)	{
			displayTextLine(1, "BLACK | blck=%d", ++blck);
		}
		else {
			displayTextLine(1, "WHITE");
			if(blck>100 && i<tDelay-600){
				nBranch++;
				Add(Q, nBranch);
				blck=0;
			}
		}
		displayTextLine(5,"NBElmt Queue = %d",NBElmt(*Q));
		sleep(1);
		i++;
	}

	motor[leftMotor] = -55;
	motor[rightMotor] = 55;
	displayTextLine(3,"Done Checking");
	displayTextLine(4, "it is %d-way junction", nBranch);
	sleep(600);
	isChecked=1;
	BW = 0;
}


int isRGB(int color){
	return color==red || color==green || color==blue;
}

task main()
{
	Queue Q;
	int r,g,b;
	int colorDetected;
	int nRed=0,nGreen=0,nBlue=0;
	int saklarNode = 150000;
	int lastRGB;

	CreateEmpty(Q);
	initialMove();
	while(colorDetected!=blue){
		displayTextLine(1,"");
		if(getColorReflected(colorSensor) < threshold)	{
			steerLeft();
			displayTextLine(1, "BLACK | BW=%d", ++BW);
		}
		else {
			steerRight();
			displayTextLine(1, "WHITE | BW=%d", ++BW);
		}
		if (isRGB(colorDetected)){
			displayTextLine(6,"lastRGB = %d",lastRGB);
			switch(colorDetected){
			case red:
				turnBack();
				displayTextLine(1,"");
				displayTextLine(2,"RED | %d", ++nRed);
				lastRGB=red;
				break;
			case green:
				if(BW>saklarNode && lastRGB!=red) checkBranch(Q);
				displayTextLine(1,"");
				displayTextLine(2,"GREEN | %d", ++nGreen);
				lastRGB=green;
				break;
			case blue:
				displayTextLine(1,"");
				displayTextLine(2,"BLUE | %d", ++nBlue);
				lastRGB=blue;
			}
			//lastRGB=colorDetected;
		}
		getColorRGB(colorSensor,r,g,b);
		colorDetected = colorCheck(r,g,b);
	}
	exitBFS();
}
