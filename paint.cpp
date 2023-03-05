/*
 * Computacao Grafica
 * Codigo Exemplo: Rasterizacao de Segmentos de Reta com GLUT/OpenGL
 * Autor: Prof. Laurindo de Sousa Britto Neto
 */

// Bibliotecas utilizadas pelo OpenGL
#ifdef __APPLE__
    #define GL_SILENCE_DEPRECATION
    #include <GLUT/glut.h>
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
#else
    #include <GL/glut.h>
    #include <GL/gl.h>
    #include <GL/glu.h>
#endif

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <forward_list>
#include "glut_text.h"
#include "algBresenham.h"
#include "drawCirculo.h"

using namespace std;

// Variaveis Globais
#define ESC 27

//Enumeracao com os tipos de formas geometricas
enum tipo_forma{LIN = 1, TRI, RET, POL, CIR }; // Linha, Triangulo, Retangulo Poligono, Circulo

//Verifica se foi realizado o primeiro clique do mouse
bool click1 = false;


//Coordenadas da posicao atual do mouse
int m_x, m_y,n_x,n_y;

//Vetor para guardar os pontos para desenho


//Coordenadas do primeiro clique e do segundo clique do mouse
int x_1, y_1, x_2, y_2;
int x_3,y_3;

//Indica o tipo de forma geometrica ativa para desenhar
int modo = RET;

//Largura e altura da janela
int width = 1024, height = 600;

int poli = 0;

float floodFillColor[] = {0, 0, 0};

bool floodFill_aux = false;

float cor1 = 0;
float cor2 = 0;
float cor3 = 0;


// Definicao de vertice
struct vertice{
    int x;
    int y;
};

// Definicao das formas geometricas
struct forma{
    int tipo;
    forward_list<vertice> v; //lista encadeada de vertices
};

// Lista encadeada de formas geometricas
forward_list<forma> formas;

// Funcao para armazenar uma forma geometrica na lista de formas
// Armazena sempre no inicio da lista
void pushForma(int tipo){
    forma f;
    f.tipo = tipo;
    formas.push_front(f);
}

// Funcao para armazenar um vertice na forma do inicio da lista de formas geometricas
// Armazena sempre no inicio da lista
void pushVertice(int x, int y){
    vertice v;
    v.x = x;
    v.y = y;
    formas.front().v.push_front(v);
}

//Fucao para armazenar uma Linha na lista de formas geometricas
void pushLinha(int x1, int y1, int x2, int y2){
    pushForma(LIN);
    pushVertice(x1, y1);
    pushVertice(x2, y2);
}

void pushRetangulo(int x1, int y1,int x2, int y2){
	pushForma(RET);
	pushVertice(x1,y1);
	pushVertice(x2,y1);
	pushVertice(x2,y2);
	pushVertice(x1,y2);
	
}

void pushTriangulo(int x1, int y1,int x2, int y2, int x3, int y3){
	pushForma(TRI);
	pushVertice(x1,y1);
	pushVertice(x2,y2);
	pushVertice(x3,y3);
}


/*
 * Declaracoes antecipadas (forward) das funcoes (assinaturas das funcoes)
 */
void init(void);
void reshape(int w, int h);
void display(void);
void menu_popup(int value);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void mousePassiveMotion(int x, int y);
void drawPixel(int x, int y);
// Funcao que percorre a lista de formas geometricas, desenhando-as na tela
void drawFormas();
// Funcao que implementa o Algoritmo Imediato para rasterizacao de segmentos de retas
//void retaImediata(double x1,double y1,double x2,double y2);
void reduceToFirstOctant(int x_1,int y_1,int x_2, int y_2);
void drawLineBresenham(int x_1,int y_1,int x_2, int y_2);
void transformFromFirstOctant (int x_1,int y_1,int x_2, int y_2);
void quadrilateros(int *a, int *b);
void triangulo(int *c, int *d);
void poligono(int *e,int *f,int n);
void translateShape(int delta_X,int delta_Y, int* x,int* y,int numPoints);
void circulo(int, int ,int);

/*
 * Funcao principal
 */
