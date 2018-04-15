/*

BY FCO. JAVIER CAMPOS (www.javiercampos.es)

Cuadro de diagonales (para la función ejecuta_tirada)


   n=0     n=1      n=2    n=3      n=4    n=5     n=6     n=7

-----------------------------------------------------------------
|       |       |       |       |       |       |       |       |
|       |       |       |       |       |       |       |       |     i=0
|       |       |       |       |       |       |       |       |
-----------------------------------------------------------------
|       |       |       |       |       |       |       |       |
|       |       |       |       |       |       |       |       |     i=1
|       |       |       |       |       |       |       |       |
-----------------------------------------------------------------
|       |Diago- |       |       |Vertical       |       |       |
|       |nal    |       |       | sup   |       |       |       |     i=2
|       |sup/izda       |       |       |       |       |       |
-----------------------------------------------------------------
|       |       | \     |       |       |       |Diago. |       |
|       |       |   \   |       |       |       |sup/   |       |     i=3
|       |       |     \ |       |       |       |dcha   |       |
-----------------------------------------------------------------
|       |       |       | \     |       |       |       |       |
|       |       |       |   \   |       |       |       |       |     i=4
|       |       |       |     \ |       |       |       |       |
-----------------------------------------------------------------
|       |Hori-  |       |       | Tiro  |       | Horiz.|       |
|       |zontal |       |       | aquí  |       | dcha  |       |     i=5
|       | izda  |       |       |       |       |       |       |
-----------------------------------------------------------------
|       |       |       |       |       |       |       |       | 
|       |       |       |       |       |       |       |       |     i=6
|       |       |       |       |       |       |       |       |
-----------------------------------------------------------------
|       |       |diago. |       |Vert.  |       |diago. |       |
|       |       |inf/   |       | inf   |       | inf/  |       |     i=7
|       |       |izda   |       |       |       | dcha  |       |
-----------------------------------------------------------------


*/




#include<stdio.h>
#include<windows.h>
#include<GL/glut.h>


#define WINX 1024
#define WINY 768

#define INVALIDO 1
#define NO_PC 2
#define NO_USER 3
#define WIN1 4
#define WIN2 5
#define EMPATE 6

#define FALSE 0
#define TRUE 1

#define TC 1 // tamaño del cuadrado

int matriz[8][8],ptos,imax,nmax;

int x_actual,z_actual;

char pierde_turno=FALSE; // es TRUE cuando al ultimo en tirar se le ha saltado


struct punto{
  int x,z;
};

/* almacena coordenadas de las fichas cambiadas tras una tirada */
/* nota: posicion 0 del vector es la nueva ficha tirada */
struct punto vcambiadas[64];
int ncambiadas; /* nº de fichas cambiadas en una tirada */


GLUquadricObj *cilindro;  // para dibujar ciclindros


  int i,n,ataca=2,no_ataca=1,cambiar,tiradai,tiradan,fin_juego=0,play=1,modo=0;
  int ia[8][8],conyazo=0,ganador=0;
  char name[1][24],name_control[50],simbolo[1];
  int numx,num0; /*almacenan el nº de casillas de cada jugador cada vez que se llama a la función 'pintala'*/

char primera_tirada=TRUE;
char msg=0;

// posicion de la luz
GLfloat light_position[] = {18.0,10.0,18.0,1.0}; 
GLfloat ambient_color[] = {0.4,0.4,0.4,1.0}; // ninguna componente igual a cero para que el objeto 
                                               //pueda ser de cualquier color (y sea visible)
GLfloat diffuse_color[] = {0.4,0.4,0.4,1.0};
GLfloat specular_color[] = {0.4,0.4,0.4,1.0};

void EscribirTexto (char *s,int tam){
  /* escribe en pantalla la cadena recibida como parámetro */

  int i;
  for (i=0; i<(int)strlen(s); i++){
    if(tam==12)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, s[i]);
    else if (tam==18)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, s[i]);
  }
}

void limpiar_pantalla(void) /*limpia la pantalla con 56 saltos de línea*/
{
  int i;
     for(i=0;i<56;i++) 
       printf("\n");
}



