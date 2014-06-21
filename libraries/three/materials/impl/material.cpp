#include <three/materials/material.h>

#include <three/materials/attribute.h>

#include <three/utils/conversion.h>

namespace three {

namespace {

int nextMaterialID() {
  static int sMaterialCount = 0;
  return ++sMaterialCount;
}

} // namespace

typedef std::pair<std::string, any> Parameter;

Material::Ptr Material::clone() const {
  Ptr cloned;
  __clone( cloned );
  THREE_ASSERT( cloned );
  return cloned;
}

void Material::__clone( Ptr& cloned ) const {

  if ( !cloned )
    cloned = create();

  Material& material = *cloned;

  material.name                = name;

  material.side                = side;

  material.attributes          = material.attributes;

  material.color               = color;
  material.ambient             = ambient;
  material.emissive            = emissive;
  material.specular            = specular;

  material.shininess           = shininess;
  material.opacity             = opacity;
  material.transparent         = transparent;

  material.size                = size;
  material.sizeAttenuation     = sizeAttenuation;

  material.shading             = shading;

  material.vertexColors        = vertexColors;

  material.blending            = blending;

  material.blendSrc            = blendSrc;
  material.blendDst            = blendDst;
  material.blendEquation       = blendEquation;

  material.depthTest           = depthTest;
  material.depthWrite          = depthWrite;

  material.polygonOffset       = polygonOffset;
  material.polygonOffsetFactor = polygonOffsetFactor;
  material.polygonOffsetUnits  = polygonOffsetUnits;

  material.alphaTest           = alphaTest;

  material.overdraw            = overdraw;

  material.visible             = visible;

  material.skinning            = skinning;
  material.morphTargets        = morphTargets;
  material.morphNormals        = morphNormals;

  material.reflectivity        = reflectivity;
  material.refractionRatio     = refractionRatio;
  material.combine             = combine;

  material.metal               = metal;
  material.perPixel            = perPixel;
  material.wrapAround          = wrapAround;
  material.wrapRGB             = wrapRGB;

  material.wireframe           = wireframe;
  material.wireframeLinewidth  = wireframeLinewidth;
  material.linewidth           = linewidth;
  material.linejoin            = linejoin;
  material.linecap             = linecap;

  material.numSupportedMorphTargets = numSupportedMorphTargets;
  material.numSupportedMorphNormals = numSupportedMorphNormals;

  material.normalMap           = normalMap;
  material.normalScale         = normalScale;

  material.fragmentShader      = fragmentShader;
  material.vertexShader        = vertexShader;
  material.uniforms            = uniforms;

  material.map         = map;
  material.envMap      = envMap;
  material.lightMap    = lightMap;
  material.bumpMap     = bumpMap;
  material.specularMap = specularMap;

  material.bumpScale       = bumpScale;

  material.fog             = fog;
  material.lights          = lights;
  material.shadowPass      = shadowPass;

  material.scale           = scale;
  material.gapSize         = shadowPass;
  material.dashSize        = shadowPass;

}


Material::Material( )
  : id( nextMaterialID() ),
    uuid( Math::generateUUID() ),
    side( THREE::FrontSide ),
    opacity( 1 ),
    transparent( false ),
    blending( THREE::NormalBlending ),
    blendSrc( THREE::SrcAlphaFactor ),
    blendDst( THREE::OneMinusSrcAlphaFactor ),
    blendEquation( THREE::AddEquation ),
    depthTest( true ),
    depthWrite( true ),
    polygonOffset( false ),
    polygonOffsetFactor( 0 ),
    polygonOffsetUnits( 0 ),
    alphaTest( 0 ),
    overdraw( .0f ),
    visible( true ),
    needsUpdate( true ),
    color( 0xffffff ),
    ambient( 0xffffff ),
    emissive( 0x000000 ),
    specular( 0x111111 ),
    shininess( 30 ),
    size( 1.f ),
    sizeAttenuation( true ),
    shading( THREE::NoShading ),
    vertexColors( THREE::NoColors ),
    skinning( false ),
    morphTargets( false ),
    morphNormals( false ),
    reflectivity( 1.f ),
    refractionRatio( 0.98f ),
    combine( THREE::MultiplyOperation ),
    metal( false ),
    perPixel( true ),
    wrapAround( false ),
    wrapRGB( 1, 1, 1 ),
    wireframe( false ),
    wireframeLinewidth( 1 ),
    linewidth( 1 ),
    linecap( THREE::Round ),
    linejoin( THREE::Round ),
    numSupportedMorphTargets( 0 ),
    numSupportedMorphNormals( 0 ),
    fragmentShader( "void main() { }" ),
    vertexShader( "void main() { }" ),
    bumpScale( 1 ),
    normalScale( 1, 1, 1 ),
    fog( false ),
    lights( false ),
    shadowPass( false ),
    scale( 1.0f ),
    gapSize ( 1.0f ),
    dashSize( 3.0f ) {

  // TODO not for every material...
  defaultAttributeValues["color"] = toVector( 1.f, 1.f, 1.f );
  defaultAttributeValues["uv"] = toVector( 0.f, 0.f );
  defaultAttributeValues["uv2"] = toVector( 0.f, 0.f );
}

Material::~Material() {}


template < typename T >
inline bool load( const std::unordered_map<std::string, any>& parameters,
                  const std::string& key,
                  T& value ) {
  auto paramIt = parameters.find( key );
  if ( paramIt != parameters.end() ) {
    try {
      value = paramIt->second.cast<T>();
    } catch ( ... ) {
      return false;
    }
    return true;
  }
  return false;
}

/*#define PARAM_LOAD(PARAM_NAME) \
  if ( loadAll || keys.count( #PARAM_NAME ) > 0 ) \
    load ( parameters.contents , #PARAM_NAME, PARAM_NAME )*/
#define PARAM_LOAD(PARAM_NAME) \
  if ( keys.count( #PARAM_NAME ) > 0 ||    \
       defaults.count( #PARAM_NAME ) > 0 ) \
    load ( parameters.contents , #PARAM_NAME, PARAM_NAME )

void Material::setParameters( const Parameters& parameters,
                              const ParameterKeys& keys /*= ParameterKeys()*/ ) {
  if ( parameters.contents.empty() )
    return;

  //bool loadAll = keys.empty();
  const auto& defaults = defaultKeys();

  PARAM_LOAD( side );
  PARAM_LOAD( color );
  PARAM_LOAD( ambient );
  PARAM_LOAD( emissive );
  PARAM_LOAD( specular );
  PARAM_LOAD( shininess );
  PARAM_LOAD( opacity );
  PARAM_LOAD( transparent );
  PARAM_LOAD( size );
  PARAM_LOAD( sizeAttenuation );
  PARAM_LOAD( shading );
  PARAM_LOAD( vertexColors );
  PARAM_LOAD( blending );
  PARAM_LOAD( blendSrc );
  PARAM_LOAD( blendDst );
  PARAM_LOAD( blendEquation );
  PARAM_LOAD( depthTest );
  PARAM_LOAD( depthWrite );
  PARAM_LOAD( polygonOffset );
  PARAM_LOAD( polygonOffsetFactor );
  PARAM_LOAD( polygonOffsetUnits );
  PARAM_LOAD( alphaTest );
  PARAM_LOAD( overdraw );
  PARAM_LOAD( visible );
  PARAM_LOAD( needsUpdate );
  PARAM_LOAD( skinning );
  PARAM_LOAD( morphTargets );
  PARAM_LOAD( morphNormals );
  PARAM_LOAD( reflectivity );
  PARAM_LOAD( refractionRatio );
  PARAM_LOAD( combine );
  PARAM_LOAD( metal );
  PARAM_LOAD( perPixel );
  PARAM_LOAD( wrapAround );
  PARAM_LOAD( wrapRGB );
  PARAM_LOAD( wireframe );
  PARAM_LOAD( wireframeLinewidth );
  PARAM_LOAD( linewidth );
  PARAM_LOAD( linecap );
  PARAM_LOAD( linejoin );
  PARAM_LOAD( program );
  PARAM_LOAD( numSupportedMorphNormals );
  PARAM_LOAD( fragmentShader );
  PARAM_LOAD( vertexShader );
  PARAM_LOAD( uniforms );
  PARAM_LOAD( attributes );
  PARAM_LOAD( bumpScale );
  PARAM_LOAD( normalScale );
  PARAM_LOAD( fog );
  PARAM_LOAD( lights );
  PARAM_LOAD( shadowPass );
  PARAM_LOAD( map );
  PARAM_LOAD( specularMap );
  PARAM_LOAD( envMap );
  PARAM_LOAD( bumpMap );
  PARAM_LOAD( normalMap );
  PARAM_LOAD( lightMap );
  PARAM_LOAD( scale );
  PARAM_LOAD( dashSize );
  PARAM_LOAD( gapSize );
}

#undef PARAM_LOAD

} // namespace three
