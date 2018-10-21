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

#ifndef LINKED_LIST_INCLUDE
#define LINKED_LIST_INCLUDE

#include<iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#define LEFT 1
#define RIGHT 2
#define UP 3
#define DOWN 4
#define MAX_WIDTH 400 // 26
#define MAX_HEIGHT 350 // 23

unsigned int food[MAX_HEIGHT][MAX_WIDTH];

GLsizei rsize = 15;
int px = 0;
int py = 0;

unsigned int points = 0;

using namespace std;

struct node {
  GLfloat x, y;
  GLfloat xstep, ystep;
  unsigned int direction;
  node *next;
};

class list {
private:
  node *head, *tail;
  
public:
  list() {
    head = NULL;
    tail = NULL;
  }

  void display_food() {
    int food_size = 10;

    for (int i = 0; i <  MAX_HEIGHT; i++) {
      for (int j = 0; j < MAX_WIDTH; j++) {
	if (food[i][j]) {
	 glColor3f(1.0f, 0.5f, 0.0f);
	  glBegin(GL_QUADS);
	  glVertex2i(i*16, j*16+food_size ); 
	  glVertex2i(i*16, j*16);
	  glVertex2i(i*16+food_size,j*16);
	  glVertex2i(i*16+food_size, j*16+food_size);               
	}
      } 
    }
  }

  void display_points(int x, int y, string String) {
    glRasterPos2i(x,y);
    for (int i=0; i<String.size(); i++) {
      glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, String[i]);
    }  
  }

  void createnode(GLfloat x, GLfloat y, GLfloat xstep, GLfloat ystep, unsigned int direction) {
    node *temp = new node;
    
    temp->x = x;
    temp->y = y;
    temp->xstep = xstep;
    temp->ystep = ystep;
    temp->direction = direction;
    temp->next=NULL;
    if (head==NULL) {
      head = temp;
      tail = temp;
      temp = NULL;
    }
    else {
      tail->next=temp;
      tail=temp;
    }
  }
  

  /* Se estiver na mesma posição da comida então come */
  void eat() {   
    if (food[px][py] == 1) {
       GLfloat x = head->x;
       GLfloat y = head->y;
       
       food[px][py] = 0;
       switch (head->direction) {
       case RIGHT:
	 x = x + rsize;
	 break;
       case LEFT:
	 x = x - rsize;
	 break;
       case UP:
	 y = y + rsize;
	 break;
       case DOWN:
	 y = y - rsize;
	 break;
       }
       insert_start(x, y, head->xstep, head->ystep, head->direction);
       points+=100;
     }
  }
    
  void display() {
    node *temp=new node;
    temp=head;
    while (temp != NULL) {
           glBegin(GL_QUADS);
	   glVertex2i(temp->x,temp->y+rsize); 
               glVertex2i(temp->x,temp->y);
               // Especifica que a cor corrente é azul
	       glColor3f(0.0f, 0.0f, 1.0f);
               glVertex2i(temp->x+rsize,temp->y);
               glVertex2i(temp->x+rsize,temp->y+rsize);               
      temp = temp->next;
    }
    display_food();
    glEnd();  
  }
  
  void move() {
    // Essa função faz a cauda tornar-se a cabeça da cobra. Isso simula a movimentação da cobra
    node *temp_head;
    node *temp_tail = new node;

    if (head->next == NULL) {
      temp_tail = head;
    }   
    temp_head=head;
    temp_tail->x = temp_head->x;
    temp_tail->y = temp_head->y;
    temp_tail->xstep = temp_head->xstep;
    temp_tail->ystep = temp_head->ystep;
    temp_tail->direction = temp_head->direction;

    switch (temp_tail->direction) {
    case RIGHT:
      temp_tail->x = temp_tail->x + temp_tail->xstep + rsize;
      px++;
      break;
    case LEFT:
      temp_tail->x = temp_tail->x + temp_tail->xstep - rsize;
      px--;
      break;
    case UP:
      temp_tail->y = temp_tail->y + temp_tail->ystep + rsize;
      py++;
      break;
    case DOWN:
      temp_tail->y = temp_tail->y + temp_tail->ystep - rsize;
      py--;
      break;
    }
    if (head->next != NULL) {
      delete_last();
      temp_tail->next = head;
      head = temp_tail;
    }
    eat();
  }

  void changeDirection(int direction) {
    head->direction = direction;
    switch (direction) {
    case DOWN:
      head->ystep = -1.0;
      break;
    case UP:
      head->ystep = 1.0;
      break;
    case LEFT:
      head->xstep = -1.0;
      break;
    case RIGHT:
      head->xstep = 1.0;
      break;
    }
  }

  int size() {
    node *temp = new node;
    temp = head;
    int counter = 0;

    while (temp != NULL) {
      counter += 1;
      temp = temp->next;
    }
    return counter;
  }

  // detect collision
  unsigned int collision() {
    if(head->x > MAX_WIDTH - rsize || head->x < 0) {
            return 1;
    }
    else if(head->y > MAX_HEIGHT+1 - rsize || head->y+1 < 0) {
          return 1;
    }      
    // Verifica as bordas.  Se a window for menor e o 
    // quadrado sair do volume de visualização 
    else if(head->x > MAX_WIDTH - rsize) {
      return 1;
    }

    else if(head->y > MAX_HEIGHT+1-rsize) {
      return 1;
    }
  
    else {
      node *body = head->next;

      while (body != NULL) {
	if (head->x == body->x && head->y == body->y) {
	  return 1;
	}
	body = body->next;
      }
      return 0;
    }
  }
  
  
  void insert_start(GLfloat x, GLfloat y, GLfloat xstep, GLfloat ystep, unsigned int direction) {
    node *temp=new node;

    temp->x = x;
    temp->y = y;
    temp->xstep = xstep;
    temp->ystep = ystep;
    temp->direction = direction;
    temp->next = head;
    head = temp;
    switch (direction) {
    case DOWN:
      py--;
      break;
    case UP:
      py++;
      break;
    case LEFT:
      px--;
      break;
    case RIGHT:
      px++;
      break;
    }   
      
  }

  void insert_position(int pos, GLfloat x, GLfloat y, GLfloat xstep, GLfloat ystep, unsigned int direction) {
    node *pre = new node;
    node *cur = new node;
    node *temp = new node;

    cur=head;
    for (int i = 1; i < pos; i++) {
      pre = cur;
      cur =cur->next;
    }
    temp->x = x;
    temp->y = y;
    temp->xstep = xstep;
    temp->ystep = ystep;
    temp->direction = direction;
    pre->next = temp;
    temp->next = cur;
  }

  void delete_first() {
    node *temp = new node;

    temp = head;
    head = head->next;
    delete temp;
  }

  void delete_last() {
    node *current;
    node *previous;
        
    current = head;
    while (current->next != NULL) {
      previous = current;
      current = current->next;
    }
    tail = previous;
    previous->next = NULL;
    //delete current;
  }

  void delete_position(int pos) {
    node *current = new node;
    node *previous = new node;

    current = head;
    for (int i = 1; i < pos; i++) {
      previous = current;
      current = current->next;
    }
    previous->next = current->next;
  }

  int getHeadDirection() {
    return head->direction;
  }
};


#endif