void pintala(int numx,int num0) //para pintar tablero según datos matriz
{
  int i,n;
  numx=0,num0=0;
  
  //limpiar_pantalla();
  printf("----------------------------------------------\n\n\n");
  
  printf("  0 1 2 3 4 5 6 7\n");

  for(i=0;i<8;i++) //dibuja tablero con 2 tipos de fichas y casillas vacías
    {
      printf("%d",i);
      for(n=0;n<8;n++)
	if(matriz[i][n]==1)
	  {
	    printf("|B");
	    numx++;
	  }
	else if(matriz[i][n]==2)
	  {
	    printf("|N");
	    num0++;
	  }
	else
	  printf("| ");
      printf("|\n");
    }

  printf("\n>>>Fichas Blancas (B):%d\n>>>Fichas Negras (N):%d\n",numx,num0);

}




void ejecuta_tirada(int ataca,int no_ataca,int tiradai,int tiradan,int simulacion,int conservar_ptos)
{ 
/*funcion que:

si simulacion = 0
     escribe en la matriz 'matriz' cómo queda el tablero después del último tiro.
si simulacion = 1
     comprueba que el jugador actual tiene opciones dónde tirar.

si conservar_ptos=0
     'ptos' se pone a 0 para que la variable simulacion tenga sentido y sirva para
     hacer lo que se dice arriba.
si conservar_ptos!=0
     'ptos' no se inicializa a 0 cada vez que se ejecuta esta función. Con esto se consigue saber si
     un jugador puede tirar, ya que si se ejecuta esta función mediante un bucle que dé todas
     casillas (i,n) sin que 'ptos' se ponga a 0, un valor ptos!=0al final de dicho bucle nos
     dice que el jugador puede tirar.
*/


 int i,n,cambiar_ficha=1;
 //int icambiadas=0;

 if(conservar_ptos==0) /*pone a 0 la variable ptos. (Esto no me interesa hacerlo cuando lo que*/
    ptos=0; /*quiero es recorrer todas las casillas y saber si aún puedo tirar en alguna parte;ptos!=0)*/
  
  if(matriz[tiradai][tiradan]==0){ /*si está vacía la casilla donde he tirado*/
    ncambiadas=0;
    vcambiadas[ncambiadas].x=tiradai;
    vcambiadas[ncambiadas].z=tiradan;
    ncambiadas++;

    if(tiradai>=2){ /*si tengo espacio de margen por arriba (2 o más casillas)*/
      if(matriz[tiradai-1][tiradan]==no_ataca){ /*para vertical sup.si la casilla inmediata en esa di-*/
	                                        /*rección es contraria a la mía*/
	for(i=tiradai-2;i>0 && matriz[i][tiradan]==no_ataca;i--)/*calculo cuántas fichas + en esa*/
	  cambiar_ficha+=1; /*dirección no son de las mías.El nºde fichas distintas a las mías desde*/
	                    /*donde tiré y en dirección vertical se almacena en 'cambiar_ficha'*/
	                    /*bucle termina cuando encuentra algo en esa direccion distinto a ficha no mía*/
	if(matriz[i][tiradan]==ataca){ /*si eso que no es una ficha-no-mía es una ficha mía*/
	  if(simulacion==0){ /*y esto no es una simulación*/
	    for(i=0;i<=cambiar_ficha;i++){ /*el conjunto de fichas entre la que tiré y la otra mía*/
	      vcambiadas[ncambiadas].x=tiradai-i;
	      vcambiadas[ncambiadas].z=tiradan;
	      ncambiadas++;
	      matriz[tiradai-i][tiradan]=ataca;/*que ha acabado el bucle pasan a ser todas mías*/
	    }
	     
	    cambiar_ficha=1;/*vuelvo a inicializar la variable que indica nº fichas a cambiar*/
	  }
	  ptos++; /*si llego aquí es porque puedo tirar*/
	}
	else cambiar_ficha=1; /*si la ficha que me sacó del bucle tampoco era mía.no hago nada.*/
      }                                                                       /*sólo inicializo*/
      if(tiradan>=2) /*si además tengo espacio de margen por la izda*/
	if(matriz[tiradai-1][tiradan-1]==no_ataca){ /*para diagonal sup. izda*/
	  for(i=tiradai-2,n=tiradan-2;i>0 && n>0 && matriz[i][n]==no_ataca;i--,n--)
	    cambiar_ficha+=1;
	  if(matriz[i][n]==ataca){
	    if(simulacion==0){
	      for(i=0,n=0;i<=cambiar_ficha;i++,n++){
		vcambiadas[ncambiadas].x=tiradai-i;
		vcambiadas[ncambiadas].z=tiradan-n;
		ncambiadas++;
		matriz[tiradai-i][tiradan-n]=ataca;
	      }
		
	      cambiar_ficha=1;
	    }		    
	    ptos++;
	  }
	  else cambiar_ficha=1;
	}

      if(tiradan<=5) /*si tengo espacio de margen por dcha (además de por arriba)*/
	if(matriz[tiradai-1][tiradan+1]==no_ataca){ /*para diagonal sup. dcha*/
	  for(i=tiradai-2,n=tiradan+2;i>0 && n<7 && matriz[i][n]==no_ataca;i--,n++)
	    cambiar_ficha+=1;
	  if(matriz[i][n]==ataca){
	    if(simulacion==0){		    
	      for(i=0,n=0;i<=cambiar_ficha;i++,n++){
		vcambiadas[ncambiadas].x=tiradai-i;
		vcambiadas[ncambiadas].z=tiradan+n;
		ncambiadas++;
		matriz[tiradai-i][tiradan+n]=ataca;
	      }
	      cambiar_ficha=1;
	    }
	    ptos++;
	  }
	  else cambiar_ficha=1;
	}
    }


    if(tiradai<=5){ /*si tengo espacio de margen por abajo*/
      if(matriz[tiradai+1][tiradan]==no_ataca){ /*para vertical inf*/
	for(i=tiradai+2;i<7 && matriz[i][tiradan]==no_ataca;i++)
	  cambiar_ficha+=1;
	if(matriz[i][tiradan]==ataca){
	  if(simulacion==0){
	    for(i=0;i<=cambiar_ficha;i++){
	      vcambiadas[ncambiadas].x=tiradai+i;
	      vcambiadas[ncambiadas].z=tiradan;
	      ncambiadas++;
	      matriz[tiradai+i][tiradan]=ataca;
	    }
	    cambiar_ficha=1;
	  }
	  ptos++;
	}
	else cambiar_ficha=1;
      }

      if(tiradan>=2) /*si tengo espacio de margen por izda (además de por abajo)*/
	if(matriz[tiradai+1][tiradan-1]==no_ataca){ /*para diagonal inf. izda*/
	  for(i=tiradai+2,n=tiradan-2;i<7 && n>0 &&matriz[i][n]==no_ataca;i++,n--)
	    cambiar_ficha+=1;
	  if(matriz[i][n]==ataca){
	    if(simulacion==0){
	      for(i=0,n=0;i<=cambiar_ficha;i++,n++){
		vcambiadas[ncambiadas].x=tiradai+i;
		vcambiadas[ncambiadas].z=tiradan-n;
		ncambiadas++;
	        matriz[tiradai+i][tiradan-n]=ataca;
	      }
	      cambiar_ficha=1;
	    }
	    ptos++;
	  }
	  else cambiar_ficha=1;
	}
	    
      if(tiradan<=5) /*para diagonal inf. dcha*/
	if(matriz[tiradai+1][tiradan+1]==no_ataca){
	  for(i=tiradai+2,n=tiradan+2;i<7 && n<7 && matriz[i][n]==no_ataca;i++,n++)
	    cambiar_ficha+=1;
	  if(matriz[i][n]==ataca){
	    if(simulacion==0){
	      for(i=0,n=0;i<=cambiar_ficha;i++,n++){
		vcambiadas[ncambiadas].x=tiradai+i;
		vcambiadas[ncambiadas].z=tiradan+n;
		ncambiadas++;
		matriz[tiradai+i][tiradan+n]=ataca;
	      }
	      cambiar_ficha=1;
	    }
	    ptos++;
	  }
	  else cambiar_ficha=1;
	}
    }

    if(tiradan>=2){ /*si tengo espacio de margen por izda (independientemente del valor 'tiradai')*/
      if(matriz[tiradai][tiradan-1]==no_ataca){ /*para horizontal izda*/
	for(n=tiradan-2;n>0 && matriz[tiradai][n]==no_ataca;n--)
	  cambiar_ficha+=1;
	if(matriz[tiradai][n]==ataca){
	  if(simulacion==0){
	    for(n=0;n<=cambiar_ficha;n++){
	      vcambiadas[ncambiadas].x=tiradai;
	      vcambiadas[ncambiadas].z=tiradan-n;
	      ncambiadas++;
	      matriz[tiradai][tiradan-n]=ataca;
	    }
	    cambiar_ficha=1;
	  }
	  ptos++;
	}
	else cambiar_ficha=1;
      }
    }

    if(tiradan<=5){ /*si tengo espacio de margen por dcha (da =valor de 'tiradai' )*/
      if(matriz[tiradai][tiradan+1]==no_ataca){ /*para horizontal dcha*/
	for(n=tiradan+2;n<7 && matriz[tiradai][n]==no_ataca;n++)
	  cambiar_ficha+=1;
	if(matriz[tiradai][n]==ataca){
	  if(simulacion==0){
	    for(n=0;n<=cambiar_ficha;n++){
	      vcambiadas[ncambiadas].x=tiradai;
	      vcambiadas[ncambiadas].z=tiradan+n;
	      ncambiadas++;
	      matriz[tiradai][tiradan+n]=ataca;
	    }
	    cambiar_ficha=1;
	  }
	  ptos++;
	}
	else cambiar_ficha=1;
      }
    }
  
  }
  
}


