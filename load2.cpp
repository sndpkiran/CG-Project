#include <stdio.h>
#include <iostream>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <cstring>
#include <fstream>
#include <unistd.h>
#include <math.h>
#include <stdbool.h>

#include "water-tex.h"

#define KEY_ESCAPE 27
#define POINTS_PER_VERTEX 3
#define TOTAL_FLOATS_IN_TRIANGLE 9

using namespace std;

float* normals;							// Stores the normals
float* Faces_Triangles;					// Stores the triangles
float* vertexBuffer;					// Stores the points which make the object
long TotalConnectedPoints;				// Stores the total number of connected verteces
long TotalConnectedTriangles;			// Stores the total number of connected triangles

float norm[3];
GLfloat fa=0,g=0;
int keyStates[256];

// set window values
	int winWidth = 512;
	int winHeight = 512;
	char *winTitle = "OpenGL/GLUT OBJ Loader.";
	int field_of_view_angle = 45;
	float winZ_near = 1.0f;
	float winZ_far = 500.0f;

float* calculateNormal( float *coord1, float *coord2, float *coord3 )
{
   /* calculate Vector1 and Vector2 */
   float va[3], vb[3], vr[3], val;
   va[0] = coord1[0] - coord2[0];
   va[1] = coord1[1] - coord2[1];
   va[2] = coord1[2] - coord2[2];
 
   vb[0] = coord1[0] - coord3[0];
   vb[1] = coord1[1] - coord3[1];
   vb[2] = coord1[2] - coord3[2];
 
   /* cross product */
   vr[0] = va[1] * vb[2] - vb[1] * va[2];
   vr[1] = vb[0] * va[2] - va[0] * vb[2];
   vr[2] = va[0] * vb[1] - vb[0] * va[1];
 
   /* normalization factor */
   val = sqrt( vr[0]*vr[0] + vr[1]*vr[1] + vr[2]*vr[2] );
 
	
	norm[0] = vr[0]/val;
	norm[1] = vr[1]/val;
	norm[2] = vr[2]/val;
 
 
	return norm;
}

 
 
int Load(char* filename)
{
	string line;
	ifstream objFile (filename);	
	if (objFile.is_open())													// If obj file is open, continue
	{
		objFile.seekg (0, ios::end);										// Go to end of the file, 
		long fileSize = objFile.tellg();									// get file size
		objFile.seekg (0, ios::beg);										// we'll use this to register memory for our 3d model
 
		vertexBuffer = (float*) malloc (fileSize);							// Allocate memory for the verteces
		Faces_Triangles = (float*) malloc(fileSize*sizeof(float));			// Allocate memory for the triangles
		normals  = (float*) malloc(fileSize*sizeof(float));					// Allocate memory for the normals
 
		int triangle_index = 0;												// Set triangle index to zero
		int normal_index = 0;												// Set normal index to zero
 
		while (! objFile.eof() )											// Start reading file data
		{		
			getline (objFile,line);											// Get line from file
 			cout<<line[0]<<endl;
			if (line.c_str()[0] == 'v')										// The first character is a v: on this line is a vertex stored.
			{
				line[0] = ' ';												// Set first character to 0. This will allow us to use sscanf
 
				sscanf(line.c_str(),"%f %f %f ",							// Read floats from the line: v X Y Z
					&vertexBuffer[TotalConnectedPoints],
					&vertexBuffer[TotalConnectedPoints+1], 
					&vertexBuffer[TotalConnectedPoints+2]);
 
				TotalConnectedPoints += POINTS_PER_VERTEX;					// Add 3 to the total connected points
			}
			if (line.c_str()[0] == 'f')										// The first character is an 'f': on this line is a point stored
			{
		    	line[0] = ' ';												// Set first character to 0. This will allow us to use sscanf
 
				int vertexNumber[4] = { 0, 0, 0 };
                sscanf(line.c_str(),"%i%i%i",								// Read integers from the line:  f 1 2 3
					&vertexNumber[0],										// First point of our triangle. This is an 
					&vertexNumber[1],										// pointer to our vertexBuffer list
					&vertexNumber[2] );										// each point represents an X,Y,Z.
 
				vertexNumber[0] -= 1;										// OBJ file starts counting from 1
				vertexNumber[1] -= 1;										// OBJ file starts counting from 1
				vertexNumber[2] -= 1;										// OBJ file starts counting from 1
 
 
				/********************************************************************
				 * Create triangles (f 1 2 3) from points: (v X Y Z) (v X Y Z) (v X Y Z). 
				 * The vertexBuffer contains all verteces
				 * The triangles will be created using the verteces we read previously
				 */
 
				int tCounter = 0;
				for (int i = 0; i < POINTS_PER_VERTEX; i++)					
				{
					Faces_Triangles[triangle_index + tCounter   ] = vertexBuffer[3*vertexNumber[i] ];
					Faces_Triangles[triangle_index + tCounter +1 ] = vertexBuffer[3*vertexNumber[i]+1 ];
					Faces_Triangles[triangle_index + tCounter +2 ] = vertexBuffer[3*vertexNumber[i]+2 ];
					tCounter += POINTS_PER_VERTEX;
				}
 
				/*********************************************************************
				 * Calculate all normals, used for lighting
				 */ 
				float coord1[3] = { Faces_Triangles[triangle_index], Faces_Triangles[triangle_index+1],Faces_Triangles[triangle_index+2]};
				float coord2[3] = {Faces_Triangles[triangle_index+3],Faces_Triangles[triangle_index+4],Faces_Triangles[triangle_index+5]};
				float coord3[3] = {Faces_Triangles[triangle_index+6],Faces_Triangles[triangle_index+7],Faces_Triangles[triangle_index+8]};
				float *norm = calculateNormal( coord1, coord2, coord3 );
 
				tCounter = 0;
				for (int i = 0; i < POINTS_PER_VERTEX; i++)
				{
					normals[normal_index + tCounter ] = norm[0];
					normals[normal_index + tCounter +1] = norm[1];
					normals[normal_index + tCounter +2] = norm[2];
					tCounter += POINTS_PER_VERTEX;
				}
 
				triangle_index += TOTAL_FLOATS_IN_TRIANGLE;
				normal_index += TOTAL_FLOATS_IN_TRIANGLE;
				TotalConnectedTriangles += TOTAL_FLOATS_IN_TRIANGLE;			
			}	
		}
		objFile.close();														// Close OBJ file
	}
	else 
	{
		cout << "Unable to open file";								
	}
	return 0;
}
 
