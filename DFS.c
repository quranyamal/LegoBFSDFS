#pragma config(StandardModel, "EV3_REMBOT")

//	Pencarian Jalan Keluar Labirin menggunakan DFS
//	Tugas Besar Strategi Algoritma
//	8 Maret 2016


#define red 1
#define green 2
#define blue 3
#define notrgb 0
#define threshold 65



//STACK

#define Top(S) (S).TOP
#define InfoTop(S) (S).T[Top(S)]



typedef struct{
		int T[101];
		int TOP;
} Stack;

void CreateEmpty(Stack * S){
	Top(*S) = 0;
}

bool isEmpty(Stack * S){
	return (Top(*S) == 0);
}

bool isFull(Stack * S){
	return (Top(*S) == 100);
}

void Push(Stack * S, int x){
		if ( !isFull(S)){
		Top(*S)++;
		InfoTop(*S) = x;
		}
}

void Pop(Stack * S, int * x){
		if( !isEmpty(S)) {
			*x = InfoTop(*S);
			Top(*S)--;
		}
}

void TulisStack(Stack * S) {
		string x;
		for (int i=1; i <= S->TOP ; i++){
				displayTextLine(i+1,"->%d",(*S).T[i]);
		}
}

Stack S;








//Maju ke depan sebesar "degree"
void moveForward(int degree){
	moveMotorTarget(leftMotor,degree,50);
	moveMotorTarget(rightMotor,degree,50);
	while ( getMotorMoving(leftMotor) || getMotorMoving(rightMotor) ) {
		sleep(1);
	}
}

int isColorDetected(int r, int g , int b) {
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



//Gerakan inisialisasi robot untuk keluar dari box "start"
void initialMove(){
	moveForward(700);
	while ( getColorHue(colorSensor) != 0){
		moveForward(100);
	}
}




bool isSensorSeesLight() {
		return (getColorReflected(colorSensor) < threshold);
}



void steerLeft() {
		motor[leftMotor]  = 55;
  	motor[rightMotor] = 15;
}

void steerRight() {
		motor[leftMotor]  = 15;
  	motor[rightMotor] = 55;
}

void steerRight2(){
	motor[leftMotor]  = -55;
 	motor[rightMotor] = 55;	
}

void steerLeft2() {
		motor[leftMotor]  = 55;
  	motor[rightMotor] = -55;
}


//Menggerakkan robot ke ujung labirin
//Mengembalikan "red" jika robot bertemu jalan buntu
//Mengembalikan "blue" jika robot bertemu jalan keluar
//Jika menemui persimpangan, prioritas robot memilih jalan
// 		1. Belok kiri
//		2. Lurus
//		3. Belok Kanan
int moveToColor() {
	int count = 0;

	while(true)
  {
  	//Line follower
  	//Jika menemui persimpangan, gunakan prioritas di atas
    if (isSensorSeesLight()){
    		steerLeft();
    }
    else {
 		   	steerRight();
    }

  	//Deteksi leaf node
    //Jika robot mendeteksi warna merah/biru , proses berhenti
    int r,g,b;
    getColorRGB(colorSensor,r,g,b);
  	int colorSeen = isColorDetected(r,g,b);
  	displayTextLine(1,"color : %d -> %d %d %d", colorSeen,r,g,b);
    if ( colorSeen ) {
				return colorSeen;
		}
  }
}

//Putar robot 180 derajat
void turnBack(){
	resetGyro(gyroSensor);
	moveMotorTarget(leftMotor,440,200);
	moveMotorTarget(rightMotor,-440,200);
	while ( getMotorMoving(leftMotor) || getMotorMoving(rightMotor) ) {
		sleep(1);
	}
	//displayTextLine(1,"%d",getGyroDegrees(gyroSensor);
}

void turnRight(){
		moveMotorTarget(leftMotor,220,200);
	moveMotorTarget(rightMotor,-220,200);
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


int scanLine(){
	int lineCount = 0;
	resetGyro(gyroSensor);
	while (getGyroDegrees(gyroSensor) > -180) {
		if (isSensorSeesLight()) {
    		steerRight2();	
    }
    else {
				lineCount++;
				turnLeft();
		}
	
   }
	while (getGyroDegrees(gyroSensor) < -5) {
		steerLeft2();		
	}
   
   
   
	displayTextLine(1,"%d line...",lineCount);
	return lineCount;
	
	
}






//Depth First Search
void DFS() {
	int colorSeen = moveToColor();
	if (colorSeen == red) {
			moveForward(300);
			turnBack();
			DFS();
	}
	else if ( colorSeen == green) {
			moveForward(300);
			turnRight();
			int x = scanLine();
			DFS();
	}
}

task main()
{

	CreateEmpty(&S);

	int r,g,b;
	getColorRGB(colorSensor,r,g,b);
displayTextLine(1,"%d %d %d",r,g,b,);
	//int x = scanLine();
	initialMove();
	DFS();
}
