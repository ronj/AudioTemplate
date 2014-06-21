#include "gtest/gtest.h"
#include <tests/test_constants.h>

#include <three/math/box3.h>
#include <three/math/matrix4.h>
#include <three/math/plane.h>
#include <three/math/ray.h>
#include <three/math/sphere.h>

using namespace three;

TEST(math_ray_test, constructor_equals) {
  auto a = Ray();
  EXPECT_TRUE( a.origin.equals( zero3 ) );
  EXPECT_TRUE( a.direction.equals( zero3 ) );

  a = Ray( two3.clone(), one3.clone() );
  EXPECT_TRUE( a.origin.equals( two3 ) );
  EXPECT_TRUE( a.direction.equals( one3 ) );
}

TEST(math_ray_test, copy_equals) {
  auto a = Ray( zero3.clone(), one3.clone() );
  auto b = Ray().copy( a );
  EXPECT_TRUE( b.origin.equals( zero3 ) );
  EXPECT_TRUE( b.direction.equals( one3 ) );

  // ensure that it is a true copy
  a.origin = zero3;
  a.direction = one3;
  EXPECT_TRUE( b.origin.equals( zero3 ) );
  EXPECT_TRUE( b.direction.equals( one3 ) );
}

TEST(math_ray_test, set) {
  auto a = Ray();

  a.set( one3, one3 );
  EXPECT_TRUE( a.origin.equals( one3 ) );
  EXPECT_TRUE( a.direction.equals( one3 ) );
}

TEST(math_ray_test, at) {
  auto a = Ray( one3.clone(), Vector3( 0, 0, 1 ) );

  EXPECT_TRUE( a.at( 0 ).equals( one3 ) );
  EXPECT_TRUE( a.at( -1 ).equals( Vector3( 1, 1, 0 ) ) );
  EXPECT_TRUE( a.at( 1 ).equals( Vector3( 1, 1, 2 ) ) );
}

TEST(math_ray_test, recast_clone) {
  auto a = Ray( one3.clone(), Vector3( 0, 0, 1 ) );

  EXPECT_TRUE( a.recast( 0 ).equals( a ) );

  auto b = a.clone();
  EXPECT_TRUE( b.recast( -1 ).equals( Ray( Vector3( 1, 1, 0 ), Vector3( 0, 0, 1 ) ) ) );

  auto c = a.clone();
  EXPECT_TRUE( c.recast( 1 ).equals( Ray( Vector3( 1, 1, 2 ), Vector3( 0, 0, 1 ) ) ) );

  auto d = a.clone();
  auto e = d.clone().recast( 1 );
  EXPECT_TRUE( d.equals( a ) );
  EXPECT_FALSE( e.equals( d ) );
  EXPECT_TRUE( e.equals( c ) );
}

TEST(math_ray_test, closestPointToPoint) {
  auto a = Ray( one3.clone(), Vector3( 0, 0, 1 ) );

  // behind the ray
  auto b = a.closestPointToPoint( zero3 );
  EXPECT_TRUE( b.equals( one3 ) );

  // front of the ray
    auto v= Vector3( 0, 0, 50 );
  auto c = a.closestPointToPoint( v );
  EXPECT_TRUE( c.equals( Vector3( 1, 1, 50 ) ) );

  // exactly on the ray
  auto d = a.closestPointToPoint( one3 );
  EXPECT_TRUE( d.equals( one3 ) );
}

TEST(math_ray_test, distanceToPoint) {
  auto a = Ray( one3.clone(), Vector3( 0, 0, 1 ) );

  // behind the ray
  auto b = a.distanceToPoint( zero3 );
  EXPECT_EQ( b, Math::sqrt( 3.f ) );

  // front of the ray
  auto c = a.distanceToPoint( Vector3( 0, 0, 50 ) );
  EXPECT_EQ( c, Math::sqrt( 2.f ) );

  // exactly on the ray
  auto d = a.distanceToPoint( one3 );
  EXPECT_EQ( d, 0 );
}

