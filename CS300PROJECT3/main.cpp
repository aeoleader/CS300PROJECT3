/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name: Nan Jiang, Pratistha Bhandari, Xiangyu Li
 * Assignment: Project 3
 * Title: program title
 * Course: CS 300
 * Semester: Fall 2015
 * Instructor: D. Byrnes
 * Date: the current date
 * Sources consulted: any books consulted in writing the program
 * Program description: a description of what your program does
 * Known bugs: description of known bugs and other program imperfections
 * Creativity: anything extra you added to the program
 * Instructions: instructions to user on how to execute your program
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <cmath>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <cstdlib>
#include <iostream>
#include "tgaClass.h"

using namespace std;
GLUquadricObj *obj;

/***** Global variables *****/
const GLint SCREEN_WIDTH = 800;      // window dimensions
const GLint SCREEN_HEIGHT = 600;

const int MAX_TEXTURES = 6;

GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };  // no material property
GLfloat black[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat cyan[] = { 0.0, 1.0, 1.0, 1.0 };

GLfloat light_ambient[] = {0.0, 0.0, 0.0, 1.0}; // black ambient light
GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0}; // white diffuse light
GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};// white specular light
GLfloat light_pos[] = { 0.3, 0.2, 0.6, 0.0 };

// This is our camera rotation degree
GLfloat g_rotateX = 0, g_rotateY = 0;

GLfloat f_rotateY = 0;
bool f_turn = false;

GLfloat trans_y = 0;

// This holds the zoom value of our scope
GLfloat g_zoom = 120;

GLUquadricObj *qobj;         // Pointer for quadric objects.

// Settings for minion

// Rotation for arms
static GLfloat armVert = 0.0, strechX = 0.0, strechY = 0.0, armSpeed = 1.0;
bool maxWave = false, waveHand = false;
static GLint fastFlag = 0;

float runningAngle = 0;
bool running = false;
int flag = 1;
float speed = 2;
int dist = 0;
int face = 0;

// Bounding box
struct BoundingBox {
    int left;
    int right;
    int top;
    int bottom;
    int front;
    int back;
    
    BoundingBox(int _left=0, int _right=0, int _top=0, int _bottom=0, int _front=0, int _back=0): left(_left), right(_right), top(_top), bottom(_bottom), front(_front), back(_back) {}
};

//////////////////////////////////////////////////////////////////////////////////////////////////
/************************** Texture Mapping Operations ******************************************/
unsigned int g_Texture[MAX_TEXTURES] = {0};

float lastx, lasty;

float xrot = 0, yrot = -90, xpos = 0, ypos = 0, zpos = 118, angle = 0.0, rx = 0, ry = 0, rz = 0;
bool jumpping = false;
bool forwarding = false;
bool backwarding = false;
bool leftshift = false;
bool rightshift = false;

int counter = 0;
void CreateTexture(unsigned int textureArray[], char * strFileName, int textureID)
{
    
    char buffer[30];	//filename holder
    int status = 0;		//error codes for file read
    TGA myTGAImage;
    
    if(!strFileName){			// Return from the function if no file name was passed in
        
        cout <<"Error occurred, bad file name "<< endl;
        return;
    }
    
    sprintf (buffer, strFileName);		//load buffer with filename
    status = myTGAImage.readTGA(buffer);// Load into a TGA object
    if(status > 1) {
        cout <<"Error occurred = " << status <<strFileName<< endl;
        exit(0);
    }
    
    glGenTextures((GLsizei)1, (GLuint *)&textureArray[textureID]);
    
    glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
    
    glBindTexture(GL_TEXTURE_2D, textureArray[textureID]);
    
    // Now comes the important part, we actually pass in all the data from the bitmap to
    // create the texture. Here is what the parameters mean in gluBuild2DMipmaps():
    // (We want a 2D texture, 3 channels (RGB), bitmap width, bitmap height, It's an RGB format,
    //  the data is stored as unsigned bytes, and the actuall pixel data);
    
    // What is a Mip map?  Mip maps are a bunch of scaled pictures from the original.  This makes
    // it look better when we are near and farther away from the texture map.  It chooses the
    // best looking scaled size depending on where the camera is according to the texture map.
    // Otherwise, if we didn't use mip maps, it would scale the original UP and down which would
    // look not so good when we got far away or up close, it would look pixelated.
    
    // Build Mipmaps (builds different versions of the picture for distances - looks better)
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3,  myTGAImage.width,
                      myTGAImage.height, myTGAImage.format, GL_UNSIGNED_BYTE, myTGAImage.data);
    
    
    // Lastly, we need to tell OpenGL the quality of our texture map.  GL_LINEAR is the smoothest.
    // GL_NEAREST is faster than GL_LINEAR, but looks blochy and pixelated.  Good for slower computers though.
    // Read more about the MIN and MAG filters at the bottom of main.cpp
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
}
/************************** Texture Mapping Operations ******************************************/
//////////////////////////////////////////////////////////////////////////////////////////////////



