#include"util/wavefront_geometric_datafile_loader.h"

namespace sd2 {
namespace util {
namespace impl {
// 4 acceptable face element formats
// float
const char* kVertexIndexRegex =
"\\s*([0-9]+|[0-9]*\\.[0-9]+){1}\\s*";

// float/float
const char* kVertexIndexTextureCoordinateRegex =
"\\s*([0-9]+|[0-9]*\\.[0-9]+){1}/([0-9]+|[0-9]*\\.[0-9]+){1}\\s*";

// float/float/float
const char* kVertexIndexTextureCoordinateNormalIndexRegex =
"\\s*([0-9]+|[0-9]*\\.[0-9]+){1}/([0-9]+|[0-9]*\\.[0-9]+){1}/([0-9]+|[0-9]*\\.[0-9]+){1}\\s*";

// float//float
const char* kVertexIndexNormalIndexRegex =
"\\s*([0-9]+|[0-9]*\\.[0-9]+){1}//([0-9]+|[0-9]*\\.[0-9]+){1}\\s*";

// not recognized
const char* kUnrecognizedRegex = "";
const char* kGeometricVerticesInputFormat = "%f %f %f %f";
const char* kTextureCoordinatesInputFormat = "%f %f %f";
const char* kVertexNormalsInputFormat = "%f %f %f";
const char* kParameterSpaceVerticesInputFormat = "%f %f %f";
const char* kVertexIndexFaceElementInputFormat = "%f";
const char* kVertexIndexTextureCoordinateFaceElementInputFormat = "%f/%f";
const char* kVertexIndexTextureCoordinateNormalIndexFaceElementInputFormat = "%f/%f/%f";
const char* kVertexIndexNormalIndexFaceElementInputFormat = "%f//%f";
}
}
}


using sd2::util::WavefrontGeometricDataFileLoader;
using sd2::util::impl::LineType;
using sd2::util::impl::FaceElementFormat;
using sd2::util::types::FileLoadResult;
using sd2::util::constants::kValueNotProvided;
using sd2::util::constants::kMaxNameLength;

namespace strings = sd2::util::strings;

WavefrontGeometricDataFileLoader::WavefrontGeometricDataFileLoader() :
 GeometricDataFileLoader(),
 data_file_(nullptr),
 kLineHandlerMap_({
  {kName, std::bind(&WavefrontGeometricDataFileLoader::HandleName, this)},
  {kComment, std::bind(&WavefrontGeometricDataFileLoader::HandleComment, this)},
  {kListOfGeometricVertices, std::bind(&WavefrontGeometricDataFileLoader::HandleListOfGeometricVertices, this)},
  {kListOfTextureCoordinates, std::bind(&WavefrontGeometricDataFileLoader::HandleListOfTextureCoordinates, this)},
  {kListOfVertexNormals, std::bind(&WavefrontGeometricDataFileLoader::HandleListOfVertexNormals, this)},
  {kParameterSpaceVertices, std::bind(&WavefrontGeometricDataFileLoader::HandleParameterSpaceVertices, this)},
  {kPolygonalFaceElements, std::bind(&WavefrontGeometricDataFileLoader::HandlePolygonalFaceElements, this)},
  {kLineElement, std::bind(&WavefrontGeometricDataFileLoader::HandleLineElement, this)},
  {kExternalMaterialFileName, std::bind(&WavefrontGeometricDataFileLoader::HandleExternalMaterialFileName, this)},
  {kMaterialName, std::bind(&WavefrontGeometricDataFileLoader::HandleMaterialName, this)},
  {kGroupName, std::bind(&WavefrontGeometricDataFileLoader::HandleGroupName, this)},
  {kSmoothingGroup, std::bind(&WavefrontGeometricDataFileLoader::HandleSmoothingGroup, this)},
  {kUnknown, std::bind(&WavefrontGeometricDataFileLoader::HandleUnknown, this)},
  {kBlankLine, std::bind(&WavefrontGeometricDataFileLoader::HandleBlankLine, this)}}),
 kVertexIndexRegex_(kVertexIndexRegex),
 kVertexIndexTextureCoordinateRegex_(kVertexIndexTextureCoordinateRegex),
 kVertexIndexTextureCoordinateNormalIndexRegex_(kVertexIndexTextureCoordinateNormalIndexRegex),
 kVertexIndexNormalIndexRegex_(kVertexIndexNormalIndexRegex),
 kUnrecognizedRegex_(kUnrecognizedRegex),
 kLineTypeMap_({
  {"o", kName},
  {"#", kComment},
  {"v", kListOfGeometricVertices},
  {"vt", kListOfTextureCoordinates},
  {"vn", kListOfVertexNormals},
  {"vp", kParameterSpaceVertices},
  {"f", kPolygonalFaceElements},
  {"l", kLineElement},
  {"mtllib", kExternalMaterialFileName},
  {"usemtl", kMaterialName},
  {"g", kGroupName},
  {"s", kSmoothingGroup},
  {"", kBlankLine}}),
 kAcceptablePolygonFaceElementFormatVector_({
  {kVertexIndexRegex_, FaceElementFormat::kVertexIndices},
  {kVertexIndexTextureCoordinateRegex_, FaceElementFormat::kVertexIndicesTextureCoordinates},
  {kVertexIndexTextureCoordinateNormalIndexRegex_, FaceElementFormat::kVertexIndicesTextureCoordinatesNormalIndices},
  {kVertexIndexNormalIndexRegex_, FaceElementFormat::kVertexIndicesNormalIndices}}) {}

