#ifndef THREE_RENDERABLE_FACE3_H
#define THREE_RENDERABLE_FACE3_H

#include <three/common.h>
#include <three/constants.h>

#include <three/renderers/renderables/renderable_face.h>

namespace three {

class RenderableFace3 : public RenderableFace {

public:

  RenderableFace3() : RenderableFace( THREE::Face3 ) { }

};

} // namespace three

#endif // THREE_RENDERABLE_FACE3_H