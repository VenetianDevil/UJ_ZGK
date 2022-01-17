/* ***************************************************************************

Program tworzy banalny graf sceny z jednym węzłem geometrii zawierającym dwie
proste bryły, przekazuje go obiektowi klasy Viewer, i uruchamia Viewer. Viewer
automatycznie tworzy kamerę obejmującą wzrokiem całą scenę.

Viewery mają domyślne mechanizmy obsługi myszki -- proszę spróbować ciągnięcia
z wciśniętym którymś z przycisków i zobaczyć co się stanie. Mają też domyślną
obsługę klawiatury -- spacja przywraca początkowe położenie kamery, a Esc
kończy działanie programu.

Viewery domyślnie używają modelu oświetlenia Phonga (włączają GL_LIGHTING)
i konfigurują pierwsze ze świateł (GL_LIGHT0) tak, aby było przymocowane do
kamery. W ten sposób, niezależnie od tego jak myszką obrócimy widok, bryły
na które patrzymy zawsze są oświetlone z przodu.

Uwaga: ten kod używa zwykłych wskaźników zamiast osg::ref_ptr<>. Nie jest to
zalecane, bo w bardziej skomplikowanych programach może być przyczyną wycieków
pamięci.

Zadanie: sprawdź w jaki sposób zorientowane są osie współrzędnych. Jeśli
trzeba dodaj do sceny dodatkowe bryły.

*************************************************************************** */

#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/Texture2D>
#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osg/MatrixTransform>
#include <osg/AnimationPath>
#include <osgGA/EventVisitor>
#include <osgGA/GUIEventAdapter>

#include <iostream>
#include <cmath>

using namespace std;

osgViewer::Viewer viewer;
osg::Group* root = new osg::Group();
osg::ref_ptr<osg::MatrixTransform> mt_barrel = new osg::MatrixTransform;
osg::ref_ptr<osg::MatrixTransform> mt_base = new osg::MatrixTransform;

class CannonModelController : public osgGA::GUIEventHandler {
  public:
    CannonModelController( osg::MatrixTransform *node ) : _model(node) {}
    virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);
    void addCannonBall();
  protected:
    osg::MatrixTransform* _model;
};

bool CannonModelController::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa) {
    (void) aa;
    // if (!_model.valid())
    //     return false;
    osg::Matrix matrix = _model->getMatrix();
    switch (ea.getEventType())
    {
    case osgGA::GUIEventAdapter::KEYDOWN:
        switch (ea.getKey())
        {
        case 'a': case 'A':
            // cout << "z: " <<  matrix.getRotate().z() << endl;
            // cout << "w: " <<  matrix.getRotate().w() << endl;            
            if(matrix.getRotate().z() < 0.5){
              matrix *= osg::Matrix::rotate( 0.1, osg::Z_AXIS);
            }
            break;
        case 'd': case 'D':
            // cout << "z: " <<  matrix.getRotate().z() << endl;
            // cout << "w: " <<  matrix.getRotate().w() << endl;
            if(matrix.getRotate().z() > -0.5){
              matrix *= osg::Matrix::rotate(-0.1, osg::Z_AXIS);
            }
            break;
        case ' ':
            cout << "fire!" << endl;
            addCannonBall();
            break;
        default:
            break;
        }
        _model->setMatrix(matrix);
        break;
    default:
        break;
    }
    return false;
}

class BarrelModelController : public osgGA::GUIEventHandler {
  public:
    BarrelModelController( osg::MatrixTransform *node ) : _model(node) {}
    virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);
  protected:
    osg::MatrixTransform* _model;
};

bool BarrelModelController::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa) {
    (void) aa;
    // if (!_model.valid())
    //     return false;
    osg::Matrix matrix = _model->getMatrix();
    switch (ea.getEventType())
    {
    case osgGA::GUIEventAdapter::KEYDOWN:
        switch (ea.getKey())
        {
        case 'w': case 'W':
            // cout << "x: " <<  matrix.getRotate().x() << endl;
            // cout << "w: " <<  matrix.getRotate().w() << endl;
            if(matrix.getRotate().x() < 0){
              matrix *= osg::Matrix::rotate( 0.1, osg::X_AXIS);
            }
            break;
        case 's': case 'S':
            // cout << "x: " <<  matrix.getRotate().x() << endl;
            // cout << "w: " <<  matrix.getRotate().w() << endl;            
            if(matrix.getRotate().x() > -0.7){
              matrix *= osg::Matrix::rotate(-0.1, osg::X_AXIS);
            }
            break;
        default:
            break;
        }
        _model->setMatrix(matrix);
        break;
    default:
        break;
    }
    return false;
}

