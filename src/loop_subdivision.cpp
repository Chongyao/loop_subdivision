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
  const string outdir = pt.get<string>("outdir");
  const size_t times = pt.get<size_t>("times");
  MatrixXi surf;
  MatrixXf nods;
  
  igl::readOBJ((indir+mesh_name+".obj").c_str(), nods, surf);
  cout << "surf: " << surf.rows() << " " << surf.cols() << endl << "nods: " << nods.rows() << " " << nods.cols() << endl;
  
  surf.transposeInPlace();
  nods.transposeInPlace();
  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>load obj<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//

  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>construct edge_core
  edge_core EC;
  shared_ptr<MatrixXi> ori_tris = make_shared<MatrixXi>(surf);
  shared_ptr<MatrixXf> ori_verts = make_shared<MatrixXf>(nods);
  shared_ptr<MatrixXi> new_tris = make_shared<MatrixXi>(0,0);
  shared_ptr<MatrixXf> new_verts = make_shared<MatrixXf>(0,0);


  for(size_t i = 0; i < times; ++i){
    EC(ori_tris, ori_verts, new_tris, new_verts);
    

    igl::writeOBJ((outdir+mesh_name+ to_string(i) + ".obj").c_str(), new_verts->transpose(), new_tris->transpose());
    

    
    ori_tris = new_tris;
    ori_verts = new_verts;
    new_tris = make_shared<MatrixXi>(0,0);
    new_verts = make_shared<MatrixXf>(0,0);
  }

      

  
  return 0;

  
}
