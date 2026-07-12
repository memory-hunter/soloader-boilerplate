#include "utils/init.h"
#include "utils/glutil.h"

#include <psp2/kernel/threadmgr.h>

#include <falso_jni/FalsoJNI.h>
#include <so_util/so_util.h>

#include "reimpl/controls.h"
#include "reimpl/asset_manager.h"
#include "vitasdk.h"
#include "utils/logger.h"

int _newlib_heap_size_user = 256 * 1024 * 1024;

#ifdef USE_SCELIBC_IO
int sceLibcHeapSize = 4 * 1024 * 1024;
#endif

so_module so_mod;

#define JNI_STR(str) jni->NewStringUTF(&jni, str)
#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 544

int init_lang(void)
{
    int lang = 0; // default to eng
    sceAppUtilSystemParamGetInt(SCE_SYSTEM_PARAM_ID_LANG, &lang);

    switch (lang)
    {
    case SCE_SYSTEM_PARAM_LANG_FRENCH:
        return 1;
    case SCE_SYSTEM_PARAM_LANG_SPANISH:
        return 2;
    case SCE_SYSTEM_PARAM_LANG_ITALIAN:
        return 3;
    case SCE_SYSTEM_PARAM_LANG_GERMAN:
        return 4;
    case SCE_SYSTEM_PARAM_LANG_RUSSIAN:
        return 5;
    case SCE_SYSTEM_PARAM_LANG_PORTUGUESE_BR:
        return 6;
    case SCE_SYSTEM_PARAM_LANG_CHINESE_S:
    case SCE_SYSTEM_PARAM_LANG_CHINESE_T:
        return 7;
    case SCE_SYSTEM_PARAM_LANG_ENGLISH_US:
    case SCE_SYSTEM_PARAM_LANG_ENGLISH_GB:
    default:
        return 0;
    }
}

int main()
{
    soloader_init_all();
    gl_init();

    int (*JNI_OnLoad)(void *jvm) = (void *)so_symbol(&so_mod, "JNI_OnLoad");
    int (*Java_eu_namcobandaigames_garfieldwr_SplashScreenActivity_initPathDirectory)(void *env, void *obj, jstring path) = (void *)so_symbol(&so_mod, "Java_eu_namcobandaigames_garfieldwr_SplashScreenActivity_initPathDirectory");
    int (*Java_eu_namcobandaigames_garfieldwr_SplashScreenActivity_nativeassetmanager)(void *env, void *obj, AAssetManager *aam) = (void *)so_symbol(&so_mod, "Java_eu_namcobandaigames_garfieldwr_SplashScreenActivity_nativeassetmanager");
    int (*Java_eu_namcobandaigames_garfieldwr_SplashScreenActivity_nativedevicelanguage)(void *env, void *obj, jint lang) = (void *)so_symbol(&so_mod, "Java_eu_namcobandaigames_garfieldwr_SplashScreenActivity_nativedevicelanguage");
    int (*Java_eu_namcobandaigames_garfieldwr_SplashScreenActivity_initGame)() = (void *)so_symbol(&so_mod, "Java_eu_namcobandaigames_garfieldwr_SplashScreenActivity_initGame");
    int (*Java_eu_namcobandaigames_garfieldwr_GlBufferView_nativedisplayresolution)(void *env, void *obj, jint screen_width, jint screen_height) = (void *)so_symbol(&so_mod, "Java_eu_namcobandaigames_garfieldwr_GlBufferView_nativedisplayresolution");
    int (*native_initGame)() = (void *)so_symbol(&so_mod, "native_initGame");
    int (*native_gl_resize)(void *env, void *obj, jint screen_width, jint screen_height) = (void *)so_symbol(&so_mod, "native_gl_resize");
    int (*native_gl_render)() = (void *)so_symbol(&so_mod, "native_gl_render");

    
    JNI_OnLoad(&jvm);
    l_debug("JNI_OnLoad called");

    Java_eu_namcobandaigames_garfieldwr_SplashScreenActivity_initPathDirectory(&jni, NULL, JNI_STR(DATA_PATH"sound.tmp"));
    l_debug("Java_eu_namcobandaigames_garfieldwr_SplashScreenActivity_initPathDirectory called");
    AAssetManager *aam = AAssetManager_create();
    Java_eu_namcobandaigames_garfieldwr_SplashScreenActivity_nativeassetmanager(&jni, NULL, aam);
    l_debug("Java_eu_namcobandaigames_garfieldwr_SplashScreenActivity_nativeassetmanager called");
    int lang = init_lang();
    Java_eu_namcobandaigames_garfieldwr_SplashScreenActivity_nativedevicelanguage(&jni, NULL, lang);
    l_debug("Java_eu_namcobandaigames_garfieldwr_SplashScreenActivity_nativedevicelanguage called");
    Java_eu_namcobandaigames_garfieldwr_SplashScreenActivity_initGame();
    l_debug("Java_eu_namcobandaigames_garfieldwr_SplashScreenActivity_initGame called");
    Java_eu_namcobandaigames_garfieldwr_GlBufferView_nativedisplayresolution(&jni, NULL, SCREEN_WIDTH, SCREEN_HEIGHT);
    l_debug("Java_eu_namcobandaigames_garfieldwr_GlBufferView_nativedisplayresolution called");
    native_initGame();
    l_debug("native_initGame called");
    native_gl_resize(&jni, NULL, SCREEN_WIDTH, SCREEN_HEIGHT);
    l_debug("native_gl_resize called, enter renderloop");

    while (1)
    {
        native_gl_render();
        gl_swap();
    }

    sceKernelExitDeleteThread(0);
}

void controls_handler_key(int32_t keycode, ControlsAction action)
{
    // Call into the .so here
}

void controls_handler_touch(int32_t id, float x, float y, ControlsAction action)
{
    // Call into the .so here
}

void controls_handler_analog(ControlsStickId which, float x, float y, ControlsAction action)
{
    // Call into the .so here
}
