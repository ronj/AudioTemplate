#ifndef THREE_MESH_H
#define THREE_MESH_H

#include <three/core/object3d.h>
#include <three/core/geometry.h>
#include <three/visitor.h>
#include <three/materials/material.h>

#include <vector>
#include <unordered_map>
#include <memory>

namespace three {

class Mesh : public Object3D {
public:

  typedef std::shared_ptr<Mesh> Ptr;

  static Ptr create( const Geometry::Ptr& geometry, const Material::Ptr& material );

  THREE_IMPL_OBJECT(Mesh)

  float boundRadius;
  int morphTargetBase;

  std::vector<int> morphTargetForcedOrder;
  std::vector<float> morphTargetInfluences;
  std::unordered_map<std::string, int> morphTargetDictionary;

  int getMorphTargetIndexByName( const std::string& name );

protected:

  Mesh( const Geometry::Ptr& geometry, const Material::Ptr& material );

};

} // namespace three

#endif // THREE_MESH_H
