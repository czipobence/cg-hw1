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

const float EPSILON = 0.001;

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
   bool operator==(const Vector& v) {
	return (*this - v).Length() < EPSILON;
   }
   
   float Length() { return sqrt(x * x + y * y + z * z); }
   float Dist(Vector v) { return (*this - v).Length(); }
   Vector norm() {return *this * (1 / this->Length()); }
   static Vector null() {return Vector (0,0);}
};

struct Line {
	Vector a;
	Vector dir;
	Line(Vector l1, Vector l2) : a(l1) {
		dir = l1 - l2;
		dir = dir.norm();
	}
	
	Line() : a(Vector::null()), dir (Vector::null()) {}
	
	float getYForX(float x) {
		float lambda = (dir.x == 0) ? 0 : (x - a.x) / (dir.x);
		return a.y + dir.y * lambda;
	}
	
	float dist (Vector v) {
		Vector d = (a  - v) - dir * ((a - v) * dir);
		return d.Length();
	}
	
	Vector getNormal() {
		return Vector(dir.y, -1 * dir.x);
	}
	
};

struct Parabola {
	Line l;
	Vector f;
	
	Parabola (Vector l1, Vector l2, Vector f) : l(l1,l2), f(f) {
		if ( ((f - l.a) % l.dir).z < 0 ) {
			l.dir = l.dir * -1;
		}
	}
	Parabola () : l(Vector::null(), Vector::null()), f(Vector::null()) {}
	
	bool in(Vector v) { return l.dist(v) > f.Dist(v);}
	
	float df() {
		return l.dist(f) / 2;
	}
	
	Vector getDerived (float t) {
		return l.dir + l.getNormal() * (t / 2 / df());
	}
	
	float getParamForPoint(Vector v) {
		Vector f_vet = l.dir * ((f - l.a) * l.dir) + l.a;
		Vector to_point = l.dir * ((f_vet - v) * l.dir);
		
		float param = to_point.Length();
		if ( (to_point % (f - f_vet)).z < 0) {
			param *= -1;
		}
		
		return param;
	}
	
	Vector getDerivedAtPoint (Vector v) {
		return getDerived(getParamForPoint(v));
	}
	
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
const Color GREEN(0,1,0);

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 600;
const int WORLD_WIDTH = 1000;
const int WORLD_HEIGHT = 1000;


struct Hermite {
	Vector p0, p1, v0, v1;
	long t0, t1;
	
	Hermite() {
	p0 = p1 = v0 = v1 = Vector(0,0);
	t0 = t1 = 0;
	}
	
	Hermite(Vector p0, Vector p1, Vector v0, Vector v1, long t0, long t1)
	: p0(p0), p1(p1), v0(v0),v1(v1),t0(t0),t1(t1) {}
	
	Vector getVal(float t) {
		float dt1 = t - t0;
		float dt = t1 - t0;
		
		Vector a0 = p0;
		Vector a1 = v0;
		Vector a2 = (p1- p0) * (3.0 / (dt * dt)) - (v1 + v0 * 2.0) * (1.0 / dt) ;
		Vector a3 = (p0 - p1) * (2.0 / dt / dt / dt) + (v0 + v1) * (1.0 / dt / dt);
		return (a3 * dt1 * dt1 * dt1 + a2 * dt1 * dt1 + a1 * dt1 + a0); 
	}
	
	Vector getDerived(float t) {
		float dt1 = t - t0;
		float dt = t1 - t0;
		
		Vector a1 = v0;
		Vector a2 = (p1- p0) * (3.0 / (dt * dt)) - (v1 + v0 * 2.0) * (1.0 / dt) ;
		Vector a3 = (p0 - p1) * (2.0 / dt / dt / dt) + (v0 + v1) * (1.0 / dt / dt);
		return (a3 * dt1 * dt1 * 3 + a2 * dt1 * 2 + a1); 
	}
	
};


struct Camera {
	
	Vector offset;
	Vector zoom;
	
	Camera() {
		offset = Vector::null();
		zoom = Vector(1,1);
	}
	
	long convert_to_screen_x(float value) {
		return (long)(((value -offset.x) / WORLD_WIDTH) * SCREEN_WIDTH * zoom.x);
	}

	long convert_to_screen_y(float value) {
		return  value;
		return (long)((((value - offset.y) / WORLD_HEIGHT) * SCREEN_HEIGHT * zoom.y)) ;
	}
	
	Vector convert_to_screen(Vector v) {
		return Vector(convert_to_screen_x(v.x), convert_to_screen_y(v.y));
	}
	
	
	float convert_screen_x(float value) {
		return value / SCREEN_WIDTH * WORLD_WIDTH / zoom.x + offset.x;
	}
	
