/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name: Nan Jiang, Pratistha Bhandari, Xiangyu Li
 * Assignment: Project 3
 * Title: 3D world navigation
 * Course: CS 300
 * Semester: Fall 2015
 * Instructor: D. Byrnes
 * Date: 11/17/15
 * Sources consulted: Lecture notes, course book, and stack overflow.
 * Program description: this program creates a 3D world that the user can navigate through keyboard and mouse controls. this is a small game of jumping over floating blocks and if you fail to make the jump, you fall and the game restarts.
 * Instructions: Complete keyboard instructions on how to navigate the world and play the game are given in the read me file.
 'w', 's', 'a', 'd' move the camera forward, backward, left, and right respectively
 spacebar to jump
 'esc' key exits the application.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <cmath>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <cstdlib>
#include <iostream>
#include "tgaClass.h"
#include <vector>

using namespace std;
GLUquadricObj *obj; //creates a glu quadric object

/***** Global variables *****/
const GLint SCREEN_WIDTH = 800;      // window dimensions
const GLint SCREEN_HEIGHT = 600;

const int MAX_TEXTURES = 3;          // number of textures

GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };  // no material property
GLfloat black[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat cyan[] = { 0.0, 1.0, 1.0, 1.0 };

GLfloat light_ambient[] = {0.0, 0.0, 0.0, 1.0}; // black ambient light
GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0}; // white diffuse light
GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};// white specular light
GLfloat light_pos[] = { 0.0, 5.0, -3.0, 0.0 };

// This is our camera rotation degree
GLfloat g_rotateX = 0, g_rotateY = 0;

GLfloat f_rotateY = 0; //holds the rotation along y-axis for the block
GLfloat rotateYtea = 0; //holds rotation of teapots along y-axis
bool f_turn = false; //turning the camera

//movements of the first person navigation in the game
float lastx, lasty;

//camera rotation
float xrot = 0, yrot = 90;

//initial camera position
float xpos = 0, ypos = 0, zpos = 0, angle = 0.0, rx = 0, ry = 0, rz = 0;

bool jumpping = false;
bool forwarding = false;
bool backwarding = false;
bool leftshift = false;
bool rightshift = false;
bool falling = false;
bool collide = false;

int counter = 0;

// This holds the zoom value of our scope
GLfloat g_zoom = 50;

// Bounding box structure
struct BoundingBox {
    float left;
    float right;
    float top;
    float bottom;
    float front;
    float back;
    
    BoundingBox(float _left=0, float _right=0, float _top=0, float _bottom=0, float _front=0, float _back=0): left(_left), right(_right), top(_top), bottom(_bottom), front(_front), back(_back) {}
};

vector<BoundingBox> floors;         // store the bounding boxes for floating objects
vector<BoundingBox> obs;            // store the bounding boxes for obstacles
BoundingBox* buffer;                // bounding box pointer
float floatingCoordinate[20][3];    // store the coordinates for random generated floating objects

/************************** Texture Mapping Operations ******************************************/
unsigned int g_Texture[MAX_TEXTURES] = {0};

float lastx, lasty;

float xrot = 0, yrot = 90, xpos = 0, ypos = 0, zpos = 0, angle = 0.0, rx = 0, ry = 0, rz = 0;
bool jumpping = false;
bool forwarding = false;
bool backwarding = false;
bool leftshift = false;
bool rightshift = false;
bool falling = false;

int counter = 0;

float floatingCoordinate[20][3];
void CreateTexture(unsigned int textureArray[], char * strFileName, int textureID)
{
    
    char buffer[30];    //filename holder
    int status = 0;     //error codes for file read
    TGA myTGAImage;
    
    if(!strFileName){           // Return from the function if no file name was passed in
        
        cout <<"Error occurred, bad file name "<< endl;
        return;
    }
    
    sprintf (buffer, strFileName);      //load buffer with filename
    status = myTGAImage.readTGA(buffer);// Load into a TGA object
    if(status > 1) {
        cout <<"Error occurred = " << status <<strFileName<< endl;
        exit(0);
    }
    
    glGenTextures((GLsizei)1, (GLuint *)&textureArray[textureID]);
    
    glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
    
    glBindTexture(GL_TEXTURE_2D, textureArray[textureID]);
    
    // Build Mipmaps (builds different versions of the picture for distances - looks better)
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3,  myTGAImage.width,
                      myTGAImage.height, myTGAImage.format, GL_UNSIGNED_BYTE, myTGAImage.data);
    
    //quality of texture map
    //MIN and MAG are the filters
    //GL_LINEAR is the smoothest.
    // GL_NEAREST is faster than GL_LINEAR, but looks blochy and pixelated
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
}
/************************** Texture Mapping Operations ******************************************/

