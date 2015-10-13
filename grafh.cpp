//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2014-tol.          
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk. 
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat. 
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni (printf is fajlmuvelet!)
// - new operatort hivni az onInitialization fĂźggvĂŠnyt kivĂŠve, a lefoglalt adat korrekt felszabadĂ­tĂĄsa nĂŠlkĂźl 
// - felesleges programsorokat a beadott programban hagyni
// - tovabbi kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan gl/glu/glut fuggvenyek hasznalhatok, amelyek
// 1. Az oran a feladatkiadasig elhangzottak ES (logikai AND muvelet)
// 2. Az alabbi listaban szerepelnek:  
// Rendering pass: glBegin, glVertex[2|3]f, glColor3f, glNormal3f, glTexCoord2f, glEnd, glDrawPixels
// Transzformaciok: glViewport, glMatrixMode, glLoadIdentity, glMultMatrixf, gluOrtho2D, 
// glTranslatef, glRotatef, glScalef, gluLookAt, gluPerspective, glPushMatrix, glPopMatrix,
// Illuminacio: glMaterialfv, glMaterialfv, glMaterialf, glLightfv
// Texturazas: glGenTextures, glBindTexture, glTexParameteri, glTexImage2D, glTexEnvi, 
// Pipeline vezerles: glShadeModel, glEnable/Disable a kovetkezokre:
// GL_LIGHTING, GL_NORMALIZE, GL_DEPTH_TEST, GL_CULL_FACE, GL_TEXTURE_2D, GL_BLEND, GL_LIGHT[0..7]
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : <VEZETEKNEV(EK)> <KERESZTNEV(EK)>
// Neptun : <NEPTUN KOD>
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy 
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem. 
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a 
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb 
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem, 
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.  
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat 
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================
#include <iostream>
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


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...

//--------------------------------------------------------
// 3D Vektor
//--------------------------------------------------------
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
   float operator*(const Vector& v) { 	// dot product
	return (x * v.x + y * v.y + z * v.z); 
   }
   Vector operator%(const Vector& v) { 	// cross product
	return Vector(y*v.z-z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
   }
   float Length() { return sqrt(x * x + y * y + z * z); }
};
 
//--------------------------------------------------------
// Spektrum illetve szin
//--------------------------------------------------------
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

const Color RED (1,0,0);
const Color YELLOW (1,1,0);
const Color WHITE(1,1,1);
const Color CYAN(0,1,1);


const int screenWidth = 600;	// alkalmazĂĄs ablak felbontĂĄsa
const int screenHeight = 600;
const int worldWidth = 1000;
const int worldHeight = 1000;

struct Hermite {
	Vector p0, p1, v0, v1;
	long t0, t1;
	
	Hermite() {
	p0 = p1 = v0 = v1 = Vector(0,0);
	t0 = t1 = 0;
	}
	
	Hermite(Vector p0, Vector p1, Vector v0, Vector v1, long t0, long t1)
	: p0(p0), p1(p1), v0(v0),v1(v1),t0(t0),t1(t1) {}
	
	Vector getVal(long t) {
		long dt1 = t - t0;
		long dt = t1 - t0;
		
		Vector a0 = p0;
		Vector a1 = v0;
		Vector a2 = (p1- p0) * (3.0 / (dt * dt)) - (v1 + v0 * 2.0) * (1.0 / dt) ;
		Vector a3 = (p0 - p1) * (2.0 / dt / dt / dt) + (v0 + v1) * (1.0 / dt / dt);
		return (a3 * dt1 * dt1 * dt1 + a2 * dt1 * dt1 + a1 * dt1 + a0); 
	}
	
};


struct Camera {
	
	int xOffset;
	int yOffset;
	float xZoom;
	float yZoom;
	
	Camera() {
		xOffset = 0;
		yOffset = 0;
		xZoom = 1;
		yZoom = 1;
	}
	
	float convert_to_screen_x(float value) {
		return ((value - xOffset) / worldWidth) * 2 * xZoom - xZoom;
	}

	float convert_to_screen_y(float value) {
		return (((value - yOffset) / worldHeight) * 2 * yZoom - yZoom) * (-1);
	}
	
	Vector convert_to_screen(Vector v) {
		return Vector(convert_to_screen_x(v.x), convert_to_screen_y(v.y));
	}
	
	float convert_to_screen(float l) {
			return l / 1000 * 2 * xZoom;
	}
	
	float convert_screen_x(float value) {
		return value / screenWidth * worldWidth * xZoom + xOffset;
	}
	
	float convert_screen_y(float value) {
		return value / screenHeight * worldHeight * yZoom + yOffset;
	}
	
	
	void drawCircle(Vector point, float radius, Color fill, Color border) {
		glColor3f(fill.r, fill.g, fill.b);
		point = convert_to_screen(point);
		radius = convert_to_screen(radius);
		glBegin(GL_POLYGON);
            for(float i = 0; i <= 2*M_PI; i+=0.1)
				glVertex2f(point.x + radius * cos(i),point.y + radius * sin(i));
		glEnd();
		
		glColor3f(border.r,  border.g, border.b);
		glBegin(GL_LINE_LOOP);
            for(float i = 0; i <= 2*M_PI; i+=0.1)
				glVertex2f(point.x + radius * cos(i),point.y + radius * sin(i));
		glEnd();
	}
	
