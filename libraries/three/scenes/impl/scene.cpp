#include <three/scenes/scene.h>

#include <three/visitor.h>

#include <three/objects/particle.h>
#include <three/objects/line.h>
#include <three/objects/mesh.h>
#include <three/lights/light.h>
#include <three/lights/directional_light.h>
#include <three/utils/template.h>


namespace three {

Scene::Scene()
  : Object3D(),
    autoUpdate( true ),
    matrixAutoUpdate( false ) { }

Scene::~Scene() { }

namespace detail {

struct FallbackVisitor : public Visitor {
  //TODO: virtual void operator()( Bone& o )     { fallback(o); }
  virtual void operator()( Camera& o )   {
     fallback(o);
  }
  virtual void operator()( Light& o )    {
    fallback(o);
  }
  virtual void operator()( Scene& o )    {
    fallback(o);
  }
  virtual void operator()( Particle& o ) {
    fallback(o);
  }
  //TODO: virtual void operator()( Sprite& o )   { fallback(o); }
  virtual void operator()( Mesh& o )     {
     fallback(o);
   }
  virtual void operator()( Line& o )     {
    fallback(o);
  }
};

struct Add : public FallbackVisitor {
  Add( Scene& s, const Object3D::Ptr& o )
    : s( s ), object( o ) { }

  void operator()( Object3D& o ) {
    if ( push_unique( s.__objects, &o ) ) {

      s.__objectsAdded.push_back( object );

      erase( s.__objectsRemoved, object );
    }
  }
  void operator()( Light& l ) {
    push_unique( s.__lights, &l );
    if ( l.target && l.target->parent == nullptr ) {
      s.add( l.target );
    }
  }
  void operator()( Bone& ) { }
  void operator()( Camera& ) { }

  Scene& s;
  const Object3D::Ptr& object;
};

struct Remove : public FallbackVisitor {
  Remove( Scene& s, const Object3D::Ptr& o ) : s( s ), object( o ) { }
  void operator()( Object3D& o ) {
    if ( erase( s.__objects, &o ) ) {
      s.__objectsRemoved.push_back( object );
      erase( s.__objectsAdded, object );
    }
  }

  void operator()( Light& light ) {
    if(light.type() == THREE::DirectionalLight) {
      auto& directionalLight = static_cast<DirectionalLight&>(light);
      for(auto& shadowCascadeLight : directionalLight.shadowCascadeArray) {
        Remove(s, shadowCascadeLight);
      }
    }
    erase( s.__lights, &light );
  }
  void operator()( Camera& o ) { }

  Scene& s;
  const Object3D::Ptr& object;
};

} // namespace detail

void Scene::__addObject( const Object3D::Ptr& object ) {
  if ( !object )
    return;

  detail::Add objectAdd( *this, object );
  object->visit( objectAdd );


  dispatchEvent( Event("objectAdded", object.get()) );
  object->dispatchEvent( Event("addedToScene", this) );

  for ( auto& child : object->children ) {
    __addObject( child );
  }
}

void Scene::__removeObject( const Object3D::Ptr& object ) {
  if ( !object )
    return;

  detail::Remove objectRemove( *this, object );
  object->visit( objectRemove );

  for ( auto& child : object->children ) {
    __removeObject( child );
  }
}

void Scene::__clone( Object3D::Ptr& cloned, bool recursive ) const {

  if ( !cloned ) cloned = create();

  Object3D::__clone( cloned, recursive );

  auto& scene = static_cast<Scene&>( *cloned );

  if ( fog ) scene.fog = fog->clone();
  if ( overrideMaterial ) scene.overrideMaterial = overrideMaterial->clone();
  scene.autoUpdate = autoUpdate;
  scene.matrixAutoUpdate = matrixAutoUpdate;

}

} // namespace three