// Generate coordinates for random floating objects
void generateRandomFloatingCoordinate()
{
    int counter = 1;
    float x, y, z;
    bool flag;
    floatingCoordinate[0][0] = 0;
    floatingCoordinate[0][1] = -2;
    floatingCoordinate[0][2] = 0;
    while (counter < 20)
    {
        x = rand() % 80;
        z = rand() % 80;
        flag = false;
        for (int i = 0; i < counter; i++)
            if (sqrt((floatingCoordinate[i][0] - x)*(floatingCoordinate[i][0] - x)+(floatingCoordinate[i][2] - z)*(floatingCoordinate[i][2] - z)) <= 8.5)
            {
                flag = true;
                break;
            }
        if (!flag)
            for (int i = 0; i < counter; i++)
                if (sqrt((floatingCoordinate[i][0] - x)*(floatingCoordinate[i][0] - x)+(floatingCoordinate[i][2] - z)*(floatingCoordinate[i][2] - z)) <= 12)
                {
                    floatingCoordinate[counter][0] = x;
                    floatingCoordinate[counter][1] = -2;
                    floatingCoordinate[counter][2] = z;
                    counter++;
                    break;
                }
    }
}

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
    CreateTexture(g_Texture, "Sky.tga", 0);         // Load our texture for the sky
    CreateTexture(g_Texture, "Float.tga", 1);       // Load our texture for the floating object
    CreateTexture(g_Texture, "Wood.tga", 2);        // Load our texture for the floating object
    
    generateRandomFloatingCoordinate();
    glClearColor(1, 1, 1, 1);           // White background
    glShadeModel (GL_SMOOTH);           // OpenGL shade model is set to GL_SMOOTH
    glEnable(GL_DEPTH_TEST);            // turn on the depth buffer
}

//this function draws the background of the world using GL_QUADS
//the world is bounded by bounding box and the texture is applied to the top of the box, which is the sky
void drawBackground(float length)
{
    glEnable(GL_TEXTURE_2D); //enable texture
    glBindTexture(GL_TEXTURE_2D,  g_Texture[0]);    // Sky texture
    
    // Top face (y = length)
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f( length,  length, -length);
    glTexCoord2f(0.0f, 1.0f);   glVertex3f(-length,  length, -length);
    glTexCoord2f(1.0f, 1.0f);   glVertex3f(-length,  length,  length);
    glTexCoord2f(1.0f, 0.0f);   glVertex3f( length,  length,  length);
    
    // Bottom face (y = -length)
    glTexCoord2f(0.0f, 0.0f);   glVertex3f( length, -length, -length);
    glTexCoord2f(0.0f, 1.0f);   glVertex3f(-length, -length, -length);
    glTexCoord2f(1.0f, 1.0f);   glVertex3f(-length, -length,  length);
    glTexCoord2f(1.0f, 0.0f);   glVertex3f( length, -length,  length);
    
    // Front face (z = length)
    glTexCoord2f(0.0f, 0.0f);   glVertex3f( length,  length,  length);
    glTexCoord2f(0.0f, 1.0f);   glVertex3f(-length,  length,  length);
    glTexCoord2f(1.0f, 1.0f);   glVertex3f(-length, -length,  length);
    glTexCoord2f(1.0f, 0.0f);   glVertex3f( length, -length,  length);
    
    // Back face (z = -length)
    glTexCoord2f(0.0f, 0.0f);   glVertex3f( length, -length, -length);
    glTexCoord2f(0.0f, 1.0f);   glVertex3f(-length, -length, -length);
    glTexCoord2f(1.0f, 1.0f);   glVertex3f(-length,  length, -length);
    glTexCoord2f(1.0f, 0.0f);   glVertex3f( length,  length, -length);
    
    // Left face (x = -length)
    glTexCoord2f(1.0f, 0.0f);   glVertex3f(-length,  length,  length);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-length,  length, -length);
    glTexCoord2f(0.0f, 1.0f);   glVertex3f(-length, -length, -length);
    glTexCoord2f(1.0f, 1.0f);   glVertex3f(-length, -length,  length);
    
    // Right face (x = length)
    glTexCoord2f(0.0f, 1.0f);   glVertex3f( length,  length, -length);
    glTexCoord2f(1.0f, 1.0f);   glVertex3f( length,  length,  length);
    glTexCoord2f(1.0f, 0.0f);   glVertex3f( length, -length,  length);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f( length, -length, -length);
    // Stop drawing
    glEnd();
    
    glDisable(GL_TEXTURE_2D); //texture is disabled
}

