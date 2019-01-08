#ifndef EDGE_CORE_H
#define EDGE_CORE_H
#include <unordered_set>
#include <set>
#include <vector>

#include <Eigen/Core>
namespace marvel{




//TODO use hash map
class edge_core{
 public:
  edge_core(const Eigen::MatrixXi& tris, const Eigen::MatrixXf& nods);
  
 private:
  size_t num_edges;
  // std::unordered_set<std::pair<size_t, size_t> > edges_;
  std::set<edge> edges_;
  std::vector<std::vector<size_t> > vertices_;

  
}


}
#endif
