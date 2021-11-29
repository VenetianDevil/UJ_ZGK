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
#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osg/MatrixTransform>

osg::Node * stworz_scene()
{
    osg::Geode * geom_node = new osg::Geode();

    osg::ShapeDrawable * drw = new osg::ShapeDrawable();
    // prostopadłościan: środek i długości boków X, Y, Z
    drw->setShape(new osg::Box(osg::Vec3(-5.0, 0.0, 0.0), 10.0, 5.0, 2.5));
    // kolor czerwony, w pełni nieprzezroczysty
    drw->setColor(osg::Vec4(1.0, 0.0, 0.0, 1.0));
    geom_node->addDrawable(drw);

    drw = new osg::ShapeDrawable();
    // sfera: środek i promień
    drw->setShape(new osg::Sphere(osg::Vec3(3.0, 2.0, 0.0), 3.0));
    // zielony, też z współczynnikiem alpha równym 100%
    drw->setColor(osg::Vec4(0.0, 1.0, 0.0, 1.0));
    geom_node->addDrawable(drw);

// biała w 0 0 0 
    drw = new osg::ShapeDrawable();
    // sfera: środek i promień
    drw->setShape(new osg::Sphere(osg::Vec3(0.0, 0.0, 0.0), 1.0));
    // zielony, też z współczynnikiem alpha równym 100%
    drw->setColor(osg::Vec4(1.0, 1.0, 1.0, 1.0));
    geom_node->addDrawable(drw);

// czerwona w 5 0 0 
    drw = new osg::ShapeDrawable();
    // sfera: środek i promień
    drw->setShape(new osg::Sphere(osg::Vec3(5.0, 0.0, 0.0), 1.0));
    // zielony, też z współczynnikiem alpha równym 100%
    drw->setColor(osg::Vec4(1.0, 0.0, 0.0, 1.0));
    geom_node->addDrawable(drw);

// zielona w 0 5 0 
    drw = new osg::ShapeDrawable();
    // sfera: środek i promień
    drw->setShape(new osg::Sphere(osg::Vec3(0.0, 5.0, 0.0), 1.0));
    // zielony, też z współczynnikiem alpha równym 100%
    drw->setColor(osg::Vec4(0.0, 1.0, 0.0, 1.0));
    geom_node->addDrawable(drw);

// zielona niebieska 0 0 5 
    drw = new osg::ShapeDrawable();
    // sfera: środek i promień
    drw->setShape(new osg::Sphere(osg::Vec3(0.0, 0.0, 5.0), 1.0));
    // zielony, też z współczynnikiem alpha równym 100%
    drw->setColor(osg::Vec4(0.0, 0.0, 1.0, 1.0));
    geom_node->addDrawable(drw);

    return geom_node;
}

int main(int argc, char * argv[])
{
    // osg::setNotifyLevel(osg::INFO);
    // jeśli chcesz, aby OSG oprócz komunikatów o błędach wyświetlała
    // również zgrubne komunikaty diagnostyczne odkomentuj powyższą linię

    osg::Node * scn = stworz_scene();
    osgViewer::Viewer viewer;

    osg::Group* root = new osg::Group(); 
    // human.obj
    osg::Node* modelNode = osgDB::readNodeFile("./human.obj");
    osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
    mt->setMatrix(osg::Matrix::translate(-2, 0,1.25));
    mt->addChild(modelNode);


    root->addChild(mt.get());
    root->addChild(scn);
    viewer.setSceneData(root);

    viewer.setUpViewInWindow(100, 100, 400, 300);
    // viewer.setSceneData(scn);
    return viewer.run();
}
