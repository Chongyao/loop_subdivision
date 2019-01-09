#ifndef EDGE_CORE_H
#define EDGE_CORE_H
#include <memory>
#include <vector>

#include <Eigen/Core>


namespace marvel{

typedef  const std::shared_ptr<const Eigen::MatrixXi> C_MI_ptr;
typedef  const std::shared_ptr<const Eigen::MatrixXf> C_MF_ptr;
typedef  const std::shared_ptr<Eigen::MatrixXi> MI_ptr;
typedef  const std::shared_ptr<Eigen::MatrixXf> MF_ptr;

struct one_edge{
  one_edge():f1(-1), f2(-1), v1(-1), v2(-1),v3(-1),v4(-1){}
  one_edge(const int& f1_, const int& f2_, const int& v1_, const int& v2_, const int& v3_, const int& v4_):f1(f1_),f2(f2_),v1(v1_),v2(v2_),v3(v3_),v4(v4_){}
  int f1,f2,v1,v2,v3,v4;
};


//TODO use hash map
class edge_core{
 public:
  edge_core(){}
  int operator ()(C_MI_ptr& ori_tris, C_MF_ptr& ori_verts, MI_ptr& new_tris, MF_ptr new_verts);
 private:
  int construct_core(const Eigen::MatrixXi& tris, const Eigen::MatrixXf& nods);
  // int loop(const Eigen::MatrixXi& tris, const Eigen::MatrixXf& nods, Eigen::MatrixXi& new_tris, Eigen::MatrixXf& new_nods)const ;
  size_t num_faces_;
  size_t num_edges_;
  size_t num_vertices_;
  std::vector<one_edge> edges_;
  std::vector<std::vector<size_t> > vertices_;

  
};


}
#endif