WavefrontGeometricDataFileLoader::~WavefrontGeometricDataFileLoader() {

}

FileLoadResult WavefrontGeometricDataFileLoader::LoadDataFromFile(const char* file_path) {

  CloseDataFile();
  if (fopen_s(&data_file_, file_path, "rb"))
    return FileLoadResult::kPathInvalid;
 
  char line_from_file[kMaxInputDataFileLineLength];
  FileLoadResult last_line_load_result = FileLoadResult::kLoadOk;
  std::map<LineType, std::function<FileLoadResult(void)>>::const_iterator line_handler_iterator = kLineHandlerMap_.cend();
  #ifdef DEBUG_
  int line_count = 0;
  #endif
  while (!feof(data_file_)) {
    // Read the first word from the current line of the data file. This should indicate what type of information the line provides, which dicates how we will process the line.
    // TODO: Check if the line is longer than max allowed. Try using strnlen_s.
    if (fscanf_s(data_file_, "%s", line_from_file, kMaxInputDataFileLineLength-1) == 0)
      return FileLoadResult::kLoadFailedDuringScanForLineType;
    #ifdef DEBUG_
    ++line_count;
    if (line_from_file == "")
      break;
    #endif
    // TODO: Log message that handler was not found for line type
    if ((line_handler_iterator = kLineHandlerMap_.find(GetLineType(line_from_file))) != kLineHandlerMap_.cend()) {
      if ((last_line_load_result = line_handler_iterator->second()) != FileLoadResult::kLoadOk) {
      #ifdef DEBUG_
      std::cerr << "Load failed. FileLoadResult is " << last_line_load_result << " line (" << line_count << ") load failed on is '" << line_from_file << "'\n";
      #endif
        return last_line_load_result;
      }
      line_from_file[0] = '\0';
    } else {
    #ifdef DEBUG_
      std::cerr << "Load failed because handler could not be found. Line (" << line_count << ") load failed on is '" << line_from_file << "'\n'";
    #endif // DEBUG_

    }
  }

  return FileLoadResult::kLoadOk;

}

LineType WavefrontGeometricDataFileLoader::GetLineType(const char* input_line) const {

  std::map<const char*, LineType>::const_iterator line_type_iterator = kLineTypeMap_.find(input_line);

  if (line_type_iterator == kLineTypeMap_.cend())
    return LineType::kUnknown;
  else
    return line_type_iterator->second;

}