void jugada_modo(int ia[8][8],int ataca,int no_ataca,int modo)
{
/*función que indica cuál es la casilla donde debe tirar el jugador 'O'
cuando se ha seleccionado el modo '2' o el modo '3' (jugador contra máquina)

Modo '2' sólo tiene en cuenta a la hora de elegir la casilla cuál implica un
mayor número de fichas contrarias convertidas.

Modo '3' se decanta antes por colocar casillas suyas en las esquinas.

*/

  int i,n,valor_max=0,esquinas=0;

  for(i=0;i<8;i++)
    for(n=0;n<8;n++)
      {
	ejecuta_tirada(ataca,no_ataca,i,n,1,0);
	ia[i][n]=ptos; /*almacena número de fichas 'convertidas' según*/
	/* la casilla (i,n) donde se tirases ahora*/

	if (ia[i][n]>valor_max) /*almacena las coordenadas de posicion (i,n)*/
	  {                     /*que contienen el valor max hasta el momento*/
	    imax=i;
	    nmax=n;
	    valor_max=ia[i][n];
	  }
	else if(ia[i][n]==valor_max) /*si encuentra una casilla con el mismo valor*/
	  if (rand()%10>4)           /*que la casilla con valor max*/
	    {                        /*hago un RAND para que no siempre se quede con*/
	      imax=i;               /*la 1ª casilla con valor max*/
	      nmax=n;
	      valor_max=ia[i][n];
	    }
      }

  /*aquí empiezan las condiciones del modo 3*/
  if(modo==3)
    {
      if(ia[0][0]!=0)
	{          /*intenta tirar en una de ls 4 esquinas*/
	  imax=0;  /*si puede en más de una, en la que implique más fichas convertidas*/
	  nmax=0;
	  valor_max=ia[0][0];
	  esquinas=1;
	}
      if(ia[0][7]!=0)
	if(ia[0][7]>valor_max) 
	  {	               
	    imax=0;
	    nmax=7;
	    esquinas=1;
	  }
	else if(esquinas==0) /*si tirar en esta esquina no supone girar más fichas que 'valor_max'*/
	  {	             /*entonces si no he podido tirar en ninguna de las anterior(es) esquina(s)*/
	    imax=0;          /*tiro en esta, ya que puedo, pues ia[0][7]!=0*/
	    nmax=7;
	    esquinas=1;
	  }
      if(ia[7][0]!=0)
	if(ia[7][0]>valor_max)
	  {	 
	    imax=7;
	    nmax=0;
	    esquinas=1;
	  }
	else if(esquinas==0)
	  {	 
	    imax=7;
	    nmax=0;
	    esquinas=1;
	  }
      if(ia[7][7]!=0)
   	if(ia[7][7]>valor_max)
	  {	 
	    imax=7;
	    nmax=7;
	  }
	else if(esquinas==0)
	  {	 
	    imax=7;
	    nmax=7;
	    esquinas=1;
	  }
    }
}


