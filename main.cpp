//=============================================================================
// Sample Application: Lighting (Per Fragment Phong)
//=============================================================================

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "glApplication.h"
#include "glutWindow.h"
#include <iostream>
#include "glsl.h"
#include <time.h>
#include "glm.h"
#include <FreeImage.h> //*** Para Textura: Incluir librer�a

//-----------------------------------------------------------------------------


class myWindow : public cwc::glutWindow
{
protected:
   cwc::glShaderManager SM;
   cwc::glShader *shader;
   cwc::glShader* shader1; //Para Textura: variable para abrir los shader de textura
   GLuint ProgramObject;
   clock_t time0,time1;
   float timer010;  // timer counting 0->1->0
   bool bUp;        // flag if counting up or down.
   // Mallas.
   GLMmodel* ulisesmodel_ptr; // Malla de Ulises.
   GLMmodel* cottagemodel_ptr;
   GLfloat cottagetranslations[2][3] = { {0.001876, 0.589663, -0.303238},{-0.34413,0.589663,0.893932} };
   GLfloat cottagerotations[2][3] = { {0,90,0},{0,0,0} };
   GLMmodel* treemodel_ptr;
   GLfloat treetranslations[7][3] = { {-0.402844, 0.531671, -0.983344},{0.159757, 0.559211, -0.983344},{-0.909577, 0.531671, 0.603076},{-0.626222,0.531671,0.838723},{0.068849,0.540978,0.090903},{0.228312,0.540978,0.33748},{0.159757,0.540978,0.870821} };
   GLMmodel* boatmodel_ptr;
   GLMmodel* wheatmodel_ptr;
   GLfloat wheattranslations[5][3] = { {0,0.56248,0},{0.149474,0.56248,0},{0.298442,0.56248,0},{0.152543,0.585631,0.689361},{0.298442,0.585631,0.689361} };
   GLMmodel* portmodel_ptr;
   GLfloat porttranslations[2][3] = { {-0.730769,0.488000,0.14},{-0.58301,0.488000,0.16} };
   // No se pudo automatizar los ciclopes ya que todos tienen una armadura distina
   GLMmodel* cyclopmodel_ptr;
   GLMmodel* cyclopidlemodel_ptr;
   GLMmodel* cyclopsitmodel_ptr;
   GLMmodel* cyclopwalkingmodel_ptr;