FaceElementFormat WavefrontGeometricDataFileLoader::GetFaceElementType(const char* input_line) const {

  FaceElementFormat result = FaceElementFormat::kUnrecognized;
  char eval_string[kMaxInputDataFileLineLength];

  CleanFaceElementInput(eval_string, input_line);

  // Search for an acceptable polygon face element format that matches the format of the string input_line
  for (auto format_regex_pair : kAcceptablePolygonFaceElementFormatVector_) {
    if (std::regex_match(eval_string, format_regex_pair.first)) {
      result = format_regex_pair.second;
      break;
    }
  }

  return result;

}

FileLoadResult WavefrontGeometricDataFileLoader::HandleName() {

  if (fgets(file_data_.name, kMaxNameLength - 1, data_file_))
    return FileLoadResult::kLoadOk;
  else
    return FileLoadResult::kLoadFailed;
}

FileLoadResult WavefrontGeometricDataFileLoader::HandleComment() {

  char file_line[kMaxInputDataFileLineLength];
  
  fgets(file_line, kMaxInputDataFileLineLength, data_file_);

  return FileLoadResult::kLoadOk;

}
FileLoadResult WavefrontGeometricDataFileLoader::HandleListOfGeometricVertices() {
  
  glm::vec4 vertices(glm::vec3(kValueNotProvided), 1.0f);
  FileLoadResult load_result = FileLoadResult::kLoadOk;
  
  if (fscanf_s(data_file_, kGeometricVerticesInputFormat, &vertices.x, &vertices.y, &vertices.z, &vertices.w) >= 3)
    file_data_.geometric_vertices.push_back(vertices);
  else
    load_result = FileLoadResult::kLoadFailed;

  return load_result;

}
FileLoadResult WavefrontGeometricDataFileLoader::HandleListOfTextureCoordinates() {

  glm::vec3 vertices(glm::vec2(kValueNotProvided), 0.0f);
  FileLoadResult load_result = FileLoadResult::kLoadOk;

  if (fscanf_s(data_file_, kTextureCoordinatesInputFormat, &vertices.x, &vertices.y, &vertices.z) >= 2)
    file_data_.texture_coordinates.push_back(vertices);
  else
    load_result = FileLoadResult::kLoadFailed;

  return load_result;

}
FileLoadResult WavefrontGeometricDataFileLoader::HandleListOfVertexNormals() {

  glm::vec3 vertices(kValueNotProvided);
  FileLoadResult load_result = FileLoadResult::kLoadOk;

  if (fscanf_s(data_file_, kVertexNormalsInputFormat, &vertices.x, &vertices.y, &vertices.z) == 3)
    file_data_.vertex_normals.push_back(vertices);
  else
    load_result = FileLoadResult::kLoadFailed;

  return load_result;

}
FileLoadResult WavefrontGeometricDataFileLoader::HandleParameterSpaceVertices() {

  glm::vec3 vertices(kValueNotProvided);
  FileLoadResult load_result = FileLoadResult::kLoadOk;

  if (fscanf_s(data_file_, kParameterSpaceVerticesInputFormat, &vertices.x, &vertices.y, &vertices.z))
    file_data_.parameter_space_vertices.push_back(vertices);
  else
    load_result = FileLoadResult::kLoadFailed;

  return load_result;

}
FileLoadResult WavefrontGeometricDataFileLoader::HandlePolygonalFaceElements() {
  
  FileLoadResult load_result = FileLoadResult::kLoadOk;
  std::vector<glm::vec3> polygon_face_element_list;
  glm::vec3 vertices(kValueNotProvided);
  int current_char_pos = 0;
  char data_file_line[kMaxInputDataFileLineLength];
  char face_element_vertices[kMaxInputDataFileLineLength];

  fgets(data_file_line, kMaxInputDataFileLineLength-1, data_file_);

  while (strings::next_word(face_element_vertices, data_file_line, current_char_pos, kMaxInputDataFileLineLength-1)) {
    if (strnlen_s(face_element_vertices, kMaxInputDataFileLineLength)) {
      switch (GetFaceElementType(face_element_vertices)) {
        case kVertexIndices:
          if (sscanf_s(face_element_vertices, kVertexIndexFaceElementInputFormat, &vertices.x) == 1)
            polygon_face_element_list.push_back(vertices);
          else
            load_result = FileLoadResult::kLoadFailed;
          break;
        case kVertexIndicesTextureCoordinates:
          if (sscanf_s(face_element_vertices, kVertexIndexTextureCoordinateFaceElementInputFormat, &vertices.x, &vertices.y) == 2)
            polygon_face_element_list.push_back(vertices);
          else
            load_result = FileLoadResult::kLoadFailed;
          break;
        case kVertexIndicesTextureCoordinatesNormalIndices:
          if (sscanf_s(face_element_vertices, kVertexIndexTextureCoordinateNormalIndexFaceElementInputFormat, &vertices.x, &vertices.y, &vertices.z) == 3)
            polygon_face_element_list.push_back(vertices);
          else
            load_result = FileLoadResult::kLoadFailed;
          break;
        case kVertexIndicesNormalIndices:
          if (sscanf_s(face_element_vertices, kVertexIndexNormalIndexFaceElementInputFormat, &vertices.x, &vertices.z) == 2)
            polygon_face_element_list.push_back(vertices);
          else
            load_result = FileLoadResult::kLoadFailed;
          break;
        case kUnrecognized:
          load_result = FileLoadResult::kLoadFailed;
          break;
      }
      if (load_result != FileLoadResult::kLoadOk)
        break;
      vertices.x = vertices.y = vertices.z = kValueNotProvided;
    }
  }
  
  if (!polygon_face_element_list.empty())
    file_data_.polygonal_face_elements.push_back(polygon_face_element_list);
  else
    load_result = FileLoadResult::kLoadFailed;
  
  return load_result;

}
FileLoadResult WavefrontGeometricDataFileLoader::HandleLineElement() {
  return FileLoadResult::kLoadOk;
}
FileLoadResult WavefrontGeometricDataFileLoader::HandleExternalMaterialFileName() {

  FileLoadResult load_result = FileLoadResult::kLoadOk;
  char file_name[kMaxInputDataFileLineLength];
  
  if (fgets(file_name, kMaxInputDataFileLineLength, data_file_))
    file_data_.external_material_file_names.push_back(file_name);
  else 
    load_result = FileLoadResult::kLoadFailed;
  
  return load_result;

}
FileLoadResult WavefrontGeometricDataFileLoader::HandleMaterialName() {

  FileLoadResult load_result = FileLoadResult::kLoadOk;
  char material_name[kMaxInputDataFileLineLength];

  if (fgets(material_name, kMaxInputDataFileLineLength, data_file_))
    file_data_.use_material_names.push_back(material_name);
  else
    load_result = FileLoadResult::kLoadFailed;

  return load_result;
}
FileLoadResult WavefrontGeometricDataFileLoader::HandleGroupName() {
  return FileLoadResult::kLoadOk;
}
FileLoadResult WavefrontGeometricDataFileLoader::HandleSmoothingGroup() {
  
  FileLoadResult load_result = FileLoadResult::kLoadOk;
  char smooth_group[kMaxInputDataFileLineLength];

  if (fgets(smooth_group, kMaxInputDataFileLineLength, data_file_))
    file_data_.smoothing_groups.push_back(smooth_group);
  else
    load_result = FileLoadResult::kLoadFailed;

  return load_result;
}
FileLoadResult WavefrontGeometricDataFileLoader::HandleUnknown() {
  return FileLoadResult::kLoadFailedRefusedToHandleUnknownInput;
}

FileLoadResult WavefrontGeometricDataFileLoader::HandleBlankLine() {
  char blank_line[kMaxInputDataFileLineLength];

  fgets(blank_line, kMaxInputDataFileLineLength, data_file_);
#ifdef DEBUG_
  std::cout << "Ignoring blank line.\n";
#endif // DEBUG_

  return FileLoadResult::kLoadOk;
}