void ejes(void){
  // muestra los ejes (1  2  3 .... 8    y    A   B   C.... H)

  char cad[2];
  int i;

  for(i=0;i<8;i++){ // numeros
    sprintf(cad,"%d",i+1);
    glRasterPos2f (0.4+i,0.1);
    EscribirTexto(cad,18);
  }

  for(i=0;i<8;i++){ // letras...
    sprintf(cad,"%c",'A'+i);
    glRasterPos3f (-0.2,0.1,0.5+i);
    EscribirTexto(cad,18);
  }
}

void mensajes(void){

  char cadena[200];

  glRasterPos3f (0.0,7.5,9.0);
  EscribirTexto("OTHELLO v.1.0             (BY FRACAMBE)",18);
  
  glRasterPos3f (0.0,7.0,9.0);
  EscribirTexto("UTILIZA LOS CURSORES PARA DESPLAZARTE POR EL TABLERO.",12);

  glRasterPos3f (0.0,6.5,9.0);
  EscribirTexto("PARA TIRAR, PULSA 'ESPACIO'.",12);

  glRasterPos3f (0.0,6.0,9.0);
  EscribirTexto("VUELVE A PULSAR 'ESPACIO' PARA QUE TIRE LA MAQUINA.",12);

  if(ataca==1 && msg!=NO_USER && msg!=WIN1 && msg!=WIN2 && msg!=EMPATE && pierde_turno==FALSE){ // ultim en tirar ha sido el usuario 1
    glRasterPos3f (0.0,5.0,9.0);
    sprintf(cadena,"HAS TIRADO EN: %c %d.",(char)('A'+tiradai),tiradan+1);
    EscribirTexto(cadena,18);
  }
  // ultim en tirar ha sido máquina y ha podido tirar..
  else if(primera_tirada==FALSE  && msg!=NO_PC && msg!=WIN1 && msg!=WIN2 && msg!=EMPATE && pierde_turno==FALSE){
    glRasterPos3f (0.0,5.0,9.0);
    //printf("DEBUG: MAQUINA HA TIRADO EN (%d,%d). AHORA ES TU TURNO.",tiradai,tiradan);

    sprintf(cadena,"MAQUINA HA TIRADO EN: %c %d.",(char)('A'+imax),nmax+1);
    EscribirTexto(cadena,18);
    
  }

    glRasterPos3f (0.0,4.0,9.0);
  switch(msg){

  case INVALIDO:
    EscribirTexto("EN ESA POSICION NO PUEDES TIRAR.",18);
    break;

  case NO_PC:
    EscribirTexto("LA MAQUINA NO PUEDE TIRAR. TE TOCA A TI..",18);
    break;

  case NO_USER:
    EscribirTexto("NO PUEDES TIRAR. PULSA 'ESPACIO' PARA SALTAR TU TURNO.",18);
    break;

  case WIN1:
    EscribirTexto("FIN DEL JUEGO. HAS GANADO.",18);
    break;

  case WIN2:
    EscribirTexto("FIN DEL JUEGO. GANA LA MAQUINA.",18);
    break;

  case EMPATE:
    EscribirTexto("FIN DEL JUEGO. EMPATE.",18);
    break;

  }

  glRasterPos3f (6.0,7.0,0.0);
  sprintf(cadena,"FICHAS BLANCAS: %d",numx);
  EscribirTexto(cadena,18);

  glRasterPos3f (6.0,6.7,0.0);
  sprintf(cadena,"FICHAS NEGRAS:  %d",num0);
  EscribirTexto(cadena,18);
  
}

