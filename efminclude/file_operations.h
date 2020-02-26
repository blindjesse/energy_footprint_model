#if defined(_WIN64)
  #define REMOVE_IF_EXISTS "if EXIST %1$s (del %1$s)"
  #define COPY_IF_EXISTS "if EXIST %1$s (copy %1$s %2$s)" 
  #define REMOVE "del "
  #define COPY "copy "
  #define RENAME "rename "
#else
  #define REMOVE_IF_EXISTS "rm -f %s"
  #define COPY_IF_EXISTS "copy %s %s 2>/dev/null"
  #define REMOVE "rm "
  #define COPY "cp "
  #define RENAME "mv -f "
#endif

