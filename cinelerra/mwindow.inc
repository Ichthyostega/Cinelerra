#ifndef MWINDOW_INC
#define MWINDOW_INC

#define PROGRAM_NAME "Cinelerra"
#define CINELERRA_VERSION "2.1"
#define DEFAULT_THEME "S.U.V."
#define CONFIG_FILE "Cinelerra_rc"

#ifdef __x86_64__
#define PLUGIN_DIR "/usr/lib64/cinelerra"
#else
#define PLUGIN_DIR "/usr/lib/cinelerra"
#endif

// What to call silence in the EDL
#define SILENCE "SILENCE"

// Behavior of region selections
#define SELECTION_SAMPLES 0
#define SELECTION_EDIT 1

class MWindow;

#endif