void keyOperations()
{
	if(keyStates['a'])
		fa-=0.2;
	if(keyStates['d'])
		fa+=0.2;
	if(keyStates['j'])
		g-=0.2;
	if(keyStates['l'])
		g+=0.2;
}

void Draw() {
	glEnableClientState(GL_VERTEX_ARRAY);						// Enable vertex arrays
 	glEnableClientState(GL_NORMAL_ARRAY);						// Enable normal arrays
	glVertexPointer(3,GL_FLOAT,	0,Faces_Triangles);				// Vertex Pointer to triangle array
	glNormalPointer(GL_FLOAT, 0, normals);						// Normal pointer to normal array
	glDrawArrays(GL_TRIANGLES, 0, TotalConnectedTriangles);		// Draw the triangles
	glDisableClientState(GL_VERTEX_ARRAY);						// Disable vertex arrays
	glDisableClientState(GL_NORMAL_ARRAY);						// Disable normal arrays
}

void loadBackgroundImage(char *image) {
	char *imageData;
	FILE *file;
	int width = 1920;
	int height = 1080;
	file = fopen(image, "r");
	imageData = (char*) malloc(width * height * 24);
	int imageSize = width * height * 24;
	fread(imageData, width * height * 4, 1, file);
	fclose(file);
	 /*
	 * TGA is stored in BGR (Blue-Green-Red) format,
	 * we need to convert this to Red-Green-Blue (RGB).
	 * The following section does BGR to RGB conversion
	 */
	for (int i = 0; i < imageSize; i+=3) {
		// 24 bits per pixel   =  3 byte per pixel
		char c = imageData[i];
		imageData[i] = imageData[i+2];
		imageData[i+2] = c;
	}
	glRasterPos2i(0,0);
	glPixelStorei (GL_UNPACK_ROW_LENGTH, width);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, imageData);
}

void initialize () 
{
    glMatrixMode(GL_PROJECTION);
	glViewport(0, 0, winWidth, winHeight);
	GLfloat aspect = (GLfloat) winWidth / winHeight;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	// gluPerspective(45, 1, 1.0f, 500.0f);
	gluOrtho2D(0.0f, 512.0f, 0.0f, 512.0f);
    
    glMatrixMode(GL_MODELVIEW);
    glShadeModel( GL_SMOOTH );
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClearDepth( 1.0f );
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
 
    GLfloat amb_light[] = { 0.1, 0.1, 0.1, 1.0 };
    GLfloat diffuse[] = { 0.6, 0.6, 0.6, 1 };
    GLfloat specular[] = { 0.7, 0.7, 0.3, 1 };
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, amb_light );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuse );
    glLightfv( GL_LIGHT0, GL_SPECULAR, specular );
    glEnable( GL_LIGHT0 );
    glEnable( GL_COLOR_MATERIAL );
    glShadeModel( GL_SMOOTH );
    glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE );
    glDepthFunc( GL_LEQUAL );
    glEnable( GL_DEPTH_TEST );
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0); 
}