// Initialize OpenGL graphics
void init(void)
{
    glMatrixMode(GL_PROJECTION);
    int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);
    glViewport(0, 0, width, height);
    gluPerspective(60.0,(GLfloat)width/(GLfloat)height, 0.1, 300);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glutSetCursor(GLUT_CURSOR_NONE);
    glutWarpPointer( SCREEN_WIDTH/2 , SCREEN_HEIGHT/2);
    CreateTexture(g_Texture, "Sky.tga", 0);			// Load our texture for the sky
    CreateTexture(g_Texture, "Float.tga", 1);		// Load our texture for the floating object
    CreateTexture(g_Texture, "Wood.tga", 2);		// Load our texture for the floating object
    
    
    glClearColor(1, 1, 1, 1);           // White background
    glShadeModel (GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);            // turn on the depth buffer
}

//////////////////////////////////////////////////////////////////////////////////////////////////
/***************************************** Draw Minion ******************************************/
// draw shoe tip
void drawShoeTip()
{
    int density = 250;
    glColor3f(0.0,0.0,0.0);
    glBegin(GL_POINTS);
    for (int i = -density; i < density; i++)
    {
        double buf = i * (1.5/density);
        double z = -buf*buf + 2.25;
        for (int j = 0; j< density; j++)
        {
            double x = j * (buf/density);
            double y = sqrt(buf*buf - x*x);
            glVertex3f(x, y, z);
        }
    }
    glEnd();
}

// draw shoe body
void drawShoeBody(float l, float w)
{
    glTranslatef(0, 0, -0.5*l);
    int density = 250;
    glColor3f(0.0,0.0,0.0);
    glBegin(GL_POINTS);
    for (int i = -density; i < density; i++)
    {
        for (int j = -density; j< density; j++)
        {
            double x = i * (w/density);
            double z = j * (0.5*l/density);
            double y = sqrt(2.25 - x*x);
            glVertex3f(x, y, z);
        }
    }
    glEnd();
}

