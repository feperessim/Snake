/* Copyright (C) 2018 Felipe de Lima Peressim
 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>
#include <ctime> 
#include <string>
#include <sstream>
#include "linked_list.h"

using namespace std;


// Tamanho e posição inicial do quadrado
GLfloat x = 0.0f;
GLfloat y = 0.0f;

// Tamanho do incremento nas direções x e y 
// (número de pixels para se mover a cada
// intervalo de tempo)
GLfloat xstep = 1.0f;
GLfloat ystep = 1.0f;

// Largura e altura da janela
GLfloat windowWidth;
GLfloat windowHeight;
int counter = 0;
int direction = RIGHT;
int time_counter = 1;

list snake;

int win() {
  return snake.size() >= ((MAX_WIDTH / rsize) -1) * ((MAX_HEIGHT / rsize) - 1);
}

void gen_food() {
  int x, y;
  srand(time(0));
  //x= rand() % MAX_HEIGHT;
  //y= rand() % MAX_WIDTH;
  x= rand() % 26;
  y= rand() % 23;
    // Verifica as bordas.  Se a window for menor e o 
    // quadrado sair do volume de visualização 
  
  if(y > MAX_WIDTH-rsize)
         y = MAX_WIDTH-rsize-1;

   if(x > MAX_HEIGHT-rsize)
         x = MAX_HEIGHT-rsize-1;
  
  food[y][x] = 1;
}

// Função callback chamada para fazer o desenho
void draw(void)
{
     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();
                   
     // Limpa a janela de visualização com a cor de fundo especificada
     glClear(GL_COLOR_BUFFER_BIT);

     // Especifica que a cor corrente é vermelha
     //         R     G     B
     glColor3f(1.0f, 1.0f, 1.0f);
     // Desenha a cobra
     snake.display(); 
     
}

string points_to_string() {
  
  std::string out_string;
  std::stringstream ss;
  ss << points;
  out_string = ss.str();
  return out_string;
}

void printtext(int x, int y, string String) {
//(x,y) is from the bottom left of the window
  glRasterPos2i(x,y);
  for (int i=0; i<String.size(); i++) {
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, String[i]);
  }
  glutSwapBuffers();
}

// Função callback chamada pela GLUT a cada intervalo de tempo
// (a window não está sendo redimensionada ou movida)
void Timer(int value) // MOVE and testing whether snake died
{
  // chamar o move da snake
    // Muda a direção quando chega na borda esquerda ou direita
    if(x > windowWidth-rsize || x < 0)
            xstep = -xstep;
    
    // Muda a direção quando chega na borda superior ou inferior
    if(y > windowHeight-rsize || y < 0)
          ystep = -ystep;
          
    // Verifica as bordas.  Se a window for menor e o 
    // quadrado sair do volume de visualização 
   if(x > windowWidth-rsize)
         x = windowWidth-rsize-1;

   if(y > windowHeight-rsize)
         y = windowHeight-rsize-1;
  
    // Move o quadrado
   time_counter++;
   snake.move();

   if (snake.collision()) {
     printtext(MAX_WIDTH/2 - 50, MAX_HEIGHT/2  , "Game Over");
     return;
   }

   if (win()) {
       printtext(MAX_WIDTH/2 - 50, MAX_HEIGHT/2  , "Parabéns você ganhou");
       return;
   }
   
   if (time_counter % 10 == 0) {
     time_counter = 1;
     gen_food();
     // Gera a comida
   }
    // Redesenha o quadrado com as novas coordenadas   
   printtext(MAX_WIDTH - 90, MAX_HEIGHT-20  , points_to_string());
   glutPostRedisplay();
   glutTimerFunc(250, Timer, 1);
}

void keyboardSpecialKeys (int key,  int x, int y1) {
  int size = snake.size();
  int headDirection = snake.getHeadDirection();
    
  switch (key) {
  case GLUT_KEY_F1:
    break;
  case GLUT_KEY_LEFT:
    if (headDirection != RIGHT || size < 2) 
      snake.changeDirection(LEFT);
    break;
  case GLUT_KEY_RIGHT:
    if (headDirection != LEFT || size < 2) 
      snake.changeDirection(RIGHT);
    break;
  case GLUT_KEY_DOWN:
    if (headDirection != UP || size < 2) 
      snake.changeDirection(DOWN);
    break;
  case GLUT_KEY_UP:
    if (headDirection != DOWN || size < 2) 
      snake.changeDirection(UP);
    break; 
  default:
    break;
  }
  glutPostRedisplay();
  glutSpecialFunc(keyboardSpecialKeys);
}

// Inicializa parâmetros de rendering
void Inicializa (void)
{   
    // Define a cor de fundo da janela de visualização como preta
  glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
}

// Função callback chamada quando o tamanho da janela é alterado 
void changeWindowSize(GLsizei w, GLsizei h)
{
     // Evita a divisao por zero
     if(h == 0) h = 1;
                           
     // Especifica as dimensões da Viewport
     glViewport(0, 0, w, h);

     // Inicializa o sistema de coordenadas
     glMatrixMode(GL_PROJECTION);
     glLoadIdentity();

     // Estabelece a janela de seleção (left, right, bottom, top)     
     if (w <= h)  {
       windowHeight = 250.0f*h/w;
       windowWidth = 250.0f;
     }
     else  { 
       windowWidth = 250.0f*w/h;
       windowHeight = 250.0f;
     }
     gluOrtho2D(0.0f, MAX_WIDTH, 0.0f, MAX_HEIGHT);
}

void fill_food_matrix() {
  for (int i = 0; i < MAX_HEIGHT; i++) {
    for (int j = 0; j < MAX_WIDTH; j++) {
      food[i][j] = 0;
    } 
  }
}

// Programa Principal 
int main(int argc, char** argv)
{
  // Preenche a matriz comida
  fill_food_matrix();
  // Inicializa a cabeça da snake.
  snake.createnode(x, y, xstep, ystep, direction);
  glutInit(&argc,argv);            			// inicia glut
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(400, 350);
  glutInitWindowPosition(0,0);
  glutCreateWindow("Snake");
  glutDisplayFunc(draw);
  glutReshapeFunc(changeWindowSize);
  glutTimerFunc(33, Timer, 1);
  glutSpecialFunc(keyboardSpecialKeys);
  Inicializa();
  glutMainLoop();
}
