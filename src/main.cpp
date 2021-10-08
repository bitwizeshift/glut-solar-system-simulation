/*
#================================================================================
# * Gravity Simulator      Ver. 1.0.0
#--------------------------------------------------------------------------------
# Author: Matthew Rodusek
# Date: January 09, 2011
# Extra Credits: Mr. Bagshaw's algorithm to determine position
#         OpenGL's documentation and support forum
#================================================================================
*/
//#==============================================================================
//# Header Files
//#==============================================================================
#include <stdio.h>        // Header File for the standard library
#include <GL/gl.h>        // Header File for the OpenGL Library
#include <GL/glut.h>      // Header File for the GLUT Library
#include <math.h>         // Header File for the math library

//#==============================================================================
//# Definitions
//#==============================================================================

typedef unsigned char UCHAR;
typedef bool          FLAG;
typedef char          INDEX;

//#==============================================================================
//# Structures & Enumerations
//#==============================================================================

// tagObjects
typedef struct
{
  double x, dx, ddx; // x component of position, velocity, acceleration
  double y, dy, ddy; // y component of position, velocity, acceleration
  double z, dz, ddz; // z componont of position, velocity, acceleration
  double mass;       // mass of the object (planet)
  double radius;     // radius of the object (for deciding width of spheres)
}tagObjects;

// Enumeration for menu index
enum
{
  MENU_CAMERA = 0x0001,
  MENU_EXIT
};

// Enumeration of planets
enum
{
  SUN = 0,  MERCURY,  VENUS,  EARTH,   MARS,
  JUPITER,  SATURN,  URANUS, NEPTUNE, PLUTO
};


//#==============================================================================
//# Globals
//#==============================================================================
// constants:
const double scale = 5E-11;  // Scale of the system

// statics:
static int WIN_WIDTH  = 640;
static int WIN_HEIGHT = 480;

static float zoomFactor   = 10.0;    // Create zoom factor starting at 1.0
static float speedFactor  = 1.0;     // Create factor for speed of calculations
static float lasty        = 0.0;     //
static INDEX activeCamera = 0;       // Check active camera

float yrot  = 0.0;  // the y rotation angle
double time = 0;    // the time to increase

// Structures:
tagObjects objects[10]; // Create 10 objects (9 planets, 1 star)



//#==============================================================================
//# Prototypes
//#==============================================================================

void drawPlanet    ( int index,  float x_pos, float y_pos, float z_pos);
void init      ( );
void calculate    ( );
void idle      ( );
void visible    ( int vis );
void reshape    ( int width,  int height);
void special    ( int key,    int x,    int y );
void keyboard    ( UCHAR key,  int x,    int y );
void mouseMotion  ( int x,    int y );
void SelectFromMenu ( int idCommand );
void moveCursor    ( int x_pos,  int y_pos );
int  BuildPopupMenu ( void );

//#==============================================================================
//# Functions
//#==============================================================================

//#==============================================================================
//# * BuildPopupMenu
//#------------------------------------------------------------------------------
//# Used to build a right-click menu
//#==============================================================================
int BuildPopupMenu ( void )
{
  int    menu = glutCreateMenu (SelectFromMenu);
  glutAddMenuEntry ("Center Camera \tc",  MENU_CAMERA    );
  glutAddMenuEntry ("Exit \tEsc",    MENU_EXIT    );
  return  menu;
}
//#==============================================================================
//# * SelectFromMenu
//#------------------------------------------------------------------------------
//# Used to determine which menu option was selected from the right-click menu
//#==============================================================================
void SelectFromMenu( int idCommand )
{
  switch (idCommand)
  {
    case MENU_CAMERA:
    yrot = 0.0;          // reset yrotation
    zoomFactor = 10.0;      // reset zoom
    break;
    case MENU_EXIT:
    exit (0);          // close program
    break;
  }
  // Almost any menu selection requires a redraw
  glutPostRedisplay();
}

