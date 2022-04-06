//
//  polyhedron.hpp
//  test_polyhedron
//
//  Created by asd on 03/09/2019.
//  Copyright © 2019 voicesync. All rights reserved.
//

#pragma once

#include "Thread.h"
#include "color.hpp"
#include "common.hpp"

class Polyhedron {
 public:
  Polyhedron() {}
  Polyhedron(const string name, const Vertexes vertexes, const Faces faces)
      : name(name),
        vertexes(vertexes),
        faces(faces),
        n_vertex(vertexes.size()),
        n_faces(faces.size()) {}

  // Polyhedron(const string name, const VertexesFloat vertexes,
  //            const vector<vector<int>> faces)
  //     : name(name),
  //       faces(faces),
  //       n_vertex(vertexes.size()),
  //       n_faces(faces.size()) {
  //   for (auto v : vertexes) this->vertexes.push_back(Vertex(simd_f3{v[0], v[1], v[2]}));
  // }

  Polyhedron recalc() {
    calc_normals();
    calc_areas();
    calc_centers();
    calc_colors();
    return *this;
  }

  void scale_vertexes() {
    float mx = -__FLT_MAX__;
    for (auto &v : vertexes) {
      mx = max(mx, v.max_abs());
    }
    float diff = mx;
    if (diff != 0.f)
      for (auto &v : vertexes) v /= diff;
  }

  void calc_normals() {  // per face
    normals = Vertexes(n_faces);
    Thread(n_faces).run([this](int f) {
      normals[f] = point3::normal(vertexes[faces[f][0]], vertexes[faces[f][1]],
                               vertexes[faces[f][2]]);
    });
  }

  void calc_normals_st() {  // per face
    normals = Vertexes(n_faces);
    for (size_t f = 0; f < n_faces; f++)
      normals[f] = point3::normal(vertexes[faces[f][0]], vertexes[faces[f][1]],
                               vertexes[faces[f][2]]);
  }

  int count_points() {  // count # of vertex used in all faces
    int np = 0;
    for (auto &face : faces) np += face.size();
    return np;
  }

  // calculate average normal vector for array of vertices
  Vertexes avg_normals() {
    auto normals = Vertexes(n_faces);

    Thread(n_faces).run([this, &normals](int i) {
      size_t face_len = faces[i].size();
      Vertex normalV = 0;
      auto v1 = vertexes[face_len - 2], v2 = vertexes[face_len - 1];

      for (auto ic : faces[i]) {  // running sum of normal vectors
        auto v3 = vertexes[ic];
        normalV += point3::normal(v1, v2, v3);
        v1 = v2;
        v2 = v3;  // shift over one
      }
      normals[i] = unit(normalV);
    });
    return normals;
  }

  Vertexes avg_normals_st() {
    auto normals = Vertexes(n_faces);

    for (size_t i = 0; i < n_faces; i++) {
      size_t face_len = faces[i].size();
      Vertex normalV = 0;
      auto v1 = vertexes[face_len - 2], v2 = vertexes[face_len - 1];

      for (auto ic : faces[i]) {  // running sum of normal vectors
        auto v3 = vertexes[ic];
        normalV += point3::normal(v1, v2, v3);
        v1 = v2;
        v2 = v3;  // shift over one
      }
      normals[i] = unit(normalV);
    }
    return normals;
  }

  void calc_centers() {  // per face
    centers = Vertexes(n_faces);
    Thread(n_faces).run([this](int f) {
      Vertex fcenter = 0;
      Face face = faces[f];
      // average vertex coords
      for (size_t ic = 0; ic < face.size(); ic++) fcenter += vertexes[face[ic]];
      centers[f] =
          fcenter / face.size();  //  return face - ordered array  of  centroids
    });
  }

  void calc_centers_st() {  // per face
    centers = Vertexes(n_faces);
    for (size_t f = 0; f < n_faces; f++) {
      Vertex fcenter = 0;
      Face face = faces[f];
      // average vertex coords
      for (size_t ic = 0; ic < face.size(); ic++) fcenter += vertexes[face[ic]];
      centers[f] =
          fcenter / face.size();  //  return face - ordered array  of  centroids
    }
  }

  void calc_areas() {  // per face
    areas = vector<float>(n_faces);
    Thread(n_faces).run([this](int f) {
      auto &face = faces[f];
      Vertex vsum = 0;
      auto fl = face.size();
      Vertex v1 = vertexes[face[fl - 2]], v2 = vertexes[face[fl - 1]];

      for (int ic = 0; ic < fl; ic++) {
        vsum += v1.cross(v2);
        v1 = v2;
        v2 = vertexes[face[ic]];
      }
      areas[f] = abs(normals[f].dot(vsum)) / 2;
    });
  }

