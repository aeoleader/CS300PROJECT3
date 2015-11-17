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
GLfloat rotateYtea = 0;
bool f_turn = false;

/////// @Pratshita /////////////
GLfloat trans_y = 0;
/////////////////////////////////

/////// @Xiangyu Li /////////////
float lastx, lasty;
float xrot = 0, yrot = -90, xpos = 0, ypos = 0, zpos = 118, angle = 0.0, rx = 0, ry = 0, rz = 0;
bool jumpping = false;
bool forwarding = false;
bool backwarding = false;
bool leftshift = false;
bool rightshift = false;
int counter = 0;
/////////////////////////////////

// This holds the zoom value of our scope
GLfloat g_zoom = 120;

GLUquadricObj *qobj;         // Pointer for quadric objects.

// Bounding box
struct BoundingBox {
    float left;
    float right;
    float top;
    float bottom;
    float front;
    float back;
    
    BoundingBox(float _left=0, float _right=0, float _top=0, float _bottom=0, float _front=0, float _back=0): left(_left), right(_right), top(_top), bottom(_bottom), front(_front), back(_back) {}
};

vector<BoundingBox> objects;
BoundingBox* buffer;

int which_object = 2;
//////////////////////////////////////////////////////////////////////////////////////////////////
/************************** Texture Mapping Operations ******************************************/
unsigned int g_Texture[MAX_TEXTURES] = {0};

float lastx, lasty;

float xrot = 0, yrot = -90, xpos = 0, ypos = 0, zpos = 130, angle = 0.0, rx = 0, ry = 0, rz = 0;
bool jumpping = false;
bool forwarding = false;
bool backwarding = false;
bool leftshift = false;
bool rightshift = false;
bool falling = false;

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
    
    CreateTexture(g_Texture, "Sky.tga", 0);			// Load our texture for the sky
    CreateTexture(g_Texture, "Float.tga", 1);		// Load our texture for the floating object
    CreateTexture(g_Texture, "Wood.tga", 2);		// Load our texture for the floating object
    
    //@Xiangyu Li
    glutSetCursor(GLUT_CURSOR_NONE);
    glutWarpPointer( SCREEN_WIDTH/2 , SCREEN_HEIGHT/2);
    
    glClearColor(1, 1, 1, 1);           // White background
    glShadeModel (GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);            // turn on the depth buffer
}

void drawBackground(void)
{
//    glEnable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D,  g_Texture[0]);    // Sky texture
    
    // Display a 2D quad with the sky texture.
    glColor3f(1.0, 0.0, 0.0);
    GLfloat width = 50.0;
    glBegin(GL_QUADS);
    // Top face (y = 10.0f)
    glVertex3f( width,  40.0f, -width);
    glVertex3f(-width,  40.0f, -width);
    glVertex3f(-width,  40.0f,  width);
    glVertex3f( width,  40.0f,  width);
    
    // Bottom face (y = -30.0f)
    glVertex3f( width, -50.0f, -width);
    glVertex3f(-width, -50.0f, -width);
    glVertex3f(-width, -50.0f,  width);
    glVertex3f( width, -50.0f,  width);
    
    // Front face (z = widthf)
    glVertex3f( width,  40.0f,  width);
    glVertex3f(-width,  40.0f,  width);
    glVertex3f(-width, -50.0f,  width);
    glVertex3f( width, -50.0f,  width);
    
    // Back face (z = -widthf)
    glVertex3f( width, -50.0f, -width);
    glVertex3f(-width, -50.0f, -width);
    glVertex3f(-width,  40.0f, -width);
    glVertex3f( width,  40.0f, -width);
    
    // Left face (x = -widthf)
    glVertex3f(-width,  40.0f,  width);
    glVertex3f(-width,  40.0f, -width);
    glVertex3f(-width, -50.0f, -width);
    glVertex3f(-width, -50.0f,  width);
    
    // Right face (x = width)
    glVertex3f( width,  40.0f, -width);
    glVertex3f( width,  40.0f,  width);
    glVertex3f( width, -50.0f,  width);
    glVertex3f( width, -50.0f, -width);
    // Stop drawing
    glEnd();
//    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0, 1.0, 1.0);
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

//creates tall towers
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
    glColor3f(0.2, 0.7, 0.6);
    glutSolidCube(10.0);
    glColor3f(1.0, 1.0, 1.0);
    glPopMatrix();
    
    glPushMatrix();
    glRotatef(-90.0,1.0,0.0,0.0);
    glTranslatef(0.0,0.0,15.0);
    glColor3f(0.2, 0.6, 0.7);
    glutSolidCone(4.0,9.0,17.0,10.0);
    glColor3f(1.0, 1.0, 1.0);
    glPopMatrix();

}