// draw feet
void minionFeet()
{
    
    glPushMatrix();
    glRotatef(runningAngle, 1.0, 0.0, 0.0);
    //Right leg
    glPushMatrix();
    glTranslatef(3.0, -16.0, 0.0);
    glRotatef(90, 1.0, 0.0, 0.0);
    gluCylinder(qobj, 3, 2, 6, 30, 30);
    glPopMatrix();
    
    glPushMatrix();
    glColor3f(0.9, 0.5, 0.0);
    glTranslatef(3, -20, 0.0);
    glRotatef(90, 1.0, 0.0, 0.0);
    gluCylinder(qobj, 1.5, 1.4, 3.2, 30, 30);
    glPopMatrix();
    
    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    glTranslatef(3, -23.3, 0.0);
    glTranslatef(0, -1, 1.5);
    drawShoeTip();
    drawShoeBody(3, 2);
    glPopMatrix();
    glPopMatrix();
    
    //Left leg
    glPushMatrix();
    glRotatef(-runningAngle, 1.0, 0.0, 0.0);
    
    glPushMatrix();
    glTranslatef(-3.0, -16.0, 0.0);
    glColor3f(0.0,0.0,0.61);
    glRotatef(90, 1.0, 0.0, 0.0);
    gluCylinder(qobj, 3, 2, 6, 30, 30);
    glPopMatrix();
    
    glPushMatrix();
    glColor3f(0.9, 0.5, 0.0);
    glTranslatef(-3, -20, 0.0);
    glRotatef(90, 1.0, 0.0, 0.0);
    gluCylinder(qobj, 1.5, 1.4, 3.2, 30, 30);
    glPopMatrix();
    
    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    glTranslatef(-3, -23.3, 0.0);
    glTranslatef(0, -1, 1.5);
    drawShoeTip();
    drawShoeBody(3, 2);
    glPopMatrix();
    glPopMatrix();
    
}