TEST(math_ray_test, isIntersectionSphere) {
  auto a = Ray( one3.clone(), Vector3( 0, 0, 1 ) );
  auto b = Sphere( zero3, 0.5 );
  auto c = Sphere( zero3, 1.5 );
  auto d = Sphere( one3, 0.1 );
  auto e = Sphere( two3, 0.1 );
  auto f = Sphere( two3, 1 );

  EXPECT_FALSE( a.isIntersectionSphere( b ) );
  EXPECT_FALSE( a.isIntersectionSphere( c ) );
  EXPECT_TRUE( a.isIntersectionSphere( d ) );
  EXPECT_FALSE( a.isIntersectionSphere( e ) );
  EXPECT_FALSE( a.isIntersectionSphere( f ) );
}

TEST(math_ray_test, isIntersectionPlane) {
  auto a = Ray( one3.clone(), Vector3( 0, 0, 1 ) );

  // parallel plane in front of the ray
  auto b = Plane().setFromNormalAndCoplanarPoint( Vector3( 0, 0, 1 ), one3.clone().sub( Vector3( 0, 0, -1 ) ) );
  EXPECT_TRUE( a.isIntersectionPlane( b ) );

  // parallel plane coincident with origin
  auto c = Plane().setFromNormalAndCoplanarPoint( Vector3( 0, 0, 1 ), one3.clone().sub( Vector3( 0, 0, 0 ) ) );
  EXPECT_TRUE( a.isIntersectionPlane( c ) );

  // parallel plane behind the ray
  auto d = Plane().setFromNormalAndCoplanarPoint( Vector3( 0, 0, 1 ), one3.clone().sub( Vector3( 0, 0, 1 ) ) );
  EXPECT_FALSE( a.isIntersectionPlane( d ) );

  // perpendical ray that overlaps exactly
  auto e = Plane().setFromNormalAndCoplanarPoint( Vector3( 1, 0, 0 ), one3 );
  EXPECT_TRUE( a.isIntersectionPlane( e ) );

  // perpendical ray that doesn't overlap
  auto f = Plane().setFromNormalAndCoplanarPoint( Vector3( 1, 0, 0 ), zero3 );
  EXPECT_FALSE( a.isIntersectionPlane( f ) );
}

TEST(math_ray_test, intersectPlane) {
  auto a = Ray( one3.clone(), Vector3( 0, 0, 1 ) );

  // parallel plane behind
  auto b = Plane().setFromNormalAndCoplanarPoint( Vector3( 0, 0, 1 ), Vector3( 1, 1, -1 ) );
  EXPECT_FALSE( a.intersectPlane( b ) );

  // parallel plane coincident with origin
  auto c = Plane().setFromNormalAndCoplanarPoint( Vector3( 0, 0, 1 ), Vector3( 1, 1, 0 ) );
  EXPECT_FALSE( a.intersectPlane( c ) );

  // parallel plane infront
  auto d = Plane().setFromNormalAndCoplanarPoint( Vector3( 0, 0, 1 ), Vector3( 1, 1, 1 ) );
  EXPECT_TRUE( a.intersectPlane( d )->equals( a.origin ) );

  // perpendical ray that overlaps exactly
  auto e = Plane().setFromNormalAndCoplanarPoint( Vector3( 1, 0, 0 ), one3 );
  EXPECT_TRUE( a.intersectPlane( e )->equals( a.origin ) );

  // perpendical ray that doesn't overlap
  auto f = Plane().setFromNormalAndCoplanarPoint( Vector3( 1, 0, 0 ), zero3 );
  EXPECT_FALSE( a.intersectPlane( f ) );
}


TEST(math_ray_test, applyMatrix4) {
  auto a = Ray( one3.clone(), Vector3( 0, 0, 1 ) );
  auto m = Matrix4();

  EXPECT_TRUE( a.clone().applyMatrix4( m ).equals( a ) );

  a = Ray( zero3.clone(), Vector3( 0, 0, 1 ) );
  m.makeRotationZ( Math::PI() );
  EXPECT_TRUE( a.clone().applyMatrix4( m ).equals( a ) );

  m.makeRotationX( Math::PI() );
  auto b = a.clone();
  b.direction.negate();
  auto a2 = a.clone().applyMatrix4( m );
  EXPECT_LT( a2.origin.distanceTo( b.origin ), 0.0001 );
  EXPECT_LT( a2.direction.distanceTo( b.direction ), 0.0001 );

  a.origin = Vector3( 0, 0, 1 );
  b.origin = Vector3( 0, 0, -1 );
  a2 = a.clone().applyMatrix4( m );
  EXPECT_LT( a2.origin.distanceTo( b.origin ), 0.0001 );
  EXPECT_LT( a2.direction.distanceTo( b.direction ), 0.0001 );
}


