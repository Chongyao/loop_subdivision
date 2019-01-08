#include <libigl/readOBJ.h>
#include <libigl/writeOBJ.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "edge_core.h"
using namespace std;
using namespace Eigen;
using namespace marvel;

int main(int argc, char**argv){
  Eigen::initParallel();
  cout << "[INFO]>>>>>>>>>>>>>>>>>>>Eigen parallel<<<<<<<<<<<<<<<<<<" << endl;
  cout << "enable parallel in Eigen in " << nbThreads() << " threads" << endl;
  
  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>load obj<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
  boost::property_tree::ptree pt;{
    const string jsonfile_path = argv[1];
    
    cout << jsonfile_path << endl;
    const size_t ext = jsonfile_path.rfind(".json");
    if (ext != std::string::npos){
      read_json(jsonfile_path, pt);
      cout << "read json successful" <<endl;
    }
    else{
      cout << "json file extension error" << endl;
      return 0;
    }
  }
  
  cout << "[INFO]>>>>>>>>>>>>>>>>>>>IMPORT MESH<<<<<<<<<<<<<<<<<<" << endl;
  const string mesh_name = pt.get<string>("surf");
  const string indir = pt.get<string>("indir");
  MatrixXi surf;
  MatrixXf nods;
  
  igl::readOBJ((indir+mesh_name+".obj").c_str(), nods, surf);
  cout << "surf: " << surf.rows() << " " << surf.cols() << endl << "nods: " << nods.rows() << " " << nods.cols() << endl;
  
  surf.transposeInPlace();
  nods.transposeInPlace();
  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>load obj<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//

  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>construct edge_core
  edge_core EC(surf, nods);
  auto e = EC.edges_[5];
  cout << e.f1 <<" "<< e.f2 <<" "<< e. v1 <<" "<< e.v2<<" " << e.v3<<" " << e.v4<<" " << endl;;
  cout << surf.col(e.f1)<<endl;
  cout << surf.col(e.f2)<<endl;

  
}
