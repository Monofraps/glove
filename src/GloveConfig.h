#define GLOVE_VERSION_MAJOR 1
#define GLOVE_VERSION_MINOR 0
#define GLOVE_VERSION_PATCH 0

#define GLOVE_VERSION_STRING #GLOVE_VERSION_MAJOR "." #GLOVE_VERSION_MINOR "." #GLOVE_VERSION_PATCH

#if !defined(ON_UINX)
#define ON_UNIX
#endif