//#==============================================================================
//# * init
//#------------------------------------------------------------------------------
//# initialize all the values, cameras, and lighting first.
//#==============================================================================
void init()
{

  glClearColor(0.0, 0.0, 0.0, 0.0); // Set window color to black.
  glEnable( GL_DEPTH_TEST );        // Set up depth

  glEnable( GL_COLOR_MATERIAL );    // Configure glColor().
  glColorMaterial( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );
  glShadeModel( GL_SMOOTH );        // set shading (For planets)

  // Initialize the values of the Object structure

  // Sun
  objects[SUN].mass     = 1.99E+30;
  objects[SUN].radius   = 695500000 * scale*50;
  objects[SUN].x        =
  objects[SUN].y        =
  objects[SUN].z        =
  objects[SUN].dx       =
  objects[SUN].dy       =
  objects[SUN].dz       = 0;

  // Mercury
  objects[MERCURY].mass   = 3.34E+23;
  objects[MERCURY].radius = 2439700 * scale*100;
  objects[MERCURY].x      =-20665696392;
  objects[MERCURY].y      =-59636889090;
  objects[MERCURY].z      =-29712844059;
  objects[MERCURY].dx     = 3.66E+04;
  objects[MERCURY].dy     =-9.51E+03;
  objects[MERCURY].dz     =-8.88E+03;

  // Venus
  objects[VENUS].mass   = 4.87E+24;
  objects[VENUS].radius = 6051800 * scale*100;
  objects[VENUS].x      =-1.07478E+11;
  objects[VENUS].y      =-6401037913;
  objects[VENUS].z      = 3920831085;
  objects[VENUS].dx     = 8.82E+02;
  objects[VENUS].dy     =-3.19E+04;
  objects[VENUS].dz     =-1.45E+04;

  // Earth
  objects[EARTH].mass   = 5.98E+24;
  objects[EARTH].radius = 6378100 * scale*100;
  objects[EARTH].x      =-26516914541;
  objects[EARTH].y      = 1.32754E+11;
  objects[EARTH].z      = 57555479554;
  objects[EARTH].dx     =-2.98E+04;
  objects[EARTH].dy     =-4.78E+03;
  objects[EARTH].dz     =-2.06E+03;

  // Mars
  objects[MARS].mass    = 6.40e23;
  objects[MARS].radius  = 3397000 * scale*100;
  objects[MARS].x       = 2.08092E+11;
  objects[MARS].y       = 1150108018;
  objects[MARS].z       =-5098849339;
  objects[MARS].dx      = 1.30E+03;
  objects[MARS].dy      = 2.39E+04;
  objects[MARS].dz      = 1.09E+04;

  // Jupiter
  objects[JUPITER].mass   = 1.90E+27;
  objects[JUPITER].radius = 71492000 * scale*100;
  objects[JUPITER].x      = 5.94749E+11;
  objects[JUPITER].y      = 4.1426E+11;
  objects[JUPITER].z      = 1.63068E+11;
  objects[JUPITER].dx     =-7.89E+03;
  objects[JUPITER].dy     = 1.02E+04;
  objects[JUPITER].dz     = 4.54E+03;

  // Saturn
  objects[SATURN].mass   = 5.69E+26;
  objects[SATURN].radius = 60268000 * scale*100;
  objects[SATURN].x      = 9.48999E+11;
  objects[SATURN].y      = 9.31359E+11;
  objects[SATURN].z      = 3.4387E+11;
  objects[SATURN].dx     =-7.44E+03;
  objects[SATURN].dy     = 6.12E+03;
  objects[SATURN].dz     = 2.85E+03;

  // Uranus
  objects[URANUS].mass   = 8.67E+25;
  objects[URANUS].radius = 25559000 * scale*100;
  objects[URANUS].x      = 2.17596E+12;
  objects[URANUS].y      =-1.85516E+12;
  objects[URANUS].z      =-8.43327E+11;
  objects[URANUS].dx     = 4.66E+03;
  objects[URANUS].dy     = 4.29E+03;
  objects[URANUS].dz     = 1.81E+03;

  // Neptune
  objects[NEPTUNE].mass   = 1.03E+26;
  objects[NEPTUNE].radius = 24764000 * scale*100;
  objects[NEPTUNE].x      = 2.5472E+12;
  objects[NEPTUNE].y      =-3.41681E+12;
  objects[NEPTUNE].z      =-1.46188E+12;
  objects[NEPTUNE].dx     = 4.50E+03;
  objects[NEPTUNE].dy     = 2.91E+03;
  objects[NEPTUNE].dz     = 1.08E+03;

  // Pluto
  objects[PLUTO].mass   = 6.58e23;
  objects[PLUTO].radius = 1180000 * scale*100;  // has to be scaled to be seen
  objects[PLUTO].x      =-1.42071E+12;
  objects[PLUTO].y      =-4.20637E+12;
  objects[PLUTO].z      =-8.84208E+11;
  objects[PLUTO].dx     = 5.28E+03;
  objects[PLUTO].dy     =-1.96E+03;
  objects[PLUTO].dz     =-2.19E+03;
}