TEST(math_ray_test, distanceSqToSegment) {
  auto a = Ray( one3.clone(), Vector3( 0, 0, 1 ) );
  auto ptOnLine = Vector3();
  auto ptOnSegment = Vector3();

  //segment in front of the ray
  auto v0 = Vector3( 3, 5, 50 );
  auto v1 = Vector3( 50, 50, 50 ); // just a far away point
  auto distSqr = a.distanceSqToSegment( v0, v1, &ptOnLine, &ptOnSegment );

  EXPECT_LT( ptOnSegment.distanceTo( v0 ), 0.0001 );
  EXPECT_LT( ptOnLine.distanceTo( Vector3(1, 1, 50) ), 0.0001 );
  // ((3-1) * (3-1) + (5-1) * (5-1) = 4 + 16 = 20
  EXPECT_LT( Math::abs( distSqr - 20 ), 0.0001 );

  //segment behind the ray
  v0 = Vector3( -50, -50, -50 ); // just a far away point
  v1 = Vector3( -3, -5, -4 );
  distSqr = a.distanceSqToSegment( v0, v1, &ptOnLine, &ptOnSegment );

  EXPECT_LT( ptOnSegment.distanceTo( v1 ), 0.0001 );
  EXPECT_LT( ptOnLine.distanceTo( one3 ), 0.0001 );
  // ((-3-1) * (-3-1) + (-5-1) * (-5-1) + (-4-1) + (-4-1) = 16 + 36 + 25 = 77
  EXPECT_LT( Math::abs( distSqr - 77 ), 0.0001 );

  //exact intersection between the ray and the segment
  v0 = Vector3( -50, -50, -50 );
  v1 = Vector3( 50, 50, 50 );
  distSqr = a.distanceSqToSegment( v0, v1, &ptOnLine, &ptOnSegment );

  EXPECT_LT( ptOnSegment.distanceTo( one3 ), 0.0001 );
  EXPECT_LT( ptOnLine.distanceTo( one3 ), 0.0001 );
  EXPECT_LE( distSqr, 0.0001 );
}

TEST(math_ray_test, intersectBox) {

  auto TOL = 0.0001;

  auto box = Box3( Vector3(  -1, -1, -1 ), Vector3( 1, 1, 1 ) );

  auto a = Ray( Vector3( -2, 0, 0 ), Vector3( 1, 0, 0) );
  //ray should intersect box at -1,0,0
  EXPECT_TRUE( a.isIntersectionBox(box) );
  EXPECT_LT( a.intersectBox(box)->distanceTo( Vector3( -1, 0, 0 ) ), TOL );

  auto b = Ray( Vector3( -2, 0, 0 ), Vector3( -1, 0, 0) );
  //ray is point away from box, it should not intersect
  EXPECT_FALSE( b.isIntersectionBox(box) );
  EXPECT_FALSE( b.intersectBox(box) );

  auto c = Ray( Vector3( 0, 0, 0 ), Vector3( 1, 0, 0) );
  // ray is inside box, should return exit point
  EXPECT_TRUE( c.isIntersectionBox(box) );
  EXPECT_LT( c.intersectBox(box)->distanceTo( Vector3( 1, 0, 0 ) ), TOL );

  auto d = Ray( Vector3( 0, 2, 1 ), Vector3( 0, -1, -1).normalize() );
  //tilted ray should intersect box at 0,1,0
  EXPECT_TRUE( d.isIntersectionBox(box) );
  EXPECT_LT( d.intersectBox(box)->distanceTo( Vector3( 0, 1, 0 ) ), TOL );

  auto e = Ray( Vector3( 1, -2, 1 ), Vector3( 0, 1, 0).normalize() );
  //handle case where ray is coplanar with one of the boxes side - box in front of ray
  EXPECT_TRUE( e.isIntersectionBox(box) );
  EXPECT_LT( e.intersectBox(box)->distanceTo( Vector3( 1, -1, 1 ) ), TOL );

  auto f = Ray( Vector3( 1, -2, 0 ), Vector3( 0, -1, 0).normalize() );
  //handle case where ray is coplanar with one of the boxes side - box behind ray
  EXPECT_FALSE( f.isIntersectionBox(box) );
  EXPECT_FALSE( f.intersectBox(box) );
}