	void drawHermite(Hermite h, Color line) {
		glColor3f(line.r,line.g,line.b);
		glBegin(GL_LINE_STRIP);
		Vector converted;
		for (long i = h.t0; i < h.t1; i++) {
			converted = convert_to_screen(h.getVal(i));
			glVertex2f(converted.x, converted.y);
		}
		
		glEnd();
	}
	
};

Camera camera;

struct SplineElement {
	Vector pos;
	Vector vel;
	long time; //ago in a galaxy far far away
	static const float RADIUS = 5;
	SplineElement *next;
	SplineElement *prev;
	Hermite h;
	
	SplineElement(Vector v, long t) {
		pos = v;
		time = t;
		next = NULL;
		prev = NULL;
		h = Hermite();
		vel = Vector(0,0);
	}
	
	void drawElement() {
		camera.drawCircle(pos,RADIUS,RED,WHITE);
		camera.drawHermite(h,WHITE);
		std::cout << "DRAW x:" << pos.x << " Y:" << pos.y << " v: "<< vel.x << ", " << vel.y << std::endl;
		//std::cout << "HERM P0: " << h.p0.x << ", " << h.p0.y << " t0:" << h.t0 << " V0: " << h.v0.x << ", " << h.v0.y <<
		//" P1: " << h.p1.x << ", " << h.p1.y << " t1:" << h.t1 << " V1: " << h.v1.x << ", " << h.v1.y << std::endl;
		std::cout << h.getVal(h.t1).x << "AAAND" << h.p1.x << std::endl;
		if (next != NULL)
			next -> drawElement();
	}
	
	void recalculateVelocity() {
		if ((prev != NULL) & (next != NULL)) {
			vel = (next -> pos - pos) * (0.5 / (next -> time - time)) +
			(pos - prev -> pos) * (0.5 / (time - prev -> time));
		} else {
			vel = Vector(0,0);
		}
	}
	
	void recalculateHermite() {
		if (next != NULL) {
			h.p0 = pos;
			h.p1 = next->pos;
			h.t0 = time;
			h.t1 = next->time;
			h.v0 = vel;
			h.v1 = next -> vel;
		}
	}
	
	~SplineElement() {
		delete next;
	}
	
};

struct Spline{
	SplineElement *first;
	SplineElement *last;
	int points;
	Spline() {
		first = NULL;
		last = NULL;
		points = 0;
	}

	void add(Vector v, long t) {
		SplineElement *spe = new SplineElement(v,t);
		if (first == NULL) {
			first = last = spe;
		} else {
			last -> next = spe;
			spe -> prev = last;
			last = spe;
			spe -> prev -> recalculateVelocity();
			if (spe->prev -> prev != NULL) {
				spe -> prev -> prev -> recalculateHermite();
			}
			spe -> prev -> recalculateHermite();
			
		}
		points++;
	}

	void draw() {
		
		SplineElement* moving = first;
		
		while (moving != NULL) {
			
			moving = moving -> next;
		}
		glEnd();
		
		if (first != NULL)
			first->drawElement();
	}

	~Spline() {
		delete first;
	}
};

Color image[screenWidth*screenHeight];	// egy alkalmazĂĄs ablaknyi kĂŠp

Spline mySpline;

// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization( ) { 
	glViewport(0, 0, screenWidth, screenHeight);

    // Peldakent keszitunk egy kepet az operativ memoriaba
    for(int Y = 0; Y < screenHeight; Y++)
		for(int X = 0; X < screenWidth; X++)
			image[Y*screenWidth + X] = Color((float)X/screenHeight,(float)Y/screenWidth,0);

	

}

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay( ) {
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);		// torlesi szin beallitasa
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles

    // ..

    // Peldakent atmasoljuk a kepet a rasztertarba
    glDrawPixels(screenWidth, screenHeight, GL_RGB, GL_FLOAT, image);
    // Majd rajzolunk egy kek haromszoget
	glColor3f(0,0,1);

	mySpline.draw();
    // ...

    glutSwapBuffers();     				// Buffercsere: rajzolas vege

}

// Billentyuzet esemenyeket lekezelo fuggveny (lenyomas)
void onKeyboard(unsigned char key, int x, int y) {
    if (key == 'd') glutPostRedisplay( ); 		// d beture rajzold ujra a kepet

}

// Billentyuzet esemenyeket lekezelo fuggveny (felengedes)
void onKeyboardUp(unsigned char key, int x, int y) {

}

// Eger esemenyeket lekezelo fuggveny
void onMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		float wx = camera.convert_screen_x(x);
		float wy = camera.convert_screen_y(y);
		long time = glutGet(GLUT_ELAPSED_TIME);
		mySpline.add(Vector(wx,wy), time);
		std::cout << wx << ", " << wy << ", " << time << std::endl;
		glutPostRedisplay( );
	}
}

// Eger mozgast lekezelo fuggveny
void onMouseMotion(int x, int y)
{

}

// `Idle' esemenykezelo, jelzi, hogy az ido telik, az Idle esemenyek frekvenciajara csak a 0 a garantalt minimalis ertek
void onIdle( ) {
    // long time = glutGet(GLUT_ELAPSED_TIME);		// program inditasa ota eltelt ido

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
