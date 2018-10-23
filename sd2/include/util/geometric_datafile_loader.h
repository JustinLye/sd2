#ifndef OBJECT_LOADER_HEADER_INCLUDED
#define OBJECT_LOADER_HEADER_INCLUDED

#include<glm/glm.hpp>

#include"util/util.h"


namespace sd2 {
namespace util {
namespace impl {
using ::sd2::util::types::FileLoadResult;
using ::sd2::util::types::GeometricFileData;


class GeometricDataFileLoader {
public:
  GeometricDataFileLoader() {};
  virtual ~GeometricDataFileLoader() {};

  virtual FileLoadResult LoadDataFromFile(const char*) = 0;

  const GeometricFileData& file_data() const { return file_data_; }
  void file_data(const GeometricFileData& data) { file_data_ = data; }

protected:
  GeometricFileData file_data_;
};
} // namespace impl

using impl::GeometricDataFileLoader;

} // namespace util



} // namespace sd2

#endif // !OBJECT_LOADER_HEADER_INCLUDED

