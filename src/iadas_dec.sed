/"sim\.h"/a\
#include "memui.h"\
#include "iadas_types.gen.h"
/"ia_exec\.h"/a\
#include "ia_das.h"
s/ [^ ]*IARd/ 0/
s/ [^ ]*IAWr/ 0/
s/ [^ ]*IAEx/ 0/
/Status iAinstFetchDecode/d
s/PIARF/int/
s/PIAWF/int/
s/PIAEF/int/
s/iAinstFetchDecode/0/
s/^BOOL ia_decode(/BOOL iadas_decode(/
s/memIAIRd/memMIAIRd/
