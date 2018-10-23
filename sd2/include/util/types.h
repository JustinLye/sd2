#ifndef UTILITY_TYPES_HEADER_INCLUDED
#define	UTILITY_TYPES_HEADER_INCLUDED

#include<vector>

#include<glm/glm.hpp>

#include"util/constants.h"

namespace sd2 {
namespace util {
namespace types {

enum FileLoadResult : unsigned int {
  kLoadOk,
  kPathInvalid,
  kLoadFailed,
  kLoadFailedDuringScanForLineType,
  kLoadFailedRefusedToHandleUnknownInput
};

struct GeometricFileData {
  char name[sd2::util::constants::kMaxNameLength];
  std::vector<glm::vec4> geometric_vertices;
  std::vector<glm::vec3> texture_coordinates;
  std::vector<glm::vec3> vertex_normals;
  std::vector<glm::vec3> parameter_space_vertices;
  std::vector<std::vector<glm::vec3>> polygonal_face_elements;
  std::vector<const char*> external_material_file_names;
  std::vector<const char*> use_material_names;
  std::vector<const char*> smoothing_groups;

};

} // namespace types
} // namespace util
} // namespace sd2

#endif // !UTILITY_TYPES_HEADER_INCLUDED

