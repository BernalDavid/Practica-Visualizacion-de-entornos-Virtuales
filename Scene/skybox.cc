#include <vector>
#include "skybox.h"
#include "tools.h"
#include "vector3.h"
#include "trfm3D.h"
#include "renderState.h"
#include "gObjectManager.h"
#include "nodeManager.h"
#include "textureManager.h"
#include "materialManager.h"
#include "shaderManager.h"


using std::vector;
using std::string;

// TODO: create skybox object given gobject, shader name of cubemap texture.
//
// This function does the following:
//
// - Create a new material.
// - Assign cubemap texture to material.
// - Assign material to geometry object gobj
// - Create a new Node.
// - Assign shader to node.
// - Assign geometry object to node.
// - Set sky node in RenderState.
//
// Parameters are:
//
//   - gobj: geometry object to which assign the new material (which incluides
//           cubemap texture)
//   - skyshader: The sky shader.
//   - ctexname: The name of the cubemap texture.
//
// Useful functions:
//
//  - MaterialManager::instance()->create(const std::string & matName): create a
//    new material with name matName (has to be unique).
//  - Material::setTexture(Texture *tex): assign texture to material.
//  - GObject::setMaterial(Material *mat): assign material to geometry object.
//  - NodeManager::instance()->create(const std::string &nodeName): create a new
//    node with name nodeName (has to be unique).
//  - Node::attachShader(ShaderProgram *theShader): attach shader to node.
//  - Node::attachGobject(GObject *gobj ): attach geometry object to node.
//  - RenderState::instance()->setSkybox(Node * skynode): Set sky node.

void CreateSkybox(GObject *gobj,
				  ShaderProgram *skyshader,
				  const std::string &ctexname) {
	if (!skyshader) {
		fprintf(stderr, "[E] Skybox: no sky shader\n");
		exit(1);
	}
	Texture *ctex = TextureManager::instance()->find(ctexname);
	if (!ctex) {
		fprintf(stderr, "[E] Cubemap texture '%s' not found\n", ctexname.c_str());
		std::string S;
		for(auto it = TextureManager::instance()->begin();
			it != TextureManager::instance()->end(); ++it)
			S += "'"+it->getName() + "' ";
		fprintf(stderr, "...avaliable textures are: ( %s)\n", S.c_str());
		exit(1);
	}
	/* =================== PUT YOUR CODE HERE ====================== */
	// Para utilizar las funciones de MaterialManager y NodeManager del comentario
	MaterialManager *materialManager = MaterialManager::instance();
	NodeManager *nodeManager = NodeManager::instance();

	// Crear un nuevo material
	string nombreMaterial = "materialSkyBox";
	Material *material = materialManager->create(nombreMaterial);
	
	// Asingnar la textura del cubemap al material
	// la textura esta en ctex
	material->setTexture(ctex);

	// Asignar el material al objeto geometrico del cubo
	// el objeto esta en gobj (se recibe como parametro)
	gobj->setMaterial(material);

	// Crear un nuevo nodo
	string nombreNodo = "nodoSkyBox";
	Node *nodo = nodeManager->create(nombreNodo);

	// Asignar el shader al nodo
	// el shader esta en skyshader (se recibe como parametro)
	nodo->attachShader(skyshader);

	// Asignar objeto geometrico al nodo
	nodo->attachGobject(gobj);

	// Guardar el nodo en el RenderState
	RenderState::instance()->setSkybox(nodo);

	/* =================== END YOUR CODE HERE ====================== */
}

// TODO: display the skybox
//
// This function does the following:
//
// - Store previous shader
// - Move Skybox to camera location, so that it always surrounds camera.
// - Disable depth test.
// - Set skybox shader
// - Draw skybox object.
// - Restore depth test
// - Set previous shader
//
// Parameters are:
//
//   - cam: The camera to render from
//
// Useful functions:
//
// - RenderState::instance()->getShader: get current shader.
// - RenderState::instance()->setShader(ShaderProgram * shader): set shader.
// - RenderState::instance()->push(RenderState::modelview): push MODELVIEW
//   matrix.
// - RenderState::instance()->pop(RenderState::modelview): pop MODELVIEW matrix.
// - Node::getShader(): get shader attached to node.
// - Node::getGobject(): get geometry object from node.
// - GObject::draw(): draw geometry object.
// - glDisable(GL_DEPTH_TEST): disable depth testing.
// - glEnable(GL_DEPTH_TEST): disable depth testing.

void DisplaySky(Camera *cam) {

	RenderState *rs = RenderState::instance();

	Node *skynode = rs->getSkybox();
	if (!skynode) return;

	/* =================== PUT YOUR CODE HERE ====================== */

	// Almacenar el shader anterior
	ShaderProgram *shaderAnterior = rs->getShader();

	// Mover el skybox a la posicion de la camara
	Vector3 posCamara = cam->getPosition(); 	//necesitamos la posicion, no la camara
	Trfm3D *matrizTransformacion = new Trfm3D();
	matrizTransformacion->setTrans(posCamara);	//transformacion que mueve a la pos de la camara
	rs->push(RenderState::modelview);
	rs->addTrfm(RenderState::modelview, matrizTransformacion);
	
	// Desactivar el z-buffer
	glDisable(GL_DEPTH_TEST);

	// Establecer el shader del skybox
	ShaderProgram *shaderSkyBox = skynode->getShader(); //igual que se guarda el shader anterior
	rs->setShader(shaderSkyBox);

	// Dibujar el objeto skybox
	GObject *objeto = skynode->getGobject();
	objeto->draw();
	rs->pop(RenderState::modelview);
	
	// Activar de nuevo z-buffer
	glEnable(GL_DEPTH_TEST);

	// Restarurar el shader anterior
	rs->setShader(shaderAnterior);


	/* =================== END YOUR CODE HERE ====================== */
}