void ficha2(void){

  glPushMatrix();
    glTranslatef(TC/2.0,0.1,TC/2.0);
    glRotatef(-90,1.0,0.0,0.0);
    gluCylinder(cilindro,0.4,0.4,0.1,16,1);
  glPopMatrix();
}

void ficha(void){

  glPushMatrix();
    glTranslatef(TC/2.0,0.1,TC/2.0);
    glScalef(1.0,0.2,1.0);
    glutSolidCube(0.6);
    //glRotatef(-90,1.0,0.0,0.0);
    //gluCylinder(cilindro,0.4,0.4,0.1,16,1);
  glPopMatrix();

}

void display(){

  int i,j;

  numx=0,num0=0; // contador num.fichas de cada.

  glClearColor (0.0, 0.0, 0.0, 0.0);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  GLfloat mat_vacia[]={0.0,0.0,0.0,1.0};
  GLfloat matR[]={1.0,0.0,0.0,1.0};
  GLfloat matG[]={0.0,1.0,0.0,1.0};
  GLfloat matB[]={0.0,0.0,1.0,1.0};

  GLfloat matBlanco[]={1.0,1.0,1.0,1.0};
  GLfloat matGris[]={0.3,0.3,0.3,1.0};

  
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,mat_vacia);
  glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,mat_vacia);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,mat_vacia);


  //glLineWidth(3.0);

  for(i=0;i<8;i++){
    for(j=0;j<8;j++){

      if((j+i)%2==0){
	//glColor3f (1.0,0.0,0.0);
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,matR);
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,matR); 
      }
      else{
	//glColor3f (0.0,0.0,1.0);
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,matB);
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,matB); 
      }

      glPushMatrix();
        glTranslatef(j*TC,0.0,i*TC);
        glBegin(GL_QUADS);
	  glNormal3f(0.0,1.0,0.0);
	  glVertex3f(TC,0.0,0.0);
	  glNormal3f(0.0,1.0,0.0);
	  glVertex3f(0.0,0.0,0.0);
	  glNormal3f(0.0,1.0,0.0);
	  glVertex3f(0.0,0.0,TC);
	  glNormal3f(0.0,1.0,0.0);
	  glVertex3f(TC,0.0,TC);
	glEnd();

	if(matriz[i][j]==1){ // ficha blanca
	  numx++;
	  //glColor3f (1.0,1.0,1.0);
	  glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,matBlanco);
	  glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,matBlanco); 
	  ficha();
	}
	else if(matriz[i][j]==2){ // ficha gris
	  num0++;
	  //glColor3f (0.3,0.3,0.3);
	  glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,matGris);
	  glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,matGris); 
	  ficha();
	}


      glPopMatrix();




    }
  }




  // casilla seleccionada (en verde)
  //glColor3f (0.0,1.0,0.0);
  glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,matG);
  glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,matG); 

  glPushMatrix();
    glTranslatef(x_actual*TC,0.01,z_actual*TC);
    glBegin(GL_QUADS);
      glVertex3f(TC,0.0,0.0);
      glVertex3f(0.0,0.0,0.0);
      glVertex3f(0.0,0.0,TC);
      glVertex3f(TC,0.0,TC);
    glEnd();
  glPopMatrix();

  glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,matBlanco);
  glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,matBlanco);   

  glColor3f (1.0,1.0,1.0);
  ejes(); // 1 2 3 4 ... A B C...

  mensajes();



  //printf(">>>Fichas X:%d\n>>>Fichas O:%d\n",numx,num0);
  
  glutSwapBuffers();
  
}