//this function draws floating blocks in the scene
//takes one parameter "mag" which the length of the block and accepts multiples of 3
//applies texture of the floating blocks
void drawFloating(float mag)  // multiples of 3
{
    glEnable(GL_TEXTURE_2D); //enable texture
    glBindTexture(GL_TEXTURE_2D,  g_Texture[1]);    // Floating texture
    glBegin(GL_QUADS);                // Begin drawing the object with 6 quads
    // Top face (y = 0.5f)
    glTexCoord2f(0.0f, 0.0f);   glVertex3f( 3.0f, 0.5f, -mag);
    glTexCoord2f(0.0f, 1.0f);   glVertex3f(-3.0f, 0.5f, -mag);
    glTexCoord2f(1.0f, 1.0f);   glVertex3f(-3.0f, 0.5f,  mag);
    glTexCoord2f(1.0f, 0.0f);   glVertex3f( 3.0f, 0.5f,  mag);
    
    // Bottom face (y = -0.5f)
    glTexCoord2f(0.0f, 0.0f);   glVertex3f( 3.0f, -0.5f, -mag);
    glTexCoord2f(0.0f, 1.0f);   glVertex3f(-3.0f, -0.5f, -mag);
    glTexCoord2f(1.0f, 1.0f);   glVertex3f(-3.0f, -0.5f,  mag);
    glTexCoord2f(1.0f, 0.0f);   glVertex3f( 3.0f, -0.5f,  mag);
    glEnd();
    
    glBindTexture(GL_TEXTURE_2D,  g_Texture[2]);    // Floating texture
    glBegin(GL_QUADS);
    // Front face  (z = 0.5f)
    glTexCoord2f(0.0f, 0.0f);   glVertex3f( 3.0f,  0.5f, mag);
    glTexCoord2f(0.0f, 1.0f);   glVertex3f(-3.0f,  0.5f, mag);
    glTexCoord2f(1.0f, 1.0f);   glVertex3f(-3.0f, -0.5f, mag);
    glTexCoord2f(1.0f, 0.0f);   glVertex3f( 3.0f, -0.5f, mag);
    
    // Back face (z = -0.5f)
    glTexCoord2f(0.0f, 0.0f);   glVertex3f( 3.0f, -0.5f, -mag);
    glTexCoord2f(0.0f, 1.0f);   glVertex3f(-3.0f, -0.5f, -mag);
    glTexCoord2f(1.0f, 1.0f);   glVertex3f(-3.0f,  0.5f, -mag);
    glTexCoord2f(1.0f, 0.0f);   glVertex3f( 3.0f,  0.5f, -mag);
    
    // Left face (x = -0.5f)
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-3.0f,  0.5f,  mag);
    glTexCoord2f(0.0f, 1.0f);   glVertex3f(-3.0f,  0.5f, -mag);
    glTexCoord2f(1.0f, 1.0f);   glVertex3f(-3.0f, -0.5f, -mag);
    glTexCoord2f(1.0f, 0.0f);   glVertex3f(-3.0f, -0.5f,  mag);
    
    // Right face (x = 0.5f)
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(3.0f,  0.5f, -mag);
    glTexCoord2f(0.0f, 1.0f);   glVertex3f(3.0f,  0.5f,  mag);
    glTexCoord2f(1.0f, 1.0f);   glVertex3f(3.0f, -0.5f,  mag);
    glTexCoord2f(1.0f, 0.0f);   glVertex3f(3.0f, -0.5f, -mag);
    glEnd();  // End of drawing color-cube
    
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
}

//this function rotates the camera
void camera()
{
    glRotatef(xrot, 1, 0, 0);
    glRotatef(yrot, 0, 1, 0);
    //gluLookAt(xpos, ypos, zpos, xrot, yrot, 0, 1, 1, 0);
    glTranslatef(-xpos, -ypos, -zpos);
}

//creates tall towers using glut 3D analytical objects and makes use of push and pop matrices
void drawTower()
{
    obj = gluNewQuadric(); //creates a new quadric object
    
    glPushMatrix();
    glRotatef(90.0,1.0,0.0,0.0);
    glTranslatef(0.0,0.0,-5.0);
    glColor3f(0.858824, 0.858824, 0.439216);
    gluCylinder(obj,5.0,5.0,100,50,50); //draw a cylinder
    glColor3f(1.0, 1.0, 1.0);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0.0,10.0,0.0);
    glColor3f(0.2, 0.7, 0.6);
    glutSolidCube(10.0); //draw a cube
    glColor3f(1.0, 1.0, 1.0);
    glPopMatrix();
    
    glPushMatrix();
    glRotatef(-90.0,1.0,0.0,0.0);
    glTranslatef(0.0,0.0,15.0);
    glColor3f(0.2, 0.6, 0.7);
    glutSolidCone(4.0,9.0,17.0,10.0); //draw a cone
    glColor3f(1.0, 1.0, 1.0);
    glPopMatrix();
    
}