	float convert_screen_y(float value) {
		return (SCREEN_HEIGHT - value) / SCREEN_HEIGHT * WORLD_HEIGHT / zoom.y + offset.y;
	}
	
	Vector getTopRight() {
		return Vector(offset.x + WORLD_WIDTH / zoom.x, offset.y + WORLD_HEIGHT / zoom.y);
	}
	
	void drawCircle(Vector point, float radius, Color fill, Color border) {
		glColor3f(fill.r, fill.g, fill.b);
		
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
			converted = h.getVal(i);
			glVertex2f(converted.x, converted.y);
		}
		glEnd();
	}
	
	void drawLine(Line l, Color c) {
		glColor3f(c.r,c.g,c.b);
		glBegin(GL_LINES);
		if (l.dir.x != 0) {
			float tmp = l.getYForX(offset.x);
			glVertex2f(offset.x,tmp);
			tmp = l.getYForX(offset.x + WORLD_WIDTH / zoom.x);
			glVertex2f(offset.x + WORLD_WIDTH / zoom.x,tmp);
		} else {
			glVertex2f(l.a.x,offset.y);
			glVertex2f(l.a.x,offset.y + WORLD_HEIGHT/ zoom.y);
		}
		
		glEnd();
	}
	
};

Camera camera;

struct SplineElement {
	Vector pos;
	Vector vel;
	long time;
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
	
	void drawPoint() {
		camera.drawCircle(pos,RADIUS,RED,WHITE);
		if (next != NULL)
			next -> drawPoint();
	}
	
	void drawSpline() {
		camera.drawHermite(h,WHITE);
		if (next != NULL)
			next -> drawSpline();
	}
	
	void recalculateVelocity(SplineElement* first, SplineElement* last, long points) {
		SplineElement *n,*p;
		long dt1, dt2;

		if (prev == NULL) {
			p = last;
			dt2 = (points == 0) ? 0 : (last -> time - first -> time)/ points;
		} else {
			p = prev;
			dt2 = time - p-> time;
		}
		
		if (next == NULL) {
			n = first;
			dt1 = (points == 0) ? 0 : (last -> time - first -> time)/ points;
		} else {
			n = next;
			dt1 = n-> time - time;
		}
		
		
		vel = (n -> pos - pos) * (0.5 / dt1) + (pos - p -> pos) * (0.5 / dt2);
	}
	
	void recalculateHermite(SplineElement *first, long points) {
		h.v0 = vel;
		h.t0 = time;
		h.p0 = pos;
		if (next != NULL) {
			h.p1 = next->pos;
			h.t1 = next->time;
			h.v1 = next -> vel;
		} else {
			h.p1 = first->pos;
			h.t1 = (points == 0) ? time : time + (time - first -> time) / points; 
			h.v1 = first -> vel;
	
		}
	}
	
	~SplineElement() {
		delete next;
	}
	
};

struct Spline{
	SplineElement *first;
	SplineElement *last;
	long points;
	Spline() {
		first = NULL;
		last = NULL;
		points = 0;
	}

	void add(Vector v, long t) {
		SplineElement *spe = new SplineElement(v,t);
		if (first == NULL) {
			first = last = spe;
			first -> recalculateVelocity(first,last,points);
			first -> recalculateHermite(first, points);
		} else {
			last -> next = spe;
			spe -> prev = last;
			last = spe;
			
			spe -> prev -> recalculateVelocity(first,last, points);
			spe -> recalculateVelocity(first,last, points);
			first -> recalculateVelocity(first,last, points);
			
			if (spe->prev -> prev != NULL) {
				spe -> prev -> prev -> recalculateHermite(first, points);
			}
			spe -> prev -> recalculateHermite(first, points);
			spe -> recalculateHermite(first, points);
			first -> recalculateHermite(first, points);
		}
		points++;
	}

	void draw() {		
		if (first != NULL) {
			first->drawSpline();
			first->drawPoint();
		}
	}

	~Spline() {
		delete first;
	}
};

struct Drawings {
	Spline mySpline;
	Parabola parabola;
	Color image[SCREEN_WIDTH*SCREEN_HEIGHT];
	Vector intersectionPoint;
	float intersectionTimeParam;
	Line cmsTangential, parabolaTangential;
	Hermite h;
	
