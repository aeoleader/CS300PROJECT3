/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * CSCI 300 Graphics Project 3
 *
 * Nan Jiang, Pratistha Bhandari, Xiangyu Li *
 *
 * Activity.cpp - An implementation file for navigation.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <cmath>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <cstdlib>
#include <iostream>
/***** Global variables *****/
GLint win_width = 800;      // window dimensions
GLint win_height = 800;

// Parameters for gluPerspective()
GLint angle = 60, nearP = 1, farP = 1000;

GLUquadricObj *obj;         // Pointer for quadric objects.

// Rotation of the whole object
static GLfloat rotate_x = 0.0, rotate_y = 0.0, rotate_z = 0.0;



// Mouse function related variables
static int moving = 0, startx, starty;



float trans_x = 0;
float trans_y = 0;
float trans_z = 0;

// Initialize OpenGL graphics
void init(void)
{
    glClearColor(0.6f, 0.6f, 0.6f, 1.0f); // gray background
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(angle, 1.0, nearP, farP);
    gluLookAt(0.0, 0.0, 350.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat gray[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat cyan[] = { 0.0, 1.0, 1.0, 1.0 };
    GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat direction[] = {1.0, 1.0, 1.0, 0.0};
    
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, cyan);
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glMaterialf(GL_FRONT, GL_SHININESS, 100);
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, gray);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);
    glLightfv(GL_LIGHT0, GL_POSITION, direction);
    
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);            // turn on the depth buffer
}


// display function
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glutSwapBuffers();
}

// reshape function
void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
        gluPerspective(angle, 1.0, nearP, farP);
    else
        gluPerspective(angle, 1.0, nearP, farP);
    gluLookAt(0.0, 0.0, 350.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glMatrixMode(GL_MODELVIEW);
    glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
}

// keyboard callback function
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'w':
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            display();
            break;
        case 'f':
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            display();
            break;
        case 113:
            exit(0);
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

// arrow keys that are used to control the rotation of the object
void specialKeys( int key, int x, int y )
{
    if (key == GLUT_KEY_RIGHT)
        rotate_x -= 5.0;
    else if (key == GLUT_KEY_LEFT)
        rotate_x -= -5.0;
    else if (key == GLUT_KEY_UP)
        rotate_y += 5.0;
    else if (key == GLUT_KEY_DOWN)
        rotate_y -= 5.0;
    
    glutPostRedisplay();
}

// mouse function
static void mouse(int button, int state, int x, int y)
{
    /* Rotate the scene with the left mouse button. */
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            moving = 1;
            startx = x;
            starty = y;
        }
        if (state == GLUT_UP) {
            moving = 0;
        }
    }
}

// motion function
static void motion(int x, int y)
{
    if (moving) {
        rotate_x = rotate_x + (x - startx);
        rotate_y = rotate_y + (y - starty);
        startx = x;
        starty = y;
        glutPostRedisplay();
    }
}


// main function
int main(int argc, char **argv)
{
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );
    glutInitWindowSize(win_width, win_height);       // 800 x 800 pixel window
    glutInitWindowPosition(100, 100);   // place window upper at the left corner on display
    glutCreateWindow("Project 2 - Minions");  // window title
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    init();
    
    glutMainLoop();
}