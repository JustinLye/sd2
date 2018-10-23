#include<iostream>
#include<string>

#include"util/util.h"

using sd2::util::WavefrontGeometricDataFileLoader;
using sd2::util::types::FileLoadResult;
using sd2::util::types::GeometricFileData;
int check_regex(int argc, char* argv[])
{
	std::string l = "1//1";
	std::regex r ("([0-9]+|[0-9]*\\.[0-9]+){1}//([0-9]+|[0-9]*\\.[0-9]+){1}");
	std::cout << std::boolalpha << std::regex_match(l, r) << '\n';
	return 0;
}

int check_map(int argc, char* argv[]) {
  using sd2::util::impl::LineType;
  std::map<const char*, LineType, sd2::util::impl::compare_const_char_ptr> kLineTypeMap_({
   {"o", LineType::kName},
   {"#", LineType::kComment},
   {"v", LineType::kListOfGeometricVertices},
   {"vt", LineType::kListOfTextureCoordinates},
   {"vn", LineType::kListOfVertexNormals},
   {"vp", LineType::kParameterSpaceVertices},
   {"f", LineType::kPolygonalFaceElements},
   {"l", LineType::kLineElement},
   {"mtllib", LineType::kExternalMaterialFileName},
   {"usemtl", LineType::kMaterialName},
   {"g", LineType::kGroupName},
   {"s", LineType::kSmoothingGroup} });

  if (kLineTypeMap_.find("#") == kLineTypeMap_.cend()) {
    std::cout << "not found\n";
  } else {
    std::cout << "found\n";
  }

  return 0;
}

int object_loader(int argc, char* argv[])
{
	std::string file_path = "cube.obj";
	std::cout << argc << '\n';
	if (argc == 2)
	{
		file_path = argv[1];
	}
	std::cout << "Received file " << file_path << '\n';

	WavefrontGeometricDataFileLoader wave_file;
	std::cout << "Loading wavefile\n";
  std::string result = "";
  FileLoadResult load_result = wave_file.LoadDataFromFile("C:\\SpaceDefender\\tests\\bin\\Debug\\cube.obj");
  std::cout << "Load result " << load_result << '\n';
	switch (load_result)
	{
		case FileLoadResult::kLoadOk:
			result = "LOAD_OK";
			break;
		case FileLoadResult::kPathInvalid:
			result = "PATH_INVALID";
			break;
		case FileLoadResult::kLoadFailed:
			result = "LOAD_FAILED";
			break;
    case FileLoadResult::kLoadFailedDuringScanForLineType:
      result = "load failed during scan for line type";
      break;
    case FileLoadResult::kLoadFailedRefusedToHandleUnknownInput:
      result = "load failed refused to handle unknown input";
      break;
		default:
			result = "UNKNOWN";
			break;
	}
	std::cout << result << '\n';
  if (load_result == FileLoadResult::kLoadOk)
  {
    const GeometricFileData& file_data = wave_file.file_data();
	  std::cout << "Object name: " << file_data.name << '\n';
	  for (int i = 0; i < file_data.geometric_vertices.size(); ++i)
	  {
		  std::cout << "(" << file_data.geometric_vertices[i].x << ", "
				    << file_data.geometric_vertices[i].y << ", "
				    << file_data.geometric_vertices[i].z << ", "
				    << file_data.geometric_vertices[i].w << ")\n";
	  }
	  for (int i = 0; i < file_data.texture_coordinates.size(); ++i)
	  {
		  std::cout << "(" << file_data.texture_coordinates[i].x << ", "
			  << file_data.texture_coordinates[i].y << ", "
			  << file_data.texture_coordinates[i].z << ")\n";
	  }
	  for (int i = 0; i < file_data.vertex_normals.size(); ++i)
	  {
		  std::cout << "(" << file_data.vertex_normals[i].x << ", "
			  << file_data.vertex_normals[i].y << ", "
			  << file_data.vertex_normals[i].z << ")\n";
	  }
	  for (int i = 0; i < file_data.parameter_space_vertices.size(); ++i)
	  {
		  std::cout << "(" << file_data.parameter_space_vertices[i].x << ", "
			  << file_data.parameter_space_vertices[i].y << ", "
			  << file_data.parameter_space_vertices[i].z << ")\n";
	  }
	  for (int i = 0; i < file_data.polygonal_face_elements.size(); ++i)
	  {
		  std::cout << "[\n";
		  for (int y = 0; y < file_data.polygonal_face_elements.size(); ++y)
		  {
			  glm::vec3 v = file_data.polygonal_face_elements[i][y];
			  std::string a,b,c;
			  a = v.x != sd2::util::constants::kValueNotProvided ? std::to_string(v.x) : "";
			  b = v.y != sd2::util::constants::kValueNotProvided ? std::to_string(v.y) : "";
			  c = v.z != sd2::util::constants::kValueNotProvided ? std::to_string(v.z) : "";
			  std::cout << "(" << a << "," << b << "," << c << ")\n";
		  }
		  std::cout << "]\n";
	  }
    return 0;
  } else {
    return 1;
  }
	
}