//#==============================================================================
//# * drawPlanet
//#------------------------------------------------------------------------------
//# this function outputs the planet and applies the texture to them
//#==============================================================================
void drawPlanet(int index, float x_pos, float y_pos, float z_pos)
{
  glPushMatrix();
  switch(index)
  {
    // Set up colors for planets
    case SUN:    glColor3f(1.0, 0.7, 0.0);  break;
    case MERCURY:  glColor3f(1.0, 0.9, 0.1);  break;
    case VENUS:    glColor3f(1.0, 0.5, 0.1);  break;
    case EARTH:    glColor3f(0.1, 8.0, 0.0);  break;
    case MARS:    glColor3f(1.0, 0.0, 0.0);  break;
    case JUPITER:  glColor3f(0.8, 0.3, 0.8);  break;
    case SATURN:  glColor3f(1.0, 0.9, 0.7);  break;
    case NEPTUNE:  glColor3f(0.2, 0.1, 1.0);  break;
    case URANUS:  glColor3f(0.0, 0.8, 0.4);  break;
    case PLUTO:    glColor3f(0.9, 0.9, 1.0);  break;
  }
  glTranslatef(x_pos*scale, y_pos*scale, z_pos*scale);  // move matrix to scaled positions
  glutSolidSphere(objects[index].radius, 30, 30);      // draw planet
  glPopMatrix();
}

//#==============================================================================
//# * camera
//#------------------------------------------------------------------------------
//# calculates viewport
//#==============================================================================
void newcamera(double radius, double x_pos, double y_pos, double z_pos)
{
  double x = x_pos * scale;      // calculate scaled x coordinate
  double y = y_pos * scale;      // calculate scaled y coordinate
  double z = z_pos * scale;      // calculate scaled z coordinate

  double x_cam = 0;          // x axis is pivot point
  double y_cam = ( zoomFactor*radius )*sin(yrot);    //
  double z_cam = ( zoomFactor*radius )*cos(yrot);    //

  // Load projection matrix
  glMatrixMode  ( GL_PROJECTION );
  glLoadIdentity  ( );
  // set perspective
  gluPerspective  ( 30, (GLfloat)WIN_WIDTH / (GLfloat)WIN_HEIGHT, 0.01, 1000000 );
  // Load modelview matrix
  glMatrixMode  ( GL_MODELVIEW );
  glLoadIdentity  ( );
  // center model
  glTranslatef  ( -x,  -y,  -z);  //center model matrix
  // Look at planet from rotated coordinates
  gluLookAt    ( 0.0, y_cam, z_cam,
            0.0, 0.0, 0.0,
            0.0, 1.0, 0.0 );

}
//#==============================================================================
//# * reshape
//#------------------------------------------------------------------------------
//# Called when the window is reshaped to recalculate the window size.
//#==============================================================================
void reshape (int width, int height)
{
  WIN_WIDTH = width;          // Set global WIN_WIDTH to width
  WIN_HEIGHT = height;        // Set global WIN_HEIGHT to height
  glViewport(0, 0, (GLsizei)width, (GLsizei)height); // change viewport
  glMatrixMode(GL_PROJECTION);                       // set new matrix
  glLoadIdentity();                                  // Load the matrix identity
  // set perspective
  gluPerspective( 0, (GLfloat)width / (GLfloat)height, 0.1, 1000000 );
  glMatrixMode(GL_MODELVIEW); // Load Modelview patrix
  glLoadIdentity();
  newcamera(
    objects[activeCamera].radius,
    objects[activeCamera].x,
    objects[activeCamera].y,
    objects[activeCamera].z
  );
  glutPostRedisplay(); // marks window to be repainted
}