void keyboard_especial (int key, int x, int y){

  /* teclas especiales controladas */

  int i,j;

  //msg=0;

  // si habia tirado en una posicion invalida y me muevo, quito el cartel de invalido
  if(msg==INVALIDO)
    msg=0;

  switch(key){

  case GLUT_KEY_LEFT:
    for(j=z_actual;j>=0;j--){
      for(i=x_actual-1;i>=0;i--){
	if(matriz[j][i]==0){
	  x_actual=i;
	  z_actual=j;
	  glutPostRedisplay();
	  return;
	}
      }
      x_actual=8;

      if(j==0)
	j=8;
    }
    break;

  case GLUT_KEY_RIGHT:
    for(j=z_actual;j<8;j++){
      for(i=x_actual+1;i<8;i++){
	if(matriz[j][i]==0){
	  x_actual=i;
	  z_actual=j;
	  glutPostRedisplay();
	  return;
	}
      }
      x_actual=-1;
      if(j==7)
	j=-1;
    }    
    break;
    
  case GLUT_KEY_UP:
    for(i=z_actual-1;i>=0;i--)
      if(matriz[i][x_actual]==0){
	z_actual=i;
	break;
      }
    break;

  case GLUT_KEY_DOWN:
    for(i=z_actual+1;i<8;i++)
      if(matriz[i][x_actual]==0){
	z_actual=i;
	glutPostRedisplay();
	return;
      }
    break;

  }

  glutPostRedisplay();

}