//creates floating teapots in the sky using the GLUT teapot and rotates the pots
//uses push and pop matrices to handle translations
void drawTeapots(void)
{
    glPushMatrix();
    glTranslatef(5.0, 2, 8.0);
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
void drawObstacle(float x, float y, float z)
{
    glPushMatrix();
    glTranslatef(x, y, z);
    buffer = new BoundingBox(x-0.8, x+0.8, y+1.75, y, z-0.8, z+0.8);
    obs.push_back(*buffer);
    glColor3f(0.0, 0.5, 0.0);
    glRotatef(-90,1.0,0.0,0.0);
    glutSolidCone(0.5,1.75,5.0,5.0); //draw solid cone
    glColor3f(1.0, 1.0, 1.0);
    glPopMatrix();
}

// display function
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST); //enable depth test
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    int width = glutGet(GLUT_WINDOW_WIDTH); //get window width
    int height = glutGet(GLUT_WINDOW_HEIGHT); //get window height
    glViewport(0, 0, width, height);
    gluPerspective(60.0,(GLfloat)width/(GLfloat)height, 0.1, 300); //use gluPerspective
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    floors.clear();
    obs.clear();
    
    camera(); //envoke camera() function
    
    /*********************** Lighting Settings ************************/
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialf(GL_FRONT, GL_SHININESS, 100);
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    
    glEnable(GL_COLOR_MATERIAL);
    /************************** End Lighting *************************/
    
    drawTeapots(); //draws teapots
    
    /************************** Draws two towers *********************/
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
    /***************************************************************/
    
    
    /***************** Draws floating objects *********************/
    //draw floating floors
    float x = 0;
    float y = -2;
    float z = 0;
    float gap = 4;
    
    for (int i = 0; i < 20; i++)
    {
        x =floatingCoordinate[i][0];
        y =floatingCoordinate[i][1];
        z =floatingCoordinate[i][2];
        glPushMatrix();
        glTranslatef(x, y, z);
        if (i%2 == 0) glRotatef(f_rotateY, 0.0, 1.0, 0.0);
        buffer = new BoundingBox(x-3, x+3, -1.5, -2.5, z-3, z+3);
        floors.push_back(*buffer);
        drawFloating(3.0);
        glPopMatrix();
    }
    
    /*************** draw obstacles on the some blocks ***************/
    drawObstacle(floatingCoordinate[2][0], floatingCoordinate[2][1], floatingCoordinate[2][2]);
    drawObstacle(floatingCoordinate[7][0], floatingCoordinate[7][1], floatingCoordinate[7][2]);
    drawObstacle(floatingCoordinate[10][0], floatingCoordinate[10][1], floatingCoordinate[10][2]);
    
    /***************************************************************/
    
    //draw background
    glPushMatrix();
    drawBackground(100.0);
    glPopMatrix();
    
    glColor3f(1.0f, 1.0f, 1.0f);
    glutSwapBuffers();
}

//function that handles mouse movements
//takes two parameters, x and y, that are the position of the mouse on the screen
void mouseMovement(int x, int y) {
    int diffx=x-lastx; //check the difference between the current x and the last x position
    int diffy=y-lasty; //check the difference between the y and the last y position
    lastx=x; //set lastx to the current x position
    lasty=y; //set lasty to the current y position
    if (xrot + (float) diffy <= 90 && xrot + (float) diffy >= -90) xrot += (float) diffy; //set the xrot to xrot with the addition of the difference in the y position
    yrot += (float) diffx;    //set the xrot to yrot with the addition of the difference in the x position
    glutPostRedisplay();
}

//this function checks for the bounding area for floating objects
void checkBounding(int x, int y)
{
    bool flag = false;
    for (int i = 0; i < floors.size();i++)
        if (x <= floors[i].right && x >= floors[i].left && y >= floors[i].front && y <= floors[i].back)
            flag = true;
    
    if (!flag) falling = true;
}

//this function checks for the bounding area for obstacles
void checkObstacle(int x, int y)
{
    bool flag = false;
    for (int i = 0; i < obs.size();i++)
        if (x <= obs[i].right && x >= obs[i].left && y >= obs[i].front && y <= obs[i].back)
            flag = true;
    
    if (flag)
        collide = true;
    else
        collide = false;
}

