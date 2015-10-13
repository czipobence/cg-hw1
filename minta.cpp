#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#define M_PI 3.14

struct Vector {
   float x, y, z;

   Vector( ) {
    x = y = z = 0;
   }
   Vector(float x0, float y0, float z0 = 0) {
    x = x0; y = y0; z = z0;
   }
   Vector operator*(float a) {
    return Vector(x * a, y * a, z * a);
   }
   Vector operator+(const Vector& v) {
     return Vector(x + v.x, y + v.y, z + v.z);
   }
   Vector operator-(const Vector& v) {
     return Vector(x - v.x, y - v.y, z - v.z);
   }
   float operator*(const Vector& v) {
    return (x * v.x + y * v.y + z * v.z);
   }
   Vector operator%(const Vector& v) {
    return Vector(y*v.z-z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
   }
   float Length() { return sqrt(x * x + y * y + z * z); }
};


struct Color {
   float r, g, b;

   Color( ) {
    r = g = b = 0;
   }
   Color(float r0, float g0, float b0) {
    r = r0; g = g0; b = b0;
   }
   Color operator*(float a) {
    return Color(r * a, g * a, b * a);
   }
   Color operator*(const Color& c) {
    return Color(r * c.r, g * c.g, b * c.b);
   }
   Color operator+(const Color& c) {
     return Color(r + c.r, g + c.g, b + c.b);
   }
};



long lastOnIdle = 0;
long nowOnIdle  = 0;

const int screenWidth = 600;
const int screenHeight = 600;
Color image[screenWidth*screenHeight];



struct Adatok{
    Vector r;   //vezpont
    Vector k;      //középpont adott vezérlőponthoz
    long t;         //amikor a cvepontot lerkajuk--> time--> ms
};

Adatok adat[11];
int adatDB = 0;


bool SPACE_GOMB = false;
int MELYIK_INDEXU_PONT_LETT_KIVALASZTVA = -1;

float SZOG_BALRA = 90;
float SZOG_JOBBRA = 90;




//téglalap, aminek az átló két végpontját jegyezzük meg
class Kamera{
public:
    Vector also;
    Vector felso;

    Kamera(){
        //58 × 68-as világ
        also = Vector(0,0);
        felso = Vector(58,68);
    }


    void rajzol_kor(Vector pont, float sugar, Color r){
        glColor3f(r.r,r.g,r.b);

        glBegin(GL_POLYGON);
            for(float i=0; i<=2*M_PI; i+=0.1)
                glVertex2f(pont.x + sugar*cos(i),pont.y + sugar*sin(i));

        glEnd();
    }

    void rajzol_pontok(){
        if(adatDB != 0){
            int i;
            for(i=0; i<adatDB; i++){
                rajzol_kor(adat[i].r,2.0, Color(0,0,0));
                }

        }
    }

};

Kamera kamera;

//sünis könyv első felében az általános egyenlet
class GorbeCR{
public:
    // általános harmadfokú egyenlet, melynek minden együtthatója egy külön metódus
    Vector gorbe(float t, int i){
        float nevezo = t - adat[i].t;
        Vector eredmeny =  a0(i)*nevezo*nevezo*nevezo+b0(i)*nevezo*nevezo+c0(i)*nevezo+d0(i);
        return eredmeny;
    }


    Vector a0(int i){
        float nevezo = adat[i+1].t-adat[i].t;
        Vector eredmeny = (sebesseg(i+1)+sebesseg(i))*(1.0/nevezo/nevezo)-(adat[i+1].r*2-adat[i].r*2)*(1.0/nevezo/nevezo/nevezo);
        return eredmeny;
    }

    Vector b0(int i){
        float nevezo = adat[i+1].t-adat[i].t;
        Vector eredmeny =  (adat[i+1].r*3-adat[i].r*3)*(1.0/nevezo/nevezo)-(sebesseg(i+1)+sebesseg(i)*2)*(1.0/nevezo);
        return eredmeny;
    }