void finjuego(void){
  // se ejecuta cuando el tablero se llena o cuando ninguno de los 2 puede tirar

  // para que no salga el mensaje del tiro anterior del usuario que no ha sido el último en tirar
  cambiar=ataca;         /*intercambia los valores de ataca y no_ataca*/
  ataca=no_ataca;
  no_ataca=cambiar;


  if (numx>num0){
    ganador=1;
    msg=WIN1;
  }
  else if(num0>numx){
    ganador=2;
    msg=WIN2;
  }
  
  if(ganador!=0){
    printf("Ha ganado %s\n",name[ganador-1]);
  }
  else{
    printf("%s y %s han quedado empatados.\n",name[0],name[1]);
    msg=EMPATE;
  }
  
  printf("\n\n\nBY FRACAMBE\n");
  //scanf("%d",&play); /*para 'frenar' el bucle,puedas leer esta información y salir*/
  play=0;
  ptos=-1;
}

void keyboard(unsigned char key, int x, int y){

  int qq;

  switch(key){
    
  case 27: // ESCAPE
    exit(0);
    break;
  
    /* SPACE */
  case 32:


    if(play){ // si no se ha acabado la partida...

    ptos=0;
    
    cambiar=ataca;         /*intercambia los valores de ataca y no_ataca*/
    ataca=no_ataca;
    no_ataca=cambiar;
    
    while(ptos==0){ /*mientras un jugador tire donde no puede*/
      
      imax=0; /*inicializa las coordenadas donde tirará PC*/
      nmax=0;
      
      if(numx+num0==64){ // si tablero está lleno...
	finjuego();
	glutPostRedisplay();
	return;
      }
      else{

	for(i=0;i<8;i++)  /*¿puedo/puede tirar en algun lado?*/
	  for(n=0;n<8;n++) /*si valor de 'ptos'!=0 al acabar este bucle,sí que puedo/puede*/
	    ejecuta_tirada(ataca,no_ataca,i,n,1,1); /*función ejecuta_tirada con*/
                                   /*simulación y sin inicializar la variable 'ptos' cada vez (ojo: la primera*/
                                   /*vez que entra [i=0 y n=0] si que se cumlpe  pto=0)*/

	/*lo siguiente escribe el nombre del atacante*/
	if (ataca==1) /*turno jugador X*/
	  printf("%s.\n",name[0]);
	else /*Turno jugador O*/
	  if(modo==1) /*si se juega en modo 1*/
	    printf("%s.\n",name[1]);
	
	
	if(ptos!=0){ /*Si puedo/puede tirar*/
	  if(modo!=1 && ataca==2){ /*si el PC es el jugador(O) y le toca atacar...*/
	    jugada_modo(ia,ataca,no_ataca,modo); /*busca la mejor jugada*/
	    ejecuta_tirada(ataca,no_ataca,imax,nmax,0,0); /*la ejecuta*/
	    fin_juego=0;/*indica que ha podido tirar*/
	    pintala(numx,num0);/*pinta el tablero*/
	    printf("%s (PC) ha tirado en la pos (%d,%d)\n\n",name[1],imax,nmax);
	    z_actual=imax;
	    x_actual=nmax;

	    pierde_turno=FALSE;

	    primera_tirada=FALSE;
	    msg=0;
	  }
	  
	  else{ /*si ataca el jugador(X) jugando en modo!=1 o si ataca alguien en modo=1*/
	    //printf("\nÍndice F/J:%d\nIntroduce tu próximo movimiento\n(fila 0~7) (espacio) (columna 0~7)\n",ptos);
	    //scanf("%d %d",&tiradai,&tiradan);
	    
	    tiradai=z_actual;
	    tiradan=x_actual;
	    
	    ejecuta_tirada(ataca,no_ataca,tiradai,tiradan,0,0);
	    fin_juego=0; /*indica que he podido tirar*/
	    pintala(numx,num0);
	    if((ptos!=0 && conyazo==2) || (ptos!=0 && modo)){ /*si he tirado en un sitio que podía, y estoy jugando en*/
	      printf("%s tiró en la posición (%d,%d)\n\n",name[ataca-1],tiradai,tiradan);/*modo lento o 2 jugadores,digo dónde*/
	      msg=0;
	      pierde_turno=FALSE;
	    }
	    else if (ptos==0){ /*si tiro donde no podía, doy un aviso*/
	      printf("Ahí no puedes tirar. Vuelve a intentarlo porque puedes.\n\n");
	      msg=INVALIDO;
	      // esto lo hago para que la próxima vez que pulse SPACE se siga con el mismo usuario
	      cambiar=ataca;         /*intercambia los valores de ataca y no_ataca*/
	      ataca=no_ataca;
	      no_ataca=cambiar;
	      glutPostRedisplay();
	      
	      return;
	      
	    }
	  }
	  
	} 
	else{ /*si ptos=0 como resultado de simulacion es porque no puedes tirar en ningún sitio*/
	  fin_juego++;
	  pierde_turno=TRUE;
	  
	  if (ataca==2){ /*muestra el nombre del jugador 2 si no puede tirar y estás jugando contra el pc*/
	    printf("%s.\n",name[1]);/*(en el caso de que no pueda tirar el jugador 1,su nombre ya aparece debido a la línea 470*/
	    msg=NO_PC;	      
	  }
	  else
	    msg=NO_USER;
	  
	  switch (fin_juego){
	    
	  case 1: /*si eljugador contrario pudo tirar en su turno, le toca a él de nuevo*/ 
	    printf("no puede tirar.\nEl turno pasa de jugador.\n");
	    //scanf("%d",&i); /*para 'frenar' el bucle y puedas leer esta información.Pongo 'i' por poner algo*/
	    pintala(numx,num0);
	    ptos=-1; 
	    break;
	    
	  case 2: /*si no puedes tirar tú ni el contrario, fin del juego*/
	    printf("tampoco puede tirar.\nFIN DEL JUEGO\n");
	  
	    finjuego();

	    break;
	  }
	  glutPostRedisplay();
	  return;
	}
      } // fin tablero no lleno

    }// fin mientras un jugador tira donde no puede

    } // fin if(play)
    else{
      exit(1);
    } 
  
    break;
  }

  glutPostRedisplay();
}

