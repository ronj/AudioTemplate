#include "common.h"

#include "three/cameras/perspective_camera.h"
#include "three/core/geometry.h"
#include "three/lights/point_light.h"
#include "three/materials/line_basic_material.h"
#include "three/objects/line.h"
#include "three/renderers/renderer_parameters.h"
#include "three/renderers/gl_renderer.h"
#include "three/scenes/fog.h"

#include <array>

using namespace three;
using namespace three_examples;

std::vector<Vector3> hilbert3D( Vector3 center,
                                float side,
                                int iterations,
                                int v0, int v1,
                                int v2, int v3,
                                int v4, int v5,
                                int v6, int v7 ) {

  const auto half = side / 2;

  const std::array<Vector3, 8> vec_s = {
    Vector3( center.x - half, center.y + half, center.z - half ),
    Vector3( center.x - half, center.y + half, center.z + half ),
    Vector3( center.x - half, center.y - half, center.z + half ),
    Vector3( center.x - half, center.y - half, center.z - half ),
    Vector3( center.x + half, center.y - half, center.z - half ),
    Vector3( center.x + half, center.y - half, center.z + half ),
    Vector3( center.x + half, center.y + half, center.z + half ),
    Vector3( center.x + half, center.y + half, center.z - half )
  };

  const std::array<Vector3, 8> vec = {
     vec_s[ v0 ], vec_s[ v1 ], vec_s[ v2 ], vec_s[ v3 ], vec_s[ v4 ], vec_s[ v5 ], vec_s[ v6 ], vec_s[ v7 ]
   };

  if( --iterations >= 0 ) {

    std::vector<Vector3> tmp;

    concat( tmp, hilbert3D ( vec[ 0 ], half, iterations, v0, v3, v4, v7, v6, v5, v2, v1 ) );
    concat( tmp, hilbert3D ( vec[ 1 ], half, iterations, v0, v7, v6, v1, v2, v5, v4, v3 ) );
    concat( tmp, hilbert3D ( vec[ 2 ], half, iterations, v0, v7, v6, v1, v2, v5, v4, v3 ) );
    concat( tmp, hilbert3D ( vec[ 3 ], half, iterations, v2, v3, v0, v1, v6, v7, v4, v5 ) );
    concat( tmp, hilbert3D ( vec[ 4 ], half, iterations, v2, v3, v0, v1, v6, v7, v4, v5 ) );
    concat( tmp, hilbert3D ( vec[ 5 ], half, iterations, v4, v3, v2, v5, v6, v1, v0, v7 ) );
    concat( tmp, hilbert3D ( vec[ 6 ], half, iterations, v4, v3, v2, v5, v6, v1, v0, v7 ) );
    concat( tmp, hilbert3D ( vec[ 7 ], half, iterations, v6, v5, v2, v1, v0, v3, v4, v7 ) );

    return tmp;

  }

  return std::vector<Vector3>(vec.begin(), vec.end());
}

/////////////////////////////////////////////////////////////////////////

void lines_colors( GLWindow& window, GLRenderer& renderer ) {

  renderer.sortObjects = false;

  auto camera = PerspectiveCamera::create(
    33, (float)renderer.width() / renderer.height(), 1, 10000
  );
  camera->position.z = 700;

  auto geometry  = Geometry::create(),
       geometry2 = Geometry::create(),
       geometry3 = Geometry::create();

  auto points = hilbert3D( Vector3( 0, 0, 0 ), 200.0, 2, 0, 1, 2, 3, 4, 5, 6, 7 );

  std::vector<Color> colors, colors2, colors3;

  int i = 0;
  for ( const auto& point : points ) {

    geometry->vertices.push_back( point );

    colors.push_back( Color( 0xffffff ) );
    colors.back().setHSL( 0.6f, 1.f, std::max( 0.f, ( 200.f - point.x ) / 400 ) * 0.5f + 0.5f );

    colors2.push_back( Color( 0xffffff ) );
    colors2.back().setHSL( 0.3f, 1.0f, std::max( 0.f, ( 200.f + point.x ) / 400 ) * 0.5f );

    colors3.push_back( Color( 0xffffff ) );
    colors3.back().setHSL( (float)i++ / points.size(), 1.0f, 0.5f );

  }

  geometry2->vertices = geometry3->vertices = geometry->vertices = std::move( points );

  geometry->colors  = std::move( colors );
  geometry2->colors = std::move( colors2 );
  geometry3->colors = std::move( colors3 );

  /////////////////////////////////////////////////////////////////////////

  auto material = LineBasicMaterial::create(
    Material::Parameters().add( "color", Color(0xffffff) )
                          .add( "opacity", 1.f )
                          .add( "linewidth", 1.5f )
  );

  material->vertexColors = enums::VertexColors;

  auto scene = Scene::create();

  auto addLine = [&scene]( Vector3 pos, float scale, Geometry::Ptr geometry, Material::Ptr material ) {
    auto line = Line::create( geometry, material );
    line->scale.x = line->scale.y = line->scale.z = scale;
    line->position = pos;
    scene->add( line );
  };

  const auto scale = 0.15f, d = 225.f;
  addLine( Vector3(-d,0,0), scale, geometry,  material );
  addLine( Vector3( 0,0,0), scale, geometry2, material );
  addLine( Vector3( d,0,0), scale, geometry3, material );

  //////////////////////////////////////////////////////////////////////////

  auto mouseX = 0.f, mouseY = 0.f;
  window.addEventListener(SDL_MOUSEMOTION, [&]( const SDL_Event& event ) {
    mouseX = 2.f * ((float)event.motion.x / renderer.width()  - 0.5f);
    mouseY = 2.f * ((float)event.motion.y / renderer.height() - 0.5f);
  });

  //////////////////////////////////////////////////////////////////////////

  auto time = 0.f;

  window.animate( [&]( float dt ) -> bool {

    time += dt;

    camera->position.x += ( -500.f * mouseX - camera->position.x ) * 3 * dt;
    camera->position.y += (  500.f * mouseY - camera->position.y ) * 3 * dt;
    camera->lookAt( scene->position );

    for ( size_t i = 0; i < scene->children.size(); i++ ) {
      if (scene->children[i]->type() == enums::Line )
        scene->children[i]->rotation().y = time * ( i % 2 ? 1 : -1);
    }

    renderer.render( *scene, *camera );

    return true;

  } );

}

int main ( int argc, char* argv[] ) {

  return RunExample( lines_colors );

}