	float getIntersection() {
		float t_start, t_end, t_mid;
		
		t_start = h.t0;
		t_end = h.t1;
		
		while (!( camera.convert_to_screen(h.getVal(t_end)) == camera.convert_to_screen(h.getVal(t_start))) && ((t_end - t_start) > EPSILON)) {
			t_mid = (t_start + t_end) / 2.0;
			if (parabola.in(h.getVal(t_start)) == parabola.in(h.getVal(t_mid))) {
				t_start = t_mid;
			} else {
				t_end = t_mid;
			}
		}
		
		return t_mid;
	
	}
	
	void fillImage() {
		for(int Y = 0; Y < SCREEN_HEIGHT; Y++) {
			for(int X = 0; X < SCREEN_WIDTH; X++) {
				Vector v(camera.convert_screen_x(X),camera.convert_screen_y(SCREEN_HEIGHT - Y));
				image[Y * SCREEN_HEIGHT + X] = (parabola.in(v)) ? YELLOW : CYAN;
			}
		}
	}
	
	void draw() {
		if (mySpline.points == 3)
			parabola = Parabola(mySpline.first -> pos, mySpline.first -> next -> pos, mySpline.first -> next -> next -> pos);
		
		if (mySpline.points > 2) {
			fillImage();
			
			h = mySpline.first->next->h;
			intersectionTimeParam = getIntersection();
			intersectionPoint = h.getVal(intersectionTimeParam);
			
			cmsTangential = Line(intersectionPoint, h.getDerived(intersectionTimeParam) + intersectionPoint);
			parabolaTangential = Line(intersectionPoint, parabola.getDerivedAtPoint(intersectionPoint) + intersectionPoint);
		}
		
		glDrawPixels(SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB, GL_FLOAT, image);
   
		mySpline.draw();

		if (mySpline.points > 2) {
			camera.drawLine(cmsTangential,GREEN);
			camera.drawLine(parabolaTangential, GREEN);
		}
		
	}
};

Drawings d;

struct Animation {
	Vector animationSpeed;
	bool started;
	long lastStep;
	static const long INTERVAL = 1000;
	
	Animation() : animationSpeed(2,3), started(false), lastStep(0) {}
	
	void step() {
		if (!started) return;
		camera.offset = camera.offset + animationSpeed;
		Vector tr = camera.getTopRight();
		if (tr.x > WORLD_WIDTH) {
			camera.offset.x -= tr.x - WORLD_WIDTH;
			animationSpeed.x *= -1;
		}
		if (tr.y > WORLD_HEIGHT) {
			camera.offset.y -= tr.y - WORLD_HEIGHT;
			animationSpeed.y *= -1;
		}
		if (camera.offset.x < 0) {
			camera.offset.x = 0;
			animationSpeed.x *= -1;
		}
		if (camera.offset.y < 0) {
			camera.offset.y = 0;
			animationSpeed.y *= -1;
		}
		
		lastStep = glutGet(GLUT_ELAPSED_TIME);
		
	}
};

Animation anim;

void onInitialization( ) { 
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    for(int Y = 0; Y < SCREEN_HEIGHT; Y++)
		for(int X = 0; X < SCREEN_WIDTH; X++)
			d.image[Y*SCREEN_WIDTH + X] = CYAN;
}

void onDisplay( ) {
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION); 
    glLoadIdentity();
    gluOrtho2D(camera.offset.x,camera.offset.x + WORLD_WIDTH / camera.zoom.x,camera.offset.y,camera.offset.y + WORLD_HEIGHT/camera.zoom.y);

	d.draw();

    glutSwapBuffers();     			

}

void onKeyboard(unsigned char key, int x, int y) {
    if (key == 'd') glutPostRedisplay( ); 
    if (key == ' ' && !anim.started) {
		camera.zoom.x = camera.zoom.y = 2;
		camera.offset.x = WORLD_WIDTH /2 - WORLD_WIDTH / camera.zoom.x / 2; 
		camera.offset.y = WORLD_HEIGHT /2 - WORLD_HEIGHT / camera.zoom.y / 2;
		anim.started = true;
		glutPostRedisplay( );
	}
}

void onKeyboardUp(unsigned char key, int x, int y) {

}

void onMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (!anim.started) {
			float wx = camera.convert_screen_x(x);
			float wy = camera.convert_screen_y(y);
			long time = glutGet(GLUT_ELAPSED_TIME);
			d.mySpline.add(Vector(wx,wy), time);
			glutPostRedisplay( );
		}
	}
}

void onMouseMotion(int x, int y)
{

}

void onIdle( ) {
    long time = glutGet(GLUT_ELAPSED_TIME);
    if (((time - anim.lastStep) > anim.INTERVAL) && anim.started) {
		anim.step();
		glutPostRedisplay( );
	} 
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