int main(int argc, char** argv){
    glutInit(&argc, argv); // Passagens de parametro C para o glut
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB); //Selecao do Modo do Display e do Sistema de cor
    glutInitWindowSize (width, height);  // Tamanho da janela do OpenGL
    glutInitWindowPosition (100, 100); //Posicao inicial da janela do OpenGL
    glutCreateWindow ("Computacao Grafica: Paint"); // Da nome para uma janela OpenGL
    init(); // Chama funcao init();
    glutReshapeFunc(reshape); //funcao callback para redesenhar a tela
    glutKeyboardFunc(keyboard); //funcao callback do teclado
    glutMouseFunc(mouse); //funcao callback do mouse
    glutPassiveMotionFunc(mousePassiveMotion); //fucao callback do movimento passivo do mouse
    glutDisplayFunc(display); //funcao callback de desenho
    
    // Define o menu pop-up
	glutCreateMenu(menu_popup);
	glutAddMenuEntry("Linha", LIN);
	glutAddMenuEntry("Retangulo", RET);
	glutAddMenuEntry("Triangulo", TRI);
	glutAddMenuEntry("Circulo", CIR);
	glutAddMenuEntry("Sair", 0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

    
    glutMainLoop(); // executa o loop do OpenGL
    return EXIT_SUCCESS; // retorna 0 para o tipo inteiro da funcao main();
}

/*
 * Inicializa alguns parametros do GLUT
 */
void init(void){
    glClearColor(1.0, 1.0, 1.0, 1.0); //Limpa a tela com a cor branca;
}

/*
 * Ajusta a projecao para o redesenho da janela
 */
void reshape(int w, int h)
{
	// Muda para o modo de projecao e reinicializa o sistema de coordenadas
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Definindo o Viewport para o tamanho da janela
	glViewport(0, 0, w, h);
	
	width = w;
	height = h;
    glOrtho (0, w, 0, h, -1 ,1);  

   // muda para o modo de desenho
	glMatrixMode(GL_MODELVIEW);
 	glLoadIdentity();

    click1 = true; //Para redesenhar os pixels selecionados
	
}

/*
 * Controla os desenhos na tela
 */
void display(void){
    glClear(GL_COLOR_BUFFER_BIT); //Limpa o buffer de cores e reinicia a matriz
    glColor3f (0.0, 0.0, 0.0); // Seleciona a cor default como preto
    drawFormas();
	// bresehanDraw(x_1, y_1 , x_2, y_2); // Desenha as formas geometricas da lista
    //Desenha texto com as coordenadas da posicao do mouse
    draw_text_stroke(0, 0, "(" + to_string(m_x) + "," + to_string(m_y) + ")", 0.2);
    glutSwapBuffers(); // manda o OpenGl renderizar as primitivas

}

/*
 * Controla o menu pop-up
 */
void menu_popup(int value){
    if (value == 0) exit(EXIT_SUCCESS);
    modo = value;
}


/*
 * Controle das teclas comuns do teclado
 */
void keyboard(unsigned char key, int x, int y){
    switch (key) { // key - variavel que possui valor ASCII da tecla precionada
        case ESC: exit(EXIT_SUCCESS); 
		break;
    }
    
}

/*
 * Controle dos botoes do mouse
 */


    
void mouse(int button, int state, int x, int y){
    switch (button) {
        case GLUT_LEFT_BUTTON:
            switch(modo){
                case LIN:
                    if(state == GLUT_DOWN) {
                        if(click1){
                            x_2 = x;
                            y_2 = height - y - 1;
                            printf("Clique 2(%d, %d)\n",x_2,y_2);
                            pushLinha(x_1, y_1, x_2, y_2);
                            click1 = false;
                            glutPostRedisplay();
                        }else{
                            click1 = true;
                            x_1 = x;
                            y_1 = height - y - 1;
                            printf("Clique 1(%d, %d)\n",x_1,y_1);
                        }
                    }
                break;
            	case TRI:
            		if(state ==GLUT_DOWN){
						if(click1){
							x_2 = x;
							y_2 = height - y - 1;
							pushTriangulo(x_1,y_1,x_2,y_2,x_3,y_3);
							click1 = false;
							glutPostRedisplay();
						}
						else{
							click1 = true;
							x_1 = x;
							y_1 = height - y -1;
							printf("Clique 1(%d, %d)\n",x_1,y_1);
						}
					}
              break;
  	          case RET:
            		if(state ==GLUT_DOWN){
						if(click1){
							x_2 = x;
							y_2 = height - y - 1;
							pushRetangulo(x_1,y_1,x_2,y_2);
							click1 = false;
							glutPostRedisplay();
						}
						else{
							click1 = true;
							x_1 = x;
							y_1 = height - y -1;
							printf("Clique 1(%d, %d)\n",x_1,y_1);
						}
					}
              break;
          	  case CIR:
            		if(state ==GLUT_DOWN){
						if(click1){
							x_2 = x;
							y_2 = height - y - 1;
							pontosCirculo(x_1,y_1,x_2,y_2);
							click1 = false;
							glutPostRedisplay();
						}
						else{
							click1 = true;
							x_1 = x;
							y_1 = height - y -1;
							printf("Clique 1(%d, %d)\n",x_1,y_1);
						}
					}
              break;
          	  
            }
        break;
    	// case GLUT_MIDDLE_BUTTON:
     //       if (state == GLUT_DOWN) {
     //           glutPostRedisplay();
     //       }
     //   break;

        case GLUT_RIGHT_BUTTON:
            if (state == GLUT_DOWN) {
                glutPostRedisplay();
            }
        break;
}    

       
            
    }

    