// draw body
void minionBody()
{
    
    glPushMatrix();
    glColor3f(0.9,0.5,0.0);
    glRotatef(90.0,1.0,0.0,0.0);
    gluCylinder(qobj,8.0,8.0,12,50,50); //middle body
    glPopMatrix();
    
    glPushMatrix();
    glColor3f(0.9,0.5,0.0);
    glutSolidSphere(8.0,20,20); //head sphere
    glPopMatrix();
    
    glPushMatrix();
    glColor3f(0.0,0.0,0.61);
    glTranslatef(0.0,-12,0.0);
    glutSolidSphere(8.0,20,20); //bottom sphere
    glPopMatrix();
    
    //goggles strap
    glPushMatrix();
    glColor3f(0.36,0.25,0.20);
    glRotatef(90,1.0,0.0,0.0);
    glutSolidTorus (1.0,7.1,30,120);
    glPopMatrix();
    
    //right goggles
    glPushMatrix();
    glColor3f(0.90,0.91,0.98);
    glTranslatef(3.0,0.0,6.0);
    for (float i = 2.0; i < 3; i = i + 0.001)
        gluCylinder(qobj,i,i,2.5,50,50);
    glPopMatrix();
    
    //left goggles
    glPushMatrix();
    glColor3f(0.90,0.91,0.98);
    glTranslatef(-3.0,0.0,6.0);
    for (float i = 2.0; i < 3; i = i + 0.001)
        gluCylinder(qobj,i,i,2.5,50,50);
    glPopMatrix();
    
    //right eye
    glPushMatrix();
    glColor3f(1.0,1.0,1.0);
    glTranslatef(2.6,0.0,6.0);
    glutSolidSphere(2.5,20,20); //white eyeball
    glPopMatrix();
    
    glPushMatrix();
    glColor3f(0.35,0.16,0.14);
    glTranslatef(2.8,0.0,8.0);
    glutSolidSphere(0.9,20,20); //brown part
    glPopMatrix();
    
    glPushMatrix();
    glColor3f(0.0,0.0,0.0);
    glTranslatef(2.8,0.0,8.4);
    glutSolidSphere(0.6,20,20); //tiny black part
    glPopMatrix();
    
    //left eyeball
    glPushMatrix();
    glColor3f(1.0,1.0,1.0);
    glTranslatef(-2.6,0.0,6.0);
    glutSolidSphere(2.5,20,20); //white eyeball
    glPopMatrix();
    
    glPushMatrix();
    glColor3f(0.35,0.16,0.14);
    glTranslatef(-2.8,0.0,8.0);
    glutSolidSphere(0.9,20,20); //brown part
    glPopMatrix();
    
    glPushMatrix();
    glColor3f(0.0,0.0,0.0);
    glTranslatef(-2.8,0.0,8.4);
    glutSolidSphere(0.6,20,20); //tiny black part
    glPopMatrix();
    
    //smile
    glPushMatrix();
    glTranslatef(0.0, -5.5, 0.0);
    glPointSize(2.0);
    glColor3f(1.0,0.0,0.0);
    glBegin(GL_POINTS);
    for (int i = 0; i < 1000; i++)
    {
        double x = i*0.003;
        double y = 0.1*x*x;
        double z = sqrt(64 - x*x);
        glVertex3f(x, y, z);
        glVertex3f(-x, y, z);
    }
    glEnd();
    glPopMatrix();
    
    //clothing
    glPushMatrix();
    glColor3f(0.0,0.0,1.0);
    
    //middle part clothing
    glBegin(GL_POLYGON);
    glVertex3f(-1.2,-7.5,8.0);
    glVertex3f(-1.2,-12.0,8.0);
    glVertex3f(1.2,-12.0,8.0);
    glVertex3f(1.2,-7.5,8.0);
    glEnd();
    
    //grey patch
    glColor3f(1.0,1.0,1.0);
    glBegin(GL_POLYGON);
    glVertex3f(1.2,-8.5,8.1);
    glVertex3f(-1.2,-8.5,8.1);
    glVertex3f(-1.2,-10.5,8.1);
    glVertex3f(1.2,-10.5,8.1);
    glEnd();
    
    //left side clothing
    glColor3f(0.0,0.0,1.0);
    glBegin(GL_POLYGON);
    glVertex3f(-1.6,-7.5,7.9);
    glVertex3f(-1.6,-12.1,7.9);
    glVertex3f(-1.2,-12.1,7.9);
    glVertex3f(-1.2,-7.5,7.9);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-2.0,-7.5,7.8);
    glVertex3f(-2.0,-12.1,7.8);
    glVertex3f(-1.6,-12.1,7.9);
    glVertex3f(-1.6,-7.5,7.9);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-2.4,-7.5,7.7);
    glVertex3f(-2.4,-12.1,7.7);
    glVertex3f(-2.0,-12.1,7.8);
    glVertex3f(-2.0,-7.5,7.8);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-3.0,-7.5,7.6);
    glVertex3f(-3.0,-12.1,7.6);
    glVertex3f(-2.4,-12.1,7.7);
    glVertex3f(-2.4,-7.5,7.7);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-4.0,-7.5,7.2);
    glVertex3f(-4.0,-12.1,7.2);
    glVertex3f(-3.0,-12.1,7.5);
    glVertex3f(-3.0,-7.5,7.5);
    glEnd();
    
    //right side clothing
    glBegin(GL_POLYGON);
    glVertex3f(1.6,-7.5,7.9);
    glVertex3f(1.6,-12.1,7.9);
    glVertex3f(1.2,-12.1,7.9);
    glVertex3f(1.2,-7.5,7.9);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(2.0,-7.5,7.8);
    glVertex3f(2.0,-12.1,7.8);
    glVertex3f(1.6,-12.1,7.9);
    glVertex3f(1.6,-7.5,7.9);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(2.4,-7.5,7.7);
    glVertex3f(2.4,-12.1,7.7);
    glVertex3f(2.0,-12.1,7.8);
    glVertex3f(2.0,-7.5,7.8);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(3.0,-7.5,7.6);
    glVertex3f(3.0,-12.1,7.6);
    glVertex3f(2.4,-12.1,7.7);
    glVertex3f(2.4,-7.5,7.7);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(4.0,-7.5,7.2);
    glVertex3f(4.0,-12.1,7.2);
    glVertex3f(3.0,-12.1,7.5);
    glVertex3f(3.0,-7.5,7.5);
    glEnd();
    glPopMatrix();
    
    //shoulder straps
    glPushMatrix();
    glColor3f(0.0,0.0,1.0);
    glTranslatef(0.0,-7.5,0.0);
    glRotatef(90,1.0,0.0,0.0);
    glutSolidTorus (1.0,7.3,30,120);
    glPopMatrix();
}

