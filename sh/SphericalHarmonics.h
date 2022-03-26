#pragma once

#include <cassert>
#include <math.h>
#include <memory>
#include <random>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include <GL/gl.h>
#include <openctmpp.h>

using std::string, std::vector, std::to_string;

// Spherical Harmonics

typedef double Tv8d[8];
typedef struct {
  double x, y, z;
} XYZ;
typedef struct {
  double r, g, b;
} COLOUR;

class SphericalHarmonics {

private:
  Tv8d m;
  int cnt;
  int resolution = 128;
  int colourmap = 7; // 1..25 types of colouring 7=rgb
  string Code;       // the parameter code

#define TWOPI (M_PI * 2.)
#define PI M_PI
#define N_SH_CODES 647

  string *SphericHarmCodes; // [N_SH_CODES];
  bool listCreated = false;

public:
  // init
  SphericalHarmonics();
  SphericalHarmonics(int code, int resolution, int colormap)
      : resolution(resolution), colourmap(colormap) {
    loadCodes();
    ReadCode(code);
  }
  void setCode(int *code) {
    for (int i = 0; i < 8; i++)
      m[i] = (double)code[i];
    createList();
  }
  void setColor(int colourmap) {
    this->colourmap = colourmap;
    createList();
  }
  void setResolution(int resol) {
    this->resolution = resol;
    createList();
  }
  void ReadCode(int code = -2);
  void draw(int ln = 5);
  void Light(void);
  void Light1(void);
  void Light(GLenum nlight, const GLfloat *LightAmbient,
             const GLfloat *LightDiffuse, const GLfloat *LightPosition);
  int getNCodes() { return N_SH_CODES; }
  string getCode(int i) { return SphericHarmCodes[i % N_SH_CODES]; }

  vector<XYZ> vertexes, normals;
  vector<COLOUR> colors;
  vector<vector<int>> trigs;

  vector<vector<int>> triangularize(vector<int> f) {
    vector<vector<int>> r;
    vector<vector<int>> tt{{0, 1, 2}, {0, 2, 3}};

    for (auto t : tt)
      r.push_back({f[t[0]], f[t[1]], f[t[2]]});
    return r;
  }

  string ReplaceString(string subject, const string &search,
                       const string &replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != string::npos) {
      subject.replace(pos, search.length(), replace);
      pos += replace.length();
    }
    return subject;
  }

  vector<float> xyz2fx3(XYZ v) { return {(float)v.x, (float)v.y, (float)v.z}; }
  vector<float> colour2fx4(COLOUR c) {
    return {(float)c.r, (float)c.g, (float)c.b, 0};
  }

  vector<uint8_t> color2u8x3(COLOUR c) {
    return {uint8_t(c.r * 255), uint8_t(c.g * 255), uint8_t(c.b * 255)};
  }

  template <typename... Args>
  string string_format(const string &format, Args... args) {
    int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) +
                 1; // Extra space for '\0'
    if (size_s <= 0) {
      throw std::runtime_error("Error during formatting.");
    }
    auto size = static_cast<size_t>(size_s);
    auto buf = std::make_unique<char[]>(size);
    std::snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(),
                       buf.get() + size - 1); // We don't want the '\0' inside
  }

  template <typename T> void append(vector<T> &v, vector<T> app) {
    v.insert(v.end(), app.begin(), app.end());
  }

  string fileName(string ext) {
    return string_format("sh-%s-%d-%d.%s", Code.c_str(), resolution, colourmap,
                         ext.c_str());
  }

  void saveCTM() {
    genTrigs();

    CTMexporter ctm;

    vector<float> vtx, nrm, cols; // convert to linear representation
    vector<int32_t> idx;
    for (auto v : vertexes)
      append(vtx, xyz2fx3(v));
    for (auto n : normals)
      append(nrm, xyz2fx3(n));
    for (auto c : colors)
      append(cols, colour2fx4(c));
    for (auto ix : trigs)
      append(idx, ix);

    ctm.DefineMesh(vtx.data(), vtx.size() / 3, (CTMuint *)idx.data(),
                   idx.size() / 3, nrm.data());
    ctm.AddAttribMap(cols.data(), "Color");

    auto filename = fileName("ctm");
    ctm.Save(filename.c_str());
  }

  void genTrigs() {
    trigs.clear();
    trigs.reserve(numTrigs());

    int n = resolution;
    for (int i = 0; i < n - 1; i++) {
      for (int j = 0; j < n - 1; j++)
        append(trigs, triangularize({(i + 1) * n + j, (i + 1) * n + j + 1,
                                     i * n + j + 1, i * n + j}));
      append(trigs, triangularize({(i + 1) * n, (i + 1) * n + n - 1, i * n,
                                   i * n + n - 1}));
    }

    for (int i = 0; i < n - 1; i++)
      append(trigs,
             triangularize({i, i + 1, n * (n - 1) + i + 1, n * (n - 1) + i}));

    assert(numTrigs() == trigs.size());
  }

  size_t numTrigs() {
    int n1 = resolution - 1;
    return size_t(2 * n1 * (n1 + 2));
  }

  void savePLY() {
    string head = R"(ply
format binary_little_endian 1.0
comment polygonizer generated
element vertex {nvertex}
property float x
property float y
property float z
property float nx
property float ny
property float nz
property uchar red
property uchar green
property uchar blue
element face {ntrigs}
property list uchar int vertex_indices
end_header
)";

    auto filename = fileName("ply");
    FILE *fh = fopen(filename.c_str(), "wb");

    if (fh) {
      char buffer[4096 * 8];
      setbuf(fh, buffer);

      int n1 = resolution - 1;
      head = ReplaceString(head, "{nvertex}", to_string(vertexes.size()));
      head = ReplaceString(head, "{ntrigs}", to_string(2 * n1 * (n1 + 2)));

      fwrite(head.c_str(), head.size(), 1, fh); // write header

      for (size_t i = 0; i < vertexes.size(); i++) {                   // mesh
        fwrite(xyz2fx3(vertexes[i]).data(), 3 * sizeof(float), 1, fh); // vertex
        fwrite(xyz2fx3(normals[i]).data(), 3 * sizeof(float), 1, fh);  // normal
        fwrite(color2u8x3(colors[i]).data(), 3 * 1, 1, fh);            // colors
      }

      genTrigs();

      for (auto t : trigs) { // write'em
        uint8_t three = 3;
        fwrite(&three, 1, 1, fh);
        fwrite(t.data(), 3 * sizeof(int32_t), 1, fh);
      }
      fclose(fh);
    }
  }

private:
  void loadCodes();
  double powc(double x, double y);
  XYZ Eval(double theta, double phi);
  XYZ CalcNormal(XYZ p, XYZ p1, XYZ p2);
  void Normalise(XYZ *p);
  COLOUR GetColour(double v, double vmin, double vmax, int type);
  void Paint(void);
  void createList(int ln = 5);
};
