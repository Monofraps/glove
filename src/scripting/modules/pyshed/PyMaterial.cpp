#include "PyMaterial.h"

#include <boost/python.hpp>
#include <scripting/PythonObjectFactory.h>

#include "shader/Material.h"

namespace glove {
namespace python {

void ExportMaterial() {
	boost::python::class_<glove::Material, std::shared_ptr<Material>>("Material", boost::python::init<ShaderProgramPointer>())
		.def("SetMaterialAttributeF", &Material::PySetMaterialAttributeF)
		.def("SetMaterialAttributeV3", &Material::PySetMaterialAttributeV3)
		.def("SetMaterialAttributeV4", &Material::PySetMaterialAttributeV4)
		.def("SetMaterialAttributeMat4", &Material::PySetMaterialAttributeMat4);
}

} // namespace python
} // namespace glove