// draw arm
void minionArm(bool isLeft, bool handUp)
{
    glColor3f(0.9,0.5,0.0);
    GLfloat radi = 1.5, fingerRadi = 0.7;
    
    int sign = isLeft ? 1 : -1; // left/right parameters offset
    
    // upper arm
    glPushMatrix();
    if (handUp && isLeft)
    {
        glRotatef(-180, 1.0, 0.0, 0.0);
        glTranslatef(strechX, strechY, 0.0);
        glRotatef(armVert, 0.0, 0.0, 1.0);
    }
    else if (handUp && !isLeft)
    {
        glRotatef(-180, 1.0, 0.0, 0.0);
        glTranslatef(-strechX, strechY, 0.0);
        glRotatef(-armVert, 0.0, 0.0, 1.0);
    }
    
    glPushMatrix();
    glRotatef(sign * -25, 0.0, 0.0, 1.0);
    glRotatef(-90, 1.0, 0.0, 0.0);
    gluCylinder(qobj, radi, radi, 4, 30, 5);
    gluSphere(qobj, radi, 30, 30);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(sign * -0.44, -1.8, 0.0);
    glRotatef(sign * -12, 0.0, 0.0, 1.0);
    glRotatef(-90, 1.0, 0.0, 0.0);
    gluCylinder(qobj, radi, radi, 2.0, 30, 5);
    gluSphere(qobj, radi, 30, 30);
    glPopMatrix();
    
    //fore arm
    glPushMatrix();
    glTranslatef(sign * -0.4, -3.0, 0.0);
    glRotatef(sign * 1, 0.0, 0.0, 1.0);
    glRotatef(-90, 1.0, 0.0, 0.0);
    gluCylinder(qobj, radi, radi, 2.0, 30, 5);
    gluSphere(qobj, radi, 30, 30);
    glPopMatrix();
    
    // glove (hand)
    glColor3f(0.0, 0.0, 0.0);
    glPushMatrix();
    glTranslatef(sign * -0.6, -5.2, 0.0);
    glRotatef(sign * 2, 0.0, 0.0, 1.0);
    glRotatef(-90, 1.0, 0.0, 0.0);
    gluCylinder(qobj, radi + 0.1, radi + 0.5, 1.6, 30, 5);
    gluSphere(qobj, radi + 0.1, 30, 30);
    glPopMatrix();
    
    // fingers
    glPushMatrix();
    glTranslatef(sign * -0.6, -7.0, 1.0);
    glRotatef(sign * 2, 0.0, 0.0, 1.0);
    glRotatef(-80, 1.0, 0.0, 0.0);
    gluCylinder(qobj, fingerRadi, fingerRadi, 1.4, 30, 5);
    gluSphere(qobj, fingerRadi, 30, 30);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(sign * 0.7, -6.6, 0.8);
    glRotatef(sign * 20, 0.0, 0.0, 1.0);
    glRotatef(-80, 1.0, 0.0, 0.0);
    gluCylinder(qobj, fingerRadi, fingerRadi, 1.4, 30, 5);
    gluSphere(qobj, fingerRadi, 30, 30);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(sign * -1.8, -6.6, 0.8);
    glRotatef(sign * -20, 0.0, 0.0, 1.0);
    glRotatef(-80, 1.0, 0.0, 0.0);
    gluCylinder(qobj, fingerRadi, fingerRadi, 1.4, 30, 5);
    gluSphere(qobj, fingerRadi, 30, 30);
    glPopMatrix();
    
    glPopMatrix();
}