//#==============================================================================
//# * calculate
//#------------------------------------------------------------------------------
//# Calculates the required positions of the planets, then outputs them.
//#==============================================================================
void calculate()
{
  double interval        = 86400 * speedFactor;  // the interval for calculation (1 day)

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear window.
  newcamera(
    objects[activeCamera].radius,
    objects[activeCamera].x, // Recalculate veiw
    objects[activeCamera].y,
    objects[activeCamera].z
  );
  double Fx[10], Fy[10], Fz[10];            // Array of force vectors
  float gravity_constant = 6.67E-11;          // Kepler's gravity constant
  // Set the forces of all the planets to 0
  for( int i = 0; i < 10 ; i++)
  {
    Fx[i] = 0;
    Fy[i] = 0;
    Fz[i] = 0;
  }
  for( int i = 0; i < 10 ; i++)
  {
    for( int j = 0; j < 10 ; j++)
    {
      if(j!=i)  // if not itself (added to ensure no division by zero error)
      {
        // Calculate force in the x direction(Sum of all forces = GMm(x2-x1)/r^3)
        Fx[i] += gravity_constant * objects[i].mass * objects[j].mass * (objects[j].x - objects[i].x)/
          pow(sqrt(
          pow((objects[j].x - objects[i].x),2) +
          pow((objects[j].y - objects[i].y),2) +
          pow((objects[j].z - objects[i].z),2)),
          3);
        // Calculate force in the y direction (Sum of all forces = GMm(x2-x1)/r^3)
        Fy[i] +=gravity_constant * objects[i].mass * objects[j].mass * (objects[j].y - objects[i].y)/
          pow(sqrt(
          pow((objects[j].x - objects[i].x),2) +
          pow((objects[j].y - objects[i].y),2) +
          pow((objects[j].z - objects[i].z),2)),
          3);
        // Calculate force in the z direction (Sum of all forces = GMm(x2-x1)/r^3)
        Fz[i] += gravity_constant * objects[i].mass * objects[j].mass * (objects[j].z - objects[i].z)/
          pow(sqrt(
          pow((objects[j].x - objects[i].x),2) +
          pow((objects[j].y - objects[i].y),2) +
          pow((objects[j].z - objects[i].z),2)),
          3);
      }
    }
  }
  time += interval;  // Increase time by the interval
  // Loop for calculating acceleration, velocity, and position
  for( int i = 0; i < 10 ; i++)
  {
    // calculate acceleration
    objects[i].ddx   =  Fx[i] / objects[i].mass;
    objects[i].ddy   =  Fy[i] / objects[i].mass;
    objects[i].ddz   =  Fz[i] / objects[i].mass;
    // calculate velocity
    objects[i].dx  +=  objects[i].ddx  * interval;
    objects[i].dy  +=  objects[i].ddy  * interval;
    objects[i].dz  +=  objects[i].ddz  * interval;
    // calculate position
    objects[i].x  +=  objects[i].dx  * interval;
    objects[i].y  +=  objects[i].dy  * interval;
    objects[i].z  +=  objects[i].dz  * interval;
    // Output the objects
    drawPlanet(i, objects[i].x, objects[i].y, objects[i].z);
  }
  //output debug information
  float days  = time/(60*60*24);    // create temporary variable for days
  float years = days/365.25;      // create temporary variable for years
  printf("%s%1.1f%s","Speed Multiplier: \t", speedFactor, "x\n");
  printf("%s%6.3f\n","Time Elapsed (s): \t", time);
  printf("%s%6.3f\n","Time Elapsed (d): \t", days);
  printf("%s%3.3f\n","Time Elapsed (y): \t", years);
  printf("%s%i \n",  "Active Camera   : \t", activeCamera);
  printf("%s%1.1f  ","Scale Factor  : \t", zoomFactor);
  glutPostRedisplay();        // marks window to be repainted
  glutSwapBuffers();          // performs a buffer swap
}