/*
 * Controle da posicao do cursor do mouse
 */
void mousePassiveMotion(int x, int y){
    m_x = x; m_y = height - y - 1;
    glutPostRedisplay();
}

/*
 * Funcao para desenhar apenas um pixel na tela
 */
void drawPixel(int x, int y){
    glBegin(GL_POINTS); // Seleciona a primitiva GL_POINTS para desenhar
        glVertex2i(x, y);
    glEnd();  // indica o fim do ponto
}

/*
 *Funcao que desenha a lista de formas geometricas
 */


void drawFormas(){
    //Apos o primeiro clique, desenha a reta com a posicao atual do mouse
    
    if(click1){
    	switch(modo){
			case LIN:
				break;
			case TRI:
				break;
			case RET:
				break;
			case CIR:
				double raio;
				raio = 100;
				circulo(x_1,y_1,raio);
				break;
			
		}
		 
	}
			
    
    //Percorre a lista de formas geometricas para desenhar
    for(forward_list<forma>::iterator f = formas.begin(); f != formas.end(); f++){
        switch (f->tipo) {
            case LIN:
            	printf("LINHA");
                 int i ,x[2], y[2];
				 i = 0;
                //Percorre a lista de vertices da forma linha para desenhar
                 for(forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, i++){
                     x[i] = v->x;
                     y[i] = v->y;
                 }
                //Desenha o segmento de reta apos dois cliques
                drawLineBresenham(x[0], y[0], x[1], y[1]);
  			break;
            
            case TRI:
				int l, c[3],d[3];
				l = 0;
				for(forward_list<vertice>::iterator v = f->v.begin(); v!= f->v.end(); v++, l++){
					c[l] = v->x;
					d[l] = v->y;
				 }
				triangulo(c,d);
				// drawLineBresenham(c[0],d[0],c[1],d[1]);
			 //    drawLineBresenham(c[1], d[1], c[2],d[2]);
			 //    drawLineBresenham(c[2],d[2], c[0],d[0]);
   	   	   break;
        
			case RET:
				printf("RETANGULO");
	    		 int j, a[4],b[4];
                 j = 0;
	    		 
				 for(forward_list<vertice>::iterator v = f->v.begin(); v!= f->v.end(); v++, j++){
					a[j] = v->x;
					b[j] = v->y;
				 }
				quadrilateros(a,b);
				// drawLineBresenham(a[0],b[0],a[1],b[1]);
			 //    drawLineBresenham(a[1], b[1], a[2],b[2]);
			 //    drawLineBresenham(a[2],b[2], a[3],b[3]);
			 //    drawLineBresenham(a[3],b[3], a[0],b[0]);
   			 break;
			case CIR:
				double raio;
				circulo(x_1,y_1,raio);
			break;
			    
				 
        }
        
     }
   }
 


void quadrilateros(int *a, int *b){
    
    
	drawLineBresenham(a[0],b[0],a[1],b[1]);
    drawLineBresenham(a[1], b[1], a[2],b[2]);
    drawLineBresenham(a[2],b[2], a[3],b[3]);
    drawLineBresenham(a[3],b[3], a[0],b[0]);
	
}

void triangulo(int *c, int *d){
	drawLineBresenham(c[0],d[0],c[1],d[1]);
	drawLineBresenham(c[1],d[1],c[2],d[2]);
	drawLineBresenham(c[2],d[2],c[0],d[0]);
	
}


