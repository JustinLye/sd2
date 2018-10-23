#ifndef UTILITY_WAVEFRONT_GEOMETRIC_DATAFILE_LOADER_HEADER_INCLUDED
#define UTILITY_WAVEFRONT_GEOMETRIC_DATAFILE_LOADER_HEADER_INCLUDED

#include"util/geometric_datafile_loader.h"

#if defined DEBUG_ || defined  TRACE_
#include<iostream>
#endif
#ifdef TRACE_
#define TRACE_FILE_FUNCT_AND_LINE std::cout << __FILE__ << ' ' << __FUNCTION__ << '[' << __LINE__ << "]\n";
#endif
#include<map>
#include<vector>
#include<functional>
#include<regex>

#include"util/constants.h"

namespace sd2 {
namespace util {
namespace impl {
using ::sd2::util::constants::kMaxInputDataFileLineLength;
// Types of lines found in .obj file
enum LineType : unsigned int {
  kName,
  kComment,
  kListOfGeometricVertices,
  kListOfTextureCoordinates,
  kListOfVertexNormals,
  kParameterSpaceVertices,
  kPolygonalFaceElements,
  kLineElement,
  kExternalMaterialFileName,
  kMaterialName,
  kGroupName,
  kSmoothingGroup,
  kUnknown
};

enum FaceElementFormat : unsigned int {
  kVertexIndices,
  kVertexIndicesTextureCoordinates,
  kVertexIndicesTextureCoordinatesNormalIndices,
  kVertexIndicesNormalIndices,
  kUnrecognized
};

struct compare_const_char_ptr {
  bool operator()(const char const* lhs, const char const* rhs) const {
    return strcmp(lhs, rhs) < 0;
  }
};

class WavefrontGeometricDataFileLoader : public GeometricDataFileLoader {
 public:
  WavefrontGeometricDataFileLoader();
  ~WavefrontGeometricDataFileLoader();
  virtual FileLoadResult LoadDataFromFile(const char*);
 
 private:
  LineType GetLineType(const char*) const;
  FaceElementFormat GetFaceElementType(const char*) const;
  void CloseDataFile() {
    if (data_file_ != nullptr)
      fclose(data_file_);
    data_file_ = nullptr;
  }

  void CleanFaceElementInput(char* output_string, const char* input_string) const {
    //TODO: Need to add some sanity checks when getting the character count, making copies, etc.
    #ifdef TRACE_
      TRACE_FILE_FUNCT_AND_LINE
      std::cout << "Max length " << kMaxInputDataFileLineLength - 1 << '\n';
    #endif
    int last_char_pos = strnlen_s(input_string, kMaxInputDataFileLineLength);
    #ifdef TRACE_
      TRACE_FILE_FUNCT_AND_LINE
    #endif //TRACE_
    if (input_string[last_char_pos] == '\n')
      --last_char_pos;
    #ifdef TRACE_
      TRACE_FILE_FUNCT_AND_LINE
      std::cout << "Last char pos " << last_char_pos << '\n';
    #endif //TRACE_
    #ifdef DEBUG_
      if (kMaxInputDataFileLineLength - 1 < last_char_pos) {
        std::cout << "last char pos " << last_char_pos << " > Max line input " << kMaxInputDataFileLineLength - 1 << '\n';
        last_char_pos = kMaxInputDataFileLineLength - 2;
      }
      std::cout << "input line is '" << input_string << "'\n"; 
      
    #endif // DEBUG_
    strncpy_s(output_string, kMaxInputDataFileLineLength - 1, input_string, last_char_pos);
    #ifdef TRACE_
      TRACE_FILE_FUNCT_AND_LINE
    #endif //TRACE_
 
  }

  // Functions for handling various line types
  FileLoadResult HandleName();
  FileLoadResult HandleComment();
  FileLoadResult HandleListOfGeometricVertices();
  FileLoadResult HandleListOfTextureCoordinates();
  FileLoadResult HandleListOfVertexNormals();
  FileLoadResult HandleParameterSpaceVertices();
  FileLoadResult HandlePolygonalFaceElements();
  FileLoadResult HandleLineElement();
  FileLoadResult HandleExternalMaterialFileName();
  FileLoadResult HandleMaterialName();
  FileLoadResult HandleGroupName();
  FileLoadResult HandleSmoothingGroup();
  FileLoadResult HandleUnknown();


  FILE* data_file_;
  const std::map<LineType, std::function<FileLoadResult(void)>> kLineHandlerMap_;
  const std::regex kVertexIndexRegex_;
  const std::regex kVertexIndexTextureCoordinateRegex_;
  const std::regex kVertexIndexTextureCoordinateNormalIndexRegex_;
  const std::regex kVertexIndexNormalIndexRegex_;
  const std::regex kUnrecognizedRegex_;
  const std::map<const char*, LineType, compare_const_char_ptr> kLineTypeMap_;
  const std::vector<std::pair<const std::regex&, FaceElementFormat>> kAcceptablePolygonFaceElementFormatVector_;
  
};
} // namespace impl

using impl::WavefrontGeometricDataFileLoader;

} // namespace util
} // namespace sd2

#endif // !UTILITY_WAVEFRONT_GEOMETRIC_DATAFILE_LOADER_HEADER_INCLUDED