   GLMmodel* terrainmodel_ptr;
   GLMmodel* oceanmodel_ptr;
   // Identificadores de texturas.
   GLuint ulisestexid; //*** Para Textura: variable que almacena el identificador de textura
   GLuint cottagetexid;
   GLuint treetexid;
   GLuint boattexid;
   GLuint wheattexid;
   GLuint porttexid;
   GLuint cycloptexid;
   GLuint terraintexid;
   GLuint oceantexid;
   

public:
	myWindow(){}
	/*
	* Método encargado de generar un identificador de textura.
	* @param textureID: El apuntador hacia el identificador de textura.
	* @return void.
	*/
	void generateIdentifier(GLuint& textureID) {
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexEnvi(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	/**
	* Método encargado de cargar una Textura cuando se tiene como imagen.
	*
	* @param fileroute: La ruta de acceso a la imagen.
	* @return void.
	*/
	void loadTexture(const char* fileroute) {
		
		// Loading JPG file
		FIBITMAP* bitmap = FreeImage_Load(
			FreeImage_GetFileType(fileroute, 0),
			fileroute);  //*** Para Textura: esta es la ruta en donde se encuentra la textura

		FIBITMAP* pImage = FreeImage_ConvertTo32Bits(bitmap);
		int nWidth = FreeImage_GetWidth(pImage);
		int nHeight = FreeImage_GetHeight(pImage);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, nWidth, nHeight,
			0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(pImage));

		FreeImage_Unload(pImage);
	}

	//*** Para Textura: aqui adiciono un m�todo que abre la textura en JPG
	void initialize_textures(void)
	{
		int w, h;
		GLubyte* data = 0;
		//data = glmReadPPM("soccer_ball_diffuse.ppm", &w, &h);
		//std::cout << "Read soccer_ball_diffuse.ppm, width = " << w << ", height = " << h << std::endl;

		//dib1 = loadImage("soccer_ball_diffuse.jpg"); //FreeImage
		
		// Todas las texturas correspondientes al modelo cottage.

		// Ulises Texture.
		generateIdentifier(ulisestexid);
		loadTexture("./Mallas/ulises.jpg");

		// Cottage Texture.
		generateIdentifier(cottagetexid);
		loadTexture("./Mallas/cottage.jpg");

		// Tree Texture.
		generateIdentifier(treetexid);
		loadTexture("./Mallas/tree.jpg");

		// Boat Texture.
		generateIdentifier(boattexid);
		loadTexture("./Mallas/boat.jpg");

		// Wheat Texture.
		generateIdentifier(wheattexid);
		loadTexture("./Mallas/FieldOfWheat_BaseColor.png");

		// Port Texture.
		generateIdentifier(porttexid);
		loadTexture("./Mallas/port.jpg");

		// Cyclop Texture.
		generateIdentifier(cycloptexid);
		loadTexture("./Mallas/cyclop.jpg");

		// Terrain Texture.
		generateIdentifier(terraintexid);
		loadTexture("./Mallas/terrain.jpg");

		// Ocean Texture.
		generateIdentifier(oceantexid);
		loadTexture("./Mallas/ocean.jpg");

		glEnable(GL_TEXTURE_2D);
	}

	/**
	* Método que genera la malla de acuerdo a los parámetros dados por el usuario.
	*
	* @param textureID: El identificador de la textura que tendrá la malla.
	* @param model: La malla que se desea importar.
	* @param x: Posición en x de la malla respecto al origen.
	* @param y: Posición en y de la malla respecto al origen.
	* @param z: Posición en z de la malla respecto al origen.
	* @param rx: Rotación en x de la malla respecto a su centro.
	* @param ry: Rotación en y de la malla respecto a su centro.
	* @param rz: Rotación en z de la malla respecto a su centro.
	* @param sx: Escalado en x de la malla respecto a su centro.
	* @param sy: Escalado en y de la malla respecto a su centro.
	* @param sz: Escalado en z de la malla respecto a su centro.
	* @return void.
	*/
	void generateMesh(GLuint textureID, GLMmodel* model, GLfloat x = 0, GLfloat y = 0, GLfloat z = 0, GLfloat rx = 0, GLfloat ry = 0, GLfloat rz = 0, GLfloat sx = 1, GLfloat sy = 1, GLfloat sz = 1) {
		glPushMatrix();
		glTranslatef(x, y, z);
		if (rx != 0) {
			glRotatef(rx, 1, 0, 0);
		}
		if (ry != 0) {
			glRotatef(ry, 0, 1, 0);
		}
		if (rz != 0) {
			glRotatef(rz, 0, 0, 1);
		}
		glScalef(sx, sy, sz);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glmDraw(model, GLM_SMOOTH | GLM_MATERIAL | GLM_TEXTURE);
		glPopMatrix();
	}

	// Variables para el control de la cámara
	float cameraX = 0;
	float cameraY = 0.52;
	float cameraZ = -1;
	float cameraYaw = 45;
	float cameraPitch = 0;
	int prevMouseX = 0;
	int prevMouseY = 0;

	virtual void OnRender(void)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
      //timer010 = 0.09; //for screenshot!

      glPushMatrix();
	  glTranslatef(cameraX, cameraY, cameraZ);  // Ajusta la posición de la cámara
	  glRotatef(cameraYaw, 1, 1, 0);            // Rotación horizontal (yaw)
	  glRotatef(cameraPitch, 1, 0, 0);          // Rotación vertical (pitch)

	  //glTranslatef(0, -0.52, -1);
	  //glRotatef(45, 1, 1, 0);

      if (shader) shader->begin();
	      //glutSolidTeapot(1.0);
      if (shader) shader->end();

	  //*** Para Textura: llamado al shader para objetos texturizados
	  if (shader1) shader1->begin();
		  generateMesh(ulisestexid, ulisesmodel_ptr, -0.730769, 0.538000, 0.14, 0, 0, 0, 0.050, 0.050, 0.050);
		  // Cottage Mesh
		  for (int i = 0; i < 2; i++) {
			  generateMesh(cottagetexid, cottagemodel_ptr, cottagetranslations[i][0], cottagetranslations[i][1], cottagetranslations[i][2], cottagerotations[i][0], cottagerotations[i][1], cottagerotations[i][2], 1, 1, 1);
		  }

		  // Tree Mesh.
		  for (int i = 0; i < 7; i++) {
			  generateMesh(treetexid, treemodel_ptr, treetranslations[i][0], treetranslations[i][1], treetranslations[i][2]);
		  }

		  // Boat Mesh.
		  generateMesh(boattexid, boatmodel_ptr, -0.680769, 0.468000, 0.14);

		  // Wheat Mesh.
		  for (int i = 0; i < 5; i++) {
			  generateMesh(wheattexid, wheatmodel_ptr, wheattranslations[i][0], wheattranslations[i][1], wheattranslations[i][2], 0, 0, 0, 0.1, 0.1, 0.1);
		  }

		  // Port Mesh.
		  for (int i = 0; i < 2; i++) {
			  generateMesh(porttexid, portmodel_ptr, porttranslations[i][0], porttranslations[i][1], porttranslations[i][2], 0,0,0, 0.1, 0.1, 0.1);
		  }
		  // All Cyclop Meshs.
		  generateMesh(cycloptexid, cyclopmodel_ptr, -0.24878, 0.563547, -0.486664, 0, -86.868, 0, 0.100, 0.100, 0.100);
		  generateMesh(cycloptexid, cyclopmodel_ptr, -0.830401, 0.574613, 0.865908, 0, 2.0291, 0, 0.100, 0.100, 0.100);
		  generateMesh(cycloptexid, cyclopsitmodel_ptr, 0.003167, 0.574613, 0, 0, -89.179, 0, 0.400, 0.400, 0.400);
		  generateMesh(cycloptexid, cyclopidlemodel_ptr, -0.533495, 0.550675, 0.380219, 0, -195.56, 0, 0.200, 0.200, 0.200);
		  generateMesh(cycloptexid, cyclopwalkingmodel_ptr, -0.46273, 0.543189, 0.682623, 0, 78.368, 0, 0.100, 0.100, 0.100);
		  generateMesh(cycloptexid, cyclopwalkingmodel_ptr, -0.272312, 0.551474, 0.548556, 0, -63.194, 0, 0.100, 0.100, 0.100);

		  // Terrain Mesh
		  generateMesh(terraintexid, terrainmodel_ptr, 0.069132, 0.541456, 0.02787);

		  // Ocean Mesh.
		  generateMesh(oceantexid, oceanmodel_ptr, 0.069132, 0.448000, 0.02787);

	  //glutSolidTeapot(1.0);
	  if (shader1) shader1->end();


      glutSwapBuffers();
      glPopMatrix();

      UpdateTimer();

		Repaint();
	}