//creates floating teapots in the sky
void drawTeapots(void)
{
    glPushMatrix();
    glTranslatef(0.0,0.85,100.0);
    glColor3f(0.5, 0.0, 0.0);
    glRotatef(rotateYtea, 0.0, 1.0, 0.0);
    glutSolidTeapot(0.25);
    
    glColor3f(1.0, 1.0, 1.0);
    glTranslatef(0.0,0.0,5.0);
    glRotatef(rotateYtea, 0.0, 1.0, 0.0);
    glColor3f(0.5, 0.0, 0.0);
    glutSolidTeapot(0.25);
    
    glColor3f(1.0, 1.0, 1.0);
    glTranslatef(0.0,0.0,5.0);
    glRotatef(rotateYtea, 0.0, 1.0, 0.0);
    glColor3f(0.5, 0.0, 0.0);
    glutSolidTeapot(0.25);
    
    glColor3f(1.0, 1.0, 1.0);
    glTranslatef(0.0,0.0,5.0);
    glRotatef(rotateYtea, 0.0, 1.0, 0.0);
    glColor3f(0.5, 0.0, 0.0);
    glutSolidTeapot(0.25);
    
    glColor3f(1.0, 1.0, 1.0);
    glPopMatrix();

}

//creates obstacles on the floating blocks (cones)
void drawObstacle(void)
{
    glPushMatrix();
    glColor3f(0.0, 0.5, 0.0);
    glRotatef(-90,1.0,0.0,0.0);
    glutSolidCone(0.5,1.75,5.0,5.0);
    glColor3f(1.0, 1.0, 1.0);
    glPopMatrix();
}

// display function
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
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



    
    //obstacles on the last block
    glPushMatrix();
    glTranslatef(-1.85,-2.0,89.0);
    drawObstacle();
    glTranslatef(3.7,0.0,0.0);
    drawObstacle();
    glPopMatrix();
    
    //draw tower
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
    
    //draw floating objects
    glPushMatrix();
    glTranslatef(0.0, -2.0, 90.0);
    buffer = new BoundingBox(-3, 3, -1.5, -2.5, 87, 93);
    objects.push_back(*buffer);
    drawFloating(3.0);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0.0, -2.0, 104.0);
    //glRotatef(f_rotateY, 0.0, 1.0, 0.0);
    buffer = new BoundingBox(-3, 3, -1.5, -2.5, 95, 113);
    objects.push_back(*buffer);
    drawFloating(9.0);
    drawObstacle();
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0.0, -2.0, 130.0);
    buffer = new BoundingBox(-3, 3, -1.5, -2.5, 127, 133);
    objects.push_back(*buffer);
    drawFloating(3.0);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0.0, -2.0, 104.0);
    drawBackground();
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


void checkBounding(int x, int y)
{
    bool flag = false;
    for (int i = 0; i < objects.size();i++)
        if (x <= objects[i].right && x >= objects[i].left && y >= objects[i].front && y <= objects[i].back) flag = true;
    if (!flag) falling = true;
}
// keyboard callback function

void reset()
{
    xpos = 0;
    ypos = 0;
    zpos = 130;
    falling = false;
    xrot = 0;
    yrot = 0;
    glutPostRedisplay();
    
}
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
    
    rotateYtea += -0.25;
    
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
        if (!jumpping) checkBounding(xpos, zpos);
    }
    
    if (backwarding)
    {
        yrotrad = (yrot / 180 * 3.141592654f);
        xrotrad = (xrot / 180 * 3.141592654f);
        xpos -= float(0.1*sin(yrotrad));
        zpos += float(0.1*cos(yrotrad));
        checkBounding(xpos, zpos);
    }
    
    if (leftshift)
    {
        yrotrad = (yrot / 180 * 3.141592654f);
        xpos -= float(0.1*cos(yrotrad));
        zpos -= float(0.1*sin(yrotrad));
        checkBounding(xpos, zpos);
    }
    
    if (rightshift)
    {
        yrotrad = (yrot / 180 * 3.141592654f);
        xpos += float(0.1*cos(yrotrad));
        zpos += float(0.1*sin(yrotrad));
        checkBounding(xpos, zpos);
    }
    
    if (falling)
    {
        ypos -= 0.3;
        if (ypos < -20) reset();
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
            forwarding = true;
            break;
        case 's':
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
    glutIdleFunc(idle);
    glutPassiveMotionFunc(mouseMovement);
    init();
    
    glutMainLoop();
}
