// polyhedron

extern crate nalgebra as na;
use na::{Isometry3, Point3, Vector3};

struct Polyhedron {
    name : String,
    faces : Vec<Vec<u32>>,
    vertexes: Vec<Point3<f32>>
}