	virtual void OnIdle() {}

	/**
	* Método que inicializa la malla.
	*
	* @param model: La malla que se desea importar.
	* @param fileroute: La ruta de acceso a la malla. Recordar que tiene que ser de tipo .obj.
	* @return void.
	*/
	void initMesh(GLMmodel*& model, char* fileroute) {
		model = NULL;

		if (!model)
		{
			model = glmReadOBJ(fileroute);
			if (!model)
				exit(0);

			glmUnitize(model);
			glmFacetNormals(model);
			glmVertexNormals(model, 90.0);
		}
	}

	// When OnInit is called, a render context (in this case GLUT-Window) 
	// is already available!
	virtual void OnInit()
	{
		glClearColor(0.5f, 0.5f, 1.0f, 0.0f);
		glShadeModel(GL_SMOOTH);
		glEnable(GL_DEPTH_TEST);

		shader = SM.loadfromFile("vertexshader.txt","fragmentshader.txt"); // load (and compile, link) from file
		if (shader==0) 
         std::cout << "Error Loading, compiling or linking shader\n";
      else
      {
         ProgramObject = shader->GetProgramObject();
      }

	 //*** Para Textura: abre los shaders para texturas
		shader1 = SM.loadfromFile("vertexshaderT.txt", "fragmentshaderT.txt"); // load (and compile, link) from file
		if (shader1 == 0)
			std::cout << "Error Loading, compiling or linking shader\n";
		else
		{
			ProgramObject = shader1->GetProgramObject();
		}

      time0 = clock();
      timer010 = 0.0f;
      bUp = true;

	  // Ulises Mesh.
	  initMesh(ulisesmodel_ptr, "./Mallas/Ulises_walking.obj");

	  // Cottage1 Mesh.
	  initMesh(cottagemodel_ptr, "./Mallas/cottage.obj");

	  // Tree Mesh.
	  initMesh(treemodel_ptr, "./Mallas/tree.obj");

	  // Boat Mesh.
	  initMesh(boatmodel_ptr, "./Mallas/boat.obj");

	  // Wheat Mesh.
	  initMesh(wheatmodel_ptr, "./Mallas/FieldOfWheat.obj");

	  // Port Mesh.
	  initMesh(portmodel_ptr, "./Mallas/port.obj");

	  // All cyclops Mesh.
	  initMesh(cyclopmodel_ptr, "./Mallas/cyclop.obj");
	  initMesh(cyclopsitmodel_ptr, "./Mallas/cyclop_sit.obj");
	  initMesh(cyclopidlemodel_ptr, "./Mallas/cyclop_idle.obj");
	  initMesh(cyclopwalkingmodel_ptr, "./Mallas/cyclop_walking.obj");

	  // Terrain Mesh.
	  initMesh(terrainmodel_ptr, "./Mallas/terrain.obj");

	  // Ocean Mesh.
	  initMesh(oceanmodel_ptr, "./Mallas/ocean.obj");
	  //*** Para Textura: abrir archivo de textura
	  initialize_textures();
      DemoLight();

	}