//when you fall from the blocks, this functions gets you back to the starting point and you can start the game all over again
void reset()
{
    xpos = 0;
    ypos = 0;
    zpos = 0;
    falling = false;
    collide = false;
    xrot = 0;
    yrot = 0;
    glutPostRedisplay();
}

//idle function keeps calling events when there are no other events in the event queue
//the middle block keeps rotating halfway
//the teapots are in a constant loops of rotation
//jumping action of the camera
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
    float yrotrad;
    //    float xrotrad;
    if (jumpping)
    {
        yrotrad = (yrot / 180 * 3.141592654f);
        //        xrotrad = (xrot / 180 * 3.141592654f);
        xpos += float(0.1*sin(yrotrad)) ;
        zpos -= float(0.1*cos(yrotrad)) ;
        
        if (counter < 20) ypos += rate;
        else if (counter < 39) ypos -= rate;
        else
        {
            jumpping = false;
            counter = 0;
            checkBounding(xpos, zpos);
            checkObstacle(xpos, zpos);
        }
        counter ++;
    }
    
    if (forwarding)
    {
        yrotrad = (yrot / 180 * 3.141592654f);
        //        xrotrad = (xrot / 180 * 3.141592654f);
        xpos += float(0.1*sin(yrotrad));
        zpos -= float(0.1*cos(yrotrad));
        if (!jumpping)
        {
            checkBounding(xpos, zpos);
            if (!falling)
            {
                checkObstacle(xpos, zpos);
                if (collide)
                {
                    xpos -= float(0.1*sin(yrotrad));
                    zpos += float(0.1*cos(yrotrad));
                }
            }
        }
    }
    
    if (backwarding)
    {
        yrotrad = (yrot / 180 * 3.141592654f);
        //        xrotrad = (xrot / 180 * 3.141592654f);
        xpos -= float(0.1*sin(yrotrad));
        zpos += float(0.1*cos(yrotrad));
        checkBounding(xpos, zpos);
        if (!falling)
        {
            checkObstacle(xpos, zpos);
            if (collide)
            {
                xpos += float(0.1*sin(yrotrad));
                zpos -= float(0.1*cos(yrotrad));
            }
        }
    }
    
    if (leftshift)
    {
        yrotrad = (yrot / 180 * 3.141592654f);
        xpos -= float(0.1*cos(yrotrad));
        zpos -= float(0.1*sin(yrotrad));
        checkBounding(xpos, zpos);
        if (!falling)
        {
            checkObstacle(xpos, zpos);
            if (collide)
            {
                xpos += float(0.1*sin(yrotrad));
                zpos += float(0.1*cos(yrotrad));
            }
        }
    }
    
    if (rightshift)
    {
        yrotrad = (yrot / 180 * 3.141592654f);
        xpos += float(0.1*cos(yrotrad));
        zpos += float(0.1*sin(yrotrad));
        checkBounding(xpos, zpos);
        if (!falling)
        {
            checkObstacle(xpos, zpos);
            if (collide)
            {
                xpos -= float(0.1*sin(yrotrad));
                zpos -= float(0.1*cos(yrotrad));
            }
        }
    }
    
    if (falling)
    {
        ypos -= 0.3;
        if (ypos < -50) reset();
    }
    
    glutPostRedisplay();
}

//keyboard callback function
//this function deals with commands when the keys are pressed down
void keyboard(unsigned char key, int x, int y)
{
    int w = glutGetWindow();
    switch (key)
    {
        case 27: //exit the application
            glutDestroyWindow(w);
            glDeleteTextures((GLsizei)MAX_TEXTURES, (GLuint *)g_Texture);
            exit(0);
            break;
        case 'w': //move forward
            forwarding = true;
            break;
        case 's': //move backward
            backwarding = true;
            break;
        case 'd': //move right
            rightshift = true;
            break;
        case 'a': //move left
            leftshift = true;
            break;
        case 32: //spacebar to jump
            jumpping  = true;
            break;
        default:
            break;
    }
    glutPostRedisplay(); //call glutPostRedisplay function
}

//keyboard callback function
//this function deals with commands when the keys are not pressed
void keyup(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'w': //move forward is set to false
            forwarding = false;
            break;
        case 's': //move backward is set to false
            backwarding = false;
            break;
        case 'a': //move left is set to false
            leftshift = false;
            break;
        case 'd': //move right is set to false
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
    glutCreateWindow("CS 300: Project 3");  // window title
    glutFullScreen();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyup);
    glutIdleFunc(idle);
    glutPassiveMotionFunc(mouseMovement);
    init();
    
    glutMainLoop();
}
