#ifndef THREE_GEOMETRY_UTILS_H
#define THREE_GEOMETRY_UTILS_H

#include <three/core/face3.h>

namespace three {

class GeometryUtils {
public:

  static void merge( Geometry& geometry1, Object3D& object2, int materialIndexOffset = 0 ) {

    if ( object2.type() == enums::Mesh ) {

      if ( object2.matrixAutoUpdate )
        object2.updateMatrix();

      Matrix4 matrix = object2.matrix;

      Matrix3 normalMatrix;
      normalMatrix.getNormalMatrix( matrix );

      merge( geometry1, *object2.geometry, materialIndexOffset, &matrix, &normalMatrix );

    } else {

      merge( geometry1, *object2.geometry, materialIndexOffset );

    }

  }

  static void merge( Geometry& geometry1,
                     Geometry& geometry2,
                     int materialIndexOffset = 0,
                     const Matrix4* matrix = nullptr,
                     const Matrix3* normalMatrix = nullptr ) {

    auto vertexOffset = (int)geometry1.vertices.size();

    auto& vertices1 = geometry1.vertices;
    auto& vertices2 = geometry2.vertices;
    auto& faces1 = geometry1.faces;
    auto& faces2 = geometry2.faces;
    auto& uvs1 = geometry1.faceVertexUvs[ 0 ];
    auto& uvs2 = geometry2.faceVertexUvs[ 0 ];

    // vertices
    concat( vertices1, vertices2 );

    if ( matrix ) {
      std::for_each( vertices1.begin() + vertexOffset, vertices1.end(),
        [&matrix] ( Vertex& v ) { v.applyMatrix4( *matrix ); }
      );
    }

    // faces

    for ( size_t i = 0, il = faces2.size(); i < il; i ++ ) {

      const auto& face = faces2[ i ];
      const auto& faceVertexNormals = face.vertexNormals;
      const auto& faceVertexColors = face.vertexColors;

      THREE_ASSERT( face.type() == enums::Face3 );
      Face3 faceCopy( face.a + vertexOffset, face.b + vertexOffset, face.c + vertexOffset );
      faceCopy.normal.copy( face.normal );

      if ( normalMatrix ) faceCopy.normal.applyMatrix3( *normalMatrix ).normalize();

      for ( size_t j = 0, jl = faceVertexNormals.size(); j < jl; j ++ ) {

        auto normal = faceVertexNormals[ j ].clone();

        if ( normalMatrix ) normal.applyMatrix3( *normalMatrix ).normalize();

        faceCopy.vertexNormals[ j ] = normal;

      }

      faceCopy.color.copy( face.color );

      faceCopy.vertexColors = faceVertexColors;

      faceCopy.materialIndex = face.materialIndex + materialIndexOffset;

      faceCopy.centroid.copy( face.centroid );

      if ( matrix ) faceCopy.centroid.applyMatrix4 ( *matrix );

      faces1.push_back( faceCopy );

    }

    // uvs

    concat( uvs1, uvs2 );

  }

}; // GeometryUtils

} // namespace three

#endif // THREE_GEOMETRY_UTILS_H



