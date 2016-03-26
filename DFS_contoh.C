

#pragma config(StandardModel, "EV3_REMBOT")
#define red 1
#define green 0
int searchSpot() {
//menelusuri garis hitam sampai menemukan "color"
	int hue,colortemp;
	int threshold = 65;
	resetGyro(gyroSensor);
	moveMotorTarget(leftMotor,200,100);
	moveMotorTarget(rightMotor,200,100);
	while(getMotorMoving(leftMotor)||getMotorMoving(rightMotor))
		sleep(1);
	while(1)
	{
		// sensor sees light:
		if(getColorReflected(colorSensor) < threshold) {
			// counter-steer right:
			motor[leftMotor]  = 55;
			motor[rightMotor] = 15;
		}
		else {// sensor sees dark:
			// counter-steer left:
			motor[leftMotor]  = 15;
			motor[rightMotor] = 55;
		}
		hue=getColorHue(colorSensor);
		//if(hue==99 || hue==253) {
		if(getColorName(colorSensor)==green) {
			colortemp=hue==85?green:red;
			break;
		}
	}
	return colortemp;
}

#define left 0
#define right 1

void turn(int dir,int deg,int lmot,int rmot) {
//berbelok ke arah "dir", sebesar "deg" derajat
	resetGyro(gyroSensor);
	setMotorSpeed(leftMotor,lmot);
	setMotorSpeed(rightMotor,rmot);
	while(getMotorMoving(leftMotor) || getMotorMoving(rightMotor)) {
		sleep(1);
		if((getGyroDegrees(gyroSensor)<deg && dir==left)
			||(getGyroDegrees(gyroSensor)>deg && dir==right))
		break;
	}
}

int cekLine(int dir,int deg,int lmot,int rmot) {
//mengembalikan 1 jika menemukan garis
	int threshold = 65;
	resetGyro(gyroSensor);
	setMotorSpeed(leftMotor,lmot);
	setMotorSpeed(rightMotor,rmot);
	while(getMotorMoving(leftMotor) || getMotorMoving(rightMotor)) {
		sleep(1);
		if(getColorReflected(colorSensor) < threshold)
			return 1;
		else if((getGyroDegrees(gyroSensor)<deg && dir==left)
			||(getGyroDegrees(gyroSensor)>deg && dir==right))
			break;
	}
	return 0;
}

void stepAhead(int degree) {
//maju degree derajat putaran roda
	moveMotorTarget(leftMotor,degree,50);
	moveMotorTarget(rightMotor,degree,50);
	while(getMotorMoving(leftMotor)||getMotorMoving(rightMotor)) {
		sleep(1);
	}
}

void cekHue(){
	int hue;
	motor[leftMotor] = 0;
	motor[rightMotor] = 0;
	while(1) {
  	displayTextLine(1,"Hue : %d",hue);
		hue=getColorHue(colorSensor);
	}
}

void DFS() {
	int found = 0;
	int color = searchSpot();
	if(color==green) {
		stepAhead(20);
		turn(right,20,30,-30);
		if(cekLine(right,70,30,-30)==1)
			turn(right,30,30,-30);
		else if(cekLine(left,-90,-30,30))
			turn(right,20,30,-30);
		else if(cekLine(left,-90,-40,30));
	}
	else if(color==red) {
		turn(left,-180,-40,30);
	}
	DFS();
}

task main() {
	//cekHue();
	DFS();
}