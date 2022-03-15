// mandelbrotCL.h

#pragma once

#include "firePalette.h"
#include "qcl.hpp"
#include "qcl_module.hpp"

#include <complex>

using std::string, std::vector, std::complex;

// openCL kernel, define base scalar and scalar2 types
// usually (float, float2), (double, double2)

template <class T, class TV>
QCL_STANDALONE_MODULE(mandelbrot_module)
QCL_ENTRYPOINT(mandelbrot)
    QCL_STANDALONE_SOURCE(QCL_IMPORT_TYPE(T) QCL_IMPORT_TYPE(TV) QCL_RAW(
        //
        //  mandelbrot.cl
        //

        TV do_scale(TV range, TV cr, int i, int j, int w, int h) {
          return cr +
                 (TV)((range.y - range.x) * i / w, (range.y - range.x) * j / h);
        }

        uint convertARGB(uint i) { // zyxw
          uint it = i;
          uint ii = i;
          __private uchar *wi4 = (__private uchar *)&ii;
          __private uchar *wti4wt = (__private uchar *)&it;

          wti4wt[0] = wi4[2];
          wti4wt[1] = wi4[1];
          wti4wt[2] = wi4[0];
          wti4wt[3] = wi4[3];
          return it;
        }

        //
        // mandelbrot kernel
        //
        kernel void mandelbrot(global uint *image, global uint *palette,
                               TV center, TV range, int w, int h, int iters) {
          size_t index = get_global_id(0);

          int i = index % w;
          int j = index / w;

          T scale = 0.8 * w / h;

          TV cr = (TV)(range.x, range.x);
          TV c = scale * do_scale(range, cr, i, j, w, h) - center;
          TV z = c;

          for (int it = 0; it < iters; it++) {
            z = (TV)(z.x * z.x - z.y * z.y, z.y * z.x + z.x * z.y) + c;
            if (dot(z, z) > 4) {
              image[index] = convertARGB(
                  0xff000000u |
                  palette[(it << 2) % iters]); // iter element of pallete
              return;
            }
          }
          image[index] = convertARGB(0xff000000u); // out of iter range -> black
        }

        ));

// class
//
template <class T, class TV> class MandelbrotCL {
public:
  MandelbrotCL() {
    platform = env.get_platform_by_preference({"NVIDIA", "AMD", "Intel"});
    global_ctx = env.create_global_context(platform);

    if (global_ctx->get_num_devices() > 0) {
      ctx = global_ctx->device(0);
      ok = true;
    }
  }

  vector<u32> generate(int w, int h, complex<T> center, complex<T> range,
                       int iters) {
    if (ok) {
      this->w = w;
      this->h = h;

      image.resize(w * h);
      palette.resize(iters);

      cl::Buffer imgBuff, paletteBuff; // img, palette buffers
      ctx->create_buffer<uint32_t>(imgBuff, image.size(), image.data());
      ctx->create_buffer<uint32_t>(paletteBuff, iters, palette.getPalette());

      cl::NDRange global_size{image.size()};
      cl::NDRange group_size{384}; // tune to your GPU # cores

      cl_int error =
          mandelbrot_module<T, TV>::mandelbrot(ctx, global_size, group_size)(
              imgBuff, paletteBuff, center, range, w, h, iters);
      qcl::check_cl_error(error, "Could not enqueue kernel!");

      ctx->get_command_queue().finish(); // exec
      ctx->memcpy_d2h(image.data(), imgBuff,
                      image.size()); // image = imgBuff,  copy result to image

      ok = true;
    }
    // convertARGB();
    return image;
  }

  bool isOK() { return ok; }

  string getDeviceName() { return ctx->get_device_name(); }
  u32 *getImage() { return image.data(); }
  int size_bytes() { return w * h * sizeof(u32); }

  void writeBinary(string name) {
    // write binary image
    FILE *fh = fopen(name.c_str(), "w+b");
    fwrite(image.data(), image.size() * 4, 1, fh);
    fclose(fh);
  }

  inline u32 convertARGB(u32 i) {
    u32 it = i;
    uint8_t *wi4 = (uint8_t *)&i, *wti4wt = (uint8_t *)&it;
    wti4wt[0] = wi4[2];
    wti4wt[1] = wi4[1];
    wti4wt[2] = wi4[0];
    wti4wt[3] = wi4[3];
    return it;
  }

  int image_size() { return w * h; }

private:
  vector<uint32_t> image;
  FirePalette palette;
  int w, h;

  qcl::environment env;
  cl::Platform platform;
  qcl::global_context_ptr global_ctx;
  qcl::device_context_ptr ctx;

  bool ok = false;
};
