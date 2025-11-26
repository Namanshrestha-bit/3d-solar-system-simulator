// Compile command (adjust paths as needed):
// g++ "C:\Users\NAMAN\OneDrive\Desktop\gvc project 2025\solar.cpp" ^
//  -I"C:\library\freeglut\include" ^
//  -L"C:\library\freeglut\lib" ^
//  -lfreeglut -lopengl32 -lglu32 -lwinmm -lgdi32 ^
//  -o solar.exe

#include <GL/freeglut.h>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <iostream>

// ---------------- Config ----------------
const float PI_F = 3.14159265358979323846f;
const int WIN_W = 1200, WIN_H = 800;

// ---------------- Structures ----------------
struct Planet {
    float distance;      // orbital semi-major radius (visual)
    float radius;        // display radius
    float orbitSpeed;    // base speed (deg/sec)
    float rotationSpeed; // self-rotation (deg/sec)
    float orbitAngle;    // current angle (deg)
    float selfAngle;     // self rotation (deg)
    float tilt;          // axial tilt (deg)
    float ecc;           // eccentricity (0=circle)
    float r,g,b;         // color
    // trail storage (for orbit trails)
    std::vector<float> trail; // x,z pairs
};

struct Comet {
    bool active;
    float x,y,z;
    float vx,vy,vz;
    float life; // seconds
};

struct Asteroid {
    float x,y,z;
    float sx; // scale
};

// ---------------- Globals ----------------
std::vector<Planet> planets;
std::vector<Asteroid> asteroids;
Comet comet;
std::vector<float> stars; // x,y,z per star

// Camera / modes
bool flyMode = false;
bool cinematicMode = true;
bool trailsOn = true;
bool autoCamera = true;

float camX=0, camY=10, camZ=60;
float camYaw = 180.0f, camPitch = -10.0f;
float camSpeed = 12.0f;
bool keyW=false,keyA=false,keyS=false,keyD=false,keyQ=false,keyE=false,boost=false;
int lastMouseX=0, lastMouseY=0; bool mouseDown=false;

float cinematicAngle = 0.0f;
float cinematicRadius = 60.0f;
float cinematicTilt = 15.0f;
bool cinematicActive = true;

// Time
int lastTime = 0;

// ---------------- Utility ----------------
float rnd(float a, float b){ return a + (b-a)*(rand()/(float)RAND_MAX); }

// ---------------- Init functions ----------------
void initPlanets() {
    planets.clear();
    // Mercury-like
    planets.push_back({ 6.0f, 0.35f, 48.0f,  40.0f, rnd(0,360), rnd(0,360), 2.0f, 0.02f, 0.7f,0.7f,0.7f });
    // Venus-like
    planets.push_back({ 9.0f, 0.55f, 34.0f,  30.0f, rnd(0,360), rnd(0,360), 3.5f, 0.03f, 0.9f,0.7f,0.2f });
    // Earth-like
    planets.push_back({ 12.0f,0.6f, 24.0f,  80.0f, rnd(0,360), rnd(0,360), 23.5f, 0.01f, 0.2f,0.5f,0.9f });
    // Mars-like
    planets.push_back({ 15.5f,0.45f,18.0f,  35.0f, rnd(0,360), rnd(0,360), 25.0f, 0.04f, 0.9f,0.3f,0.2f });
    // Jupiter-like
    planets.push_back({ 20.5f,1.4f, 12.0f,  12.0f, rnd(0,360), rnd(0,360), 3.0f, 0.02f, 0.9f,0.6f,0.3f });
    // Saturn-like
    planets.push_back({ 25.5f,1.0f, 9.0f,   10.0f, rnd(0,360), rnd(0,360), 26.7f, 0.02f, 0.9f,0.8f,0.5f });
    // Uranus-like
    planets.push_back({ 30.5f,0.9f, 7.5f,   9.0f,  rnd(0,360), rnd(0,360), 97.8f, 0.01f, 0.4f,0.8f,0.9f });
    // Neptune-like
    planets.push_back({ 35.5f,0.85f,6.5f,   8.0f,  rnd(0,360), rnd(0,360), 28.3f, 0.02f, 0.3f,0.5f,0.8f });

    // initialize trails small capacity
    for(auto &p : planets) p.trail.reserve(1024);
}