void reshape (int w, int h){

  /* Función de redimensionado durante la reproducción y la grabación */


  // define marco
  glViewport (0, 0, (GLsizei)w, (GLsizei)h); 
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  /* inicializa la proyección: */
  /* campo de visión, razón de aspecto, plano delantero y trasero */
  gluPerspective( 30 ,	(float)w/(float)h,1,100);
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  /* Situa la cámara: posición, hacia donde apunta, vector up */
  gluLookAt (18, 10, 18,0,0,0, 0,1,0);

}

void  inicializar_var_globales(){

  x_actual=0;
  z_actual=0;

  cilindro=gluNewQuadric(); // para crear cilindros alámbricos
  gluQuadricDrawStyle(cilindro,GLU_LINE);

  ncambiadas=0;

}

int main(int argc, char **argv){


  
  inicializar_var_globales();

  for(i=0;i<8;i++) /*inicializo matriz a '0'*/
    for(n=0;n<8;n++)
      matriz[i][n]=0;

  matriz[3][3]=1; /*relleno las 4 casillas iniciales*/
  matriz[4][4]=1;
  matriz[3][4]=2;
  matriz[4][3]=2;
    
 limpiar_pantalla();

 printf("OTHELLO V1.0  GRAFICO\t\t BY FRACAMBE");
 //scanf("%d",&i);

 sprintf(name[0],"Jugador (blancas)");
 sprintf(name[1],"Maquina (negras)");


 modo=3; 
 
 conyazo=1;
 
  glutInit(&argc, argv);
  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

  glutInitWindowSize (WINX,WINY); 
  glutInitWindowPosition (0, 0);
  glutCreateWindow ("Otero v.1.0     F.Javier Campos");
  
  glShadeModel (GL_SMOOTH);
  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST); // Z-buffer activado

  // posicion luz 0
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_color);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_color);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specular_color);
  
  glEnable(GL_LIGHT0); // luz 0 activada

  glutDisplayFunc(display); // escena inicial
  glutSpecialFunc(keyboard_especial); // teclas especiales (los cursores)
  glutKeyboardFunc(keyboard);
  //glutMouseFunc(raton);
  glutReshapeFunc(reshape);

  glutFullScreen();

  glutMainLoop();

  return 0;
}


