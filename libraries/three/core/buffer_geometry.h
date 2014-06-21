#ifndef THREE_BUFFER_GEOMETRY_H
#define THREE_BUFFER_GEOMETRY_H

#include <three/common.h>

#include <three/core/geometry.h>

namespace three {

struct Offset {
  Offset() : start( 0 ), count( 0 ), index( 0 ) { }
  int start, count, index;
};

class THREE_DECL BufferGeometry : public Geometry {
public:

  typedef std::shared_ptr<BufferGeometry> Ptr;

  Ptr create() {
    return make_shared<BufferGeometry>();
  }

  virtual THREE::GeometryType type() const {
    return THREE::BufferGeometry;
  }

  std::vector<Offset> offsets;

  virtual void applyMatrix( Matrix4& matrix ) ;

  void computeBoundingBox();
  void computeBoundingSphere();
  void computeVertexNormals( bool areaWeighted = false );
  void computeTangents();

protected:

  BufferGeometry();

};

} // namespace three

#endif // THREE_BUFFER_GEOMETRY_H