void display() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	keyOperations();
	// int i, j, tmp;
	// float tx, ty;
	// float texd = (float)1/WATERSIZE;		/* for texture mapping */

	// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// glPushMatrix();
	// glTranslatef(0, 0, spin_z-110);
	// glRotatef(spin_x, 0, 1, 0);
	// glRotatef(spin_y-60, 1, 0, 0);

	// calcwater();
	// glBegin(GL_TRIANGLES);
	// for(i = 0; i < WATERSIZE-1; i++) {
	// 	for(j = 0; j < WATERSIZE-1; j++) {
	// 		tx = (float)j/WATERSIZE;
	// 		ty = (float)i/WATERSIZE;
	// 		//texd = (float)1/WATERSIZE;

	// 		glTexCoord2f(tx, ty); 
	// 		glVertex3f(j-WATERSIZE/2, i-WATERSIZE/2, water[t][j][i]);
	// 		glTexCoord2f(tx+texd, ty); 
	// 		glVertex3f(j+1-WATERSIZE/2, i-WATERSIZE/2, water[t][j+1][i]);
	// 		glTexCoord2f(tx+texd, ty+texd); 
	// 		glVertex3f(j+1-WATERSIZE/2, i+1-WATERSIZE/2, water[t][j+1][i+1]);

	// 		glTexCoord2f(tx, ty+texd); 
	// 		glVertex3f(j-WATERSIZE/2, i+1-WATERSIZE/2, water[t][j][i+1]);
	// 		glTexCoord2f(tx, ty); 
	// 		glVertex3f(j-WATERSIZE/2, i-WATERSIZE/2, water[t][j][i]);
	// 		glTexCoord2f(tx+texd, ty+texd); 
	// 		glVertex3f(j+1-WATERSIZE/2, i+1-WATERSIZE/2, water[t][j+1][i+1]);

	// 	}
	// }
	// glEnd();
	// tmp = t; t = f; f = tmp;
	// glPopMatrix();


	glColor3f(0.0, 1.0, 0.0);

		glBegin(GL_QUADS);
			glVertex2f(0, 0);
			glVertex2f(200, 0);
			glVertex2f(200, 10);
			glVertex2f(0, 10);
		glEnd();
	// glPushMatrix();
	// 	gluLookAt(15, -52, 300, 0, 0, 0, 0, 0, 1);

	// 	glTranslatef(-150, -100, -10);
	// 	glBegin(GL_QUADS);
	// 		glVertex2f(0, 0);
	// 		glVertex2f(200, 0);
	// 		glVertex2f(200, 10);
	// 		glVertex2f(0, 10);
	// 	glEnd();
	// glPopMatrix();
	// glColor3f(1, 0, 0);
	glPushMatrix();
		// gluLookAt( 15,1,50, 0,0,0, 0,1,0);
		glRotatef(45,0,1,0);
		glRotatef(90,0,1,0);
		glTranslatef(0,-5,-5);
		glScalef(0.5,0.5,0.5);
		glTranslatef(0,0,g);
		// g_rotation++;
		Draw();
		
	glPopMatrix();
	// glColor3f(0.0, 1.0, 1.0);

	// glPushMatrix();
	// 	gluLookAt( 15,1,40, 0,0,0, 0,1,0);
	// 	glRotatef(45,0,1,0);
	// 	glRotatef(90,0,1,0);
	// 	glTranslatef(0,6,6);
	// 	glScalef(0.5,0.5,0.5);
	// 	glTranslatef(0,0,fa);
	// //	g_rotation++;
	// 	Draw();	
	// glPopMatrix();
	glutSwapBuffers();
	glFlush();

}





 
void keyboard ( unsigned char key, int x, int y ) 
{
  if(key == KEY_ESCAPE)
	exit(0);
  else
	keyStates[key] = true;
  //glutPostRedisplay();
}

void keyboardup ( unsigned char key, int x, int y )
{
	keyStates[key] = false;
}

/*void idle()
{
	water[f][WATERSIZE/2][WATERSIZE/2] = -50;
 	glutPostRedisplay();
}*/

int main(int argc, char **argv) 
{ 
	// initialize and run program
	glutInit(&argc, argv);                                      // GLUT initialization
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );  // Display Mode
	glutInitWindowSize(winWidth,winHeight);					// set window size
	glutCreateWindow(winTitle);								// create Window
	glutDisplayFunc(display);									// register Display Function
	glutIdleFunc(idle);									// register Idle Function
    glutKeyboardFunc( keyboard );
	glutKeyboardUpFunc( keyboardup );								// register Keyboard Handler
	// LoadTexture();
	initialize();
	Load("absship.obj");
	// Load("absship.obj");
	glutMainLoop();												// run GLUT mainloop
	return 0;
}