#pragma config(StandardModel, "EV3_REMBOT")
//	Pencarian Jalan Keluar Labirin menggunakan DFS
//	Tugas Besar Strategi Algoritma
//	8 Maret 2016

#define red 1
#define green 2
#define blue 3
#define bw 0
#define threshold 65


void moveForward(int degree){
	moveMotorTarget(leftMotor,degree,50);
	moveMotorTarget(rightMotor,degree,50);
	while ( getMotorMoving(leftMotor) || getMotorMoving(rightMotor) ) {
		sleep(1);
	}
}

void initialMove(){
	moveForward(700);
}

void turnBack(){
	resetGyro(gyroSensor);
	moveMotorTarget(leftMotor,447,100);
	moveMotorTarget(rightMotor,-447,100);
	while ( getMotorMoving(leftMotor) || getMotorMoving(rightMotor) ) {
		sleep(1);
	}
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

void steerLeft() {
		motor[leftMotor]  = 55;
  	motor[rightMotor] = 15;
}

void steerRight() {
		motor[leftMotor]  = 15;
  	motor[rightMotor] = 55;
}

void steerRightFixedAxis(){
	motor[leftMotor]  = -55;
 	motor[rightMotor] = 55;
}

void steerLeftFixedAxis() {
		motor[leftMotor]  = 55;
  	motor[rightMotor] = -55;
}


bool isSensorSeesLight() {
		return (getColorReflected(colorSensor) < threshold);
}

int detectColor(int r, int g , int b) {
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
			return bw;
	}
}


int moveToColor() {
	int r,g,b,colorSeen;
	while(true)
  {
    if (isSensorSeesLight()){
    		steerLeft();
    }
    else {
 		   	steerRight();
    }
    getColorRGB(colorSensor,r,g,b);
  	colorSeen = detectColor(r,g,b);
  	displayTextLine(0,"color seen : %d -> %d %d %d", colorSeen,r,g,b);
    if ( colorSeen != bw ) return colorSeen;
  }
}


int alignToLine(){
	int nowGyro = getGyroDegrees(gyroSensor);
	int deltaGyro = 0;
	int result = 1;

	while (!isSensorSeesLight() && (( nowGyro - getGyroDegrees(gyroSensor)) <= 120)) {
    	steerRightFixedAxis();
  }
  deltaGyro = nowGyro-getGyroDegrees(gyroSensor);
  displayTextLine(12,"%d",deltaGyro);

  //Kasus menemukan sudut tumpul
  if  ((deltaGyro >60) && (deltaGyro <122)) {
  		result = 2;
	}
	//Kasus tidak menemukan garis
  if ( !isSensorSeesLight()) {
  	while( nowGyro-getGyroDegrees(gyroSensor) >= 0) {
  			steerLeftFixedAxis();
  	}
  	result = 0;
	}

	return result;
}


void backToPreviousNode(){
			turnBack();
			moveToColor();
			moveForward(300);
}

int alignToBranch(int x){
		int result = x;
		int isLineDetected = 0;


		turnRight();
		moveForward(300);
		if (x==3) {
				return 4;
		}

		//Cari Branch terdekat hingga maksimal 3 branch
		isLineDetected = alignToLine();
		while ( !isLineDetected && (result != 3) ){
				moveForward(-300);
				turnLeft();
				moveForward(300);
				result++;
				isLineDetected = alignToLine();
		}
		result++;

		//Kasus khusus jika terdeteksi sudut tumpul
		if (isLineDetected == 2) {
				result = result * (-1);
		}
		return result;
}



void proceduralDFS() {
	int found = false;
	int depth = 0;
	int branch = 0;
	int pathChosen[50][2];
	int nodeFound;
	int i;

	while (!found) {
			//find a node
			depth++;
			displayTextLine(depth,"Finding for node..");
			nodeFound = moveToColor();

			//if got blue.. then success
			if (nodeFound == blue) {
					displayTextLine(depth,"Success",depth);
					turnBack();
					found = true;
			}

			//if got red, do backtrack
			else if (nodeFound == red) {
						displayTextLine(depth,"Failure..");
						depth--;

						//Cari Branch Selanjutnya
						backToPreviousNode();
						displayTextLine(depth+1,"");
						branch = alignToBranch(pathChosen[depth-1][0]);

						//Jika tak ada, backtrack ke node sebelumnya
						while (branch == 4) {
								depth--;
								displayTextLine(depth+1,"");
								moveToColor();
								moveForward(300);
								branch = alignToBranch(pathChosen[depth-1][0]);
						}

						//Pilih jalan
						displayTextLine(depth,"lv%d: Branching to %d",depth,branch);
						pathChosen[depth-1][0] = branch;
						if (branch <0) {
							pathChosen[depth-1][0] *= -1;
							pathChosen[depth-1][1] = 1;
						}
						else {
							pathChosen[depth-1][1] = 0;
						}

			}

			//if got green, prepare for deeper depth..
			else if (nodeFound == green){
					moveForward(300);
					branch = alignToBranch(0);
					displayTextLine(depth,"lv%d: Branching to %d",depth,branch);
					pathChosen[depth-1][0] = branch;
					if (branch <0) {
							pathChosen[depth-1][0] *= -1;
							pathChosen[depth-1][1] = 1;
					}
					else {
							pathChosen[depth-1][1] = 0;
					}
			}
	}

		//Tampilkan rute ditemukan
		depth--;
		for (i=0; i<depth ; i++) {
			displayTextLine(i+1,"path chosen -> %d",pathChosen[i][0]);
		}
		displayTextLine(i+9,"Mission Accomplished..");

		//Kembali pulang berdasarkan rute tercatat
		for (int i=depth-1 ; i>=0 ; i--) {
				moveToColor();
				moveForward(300);
				switch(pathChosen[i][0]) {
					case 3	:	turnRight();
										moveForward(200);
										break;
					case 2	:	if (pathChosen[i][1] == 1) turnRight();
										moveForward(400);
										break;
					case 1	:	turnLeft();
										moveForward(200);
										break;
				}
				moveToColor();
		}

}


task main()
{
		initialMove();
		proceduralDFS();
}