void initStars(int n=900) {
    stars.clear(); stars.reserve(n*3);
    for(int i=0;i<n;i++){
        float x = rnd(-300,300), y = rnd(-200,200), z = rnd(-300,300);
        stars.push_back(x); stars.push_back(y); stars.push_back(z);
    }
}

void initAsteroids(int count=400) {
    asteroids.clear(); asteroids.reserve(count);
    for(int i=0;i<count;i++){
        float r = rnd(17.5f, 22.0f);
        float a = rnd(0, 2*PI_F);
        float h = rnd(-1.2f, 1.2f);
        Asteroid aobj; aobj.x = r*cosf(a); aobj.z = r*sinf(a); aobj.y = h; aobj.sx = rnd(0.03f,0.12f);
        asteroids.push_back(aobj);
    }
}

// Comet init
void initComet() {
    comet.active=false; comet.life=0;
    comet.x = -150; comet.y = rnd(-20,40); comet.z = -150;
    comet.vx = rnd(1.5f,4.5f); comet.vy = rnd(-0.1f,0.2f); comet.vz = rnd(1.5f,4.5f);
}

// ---------------- Drawing helpers ----------------
void drawStars() {
    glDisable(GL_LIGHTING);
    glPointSize(1.2f);
    glBegin(GL_POINTS);
    for(size_t i=0;i<stars.size(); i+=3){
        float tw = 0.8f + 0.2f * (rand()/(float)RAND_MAX);
        glColor3f(tw,tw,tw);
        glVertex3f(stars[i], stars[i+1], stars[i+2]);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

// draw glowing sun (A,B)
void drawSun(){
    // core
    glPushMatrix();
      GLfloat mat_em[4] = {1.0f,0.92f,0.6f,1.0f};
      glMaterialfv(GL_FRONT, GL_EMISSION, mat_em);
      glColor3f(1.0f,0.9f,0.2f);
      glutSolidSphere(3.2, 36,36);
      GLfloat no_em[4] = {0,0,0,1};
      glMaterialfv(GL_FRONT, GL_EMISSION, no_em);
    glPopMatrix();
    // outer glow
    glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE);
      glColor4f(1.0f,0.6f,0.1f,0.25f);
      glutSolidSphere(4.2,32,32);
    glDisable(GL_BLEND);
}

// draw planet with lighting and optional trail
void drawPlanet(const Planet &p, bool drawTrail=true){
    // compute position using eccentricity (slightly elliptical)
    float ang = p.orbitAngle * PI_F/180.0f;
    float a = p.distance;
    float b = a * (1.0f - p.ecc); // semiminor approx
    float x = a * cosf(ang);
    float z = b * sinf(ang);
    // add to trail
    if(drawTrail && trailsOn) {
        // push x,z pairs
        // limit trail length
    }
    // position & draw
    glPushMatrix();
      glTranslatef(x, 0.0f, z);
      // axial tilt
      glRotatef(p.tilt, 0,0,1);
      // self-rotation
      glRotatef(p.selfAngle, 0,1,0);
      // material color - slight spec for shiny
      GLfloat mat_diff[] = { p.r*0.8f, p.g*0.8f, p.b*0.8f, 1.0f};
      GLfloat mat_amb[]  = { p.r*0.2f, p.g*0.2f, p.b*0.2f, 1.0f};
      glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diff);
      glMaterialfv(GL_FRONT, GL_AMBIENT, mat_amb);
      glutSolidSphere(p.radius, 28,28);
    glPopMatrix();
}