	virtual void OnResize(int w, int h)
   {
      if(h == 0) h = 1;
	   float ratio = 1.0f * (float)w / (float)h;

      glMatrixMode(GL_PROJECTION);
	   glLoadIdentity();
	
	   glViewport(0, 0, w, h);

      gluPerspective(45,ratio,1,100);
	   glMatrixMode(GL_MODELVIEW);
	   glLoadIdentity();
	   gluLookAt(0.0f,0.0f,4.0f, 
		          0.0,0.0,-1.0,
			       0.0f,1.0f,0.0f);
   }
	virtual void OnClose(void){}
	virtual void OnMouseDown(int button, int x, int y) {}    
	virtual void OnMouseUp(int button, int x, int y) {}
	virtual void OnMouseWheel(int nWheelNumber, int nDirection, int x, int y){}


	// Función para el manejo de teclado
	virtual void OnKeyDown(int nKey, char cAscii)
	{
		// Control de movimiento de la cámara
		float cameraSpeed = 0.8f;

		if (cAscii == 27) // 0x1b = ESC
		{
			this->Close(); // Close Window!
		}

		if (nKey == GLUT_KEY_UP) // Avanzar
			cameraZ -= cameraSpeed;
		else if (nKey == GLUT_KEY_DOWN) // Retroceder
			cameraZ += cameraSpeed;
		else if (nKey == GLUT_KEY_LEFT) // Girar izquierda
			cameraYaw -= 3.0f;
		else if (nKey == GLUT_KEY_RIGHT) // Girar derecha
			cameraYaw += 3.0f;
	}

	// Función para el manejo de movimiento del ratón
	void OnMouseMotion(int x, int y)
	{
		// Control de rotación de la cámara
		float sensitivity = 0.1f;

		// Calcula la diferencia de posición del ratón
		float deltaX = x - prevMouseX;
		float deltaY = y - prevMouseY;

		// Actualiza la orientación de la cámara
		cameraYaw += deltaX * sensitivity;
		cameraPitch += deltaY * sensitivity;

		// Limita la rotación vertical de la cámara
		if (cameraPitch > 90.0f)
			cameraPitch = 90.0f;
		if (cameraPitch < -90.0f)
			cameraPitch = -90.0f;

		// Actualiza la posición anterior del ratón
		prevMouseX = x;
		prevMouseY = y;
	}