void drawMinion(void)
{
    glPushMatrix();
    qobj = gluNewQuadric();
    glScalef(0.1, 0.1, 0.1);
    glTranslatef(0.0, 30.0, 0.0);
    glRotatef(180, 0.0, 1.0, 0.0);
    minionBody();
    minionFeet();
    
    // left arm
    glPushMatrix();
    if (waveHand)
    {
        glTranslatef(-8.0, -7.0, 0.0);
    }
    else
        glTranslatef(-7.8, -11.0, 0.0);
    minionArm(true, waveHand);
    glPopMatrix();
    
    // right arm
    glPushMatrix();
    if (waveHand && fastFlag)
    {
        glTranslatef(8.0, -7.0, 0.0);
        minionArm(false, waveHand);
    }
    else
    {
        glTranslatef(7.8, -11.0, 0.0);
        minionArm(false, false);
    }
    glPopMatrix();
    glPopMatrix();
}
/***************************************** Draw Minion ******************************************/
//////////////////////////////////////////////////////////////////////////////////////////////////

void drawBackground(void)
{
    // Draw 2D background
    glMatrixMode(GL_PROJECTION );
    glLoadIdentity();
    glOrtho(0,1,0,1,-1,1);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDepthMask(GL_FALSE);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,  g_Texture[0]);    // Sky texture
    
    // Display a 2D quad with the sky texture.
    glBegin(GL_QUADS);
    
    // Display the top left point of the 2D image
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(0, 0);
    
    // Display the bottom left point of the 2D image
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(0, 1);
    
    // Display the bottom right point of the 2D image
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(1, 1);
    
    // Display the top right point of the 2D image
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(1, 0);
    
    // Stop drawing
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void drawFloating(float mag)  // multiples of 3
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,  g_Texture[1]);    // Floating texture
    glBegin(GL_QUADS);                // Begin drawing the object with 6 quads
    
    for (int i = -mag; i < mag; i+=6.0)
    {
        // Top face (y = 0.5f)
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f( 3.0f, 0.5f, i);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-3.0f, 0.5f, i);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(-3.0f, 0.5f, i+6.0);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f( 3.0f, 0.5f, i+6.0);
        
        // Bottom face (y = -0.5f)
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f( 3.0f, -0.5f, i);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-3.0f, -0.5f, i);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(-3.0f, -0.5f, i+6.0);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f( 3.0f, -0.5f, i+6.0);
    }
    glEnd();
    
    //glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,  g_Texture[2]);    // Floating texture
    glBegin(GL_QUADS);
    // Front face  (z = 0.5f)
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f( 3.0f,  0.5f, mag);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-3.0f,  0.5f, mag);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-3.0f, -0.5f, mag);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f( 3.0f, -0.5f, mag);
    
    // Back face (z = -0.5f)
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f( 3.0f, -0.5f, -mag);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-3.0f, -0.5f, -mag);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-3.0f,  0.5f, -mag);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f( 3.0f,  0.5f, -mag);
    
    // Left face (x = -0.5f)
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-3.0f,  0.5f,  mag);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-3.0f,  0.5f, -mag);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-3.0f, -0.5f, -mag);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-3.0f, -0.5f,  mag);
    
    // Right face (x = 0.5f)
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(3.0f,  0.5f, -mag);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(3.0f,  0.5f,  mag);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(3.0f, -0.5f,  mag);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(3.0f, -0.5f, -mag);
    glEnd();  // End of drawing color-cube
    glColor3f(1.0f, 1.0f, 1.0f);
}
void camera()
{
    glRotatef(xrot, 1, 0, 0);
    glRotatef(yrot, 0, 1, 0);
    //gluLookAt(xpos, ypos, zpos, xrot, yrot, 0, 1, 1, 0);

    glTranslatef(-xpos, -ypos, -zpos);
}

void drawTower()
{
    obj = gluNewQuadric(); //creates a new quadric object
    
    glPushMatrix();
    glRotatef(90.0,1.0,0.0,0.0);
    glTranslatef(0.0,0.0,-5.0);
    glColor3f(0.858824, 0.858824, 0.439216);
    gluCylinder(obj,5.0,5.0,100,50,50);
    glColor3f(1.0, 1.0, 1.0);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0.0,10.0,0.0);
    glutSolidCube(10.0);
    glPopMatrix();
    
    glPushMatrix();
    glRotatef(-90.0,1.0,0.0,0.0);
    glTranslatef(0.0,0.0,15.0);
    glutSolidCone(4.0,9.0,17.0,10.0);
    //glutSolidSphere(5.0,10.0,10.0);
    glPopMatrix();

}

