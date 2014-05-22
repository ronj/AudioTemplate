#ifndef THREE_FONT_H
#define THREE_FONT_H

#include <three/fwd.h>
#include <three/core/face.h>
#include <three/textures/texture.h>
#include <three/utils/noncopyable.h>

#include <array>
#include <memory>

namespace three {

class Font : NonCopyable {
public:

  typedef std::shared_ptr<Font> Ptr;

  static Ptr create( const std::string& ttf,
                     float fontSize     = 30,
                     int textureWidth   = 512,
                     int textureHeight  = 512,
                     int firstCharacter = 32,
                     int countCharacter = 96 );

  void generate( const std::string& text,
                 std::vector<Vertex>& vertices,
                 std::vector<Face>& faces,
                 std::vector<std::array<Vector2, 3>>& faceUvs );

  const Texture::Ptr& texture() const;

  ~Font();

protected:

  Font();
  bool initialize( const std::string& ttf,
                   float fontSize,
                   int textureWidth,
                   int textureHeight,
                   int firstCharacter,
                   int countCharacter );

private:
  struct Impl;
  std::unique_ptr<Impl> impl;

};

} // namespace three

#endif // THREE_FONT_H