	// Registra las funciones de manejo de eventos en la función OnInit
	

	virtual void OnKeyUp(int nKey, char cAscii)
	{
      if (cAscii == 's')      // s: Shader
         shader->enable();
      else if (cAscii == 'f') // f: Fixed Function
         shader->disable();
	}

   void UpdateTimer()
   {
      time1 = clock();
      float delta = static_cast<float>(static_cast<double>(time1-time0)/static_cast<double>(CLOCKS_PER_SEC));
      delta = delta / 4;
      if (delta > 0.00005f)
      {
         time0 = clock();
         if (bUp)
         {
            timer010 += delta;
            if (timer010>=1.0f) { timer010 = 1.0f; bUp = false;}
         }
         else
         {
            timer010 -= delta;
            if (timer010<=0.0f) { timer010 = 0.0f; bUp = true;}
         }
      }
   }

   void DemoLight(void)
   {
     glEnable(GL_LIGHTING);
     glEnable(GL_LIGHT0);
     glEnable(GL_NORMALIZE);
     // Light model parameters:
     // -------------------------------------------
     /*
     GLfloat lmKa[] = {0.0, 0.0, 0.0, 0.0 };
     glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmKa);
     
     glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0);
     glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 0.0);
     
     // -------------------------------------------
     // Spotlight Attenuation
     
     GLfloat spot_direction[] = {1.0, -1.0, -1.0 };
     GLint spot_exponent = 30;
     GLint spot_cutoff = 180;
     
     glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
     glLighti(GL_LIGHT0, GL_SPOT_EXPONENT, spot_exponent);
     glLighti(GL_LIGHT0, GL_SPOT_CUTOFF, spot_cutoff);
    
     GLfloat Kc = 1.0;
     GLfloat Kl = 0.0;
     GLfloat Kq = 0.0;
     
     glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION,Kc);
     glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, Kl);
     glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, Kq);
     
     
     // ------------------------------------------- 
     // Lighting parameters:

     GLfloat light_pos[] = {0.0f, 5.0f, 5.0f, 1.0f};
     GLfloat light_Ka[]  = {1.0f, 0.5f, 0.5f, 1.0f};
     GLfloat light_Kd[]  = {1.0f, 0.1f, 0.1f, 1.0f};
     GLfloat light_Ks[]  = {1.0f, 1.0f, 1.0f, 1.0f};

     glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
     glLightfv(GL_LIGHT0, GL_AMBIENT, light_Ka);
     glLightfv(GL_LIGHT0, GL_DIFFUSE, light_Kd);
     glLightfv(GL_LIGHT0, GL_SPECULAR, light_Ks);

     // -------------------------------------------
     // Material parameters:

     GLfloat material_Ka[] = {0.5f, 0.0f, 0.0f, 1.0f};
     GLfloat material_Kd[] = {0.4f, 0.4f, 0.5f, 1.0f};
     GLfloat material_Ks[] = {0.8f, 0.8f, 0.0f, 1.0f};
     GLfloat material_Ke[] = {0.1f, 0.0f, 0.0f, 0.0f};
     GLfloat material_Se = 20.0f;

     glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material_Ka);
     glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_Kd);
     glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_Ks);
     glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material_Ke);
     glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material_Se);
	 */
   }
};

//-----------------------------------------------------------------------------

class myApplication : public cwc::glApplication
{
public:
	virtual void OnInit() { std::cout << "Hello World!\n"; };
};
//-----------------------------------------------------------------------------

int main(void)
{
	myApplication*  pApp = new myApplication;
	myWindow* myWin = new myWindow();

	pApp->run();
	delete pApp;
	return 0;
}

//-----------------------------------------------------------------------------