// display function
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawBackground();
    
    // Draw 3D Scene
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);
    glViewport(0, 0, width, height);
    gluPerspective(60.0,(GLfloat)width/(GLfloat)height, 0.1, 300);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    camera();
//    /******** Lighting Settings ********/
//    glEnable(GL_LIGHTING);
//    glEnable(GL_LIGHT0);
//    
//    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
//    glMaterialf(GL_FRONT, GL_SHININESS, 100);
//    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
//    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
//    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
//    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
//    
//    glEnable(GL_COLOR_MATERIAL);
//    /******** End Lighting ********/


    //    /******** Lighting Settings ********/
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
    //
        glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
        glMaterialf(GL_FRONT, GL_SHININESS, 100);
        glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
        glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    //
        glEnable(GL_COLOR_MATERIAL);
    //    /******** End Lighting ********/


    
    glPushMatrix();
    glTranslatef(0.0, -2.0, 90.0);
    drawFloating(3.0);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0.0, -2.0, 104.0);
    glRotatef(f_rotateY, 0.0, 1.0, 0.0);
    drawFloating(9.0);
    glPopMatrix();
    
//    glPushMatrix();
//    glTranslatef(0.0, -2.0, 118.0);
//    drawFloating(3.0);
//    drawMinion();
//    glPopMatrix();
//    
    glPushMatrix();
    glTranslatef(0.0,0.85,100.0);
    glutSolidTeapot(0.25);
    glTranslatef(0.0,0.0,5.0);
    glutSolidTeapot(0.25);
    glTranslatef(0.0,0.0,5.0);
    glutSolidTeapot(0.25);
    glTranslatef(0.0,0.0,5.0);
    glutSolidTeapot(0.25);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-40.0,0.0,50.0);
    drawTower();
    glPopMatrix();
    
    glPushMatrix();
    glColor3f(0.858824, 0.858824, 0.439216);
    glTranslatef(40.0,0.0,50.0);
    drawTower();
    glColor3f(1.0, 1.0, 1.0);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0.0, -2.0, 90.0);
    drawFloating(3.0);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0.0, -2.0, 104.0);
    glRotatef(f_rotateY, 0.0, 1.0, 0.0);
    drawFloating(9.0);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0.0, -2.0, 118.0);
    drawFloating(3.0);
    glPopMatrix();
    
    //glColor3f(1.0f, 1.0f, 1.0f);
    glutSwapBuffers();
}


void mouseMovement(int x, int y) {
    int diffx=x-lastx; //check the difference between the current x and the last x position
    int diffy=y-lasty; //check the difference between the y and the last y position
    lastx=x; //set lastx to the current x position
    lasty=y; //set lasty to the current y position
    if (xrot + (float) diffy <= 90 && xrot + (float) diffy >= -90) xrot += (float) diffy; //set the xrot to xrot with the addition of the difference in the y position
    yrot += (float) diffx;    //set the xrot to yrot with the addition of the difference in the x position
    glutPostRedisplay();
}

// keyboard callback function