  void calc_areas_st() {  // per face
    areas = vector<float>(n_faces);
    for (size_t f = 0; f < n_faces; f++) {
      auto &face = faces[f];
      Vertex vsum = 0;
      auto fl = face.size();
      Vertex v1 = vertexes[face[fl - 2]], v2 = vertexes[face[fl - 1]];

      for (size_t ic = 0; ic < fl; ic++) {
        vsum += v1.cross(v2);
        v1 = v2;
        v2 = vertexes[face[ic]];
      }
      areas[f] = abs(normals[f].dot(vsum)) / 2;
    }
  }

  void calc_colors() {  // per areas
    calc_areas();       // areas required

    auto pallette = Color::random_pallete();
    map<int, Vertex> color_dict;  // color dict<sigfigs, pallette>

    for (auto a : areas) {
      auto k = sigfigs(a);
      color_dict[k] = pallette[color_dict.size() % pallette.size()];
    }
    colors.clear();
    for (auto a : areas) colors.push_back(color_dict[sigfigs(a)]);
  }

  Vertex centroid(Face &face) {
    Vertex centroid = 0;  // calc centroid of face
    for (auto ic : face) centroid += vertexes[ic];
    return centroid / face.size();
  }

  // sets
  //  void set_faces(Faces &faces) {
  //    this->faces = faces;
  //    this->n_faces = faces.size();
  //  }

  // gets
  string get_name() { return name; }
  Vertexes get_vertexes() { return vertexes; }
  Faces get_faces() { return faces; }
  Vertexes get_normals() {
    if (normals.empty()) calc_normals();
    return normals;
  }
  vector<float> get_areas() {
    if (areas.empty()) calc_areas();
    return areas;
  }
  Vertexes get_centers() {
    if (centers.empty()) calc_centers();
    return centers;
  }
  Vertexes get_colors() {
    get_areas();  // required
    if (colors.empty()) calc_colors();
    return colors;
  }

  Vertex get_color(int face) {  // get face color according to face area
    return colors[face];
  }

  Vertex get_normal(int face) { return normals[face]; }

  void set_colors(Vertexes &colors) { this->colors = colors; }

  void set_normals(Vertexes &normals) { this->normals = normals; }

  void set_vertexes(Vertexes &vertexes) {
    this->vertexes = vertexes;
    n_vertex = vertexes.size();
  }

  void replace(Vertexes vertexes, Vertexes normals, Vertexes colors) {
    this->vertexes = vertexes;
    n_vertex = vertexes.size();
    this->normals = normals;
    this->colors = colors;
  }

  void new_colors() {
    colors.clear();
    calc_colors();
  }

  // print
  void print_stat() {
    recalc();

    printf("name    : %s\n", name.c_str());
    printf("vertices: %ld\n", vertexes.size());
    printf("faces   : %ld\n", faces.size());

    for (int i = 0; i < faces.size(); i++) {
      auto face = faces[i];  // traverse vertexes
      for (auto ixc : face) vertexes[ixc];
    }
    puts("traverse ok");
  }

  void print() {
    recalc();

    printf("name:%s\n", name.c_str());

    printf("vertices %ld\n", vertexes.size());
    int iv = 0;
    for (auto v : vertexes)
      printf("%d:(%.2f, %.2f, %.2f), ", iv++, v.x, v.y, v.z);

    printf("\nfaces %ld\n", faces.size());
    int iface = 0;
    for (int i = 0; i < faces.size(); i++) {
      auto face = faces[i];
      printf("%d:(", iface++);
      for (auto ic : face) printf("%d ", ic);
      printf("), ");
    }

    puts("\nnormals");
    for (size_t i = 0; i < faces.size(); i++)
      printf("%ld:(%.2f, %.2f, %.2f), ", i, normals[i].x, normals[i].y,
             normals[i].z);

    puts("\nareas");
    for (size_t i = 0; i < faces.size(); i++)
      printf("%ld:(%.2f), ", i, areas[i]);

    puts("\ncolors");
    for (size_t i = 0; i < colors.size(); i++)
      printf("%ld:(%.2f, %.2f, %.2f, %s), ", i, colors[i].x, colors[i].y,
             colors[i].z, Color::rgb2hex(colors[i]).c_str());

    puts("\ncenters");
    for (size_t i = 0; i < faces.size(); i++)
      printf("%ld:(%.2f, %.2f, %.2f), ", i, centers[i].x, centers[i].y,
             centers[i].z);
    puts("");
  }

 public:
  string name = "";
  Vertexes vertexes;
  Faces faces;
  size_t n_vertex = 0, n_faces = 0;

 private:
  Vertexes normals, colors, centers;
  vector<float> areas;

 private:

  int sigfigs_org(
      float f,
      int nsigs = 2) {  // returns string w. nsigs digits ignoring magnitude
    if (f == 0.f) return 0;
    float mantissa = f / powf(10, floor(log10f(f)));
    return int(roundf(mantissa * powf(10, (nsigs - 1))));
  }
  int sigfigs(float f) {  // returns string w. nsigs digits ignoring magnitude
    return int(f * 100);
  }
};

