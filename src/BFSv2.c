#pragma config(StandardModel, "EV3_REMBOT")


/////////////////////////////// ADT QUEUE ////////////////////////////////////////
#define Head(Q) (Q).head
#define Tail(Q) (Q).tail
#define InfoHead(Q) (Q).T[(Q).head]
#define InfoTail(Q) (Q).T[(Q).tail]
#define MaxEl(Q) (Q).maxEl
#define Nil 0
#define MaxElQueue 30

typedef struct{
	int ID;
	int prentID;
	int branch1;
	int branch2;
	int branch3;
	int is90deg;
}Node;

typedef struct {
  int head;
  int tail;
  int maxEl;
  Node T[MaxElQueue];
} Queue;

int IsEmpty(Queue Q) {
	return(InfoHead(Q).ID==Nil && InfoTail(Q).ID==Nil);
}

int isOneElmt(Queue Q){
	Head(Q) == Tail(Q);
}

int IsFull(Queue Q) {
	int m = Head(Q) - Tail(Q);
	return((m==1) || (m==(-1*MaxEl(Q)+1)));
}


int NBElmt (Queue Q)
{
	int n;
	if (IsEmpty(Q)) n = 0;
	else if (Head(Q) <= Tail(Q)) n = Tail(Q) - Head(Q) + 1;
	else n = MaxEl(Q) - Tail(Q) + Head(Q) + 1;
	return n;
}

void CreateEmpty (Queue *Q)
{
	MaxEl(*Q) = MaxElQueue;
	Head(*Q) = Nil;
	Tail(*Q) = Nil;
}

void Add (Queue *Q, Node X)
{
	if (IsEmpty(*Q))
	{
		Tail(*Q) = 1;
		Head(*Q) = 1;
	}
	else
	{
		Tail(*Q) += 1;
		if (Tail(*Q) == (*Q).maxEl + 1) Tail(*Q) = 1;
	}
	InfoTail(*Q) = X;
	displayTextLine(3,"Add %d to Queue", (X).ID);
}

void Del (Queue *Q, Node *X)
{
	(*X) = InfoHead(*Q);
	if (isOneElmt(*Q)){
		Head(*Q) = Nil;
		Tail(*Q) = Nil;
	}
	else {
		Head(*Q)++;
		if (Head(*Q) == MaxEl(*Q) + 1) Head(*Q) = 1;
	}
	displayTextLine(3,"Del %d from Queue",(*X));
}

//----------------------------- END OF ADT ----------------------------------//


/////////////////////////////-|HEADER BFS|-////////////////////////////////////

#define red 10
#define green 20
#define blue 30
#define bw 50
#define threshold 65

void moveForward(int degree){
	displayTextLine(4,"Moving Forward");
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

int colorCheck(int r, int g , int b) {
	if ( r > (g+b) ) { return red; }
	else if ( g > (r+b)) { return green; }
	else if ( b> (r+g) ) { return blue; }
	else { return bw; }
}

void steerRight(){
	motor[leftMotor]  = 15;
    motor[rightMotor] = 55;
}

void steerLeft(){
   motor[leftMotor]  = 55;
   motor[rightMotor] = 15;
}

int isRGB(int color){
	return (color==red || color==green || color==blue);
}

void turnBack(){
	motor[leftMotor] = 100;
	motor[rightMotor] = -100;
	sleep(500);
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

set2MotorPower(int leftMotorPower, rightMotorPower){
		motor[leftMotor] = 50;
	motor[rightMotor] = 90;
}

void checkBranch(Queue *Q){
	displayTextLine(3,"Checking Branch");
	turnRight();

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

// M A I N   P R O G R A M --------------------------------------------------------
task main(){
	Queue Q;
	initialMove();
	int colorDetected;
	int redIntensity,greenIntensity,blueIntensity, lastRgbDetected;

	while (colorDetected != blue){
		getColorRGB(colorSensor,redIntensity,greenIntensity,lastRgbDetected);
		colorDetected = colorCheck(redIntensity,greenIntensity,lastRgbDetected);
		if(getColorReflected(colorSensor) < threshold)	{
			steerLeft();
			displayTextLine(1, "BLACK");
		}
		else {
			steerRight();
			displayTextLine(1, "WHITE");
		}

		if (isRGB(colorDetected)){
		  switch(colorDetected){
			case red:
				turnBack();
				displayTextLine(1,"");
				displayTextLine(2,"it is RED");
				lastRgbDetected=red;
				break;
			case green:
				if(lastRgbDetected!=red){
					moveForward(-150);
					checkBranch(Q);
				}
				displayTextLine(1,"");
				displayTextLine(2,"it is GREEN");
				lastRGB=green;
				break;
			case blue:
				displayTextLine(1,"");
				displayTextLine(2,"it is BLUE");
				lastRgbDetected=blue;
			}
		}
	}

}
