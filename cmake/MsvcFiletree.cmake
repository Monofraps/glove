# Fix source tree representation in Visual Studio
if(MSVC)
	macro(GroupSources curdir)
		file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir} ${PROJECT_SOURCE_DIR}/${curdir}/*)

	    foreach(child ${children})
			if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})
	        	GroupSources(${curdir}/${child})
	        else()
	            string(REPLACE "/" "\\" groupname ${curdir})
	            string(REPLACE "." "src" groupname ${groupname})

	            source_group(${groupname} FILES ${PROJECT_SOURCE_DIR}/${curdir}/${child})
	        endif()
	    endforeach()
	endmacro()

	GroupSources(.)
endif()