// draw orbit trail for a planet
void drawOrbitTrail(const Planet &p) {
    if(!trailsOn) return;
    glDisable(GL_LIGHTING);
    glBegin(GL_LINE_STRIP);
    glColor3f(p.r*0.8f, p.g*0.8f, p.b*0.8f);
    int steps = 160;
    float a = p.distance; float b = a*(1.0f-p.ecc);
    for(int i=0;i<=steps;i++){
        float ang = (i/(float)steps) * 2*PI_F;
        float x = a * cosf(ang);
        float z = b * sinf(ang);
        glVertex3f(x,0,z);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

// draw asteroid belt
void drawAsteroids(){
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glColor3f(0.7f,0.7f,0.6f);
    for(auto &a : asteroids){
        glPushMatrix();
        glTranslatef(a.x, a.y, a.z);
        glutSolidSphere(a.sx,8,8);
        glPopMatrix();
    }
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

// draw comet
void drawComet(){
    if(!comet.active) return;
    glPushMatrix();
      glTranslatef(comet.x, comet.y, comet.z);
      glDisable(GL_LIGHTING);
      glColor3f(1.0f,0.9f,0.6f);
      glutSolidSphere(0.55f, 12,12);
      // tail (billboard-ish)
      glBegin(GL_TRIANGLES);
         glColor4f(1,0.9,0.6,0.6f);
         glVertex3f(-1.0f,0.0f,-1.0f);
         glVertex3f(-4.0f,0.4f,-4.0f);
         glVertex3f(-1.0f,0.0f,1.0f);
      glEnd();
      glEnable(GL_LIGHTING);
    glPopMatrix();
}

// draw Saturn ring (improved)
void drawRing(const Planet &p){
    float ang = p.orbitAngle * PI_F/180.0f;
    float x = p.distance * cosf(ang);
    float z = p.distance * sinf(ang);
    glPushMatrix();
      glTranslatef(x,0,z);
      glRotatef(20.0f, 0,0,1); // tilt
      glDisable(GL_LIGHTING);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glColor4f(0.9f,0.85f,0.7f,0.6f);
      // draw many strips for soft ring
      int slices = 200;
      float inner = p.radius*1.6f, outer = p.radius*4.0f;
      glBegin(GL_TRIANGLE_STRIP);
      for(int i=0;i<=slices;i++){
          float a = (i/(float)slices) * 2*PI_F;
          glVertex3f(inner*cosf(a), 0.0f, inner*sinf(a));
          glVertex3f(outer*cosf(a), 0.0f, outer*sinf(a));
      }
      glEnd();
      glDisable(GL_BLEND);
      glEnable(GL_LIGHTING);
    glPopMatrix();
}

// draw planet trails (simple fading)
void drawPlanetTrails(){
    if(!trailsOn) return;
    glDisable(GL_LIGHTING);
    for(auto &p : planets){
        glBegin(GL_LINE_STRIP);
        // faint colored line
        glColor4f(p.r*0.9f, p.g*0.9f, p.b*0.9f, 0.6f);
        int steps=160;
        float a = p.distance; float b = a*(1.0f-p.ecc);
        for(int i=0;i<=steps;i++){
            float ang = (i/(float)steps) * 2*PI_F;
            float x = a * cosf(ang), z = b * sinf(ang);
            glVertex3f(x,0,z);
        }
        glEnd();
    }
    glEnable(GL_LIGHTING);
}

// ---------------- Lighting ----------------
void setupLights(){
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    // Sun light in origin
    GLfloat pos[] = {0,0,0,1.0f};
    GLfloat amb[] = {0.06f,0.06f,0.06f,1.0f};
    GLfloat diff[] = {1.0f,0.95f,0.85f,1.0f};
    GLfloat spec[] = {1.0f,1.0f,1.0f,1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);
    glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
    // slight hemisphere ambient imitation using global ambient
    GLfloat globAmb[] = {0.02f,0.02f,0.03f,1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

// ---------------- Camera updates ----------------
void updateCamera(float dt){
    if(cinematicMode){
        // auto orbit camera around origin
        cinematicAngle += 6.0f * dt; // deg/sec
        float rad = cinematicAngle * PI_F / 180.0f;
        float r = cinematicRadius;
        camX = r * cosf(rad);
        camZ = r * sinf(rad);
        camY = 10.0f + 6.0f * sinf(rad*0.4f);
        camYaw = 180.0f - cinematicAngle;
        camPitch = cinematicTilt + 4.0f * sinf(rad*0.5f);
    } else if(flyMode){
        // classical free-fly
        float speed = camSpeed * (boost?3.0f:1.0f) * dt;
        float yRad = camYaw * PI_F/180.0f;
        float pRad = camPitch * PI_F/180.0f;
        float fx = cosf(pRad)*sinf(yRad);
        float fy = sinf(pRad);
        float fz = cosf(pRad)*cosf(yRad);
        float rx = sinf(yRad-PI_F/2.0f);
        float rz = cosf(yRad-PI_F/2.0f);
        if(keyW){ camX += fx*speed; camY += fy*speed; camZ += fz*speed;}
        if(keyS){ camX -= fx*speed; camY -= fy*speed; camZ -= fz*speed;}
        if(keyA){ camX -= rx*speed; camZ -= rz*speed;}
        if(keyD){ camX += rx*speed; camZ += rz*speed;}
        if(keyQ){ camY -= speed; }
        if(keyE){ camY += speed; }
    } else {
        // gentle idle bob when neither cinematic nor fly
        float t = glutGet(GLUT_ELAPSED_TIME)/1000.0f;
        camX = 0.0f + 1.5f*sinf(t*0.25f);
        camZ = cinematicRadius - 6.0f + 3.0f * cosf(t*0.12f);
        camY = 10.0f + 1.5f*sinf(t*0.45f);
        camYaw += 0.01f;
    }
}

// ---------------- Render loop ----------------
void renderScene(){
    int now = glutGet(GLUT_ELAPSED_TIME);
    float dt = (lastTime>0) ? (now-lastTime)/1000.0f : 0.016f;
    lastTime = now;

    // advance physics
    for(auto &p : planets){
        p.orbitAngle += p.orbitSpeed * dt;
        if(p.orbitAngle >= 360.0f) p.orbitAngle -= 360.0f;
        p.selfAngle += p.rotationSpeed * dt;
        if(p.selfAngle >= 360.0f) p.selfAngle -= 360.0f;
    }

    // comet
    if(comet.active){
        comet.x += comet.vx * dt;
        comet.y += comet.vy * dt;
        comet.z += comet.vz * dt;
        comet.life -= dt;
        if(comet.life <= 0) comet.active=false;
    }

    // rotate asteroid belt
    for(auto &a : asteroids){
        float theta = atan2f(a.z, a.x);
        float r = sqrtf(a.x*a.x + a.z*a.z);
        theta += 0.02f * dt; // slow drift
        a.x = r * cosf(theta);
        a.z = r * sinf(theta);
    }

    // camera update
    updateCamera(dt);

    // clear
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // compute camera look
    float yawRad = camYaw * PI_F/180.0f;
    float pitchRad = camPitch * PI_F/180.0f;
    float fx = cosf(pitchRad)*sinf(yawRad);
    float fy = sinf(pitchRad);
    float fz = cosf(pitchRad)*cosf(yawRad);
    gluLookAt(camX, camY, camZ, camX+fx, camY+fy, camZ+fz, 0,1,0);

    // sky stars
    drawStars();

    // set lights (sun at origin)
    setupLights();

    // draw sun
    drawSun();

    // draw asteroid belt (between Mars and Jupiter)
    drawAsteroids();

    // draw planets (and rings / moon)
    for(size_t i=0;i<planets.size();++i){
        drawOrbitTrail(planets[i]);
    }
    for(size_t i=0;i<planets.size();++i){
        Planet &p = planets[i];
        // special: draw trail (simple line) already drawn above
        drawPlanet(p, true);
        // Saturn ring (planet index 5)
        if((int)i==5) drawRing(p);
    }

    // draw moon orbiting Earth (planet index 2)
    {
        Planet &earth = planets[2];
        float ang = earth.orbitAngle*PI_F/180.0f;
        float a = earth.distance, b = a*(1.0f-earth.ecc);
        float ex = a * cosf(ang), ez = b * sinf(ang);
        glPushMatrix();
          glTranslatef(ex, 0, ez);
          float moonAng = glutGet(GLUT_ELAPSED_TIME)*0.02f;
          float mx = 1.6f * cosf(moonAng), mz = 1.6f * sinf(moonAng);
          glTranslatef(mx,0,mz);
          glColor3f(0.85f,0.85f,0.85f);
          glutSolidSphere(0.19f, 16,16);
        glPopMatrix();
    }

    // comet
    drawComet();

    // space fog / dust (simple blended quads near camera)
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      float fogAlpha = 0.06f;
      glColor4f(0.6f,0.5f,0.7f,fogAlpha);
      // screen-space billboard-ish layers (cheap)
      glBegin(GL_QUADS);
        glVertex3f(camX-200, camY-30, camZ-200);
        glVertex3f(camX+200, camY-30, camZ-200);
        glVertex3f(camX+200, camY+30, camZ+200);
        glVertex3f(camX-200, camY+30, camZ+200);
      glEnd();
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);

    glutSwapBuffers();
}

// ---------------- Callbacks ----------------
void reshape(int w,int h){
    if(h==0) h=1;
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w/h, 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);
}

void keyboardDown(unsigned char key,int x,int y){
    if(key=='w'||key=='W') keyW=true;
    if(key=='s'||key=='S') keyS=true;
    if(key=='a'||key=='A') keyA=true;
    if(key=='d'||key=='D') keyD=true;
    if(key=='q'||key=='Q') keyQ=true;
    if(key=='e'||key=='E') keyE=true;
    if(key=='f'||key=='F') boost=!boost;
    if(key=='r'||key=='R'){ // reset camera and modes
        flyMode=false; cinematicMode=true; cinematicActive=true;
        camX=0;camY=10;camZ=60; camYaw=180; camPitch=-10; cinematicAngle=0;
        std::cout<<"Camera reset\n";
    }
    if(key=='c'||key=='C'){ cinematicMode = !cinematicMode; flyMode = !cinematicMode; std::cout<<"Cinematic mode: "<<cinematicMode<<"\n"; }
    if(key=='t'||key=='T'){ trailsOn = !trailsOn; std::cout<<"Trails: "<<trailsOn<<"\n"; }
    if(key==' '){ // space => spawn comet
        if(!comet.active){
           comet.active=true;
           comet.x = -180; comet.y = rnd(10,40); comet.z = rnd(-120, -40);
           comet.vx = rnd(80.0f,140.0f); comet.vy = rnd(-2.0f,2.0f); comet.vz = rnd(60.0f,120.0f);
           comet.life = 8.0f;
           std::cout<<"Comet launched\n";
        }
    }
    if(key=='+') cinematicRadius -= 3.0f;
    if(key=='-') cinematicRadius += 3.0f;
    if(key==27) exit(0);
}

void keyboardUp(unsigned char key,int x,int y){
    if(key=='w'||key=='W') keyW=false;
    if(key=='s'||key=='S') keyS=false;
    if(key=='a'||key=='A') keyA=false;
    if(key=='d'||key=='D') keyD=false;
    if(key=='q'||key=='Q') keyQ=false;
    if(key=='e'||key=='E') keyE=false;
}

void specialKeys(int key,int x,int y){
    const float STEP=3.0f;
    if(key==GLUT_KEY_LEFT) camYaw -= STEP;
    if(key==GLUT_KEY_RIGHT) camYaw += STEP;
    if(key==GLUT_KEY_UP) camPitch += STEP;
    if(key==GLUT_KEY_DOWN) camPitch -= STEP;
}

void mouseButton(int button,int state,int x,int y){
    if(button==GLUT_LEFT_BUTTON){
        mouseDown = (state==GLUT_DOWN);
        lastMouseX = x; lastMouseY = y;
    }
    // wheel handling: freeglut uses buttons 3/4
    if(button==3) { // wheel up
        cinematicRadius -= 2.0f; if(cinematicRadius<10) cinematicRadius=10;
    }
    if(button==4) { cinematicRadius += 2.0f; }
}

void mouseMotion(int x,int y){
    if(mouseDown){
        int dx = x - lastMouseX; int dy = y - lastMouseY;
        lastMouseX = x; lastMouseY = y;
        camYaw += dx * 0.12f;
        camPitch -= dy * 0.12f;
        if(camPitch>89) camPitch=89; if(camPitch<-89) camPitch=-89;
    }
}

// mouse passive (optional)
void passiveMouse(int x,int y){}

// Timer / Idle
void timerFunc(int v){
    glutPostRedisplay();
    glutTimerFunc(16, timerFunc, 0);
}

// ---------------- OpenGL Init ----------------
void initGL(){
    glClearColor(0.01f,0.01f,0.03f,1.0f);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    initPlanets();
    initStars();
    initAsteroids();
    initComet();

    // camera defaults
    camX=0; camY=10; camZ=60; camYaw=180; camPitch=-10;
}

// ---------------- Main ----------------
int main(int argc,char** argv){
    srand((unsigned int)time(NULL));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(WIN_W, WIN_H);
    glutCreateWindow("Enhanced Solar System (A..E) - FreeGLUT");

    initGL();

    glutDisplayFunc(renderScene);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(specialKeys);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    glutPassiveMotionFunc(passiveMouse);

    glutTimerFunc(16, timerFunc, 0);

    std::cout<<"Controls:\n W/A/S/D fly (when not cinematic)\n Q/E up/down\n F toggle boost\n C toggle cinematic\n T toggle trails\n SPACE launch comet\n +/- zoom cinematic\n R reset\n ESC exit\n";

    glutMainLoop();
    return 0;
}
