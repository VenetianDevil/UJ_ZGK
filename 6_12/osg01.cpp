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

using namespace std;

osgViewer::Viewer viewer;

osg::Node * stworz_scene()
{
  osg::Image * wood = osgDB::readImageFile("./wood.jpg");
  osg::Texture2D * texture  = new osg::Texture2D();
  texture->setDataVariance(osg::Object::DYNAMIC);
  texture->setImage(wood);
 
  osg::ShapeDrawable * drw = new osg::ShapeDrawable();
  drw->setShape(new osg::Box(osg::Vec3(0, 0, -2.5), 14.0, 5.0, 2.5));
 
  osg::Geode * geom_node = new osg::Geode();
  geom_node->addDrawable(drw);
  geom_node->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  geom_node->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);

  return geom_node;
}

class ModelController : public osgGA::GUIEventHandler {
  public:
    ModelController( osg::MatrixTransform *node ) : _model(node) {}
    virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);
  protected:
    osg::MatrixTransform* _model;
};

bool ModelController::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
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
            matrix *= osg::Matrix::rotate(-0.1, osg::Z_AXIS);
            break;
        case 'd': case 'D':
            matrix *= osg::Matrix::rotate( 0.1, osg::Z_AXIS);
            break;
        case 'w': case 'W':
            matrix *= osg::Matrix::rotate(-0.1, osg::X_AXIS);
            break;
        case 's': case 'S':
            matrix *= osg::Matrix::rotate( 0.1, osg::X_AXIS);
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

void addMonkey(osg::Group* root, float x, float y, float z, bool animate = false, bool handle = false) {
    // malpka
    osg::Node* modelNode = osgDB::readNodeFile("./suzanne.obj");
    osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
    mt->setMatrix(osg::Matrix::translate(x, y, z));
    mt->addChild(modelNode);

    if(!!animate){
      // https://osgart.org/stuff/rigid_body_animation/
      osg::AnimationPath* _animationPath = new osg::AnimationPath;
      _animationPath->setLoopMode(osg::AnimationPath::SWING);
      _animationPath->insert(0.0, osg::AnimationPath::ControlPoint(osg::Vec3d(x, y, z)));
      _animationPath->insert(1.0, osg::AnimationPath::ControlPoint(osg::Vec3d(x, y, z+2)));

      mt->setUpdateCallback(new osg::AnimationPathCallback(_animationPath));
    }

    root->addChild(mt.get());

    if(!!handle){
      // https://sudonull.com/post/1335-OpenSceneGraph-Event-Handling
      ModelController* mcontrol = new ModelController(mt.get());
      viewer.addEventHandler(mcontrol);
    }

}

int main(int argc, char * argv[])
{
    // osg::setNotifyLevel(osg::INFO);
    // jeśli chcesz, aby OSG oprócz komunikatów o błędach wyświetlała
    // również zgrubne komunikaty diagnostyczne odkomentuj powyższą linię

    osg::Node * scn = stworz_scene();

    osg::Group* root = new osg::Group();
    addMonkey(root, 0, 0, 0, false, true);
    addMonkey(root, -5, 0, 0, true);
    addMonkey(root, 5, 0, 0);

    root->addChild(scn);
    viewer.setSceneData(root);

    viewer.setUpViewInWindow(100, 100, 400, 300);
    // viewer.setSceneData(scn);
    return viewer.run();
}