//#==============================================================================
//# * idle
//#------------------------------------------------------------------------------
//# continuesly redraw window whenever screen is idle
//#==============================================================================
void idle()
{
  static int lastTime = 0;                // time of last redraw
  int time = glutGet(GLUT_ELAPSED_TIME);  // current time

  if(lastTime == 0 || time >= lastTime )
  {
    lastTime = time;
    glutPostRedisplay();
  }
}

//#==============================================================================
//# * visible
//#------------------------------------------------------------------------------
//# When window becomes visible, continuesly repaint itself
//#==============================================================================
void visible(int vis)
{
  // if vis is true, define idle function. Otherwise no function
  glutIdleFunc(vis == GLUT_VISIBLE ? idle : NULL);
}

//#==============================================================================
//# * special
//#------------------------------------------------------------------------------
//# Called when a "special" key is hit. Used to determine when the directional
//# keys are hit.
//#==============================================================================
void special(int key, int x, int y)
{
  // check for key that was pressed
  switch(key)
  {
    // If Up arrow is pressed
    case GLUT_KEY_UP:    if(zoomFactor>2.0)    zoomFactor  -= 0.1;  break;
    // If Down arrow is pressed
    case GLUT_KEY_DOWN:  if(zoomFactor<100.0)  zoomFactor  += 0.1;  break;
    // If Left arrow is pressed
    case GLUT_KEY_LEFT:
      if(activeCamera>0)
        {activeCamera--;}
      else{activeCamera=9;}
      break;
      //user_theta  += 0.1; break;
    // If Right arrow is pressed
    case GLUT_KEY_RIGHT:
      if(activeCamera<9)
        {activeCamera++;}
      else{activeCamera=0;}
      break;
      //user_theta  -= 0.1; break;
  }
  //computeLocation();          // Compute camera location
  newcamera(objects[activeCamera].radius, objects[activeCamera].x,
        objects[activeCamera].y,    objects[activeCamera].z );
  glutPostRedisplay();        // marks window to be repainted
}

//#==============================================================================
//# * keyboard
//#------------------------------------------------------------------------------
//# Called when a key is hit. Used to follow planets via cameras
//#==============================================================================
void keyboard(UCHAR key, int x, int y)
{
  // check for key that was pressed
  switch( key )
  {
  // if q key was pressed
  case 'q':  if(speedFactor < 2.0 )  speedFactor+=.1;  break;
  // if a key was pressed
  case 'a':  if(speedFactor > 0.1 )  speedFactor-=.1;  break;
  }
}

//#==============================================================================
//# * mouseMotion
//#------------------------------------------------------------------------------
//# Called when the mouse is moved. Set to only to rotate camera when
//# the left button is held down and mouse is moved.
//#==============================================================================
void mouseMotion( int x, int y )
{
  int diffy      = y - lasty;  // check the difference between the current y and the last y position
  lasty        = y;            // set lasty to the current y position
    yrot += (float) diffy/100; // set the xrot to yrot with the addition of the difference in the x position
  if(yrot>360){yrot=0;}        // if rotation angle is greater than 360; reset to 0
  else if(yrot<0){yrot=360;}   // if angle is less than 0; reset to 360;
}

//#==============================================================================
//# * Main
//#------------------------------------------------------------------------------
//# The main processing loop. The program starts and ends here
//#==============================================================================
int main(int argc, char **argv)
{
  glutInit(&argc, argv);          // Initialize GLUT with main's parameters
  glutInitDisplayMode( GLUT_DEPTH  | GLUT_DOUBLE | GLUT_RGB );
  glutInitWindowPosition( 50, 100 );    // Set up display window's position.
  glutInitWindowSize( WIN_WIDTH, WIN_HEIGHT ); // Initial window size
  glutCreateWindow( "Solar System" );          // Name window

  init(); // initialize function

  glutDisplayFunc( calculate );  // Main draw function
  glutVisibilityFunc( visible ); // test if window is visible
  glutReshapeFunc( reshape );    // Text for window resize
  glutSpecialFunc( special );    // Get special keyboard input
  glutKeyboardFunc( keyboard );  // Get Keyboard input
  glutMotionFunc( mouseMotion ); // Get mouse movement input

  // Build popup menu upon rightclick
  BuildPopupMenu();
  glutAttachMenu( GLUT_RIGHT_BUTTON );

  glutMainLoop(); // call main loop
  return 0;
}