void idle(void)
{
    if (f_rotateY >= 45)
        f_turn = !f_turn;
    else if (f_rotateY <= -45)
        f_turn = !f_turn;
    
    if (!f_turn)
        f_rotateY += 0.8;
    else
        f_rotateY -= 0.8;
    
    float rate = 0.2;
    float xrotrad, yrotrad;
    if (jumpping)
    {
        yrotrad = (yrot / 180 * 3.141592654f);
        xrotrad = (xrot / 180 * 3.141592654f);
        xpos += float(0.1*sin(yrotrad)) ;
        zpos -= float(0.1*cos(yrotrad)) ;
        
        if (counter < 20) ypos += rate;
        else if (counter < 39) ypos -= rate;
        else
        {jumpping = false; counter = 0;}
        counter ++;
    }
    
    if (forwarding)
    {
        yrotrad = (yrot / 180 * 3.141592654f);
        xrotrad = (xrot / 180 * 3.141592654f);
        xpos += float(0.1*sin(yrotrad));
        zpos -= float(0.1*cos(yrotrad));
    }
    
    if (backwarding)
    {
        yrotrad = (yrot / 180 * 3.141592654f);
        xrotrad = (xrot / 180 * 3.141592654f);
        xpos -= float(0.1*sin(yrotrad));
        zpos += float(0.1*cos(yrotrad));
    }
    
    if (leftshift)
    {
        yrotrad = (yrot / 180 * 3.141592654f);
        xpos -= float(0.1*cos(yrotrad));
        zpos -= float(0.1*sin(yrotrad));
    }
    
    if (rightshift)
    {
        yrotrad = (yrot / 180 * 3.141592654f);
        xpos += float(0.1*cos(yrotrad));
        zpos += float(0.1*sin(yrotrad));
    }

    glutPostRedisplay();
}

/*  key *** function
 *  esc  -  exit the application
 *  'w'  -  look up
 *  's'  -  look down
 */

void keyboard(unsigned char key, int x, int y)
{
    int w = glutGetWindow();
    float xrotrad, yrotrad;
    switch (key)
    {
        case 27:
            glutDestroyWindow(w);
            glDeleteTextures((GLsizei)MAX_TEXTURES, (GLuint *)g_Texture);
            exit(0);
            break;
        case 'w':
//            yrotrad = (yrot / 180 * 3.141592654f);
//            xrotrad = (xrot / 180 * 3.141592654f);
//            xpos += float(0.1*sin(yrotrad)) ;
//            zpos -= float(0.1*cos(yrotrad)) ;
//            ypos -= float(0.1*sin(xrotrad)) ;
            forwarding = true;
            break;
        case 's':
//            yrotrad = (yrot / 180 * 3.141592654f);
//            xrotrad = (xrot / 180 * 3.141592654f);
//            xpos -= float(0.1*sin(yrotrad));
//            zpos += float(0.1*cos(yrotrad)) ;
//            ypos += float(0.1*sin(xrotrad));
            backwarding = true;
            break;
        case 'd':
            rightshift = true;
            break;
        case 'a':
            leftshift = true;
            break;
        case 32:
            jumpping  = true;
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

void keyup(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'w':
            forwarding = false;
            break;
        case 's':
            backwarding = false;
            break;
        case 'a':
            leftshift = false;
            break;
        case 'd':
            rightshift = false;
            break;
        default:
            break;
    }
}

// arrow keys that are used to control the rotation of the object
void arrowkeys( int key, int x, int y )
{
    switch (key) {
        case GLUT_KEY_UP:							// If we hit the UP arrow key
            g_zoom -= 1;
            glutPostRedisplay();
            break;
        case GLUT_KEY_DOWN:							// If we hit the DOWN arrow key
            g_zoom += 1;
            glutPostRedisplay();
            break;
        case GLUT_KEY_LEFT:							// If we hit the LEFT arrow key
            g_rotateX -= 2;
            glutPostRedisplay();
            break;
        case GLUT_KEY_RIGHT:						// If we hit the RIGHT arrow key
            g_rotateX += 2;
            glutPostRedisplay();
            break;
        default:
            break;
    }
    glutPostRedisplay();
}


// main function
int main(int argc, char **argv)
{
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA );
    glutCreateWindow("Project 3 - I Have No Idea What It Is");  // window title
    glutFullScreen();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyup);
    glutSpecialFunc(arrowkeys);
    glutIdleFunc(idle);
    glutPassiveMotionFunc(mouseMovement);
    init();
    
    glutMainLoop();
}