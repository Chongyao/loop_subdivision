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
  bool operator == (const one_edge& other){
    if (v1 == other.v1 && v2 == other.v2)
      return f1 == other.f1 && f2 == other.f2 && v3 == other.v3 && v4 == other.v4;
    else if(v1 == other.v2 && v2 == other.v1)
      return f1 == other.f2 && f2 == other.f1 && v3 == other.v4 && v4 == other.v3;
  }
};


//TODO use hash map
class edge_core{
 public:
  edge_core(bool if_update):if_update_(if_update), if_check(false){}
  int operator ()(C_MI_ptr& ori_tris, C_MF_ptr& ori_verts, MI_ptr& new_tris, MF_ptr& new_verts);

  
 private:
  int construct_core(const Eigen::MatrixXi& tris, const Eigen::MatrixXf& nods);

  int calculate_odd_points(C_MF_ptr& ori_verts, MF_ptr& new_verts);
  int calculate_even_points(C_MF_ptr& ori_verts, MF_ptr& new_verts);
  int set_odds_to_new_tris(C_MI_ptr& ori_tris, MI_ptr& new_tris);
  int set_evens_to_new_tris(C_MI_ptr& ori_tris, MI_ptr& new_tris);

  int update_edges_and_valence(C_MI_ptr& ori_tris, C_MI_ptr& new_tris);
  int get_order(const size_t& v_id, const Eigen::Vector3i& f);
  
  size_t num_faces_;
  size_t num_edges_;
  size_t num_vertices_;
  std::vector<one_edge> edges_;
  
  std::vector<size_t> valences_;

  float get_beta(const size_t& valence);

  bool if_update_;
  bool if_construct_;
  bool if_check;

  
};


}
#endif