    Vector c0(int i){
        return sebesseg(i);
        }

    Vector d0(int i){
        return adat[i].r;
        }

    //sünis könyv második felében
    Vector sebesseg(int i){
        Vector eredmeny(0,0,0);
        if(i==0 || (i+1) == adatDB) // az első és utolsó vezérlőpontbam a sebesség 0
            return Vector(0,0,0);


        eredmeny =  (adat[i].r-adat[i-1].r)*(1.0/(adat[i].t-adat[i-1].t))+(adat[i+1].r-adat[i].r)*(1.0/(adat[i+1].t-adat[i].t));
        return (eredmeny*0.5);
    }

    void rajzol_catmull_rom(){
        glColor3f(0,1,0);

        glBegin(GL_LINE_STRIP);

            for(int i=0; i<adatDB-1; i++){
                float lepeskoz = (adat[i+1].t-adat[i].t)/1200.0;
                float t;
                for(t=adat[i].t; t<adat[i+1].t; t+=lepeskoz){
                    Vector e = gorbe(t,i);
                    glVertex2f(e.x,e.y);
                }
            }
         glEnd();
    }

};


void onInitialization( ) {
    glViewport(0, 0, screenWidth, screenHeight);
}

GorbeCR rom;

void rajzol(){
        rom.rajzol_catmull_rom();
}

void onDisplay( ) {
    glClearColor(0.25f, 0.25f, 0.25f, 0.25f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);    //transzformáláshoz kell
    glLoadIdentity();
    gluOrtho2D(kamera.also.x,kamera.felso.x,kamera.also.y,kamera.felso.y);

    //ha van pontunk rajzolunk
    if(adatDB > 1){
        rajzol();
    }
    kamera.rajzol_pontok();

    glutSwapBuffers();

}

void onKeyboard(unsigned char key, int x, int y) {

}

void onKeyboardUp(unsigned char key, int x, int y) {

}

void onMouse(int button, int state, int x, int y) {

    float xV = x / 10.34 + kamera.also.x;
    float yV = (600 - y) / 8.82 + kamera.also.y;

    if (button == GLUT_LEFT && state == GLUT_DOWN && SPACE_GOMB==false){

        if(adatDB<10){

            adat[adatDB].t = glutGet(GLUT_ELAPSED_TIME);
            adat[adatDB++].r = Vector(xV,yV);
            adat[adatDB-1].k=adat[adatDB-1].r-Vector(0,5);
            glutPostRedisplay();

            }
    }
}

void onMouseMotion(int x, int y){
}



void onIdle( ) {
    glutPostRedisplay();
}
// ...Idaig modosithatod
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// A C++ program belepesi pontja, a main fuggvenyt mar nem szabad bantani
int main(int argc, char **argv) {
    glutInit(&argc, argv); 				// GLUT inicializalasa
    glutInitWindowSize(600, 600);			// Alkalmazas ablak kezdeti merete 600x600 pixel
    glutInitWindowPosition(100, 100);			// Az elozo alkalmazas ablakhoz kepest hol tunik fel
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);	// 8 bites R,G,B,A + dupla buffer + melyseg buffer

    glutCreateWindow("Grafika hazi feladat");		// Alkalmazas ablak megszuletik es megjelenik a kepernyon

    glMatrixMode(GL_MODELVIEW);				// A MODELVIEW transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);			// A PROJECTION transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();

    onInitialization();					// Az altalad irt inicializalast lefuttatjuk

    glutDisplayFunc(onDisplay);				// Esemenykezelok regisztralasa
    glutMouseFunc(onMouse);
    glutIdleFunc(onIdle);
    glutKeyboardFunc(onKeyboard);
    glutKeyboardUpFunc(onKeyboardUp);
    glutMotionFunc(onMouseMotion);

    glutMainLoop();					// Esemenykezelo hurok

    return 0;
}
