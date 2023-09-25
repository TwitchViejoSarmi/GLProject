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
   GLMmodel* ulisesmodel_ptr; // Malla de Ulises.
   GLuint ulisestexid; //*** Para Textura: variable que almacena el identificador de textura
   GLuint cottagetexid;
   GLMmodel* cottage1model_ptr;

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
		loadTexture("./Mallas/ulises.jpg"); // FIXME: La textura de Ulises no carga correctamente.

		// Cottage Texture
		generateIdentifier(cottagetexid);
		loadTexture("./Mallas/cottage.jpg");

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

	

	virtual void OnRender(void)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
      //timer010 = 0.09; //for screenshot!

      glPushMatrix();
	  glTranslatef(-15, -15.5, -23.5); // FIXME: Busca el modelo con la cámara para que se vea centrado.

      if (shader) shader->begin();
	      //glutSolidTeapot(1.0);
      if (shader) shader->end();

	  //*** Para Textura: llamado al shader para objetos texturizados
	  if (shader1) shader1->begin();
		  generateMesh(ulisestexid, ulisesmodel_ptr, -20.57494, 15.123, -0.30686, 0, 0, 0, 0.00759, 0.00759, 0.00759);
		  // Ejemplo de modelo con multiples texturas.
		  generateMesh(cottagetexid, cottage1model_ptr, -9.30081, 15.93683, -24.16032, -90, 0, 92.121);
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

	  //Malla de Ulises.
	  initMesh(ulisesmodel_ptr, "./Mallas/Ulises_walking.obj");

	  // Malla de Cottage1.
	  initMesh(cottage1model_ptr, "./Mallas/cottage.obj");
 
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

	virtual void OnKeyDown(int nKey, char cAscii)
	{       
		if (cAscii == 27) // 0x1b = ESC
		{
			this->Close(); // Close Window!
		} 
	};

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
	virtual void OnInit() {std::cout << "Hello World!\n"; }
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

