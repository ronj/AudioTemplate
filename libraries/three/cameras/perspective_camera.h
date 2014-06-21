#ifndef THREE_PERSPECTIVE_CAMERA_H
#define THREE_PERSPECTIVE_CAMERA_H

#include <three/common.h>

#include <three/math/math.h>
#include <three/cameras/camera.h>

namespace three {

class PerspectiveCamera : public Camera {

public:

  THREE_IMPL_OBJECT(PerspectiveCamera);

  static Ptr create( float fov = 50.f, float aspect = 1.f, float near = 0.1f, float far = 2000.f ) {

    return make_shared<PerspectiveCamera>( fov, aspect, near, far );

  }

  float fov, aspect;

  float fullWidth, fullHeight;

  float x, y;

  float width, height;

  void setLens( float focalLength, float frameHeight = 24.f ) {

    fov = 2.f * Math::radToDeg( Math::atan( frameHeight / ( focalLength * 2.f ) ) );

    updateProjectionMatrix();

  }

  /**
   * Sets an offset in a larger frustum. This is useful for multi-window or
   * multi-monitor/multi-machine setups.
   *
   * For example, if you have 3x2 monitors and each monitor is 1920x1080 and
   * the monitors are in grid like this
   *
   *   +---+---+---+
   *   | A | B | C |
   *   +---+---+---+
   *   | D | E | F |
   *   +---+---+---+
   *
   * then for each monitor you would call it like this
   *
   *   var w = 1920;
   *   var h = 1080;
   *   var fullWidth = w * 3;
   *   var fullHeight = h * 2;
   *
   *   --A--
   *   camera.setOffset( fullWidth, fullHeight, w * 0, h * 0, w, h );
   *   --B--
   *   camera.setOffset( fullWidth, fullHeight, w * 1, h * 0, w, h );
   *   --C--
   *   camera.setOffset( fullWidth, fullHeight, w * 2, h * 0, w, h );
   *   --D--
   *   camera.setOffset( fullWidth, fullHeight, w * 0, h * 1, w, h );
   *   --E--
   *   camera.setOffset( fullWidth, fullHeight, w * 1, h * 1, w, h );
   *   --F--
   *   camera.setOffset( fullWidth, fullHeight, w * 2, h * 1, w, h );
   *
   *   Note there is no reason monitors have to be the same size or in a grid.
   */
  void setViewOffset( float fullWidthIn, float fullHeightIn, float xIn, float yIn, float widthIn, float heightIn ) {

    fullWidth  = fullWidthIn;
    fullHeight = fullHeightIn;
    x          = xIn;
    y          = yIn;
    width      = widthIn;
    height     = heightIn;

    updateProjectionMatrix();

  }

  void updateProjectionMatrix() {

    if ( fullWidth != 0.f ) {

      auto aspect = fullWidth / fullHeight;
      auto top = Math::tan( Math::degToRad( fov * 0.5f ) ) * near;
      auto bottom = -top;
      auto left = aspect * bottom;
      auto right = aspect * top;
      auto width = Math::abs( right - left );
      auto height = Math::abs( top - bottom );

      projectionMatrix.makeFrustum( left + x * width / fullWidth,
                                    left + ( x + width ) * width / fullWidth,
                                    top - ( y + height ) * height / fullHeight,
                                    top - y * height / fullHeight,
                                    near,
                                    far );

    } else {

      projectionMatrix.makePerspective( fov, aspect, near, far );

    }

  }

protected:

  PerspectiveCamera( float fov, float aspect, float near, float far )
    : Camera( near, far ), fov( fov ), aspect( aspect ),
      fullWidth( 0 ), fullHeight( 0 ), x( 0 ), y( 0 ), width( 0 ), height( 0 ) {

    updateProjectionMatrix();

  }

  virtual void __clone( Object3D::Ptr& cloned, bool recursive ) const THREE_OVERRIDE {

    if ( !cloned ) cloned = create();

    Camera::__clone( cloned, recursive );

    auto& camera = static_cast<PerspectiveCamera&>( *cloned );
    camera.fov = fov;
    camera.aspect = aspect;
    camera.fullWidth = fullWidth;
    camera.fullHeight = fullHeight;
    camera.x = x;
    camera.y = y;
    camera.width = width;
    camera.height = height;

  }


};

} // namespace three

#endif // THREE_PERSPECTIVE_CAMERA_H