void translacao (double *x, double *y, double *tx, double *ty){
	double matriz [3][3] = {
	{1,0,0},
	{0,1,0},
	{*tx,*ty,1}
	};
	double k[3] = {*x, *y, 1};
	double temp = 0, result[3];
	int i,j;
	
	for (i = 0;i<3;i++){
		temp = 0;
		for(j = 0;j<3;j++){
			temp += k[j] * matriz[j][i];
		}
		result[i] = temp;
	}
	*x = result[0];
	*y = result[1];
}

void escala (double *x, double *y, double *sx, double *sy){
	double matriz [3][3] = {
	{*sx,0,0},
	{0,*sy,0},
	{0,0,1}
};
	double k[3] = {*x, *y, 1};
	double temp = 0, result[3];
	int i,j;

	for (i = 0;i<3;i++){
		temp = 0;
		for(j = 0;j<3;j++){
			temp += k[j] * matriz[j][i];
		}
		result[i] = temp;
	}
	*x = result[0];
	*y = result[1];
}

void cisalhamento (double *x,double *y, double C, double direcao){
	double matriz[3][3];
	double k[3] = {*x,*y,1};
	double temp = 0,result[3];
	int i,j;
	
	if(direcao == 1){
		double matriz [3][3] = {
		{1,0,0},
		{C,1,0},
		{0,0,1}
		};
	}else if(direcao ==2){
		double matriz [3][3] = {
		{1,C,0},
		{0,1,0},
		{0,0,1}
		};
		
	}


	for (i = 0;i<3;i++){
		temp = 0;
		for(j = 0;j<3;j++){
			temp += k[j] * matriz[j][i];
		}
		result[i] = temp;
	}
	*x = result[0];
	*y = result[1];
}
void reflexao(double *x,double *y,int direcao){
	double matriz[3][3];
	double k[3] = {*x,*y,1};
	double temp = 0,result[3];
	int i,j;
	
	if(direcao == 1){
		double matriz [3][3] = {
		{1,0,0},
		{0,-1,0},
		{0,0,1}
		};
	}else if(direcao ==2){
		double matriz [3][3] = {
		{-1,0,0},
		{0,1,0},
		{0,0,1}
		};
		
	}else if(direcao == 3){
		double matriz [3][3] = {
		{-1,0,0},
		{0,-1,0},
		{0,0,1}
		};
	}
	
	for (i = 0;i<3;i++){
		temp = 0;
		for(j = 0;j<3;j++){
			temp += k[j] * matriz[j][i];
		}
		result[i] = temp;
	}
	*x = result[0];
	*y = result[1];
}

void rotacao(double *x, double *y, double grau){
	double k[3] = {*x, *y, 1};
	double cosseno, seno;
	cosseno = cos( grau * 3.14 / 180.0 );
	seno = sin(grau * 3.14 / 180.0);
	double matriz[3][3] = {
		{cosseno, seno, 0},
		{ -seno, cosseno, 0},
		{0, 0, 1}
	};
	int aux = 0, result[3];
	int i, j;

	for (i = 0; i < 3; i++)
	{
		aux = 0;
		for(j = 0; j < 3; j++)
		{
			aux += k[j] * matriz[j][i];
		}
		result[i] = aux;
	}

	*x = result[0];
	*y = result[1];
}

void floodFill(GLint x, GLint y, float* fillColor, float* bc)
{
	float color[3];
	glReadPixels(x, y, 1.0, 1.0, GL_RGB, GL_FLOAT, color);
	if((color[0] != bc[0] || color[1] != bc[1] || color[2] != bc[2]) && (
				color[0] != fillColor[0] || color[1] != fillColor[1] || color[2] != fillColor[2]))
	{
		glColor3f(fillColor[0], fillColor[1], fillColor[2]);
		cor1 = fillColor[0];
		cor2 = fillColor[1];
		cor3 = fillColor[2];
		glBegin(GL_POINTS);
		glVertex2i(x, y);
		glEnd();
		drawPixel(x,y);
		glutSwapBuffers();
		floodFill(x + 1, y, fillColor, bc);
		floodFill(x - 2, y, fillColor, bc);
		floodFill(x, y + 2, fillColor, bc);
		floodFill(x, y - 2, fillColor, bc);
	}
}






