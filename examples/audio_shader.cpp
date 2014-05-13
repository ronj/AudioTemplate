#include <audio/audio_io.h>
#include <audio/codec_repository.h>

#include <common/stride_iterator.h>
#include <common/readerwriterqueue.h>

#include <dsp/analysis/fft.h>

#include <three/config.hpp>
#include <three/extras/anim.hpp>
#include <three/extras/glew.hpp>
#include <three/extras/sdl.hpp>
#include <three/renderers/gl_renderer.hpp>
#include <three/renderers/renderer_parameters.hpp>
#include <three/core/geometry.hpp>
#include <three/cameras/camera.hpp>
#include <three/objects/mesh.hpp>
#include <three/extras/geometries/plane_geometry.hpp>
#include <three/materials/shader_material.hpp>

#include <iostream>
#include <random>
#include <fstream>
#include <streambuf>
#include <string>
#include <cerrno>

using Audio = AudioIO<float>;
using namespace three;

// http://forum.libcinder.org/topic/passing-audio-data-to-pixel-shader
// https://gist.github.com/paulhoux/7544133
// https://www.shadertoy.com/view/Mss3Dr

std::string get_file_contents(const std::string& filename)
{
  std::ifstream in(filename, std::ios::in | std::ios::binary);

  if (in)
  {
    return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
  }

  throw(errno);
}

const std::string vertexShader = "\
void main() { \
  gl_Position = vec4(position, 1.0); \
}";

Texture::Ptr createAudioTexture()
{
  unsigned width = 512;
  unsigned height = 2;
  unsigned size = width * height;
  std::vector<unsigned char> data(size, 0);

  auto texture = Texture::create(
    TextureDesc(Image(data, width, height), THREE::LuminanceFormat)
  );

  return texture;
}

void audioShader(GLRenderer::Ptr renderer, moodycamel::ReaderWriterQueue<std::vector<Audio::sample_type>>& q, const std::string& shader)
{
  std::shared_ptr<Fft> fft = Fft::create();
  auto audioTexture = createAudioTexture();
  std::vector<Audio::sample_type> leftChannelData(Audio::DefaultBufferSize);

  auto camera = Camera::create();
  camera->position.z = 1;

  auto scene = Scene::create();

  float time = 1;

  Uniforms uniforms;
  uniforms["time"]       = Uniform(THREE::f, time);
  uniforms["resolution"] = Uniform(THREE::v2, Vector2((float)renderer->width(),
                                                      (float)renderer->height()));
  uniforms["texture"]    = Uniform(THREE::t, audioTexture.get());

  auto material = ShaderMaterial::create(vertexShader, get_file_contents(shader), uniforms);
  auto mesh = Mesh::create(PlaneGeometry::create(2, 2), material);
  scene->add(mesh);

  auto running = true;
  sdl::addEventListener(SDL_KEYDOWN, [&](const sdl::Event&) {
    running = false;
  });
  sdl::addEventListener(SDL_QUIT, [&](const sdl::Event&) {
    running = false;
  });

  anim::gameLoop([&](float dt) -> bool {
    time += dt;
    material->uniforms["time"].value = time;
    std::vector<Audio::sample_type> audioData;

    if (q.try_dequeue(audioData))
    {
      stride_iter<Audio::sample_type*> leftChannelBegin(audioData.data(), 2);
      stride_iter<Audio::sample_type*> leftChannelEnd(audioData.data() + audioData.size(), 2);
      std::copy(leftChannelBegin, leftChannelEnd, leftChannelData.begin());

      fft->setSignal(leftChannelData);
      float* amp = fft->getAmplitude();

      for (std::size_t i = 0; i < fft->getBinSize(); ++i)
      {
        audioTexture->image[0].data[i] = static_cast<unsigned char>((amp[i] * 0.5f + 0.5f) * 254.0f);
      }

      for (std::size_t i = 0; i < leftChannelData.size(); ++i)
      {
        audioTexture->image[0].data[512 + i] = static_cast<unsigned char>((leftChannelData[i] * 0.5f + 0.5f) * 254.0f);
      }

      audioTexture->needsUpdate = true;
    }

    renderer->render(*scene, *camera);

    return running;
  });
}

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " file.[mp3|ogg|flac|wav|aiff]" << std::endl;
    return -1;
  }

  auto onQuit = defer(sdl::quit);

  RendererParameters parameters;
  parameters.antialias = true;

  if (!sdl::init(parameters) || !glew::init(parameters))
  {
    return -2;
  }

  auto renderer = GLRenderer::create(parameters);
  if (!renderer)
  {
    return -3;
  }

  CodecRepository<Audio::sample_type> codecs;
  auto codec = codecs.open(argv[1]);

  std::vector<Audio::sample_type> decodingBuffer(Audio::DefaultBufferSize * codec->info().channels());
  std::vector<Audio::sample_type> audioData(decodingBuffer.capacity());
  moodycamel::ReaderWriterQueue<std::vector<Audio::sample_type>> q(2);

  Audio audio([&](Audio::sample_iterator aInBegin,
                  Audio::sample_iterator aInEnd,
                  Audio::sample_iterator aOutBegin,
                  Audio::sample_iterator aOutEnd)
              {
                std::size_t decodedSamples = codec->decode(decodingBuffer.data(), decodingBuffer.capacity());

                if (decodedSamples > 0)
                {
                  std::copy(decodingBuffer.cbegin(), decodingBuffer.cbegin() + decodedSamples, audioData.begin());
                  if (!q.try_enqueue(audioData))
                  {
                    std::cout << "Dropping audio frame" << std::endl;
                  }
                  std::cout << audioData.size() << " " << audioData.capacity() << std::endl;
                  std::copy(decodingBuffer.cbegin(), decodingBuffer.cbegin() + decodedSamples, aOutBegin);
                }
              });

  audio.play();

  std::cout << "Stream opened with a buffer size of " << audio.bufferSize()
            << " samples, and a stream latency of " << audio.streamLatency()
            << " samples (" << (audio.streamLatency() / audio.samplerateControl().getSamplerate()) * 1000. << " ms.)" << std::endl
            << "Playing back " << argv[1] << ", which is " << codec->info().toString() << std::endl;

  audioShader(renderer, q, argv[2]);

  return 0;
}
