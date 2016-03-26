#pragma config(StandardModel, "EV3_REMBOT")
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
	infotype T[20]; /* tabel penyimpan elemen */
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
	maxEl(*Q) = 10;
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
////////////////
//Definitions
//#define 2t1 (float)100/255
//#define 1t2 (float)255/100

//Configuration
#define color_number 7 //Sesuaikan
#define color_tolerance 20

int color_lib[color_number][3]={
//  R   G   B  //Skala 0-100. kalau 0-255, tinggal kalikan 2t1. misal {  0,200*2t1,255*2t1}
	{100,100,100},	//0 Putih_Arena
	{ 14, 14, 14},	//1 Hitam_Garis
	{  0,  0,100},	//2 Biru_Start
	{ 17, 87, 17},  //3 Ijo_Simpang
	{100, 16, 16},  //4 Merah_Ujung
	{100,  0,100},  //5 Pink_Buntu
	{  4, 72, 36}   //6 Ijo_Pudar
//	{   ,   ,   },  //Tambahin kalo perlu, dengan nama warna yang jelas.
	};

int check_color(){
	int res=-1; //if no color.
	int i=0;
	long red=0;
	long grn=0;
	long blu=0;
	getColorRGB(colorSensor,red,grn,blu);//Baca warna
  displayTextLine(1,"R: %d, G: %d, B: %d",red,grn,blu);
	while(res==-1 && i<color_number){
		if(red>color_lib[i][0]-color_tolerance && red<color_lib[i][0]+color_tolerance &&
			 grn>color_lib[i][1]-color_tolerance && grn<color_lib[i][1]+color_tolerance &&
			 blu>color_lib[i][2]-color_tolerance && blu<color_lib[i][2]+color_tolerance){
			 res=i;
			 break;
		}
		i++;
	}
	return res;
}

////////////////
#define red 4
#define green 3
int searchSpot() {
//menelusuri garis hitam sampai menemukan "color"
	int a;
	while(1)
	{
		a=check_color();
  	switch(a){
    case 0:	//Putih_Arena -> Belok Kanan
    			motor[leftMotor]  = 15;
      			motor[rightMotor] = 55;
      			break;
    case 1: //Hitam_Garis -> Belok Kiri
    			motor[leftMotor]  = 55;
      			motor[rightMotor] = 15;
      			break;
    case 3: //Ijo_Simpang -> diem
    				return a;
    case 4: //Merah_ujung -> Balik kanan
    				motor[leftMotor] = 100;
    				motor[rightMotor] = -100;
    				sleep(500);
      			break;

     default:
     				//motor[leftMotor]  = 0;
      			//motor[rightMotor] = 0;
    }
	}
	return a;
}

#define left 0
#define right 1
#define first -2
#define straight -1
void turn(int direction,int degree) {
//berbelok ke arah "direction", sebesar "degree"
	int rightpower=direction==right?-71:71;
	moveMotorTarget(leftMotor,degree,-rightpower);
	moveMotorTarget(rightMotor,-degree,rightpower);
	while(getMotorMoving(leftMotor) || getMotorMoving(rightMotor)) {
		sleep(1);
	}
}

void stepAhead(int degree) {
//maju degree derajat putaran roda
	moveMotorTarget(leftMotor,degree,50);
	moveMotorTarget(rightMotor,degree,50);
	while(getMotorMoving(leftMotor)||getMotorMoving(rightMotor)) {
		sleep(1);
	}
}

int checkLine(int dir) {
	int a,degmax=0;
	int incdegree=dir==right?10:-10;
	int rightpower=dir==right?-50:50;
	int rightdegree=dir==right?-10:10;
	while((degmax<2300&&dir==right) || (degmax>-2300&&dir==left)) {
		a=check_color();
		switch(a) {
			case 1:
				motor[leftMotor]=0;
				motor[rightMotor]=180;
				sleep(100);
				return 1;
			default:
 				moveMotorTarget(leftMotor,-rightdegree,-rightpower);
				moveMotorTarget(rightMotor,rightdegree,rightpower);
				while(getMotorMoving(leftMotor) && getMotorMoving(rightMotor))
					sleep(1);
				degmax+=incdegree;
		}
 	}
 	return 0;
}

void cekSimpang(Queue *Q) {
	displayTextLine(3,"cekSimpang running");
	if(!checkLine(right)) {
		Add(Q,right);
		turn(left,-31);	//A: this is so the color sensor
		//call checkLine function below outside the blackline
	}
	if(checkLine(left)) {
		Add(Q,straight);
		turn(right,31); //same as A
	}
	if(checkLine(left)) {
		Add(Q,left);
	}
	turn(right,270);
}

void BFS(int dirBack,Queue *Q) {
	int x,y,degree,color;
	if(IsEmpty(*Q)) {}
	else {
		Del(Q,&x);
		switch(dirBack) {
			case right:
				y=(x==straight)?right:straight;
				break;
			case straight:
				y=(x==right)?left:right;
				break;
			case left:
				y=(x==straight)?left:straight;
				break;
			default:
				y=x; //first
		}
		if(y!=straight) {
			displayTextLine(4,"masuk");
			checkLine(y);
		}
		color = searchSpot();
		if(color==green) {
			Add(Q,x);
			cekSimpang(Q);
		}
		turn(-1,-180);
		BFS(x,Q);
	}
}

task main()
{
/*	Queue Q;
	CreateEmpty(&Q);
	//turn(left,-213);
	int color = searchSpot();
	if(color==green) {
		stepAhead(220);
		cekSimpang(&Q);
	}
	BFS(first,&Q);
*/
	displayTextLine(1,"START");
	stepAhead(300);
	displayTextLine(1,"BEFORE");
	wait1Msec(3000);
	displayTextLine(1,"AFTER");
	turn(right, 270);
	displayTextLine(1,"END");
}