void CannonModelController::addCannonBall() {
    osg::Geode * geom_node_ball = new osg::Geode();

    float x = 0.0;
    float y = 0.0;
    float z = 0.0;

    osg::ShapeDrawable * drw_ball = new osg::ShapeDrawable();
    drw_ball->setShape(new osg::Sphere(osg::Vec3(0.0, 0.2, 1.5), 0.2));
    drw_ball->setColor(osg::Vec4(0.0, 0.0, 0.0, 1.0));
    geom_node_ball->addDrawable(drw_ball);

    osg::ref_ptr<osg::MatrixTransform> mt_ball = new osg::MatrixTransform;
    mt_ball->addChild(geom_node_ball);
    mt_barrel->addChild(mt_ball.get());
    // mt_ball->setMatrix(osg::Matrix::rotate(M_PI/2, osg::X_AXIS)*osg::Matrix::translate(0, 1.7, 1.5));

    // animate
    osg::AnimationPath* _animationPath = new osg::AnimationPath;
    _animationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);
    _animationPath->insert(0.0, osg::AnimationPath::ControlPoint(osg::Vec3d(x, y, z)));
    _animationPath->insert(1.0, osg::AnimationPath::ControlPoint(osg::Vec3d(x, y, z+35))); //1+y

    mt_ball->setUpdateCallback(new osg::AnimationPathCallback(_animationPath));

    // root->addChild(mt_ball.get());
}

void addTarget(){
    osg::Node* modelNode = osgDB::readNodeFile("./Pumkin.obj");
    osg::ref_ptr<osg::MatrixTransform> mt_target = new osg::MatrixTransform;
    mt_target->setMatrix(osg::Matrix::translate(rand() % 30, rand() % 30, rand() % 20));
    mt_target->addChild(modelNode);

    root->addChild(mt_target.get());
}

osg::Node * stworz_scene() {
  osg::Image * wood = osgDB::readImageFile("./grass.jpg");
  osg::Texture2D * texture  = new osg::Texture2D();
  texture->setDataVariance(osg::Object::DYNAMIC);
  texture->setImage(wood);
 
  osg::ShapeDrawable * drw = new osg::ShapeDrawable();
  drw->setShape(new osg::Box(osg::Vec3(0, 15, -1.5), 60.0, 40, .5));
 
  osg::Geode * geom_node = new osg::Geode();
  geom_node->addDrawable(drw);
  geom_node->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  geom_node->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);

  return geom_node;
}

void addCannon() {
  // podstawa armaty
    osg::Image * wood = osgDB::readImageFile("./wood.jpg"); 
    osg::Texture2D * texture_wood  = new osg::Texture2D();
    texture_wood->setDataVariance(osg::Object::DYNAMIC);
    texture_wood->setImage(wood);

    osg::ShapeDrawable * drw_base = new osg::ShapeDrawable();
    drw_base->setShape(new osg::Box(osg::Vec3(0.0, 0.0, -1.0), 2.0, 4, 1));
    
    osg::Geode * geom_node_base = new osg::Geode();
    geom_node_base->addDrawable(drw_base);
    geom_node_base->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    geom_node_base->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture_wood, osg::StateAttribute::ON);

  // lufa armaty
    osg::Image * metal = osgDB::readImageFile("./metal.jpg");
    osg::Texture2D * texture_metal  = new osg::Texture2D();
    texture_metal->setDataVariance(osg::Object::DYNAMIC);
    texture_metal->setImage(metal);

    osg::ShapeDrawable * drw_barrel = new osg::ShapeDrawable();
    drw_barrel->setShape(new osg::Cylinder(osg::Vec3(0.0, 0.2, 0.0), 0.3, 3));
    
    osg::Geode * geom_node_barrel = new osg::Geode();
    geom_node_barrel->addDrawable(drw_barrel);
    geom_node_barrel->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    geom_node_barrel->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture_metal, osg::StateAttribute::ON);

  //  macierze transformacji
    // osg::ref_ptr<osg::MatrixTransform> mt_barrel = new osg::MatrixTransform;
    mt_barrel->setMatrix(osg::Matrix::rotate(-M_PI/2, osg::X_AXIS));
    mt_barrel->addChild(geom_node_barrel);

    // mt_base->setMatrix(osg::Matrix::translate(x, y, z));
    mt_base->addChild(geom_node_base);
    mt_base->addChild(mt_barrel.get());

    root->addChild(mt_base.get());
    // root->addChild(mt_barrel.get());

    // https://sudonull.com/post/1335-OpenSceneGraph-Event-Handling
    BarrelModelController* mcontrol_1 = new BarrelModelController(mt_barrel.get());
    viewer.addEventHandler(mcontrol_1);

    // https://sudonull.com/post/1335-OpenSceneGraph-Event-Handling
    CannonModelController* mcontrol_2 = new CannonModelController(mt_base.get());
    viewer.addEventHandler(mcontrol_2);

}

int main(int argc, char * argv[]) {
    osg::Node * scn = stworz_scene();

    addCannon();
    addTarget();

    root->addChild(scn);
    viewer.setSceneData(root);

    viewer.setUpViewInWindow(100, 100, 400, 300);
    // viewer.setSceneData(scn);
    return viewer.run();
}
