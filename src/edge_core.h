#ifndef EDGE_CORE_H
#define EDGE_CORE_H
#include <unordered_set>
#include <set>
#include <vector>

#include <Eigen/Core>
namespace marvel{

struct one_edge{
  one_edge():f1(-1), f2(-1), v1(-1), v2(-1),v3(-1),v4(-1){}
  one_edge(const int& f1_, const int& f2_, const int& v1_, const int& v2_, const int& v3_, const int& v4_):f1(f1_),f2(f2_),v1(v1_),v2(v2_),v3(v3_),v4(v4_){}
  int f1,f2,v1,v2,v3,v4;
};


//TODO use hash map
class edge_core{
 public:
  edge_core(const Eigen::MatrixXi& tris, const Eigen::MatrixXf& nods);
  int loop(const Eigen::MatrixXi& tris, const Eigen::MatrixXf& nods, Eigen::MatrixXi& new_tris, Eigen::MatrixXf& new_nods);
 private:
  size_t num_faces_;
  size_t num_edges_;
  size_t num_vertices_;
  std::vector<one_edge> edges_;
  std::vector<std::vector<size_t> > vertices_;

  
};


}
#endif
