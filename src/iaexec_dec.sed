/undef MAX/a\
#undef PAGESIZE
/"sim\.h"/a\
#include "memory.h"\
#include "ia_types.h"
/"ia_exec\.h"/a\
#include "ia_read.h"\
#include "ia_write.h"\
#include "ia_decoder.h"
s/ [^ ]*_das/ 0/
/EaInfoPtr ea/,/^}/d
